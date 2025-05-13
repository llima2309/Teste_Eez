#include <lvgl.h>
#include <TFT_eSPI.h>
#include "ui.h" // inclua o header gerado pelo EEZ
#include "images.h"
#include <Preferences.h>


#define LV_LVGL_H_INCLUDE_SIMPLE 1
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint8_t *draw_buf;
uint32_t lastTick = 0;
Preferences preferences;

void update_UI()
{
  lv_tick_inc(millis() - lastTick); //--> Update the tick timer. Tick is new for LVGL 9.
  lastTick = millis();
  lv_timer_handler(); //--> Update the UI.
}
void log_print(lv_log_level_t level, const char *buf)
{
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}
void setup()
{
  Serial.begin(115200);
  Serial.println();

  //----------------------------------------LVGL setup.
  // Start LVGL.
  lv_init();
  // Create a display object.
  lv_display_t *disp;
  // Initialize the TFT display using the TFT_eSPI library.
  draw_buf = new uint8_t[DRAW_BUF_SIZE];
  disp = lv_tft_espi_create(SCREEN_HEIGHT, SCREEN_WIDTH, draw_buf, DRAW_BUF_SIZE);
  // If the display on the TFT LCD has problems, try the line of code below:
  // disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, DRAW_BUF_SIZE);
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);
  Serial.println("LVGL Setup Completed.");
  delay(500);
  //----------------------------------------

  // Integrate EEZ Studio GUI.
  ui_init();
  lv_label_set_text(objects.label_nome_maquina,"Tear 25");
  update_UI();
  Serial.println();
  Serial.println("Setup complete.");
}

void loop()
{
  lv_led_set_color(objects.ledindicador, lv_color_hex(0x00ff0f));
  update_UI();
  delay(5000);
  lv_led_set_color(objects.ledindicador, lv_color_hex(0x0032ff));
  update_UI();
  delay(5000);
}
