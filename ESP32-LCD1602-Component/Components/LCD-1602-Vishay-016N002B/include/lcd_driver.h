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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "lcd_data.h"
#include "lcd_commands.h"

typedef struct {
	uint8_t bit_mode;
	uint8_t lines;
	uint8_t font;
	uint8_t display_state;
	uint8_t cursor_shown_state;
	uint8_t cursor_blink_state;
	uint8_t curs_dir;
	uint8_t screen_shift;
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

void initialize_bus_and_lcd_i2c(LCD_Device_Config* lcd, I2C_MST_Config* mst_bus);

void initialize_lcd_i2c(i2c_master_bus_handle_t* mst_handle, LCD_Device_Config* lcd);

void initialize_lcd_function(i2c_master_dev_handle_t lcd_handle, LCD_Start_Config* config);

void send_cmd(uint8_t cmd, i2c_master_dev_handle_t lcd_handle);

void send_data(uint8_t data, i2c_master_dev_handle_t lcd_handle);

#endif