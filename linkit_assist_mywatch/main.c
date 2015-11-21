/*
 This example code is in public domain.

 This example code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 This example shows how to receive touch event and the x,y value from the board.

 This example use vm_touch_register_event_callback() to register touch handler.
 when touch the lcd on the board,the touch event and x,y value will be sent to application
 through this handler.

 Just run this application,and see the catcher log or monitor log or lcd.
 */
#include "vmtype.h" 
#include "vmlog.h"
#include "vmsystem.h"
#include "vmpwr.h"
#include "vmgraphic.h"
#include "vmgraphic_font.h"
#include "vmtouch.h"
#include "vmdcl.h"
#include "vmboard.h"
#include "vmgsm_cell.h"
#include "vmdcl_pwm.h"
#include "stdio.h"

VMCHAR g_buff[256];

vm_graphic_frame_t g_frame;
const vm_graphic_frame_t* g_frame_blt_group[1];
static VMUINT8* g_font_pool;

static void font_init(void) {
	VM_RESULT result;
	VMUINT32 pool_size;

	result = vm_graphic_get_font_pool_size(0, 0, 0, &pool_size);
	if (VM_IS_SUCCEEDED(result)) {
		g_font_pool = (VMUINT8*) vm_malloc(pool_size);
		if (NULL != g_font_pool) {
			vm_graphic_init_font_pool(g_font_pool, pool_size);
		}
		vm_log_info("allocate font pool memory failed");
	} else {
		vm_log_info("get font pool size failed, result:%d", result);
	}
}

/**
 * max is 8191
 */
void lcd_backlight_level(VMUINT32 ulValue) {
	VM_DCL_HANDLE pwm_handle;
	vm_dcl_pwm_set_clock_t pwm_clock;
	vm_dcl_pwm_set_counter_threshold_t pwm_config_adv;
	vm_dcl_config_pin_mode(VM_PIN_P1, VM_DCL_PIN_MODE_PWM);
	pwm_handle = vm_dcl_open(PIN2PWM(VM_PIN_P1), vm_dcl_get_owner_id());
	vm_dcl_control(pwm_handle, VM_PWM_CMD_START, 0);
	pwm_config_adv.counter = 100;
	pwm_config_adv.threshold = ulValue;
	pwm_clock.source_clock = 0;
	pwm_clock.source_clock_division = 3;
	vm_dcl_control(pwm_handle, VM_PWM_CMD_SET_CLOCK, (void *) (&pwm_clock));
	vm_dcl_control(pwm_handle, VM_PWM_CMD_SET_COUNTER_AND_THRESHOLD,
			(void *) (&pwm_config_adv));
	vm_dcl_close(pwm_handle);
}

void log_init(void) {
	vm_graphic_color_argb_t color; /* use to set screen and text color */
	vm_graphic_point_t frame_position[1] = { 0, 0 };

	g_frame.width = 240;
	g_frame.height = 240;
	g_frame.color_format = VM_GRAPHIC_COLOR_FORMAT_16_BIT;
	g_frame.buffer = (VMUINT8*) vm_malloc_dma(
			g_frame.width * g_frame.height * 2);
	g_frame.buffer_length = (g_frame.width * g_frame.height * 2);
	g_frame_blt_group[0] = &g_frame;

	/* set color and draw bg*/
	color.a = 255;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	vm_graphic_set_color(color);
	vm_graphic_draw_solid_rectangle(&g_frame, 0, 0, 240, 240);

	vm_graphic_blt_frame(g_frame_blt_group, frame_position, (VMINT) 1);
}

void log_info(VMINT line, VMSTR str) {
	VMWCHAR s[260]; /* string's buffer */
	VMUINT32 size;
	vm_graphic_color_argb_t color; /* use to set screen and text color */
	vm_graphic_point_t frame_position[1] = { 0, 0 };

	vm_chset_ascii_to_ucs2(s, 260, str);
	/* set color and draw bg*/
	color.a = 0;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	vm_graphic_set_color(color);
	vm_graphic_draw_solid_rectangle(&g_frame, 0, line * 20, 240, 20);

	/* set color and draw text*/
	color.a = 0;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	vm_graphic_set_color(color);
	vm_graphic_set_font_size(18);
	vm_graphic_draw_text(&g_frame, 0, line * 20, s);

	/* flush the screen with text data */
	vm_graphic_blt_frame(g_frame_blt_group, frame_position, (VMINT) 1);
}

void handle_cell_info_change();

void handle_touchevt(VM_TOUCH_EVENT event, VMINT x, VMINT y) {
	vm_log_info("touch event=%d,touch x=%d,touch y=%d", event, x, y); /* output log to monitor or catcher */
	sprintf(g_buff, "touch event = %d, x=%d, y=%d", event, x, y);
	log_info(2, g_buff); /* output log to LCD if have */
	VMINT level = vm_pwr_get_battery_level();
	VMBOOL is_charging = vm_pwr_is_charging();
	sprintf(g_buff, "battery level: %d%%, state: %s", level,
			is_charging ? "charging" : "not charging");
	log_info(0, g_buff); // eggfly
	handle_cell_info_change();
}

vm_gsm_cell_info_t g_info; /* cell information data */

void handle_cell_info_change() {
	vm_gsm_cell_get_current_cell_info(&g_info);
	VMCHAR buf[256];
	sprintf(buf, "ar=%d, bsic=%d, rxlev=%d, mcc=%d, mnc=%d, lac=%d, ci=%d",
			g_info.arfcn, g_info.bsic, g_info.rxlev, g_info.mcc, g_info.mnc,
			g_info.lac, g_info.ci);
	vm_log_info(buf);
	log_info(4, buf);
	sprintf(buf, "GSM Signal Level: %d%%", g_info.rxlev * 100 / 63);
	log_info(5, buf);
}
void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		break;
	case VM_EVENT_PAINT:
		log_init();
		log_info(1, "open application success");
		break;
	case VM_EVENT_CELL_INFO_CHANGE:
		handle_cell_info_change();
		/* After opening the cell, this event will occur when the cell info changes.
		 * The new data of the cell info can be obtained from here. */
		break;
	case VM_EVENT_QUIT:
		break;
	}
}

static VMUINT32 backlight = 50;

/* Entry point */
void vm_main(void) {
	/* register system events handler */
	lcd_st7789s_init();
	lcd_backlight_level(backlight);
	tp_gt9xx_init();
	font_init();

	// init cells
	VMINT result = vm_gsm_cell_open();
	vm_log_info("open result = %d", result);

	vm_pmng_register_system_event_callback(handle_sysevt);
	vm_touch_register_event_callback(handle_touchevt);
}
