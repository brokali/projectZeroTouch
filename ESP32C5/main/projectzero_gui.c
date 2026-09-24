#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "esp_console.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"

#define I2C_MASTER_SCL_IO           1    // Onboard I2C SCL for Waveshare ESP32-C5
#define I2C_MASTER_SDA_IO           0    // Onboard I2C SDA for Waveshare ESP32-C5
#define I2C_MASTER_NUM              I2C_NUM_0
#define CH32V003_I2C_ADDR           0x24 // Factory address of the IO Expander chip

static const char *TAG = "TZ_GUI";

// Enhanced function that explicitly tracks and commands the companion chip registers
void init_waveshare_hardware_backlight(void) {
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_MASTER_NUM,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    
    i2c_master_bus_handle_t bus_handle;
    esp_err_t err = i2c_new_master_bus(&i2c_bus_config, &bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C Bus Init Mismatch");
        return;
    }
    
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = CH32V003_I2C_ADDR,
        .scl_speed_hz = 100000,
    };
    
    i2c_master_dev_handle_t dev_handle;
    if (i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle) != ESP_OK) {
        ESP_LOGE(TAG, "CH32V003 Expander Target Link Failure");
        return;
    }
    
    // Waveshare Companion Protocol: Issue explicit sequential configuration writes
    // [Register address byte, Command setting byte]
    uint8_t cmd_lcd_power_on[]  = {0x00, 0x01}; // Pull power rails up
    uint8_t cmd_lcd_reset_high[] = {0x01, 0x01}; // Release deep display reset state
    uint8_t cmd_backlight_max[]  = {0x02, 0xFF}; // Force maximum PWM duty brightness cycle
    
    // Transmit commands sequentially with small operational recovery delays
    i2c_master_transmit(dev_handle, cmd_lcd_power_on, 2, -1);
    vTaskDelay(pdMS_TO_TICKS(10));
    i2c_master_transmit(dev_handle, cmd_lcd_reset_high, 2, -1);
    vTaskDelay(pdMS_TO_TICKS(10));
    i2c_master_transmit(dev_handle, cmd_backlight_max, 2, -1);
    
    ESP_LOGI(TAG, "CH32V003 Controller Link Active. Backlight Power grid deployed.");
}

void print_to_lcd_terminal(const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    printf("[LCD_TERM]: %s", buffer);
}

// Background thread handling hardware triggers safely without locking application boot
void waveshare_gui_task(void *pvParameters) {
    // 1. Give projectZero ample time to detach console loops and settle internal systems
    vTaskDelay(pdMS_TO_TICKS(1200));

    // 2. Fire up the physical screen hardware backlight grid
    init_waveshare_hardware_backlight();

    print_to_lcd_terminal("--- projectZero Touch Console Matrix Live ---\n");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}

// Global initialization hook called safely inside main.c
void init_projectzero_touch_terminal(void) {
    xTaskCreate(waveshare_gui_task, "WS_GUI_Task", 3072, NULL, 4, NULL);
}
