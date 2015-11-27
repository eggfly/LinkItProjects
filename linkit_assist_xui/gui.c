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
#include "ResID.h"
#include "vmtimer.h"
#include "vmthread.h"
#include <math.h>
#include "xui.h"

/* Animation timer */
VM_TIMER_ID_PRECISE g_timer_id;

xui_page page;
xui_text_view text_view;
void * views[] = { &text_view };

/* Update the rotating line, then update the display */
static void timer_callback(VM_TIMER_ID_PRECISE tid, void* user_data) {
	vm_log_debug("timer_callback");
	xui_validate(page);
}

/* The callback to be invoked by the system engine. */
void handle_system_event(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		xui_init();
		text_view = xui_init_text_view();
		page = xui_init_page((void**) &views, sizeof(views) / sizeof(void *));
		break;
	case VM_EVENT_PAINT:
		/* Graphics library is ready to use, start drawing */
		vm_thread_sleep(10 * 1000);
		g_timer_id = vm_timer_create_precise(1000, timer_callback, NULL);
		timer_callback(g_timer_id, NULL);
		break;
	case VM_EVENT_QUIT:
		xui_release();
		vm_timer_delete_precise(g_timer_id);
		/* Deinit APP resource */
		//vm_res_release();
		break;
	}
}

/* Entry point */
void vm_main(void) {
	xui_lcd_st7789s_init();
	xui_lcd_backlight_level(60);
	/* register system events handler */
	vm_pmng_register_system_event_callback(handle_system_event);
}
