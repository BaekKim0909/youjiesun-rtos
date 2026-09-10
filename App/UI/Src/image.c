//
// Created by 74222 on 2026/9/2.
//
#include "image.h"

#define IMG_SIZE_OF_80PX    (80U * 80U * 3U)
#define IMG_SIZE_OF_72PX    (72U * 72U * 3U)
#define IMG_SIZE_OF_30PX    (30U * 30U * 3U)

/* 测试标准icon图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t measure_standard_img_map[IMG_SIZE_OF_72PX] = {};

/* 测试电极icon图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t electrode_img_map[IMG_SIZE_OF_72PX] = {};

/* 样品ID图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t sample_img_map[IMG_SIZE_OF_72PX] = {};

/* 体积电阻率图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t resistance_img_map[IMG_SIZE_OF_72PX] = {};

/* 温度过高图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t temperature_high_image_map[IMG_SIZE_OF_30PX] = {};

/* 温度图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t temperature_normal_image_map[IMG_SIZE_OF_30PX] = {};

/* 盖子关闭图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t lid_close_image_map[IMG_SIZE_OF_30PX] = {};

/* 盖子打开图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t lid_open_image_map[IMG_SIZE_OF_30PX] = {};

/* 油杯正常图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t oil_cup_normal_image_map[IMG_SIZE_OF_30PX] = {};
/* 油杯异常图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t oil_cup_abnormal_image_map[IMG_SIZE_OF_30PX] = {};

/* 排油开关关闭图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t pour_oil_close_image_map[IMG_SIZE_OF_30PX] = {};

/* 排油开关打开图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t pour_oil_open_image_map[IMG_SIZE_OF_30PX] = {};

/* 测试导航栏选中状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t test_nav_focused_image_map[IMG_SIZE_OF_30PX] = {};

/* 测试导航栏默认状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t test_nav_normal_image_map[IMG_SIZE_OF_30PX] = {};

/* 测试记录导航栏选中状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t data_record_nav_focused_image_map[IMG_SIZE_OF_30PX] = {};

/* 测试记录导航栏默认状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t data_record_nav_normal_image_map[IMG_SIZE_OF_30PX] = {};

/* 测试标准导航栏选中状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t test_standard_nav_focused_image_map[IMG_SIZE_OF_30PX] = {};

/* 测试标准导航栏默认状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t test_standard_nav_normal_image_map[IMG_SIZE_OF_30PX] = {};

/* 电极导航栏选中状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t electrode_nav_focused_image_map[IMG_SIZE_OF_30PX] = {};

/* 电极导航栏默认状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t electrode_nav_normal_image_map[IMG_SIZE_OF_30PX] = {};

/* 设置导航栏选中状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t setting_nav_focused_image_map[IMG_SIZE_OF_30PX] = {};

/* 设置导航栏默认状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t setting_nav_normal_image_map[IMG_SIZE_OF_30PX] = {};

/* 设备信息导航栏选中状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t info_nav_focused_image_map[IMG_SIZE_OF_30PX] = {};

/* 设备信息导航栏默认状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t info_nav_normal_image_map[IMG_SIZE_OF_30PX] = {};

/* 闪电亮状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t lightning_light_image_map[IMG_SIZE_OF_80PX] = {};

/* 闪电暗状态图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t lightning_dark_image_map[IMG_SIZE_OF_80PX] = {};

/* 加热图片数组 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t heat_image_map[IMG_SIZE_OF_80PX] = {};

/* 公司公众号二维码图片 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t QR_code_image_map[143 * 143 * 3] = {};

/* 校准日期图标 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t calibration_data_image_map[IMG_SIZE_OF_30PX] = {};

/* 联系电话图标 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t phone_number_image_map[IMG_SIZE_OF_30PX] = {};

/* 序列号图标 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t serial_number_image_map[IMG_SIZE_OF_30PX] = {};

/* 软件版本图标 */
__attribute__((section(".sdram"),aligned(4)))
static uint8_t software_version_image_map[IMG_SIZE_OF_30PX] = {};
image_t image_list[IMAGE_LIST_COUNT] = {
    {
        .img_file_name = "measure_standard_icon.bin",
        .img_size = IMG_SIZE_OF_72PX,
        .img_map = measure_standard_img_map
    },
    {
        .img_file_name = "electrode_icon.bin",
        .img_size = IMG_SIZE_OF_72PX,
        .img_map = electrode_img_map
    },
    {
        .img_file_name = "sample_icon.bin",
        .img_size = IMG_SIZE_OF_72PX,
        .img_map = sample_img_map
    },
    {
        .img_file_name = "resistance_icon.bin",
        .img_size = IMG_SIZE_OF_72PX,
        .img_map = resistance_img_map
    },
    {
        .img_file_name = "temperature_high_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = temperature_high_image_map
    },
    {
        .img_file_name = "temperature_normal_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = temperature_normal_image_map
    },
    {
        .img_file_name = "lid_close_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = lid_close_image_map
    },
    {
        .img_file_name = "lid_open_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = lid_open_image_map
    },
    {
        .img_file_name = "pour_oil_close_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = pour_oil_close_image_map
    },
    {
        .img_file_name = "pour_oil_open_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = pour_oil_open_image_map
    },
    {
        .img_file_name = "oil_cup_normal_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = oil_cup_normal_image_map
    },
    {
        .img_file_name = "oil_cup_abnormal_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = oil_cup_abnormal_image_map
    },
    {
        .img_file_name = "test_nav_normal_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = test_nav_normal_image_map
    },
    {
        .img_file_name = "test_nav_focused_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = test_nav_focused_image_map
    },
    {
        .img_file_name = "data_record_nav_normal_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = data_record_nav_normal_image_map
    },
    {
        .img_file_name = "data_record_nav_focused_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = data_record_nav_focused_image_map
    },
    {
        .img_file_name = "test_standard_nav_normal_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = test_standard_nav_normal_image_map
    },
    {
        .img_file_name = "test_standard_nav_focused_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = test_standard_nav_focused_image_map
    },
    {
        .img_file_name = "electrode_nav_normal_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = electrode_nav_normal_image_map
    },
    {
        .img_file_name = "electrode_nav_focused_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = electrode_nav_focused_image_map
    },
    {
        .img_file_name = "setting_nav_normal_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = setting_nav_normal_image_map
    },
    {
        .img_file_name = "setting_nav_focused_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = setting_nav_focused_image_map
    },
    {
        .img_file_name = "info_nav_normal_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = info_nav_normal_image_map
    },
    {
        .img_file_name = "info_nav_focused_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = info_nav_focused_image_map
    },
    {
        .img_file_name = "lightning_light_icon.bin",
        .img_size = IMG_SIZE_OF_80PX,
        .img_map = lightning_light_image_map
    },
    {
        .img_file_name = "lightning_dark_icon.bin",
        .img_size = IMG_SIZE_OF_80PX,
        .img_map = lightning_dark_image_map
    },
    {
        .img_file_name = "heat_icon.bin",
        .img_size = IMG_SIZE_OF_80PX,
        .img_map = heat_image_map
    },
    {
        .img_file_name = "QR_code_icon.bin",
        .img_size = 143 * 143 * 3,
        .img_map = QR_code_image_map
    },
    {
        .img_file_name = "phone_number_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = phone_number_image_map
    },
    {
        .img_file_name = "calibration_data_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = calibration_data_image_map
    },
    {
        .img_file_name = "serial_number_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = serial_number_image_map
    },
    {
        .img_file_name = "software_version_icon.bin",
        .img_size = IMG_SIZE_OF_30PX,
        .img_map = software_version_image_map
    },
};

/* 测试标准图片对象 */
const lv_image_dsc_t measure_standard_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 72,
    .header.h = 72,
    .data_size = 5184 * 3,
    .data = measure_standard_img_map,
};

/* 电极图片对象 */
const lv_image_dsc_t electrode_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 72,
    .header.h = 72,
    .data_size = 5184 * 3,
    .data = electrode_img_map,
};

/* 样品ID对象 */
const lv_image_dsc_t sample_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 72,
    .header.h = 72,
    .data_size = 5184 * 3,
    .data = sample_img_map,
};

/* 体积电阻率图片对象 */
const lv_image_dsc_t resistance_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 72,
    .header.h = 72,
    .data_size = 5184 * 3,
    .data = resistance_img_map,
};

/* 温度过高图片对象 */
const lv_image_dsc_t temperature_high_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = temperature_high_image_map,
};

/* 温度正常图片对象 */
const lv_image_dsc_t temperature_normal_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = temperature_normal_image_map,
};

/* 盖子关闭图片对象 */
const lv_image_dsc_t lid_close_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = lid_close_image_map,
};

/* 盖子打开图片对象 */
const lv_image_dsc_t lid_open_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = lid_open_image_map,
};

/* 排油开关关闭图片对象 */
const lv_image_dsc_t pour_oil_close_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = pour_oil_close_image_map,
};

/* 排油开关打开图片对象 */
const lv_image_dsc_t pour_oil_open_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = pour_oil_open_image_map,
};

/* 油杯正常图片对象 */
const lv_image_dsc_t oil_cup_normal_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = oil_cup_normal_image_map,
};

/* 油杯异常图片对象 */
const lv_image_dsc_t oil_cup_abnormal_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = oil_cup_abnormal_image_map,
};

/* 测试导航栏默认状态图片对象 */
const lv_image_dsc_t test_nav_normal_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = test_nav_normal_image_map,
};

/* 测试导航栏选中状态图片对象 */
const lv_image_dsc_t test_nav_focused_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = test_nav_focused_image_map,
};

/* 测试记录导航栏默认状态图片对象 */
const lv_image_dsc_t data_record_nav_normal_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = data_record_nav_normal_image_map,
};

/* 测试记录导航栏选中状态图片对象 */
const lv_image_dsc_t data_record_nav_focused_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = data_record_nav_focused_image_map,
};
/* 测试标准导航栏默认状态图片对象 */
const lv_image_dsc_t test_standard_nav_normal_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = test_standard_nav_normal_image_map,
};

/* 测试标准导航栏选中状态图片对象 */
const lv_image_dsc_t test_standard_nav_focused_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = test_standard_nav_focused_image_map,
};
/* 电极导航栏默认状态图片对象 */
const lv_image_dsc_t electrode_nav_normal_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = electrode_nav_normal_image_map,
};

/* 电极导航栏选中状态图片对象 */
const lv_image_dsc_t electrode_nav_focused_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = electrode_nav_focused_image_map,
};
/* 设置导航栏默认状态图片对象 */
const lv_image_dsc_t setting_nav_normal_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = setting_nav_normal_image_map,
};

/* 设置导航栏选中状态图片对象 */
const lv_image_dsc_t setting_nav_focused_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = setting_nav_focused_image_map,
};

/* 设备信息导航栏默认状态图片对象 */
const lv_image_dsc_t info_nav_normal_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = info_nav_normal_image_map,
};

/* 设备信息导航栏选中状态图片对象 */
const lv_image_dsc_t info_nav_focused_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = info_nav_focused_image_map,
};


/* 闪电亮状态图片对象 */
const lv_image_dsc_t lightning_light_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 80,
    .header.h = 80,
    .data_size = 80 * 80 * 3,
    .data = lightning_light_image_map,
};

/* 闪电暗状态图片对象 */
const lv_image_dsc_t lightning_dark_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 80,
    .header.h = 80,
    .data_size = 80 * 80 * 3,
    .data = lightning_dark_image_map,
};

/* 加热图片对象 */
const lv_image_dsc_t heat_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 80,
    .header.h = 80,
    .data_size = 80 * 80 * 3,
    .data = heat_image_map,
};

/* 公司二维码图片对象 */
const lv_image_dsc_t QR_code_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 143,
    .header.h = 143,
    .data_size = 143 * 143 * 3,
    .data = QR_code_image_map,
};
/* 联系电话图片对象 */
const lv_image_dsc_t phone_number_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = phone_number_image_map,
};
/* 校准日期图片对象 */
const lv_image_dsc_t calibration_data_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = calibration_data_image_map,
};
/* 序列号图片对象 */
const lv_image_dsc_t serial_number_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = serial_number_image_map,
};
/* 软件版本图片对象 */
const lv_image_dsc_t software_version_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 30,
    .header.h = 30,
    .data_size = 30 * 30 * 3,
    .data = software_version_image_map,
};
