/**
 * @file lv_port_indev_template.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "bsp_matrix_key.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

// static void touchpad_init(void);
// static void touchpad_read(lv_indev_t * indev, lv_indev_data_t * data);
// static bool touchpad_is_pressed(void);
// static void touchpad_get_xy(int32_t * x, int32_t * y);
//
// static void mouse_init(void);
// static void mouse_read(lv_indev_t * indev, lv_indev_data_t * data);
// static bool mouse_is_pressed(void);
// static void mouse_get_xy(int32_t * x, int32_t * y);

static void keypad_init(void);
static void keypad_read(lv_indev_t * indev, lv_indev_data_t * data);
static uint32_t keypad_key_to_lvgl(KEY_CODE key);

// static void encoder_init(void);
// static void encoder_read(lv_indev_t * indev, lv_indev_data_t * data);
// static void encoder_handler(void);

// static void button_init(void);
// static void button_read(lv_indev_t * indev, lv_indev_data_t * data);
// static int8_t button_get_pressed_id(void);
// static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
// lv_indev_t * indev_touchpad;
// lv_indev_t * indev_mouse;
lv_indev_t * indev_keypad;
// lv_indev_t * indev_encoder;
// lv_indev_t * indev_button;

// static int32_t encoder_diff;
// static lv_indev_state_t encoder_state;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    // touchpad_init();

    /*Register a touchpad input device*/
    // indev_touchpad = lv_indev_create();
    // lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    // lv_indev_set_read_cb(indev_touchpad, touchpad_read);

    /*------------------
     * Mouse
     * -----------------*/

    /*Initialize your mouse if you have*/
    // mouse_init();

    /*Register a mouse input device*/
    // indev_mouse = lv_indev_create();
    // lv_indev_set_type(indev_mouse, LV_INDEV_TYPE_POINTER);
    // lv_indev_set_read_cb(indev_mouse, mouse_read);

    /*Set cursor. For simplicity set a HOME symbol now.*/
    // lv_obj_t * mouse_cursor = lv_image_create(lv_screen_active());
    // lv_image_set_src(mouse_cursor, LV_SYMBOL_HOME);
    // lv_indev_set_cursor(indev_mouse, mouse_cursor);

    /*------------------
     * Keypad
     * -----------------*/

    /*Initialize your keypad or keyboard if you have*/
    keypad_init();

    /*Register a keypad input device*/
    indev_keypad = lv_indev_create();
    lv_indev_set_type(indev_keypad, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev_keypad, keypad_read);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_keypad, group);`*/

    /*------------------
     * Encoder
     * -----------------*/

    /*Initialize your encoder if you have*/
    // encoder_init();

    /*Register a encoder input device*/
    // indev_encoder = lv_indev_create();
    // lv_indev_set_type(indev_encoder, LV_INDEV_TYPE_ENCODER);
    // lv_indev_set_read_cb(indev_encoder, encoder_read);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_encoder, group);`*/

    /*------------------
     * Button
     * -----------------*/

    /*Initialize your button if you have*/
    // button_init();

    /*Register a button input device*/
    // indev_button = lv_indev_create();
    // lv_indev_set_type(indev_button, LV_INDEV_TYPE_BUTTON);
    // lv_indev_set_read_cb(indev_button, button_read);
    //
    // /*Assign buttons to points on the screen*/
    // static const lv_point_t btn_points[2] = {
    //     {10, 10},   /*Button 0 -> x:10; y:10*/
    //     {40, 100},  /*Button 1 -> x:40; y:100*/
    // };
    // lv_indev_set_button_points(indev_button, btn_points);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/



// /*Initialize your touchpad*/
// static void touchpad_init(void)
// {
//     /*Your code comes here*/
// }
//
// /*Will be called by the library to read the touchpad*/
// static void touchpad_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
// {
//     static int32_t last_x = 0;
//     static int32_t last_y = 0;
//
//     /*Save the pressed coordinates and the state*/
//     if(touchpad_is_pressed()) {
//         touchpad_get_xy(&last_x, &last_y);
//         data->state = LV_INDEV_STATE_PRESSED;
//     }
//     else {
//         data->state = LV_INDEV_STATE_RELEASED;
//     }
//
//     /*Set the last pressed coordinates*/
//     data->point.x = last_x;
//     data->point.y = last_y;
// }
//
// /*Return true is the touchpad is pressed*/
// static bool touchpad_is_pressed(void)
// {
//     /*Your code comes here*/
//
//     return false;
// }
//
// /*Get the x and y coordinates if the touchpad is pressed*/
// static void touchpad_get_xy(int32_t * x, int32_t * y)
// {
//     /*Your code comes here*/
//
//     (*x) = 0;
//     (*y) = 0;
// }


/*------------------
 * Mouse
 * -----------------*/

// /*Initialize your mouse*/
// static void mouse_init(void)
// {
//     /*Your code comes here*/
// }
//
// /*Will be called by the library to read the mouse*/
// static void mouse_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
// {
//     /*Get the current x and y coordinates*/
//     mouse_get_xy(&data->point.x, &data->point.y);
//
//     /*Get whether the mouse button is pressed or released*/
//     if(mouse_is_pressed()) {
//         data->state = LV_INDEV_STATE_PRESSED;
//     }
//     else {
//         data->state = LV_INDEV_STATE_RELEASED;
//     }
// }
//
// /*Return true is the mouse button is pressed*/
// static bool mouse_is_pressed(void)
// {
//     /*Your code comes here*/
//
//     return false;
// }
//
// /*Get the x and y coordinates if the mouse is pressed*/
// static void mouse_get_xy(int32_t * x, int32_t * y)
// {
//     /*Your code comes here*/
//
//     (*x) = 0;
//     (*y) = 0;
// }

/*------------------
 * Keypad
 * -----------------*/

/*
 * 矩阵 GPIO 和扫描状态机由 KeyScanTask 初始化，此处不重复操作硬件。
 * 保留该函数是为了维持 LVGL 输入端口模板的初始化结构。
 */
static void keypad_init(void)
{
}

/*
 * 由 LVGL 周期调用，只读取 BSP 已完成消抖的稳定快照，不直接扫描 GPIO。
 * 释放状态必须继续携带 last_key，LVGL 才能将释放事件与之前的按下事件配对。
 */
static void keypad_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0U;
    KEY_DATA key_data = bsp_matrix_key_get_state();

    (void)indev_drv;

    if (key_data.state == KEY_STATE_PRESSED)
    {
        /* 按住期间持续返回 PRESSED，由 LVGL 处理长按和自动重复。 */

        last_key = current_key;
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else
    {
        /* KEY_NONE 或尚未映射到 LVGL 的业务键均不向 LVGL 上报按下。 */
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->key = last_key;
}

/* 将硬件无关的业务键值转换成 LVGL 控制键或可输入字符。 */
static uint32_t keypad_key_to_lvgl(KEY_CODE key)
{
    switch (key)
    {
        case KEY_LEFT:
            return LV_KEY_PREV;
        case KEY_RIGHT:
            return LV_KEY_NEXT;
        case KEY_RETURN:
            return LV_KEY_ESC;
        case KEY_ENTER:
            return LV_KEY_ENTER;
        case KEY_DELETE:
            return LV_KEY_DEL;
        case KEY_UP:
            return LV_KEY_UP;
        case KEY_DOWN:
            return LV_KEY_DOWN;
        case KEY_0:
            return '0';
        case KEY_1:
            return '1';
        case KEY_2:
            return '2';
        case KEY_3:
            return '3';
        case KEY_4:
            return '4';
        case KEY_5:
            return '5';
        case KEY_6:
            return '6';
        case KEY_7:
            return '7';
        case KEY_8:
            return '8';
        case KEY_9:
            return '9';
        case KEY_DOT:
            return '.';
        case KEY_NONE:
        case KEY_TEST:
        default:
            return 0U;
    }
}

/*------------------
 * Encoder
 * -----------------*/

// /*Initialize your encoder*/
// static void encoder_init(void)
// {
//     /*Your code comes here*/
// }
//
// /*Will be called by the library to read the encoder*/
// static void encoder_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
// {
//
//     data->enc_diff = encoder_diff;
//     data->state = encoder_state;
// }
//
// /*Call this function in an interrupt to process encoder events (turn, press)*/
// static void encoder_handler(void)
// {
//     /*Your code comes here*/
//
//     encoder_diff += 0;
//     encoder_state = LV_INDEV_STATE_RELEASED;
// }
//
// /*------------------
//  * Button
//  * -----------------*/
//
// /*Initialize your buttons*/
// static void button_init(void)
// {
//     /*Your code comes here*/
// }
//
// /*Will be called by the library to read the button*/
// static void button_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
// {
//
//     static uint8_t last_btn = 0;
//
//     /*Get the pressed button's ID*/
//     int8_t btn_act = button_get_pressed_id();
//
//     if(btn_act >= 0) {
//         data->state = LV_INDEV_STATE_PRESSED;
//         last_btn = btn_act;
//     }
//     else {
//         data->state = LV_INDEV_STATE_RELEASED;
//     }
//
//     /*Save the last pressed button's ID*/
//     data->btn_id = last_btn;
// }
//
// /*Get ID  (0, 1, 2 ..) of the pressed button*/
// static int8_t button_get_pressed_id(void)
// {
//     uint8_t i;
//
//     /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
//     for(i = 0; i < 2; i++) {
//         /*Return the pressed button's ID*/
//         if(button_is_pressed(i)) {
//             return i;
//         }
//     }
//
//     /*No button pressed*/
//     return -1;
// }
//
// /*Test if `id` button is pressed or not*/
// static bool button_is_pressed(uint8_t id)
// {
//
//     /*Your code comes here*/
//
//     return false;
// }

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
