//
// Created by 74222 on 2026/8/7.
//

#include "bsp_matrix_key.h"
#include "main.h"

/* 矩阵键盘尺寸：4 行 × 5 列。 */
#define KEY_ROW_COUNT             4U
#define KEY_COL_COUNT             5U

/*
 * 按键任务每 5 ms 扫描一次，原始结果连续相同 4 次才确认为稳定状态，
 * 因此按下和释放的消抖时间约为 20 ms。
 */
#define KEY_DEBOUNCE_SCAN_COUNT   4U

/* 行电平切换后执行少量空指令，等待 GPIO 和外部上拉网络的电平稳定。 */
#define KEY_SCAN_SETTLE_CYCLES    16U

/* 32 位按键快照中，第 0～7 位保存 KEY_CODE，第 8 位保存 KEY_STATE。 */
#define KEY_SNAPSHOT_STATE_SHIFT  8U

/* GPIO 端口与引脚的组合描述。 */
typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} Key_GPIO_t;

/*
 * 一轮完整物理扫描的结果类型。
 * RAW_KEY_RELEASED 与 RAW_KEY_UNMAPPED 必须区分：未映射位置虽然不上报，
 * 但它仍表示有一个物理按键处于按下状态。
 */
typedef enum
{
    RAW_KEY_RELEASED = 0, /* 没有检测到物理按键 */
    RAW_KEY_VALID,        /* 检测到一个已映射的有效按键 */
    RAW_KEY_UNMAPPED,     /* 检测到一个未映射的物理按键 */
    RAW_KEY_MULTIPLE      /* 检测到两个或更多物理按键 */
} Raw_Key_Type_t;

/* 一轮扫描的原始结果；只有 RAW_KEY_VALID 时 code 才是有效业务键值。 */
typedef struct
{
    Raw_Key_Type_t type;
    KEY_CODE code;
} Raw_Key_t;

/*
 * 按键状态机：
 * WAIT_RELEASE：启动后等待所有按键稳定释放，防止上电时误触发；
 * READY：       当前没有活动键，可以接受新的单键；
 * PRESSED：     一个有效按键已经稳定按下；
 * LOCKED：      检测到多键、未映射键或未释放直接换键，等待全部释放。
 */
typedef enum
{
    KEY_SCAN_WAIT_RELEASE = 0,
    KEY_SCAN_READY,
    KEY_SCAN_PRESSED,
    KEY_SCAN_LOCKED
} Key_Scan_State_t;

/*
 * 行引脚顺序必须与 key_map 的行顺序一致。
 * 根据当前硬件方案，扫描时当前行输出低电平，其余行保持推挽高电平。
 */
static const Key_GPIO_t row_gpios[KEY_ROW_COUNT] = {
    {.GPIOx = KB_R1_GPIO_Port, .GPIO_Pin = KB_R1_Pin},
    {.GPIOx = KB_R2_GPIO_Port, .GPIO_Pin = KB_R2_Pin},
    {.GPIOx = KB_R3_GPIO_Port, .GPIO_Pin = KB_R3_Pin},
    {.GPIOx = KB_R4_GPIO_Port, .GPIO_Pin = KB_R4_Pin}
};

/*
 * 列引脚由 PCB 外部上拉：无按键时读取为高电平；当前扫描行拉低后，
 * 某列读取为低电平表示该行与该列交叉位置的按键已经闭合。
 */
static const Key_GPIO_t col_gpios[KEY_COL_COUNT] = {
    {KB_C1_GPIO_Port, KB_C1_Pin},
    {KB_C2_GPIO_Port, KB_C2_Pin},
    {KB_C3_GPIO_Port, KB_C3_Pin},
    {KB_C4_GPIO_Port, KB_C4_Pin},
    {KB_C5_GPIO_Port, KB_C5_Pin}
};

/*
 * 物理行列位置到业务键值的映射，按 R1C1 至 R4C5 排列。
 * KEY_NONE 表示该物理位置暂不产生业务按键，但仍参与物理按键数量统计。
 */
static const KEY_CODE key_map[KEY_ROW_COUNT][KEY_COL_COUNT] = {
    /* C1        C2         C3          C4          C5 */
    {KEY_TEST,   KEY_LEFT,  KEY_RIGHT,  KEY_RETURN, KEY_ENTER}, /* R1 */
    {KEY_1,      KEY_2,     KEY_3,      KEY_DELETE, KEY_UP},    /* R2 */
    {KEY_4,      KEY_5,     KEY_6,      KEY_0,      KEY_DOWN},  /* R3 */
    {KEY_7,      KEY_8,     KEY_9,      KEY_DOT,    KEY_NONE}   /* R4 */
};

/*
 * KeyScanTask 是快照的唯一写入者，LVGL 任务只读取快照。
 * 键值和状态打包进一个 32 位变量，避免分别读取时得到不一致的数据。
 */
static volatile uint32_t key_snapshot;

/* 以下状态只由 KeyScanTask 访问，不需要额外互斥保护。 */

/* 当前扫描状态机所处阶段，决定是否允许接收新按键或必须等待全部释放。 */
static Key_Scan_State_t scan_state;

/* 当前已经稳定按下并发布给上层的按键；发布释放状态时仍需使用该键值。 */
static KEY_CODE active_key;

/* 当前正在进行消抖确认的原始扫描候选结果，包含结果类型和候选键值。 */
static Raw_Key_t debounce_candidate;

/* 候选结果连续出现的次数；达到 KEY_DEBOUNCE_SCAN_COUNT 后才确认其稳定。 */
static uint8_t debounce_count;

/* 将四根行线统一设置为指定电平。 */
static void key_set_all_rows(GPIO_PinState state)
{
    for (uint8_t row = 0U; row < KEY_ROW_COUNT; row++)
    {
        HAL_GPIO_WritePin(row_gpios[row].GPIOx, row_gpios[row].GPIO_Pin, state);
    }
}

/*
 * 等待行输出与列输入电平稳定。
 * 这里只需要很短的建立时间，因此使用 __NOP()，不使用毫秒级任务延时。
 */
static void key_wait_settle(void)
{
    for (uint32_t cycle = 0U; cycle < KEY_SCAN_SETTLE_CYCLES; cycle++)
    {
        __NOP();
    }
}

/*
 * @brief : 返回一轮扫描 的扫描结果
 * 完成一轮 4×5 物理扫描并分类原始结果。
 * 必须扫描全部 20 个交叉点，不能检测到第一个按键后立即返回， 否则无法区分单键和多键状态。
 */
static Raw_Key_t key_read_raw(void)
{
    Raw_Key_t result = {RAW_KEY_RELEASED, KEY_NONE};
    uint8_t pressed_count = 0U;
    uint8_t pressed_row = 0U;
    uint8_t pressed_col = 0U;

    for (uint8_t row = 0U; row < KEY_ROW_COUNT; row++)
    {
        /* 先将所有行恢复为高电平，再只把当前扫描行拉低。 */
        key_set_all_rows(GPIO_PIN_SET);
        HAL_GPIO_WritePin(row_gpios[row].GPIOx, row_gpios[row].GPIO_Pin, GPIO_PIN_RESET);
        key_wait_settle();

        for (uint8_t col = 0U; col < KEY_COL_COUNT; col++)
        {
            /* 列线外部上拉，因此低电平表示当前行列交叉点的按键闭合。 */
            if (HAL_GPIO_ReadPin(col_gpios[col].GPIOx, col_gpios[col].GPIO_Pin) == GPIO_PIN_RESET)
            {
                pressed_count++;

                /* 只保存第一个位置；检测到第二个后只需依靠计数判定多键。 */
                if (pressed_count == 1U)
                {
                    pressed_row = row;
                    pressed_col = col;
                }
            }
        }
    }

    /* 一轮扫描结束后，将全部行恢复为默认高电平。 */
    key_set_all_rows(GPIO_PIN_SET);

    if (pressed_count > 1U)
    {
        result.type = RAW_KEY_MULTIPLE;
    }
    else if (pressed_count == 1U)
    {
        result.code = key_map[pressed_row][pressed_col];
        result.type = (result.code == KEY_NONE) ? RAW_KEY_UNMAPPED : RAW_KEY_VALID;
    }

    return result;
}

/* 原始类型和键值都相同时，才认为两轮扫描结果一致。 */
static uint8_t key_raw_is_equal(Raw_Key_t left, Raw_Key_t right)
{
    return (left.type == right.type) && (left.code == right.code);
}

/*
 * 发布稳定按键状态。
 * 松开时仍发布最后一次按下的 active_key，使 LVGL 能够配对按下和释放事件。
 */
static void key_publish(KEY_CODE code, KEY_STATE state)
{
    key_snapshot = ((uint32_t)state << KEY_SNAPSHOT_STATE_SHIFT) | (uint32_t)code;
}

/* 使用已经完成消抖的结果推进按键状态机。 */
static void key_process_stable(Raw_Key_t key)
{
    switch (scan_state)
    {
        case KEY_SCAN_WAIT_RELEASE:
            /* 系统启动后先确认矩阵完全释放，之后才允许接收按键。 */
            if (key.type == RAW_KEY_RELEASED)
            {
                scan_state = KEY_SCAN_READY;
            }
            break;

        case KEY_SCAN_READY:
            /* 空闲状态只接受一个已经映射的有效按键。 */
            if (key.type == RAW_KEY_VALID)
            {
                active_key = key.code;
                key_publish(active_key, KEY_STATE_PRESSED);
                scan_state = KEY_SCAN_PRESSED;
            }
            else if (key.type != RAW_KEY_RELEASED)
            {
                scan_state = KEY_SCAN_LOCKED;
            }
            break;

        case KEY_SCAN_PRESSED:
            /* 同一个按键持续按下时保持 PRESSED，供 LVGL 处理长按和重复。 */
            if ((key.type == RAW_KEY_VALID) && (key.code == active_key))
            {
                break;
            }

            /*
             * 全部释放时结束本次按键；若直接变成其他键，则释放旧键并进入锁定，
             * 防止未经过全释放就把一次重叠操作识别成两个有效按键。
             */
            key_publish(active_key, KEY_STATE_RELEASED);
            active_key = KEY_NONE;
            scan_state = (key.type == RAW_KEY_RELEASED) ? KEY_SCAN_READY : KEY_SCAN_LOCKED;
            break;

        case KEY_SCAN_LOCKED:
            /* 锁定期间忽略所有按键，稳定检测到全部释放后才重新启用输入。 */
            if (key.type == RAW_KEY_RELEASED)
            {
                scan_state = KEY_SCAN_READY;
            }
            break;

        default:
            /* 状态异常时回到安全初始状态，并重新等待全部释放。 */
            scan_state = KEY_SCAN_WAIT_RELEASE;
            active_key = KEY_NONE;
            key_publish(KEY_NONE, KEY_STATE_RELEASED);
            break;
    }
}

void bsp_matrix_key_init(void)
{
    /* GPIO 已由 MX_GPIO_Init() 配置，此处只设置默认电平并复位软件状态。 */
    key_set_all_rows(GPIO_PIN_SET);
    key_snapshot = 0U;
    scan_state = KEY_SCAN_WAIT_RELEASE;
    active_key = KEY_NONE;
    debounce_candidate.type = RAW_KEY_RELEASED;
    debounce_candidate.code = KEY_NONE;
    debounce_count = 0U;
}


void bsp_matrix_key_scan(void)
{
    Raw_Key_t raw_key = key_read_raw();

    /*
     * 多键或未映射物理键不等待 20 ms 消抖，一经检测到就立即进入锁定。
     * 这样即使重叠只持续一个扫描周期，也不能绕过“全部释放后恢复”的规则。
     */
    if ((raw_key.type == RAW_KEY_MULTIPLE) || (raw_key.type == RAW_KEY_UNMAPPED))
    {
        if (scan_state == KEY_SCAN_PRESSED)
        {
            /* 已有有效按键时，先向 LVGL 发布该键的释放状态。 */
            key_publish(active_key, KEY_STATE_RELEASED);
            active_key = KEY_NONE;
        }

        scan_state = KEY_SCAN_LOCKED;
        debounce_candidate = raw_key;
        debounce_count = 0U;
        return;
    }

    /* 原始结果发生变化时重新计数；连续相同时累加计数。 */
    if (key_raw_is_equal(raw_key, debounce_candidate) != 0U)
    {
        if (debounce_count < KEY_DEBOUNCE_SCAN_COUNT)
        {
            debounce_count++;
        }
    }
    else
    {
        debounce_candidate = raw_key;
        debounce_count = 1U;
    }

    /*
     * 达到消抖阈值时只处理一次。处理后将计数再加一，持续不变的结果不会
     * 重复触发状态机；原始结果变化后，计数会重新从 1 开始。
     */
    if (debounce_count == KEY_DEBOUNCE_SCAN_COUNT)
    {
        key_process_stable(debounce_candidate);
        debounce_count++;
    }
}

KEY_DATA bsp_matrix_key_get_state(void)
{
    /* 一次性读取 32 位快照，再从局部副本中拆出键值和状态。 */
    uint32_t snapshot = key_snapshot;
    KEY_DATA data;

    data.code = (KEY_CODE)(snapshot & 0xFFU);
    data.state = (KEY_STATE)((snapshot >> KEY_SNAPSHOT_STATE_SHIFT) & 0x01U);

    return data;
}
