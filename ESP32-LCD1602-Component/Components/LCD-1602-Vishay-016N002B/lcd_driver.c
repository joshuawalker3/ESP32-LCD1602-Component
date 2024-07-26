/*
Author: Joshua Walker
Date: 18-July-2024
Version: 1.0
*/

#include "driver/i2c_master.h"
#include "lcd_driver.h"
#include "lcd_commands.h"
#include "lcd_data.h"
#include "esp_log.h"

uint8_t curs_location = 0x00;

uint8_t heart_char[8] = { 0x00,
						  0x0A,
						  0x15,
						  0x11,
						  0x0A,
						  0x04,
						  0x00,
						  0x00 };

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

	ESP_ERROR_CHECK(i2c_master_bus_add_device(*mst_bus->mst_handle, &lcd_config, lcd->lcd_handle));
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
	const char* TAG = "Initialize LCD Function";

	//delay(100);

	send_cmd(FUNCTION_SET|BIT_MODE_8, lcd_handle);

	delay(SHORT_DELAY);

	send_cmd(FUNCTION_SET|BIT_MODE_4|LINES_1|FONT_5x8, lcd_handle);

	delay(SHORT_DELAY);

	send_cmd(FUNCTION_SET|config->func_opt, lcd_handle);

	delay(SHORT_DELAY);

	send_cmd(DISP_CTRL|config->disp_opt, lcd_handle);

	delay(SHORT_DELAY);

	send_cmd(CLEAR_DISPLAY, lcd_handle);

	delay(LONG_DELAY);

	send_cmd(ENTRY_MODE_SET|config->entry_opt, lcd_handle);

	delay(SHORT_DELAY);

	send_cmd(RET_HOME, lcd_handle);

	delay(LONG_DELAY);

	ESP_LOGI(TAG, "Init complete...");
}

void put_string(const char* string, i2c_master_dev_handle_t lcd_handle) {
	int i = 0;

	while (string[i] != '\0') {
		send_data(string[i++], lcd_handle);
		delay(SHORT_DELAY);
	}
}

void put_string_center(const char* string, i2c_master_dev_handle_t lcd_handle) {
	uint8_t size = 0;
	uint8_t i = 0;

	while (string[i] != '\0') {
		send_data(string[i++], lcd_handle);
		delay(SHORT_DELAY);
		size++;
	}

	uint8_t shift = (COL - size) / 2;

	shift_screen_right(shift, lcd_handle);
}

void generate_cust_character(uint8_t addr, uint8_t row[8], i2c_master_dev_handle_t lcd_handle) {
	if (addr > 0x07) {
		return;
	}

	send_cmd(CGRAM_SET|(addr<<3), lcd_handle);

	for (uint8_t i = 0x00; i < 8; i++) {
		send_data(row[i], lcd_handle);
		delay(SHORT_DELAY);
	}

	send_cmd(RET_HOME, lcd_handle);

	delay(LONG_DELAY);
}

void write_cust_character(uint8_t addr, i2c_master_dev_handle_t lcd_handle) {
	send_data(addr, lcd_handle);

	delay(SHORT_DELAY);
}

void cursor_on_static(i2c_master_dev_handle_t lcd_handle) {
	send_cmd(DISP_CTRL|DISP_ON|CURS_ON|CURS_SOLID, lcd_handle);

	delay(SHORT_DELAY);
}

void cursor_on_blink(i2c_master_dev_handle_t lcd_handle) {
	send_cmd(DISP_CTRL|DISP_ON|CURS_ON|CURS_BLINK, lcd_handle);

	delay(SHORT_DELAY);
}

void cursor_off(i2c_master_dev_handle_t lcd_handle) {
	send_cmd(DISP_CTRL|DISP_ON|CURS_OFF,lcd_handle);

	delay(SHORT_DELAY);
}

void shift_screen_right(uint8_t distance, i2c_master_dev_handle_t lcd_handle) {
	for (uint8_t i = 0; i < distance; i++) {
		send_cmd(CURS_DISP_SHIFT|SHIFT_DISP|SHIFT_RIGHT, lcd_handle);
		delay(SHORT_DELAY);
	}
}

void shift_screen_left(uint8_t distance, i2c_master_dev_handle_t lcd_handle) {
	for (uint8_t i = 0; i < distance; i++) {
		send_cmd(CURS_DISP_SHIFT|SHIFT_DISP|SHIFT_LEFT, lcd_handle);
		delay(SHORT_DELAY);
	}
}

void send_cmd(uint8_t cmd,  i2c_master_dev_handle_t lcd_handle) {
	uint8_t upper_data = (cmd&BIT_COMP);
	uint8_t lower_data = ((cmd<<4)&BIT_COMP);

	uint8_t data_byte[4];

	data_byte[0] = upper_data|EN_HI;
	data_byte[1] = upper_data|ALL_LO;
	data_byte[2] = lower_data|EN_HI;
	data_byte[3] = lower_data|ALL_LO; 

	ESP_ERROR_CHECK(i2c_master_transmit(lcd_handle, data_byte, 4, MAX_TIMEOUT));
}

void send_data(uint8_t data, i2c_master_dev_handle_t lcd_handle) {
	uint8_t upper_data = (data&BIT_COMP);
	uint8_t lower_data = ((data<<4)&BIT_COMP);

	uint8_t data_byte[4];

	data_byte[0] = upper_data|(EN_HI|RS_HI);
	data_byte[1] = upper_data|(ALL_LO|RS_HI);
	data_byte[2] = lower_data|(EN_HI|RS_HI);
	data_byte[3] = lower_data|(ALL_LO|RS_HI); 

	ESP_ERROR_CHECK(i2c_master_transmit(lcd_handle, data_byte, 4, MAX_TIMEOUT));
}

void move_cursor(uint8_t location, i2c_master_dev_handle_t lcd_handle) {
	if (location > 0x67) { 
		curs_location = location = 0x67; 
	}
	if (location > 0x27 && location < 0x40) {
		curs_location = location = 0x40;
	}

	send_cmd(DDRAM_SET|curs_location, lcd_handle);

	delay(SHORT_DELAY);
}