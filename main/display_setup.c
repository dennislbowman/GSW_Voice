#include "lvgl.h"
lv_obj_t * button_2;
lv_obj_t * button_3;
lv_obj_t * button_4;
lv_obj_t * button_5;
lv_obj_t * button_6;

 lv_obj_t * label_2;

lv_obj_t *label_3;
lv_obj_t *label_1;
lv_obj_t *label_4;
lv_obj_t *label_5;
lv_obj_t *label_6;
lv_obj_t *label_7;
lv_obj_t *label_8;
lv_obj_t * imgdir;

extern void wifi_init_sta();
extern void send_cmd(int cmd);

bool connectf;
static void event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    connectf=true;
}
static void event_cb3(lv_event_t * e)
{
    LV_UNUSED(e);
   send_cmd(1);
}
static void event_cb4(lv_event_t * e)
{
    LV_UNUSED(e);
    send_cmd(12);
}
static void event_cb5(lv_event_t * e)
{
    LV_UNUSED(e);
    send_cmd(11);
}
static void event_cb6(lv_event_t * e)
{
    LV_UNUSED(e);
    send_cmd(0);
}
static void event_cmd_up(lv_event_t * e)
{
   lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    send_cmd(3);
  }
}
static void event_cmd_dn(lv_event_t * e)
{
   lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    send_cmd(4);
  }
}
static void event_cmd_fwd(lv_event_t * e)
{
   lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    send_cmd(7);
  }
}
static void event_cmd_bak(lv_event_t * e)
{
   lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    send_cmd(8);
  }
}
static void event_cmd_left(lv_event_t * e)
{
   lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    send_cmd(6);
  }
}
static void event_cmd_right(lv_event_t * e)
{
   lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    send_cmd(5);
  }
}
static void event_cmd_ylf(lv_event_t * e)
{
   lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    send_cmd(9);
  }
}
static void event_cmd_yrt(lv_event_t * e)
{
   lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    send_cmd(10);
  }
}

void display_setup(void)
{
    /* Scroll mode example */
    static lv_style_t style_label_bg;
    lv_obj_t *screen1 = lv_screen_active();
  

//        lv_obj_t * tabview = lv_tabview_create(screen);
//    lv_obj_set_size(tabview, lv_pct(100), lv_pct(100));
//    lv_tabview_set_tab_bar_position(tabview, LV_DIR_BOTTOM);
//    lv_obj_t * lv_tabview_tab_0 = lv_tabview_add_tab(tabview, "STAT");
//    lv_obj_t * lv_tabview_tab_1 = lv_tabview_add_tab(tabview, "CTL");
//    lv_obj_t * lv_tabview_tab_2 = lv_tabview_add_tab(tabview, "MOV");

    lv_style_init(&style_label_bg);
    label_3 = lv_label_create(screen1);
    lv_obj_set_width(label_3, 300);
    lv_obj_set_align(label_3, LV_ALIGN_TOP_MID);
    lv_label_set_long_mode(label_3, LV_LABEL_LONG_MODE_SCROLL);
    lv_obj_set_style_text_align(label_3, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_bg_color(label_3, lv_color_hex(0xfdebd0), 0);
    lv_label_set_text(label_3, "-");
    lv_obj_add_style(label_3, &style_label_bg, 0);
    
    label_1 = lv_label_create(screen1);
    lv_obj_set_width(label_1, 300);
//    lv_obj_set_align(label_1, LV_ALIGN_TOP_MID);
    lv_obj_align(label_1, LV_ALIGN_TOP_MID, 0, 110);
    lv_label_set_long_mode(label_1, LV_LABEL_LONG_MODE_SCROLL);
    lv_obj_set_style_text_align(label_1, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label_1, "N/A");

    label_4 = lv_label_create(screen1);
    lv_obj_set_width(label_4, 300);
//    lv_obj_set_align(label_4, LV_ALIGN_TOP_MID);
    lv_obj_align(label_4, LV_ALIGN_TOP_MID, 0, 150);
    lv_label_set_long_mode(label_4, LV_LABEL_LONG_MODE_SCROLL);
    lv_obj_set_style_text_align(label_4, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label_4, "N/A");

    label_5 = lv_label_create(screen1);
    lv_obj_set_width(label_5, 300);
    lv_obj_align(label_5, LV_ALIGN_TOP_MID, 0, 190);
    lv_label_set_long_mode(label_5, LV_LABEL_LONG_MODE_SCROLL);
    lv_obj_set_style_text_align(label_5, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label_5, "N/A");

    label_6 = lv_label_create(screen1);
    lv_obj_set_width(label_6, 300);
    lv_obj_align(label_6, LV_ALIGN_TOP_MID, 0, 230);
    lv_label_set_long_mode(label_6, LV_LABEL_LONG_MODE_SCROLL);
    lv_obj_set_style_text_align(label_6, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label_6, "N/A");

    label_7 = lv_label_create(screen1);
    lv_obj_set_width(label_7, 300);
    lv_obj_set_align(label_7, LV_ALIGN_BOTTOM_MID);
//    lv_obj_align(label_7, LV_ALIGN_TOP_MID, 0, 270);
    lv_label_set_long_mode(label_7, LV_LABEL_LONG_MODE_SCROLL);
    lv_obj_set_style_text_align(label_7, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label_7, "N/A");

    label_8 = lv_label_create(screen1);
    lv_obj_set_width(label_8, 200);
//    lv_obj_set_align(label_8, LV_ALIGN_BOTTOM_MID);
    lv_obj_align(label_8, LV_ALIGN_BOTTOM_MID, 0, -50);
//    lv_label_set_long_mode(label_8, LV_LABEL_LONG_MODE_SCROLL);
    lv_obj_set_style_text_align(label_8, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label_8, "N/A");

    button_2 = lv_button_create(screen1);
    lv_obj_set_size(button_2, 300, 60);
    lv_obj_align(button_2, LV_ALIGN_TOP_MID, 0, 50);
    //lv_obj_set_align(button_2, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(button_2, event_cb, LV_EVENT_CLICKED, NULL);
    label_2 = lv_label_create(button_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Connect");

    button_3 = lv_button_create(screen1);
    lv_obj_set_size(button_3, 150, 60);
    lv_obj_align(button_3, LV_ALIGN_CENTER, 100, 270);
    //lv_obj_set_align(button_3, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(button_3, event_cb3, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_3b = lv_label_create(button_3);
    lv_obj_set_align(label_3b, LV_ALIGN_CENTER);
    lv_label_set_text(label_3b, "Take");

    button_4 = lv_button_create(screen1);
    lv_obj_set_size(button_4, 150, 60);
    lv_obj_align(button_4, LV_ALIGN_CENTER, -100, 270);
    //lv_obj_set_align(button_4, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(button_4, event_cb4, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_4b = lv_label_create(button_4);
    lv_obj_set_align(label_4b, LV_ALIGN_CENTER);
    lv_label_set_text(label_4b, "Land");

    button_5 = lv_button_create(screen1);
    lv_obj_set_size(button_5, 150, 60);
    lv_obj_align(button_5, LV_ALIGN_CENTER, -100, 370);
    //lv_obj_set_align(button_5, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(button_5, event_cb5, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_5b = lv_label_create(button_5);
    lv_obj_set_align(label_5b, LV_ALIGN_CENTER);
    lv_label_set_text(label_5b, "RTL");

    button_6 = lv_button_create(screen1);
    lv_obj_set_size(button_6, 150, 60);
    lv_obj_align(button_6, LV_ALIGN_CENTER, 100, 370);
    //lv_obj_set_align(button_6, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(button_6, event_cb6, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_6b = lv_label_create(button_6);
    lv_obj_set_align(label_6b, LV_ALIGN_CENTER);
    lv_label_set_text(label_6b, "ARM");

lv_scr_load(screen1);




LV_IMG_DECLARE(ArrowLeft);
LV_IMG_DECLARE(Arrowright);
LV_IMG_DECLARE(Arrowup);
LV_IMG_DECLARE(Arrowdn);
LV_IMG_DECLARE(Arrowfwd);
LV_IMG_DECLARE(Arrowback);
LV_IMG_DECLARE(ArrowYleft);
LV_IMG_DECLARE(ArrowYright);
LV_IMG_DECLARE(ArrowDir);

 /*Darken the button when pressed and make it wider*/
  static lv_style_t style_pr;
  lv_style_init(&style_pr);
  lv_style_set_img_recolor_opa(&style_pr, LV_OPA_30);
  lv_style_set_img_recolor(&style_pr, lv_color_black());
  lv_style_set_transform_width(&style_pr, 20);

  lv_obj_t *up_bt = lv_imgbtn_create(screen1);
  lv_imgbtn_set_src(up_bt, LV_IMGBTN_STATE_RELEASED, &Arrowup, NULL, NULL);
  lv_obj_set_size(up_bt, 75, 75);
  lv_obj_add_event_cb(up_bt, event_cmd_up, LV_EVENT_ALL, NULL);
  lv_obj_align(up_bt, LV_ALIGN_BOTTOM_RIGHT, 125, -150);
  lv_obj_add_style(up_bt, &style_pr, LV_STATE_PRESSED);

  lv_obj_t *dn_bt = lv_imgbtn_create(screen1);
  lv_imgbtn_set_src(dn_bt, LV_IMGBTN_STATE_RELEASED, &Arrowdn, NULL, NULL);
  lv_obj_set_size(dn_bt, 75, 75);
  lv_obj_add_event_cb(dn_bt, event_cmd_dn, LV_EVENT_ALL, NULL);
  lv_obj_align(dn_bt, LV_ALIGN_BOTTOM_RIGHT, 125, 0);
  lv_obj_add_style(dn_bt, &style_pr, LV_STATE_PRESSED);

  lv_obj_t *rt_bt = lv_imgbtn_create(screen1);
  lv_imgbtn_set_src(rt_bt, LV_IMGBTN_STATE_RELEASED, &Arrowright, NULL, NULL);
  lv_obj_set_size(rt_bt, 75, 75);
  lv_obj_add_event_cb(rt_bt, event_cmd_right, LV_EVENT_ALL, NULL);
  lv_obj_align(rt_bt, LV_ALIGN_TOP_RIGHT, 200, 75);
  lv_obj_add_style(rt_bt, &style_pr, LV_STATE_PRESSED);

  lv_obj_t *lf_bt = lv_imgbtn_create(screen1);
  lv_imgbtn_set_src(lf_bt, LV_IMGBTN_STATE_RELEASED, &ArrowLeft, NULL, NULL);
  lv_obj_set_size(lf_bt, 75, 75);
  lv_obj_add_event_cb(lf_bt, event_cmd_left, LV_EVENT_ALL, NULL);
  lv_obj_align(lf_bt, LV_ALIGN_TOP_RIGHT, 50, 75);
  lv_obj_add_style(lf_bt, &style_pr, LV_STATE_PRESSED);

  lv_obj_t *fw_bt = lv_imgbtn_create(screen1);
  lv_imgbtn_set_src(fw_bt, LV_IMGBTN_STATE_RELEASED, &Arrowfwd, NULL, NULL);
  lv_obj_set_size(fw_bt, 75, 75);
  lv_obj_add_event_cb(fw_bt, event_cmd_fwd, LV_EVENT_ALL, NULL);
  lv_obj_align(fw_bt, LV_ALIGN_TOP_RIGHT, 125, 0);
  lv_obj_add_style(fw_bt, &style_pr, LV_STATE_PRESSED);

  lv_obj_t *bk_bt = lv_imgbtn_create(screen1);
  lv_imgbtn_set_src(bk_bt, LV_IMGBTN_STATE_RELEASED, &Arrowback, NULL, NULL);
  lv_obj_set_size(bk_bt, 75, 75);
  lv_obj_add_event_cb(bk_bt, event_cmd_bak, LV_EVENT_ALL, NULL);
  lv_obj_align(bk_bt, LV_ALIGN_TOP_RIGHT, 125, 150);
  lv_obj_add_style(bk_bt, &style_pr, LV_STATE_PRESSED);


  lv_obj_t *yrt_bt = lv_imgbtn_create(screen1);
  lv_imgbtn_set_src(yrt_bt, LV_IMGBTN_STATE_RELEASED, &ArrowYright, NULL, NULL);
  lv_obj_set_size(yrt_bt, 75, 75);
  lv_obj_add_event_cb(yrt_bt, event_cmd_yrt, LV_EVENT_ALL, NULL);
  lv_obj_align(yrt_bt, LV_ALIGN_BOTTOM_RIGHT, 200, -75);
  lv_obj_add_style(yrt_bt, &style_pr, LV_STATE_PRESSED);

  lv_obj_t *ylf_bt = lv_imgbtn_create(screen1);
  lv_imgbtn_set_src(ylf_bt, LV_IMGBTN_STATE_RELEASED, &ArrowYleft, NULL, NULL);
  lv_obj_set_size(ylf_bt, 75, 75);
  lv_obj_add_event_cb(ylf_bt, event_cmd_ylf, LV_EVENT_ALL, NULL);
  lv_obj_align(ylf_bt, LV_ALIGN_BOTTOM_RIGHT, 50, -75);
  lv_obj_add_style(ylf_bt, &style_pr, LV_STATE_PRESSED);

    imgdir = lv_image_create(screen1);
    lv_image_set_src(imgdir, &ArrowDir);
    lv_obj_align(imgdir, LV_ALIGN_BOTTOM_MID, 0, -100);
    lv_image_set_rotation(imgdir, 450);

}
