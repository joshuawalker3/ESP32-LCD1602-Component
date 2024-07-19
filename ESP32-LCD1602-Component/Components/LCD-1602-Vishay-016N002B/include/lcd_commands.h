/*
Author: Joshua Walker
Date: 18-July-2024
Version: 1.0
*/

#ifndef LCD_COMMANDS_H
#define LCD_COMMANDS_H

//commands to clear display and return home
#define CLEAR_DISPLAY 0x01
#define RET_HOME 0x02

//Entry mode set and options
#define ENTRY_MODE_SET 0x04
#define CURS_INCR 0x02
#define CURS_DECR 0x00
#define SCREEN_SHIFT_EN 0x01
#define SCREEN_SHIFT_DIS 0x00

//Display control
#define DISP_CTRL 0x08
#define DISP_ON 0x04
#define DISP_OFF 0x00
#define CURS_ON 0x02
#define CURS_OFF 0x00
#define CURS_BLINK 0x01
#define CURS_SOLID 0x00

//Cursor and Display Shift
#define CURS_DISP_SHIFT 0x10
#define SHIFT_DISP 0x08
#define SHIFT_CURS 0x00
#define SHIFT_RIGHT 0x04
#define SHIFT_LEFT 0x00

//Function Set
#define FUNCTION_SET 0x20
#define BIT_MODE_8 0x10
#define BIT_MODE_4 0x00
#define LINES_2 0x08
#define LINES_1 0x00
#define FONT_5x11 0x04
#define FONT_5x8 0x00

//Setting DDRAM and CGRAM Address
#define CGRAM_SET 0x40
#define DDRAM_SET 0x80

//bytes for LCD pin control and byte transmission
#define BIT_COMP 0xF0
#define EN_HI 0x0C
#define ALL_LO 0x08
#define RS_HI 0x09

#endif