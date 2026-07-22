//
// Created by 74222 on 2026/7/14.
//

#ifndef YOUJIESUN_HEADER_H
#define YOUJIESUN_HEADER_H
#include "lvgl.h"
/*   */
lv_obj_t *header_get(void);
void header_init(void);
void header_load(void);
void update_time_cb(lv_timer_t *timer);
void update_temperature_cb(lv_timer_t *timer);
#endif //YOUJIESUN_HEADER_H
