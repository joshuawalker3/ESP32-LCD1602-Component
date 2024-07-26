#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd_driver.h"
#include "lcd_data.h"
#include "esp_log.h"

#define STACK_SIZE 5000
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define GLITCH_IGNORE 7
#define INTR_PRIORITY 0
#define LCD_ADDR 0x27
#define FREQ 100000

void hello_task(void* args);

void app_main(void)
{
	i2c_master_dev_handle_t lcd_handle = NULL;
	i2c_master_bus_handle_t mst_handle = NULL;

	I2C_MST_Config mst_bus = {
		.mst_handle = &mst_handle,
		.port = I2C_NUM_0,
		.sda = I2C_MASTER_SDA_IO,
		.scl = I2C_MASTER_SCL_IO,
		.clk_src = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = GLITCH_IGNORE,
		.intr_priority = INTR_PRIORITY,
		.enable_internal_pullup = false,
	};

	LCD_Device_Config lcd = {
		.lcd_handle = &lcd_handle,
		.addr_length = I2C_ADDR_BIT_LEN_7,
		.dev_addr = LCD_ADDR,
		.speed = FREQ,
		.wait_time = 0, //default
	};

	LCD_Start_Config lcd_config = {
		.func_opt = BIT_MODE_4|LINES_2|FONT_5x8,
		.disp_opt = DISP_ON|CURS_OFF|CURS_BLINK,
		.entry_opt = CURS_INCR|SCREEN_SHIFT_DIS,
	};

	initialize_bus_and_lcd_i2c(&lcd, &mst_bus);
	initialize_lcd_function(lcd_handle, &lcd_config);
	generate_cust_character(CUSTOM_CHAR_1, heart_char, lcd_handle);

	xTaskCreate(hello_task, "Hello", STACK_SIZE, (void*)&lcd_handle, 2, NULL);

}

void hello_task(void* args) {
	i2c_master_dev_handle_t lcd_handle = *(i2c_master_dev_handle_t*)args;
	const char* TAG = "task";

	char* hello = "Hello, world!";

	while(1) {
		ESP_LOGI(TAG, "Printing...");

		put_string(hello, lcd_handle);

		write_cust_character(CUSTOM_CHAR_1, lcd_handle);

		cursor_off(lcd_handle);

		delay(1000);

		cursor_on_static(lcd_handle);

		delay(1000);

		cursor_on_blink(lcd_handle);

		send_cmd(CLEAR_DISPLAY, lcd_handle);

		delay(2000);

		put_string_center(hello, lcd_handle);

		move_cursor(1, lcd_handle);

		delay(2000);

		send_cmd(CLEAR_DISPLAY, lcd_handle);

		delay(2000);
	}
}
