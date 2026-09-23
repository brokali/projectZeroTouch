#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "esp_console.h"
#include "esp_log.h"

static const char *TAG = "TZ_GUI";

static lv_obj_t *terminal_output_log;
static lv_obj_t *command_input_box;
static lv_obj_t *touch_keyboard;

void print_to_lcd_terminal(const char *format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (terminal_output_log) {
        lv_textarea_add_text(terminal_output_log, buffer);
    }
    printf("%s", buffer);
}

static void keyboard_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_READY) {
        const char *raw_command = lv_textarea_get_text(command_input_box);
        
        if (strlen(raw_command) == 0) return;
        
        print_to_lcd_terminal("\nprojectZero# %s\n", raw_command);
        
        int execution_return_code;
        esp_err_t result = esp_console_run(raw_command, &execution_return_code);
        
        if (result == ESP_ERR_NOT_FOUND) {
            print_to_lcd_terminal("Error: Unknown command syntax.\n");
        } else if (result == ESP_FAIL) {
            print_to_lcd_terminal("Command execution failure.\n");
        }
        
        lv_textarea_set_text(command_input_box, "");
    }
}

void init_projectzero_touch_terminal(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    
    terminal_output_log = lv_textarea_create(scr);
    lv_obj_set_size(terminal_output_log, 240, 140);
    lv_obj_align(terminal_output_log, LV_ALIGN_TOP_MID, 0, 5);
    lv_textarea_set_cursor_click_pos(terminal_output_log, false);
    lv_obj_set_style_bg_color(terminal_output_log, lv_color_make(15, 15, 15), 0);
    lv_obj_set_style_text_color(terminal_output_log, lv_color_make(0, 255, 0), 0);
    
    command_input_box = lv_textarea_create(scr);
    lv_obj_set_size(command_input_box, 240, 35);
    lv_obj_align(command_input_box, LV_ALIGN_CENTER, 0, -15);
    lv_textarea_set_one_line(command_input_box, true);
    lv_obj_set_style_text_color(command_input_box, lv_color_white(), 0);
    
    touch_keyboard = lv_keyboard_create(scr);
    lv_obj_set_size(touch_keyboard, 240, 115);
    lv_obj_align(touch_keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    
    lv_keyboard_set_textarea(touch_keyboard, command_input_box);
    lv_obj_add_event_cb(touch_keyboard, keyboard_event_handler, LV_EVENT_READY, NULL);
    
    print_to_lcd_terminal("--- projectZero CLI Shell ---\nType command and hit Enter...\n");
}
