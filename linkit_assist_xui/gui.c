/*
 This sample code is in public domain.

 This sample code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 This sample shows a line that keeps spinning on top of a gradient
 background image.

 It decodes a PNG image resource into a frame buffer with
 vm_graphic_draw_image_memory,
 and draws a line with vm_graphic_draw_line, then start a timer with
 vm_timer_create_precise for animation.
 The timer runs every 1/10 secs, resulting in a 10 FPS animation. For each
 frame in the animation, vm_graphic_linear_transform is used to rotate
 the line, and then vm_graphic_blt_frame is called to composite the rotated line
 with the background image, and displays the result.

 Modify the macro SCREEN_WIDTH and SCREEN_HEIGHT to fit your display device.
 */

#include "vmtype.h"
#include "vmsystem.h"
#include "vmres.h"
#include "vmmemory.h"
#include "vmlog.h"
#include "vmgraphic.h"
#include "vmgraphic_font.h"
#include "ResID.h"
#include "vmtimer.h"
#include "vmthread.h"
#include <math.h>
#include "lcd_sitronix_st7789s.h"
#include "vmdcl.h"
#include "vmdcl_gpio.h"
#include "vmdcl_pwm.h"
#include "xui.h"

#define BACKLIGHT_PIN VM_PIN_P1

/* Animation timer */
VM_TIMER_ID_PRECISE g_timer_id;

// eggfly
xui_page page;
xui_text_view text_view;
void * views[] = { &text_view };

/* Update the rotating line, then update the display */
static void timer_callback(VM_TIMER_ID_PRECISE tid, void* user_data) {
	vm_log_debug("timer_callback");
	xui_validate(page);
	// TODO remove
	while (1) {
		vm_thread_sleep(10000);
	}
}

/* Free one frame */
void free_frame(vm_graphic_frame_t *frame) {
	if (NULL != frame->buffer) {
		vm_free(frame->buffer);
		frame->buffer = NULL;
	}
	frame->buffer_length = 0;
}

/* Release all memory allocated for graphics frame */
void free_drawing_resource(void) {
	//free_frame(&g_frame[0]);
//	free_frame(&g_frame[1]);
	//g_frame_group[0] = NULL;
//	g_frame_group[1] = NULL;
	vm_timer_delete_precise(g_timer_id);
}

// for debug

struct _xui_view;

typedef void (*_render_func)(struct _xui_view * view);

#define XUI_VIEW_MEMBERS \
	VMINT x, y;\
	VMINT width, height;\
	VMCHAR visibility;\
	_render_func render;

struct _xui_view {
	XUI_VIEW_MEMBERS
};

struct _xui_text_view {
	XUI_VIEW_MEMBERS // must be the first
	VMWSTR text;
	VMINT text_size;
	vm_graphic_color_argb_t text_color;
};

static void debug(void * view) {
	struct _xui_text_view * inner = (struct _xui_text_view *) view;
	vm_log_debug("inner->width,height: %d %d", inner->width, inner->height);
}
// debug end

/* The callback to be invoked by the system engine. */
void handle_system_event(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		// eggfly
		xui_init();
		text_view = xui_init_text_view();
		vm_log_debug("xui_text_view *: %d", &text_view);
		vm_log_debug("xui_text_view.view : %d", text_view.view);
		debug(text_view.view);
		page = xui_init_page((void**) &views, sizeof(views) / sizeof(void *));
		vm_log_debug("xui_page *: %d", &page);
		vm_log_debug("xui_page.page : %d", page.page);
		debug(text_view.view);
		break;
	case VM_EVENT_PAINT:
		/* Graphics library is ready to use, start drawing */
		vm_thread_sleep(10 * 1000);
		g_timer_id = vm_timer_create_precise(100, timer_callback, NULL);
		timer_callback(g_timer_id, NULL);
		break;
	case VM_EVENT_QUIT:
		//free_drawing_resource();
		/* Deinit APP resource */
		//vm_res_release();
		break;
	}
}

void lcd_backlight_level(VMUINT32 ulValue) {

	VM_DCL_HANDLE pwm_handle;
	vm_dcl_pwm_set_clock_t pwm_clock;
	vm_dcl_pwm_set_counter_threshold_t pwm_config_adv;

	vm_dcl_config_pin_mode(BACKLIGHT_PIN, VM_DCL_PIN_MODE_PWM);

	pwm_handle = vm_dcl_open(PIN2PWM(BACKLIGHT_PIN), vm_dcl_get_owner_id());
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

/* Entry point */
void vm_main(void) {
	lcd_st7789s_init();
	lcd_backlight_level(60);
	/* register system events handler */
	vm_pmng_register_system_event_callback(handle_system_event);
}
