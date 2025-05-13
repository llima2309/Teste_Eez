#include <lvgl.h>
#include <TFT_eSPI.h>
#include "ui.h" // inclua o header gerado pelo EEZ
#include <Preferences.h>

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
  delay(3000);

  Serial.println("Start setup.");

  String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.println();
  Serial.println(LVGL_Arduino);
  delay(500);

  //----------------------------------------LVGL setup.
  Serial.println();
  Serial.println("Start LVGL Setup...");
  delay(500);

  // Start LVGL.
  lv_init();

  // Register print function for debugging.
  lv_log_register_print_cb(log_print);

  // Create a display object.
  lv_display_t *disp;

  // Initialize the TFT display using the TFT_eSPI library.
  draw_buf = new uint8_t[DRAW_BUF_SIZE];
  disp = lv_tft_espi_create(SCREEN_HEIGHT, SCREEN_WIDTH, draw_buf, DRAW_BUF_SIZE);
  // If the display on the TFT LCD has problems, try the line of code below:
  // disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, DRAW_BUF_SIZE);
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);

  // Initialize an LVGL input device object (Touchscreen).
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);

  // Set the callback function to read Touchscreen input.

  // my_group = lv_group_create();
  // lv_indev_set_group(indev, my_group);

  Serial.println("LVGL Setup Completed.");
  delay(500);
  //----------------------------------------

  // Integrate EEZ Studio GUI.
  ui_init();
  Serial.println();
  Serial.println("Setup complete.");
}

void loop()
{
  update_UI();
  delay(5);
}
