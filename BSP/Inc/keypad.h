//
// Created by 74222 on 2026/7/15.
//

#ifndef YOUJIESUN_KEYPAD_H
#define YOUJIESUN_KEYPAD_H

typedef enum
{
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER,
    KEY_RETURN,
    KEY_DELETE,
    KEY_TEST,
    KEY_SIGN,
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
}KEY_CODE;

//按键扫描函数
KEY_CODE key_scan(void);
#endif //YOUJIESUN_KEYPAD_H
