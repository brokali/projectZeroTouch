#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   GRAPHICAL LAYER SETTINGS
 *====================*/
#define LV_COLOR_DEPTH          16  /* 16-bit RGB565 color profiling for the ST7789 panel */
#define LV_COLOR_16_SWAP        1   /* Waveshare panels require byte swapping for true color */

/*====================
   MALLOC ENGINE DEFINITIONS
 *====================*/
#define LV_USE_BUILTIN_MALLOC   1
#define LV_USE_BUILTIN_STRING   1
#define LV_USE_BUILTIN_SPRINTF  1

/*====================
   SYSTEM CONTROLS AND TICKING
 *====================*/
#define LV_DEF_REFR_PERIOD      33  /* ~30 Frames Per Second screen update cycles */
#define LV_DPI_DEF              130 /* Dot density tracking scale for a 2.8" layout grid */

/*====================
   WIDGET RESOURCE ALLOCATIONS
 *====================*/
#define LV_USE_ANIMATION        1
#define LV_USE_TEXTAREA         1
#define LV_USE_KEYBOARD         1

/*====================
   FONT ENGINE REGISTER MAP
 *====================*/
#define LV_FONT_MONO_12         1  /* Retro terminal font mapping style orientation */
#define LV_FONT_DEFAULT         &lv_font_montserrat_14

#endif /*LV_CONF_H*/
