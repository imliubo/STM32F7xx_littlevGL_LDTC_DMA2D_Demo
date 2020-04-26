#include "test.h"
#include "stdio.h"
#include "main.h"
#include "../lvgl/lvgl.h"
#include "../lv_conf.h"


static void obj_test(void)
{
	  lv_obj_t * obj1;
    obj1 = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj1, 200, 100);
    lv_obj_set_style(obj1, &lv_style_plain_color);
    lv_obj_align(obj1, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);

    lv_obj_t * obj2;
    obj2 = lv_obj_create(lv_scr_act(), obj1);
    lv_obj_set_style(obj2, &lv_style_pretty_color);
    lv_obj_align(obj2, NULL, LV_ALIGN_CENTER, 0, -100);

    static lv_style_t style_shadow;
    lv_style_copy(&style_shadow, &lv_style_pretty);
    style_shadow.body.shadow.width = 6;
    style_shadow.body.radius = LV_RADIUS_CIRCLE;
    
    lv_obj_t * obj3;
    obj3 = lv_obj_create(lv_scr_act(), obj2);
    lv_obj_set_style(obj3, &style_shadow);
    lv_obj_set_pos(obj3,400,300);
}

static void arc1_test(void){
    /*Create style for the Arcs*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.line.color = LV_COLOR_BLUE;           /*Arc color*/
    style.line.width = 15;                       /*Arc width*/

    /*Create an Arc*/
    lv_obj_t * arc = lv_arc_create(lv_scr_act(), NULL);
    lv_arc_set_style(arc, LV_ARC_STYLE_MAIN, &style);          /*Use the new style*/
		lv_obj_set_size(arc, 200, 200);  
		lv_arc_set_angles(arc, 90, 60);
    
    lv_obj_align(arc, NULL, LV_ALIGN_CENTER, -200, 0);
}

/**
 * An `lv_task` to call periodically to set the angles of the arc
 * @param t
 */
static void arc_loader(lv_task_t * t)
{
    static int16_t a = 0;

    a+=5;
    if(a >= 359) a = 359;

    if(a < 180) lv_arc_set_angles(t->user_data, 180-a ,180);
    else lv_arc_set_angles(t->user_data, 540-a ,180);

    if(a == 359) {
        lv_task_del(t);
        return;
    }
}

/**
 * Create an arc which acts as a loader.
 */
static void arc2_test(void)
{
  /*Crarc_test2eate style for the Arcs*/
  static lv_style_t style;
  lv_style_copy(&style, &lv_style_plain);
  style.line.color = LV_COLOR_NAVY;           /*Arc color*/
  style.line.width = 15;                      /*Arc width*/

  /*Create an Arc*/
  lv_obj_t * arc = lv_arc_create(lv_scr_act(), NULL);
	lv_obj_set_size(arc, 200, 200);
  lv_arc_set_angles(arc, 180, 180);
  lv_arc_set_style(arc, LV_ARC_STYLE_MAIN, &style);
  lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 200, 0);

  /* Create an `lv_task` to update the arc.
   * Store the `arc` in the user data*/
  lv_task_create(arc_loader, 20, LV_TASK_PRIO_LOWEST, arc);

}

static void bar_test(void)
{
    lv_obj_t * bar1 = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_size(bar1, 350, 40);
    lv_obj_align(bar1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_anim_time(bar1, 2000);
    lv_bar_set_value(bar1, 100, LV_ANIM_ON);
}

static lv_obj_t * btn1;
static lv_obj_t * btn2;
static lv_obj_t * btn_label;
static void btn_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (obj == btn1) {
        if(event == LV_EVENT_CLICKED) {
            printf("Btn1 Clicked\n");
            lv_label_set_text(btn_label,"Btn1 Clicked");
        }
    }else if(obj == btn2){
        if(event == LV_EVENT_VALUE_CHANGED) {
            printf("Btn2 Toggled\n");
            lv_label_set_text(btn_label,"Btn2 Toggled");
        }
    }
}

static void btn_test(void)
{
    lv_obj_t * label;

    btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn1, btn_event_handler);
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button");

    btn2 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn2, btn_event_handler);
    lv_obj_align(btn2, NULL, LV_ALIGN_CENTER, 0, 40);
    lv_btn_set_toggle(btn2, true);
    lv_btn_toggle(btn2);
    lv_btn_set_fit2(btn2, LV_FIT_NONE, LV_FIT_TIGHT);

    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Toggled");
    
    btn_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(btn_label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(btn_label, 180);
    lv_label_set_text(btn_label,"Watting BTN for clicked!");
    lv_obj_align(btn_label, NULL, LV_ALIGN_CENTER, 0, 90);
    lv_label_set_align(btn_label, LV_LABEL_ALIGN_CENTER);
}

static void btm_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        const char * txt = lv_btnm_get_active_btn_text(obj);

        printf("%s was pressed\n", txt);
    }
}


static const char * btnm_map[] = {"1", "2", "3", "4", "5", "\n",
                                  "6", "7", "8", "9", "0", "\n",
                                  "Action1", "Action2", ""};

static void btnm_test(void)
{
    lv_obj_t * btnm1 = lv_btnm_create(lv_scr_act(), NULL);
    lv_btnm_set_map(btnm1, btnm_map);
    lv_btnm_set_btn_width(btnm1, 10, 2);        /*Make "Action1" twice as wide as "Action2"*/
    lv_obj_align(btnm1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btnm1, btm_event_handler);
}

static void calender_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        lv_calendar_date_t * date = lv_calendar_get_pressed_date(obj);
        if(date) {
            lv_calendar_set_today_date(obj, date);
        }
    }
}

static void calendar_test(void)
{
    lv_obj_t  * calendar = lv_calendar_create(lv_scr_act(), NULL);
    lv_obj_set_size(calendar, 460, 460);
    lv_obj_align(calendar, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(calendar, calender_event_handler);

    /*Set the today*/
    lv_calendar_date_t today;
    today.year = 2020;
    today.month = 3;
    today.day = 18;

    lv_calendar_set_today_date(calendar, &today);
    lv_calendar_set_showed_date(calendar, &today);

    /*Highlight some days*/
    static lv_calendar_date_t highlihted_days[1];       /*Only it's pointer will be saved so should be static*/
    highlihted_days[0].year = 2020;
    highlihted_days[0].month = 3;
    highlihted_days[0].day = 28;

    lv_calendar_set_highlighted_dates(calendar, highlihted_days, 1);
}

#define CANVAS_WIDTH   100
#define CANVAS_HEIGHT  100

void canvas_test(void)
{
	  static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.body.main_color = LV_COLOR_RED;
    style.body.grad_color = LV_COLOR_MAROON;
    style.body.radius = 4;
    style.body.border.width = 2;
    style.body.border.color = LV_COLOR_WHITE;
    style.body.shadow.color = LV_COLOR_WHITE;
    style.body.shadow.width = 4;
    style.line.width = 2;
    style.line.color = LV_COLOR_BLACK;
    style.text.color = LV_COLOR_BLUE;
	
    /*Create a button to better see the transparency*/
//		lv_obj_t * button = lv_btn_create(lv_scr_act(), NULL);
//		lv_obj_align(button, NULL, LV_ALIGN_CENTER, 0, 0);
    /*Create a buffer for the canvas*/
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_4BIT(CANVAS_WIDTH, CANVAS_HEIGHT)];

    /*Create a canvas and initialize its the palette*/
    lv_obj_t * canvas = lv_canvas_create(lv_scr_act(), NULL);
		lv_obj_align(canvas, NULL, LV_ALIGN_CENTER, -25, -25);
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_INDEXED_4BIT);
    lv_canvas_set_palette(canvas, 0, LV_COLOR_TRANSP);
    lv_canvas_set_palette(canvas, 1, LV_COLOR_RED);
		
    /*Create colors with the indices of the palette*/
    lv_color_t c0;
    lv_color_t c1;

    c0.full = 0;
    c1.full = 1;

    /*Transparent background*/
    lv_canvas_fill_bg(canvas, c1);

    /*Create hole on the canvas*/
    uint32_t x;
    uint32_t y;
    for( y = 20; y < 35; y++) {
        for( x = 20; x < 35; x++) {
            lv_canvas_set_px(canvas, x, y, c0);
        }
    }
		lv_canvas_draw_arc(canvas, 5, 5, 10, 0, 360, &style);
}

static void cb_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        printf("State: %s\n", lv_cb_is_checked(obj) ? "Checked" : "Unchecked");
    }
}

static void cb_test(void)
{
    lv_obj_t * cb = lv_cb_create(lv_scr_act(), NULL);
    lv_cb_set_text(cb, "I agree to terms and conditions.");
    lv_obj_align(cb, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(cb, cb_event_handler);
}


static void label_test(void)
{
    lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_align(label1, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
		
    lv_label_set_text(label1, "#000080 Re-color# #0000ff words# #6666ff of a# label "
                              "and  wrap long text automatically.");
    lv_obj_set_width(label1, 300);
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, -30);

    lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SROLL_CIRC);     /*Circular scroll*/
    lv_obj_set_width(label2, 300);
    lv_label_set_text(label2, "It is a circularly scrolling text. It is a circularly scrolling text.It is a circularly scrolling text.");
    lv_obj_align(label2, NULL, LV_ALIGN_CENTER, 0, 30);
}

static void led_test(void)
{
    /*Create a style for the LED*/
    static lv_style_t style_led;
    lv_style_copy(&style_led, &lv_style_pretty_color);
    style_led.body.radius = LV_RADIUS_CIRCLE;
    style_led.body.main_color = LV_COLOR_MAKE(0xb5, 0x0f, 0x04);
    style_led.body.grad_color = LV_COLOR_MAKE(0x50, 0x07, 0x02);
    style_led.body.border.color = LV_COLOR_MAKE(0xfa, 0x0f, 0x00);
    style_led.body.border.width = 3;
    style_led.body.border.opa = LV_OPA_30;
    style_led.body.shadow.color = LV_COLOR_MAKE(0xb5, 0x0f, 0x04);
    style_led.body.shadow.width = 5;

    /*Create a LED and switch it OFF*/
    lv_obj_t * led1  = lv_led_create(lv_scr_act(), NULL);
    lv_led_set_style(led1, LV_LED_STYLE_MAIN, &style_led);
    lv_obj_align(led1, NULL, LV_ALIGN_CENTER, -400, 0);
    lv_led_off(led1);

    /*Copy the previous LED and set a brightness*/
    lv_obj_t * led2  = lv_led_create(lv_scr_act(), led1);
    lv_obj_align(led2, NULL, LV_ALIGN_CENTER, -320, 0);
    lv_led_set_bright(led2, 190);

    /*Copy the previous LED and switch it ON*/
    lv_obj_t * led3  = lv_led_create(lv_scr_act(), led1);
    lv_obj_align(led3, NULL, LV_ALIGN_CENTER, -240, 0);
    lv_led_on(led3);
}

static void table_test(void)
{
    /*Create a normal cell style*/
    static lv_style_t style_cell1;
    lv_style_copy(&style_cell1, &lv_style_plain);
    style_cell1.body.border.width = 1;
    style_cell1.body.border.color = LV_COLOR_BLACK;

    /*Crealte a header cell style*/
    static lv_style_t style_cell2;
    lv_style_copy(&style_cell2, &lv_style_plain);
    style_cell2.body.border.width = 1;
    style_cell2.body.border.color = LV_COLOR_BLACK;
    style_cell2.body.main_color = LV_COLOR_SILVER;
    style_cell2.body.grad_color = LV_COLOR_SILVER;

    lv_obj_t * table = lv_table_create(lv_scr_act(), NULL);
    lv_table_set_style(table, LV_TABLE_STYLE_CELL1, &style_cell1);
    lv_table_set_style(table, LV_TABLE_STYLE_CELL2, &style_cell2);
    lv_table_set_style(table, LV_TABLE_STYLE_BG, &lv_style_transp_tight);
    lv_table_set_col_cnt(table, 2);
    lv_table_set_row_cnt(table, 4);
    lv_obj_align(table, NULL, LV_ALIGN_CENTER, -300, 0);

    /*Make the cells of the first row center aligned */
    lv_table_set_cell_align(table, 0, 0, LV_LABEL_ALIGN_CENTER);
    lv_table_set_cell_align(table, 0, 1, LV_LABEL_ALIGN_CENTER);

    /*Make the cells of the first row TYPE = 2 (use `style_cell2`) */
    lv_table_set_cell_type(table, 0, 0, LV_TABLE_STYLE_CELL2);
    lv_table_set_cell_type(table, 0, 1, LV_TABLE_STYLE_CELL2);

    /*Fill the first column*/
    lv_table_set_cell_value(table, 0, 0, "Name");
    lv_table_set_cell_value(table, 1, 0, "Apple");
    lv_table_set_cell_value(table, 2, 0, "Banana");
    lv_table_set_cell_value(table, 3, 0, "Citron");

    /*Fill the second column*/
    lv_table_set_cell_value(table, 0, 1, "Price");
    lv_table_set_cell_value(table, 1, 1, "$7");
    lv_table_set_cell_value(table, 2, 1, "$4");
    lv_table_set_cell_value(table, 3, 1, "$6");
}

static void win_test(void)
{
    /*Create a window*/
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "Window title");                        /*Set the title*/

    /*Add control button to the header*/
    lv_obj_t * close_btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);           /*Add close button and use built-in close action*/
    lv_obj_set_event_cb(close_btn, lv_win_close_event_cb);
    lv_win_add_btn(win, LV_SYMBOL_SETTINGS);        /*Add a setup button*/

    /*Add some dummy content*/
    lv_obj_t * txt = lv_label_create(win, NULL);
    lv_label_set_text(txt, "This is the content of the window\n\n"
                           "You can add control buttons to\n"
                           "the window header\n\n"
                           "The content area becomes automatically\n"
                           "scrollable is it's large enough.\n\n"
                           " You can scroll the content\n"
                           "See the scroll bar on the right!");
}

static void sw_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        printf("State: %s\n", lv_sw_get_state(obj) ? "On" : "Off");
    }
}

static void sw_test(void)
{
    /*Create styles for the switch*/
    static lv_style_t bg_style;
    static lv_style_t indic_style;
    static lv_style_t knob_on_style;
    static lv_style_t knob_off_style;

    lv_style_copy(&bg_style, &lv_style_pretty);
    bg_style.body.radius = LV_RADIUS_CIRCLE;
    bg_style.body.padding.top = 6;
    bg_style.body.padding.bottom = 6;

    lv_style_copy(&indic_style, &lv_style_pretty_color);
    indic_style.body.radius = LV_RADIUS_CIRCLE;
    indic_style.body.main_color = LV_COLOR_BLUE;
    indic_style.body.grad_color = LV_COLOR_NAVY;
    indic_style.body.padding.left = 0;
    indic_style.body.padding.right = 0;
    indic_style.body.padding.top = 0;
    indic_style.body.padding.bottom = 0;

    lv_style_copy(&knob_off_style, &lv_style_pretty);
    knob_off_style.body.radius = LV_RADIUS_CIRCLE;
    knob_off_style.body.shadow.width = 4;
    knob_off_style.body.shadow.type = LV_SHADOW_BOTTOM;

    lv_style_copy(&knob_on_style, &lv_style_pretty_color);
    knob_on_style.body.radius = LV_RADIUS_CIRCLE;
    knob_on_style.body.shadow.width = 4;
    knob_on_style.body.shadow.type = LV_SHADOW_BOTTOM;

    /*Create a switch and apply the styles*/
    lv_obj_t *sw1 = lv_sw_create(lv_scr_act(), NULL);
    lv_sw_set_style(sw1, LV_SW_STYLE_BG, &bg_style);
    lv_sw_set_style(sw1, LV_SW_STYLE_INDIC, &indic_style);
    lv_sw_set_style(sw1, LV_SW_STYLE_KNOB_ON, &knob_on_style);
    lv_sw_set_style(sw1, LV_SW_STYLE_KNOB_OFF, &knob_off_style);
    lv_obj_align(sw1, NULL, LV_ALIGN_CENTER, -400, -50);
    lv_obj_set_event_cb(sw1, sw_event_handler);

    /*Copy the first switch and turn it ON*/
    lv_obj_t *sw2 = lv_sw_create(lv_scr_act(), sw1);
    lv_sw_on(sw2, LV_ANIM_ON);
    lv_obj_align(sw2, NULL, LV_ALIGN_CENTER, -400, 50);
}

static void roller_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        printf("Selected month: %s\n", buf);
    }
}


static void roller_test(void)
{
    lv_obj_t *roller1 = lv_roller_create(lv_scr_act(), NULL);
    lv_roller_set_options(roller1,
                        "January\n"
                        "February\n"
                        "March\n"
                        "April\n"
                        "May\n"
                        "June\n"
                        "July\n"
                        "August\n"
                        "September\n"
                        "October\n"
                        "November\n"
                        "December",
                        LV_ROLLER_MODE_INIFINITE);

    lv_roller_set_visible_row_count(roller1, 4);
    lv_obj_align(roller1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(roller1, roller_event_handler);
}


static lv_obj_t * slider_label;
static void slider_event_cb(lv_obj_t * slider, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
        snprintf(buf, 4, "%u", lv_slider_get_value(slider));
        lv_label_set_text(slider_label, buf);
    }
}

static void slider_test(void)
{
    /* Create a slider in the center of the display */
    lv_obj_t * slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_width(slider, 200);
    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(slider, slider_event_cb);
    lv_slider_set_range(slider, 0, 100);

    /* Create a label below the slider */
    slider_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(slider_label, "0");
    lv_obj_set_auto_realign(slider_label, true);
    lv_obj_align(slider_label, slider, LV_ALIGN_CENTER, 0, 30);

    /* Create an informative label */
    lv_obj_t * info = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(info, "Welcome to the slider+label demo!\n"
                            "Move the slider and see that the label\n"
                            "updates to match it.");
    lv_obj_align(info, NULL, LV_ALIGN_CENTER, 0, -50);
}

void cont_test(void)
{
    lv_obj_t * cont1;
		lv_obj_t * cont2;
		lv_obj_t * cont3;
	
    cont1 = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(cont1, true);                    /*Auto realign when the size changes*/
    lv_obj_align_origo(cont1, NULL, LV_ALIGN_CENTER, 150, 0);  /*This parametrs will be sued when realigned*/
    lv_cont_set_fit(cont1, LV_FIT_TIGHT);
    lv_cont_set_layout(cont1, LV_LAYOUT_COL_M);
    lv_obj_t * label;
    label = lv_label_create(cont1, NULL);
    lv_label_set_text(label, "Short text");
	
	  cont2 = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(cont2, true);                    /*Auto realign when the size changes*/
    lv_obj_align_origo(cont2, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
    lv_cont_set_fit(cont2, LV_FIT_TIGHT);
    lv_cont_set_layout(cont2, LV_LAYOUT_COL_M);
		HAL_Delay(1000);
    label = lv_label_create(cont2, NULL);
    lv_label_set_text(label, "It is a long text");
		
		cont3 = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_auto_realign(cont3, true);                    /*Auto realign when the size changes*/
    lv_obj_align_origo(cont3, NULL, LV_ALIGN_CENTER, -200, 0);  /*This parametrs will be sued when realigned*/
    lv_cont_set_fit(cont3, LV_FIT_TIGHT);
    lv_cont_set_layout(cont3, LV_LAYOUT_COL_M);
		HAL_Delay(1000);
    label = lv_label_create(cont3, NULL);
    lv_label_set_text(label, "Here is an even longer text");
}

static void spinbox_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        printf("Value: %d\n", lv_spinbox_get_value(obj));
    }
    else if(event == LV_EVENT_CLICKED) {
        /*For simple test: Click the spinbox to increment its value*/
        lv_spinbox_increment(obj);
    }
}

static void spinbox_test(void)
{
    lv_obj_t * spinbox;
    spinbox = lv_spinbox_create(lv_scr_act(), NULL);  
    lv_spinbox_set_digit_format(spinbox, 5, 3);
    lv_spinbox_set_step(spinbox, 10);
    lv_spinbox_set_range(spinbox, 0, 100);
    lv_obj_align(spinbox, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(spinbox, spinbox_event_handler);
}

static void ddlist_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_ddlist_get_selected_str(obj, buf, sizeof(buf));
        printf("Option: %s\n", buf);
    }
}

static void ddlist_test1(void)
{
    /*Create a drop down list*/
    lv_obj_t * ddlist = lv_ddlist_create(lv_scr_act(), NULL);
    lv_ddlist_set_options(ddlist, "Apple\n"
            "Banana\n"
            "Orange\n"
            "Melon\n"
            "Grape\n"
            "Raspberry");

    lv_ddlist_set_fix_width(ddlist, 150);
    lv_ddlist_set_draw_arrow(ddlist, true);
    lv_obj_align(ddlist, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
    lv_obj_set_event_cb(ddlist, ddlist_event_handler);
}

static void ddlist_test2(void)
{

    /*Create a drop down list*/
    lv_obj_t * ddlist = lv_ddlist_create(lv_scr_act(), NULL);
    lv_ddlist_set_options(ddlist, "Apple\n"
            "Banana\n"
            "Orange\n"
            "Melon\n"
            "Grape\n"
            "Raspberry");

    lv_ddlist_set_fix_width(ddlist, 150);
    lv_ddlist_set_fix_height(ddlist, 150);
    lv_ddlist_set_draw_arrow(ddlist, true);

    /* Enable auto-realign when the size changes.
     * It will keep the bottom of the ddlist fixed*/
    lv_obj_set_auto_realign(ddlist, true);

    /*It will be called automatically when the size changes*/
    lv_obj_align(ddlist, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);
}

static lv_obj_t * mbox1;
static void mbox_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        printf("Button: %s\n", lv_mbox_get_active_btn_text(obj));
        lv_mbox_start_auto_close(mbox1, 10);
    }
}

static void mbox_test(void)
{
    static const char * btns[] ={"Apply", "Close", ""};

    mbox1 = lv_mbox_create(lv_scr_act(), NULL);
    lv_mbox_set_text(mbox1, "A message box with two buttons.");
    lv_mbox_add_btns(mbox1, btns);
    lv_obj_set_width(mbox1, 200);
    lv_obj_set_event_cb(mbox1, mbox_event_handler);
    lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
}


static void chart_test(void)
{
    /*Create chart1*/
    lv_obj_t * chart1;
    chart1 = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart1, 200, 150);
    lv_obj_align(chart1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart1, LV_CHART_TYPE_AREA );   /*Show lines and points too*/
    lv_chart_set_series_opa(chart1, LV_OPA_70);       /*Opacity of the data series*/
    lv_chart_set_series_width(chart1, 4);            /*Line width and point radious*/
	  lv_chart_set_range(chart1, 0, 100);
	
		/*Create chart2*/
	  lv_obj_t * chart2;
    chart2 = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart2, 200, 150);
    lv_obj_align(chart2, NULL, LV_ALIGN_CENTER, -250, 0);
    lv_chart_set_type(chart2, LV_CHART_TYPE_LINE );   /*Show lines and points too*/
    lv_chart_set_series_opa(chart2, LV_OPA_70);       /*Opacity of the data series*/
    lv_chart_set_series_width(chart2, 4);            /*Line width and point radious*/
		lv_chart_set_range(chart2, 0, 100);
	
		/*Create chart3*/
		lv_obj_t * chart3;
    chart3 = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart3, 200, 150);
    lv_obj_align(chart3, NULL, LV_ALIGN_CENTER, 250, 0);
    lv_chart_set_type(chart3, LV_CHART_TYPE_POINT );   /*Show lines and points too*/
    lv_chart_set_series_opa(chart3, LV_OPA_70);       /*Opacity of the data series*/
    lv_chart_set_series_width(chart3, 4);            /*Line width and point radious*/
		lv_chart_set_range(chart3, 0, 100);
	
    /*Add two data series*/
    lv_chart_series_t * ser1 = lv_chart_add_series(chart1, LV_COLOR_RED);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart1, LV_COLOR_GREEN);

    /*Set the next points on 'dl1'*/
    lv_chart_set_next(chart1, ser1, 10);
    lv_chart_set_next(chart1, ser1, 10);
    lv_chart_set_next(chart1, ser1, 10);
    lv_chart_set_next(chart1, ser1, 10);
    lv_chart_set_next(chart1, ser1, 10);
    lv_chart_set_next(chart1, ser1, 10);
    lv_chart_set_next(chart1, ser1, 10);
    lv_chart_set_next(chart1, ser1, 30);
    lv_chart_set_next(chart1, ser1, 70);
    lv_chart_set_next(chart1, ser1, 90);

    /*Directly set points on 'dl2'*/
    ser2->points[0] = 90;
    ser2->points[1] = 70;
    ser2->points[2] = 65;
    ser2->points[3] = 65;
    ser2->points[4] = 65;
    ser2->points[5] = 65;
    ser2->points[6] = 65;
    ser2->points[7] = 65;
    ser2->points[8] = 65;
    ser2->points[9] = 65;

    lv_chart_refresh(chart1); /*Required after direct set*/
		
		    /*Add two data series*/
    lv_chart_series_t * ser3 = lv_chart_add_series(chart2, LV_COLOR_RED);
    lv_chart_series_t * ser4 = lv_chart_add_series(chart2, LV_COLOR_GREEN);

    /*Set the next points on 'dl1'*/
    lv_chart_set_next(chart2, ser3, 10);
    lv_chart_set_next(chart2, ser3, 10);
    lv_chart_set_next(chart2, ser3, 10);
    lv_chart_set_next(chart2, ser3, 10);
    lv_chart_set_next(chart2, ser3, 10);
    lv_chart_set_next(chart2, ser3, 10);
    lv_chart_set_next(chart2, ser3, 10);
    lv_chart_set_next(chart2, ser3, 30);
    lv_chart_set_next(chart2, ser3, 70);
    lv_chart_set_next(chart2, ser3, 90);

    /*Directly set points on 'dl2'*/
    ser4->points[0] = 90;
    ser4->points[1] = 70;
    ser4->points[2] = 65;
    ser4->points[3] = 65;
    ser4->points[4] = 65;
    ser4->points[5] = 65;
    ser4->points[6] = 65;
    ser4->points[7] = 65;
    ser4->points[8] = 65;
    ser4->points[9] = 65;

    lv_chart_refresh(chart2); /*Required after direct set*/
		
		    /*Add two data series*/
    lv_chart_series_t * ser5 = lv_chart_add_series(chart3, LV_COLOR_RED);
    lv_chart_series_t * ser6 = lv_chart_add_series(chart3, LV_COLOR_GREEN);

    /*Set the next points on 'dl1'*/
    lv_chart_set_next(chart3, ser5, 10);
    lv_chart_set_next(chart3, ser5, 10);
    lv_chart_set_next(chart3, ser5, 10);
    lv_chart_set_next(chart3, ser5, 10);
    lv_chart_set_next(chart3, ser5, 10);
    lv_chart_set_next(chart3, ser5, 10);
    lv_chart_set_next(chart3, ser5, 10);
    lv_chart_set_next(chart3, ser5, 30);
    lv_chart_set_next(chart3, ser5, 70);
    lv_chart_set_next(chart3, ser5, 90);

    /*Directly set points on 'dl2'*/
    ser6->points[0] = 90;
    ser6->points[1] = 70;
    ser6->points[2] = 65;
    ser6->points[3] = 65;
    ser6->points[4] = 65;
    ser6->points[5] = 65;
    ser6->points[6] = 65;
    ser6->points[7] = 65;
    ser6->points[8] = 65;
    ser6->points[9] = 65;

    lv_chart_refresh(chart3); /*Required after direct set*/
}

static void preload_test(void)
{
    /*Create a style for the Preloader*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.line.width = 10;                         /*10 px thick arc*/
    style.line.color = lv_color_hex3(0x258);       /*Blueish arc color*/

    style.body.border.color = lv_color_hex3(0xBBB); /*Gray background color*/
    style.body.border.width = 10;
    style.body.padding.left = 0;

    /*Create a Preloader object*/
    lv_obj_t * preload = lv_preload_create(lv_scr_act(), NULL);
    lv_obj_set_size(preload, 100, 100);
    lv_obj_align(preload, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_preload_set_style(preload, LV_PRELOAD_STYLE_MAIN, &style);
}

static void gauge_test(void)
{
    /*Create a style*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_pretty_color);
    style.body.main_color = lv_color_hex3(0x666);     /*开始的刻度条颜色*/
    style.body.grad_color =  lv_color_hex3(0x666);    /*结束的刻度条颜色*/
    style.body.padding.left = 10;                     /*刻度条的长度*/
    style.body.padding.inner = 8 ;                    /*文本与刻度条的距离*/
    style.body.border.color = lv_color_hex3(0x333);   /*指针圆环的颜色*/
    style.line.width = 3;                             /*线条宽度*/
    style.text.color = lv_color_hex3(0x333);          /*文本颜色*/
    style.line.color = LV_COLOR_RED;                  /*超出临界值的线条颜色*/

    /*Describe the color for the needles*/
    static lv_color_t needle_colors[3];
    needle_colors[0] = LV_COLOR_BLUE;
    needle_colors[1] = LV_COLOR_ORANGE;
    needle_colors[2] = LV_COLOR_PURPLE;

    /*Create a gauge*/
    lv_obj_t * gauge1 = lv_gauge_create(lv_scr_act(), NULL);
    lv_gauge_set_style(gauge1, LV_GAUGE_STYLE_MAIN, &style);
    lv_gauge_set_needle_count(gauge1, 3, needle_colors);
    lv_obj_set_size(gauge1, 150, 150);
    lv_obj_align(gauge1, NULL, LV_ALIGN_CENTER, 0, 20);
    lv_gauge_set_critical_value(gauge1, 80);
    lv_gauge_set_range(gauge1, 0, 100);

    /*Set the values*/
    lv_gauge_set_value(gauge1, 0, 10);
    lv_gauge_set_value(gauge1, 1, 20);
    lv_gauge_set_value(gauge1, 2, 30);

}

static void line_test(void)
{
    /*Create an array for the points of the line*/
    static lv_point_t line_points[] = { {5, 5}, {70, 70}, {120, 10}, {180, 60}, {240, 10} };

    /*Create new style (thick dark blue)*/
    static lv_style_t style_line;
    lv_style_copy(&style_line, &lv_style_plain);
    style_line.line.color = LV_COLOR_MAKE(0x00, 0x3b, 0x75);
    style_line.line.width = 3;
    style_line.line.rounded = 1;

    /*Copy the previous line and apply the new style*/
    lv_obj_t * line1;
    line1 = lv_line_create(lv_scr_act(), NULL);
    lv_line_set_points(line1, line_points, 5);     /*Set the points*/
    lv_line_set_style(line1, LV_LINE_STYLE_MAIN, &style_line);
    lv_obj_align(line1, NULL, LV_ALIGN_CENTER, 0, 0);
}

static void tabview_test(void)
{
    /*Create a Tab view object*/
    lv_obj_t *tabview;
    tabview = lv_tabview_create(lv_scr_act(), NULL);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Tab 2");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Tab 3");


    /*Add content to the tabs*/
    lv_obj_t * label = lv_label_create(tab1, NULL);
    lv_label_set_text(label, "This the first tab\n\n"
                             "If the content\n"
                             "of a tab\n"
                             "become too long\n"
                             "the it \n"
                             "automatically\n"
                             "become\n"
                             "scrollable.");

    label = lv_label_create(tab2, NULL);
    lv_label_set_text(label, "Second tab");

    label = lv_label_create(tab3, NULL);
    lv_label_set_text(label, "Third tab");
}

static void page_test(void)
{
    /*Create a scroll bar style*/
    static lv_style_t style_sb;
    lv_style_copy(&style_sb, &lv_style_plain);
    style_sb.body.main_color = LV_COLOR_BLACK;
    style_sb.body.grad_color = LV_COLOR_BLACK;
    style_sb.body.border.color = LV_COLOR_WHITE;
    style_sb.body.border.width = 1;
    style_sb.body.border.opa = LV_OPA_70;
    style_sb.body.radius = LV_RADIUS_CIRCLE;
    style_sb.body.opa = LV_OPA_60;
    style_sb.body.padding.right = 3;
    style_sb.body.padding.bottom = 3;
    style_sb.body.padding.inner = 8;        /*Scrollbar width*/

    /*Create a page*/
    lv_obj_t * page = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_size(page, 150, 200);
    lv_obj_align(page, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_page_set_style(page, LV_PAGE_STYLE_SB, &style_sb);           /*Set the scrollbar style*/

    /*Create a label on the page*/
    lv_obj_t * label = lv_label_create(page, NULL);
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);            /*Automatically break long lines*/
    lv_obj_set_width(label, lv_page_get_fit_width(page));          /*Set the label width to max value to not show hor. scroll bars*/
    lv_label_set_text(label, "Lorem ipsum dolor sit amet, consectetur adipiscing elit,\n"
                             "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
                             "Ut enim ad minim veniam, quis nostrud exercitation ullamco\n"
                             "laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure\n"
                             "dolor in reprehenderit in voluptate velit esse cillum dolore\n"
                             "eu fugiat nulla pariatur.\n"
                             "Excepteur sint occaecat cupidatat non proident, sunt in culpa\n"
                             "qui officia deserunt mollit anim id est laborum.");
}

static void kb_test(void)
{
    /*Create styles for the keyboard*/
    static lv_style_t rel_style, pr_style;

    lv_style_copy(&rel_style, &lv_style_btn_rel);
    rel_style.body.radius = 0;
    rel_style.body.border.width = 1;

    lv_style_copy(&pr_style, &lv_style_btn_pr);
    pr_style.body.radius = 0;
    pr_style.body.border.width = 1;

    /*Create a keyboard and apply the styles*/
    lv_obj_t *kb = lv_kb_create(lv_scr_act(), NULL);
    lv_kb_set_cursor_manage(kb, true);
    lv_kb_set_style(kb, LV_KB_STYLE_BG, &lv_style_transp_tight);
    lv_kb_set_style(kb, LV_KB_STYLE_BTN_REL, &rel_style);
    lv_kb_set_style(kb, LV_KB_STYLE_BTN_PR, &pr_style);

    /*Create a text area. The keyboard will write here*/
    lv_obj_t *ta = lv_ta_create(lv_scr_act(), NULL);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_ta_set_text(ta, "");

    /*Assign the text area to the keyboard*/
    lv_kb_set_ta(kb, ta);
}

static void list_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        printf("Clicked: %s\n", lv_list_get_btn_text(obj));
    }
}

static void list_test(void)
{
    /*Create a list*/
    lv_obj_t * list1 = lv_list_create(lv_scr_act(), NULL);
    lv_obj_set_size(list1, 160, 200);
    lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Add buttons to the list*/

    lv_obj_t * list_btn;

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_FILE, "New");
    lv_obj_set_event_cb(list_btn, list_event_handler);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, "Open");
    lv_obj_set_event_cb(list_btn, list_event_handler);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "Delete");
    lv_obj_set_event_cb(list_btn, list_event_handler);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_EDIT, "Edit");
    lv_obj_set_event_cb(list_btn, list_event_handler);

    list_btn = lv_list_add_btn(list1, NULL, "Save");
    lv_obj_set_event_cb(list_btn, list_event_handler);
}

static void lmeter_test(void)
{
    /*Create a style for the line meter*/
    static lv_style_t style_lmeter;
    lv_style_copy(&style_lmeter, &lv_style_pretty_color);
    style_lmeter.line.width = 2;
    style_lmeter.line.color = LV_COLOR_SILVER;
    style_lmeter.body.main_color = lv_color_hex(0x91bfed);         /*Light blue*/
    style_lmeter.body.grad_color = lv_color_hex(0x04386c);         /*Dark blue*/
    style_lmeter.body.padding.left = 16;                           /*Line length*/

    /*Create a line meter */
    lv_obj_t * lmeter;
    lmeter = lv_lmeter_create(lv_scr_act(), NULL);
    lv_lmeter_set_range(lmeter, 0, 100);                   /*Set the range*/
    lv_lmeter_set_value(lmeter, 80);                       /*Set the current value*/
    lv_lmeter_set_scale(lmeter, 240, 31);                  /*Set the angle and number of lines*/
    lv_lmeter_set_style(lmeter, LV_LMETER_STYLE_MAIN, &style_lmeter);           /*Apply the new style*/
    lv_obj_set_size(lmeter, 150, 150);
    lv_obj_align(lmeter, NULL, LV_ALIGN_CENTER, 0, 0);
}

LV_IMG_DECLARE(WaveShare_LOGO)
void img_test(void)
{
    lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &WaveShare_LOGO);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t * img2 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img2, LV_SYMBOL_AUDIO LV_SYMBOL_VIDEO
                         LV_SYMBOL_OK  LV_SYMBOL_LIST
                         LV_SYMBOL_CLOSE LV_SYMBOL_HOME
                         LV_SYMBOL_WIFI LV_SYMBOL_BATTERY_EMPTY
                         LV_SYMBOL_BLUETOOTH );
    lv_obj_align(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

static lv_obj_t * imgbtn1;
static lv_obj_t * imgbtn_label;

static void imgbtn_event_handler(lv_obj_t * obj, lv_event_t event)
{
    static int i = 0;;
    static char buffer[32];

    if (obj == imgbtn1) {
        if(event == LV_EVENT_CLICKED) {
            i++;
            sprintf(buffer, "imgBtn Clicked %d", i);
            lv_label_set_text(imgbtn_label,buffer);
        }
    }
}

void imgbtn_test(void)
{
    static lv_style_t style_pr;
    lv_style_copy(&style_pr, &lv_style_plain);
    style_pr.image.color = LV_COLOR_BLACK;
    style_pr.image.intense = LV_OPA_50;

    /*Create an Image button*/
    imgbtn1 = lv_imgbtn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(imgbtn1, imgbtn_event_handler);
    lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_REL, &WaveShare_LOGO);
    lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_PR, &WaveShare_LOGO);
    lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_TGL_REL, &WaveShare_LOGO);
    lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_TGL_PR, &WaveShare_LOGO);
    lv_imgbtn_set_style(imgbtn1, LV_BTN_STATE_PR, &style_pr);        /*Use the darker style in the pressed state*/
    lv_imgbtn_set_style(imgbtn1, LV_BTN_STATE_TGL_PR, &style_pr);
    lv_imgbtn_set_toggle(imgbtn1, true);
    lv_obj_align(imgbtn1, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);

    imgbtn_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(imgbtn_label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(imgbtn_label, 300);
    lv_label_set_text(imgbtn_label,"Watting imgBtn for clicked!");
    lv_obj_align(imgbtn_label, NULL, LV_ALIGN_IN_LEFT_MID, 0, 90);
    lv_label_set_align(imgbtn_label, LV_LABEL_ALIGN_CENTER);
}

LV_FONT_DECLARE(ChineseFont)

void Chinese_character_test(void){
    static lv_style_t style1;
    lv_style_copy(&style1, &lv_style_plain);
    style1.text.font = &ChineseFont; /* 设置自定义字体 */

    /*Create a label and set new text*/
    lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);       /* 创建标签 */
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, -500, 0);         /* 设置相对位置 */
    lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &style1);     /* 设置样式 */
    lv_label_set_text(label, "你好,微雪课堂!这里是汉字显示展示!"); /* 显示汉字 */
}

void demo_test(void)
{
//	obj_test();
	
//	arc1_test();
//	arc2_test();
	
//	bar_test();
	
//	btn_test();
	
//	btnm_test();
	
//	calendar_test();
	
//	canvas_test();
	
//	cb_test();
	
//	label_test();
	
//	led_test();
	
//	table_test();

//	win_test();

	sw_test();

//	roller_test();
	
//	slider_test();

//	cont_test();
	
//	spinbox_test();
	
//	ddlist_test1();
//	ddlist_test2();
	
//	mbox_test();
	
//	chart_test();
	
//	preload_test();
	
//	gauge_test();
	
//	line_test();
	
//	tabview_test();
	
//	page_test();
	
//	kb_test();
	
//	list_test();
	
//	lmeter_test();

//	img_test();

//	imgbtn_test();

//	Chinese_character_test();
}
