
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"

/*********************
 *      DEFINES
 *********************/

#if DEBUG
#define ECU_LOADING_DONE 1
#else
#define ECU_LOADING_DONE 5
#endif

    /**********************
     *      TYPEDEFS
     **********************/
    /**********************
     *  STATIC PROTOTYPES
     **********************/
    static lv_display_t *
    hal_init(int32_t w, int32_t h);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
  ECU_UI_LOADING,
  ECU_UI_MAIN

} ecu_ui_state_t;
ecu_ui_state_t state = ECU_UI_LOADING;
/**********************
 *      VARIABLES
 **********************/
extern lv_img_dsc_t wrover_startup;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void auto_step_cb(lv_timer_t *timer);

/**********************
 *   Test code
 **********************/

void set_main_screen()
{

  lv_obj_t *count_label;

  lv_obj_clean(lv_scr_act());

  count_label = lv_label_create(lv_scr_act());
  lv_label_set_text(count_label, "Welcome!!");
  lv_obj_align(count_label, LV_ALIGN_BOTTOM_MID, 0, 0);
}

void set_startup_screen()
{

  lv_obj_t *hello_world_label, *img;
  lv_obj_t *count_label;

  img = lv_img_create(lv_scr_act());
  lv_img_set_src(img, &wrover_startup);

  // lv_label_set_text(hello_world_label, "Hello world!");
  // lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);

  count_label = lv_label_create(lv_scr_act());
  lv_label_set_text(count_label, "Loading");
  lv_obj_align(count_label, LV_ALIGN_BOTTOM_MID, 0, 0);

  lv_timer_create(auto_step_cb, 1000, NULL);
  // lv_task_handler();
  // display_blanking_off(display_dev);
}

static void auto_step_cb(lv_timer_t *t)
{
  LV_UNUSED(t);
  static uint32_t loading_delay = 0;

  // const lv_font_t * font_small = &lv_font_montserrat_12;
  // const lv_font_t * font_large = &lv_font_montserrat_16;
  printf("delay %d", loading_delay);
  switch (state)
  {
  case ECU_UI_LOADING:
    loading_delay++;
    if (ECU_LOADING_DONE == loading_delay)
    {
      state = ECU_UI_MAIN;
      set_main_screen();
    }
    break;

  case ECU_UI_MAIN:
    break;
  }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init(320, 240);

  set_startup_screen();

  while (1)
  {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    lv_timer_handler();
    usleep(5 * 1000);
  }

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static lv_display_t *hal_init(int32_t w, int32_t h)
{

  lv_group_set_default(lv_group_create());

  lv_display_t *disp = lv_sdl_window_create(w, h);

  lv_indev_t *mouse = lv_sdl_mouse_create();
  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_display(mouse, disp);
  lv_display_set_default(disp);

  LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  lv_obj_t *cursor_obj;
  cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
  lv_image_set_src(cursor_obj, &mouse_cursor_icon); /*Set the image source*/
  lv_indev_set_cursor(mouse, cursor_obj);           /*Connect the image  object to the driver*/

  lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
  lv_indev_set_display(mousewheel, disp);
  lv_indev_set_group(mousewheel, lv_group_get_default());

  lv_indev_t *kb = lv_sdl_keyboard_create();
  lv_indev_set_display(kb, disp);
  lv_indev_set_group(kb, lv_group_get_default());

  return disp;
}