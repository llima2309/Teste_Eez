// //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 03_Dynamic_WiFi_Configuration_-_WiFi_Settings
// //---------------------------------------- Including Libraries.
// #include "WiFi.h"
// #include <lvgl.h>
// #include <TFT_eSPI.h>
// #include <XPT2046_Touchscreen.h>
// #include "ui.h"
// #include "images.h"
// #include <Preferences.h>
// #include "time.h"
// //---------------------------------------- 

// // Defines the T_CS Touchscreen PIN.
// #define T_CS_PIN  13 //--> T_CS

// // Defines the screen resolution.
// #define SCREEN_WIDTH  320
// #define SCREEN_HEIGHT 240

// //---------------------------------------- Defines the Touchscreen calibration result value.
// // Replace with the calibration results on your TFT LCD Touchscreen.
// #define touchscreen_Min_X 0
// #define touchscreen_Max_X 0
// #define touchscreen_Min_Y 0
// #define touchscreen_Max_Y 0
// //---------------------------------------- 

// // LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes.
// #define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
// uint8_t *draw_buf;

// // Variables for x, y and z values ​​on the touchscreen.
// uint16_t x, y, z;

// // Used to track the tick timer.
// uint32_t lastTick = 0;

// bool execute_WiFi_Scan = false;
// bool execute_Connecting_To_WiFi = false;
// bool execute_Get_TimeDate_from_NTP_server = false;

// String get_Selected_SSID;
// String get_Entered_SSID_Password;

// bool Wifi_On_Off_Status = false;
// bool auto_Connect = false;
// String saved_SSID;
// String saved_Password;

// //---------------------------------------- NTP Server and its settings.
// // The DS3231 RTC module is not always accurate. So every time the ESP32 is turned on or rebooted,
// // the time and date on the DS3231 RTC module will be set based on the time from the NTP Server.
// // Please adjust the settings below to suit your area.

// // Source : https://lastminuteengineers.com/esp32-ntp-server-date-time-tutorial/

// const char* ntpServer = "pool.ntp.org";

// // Example setting for "gmtOffset_sec".
// // - For UTC -5.00 : -5 * 60 * 60 = -18000
// // - For UTC +1.00 : 1 * 60 * 60 = 3600
// // - For UTC +0.00 : 0 * 60 * 60 = 0
// // Check the UTC list here: https://en.wikipedia.org/wiki/List_of_UTC_offsets
// // Where I live uses UTC+07:00, so : 7 * 60 * 60 = 25200
// // or 3600 * 7 = 25200
// const long  gmtOffset_sec = 3600 * 7;

// // Set it to 3600 if your country observes Daylight saving time. Otherwise, set it to 0.
// // https://en.wikipedia.org/wiki/Daylight_saving_time
// const int   daylightOffset_sec = 0;
// //----------------------------------------

// // Variable for status of getting time and date data from NTP Server.
// // true = success.
// // false = failure.
// bool get_NTP_TimeDate_Sta = false;

// int d_month, d_day, d_year;
// int t_hour, t_minute, t_second;

// // Timer/millis variable for time and date updates.
// unsigned long previousMillis = 0;
// const long interval = 1000;

// // Declaring the "XPT2046_Touchscreen" object as "touchscreen" and its settings.
// XPT2046_Touchscreen touchscreen(T_CS_PIN);

// // Declaring the "Preferences" object as "preferences".
// Preferences preferences;



// //________________________________________________________________________________ log_print()
// // If logging is enabled, it will inform the user about what is happening in the library.
// void log_print(lv_log_level_t level, const char * buf) {
//   LV_UNUSED(level);
//   Serial.println(buf);
//   Serial.flush();
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ update_UI()
// // Subroutines to update screen displays or widgets.
// void update_UI() {
//   lv_tick_inc(millis() - lastTick); //--> Update the tick timer. Tick is new for LVGL 9.
//   lastTick = millis();
//   lv_timer_handler(); //--> Update the UI.
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ touchscreen_read()
// // Get the Touchscreen data.
// void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
//   // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z)
//   if (touchscreen.touched()) {
//     // Get Touchscreen points
//     TS_Point p = touchscreen.getPoint();
    
//     //---------------------------------------- 
//     // Calibrate Touchscreen points with map function to the correct width and height.
//     x = map(p.x, touchscreen_Max_X, touchscreen_Min_X, 1, SCREEN_HEIGHT);
//     y = map(p.y, touchscreen_Max_Y, touchscreen_Min_Y, 1, SCREEN_WIDTH);
    
//     // If the touchscreen on the TFT LCD is upside down, try the code line below.
//     // If there is an error on the touchscreen, edit and try other settings in the code section below.
//     // Because my TFT LCD Touchscreen device may have different settings from your TFT LCD device.
//     //x = map(p.x, touchscreen_Min_X, touchscreen_Max_X, 1, SCREEN_HEIGHT);
//     //y = map(p.y, touchscreen_Min_Y, touchscreen_Max_Y, 1, SCREEN_WIDTH);
//     //---------------------------------------- 
    
//     z = p.z;

//     // Set the coordinates.
//     data->point.x = x;
//     data->point.y = y;

//     data->state = LV_INDEV_STATE_PRESSED;

//     // Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor.
//     /*Serial.print("X = ");
//     Serial.print(x);
//     Serial.print(" | Y = ");
//     Serial.print(y);
//     Serial.print(" | Pressure = ");
//     Serial.print(z);
//     Serial.println();*/
//   }
//   else {
//     data->state = LV_INDEV_STATE_RELEASED;
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ wifi_Scan()
// // Subroutine to scan WiFi networks.
// void wifi_Scan() {
//   // Creates a delay of a few milliseconds, so the "switch" can complete its click animation.
//   for (byte i = 0; i < 50; i++) {update_UI(); delay(20);}
//   delay(100);

//   bool saved_SSID_Found = false;
  
//   Serial.println();
//   Serial.println("-----------------WiFi Scan");
//   Serial.println("WiFi scan started. Please wait...");

//   lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_process);
//   lv_obj_clean(objects.list_wifi_network_ws);
//   lv_list_add_text(objects.list_wifi_network_ws, "WiFi scan started.\nPlease wait...");
//   update_UI();

//   // WiFi.scanNetworks will return the number of networks found.
//   int n = WiFi.scanNetworks();
//   Serial.println();
//   Serial.println("Scan done.");

//   //----------------------------------------WiFi network scan results.
//   if (n == 0 || n < 0) {
//     Serial.println();
//     Serial.println("No networks found.");

//     lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_on);
//     lv_obj_clean(objects.list_wifi_network_ws);
//     lv_list_add_text(objects.list_wifi_network_ws, "No networks found.");
//     update_UI();
//   } else {
//     String msg = String(n) + " networks found.";
    
//     lv_obj_clean(objects.list_wifi_network_ws);
//     lv_list_add_text(objects.list_wifi_network_ws, msg.c_str());
//     update_UI();

//     lv_obj_t * btn;
    
//     Serial.println();
//     Serial.print(n);
//     Serial.println(" networks found.");
//     Serial.println();
//     Serial.println("No | SSID                             | RSSI | CH | Encryption");
//     for (int i = 0; i < n; ++i) {
//       btn = lv_list_add_button(objects.list_wifi_network_ws, LV_SYMBOL_WIFI, WiFi.SSID(i).c_str());
//       lv_obj_add_event_cb(btn, list_wifi_network_ws_event_handler, LV_EVENT_CLICKED, NULL);
//       update_UI();

//       if (auto_Connect == true) {
//         if (saved_SSID == WiFi.SSID(i)) {
//           saved_SSID_Found = true;
//         }
//       }
    
//       // Print SSID and RSSI for each network found.
//       Serial.printf("%2d",i + 1);
//       Serial.print(" | ");
//       Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
//       Serial.print(" | ");
//       Serial.printf("%4d", WiFi.RSSI(i));
//       Serial.print(" | ");
//       Serial.printf("%2d", WiFi.channel(i));
//       Serial.print(" | ");
//       switch (WiFi.encryptionType(i)) {
//         case WIFI_AUTH_OPEN:
//           Serial.print("open");
//           break;
//         case WIFI_AUTH_WEP:
//           Serial.print("WEP");
//           break;
//           case WIFI_AUTH_WPA_PSK:
//           Serial.print("WPA");
//           break;
//         case WIFI_AUTH_WPA2_PSK:
//           Serial.print("WPA2");
//           break;
//         case WIFI_AUTH_WPA_WPA2_PSK:
//           Serial.print("WPA+WPA2");
//           break;
//         case WIFI_AUTH_WPA2_ENTERPRISE:
//           Serial.print("WPA2-EAP");
//           break;
//         case WIFI_AUTH_WPA3_PSK:
//           Serial.print("WPA3");
//           break;
//         case WIFI_AUTH_WPA2_WPA3_PSK:
//           Serial.print("WPA2+WPA3");
//           break;
//         case WIFI_AUTH_WAPI_PSK:
//           Serial.print("WAPI");
//           break;
//         default:
//           Serial.print("unknown");
//       }
//       Serial.println();
//       delay(10);
//     }
//   }
//   //---------------------------------------- 

//   lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_on);
//   update_UI();
  
//   Serial.println("-----------------");

//   // Delete the scan result to free memory for code below.
//   WiFi.scanDelete();

//   // Conditions for auto-connect.
//   if (saved_SSID_Found == true && auto_Connect == true) {
//     get_Selected_SSID = saved_SSID;
//     get_Entered_SSID_Password = saved_Password;
//     connecting_To_WiFi();
//   }

//   // The WiFi network scan is complete, so set "execute_WiFi_Scan" to "false".
//   execute_WiFi_Scan = false;
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ connecting_To_WiFi()
// void connecting_To_WiFi() {
//   // Creates a delay of a few milliseconds, so the "button" can complete its click animation.
//   for (byte i = 0; i < 50; i++) {update_UI(); delay(20);}
//   delay(100);

//   lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_process);
//   lv_obj_add_state(objects.button_connect_ws, LV_STATE_DISABLED);
//   lv_obj_add_state(objects.button_cancel_ws, LV_STATE_DISABLED);
//   lv_obj_set_style_text_color(objects.label_connecting_process_info_ws, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
//   lv_label_set_text(objects.label_connecting_process_info_ws, "");
//   update_UI();

//   if (auto_Connect == true) {
//     String msg = "Connecting to : " + get_Selected_SSID + ".\nPlease wait...";
//     lv_obj_t * child = lv_obj_get_child(objects.list_wifi_network_ws, 0);
//     lv_label_set_text(child, msg.c_str());

//     update_UI();
//   }
  
//   //---------------------------------------- Connect to Wi-Fi.
//   Serial.println();
//   Serial.println("-------------Connecting To WiFi");
//   Serial.print("Connecting to : ");
//   Serial.println(get_Selected_SSID);
//   WiFi.disconnect();
//   WiFi.begin(get_Selected_SSID.c_str(), get_Entered_SSID_Password.c_str());

//   String PW;
//   byte cnt_PW = 0;
  
//   //:::::::::::::::::: The process of connecting ESP32 with WiFi Hotspot / WiFi Router.
//   int connecting_process_timed_out = 10; //--> 10 = 10 seconds.
//   connecting_process_timed_out = connecting_process_timed_out * 2;
//   while (WiFi.status() != WL_CONNECTED) {
//     cnt_PW++;
//     if (cnt_PW > 3) cnt_PW = 1;

//     if (cnt_PW == 1) {
//       PW = "Connecting ! Please wait.";
//     } else if (cnt_PW == 2) {
//       PW = "Connecting ! Please wait..";
//     } else if (cnt_PW == 3) {
//       PW = "Connecting ! Please wait...";
//     } 
    
//     lv_label_set_text(objects.label_connecting_process_info_ws, PW.c_str());
//     update_UI();
    
//     Serial.print(".");
//     delay(500);
    
//     if(connecting_process_timed_out > 0) connecting_process_timed_out--;
//     if(connecting_process_timed_out == 0) {
//       Serial.println();
//       Serial.println("Failed to connect to WiFi.");
//       Serial.println("Double check your password or try connecting again.");
//       Serial.println("-------------");

//       lv_obj_remove_state(objects.button_connect_ws, LV_STATE_DISABLED);
//       lv_obj_remove_state(objects.button_cancel_ws, LV_STATE_DISABLED);

//       lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_on);
  
//       lv_obj_set_style_text_color(objects.label_connecting_process_info_ws, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//       lv_label_set_text(objects.label_connecting_process_info_ws, "Failed ! Double check the password or try connecting again.");

//       if (auto_Connect == true) {
//         String msg = "Failed to connect to : " + get_Selected_SSID;
//         lv_obj_t * child = lv_obj_get_child(objects.list_wifi_network_ws, 0);
//         lv_label_set_text(child, msg.c_str());
//       }
      
//       update_UI();

//       execute_Connecting_To_WiFi = false;
      
//       delay(1000);
      
//       return;
//     }
//   }
  
//   Serial.println();
//   Serial.println("WiFi connected");
//   Serial.print("Successfully connected to : ");
//   Serial.println(get_Selected_SSID);
//   Serial.println("-------------");

//   lv_obj_remove_state(objects.button_connect_ws, LV_STATE_DISABLED);
//   lv_obj_remove_state(objects.button_cancel_ws, LV_STATE_DISABLED);

//   lv_obj_add_flag(objects.label_connecting_process_info_ws, LV_OBJ_FLAG_HIDDEN);
//   lv_obj_add_flag(objects.panel_connect_ws, LV_OBJ_FLAG_HIDDEN);
//   lv_obj_remove_flag(objects.panel_main_ws, LV_OBJ_FLAG_HIDDEN);
  
//   lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_connected);
//   lv_img_set_src(objects.image_wifi_indicator_hm, &img_img_wifi_connected);
  
//   String msg = "Connected with :\n" + get_Selected_SSID;
//   lv_obj_t * child = lv_obj_get_child(objects.list_wifi_network_ws, 0);
//   lv_label_set_text(child, msg.c_str());
//   update_UI();

//   // The process of connecting to a WiFi network or access point is complete, so set "execute_Connecting_To_WiFi" to "false".
//   execute_Connecting_To_WiFi = false;
//   //:::::::::::::::::: 
//   //---------------------------------------- 
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ get_TimeDate_from_NTP_server()
// // Subroutine to get time and date data from NTP Server.
// void get_TimeDate_from_NTP_server() {
//   // Creates a delay of a few milliseconds, so the "button" can complete its click animation.
//   for (byte i = 0; i < 50; i++) {update_UI(); delay(20);}
//   delay(100);

//   get_NTP_TimeDate_Sta = false;

//   Serial.println();
//   Serial.println("-------------NTP server");
//   Serial.println("Get date and time from NTP server.");
//   Serial.println("Please wait...");

//   lv_label_set_text(objects.label_ntp_time_hm, "NN:NN:NN");
//   lv_label_set_text(objects.label_ntp_date_hm, "NN-NN-NNNN");

//   if (Wifi_On_Off_Status != true) {
//     Serial.println();
//     Serial.println("Failed ! WiFi is off.");
//     Serial.println("-------------");
  
//     lv_label_set_text(objects.label_ntp_info_hm, "Failed !\nWiFi is off.");
//     update_UI();
//     delay(2000);

//     lv_label_set_text(objects.label_ntp_info_hm, "NTP Clock");
//     update_UI();
    
//     return;
//   }

//   if (WiFi.status() != WL_CONNECTED) {
//     Serial.println();
//     Serial.println("Failed ! WiFi not connected.");
//     Serial.println("-------------");
    
//     lv_label_set_text(objects.label_ntp_info_hm, "Failed !\nWiFi not connected.");
//     update_UI();
//     delay(2000);

//     lv_label_set_text(objects.label_ntp_info_hm, "NTP Clock");
//     update_UI();
    
//     return;
//   }
  
//   lv_label_set_text(objects.label_ntp_info_hm, "Get date & time from NTP server. Please wait...");
//   update_UI();

//   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
//   delay(50);
  
//   struct tm timeinfo;

//   if (!getLocalTime(&timeinfo)) {
//     get_NTP_TimeDate_Sta = false;
    
//     lv_label_set_text(objects.label_ntp_info_hm, "Failed to obtain date & time !");
//     update_UI();
  
//     Serial.println();
//     Serial.println("Failed to obtain date and time !");
//     Serial.println("-------------");
//     delay(2000);

//     lv_label_set_text(objects.label_ntp_info_hm, "NTP Clock");
//     update_UI();

//     return;
//   }

//   get_NTP_TimeDate_Sta = true;
  
//   lv_label_set_text(objects.label_ntp_info_hm, "Successfully got time and date from NTP Server.");
//   update_UI();

//   Serial.println();
//   Serial.println("Successfully set date and time from NTP server.");
//   Serial.println("-------------");
//   delay(2000);

//   lv_label_set_text(objects.label_ntp_info_hm, "NTP Clock");
//   update_UI();
  
//   delay(500);
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ image_wifi_indicator_hm_event_handler(lv_event_t * e)
// // Callback that is triggered when "image_wifi_indicator_hm" is clicked.
// static void image_wifi_indicator_hm_event_handler(lv_event_t * e) {
//   lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  
//   if (code == LV_EVENT_CLICKED) {
//     Serial.println();
//     Serial.println("image_wifi_indicator_hm is clicked/touched.");
//     Serial.println("Go to the WiFi Settings page.");
  
//     lv_scr_load_anim(objects.wifi_settings, LV_SCR_LOAD_ANIM_OVER_LEFT, 400, 0, false);

//     if (Wifi_On_Off_Status == true) {
//       lv_img_set_src(objects.image_wifi_indicator_hm, &img_img_wifi_on);
//       lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_on);

//       lv_obj_add_state(objects.switch_ctrl_wifi_ws, LV_STATE_CHECKED);
//       lv_label_set_text(objects.label_wifi_info_ws, "WiFi : On");
//     } else {
//       lv_img_set_src(objects.image_wifi_indicator_hm, &img_img_wifi_off);
//       lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_off);

//       lv_obj_remove_state(objects.switch_ctrl_wifi_ws, LV_STATE_CHECKED);
//       lv_label_set_text(objects.label_wifi_info_ws, "WiFi : Off");
//     }

//     if (auto_Connect == true) {
//       lv_obj_add_state(objects.checkbox_auto_connect_ws, LV_STATE_CHECKED);
//     } else {
//       lv_obj_remove_state(objects.checkbox_auto_connect_ws, LV_STATE_CHECKED);
//     }

//     if (Wifi_On_Off_Status == true && WiFi.status() == WL_CONNECTED) {
//       lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_connected);
//       lv_img_set_src(objects.image_wifi_indicator_hm, &img_img_wifi_connected);
//       String msg = "Connected with :\n" + get_Selected_SSID;
//       lv_obj_clean(objects.list_wifi_network_ws);
//       lv_list_add_text(objects.list_wifi_network_ws, msg.c_str());
//     }

//     if (Wifi_On_Off_Status == true && WiFi.status() != WL_CONNECTED) {
//       execute_WiFi_Scan = true;
//     }
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ button_refresh_ntp_hm_event_handler(lv_event_t * e)
// // Callback that is triggered when "button_refresh_ntp_hm" is clicked.
// static void button_refresh_ntp_hm_event_handler(lv_event_t * e) {
//   lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  
//   if (code == LV_EVENT_CLICKED) {
//     Serial.println();
//     Serial.println("button_refresh_ntp_hm is clicked/touched.");
//     Serial.println("Get time and date data from NTP Server.");
    
//     execute_Get_TimeDate_from_NTP_server = true;
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ button_back_ws_event_handler(lv_event_t * e)
// // Callback that is triggered when "button_back_ws" is clicked.
// static void button_back_ws_event_handler(lv_event_t * e) {
//   lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  
//   if (code == LV_EVENT_CLICKED) {
//     Serial.println();
//     Serial.println("button_back_ws is clicked/touched.");
//     Serial.println("Back to Home page.");
    
//     lv_obj_clean(objects.list_wifi_network_ws);
//     lv_scr_load_anim(objects.home, LV_SCR_LOAD_ANIM_OVER_LEFT, 400, 0, false);
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ switch_ctrl_wifi_ws_event_handler()
// // Callback that is triggered when "switch_ctrl_wifi_ws" is clicked/toggled.
// static void switch_ctrl_wifi_ws_event_handler(lv_event_t * e) {
//   lv_event_code_t code = lv_event_get_code(e);                    //--> Get the event code.
//   lv_obj_t * target_switch = (lv_obj_t*) lv_event_get_target(e);  //--> Switch that generated the event.
  
//   if (code == LV_EVENT_VALUE_CHANGED) {
//     LV_UNUSED(target_switch);

//     Serial.println();
//     Serial.println("switch_ctrl_wifi_ws is clicked/touched.");

//     bool checked = lv_obj_has_state(target_switch, LV_STATE_CHECKED);

//     if (checked == true) {
//       Serial.println("Turn on WiFi.");
//       WiFi.mode(WIFI_STA);
//       WiFi.disconnect();

//       //----------------------------------------Save WiFi status. WiFi status : On.
//       // Open Preferences with "mySet" namespace.
//       // Note: Namespace name is limited to 15 chars.
//       preferences.begin("mySet", false);
    
//       // Save settings to flash memory.
//       // Note: Key name is limited to 15 chars.
//       preferences.putBool("Wifi_On_Off_Sta", true);

//       // Get the "Wifi_On_Off_Status" value, if the key does not exist, return a default value of false.
//       Wifi_On_Off_Status = preferences.getBool("Wifi_On_Off_Sta", false);
    
//       // Close the Preferences.
//       preferences.end();
//       //---------------------------------------- 
      
//       lv_label_set_text(objects.label_wifi_info_ws, "WiFi : On");
//       lv_img_set_src(objects.image_wifi_indicator_hm, &img_img_wifi_on);
//       lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_on);
      
//       execute_WiFi_Scan = true;
//     } else {
//       Serial.println("Turn off WiFi.");
//       WiFi.disconnect();
//       WiFi.mode(WIFI_OFF);

//       //----------------------------------------Save WiFi status. WiFi status : Off.
//       // Open Preferences with "mySet" namespace.
//       // Note: Namespace name is limited to 15 chars.
//       preferences.begin("mySet", false);
    
//       // Save settings to flash memory.
//       // Note: Key name is limited to 15 chars.
//       preferences.putBool("Wifi_On_Off_Sta", false);

//       // Get the "Wifi_On_Off_Status" value, if the key does not exist, return a default value of false.
//       Wifi_On_Off_Status = preferences.getBool("Wifi_On_Off_Sta", false);
    
//       // Close the Preferences.
//       preferences.end();
//       //---------------------------------------- 
      
//       lv_label_set_text(objects.label_wifi_info_ws, "WiFI : Off");
//       lv_img_set_src(objects.image_wifi_indicator_hm, &img_img_wifi_off);
//       lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_off);
//       lv_obj_clean(objects.list_wifi_network_ws);
//     }
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ list_wifi_network_ws_event_handler(lv_event_t * e)
// // Callback that is triggered when the "list_wifi_network_ws" item is clicked.
// static void list_wifi_network_ws_event_handler(lv_event_t * e) {
//   lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
//   lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
  
//   if(code == LV_EVENT_CLICKED) {
//     LV_UNUSED(obj);
//     //LV_LOG_USER("Clicked: %s", lv_list_get_button_text(objects.list_wifi_network_ws, obj));

//     get_Selected_SSID = "";
//     get_Selected_SSID = String(lv_list_get_button_text(objects.list_wifi_network_ws, obj));

//     Serial.println();
//     Serial.print(get_Selected_SSID);
//     Serial.println(" is clicked/touched.");

//     if (WiFi.status() == WL_CONNECTED) {
//       if (get_Selected_SSID == saved_SSID) return;
//     }

//     // Show the "panel_connect_ws" panel.
//     lv_obj_remove_flag(objects.panel_connect_ws, LV_OBJ_FLAG_HIDDEN);

//     // Hide the "panel_main_ws" panel.
//     lv_obj_add_flag(objects.panel_main_ws, LV_OBJ_FLAG_HIDDEN);

//     if (get_Selected_SSID == saved_SSID) {
//       get_Selected_SSID = saved_SSID;
//       get_Entered_SSID_Password = saved_Password;

//       lv_textarea_set_text(objects.textarea_password_ws, get_Entered_SSID_Password.c_str());
//     } else {
//       lv_textarea_set_text(objects.textarea_password_ws, "");
//     }

//     String msg = "SSID : " + get_Selected_SSID;
//     lv_label_set_text(objects.label_selected_ssid_ws, msg.c_str());

//     lv_obj_remove_flag(objects.label_connecting_process_info_ws, LV_OBJ_FLAG_HIDDEN);
//     lv_obj_set_style_text_color(objects.label_connecting_process_info_ws, lv_color_hex(0xff2a2c3b), LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_label_set_text(objects.label_connecting_process_info_ws, "");

//     update_UI();
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ button_disconnect_ws_event_handler(lv_event_t * e)
// // Callback that is triggered when "button_disconnect_ws" is clicked.
// static void button_disconnect_ws_event_handler(lv_event_t * e) {
//   lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  
//   if (code == LV_EVENT_CLICKED) {
//     Serial.println();
//     Serial.println("button_disconnect_ws is clicked/touched.");
//     Serial.println("Disconnect WiFi.");
    
//     WiFi.disconnect();
    
//     lv_img_set_src(objects.image_wifi_indicator_hm, &img_img_wifi_on);
//     lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_on);
    
//     lv_obj_clean(objects.list_wifi_network_ws);
//     lv_list_add_text(objects.list_wifi_network_ws, "Not connected.");
//     lv_list_add_text(objects.list_wifi_network_ws, "To rescan the network, please turn off then turn on WiFi or touch the back button then return to this menu.");
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ textarea_password_ws_event_handler(lv_event_t * e)
// // Callback that is triggered when the "textarea_password_ws" is clicked.
// static void textarea_password_ws_event_handler(lv_event_t * e) {
//   lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
//   lv_obj_t * ta = (lv_obj_t*) lv_event_get_target(e);
//   lv_obj_t * kb = (lv_obj_t*) lv_event_get_user_data(e);
  
//   if(code == LV_EVENT_FOCUSED || code == LV_EVENT_CLICKED) {
//       lv_keyboard_set_textarea(kb, ta);
//       lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
//   }

//   if(code == LV_EVENT_DEFOCUSED) {
//       lv_keyboard_set_textarea(kb, NULL);
//       lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ button_connect_ws_event_handler(lv_event_t * e)
// // Callback that is triggered when "button_connect_ws" is clicked.
// static void button_connect_ws_event_handler(lv_event_t * e) {
//   lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  
//   if (code == LV_EVENT_CLICKED) {
//     Serial.println();
//     Serial.println("button_connect_ws is clicked/touched.");
//     Serial.println("Connect WiFi.");
    
//     get_Entered_SSID_Password = "";
//     get_Entered_SSID_Password = String(lv_textarea_get_text(objects.textarea_password_ws));

//     Serial.println();
//     Serial.print("SSID : ");
//     Serial.println(get_Selected_SSID);
//     Serial.print("PASS : ");
//     Serial.println(get_Entered_SSID_Password);

//     //----------------------------------------Save the SSID and Password.
//     // Open Preferences with "mySet" namespace.
//     // Note: Namespace name is limited to 15 chars.
//     preferences.begin("mySet", false);
    
//     // Get settings stored in flash memory.
//     saved_SSID     = preferences.getString("saved_SSID", "");
//     saved_Password = preferences.getString("saved_Password", "");

//     // Save settings to flash memory.
//     // Note: Key name is limited to 15 chars.
//     if (saved_SSID != get_Selected_SSID) {
//       preferences.putString("saved_SSID", get_Selected_SSID);
//       saved_SSID = preferences.getString("saved_SSID", "");
//     }
//     if (saved_Password != get_Entered_SSID_Password) {
//       preferences.putString("saved_Password", get_Entered_SSID_Password);
//       saved_Password = preferences.getString("saved_Password", "");
//     }
    
//     // Close the Preferences.
//     preferences.end();
//     //----------------------------------------

//     // SSID and Password are already obtained. So set "execute_Connecting_To_WiFi" to "true" to start connecting.
//     execute_Connecting_To_WiFi = true;
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ checkbox_auto_connect_ws_event_handler(lv_event_t * e)
// // Callback that is triggered when "checkbox_auto_connect_ws" is clicked.
// static void checkbox_auto_connect_ws_event_handler(lv_event_t * e) {
//   lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
//   lv_obj_t * obj = (lv_obj_t*)lv_event_get_target(e);
  
//   if(code == LV_EVENT_VALUE_CHANGED) {
//     LV_UNUSED(obj);
//     //const char * txt = lv_checkbox_get_text(obj);
//     //const char * state = lv_obj_get_state(obj) & LV_STATE_CHECKED ? "Checked" : "Unchecked";
//     //LV_UNUSED(txt);
//     //LV_UNUSED(state);
//     //LV_LOG_USER("%s: %s", txt, state);

//     bool checked = lv_obj_has_state(obj, LV_STATE_CHECKED);

//     Serial.println();
//     Serial.println("checkbox_auto_connect_ws is clicked/touched.");

//     if (checked == true) {
//       Serial.println("Auto-connect : On.");
      
//       //----------------------------------------Save auto-connect status. Auto-connect : On.
//       // Open Preferences with "mySet" namespace.
//       // Note: Namespace name is limited to 15 chars.
//       preferences.begin("mySet", false);
      
//       // Save settings to flash memory.
//       // Note: Key name is limited to 15 chars.
//       preferences.putBool("auto_Connect", true);
      
//       // Get settings stored in flash memory.
//       auto_Connect = preferences.getBool("auto_Connect", false);
      
//       // Close the Preferences.
//       preferences.end();
//       //---------------------------------------- 
//     } else {
//       Serial.println("Auto-connect : Off.");
      
//       //----------------------------------------Save auto-connect status. Auto-connect : Off.
//       // Open Preferences with "mySet" namespace.
//       // Note: Namespace name is limited to 15 chars.
//       preferences.begin("mySet", false);
      
//       // Save settings to flash memory.
//       // Note: Key name is limited to 15 chars.
//       preferences.putBool("auto_Connect", false);
      
//       // Get settings stored in flash memory.
//       auto_Connect = preferences.getBool("auto_Connect", false);
      
//       // Close the Preferences.
//       preferences.end();
//       //---------------------------------------- 
//     }
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ button_cancel_ws_event_handler(lv_event_t * e)
// // Callback that is triggered when "button_cancel_ws" is clicked.
// static void button_cancel_ws_event_handler(lv_event_t * e) {
//   lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  
//   if (code == LV_EVENT_CLICKED) {
//     Serial.println();
//     Serial.println("button_cancel_ws is clicked/touched.");
    
//     lv_obj_add_flag(objects.panel_connect_ws, LV_OBJ_FLAG_HIDDEN);
//     lv_obj_remove_flag(objects.panel_main_ws, LV_OBJ_FLAG_HIDDEN);
//     lv_textarea_set_text(objects.textarea_password_ws, "");
//     update_UI();
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ keyboard_password_ws_event_handler(lv_event_t * e)
// // Callback that is triggered when "keyboard_password_ws" is clicked.
// static void keyboard_password_ws_event_handler(lv_event_t * e) {
//   lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
//   lv_obj_t * kb = (lv_obj_t*)lv_event_get_target(e);
//   uint32_t btn_id = lv_keyboard_get_selected_btn(kb);
//   const char * txt = lv_keyboard_get_btn_text(kb, btn_id);
  
//   if(code == LV_EVENT_VALUE_CHANGED) {
//     LV_UNUSED(kb);

//     // If the "OK" button (the "OK" button is the button with the ✔ symbol) on the keyboard is clicked/touched, then hide the keyboard.
//     if(strcmp(txt, LV_SYMBOL_OK) == 0) {
//       lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
//     }
//   }
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ VOID SETUP()
// void setup() {
//   // put your setup code here, to run once:

//   Serial.begin(115200);
//   Serial.println();
//   delay(3000);

//   Serial.println("Start setup.");

//   Serial.println();
//   Serial.println("ESP32 + TFT LCD Touchscreen ILI9341 240x320 + LVGL + EEZ Studio");
//   Serial.println("Dynamic WiFi Configuration (WiFi Settings)");
//   delay(500);

//   //----------------------------------------Loads settings stored in flash memory.
//   // Open Preferences with "mySet" namespace.
//   // Note: Namespace name is limited to 15 chars.
//   preferences.begin("mySet", false);
//   delay(100);

//   // Get the settings stored in flash memory.
//   // Note: Key name is limited to 15 chars.
//   Wifi_On_Off_Status = preferences.getBool("Wifi_On_Off_Sta", false); //--> Get the "Wifi_On_Off_Status" value, if the key does not exist, return a default value of false.
//   auto_Connect       = preferences.getBool("auto_Connect", false);    //--> Get the "auto_Connect" value, if the key does not exist, return a default value of false.
//   saved_SSID         = preferences.getString("saved_SSID", "");       //--> Get the "saved_SSID" value, if the key does not exist, return a default value of "".
//   saved_Password     = preferences.getString("saved_Password", "");   //--> Get the "saved_Password" value, if the key does not exist, return a default value of "".
//   delay(100);

//   // Close the Preferences.
//   preferences.end();
//   delay(100);

//   Serial.println();
//   Serial.println("-------------Settings stored in flash memory.");
//   Serial.print("Wifi_On_Off_Status : "); 
//   Serial.print(Wifi_On_Off_Status);
//   if (Wifi_On_Off_Status == 1) Serial.println(" (On).");
//   if (Wifi_On_Off_Status == 0) Serial.println(" (Off).");
//   Serial.print("auto_Connect : ");
//   Serial.print(auto_Connect);
//   if (auto_Connect == 1) Serial.println(" (On).");
//   if (auto_Connect == 0) Serial.println(" (Off).");
//   Serial.print("saved_SSID : ");
//   Serial.println(saved_SSID);
//   Serial.print("saved_Password : ");
//   Serial.println(saved_Password);
//   Serial.println("-------------");
//   //---------------------------------------- 

//   get_Selected_SSID = saved_SSID;
//   get_Entered_SSID_Password = saved_Password;

//   String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
//   Serial.println();
//   Serial.println(LVGL_Arduino);
//   delay(500);

//   // Start the SPI for the touchscreen and init the touchscreen.
//   touchscreen.begin();

//   // Set the Touchscreen rotation in landscape mode.
//   // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 0. "touchscreen.setRotation(0);"
//   touchscreen.setRotation(2);

//   //----------------------------------------LVGL setup.
//   Serial.println();
//   Serial.println("Start LVGL Setup...");
//   delay(500);

//   // Start LVGL.
//   lv_init();
  
//   // Register print function for debugging.
//   lv_log_register_print_cb(log_print);

//   // Create a display object.
//   lv_display_t * disp;
  
//   // Initialize the TFT display using the TFT_eSPI library.
//   draw_buf = new uint8_t[DRAW_BUF_SIZE];
//   disp = lv_tft_espi_create(SCREEN_HEIGHT, SCREEN_WIDTH, draw_buf, DRAW_BUF_SIZE);
//   // If the display on the TFT LCD has problems, try the line of code below:
//   //disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, DRAW_BUF_SIZE);
//   lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);
    
//   // Initialize an LVGL input device object (Touchscreen).
//   lv_indev_t * indev = lv_indev_create();
//   lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  
//   // Set the callback function to read Touchscreen input.
//   lv_indev_set_read_cb(indev, touchscreen_read);

//   //my_group = lv_group_create();
//   //lv_indev_set_group(indev, my_group);

//   Serial.println("LVGL Setup Completed.");
//   delay(500);
//   //---------------------------------------- 

//   // Integrate EEZ Studio GUI.
//   ui_init();

//   // Register "image_wifi_indicator_hm" event handler.
//   lv_obj_add_event_cb(objects.image_wifi_indicator_hm, image_wifi_indicator_hm_event_handler, LV_EVENT_ALL, NULL);

//   // Register "button_refresh_ntp_hm" event handler.
//   lv_obj_add_event_cb(objects.button_refresh_ntp_hm, button_refresh_ntp_hm_event_handler, LV_EVENT_ALL, NULL);

//   // Register "button_back_ws" event handler.
//   lv_obj_add_event_cb(objects.button_back_ws, button_back_ws_event_handler, LV_EVENT_ALL, NULL);

//   // Register "switch_ctrl_wifi_ws" event handler.
//   lv_obj_add_event_cb(objects.switch_ctrl_wifi_ws, switch_ctrl_wifi_ws_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

//   // Register "button_clear_auto_connect_ws" event handler.
//   //lv_obj_add_event_cb(objects.button_clear_auto_connect_ws, button_clear_auto_connect_ws_event_handler, LV_EVENT_ALL, NULL);

//   // Register "button_disconnect_ws" event handler.
//   lv_obj_add_event_cb(objects.button_disconnect_ws, button_disconnect_ws_event_handler, LV_EVENT_ALL, NULL);

//   // Register "textarea_password_ws" event handler.
//   lv_obj_add_event_cb(objects.textarea_password_ws, textarea_password_ws_event_handler, LV_EVENT_ALL, objects.keyboard_password_ws);

//   // Register "button_connect_ws" event handler.
//   lv_obj_add_event_cb(objects.button_connect_ws, button_connect_ws_event_handler, LV_EVENT_ALL, NULL);

//   // Register "checkbox_auto_connect_ws" event handler.
//   lv_obj_add_event_cb(objects.checkbox_auto_connect_ws, checkbox_auto_connect_ws_event_handler, LV_EVENT_ALL, NULL);

//   // Register "button_cancel_ws" event handler.
//   lv_obj_add_event_cb(objects.button_cancel_ws, button_cancel_ws_event_handler, LV_EVENT_ALL, NULL);

//   // Register "keyboard_password_ws" event handler.
//   lv_obj_add_event_cb(objects.keyboard_password_ws, keyboard_password_ws_event_handler, LV_EVENT_ALL, NULL);

//   delay(100);

//   //----------------------------------------
//   if (Wifi_On_Off_Status == true) {
//     // Set WiFi to station mode and disconnect from an AP if it was previously connected.
//     Serial.println();
//     Serial.println("Turn on WiFi.");
    
//     WiFi.mode(WIFI_STA);
//     WiFi.disconnect();

//     lv_img_set_src(objects.image_wifi_indicator_hm, &img_img_wifi_on);
//     lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_on);
//     update_UI();
//   } else {
//     Serial.println();
//     Serial.println("Turn off WiFi.");
    
//     WiFi.disconnect();
//     WiFi.mode(WIFI_OFF);
    
//     lv_img_set_src(objects.image_wifi_indicator_hm, &img_img_wifi_off);
//     lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_off);
//     update_UI();
//   }
//   //----------------------------------------

//   //----------------------------------------
//   if (Wifi_On_Off_Status == true && auto_Connect == true) {
//     Serial.println();
//     Serial.println("-------------Connecting To WiFi");
//     Serial.print("Connecting to : ");
//     Serial.println(get_Selected_SSID);

//     lv_label_set_text(objects.label_header_hm, "Connecting. Please wait...");
//     update_UI();

//     delay(1000);

//     WiFi.begin(get_Selected_SSID.c_str(), get_Entered_SSID_Password.c_str());

//     int connecting_process_timed_out = 10; //--> 10 = 10 seconds.
//     connecting_process_timed_out = connecting_process_timed_out * 2;
//     while (WiFi.status() != WL_CONNECTED) {
//       Serial.print(".");
//       delay(500);
      
//       if(connecting_process_timed_out > 0) connecting_process_timed_out--;
//       if(connecting_process_timed_out == 0) {
//         lv_obj_add_flag(objects.image_wifi_indicator_hm, LV_OBJ_FLAG_CLICKABLE);
//         lv_img_set_src(objects.image_wifi_indicator_hm, &img_img_wifi_on);
//         lv_label_set_text(objects.label_header_hm, "Failed to connect to WiFi.");
//         update_UI();
  
//         Serial.println();
//         Serial.println("Failed to connect to WiFi.");
//         Serial.println("-------------");

//         delay(2000);

//         break;
//       }
//     }

//     if (WiFi.status() == WL_CONNECTED) {
//       lv_obj_add_flag(objects.image_wifi_indicator_hm, LV_OBJ_FLAG_CLICKABLE);
//       lv_img_set_src(objects.image_wifi_indicator_ws, &img_img_wifi_connected);
//       lv_img_set_src(objects.image_wifi_indicator_hm, &img_img_wifi_connected);
//       lv_label_set_text(objects.label_header_hm, "Connecting successfully.");
//       update_UI();

//       Serial.println();
//       Serial.println("WiFi connected");
//       Serial.print("Successfully connected to : ");
//       Serial.println(saved_SSID);
//       Serial.println("-------------");

//       delay(3000);
//     }
//   }
//   //----------------------------------------

//   get_TimeDate_from_NTP_server();

//   lv_label_set_text(objects.label_header_hm, "HOME");
//   update_UI();

//   Serial.println();
//   Serial.println("Setup complete.");
// }
// //________________________________________________________________________________ 



// //________________________________________________________________________________ VOID LOOP()
// void loop() {
//   // put your main code here, to run repeatedly:

//   //---------------------------------------- 
//   if (execute_WiFi_Scan == true) wifi_Scan();
//   if (execute_Connecting_To_WiFi == true) connecting_To_WiFi();
  
//   if (execute_Get_TimeDate_from_NTP_server == true) {
//     get_TimeDate_from_NTP_server();
//     execute_Get_TimeDate_from_NTP_server = false;
//   }
//   //---------------------------------------- 

//   //----------------------------------------Timer/millis to update time and date after successfully getting time and date data from NTP Server.
//   unsigned long currentMillis = millis();

//   if (currentMillis - previousMillis >= interval) {
//     previousMillis = currentMillis;

//     if (get_NTP_TimeDate_Sta == true) {
//       struct tm timeinfo;
      
//       if (!getLocalTime(&timeinfo)) {
//         get_NTP_TimeDate_Sta = false;
        
//         lv_label_set_text(objects.label_ntp_info_hm, "Failed to obtain date & time !");
//         lv_label_set_text(objects.label_ntp_time_hm, "NN:NN:NN");
//         lv_label_set_text(objects.label_ntp_date_hm, "NN-NN-NNNN");
//         update_UI();
      
//         Serial.println();
//         Serial.println("Failed to obtain date and time !");
//       } else {
//         t_hour   = timeinfo.tm_hour;
//         t_minute = timeinfo.tm_min;
//         t_second = timeinfo.tm_sec;
      
//         d_day   = timeinfo.tm_mday;
//         d_month = timeinfo.tm_mon+1;
//         d_year  = timeinfo.tm_year+1900;
      
//         char NTP_Date[18];
//         sprintf(NTP_Date, "%02d-%02d-%d", d_day, d_month, d_year);
        
//         char NTP_Time[16];
//         sprintf(NTP_Time, "%02d:%02d:%02d", t_hour, t_minute, t_second);
        
//         //Serial.print("Date : ");Serial.print(NTP_Date);Serial.print(" | Time : ");Serial.println(NTP_Time);

//         lv_label_set_text(objects.label_ntp_time_hm, NTP_Time);
//         lv_label_set_text(objects.label_ntp_date_hm, NTP_Date);
//       }
//     }
//   }
//   //---------------------------------------- 

//   //----------------------------------------
//   update_UI();
//   delay(5);
//   //---------------------------------------- 
// }
// //________________________________________________________________________________ 
// //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<