#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    {
        lv_obj_t *parent_obj = obj;
        {
            // labelNomeMaquina
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_nome_maquina = obj;
            lv_obj_set_pos(obj, 9, 11);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Tear 02");
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 206, 6);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_logo_msb);
            lv_image_set_pivot(obj, 0, 0);
        }
        {
            // AnimationImage
            lv_obj_t *obj = lv_animimg_create(parent_obj);
            objects.animation_image = obj;
            lv_obj_set_pos(obj, 179, 11);
            lv_obj_set_size(obj, 26, 26);
            static const lv_image_dsc_t *images[4] = {
                &img_wifi_icone,
                &img_wifi_icone_3,
                &img_wifi_icone_2,
                &img_wifi_icone_1,
            };
            lv_animimg_set_src(obj, (const void **)images, 4);
            lv_animimg_set_duration(obj, 5000);
            lv_animimg_set_repeat_count(obj, LV_ANIM_REPEAT_INFINITE);
            lv_animimg_start(obj);
        }
        {
            // ledindicador
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.ledindicador = obj;
            lv_obj_set_pos(obj, 150, 17);
            lv_obj_set_size(obj, 20, 20);
            lv_led_set_color(obj, lv_color_hex(0xff0032ff));
            lv_led_set_brightness(obj, 255);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_x(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}
