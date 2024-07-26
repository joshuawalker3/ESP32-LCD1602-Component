/*
Author: Joshua Walker
Date: 18-July-2024
Version: 1.0
*/

#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#ifndef delay
#define delay(time) vTaskDelay(time / portTICK_PERIOD_MS) //delays for time ms
#endif

#define SHORT_DELAY 10
#define LONG_DELAY 100
#define MAX_TIMEOUT -1

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "lcd_commands.h"

typedef struct {
	uint8_t func_opt;
	uint8_t disp_opt;
	uint8_t entry_opt;
}LCD_Start_Config;

typedef struct {
	i2c_master_dev_handle_t* lcd_handle;
	i2c_addr_bit_len_t addr_length;
	uint16_t dev_addr;
	uint32_t speed;
	uint32_t wait_time;
}LCD_Device_Config;

typedef struct {
	i2c_master_bus_handle_t* mst_handle;
	i2c_port_num_t port;
	gpio_num_t sda;
	gpio_num_t scl;
	i2c_clock_source_t clk_src;
	uint8_t glitch_ignore_cnt;
	int intr_priority;
	uint32_t enable_internal_pullup; 
}I2C_MST_Config;

extern uint8_t curs_location;
extern uint8_t heart_char[8];

//Initialization of I2C master bus and/or LCD
void initialize_bus_and_lcd_i2c(LCD_Device_Config* lcd, I2C_MST_Config* mst_bus);

void initialize_lcd_i2c(i2c_master_bus_handle_t* mst_handle, LCD_Device_Config* lcd);

void initialize_lcd_function(i2c_master_dev_handle_t lcd_handle, LCD_Start_Config* config);

//User commands
void put_string(const char* string, i2c_master_dev_handle_t lcd_handle);

void put_string_center(const char* string, i2c_master_dev_handle_t lcd_handle);

void generate_cust_character(uint8_t addr, uint8_t row[8], i2c_master_dev_handle_t lcd_handle);

void write_cust_character(uint8_t addr, i2c_master_dev_handle_t lcd_handle);

void cursor_on_static(i2c_master_dev_handle_t lcd_handle);

void cursor_on_blink(i2c_master_dev_handle_t lcd_handle);

void cursor_off(i2c_master_dev_handle_t lcd_handle);

void shift_screen_right(uint8_t distance, i2c_master_dev_handle_t lcd_handle);

void shift_screen_left(uint8_t distance, i2c_master_dev_handle_t lcd_handle);

//Command and data writing
void send_cmd(uint8_t cmd, i2c_master_dev_handle_t lcd_handle);

void send_data(uint8_t data, i2c_master_dev_handle_t lcd_handle);

void move_cursor(uint8_t location, i2c_master_dev_handle_t lcd_handle);

#endif