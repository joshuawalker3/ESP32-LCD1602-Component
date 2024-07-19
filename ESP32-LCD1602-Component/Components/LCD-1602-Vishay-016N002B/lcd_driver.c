/*
Author: Joshua Walker
Date: 18-July-2024
Version: 1.0
*/

#include "driver/i2c_master.h"
#include "lcd_driver.h"
#include "lcd_data.h"
#include "lcd_commands.h"

void initialize_bus_and_lcd_i2c(LCD_Device_Config* lcd, I2C_MST_Config* mst_bus) {
	i2c_master_bus_config_t mst_config = {
		.i2c_port = mst_bus->port,
		.sda_io_num = mst_bus->sda,
		.scl_io_num = mst_bus->scl,
		.clk_source = mst_bus->clk_src,
		.glitch_ignore_cnt = mst_bus->glitch_ignore_cnt,
		.intr_priority = mst_bus->intr_priority,
		.flags.enable_internal_pullup = mst_bus->enable_internal_pullup, 
	};

	ESP_ERROR_CHECK(i2c_new_master_bus(&mst_config, mst_bus->mst_handle));

	i2c_device_config_t lcd_config = {
		.dev_addr_length = lcd->addr_length,
		.device_address = lcd->dev_addr,
		.scl_speed_hz = lcd->speed,
		.scl_wait_us = lcd->wait_time,
	};

	ESP_ERROR_CHECK(i2c_master_bus_add_device(*(mst_bus->mst_handle), &lcd_config, lcd->lcd_handle));
}

void initialize_lcd_i2c(i2c_master_bus_handle_t* mst_handle, LCD_Device_Config* lcd) {
	i2c_device_config_t lcd_config = {
		.dev_addr_length = lcd->addr_length,
		.device_address = lcd->dev_addr,
		.scl_speed_hz = lcd->speed,
		.scl_wait_us = lcd->wait_time,
	};

	ESP_ERROR_CHECK(i2c_master_bus_add_device(*mst_handle, &lcd_config, lcd->lcd_handle));
}

void initialize_lcd_function(i2c_master_dev_handle_t lcd_handle, LCD_Start_Config* config) {
	uint8_t function_options = config->bit_mode|config->lines|config->font;
	uint8_t display_options = config->display_state|config->cursor_shown_state|config->cursor_blink_state;
	uint8_t entry_mode_options = config->curs_dir|config->screen_shift;

	send_cmd(FUNCTION_SET|BIT_MODE_8|LINES_1|FONT_5x8, lcd_handle);

	delay(1);

	send_cmd(FUNCTION_SET|function_options, lcd_handle);

	delay(1);

	send_cmd(FUNCTION_SET|function_options, lcd_handle);

	delay(1);

	send_cmd(DISP_CTRL|display_options, lcd_handle);

	delay(10);

	send_cmd(CLEAR_DISPLAY, lcd_handle);

	delay(10);

	send_cmd(ENTRY_MODE_SET|entry_mode_options, lcd_handle);

	delay(100);
}

void send_cmd(uint8_t cmd,  i2c_master_dev_handle_t lcd_handle) {
	uint8_t upper_data = (cmd&BIT_COMP);
	uint8_t lower_data = ((cmd<<4)&BIT_COMP);

	uint8_t data_byte[4];

	data_byte[0] = upper_data|EN_HI;
	data_byte[1] = upper_data|ALL_LO;
	data_byte[2] = lower_data|EN_HI;
	data_byte[3] = lower_data|ALL_LO; 

	ESP_ERROR_CHECK(i2c_master_transmit(lcd_handle, data_byte, 4, 100));
}

void send_data(uint8_t data, i2c_master_dev_handle_t lcd_handle) {
	uint8_t upper_data = (data&BIT_COMP);
	uint8_t lower_data = ((data<<4)&BIT_COMP);

	uint8_t data_byte[4];

	data_byte[0] = upper_data|(EN_HI|RS_HI);
	data_byte[1] = upper_data|(ALL_LO|RS_HI);
	data_byte[2] = lower_data|(EN_HI|RS_HI);
	data_byte[3] = lower_data|(ALL_LO|RS_HI); 

	ESP_ERROR_CHECK(i2c_master_transmit(lcd_handle, data_byte, 4, 100));
}