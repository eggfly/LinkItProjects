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
#include "vmlog.h"
#include "ResID.h"
#include "vmtimer.h"
#include "vmgraphic.h"
#include "vmthread.h"
#include "xui/xui.h"
#include "vmtouch.h"

/* Animation timer */
VM_TIMER_ID_PRECISE g_timer_id;

#define ROWS 4
#define COLUMNS 3
#define VIEW_COUNT (ROWS * COLUMNS)
xui_page page;
xui_text_view texts[ROWS][COLUMNS];
void * views[VIEW_COUNT] = { };
VMINT x = 0, y = 0;

/* Update the rotating line, then update the display */
static void timer_callback(VM_TIMER_ID_PRECISE tid, void* user_data) {
	vm_log_debug("timer_callback");
	xui_validate(page);
}

static vm_graphic_color_argb_t white() {
	vm_graphic_color_argb_t color;
	color.a = 255;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	return color;
}

/* The callback to be invoked by the system engine. */
void handle_system_event(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		xui_init();
		int row, col;
		for (row = 0; row < ROWS; row++) {
			for (col = 0; col < COLUMNS; col++) {
				texts[row][col] = xui_init_text_view();
				void * view = texts[row][col];
				vm_graphic_color_argb_t color;
				color.a = 255;
				color.r = 0;
				color.g = 255;
				color.b = 0;
				xui_view_set_background_color(view, color);
				xui_view_set_x(view, 30 + col * 60);
				xui_view_set_y(view, 30 + row * 30);
				xui_view_set_width(view, 60);
				xui_view_set_height(view, 30);
				views[row * COLUMNS + col] = &texts[row][col];
			}
		}
		xui_text_view_set_text(texts[0][0], (VMCHAR *) "1");
		xui_text_view_set_text(texts[0][1], (VMCHAR *) "2");
		xui_text_view_set_text(texts[0][2], (VMCHAR *) "3");
		xui_text_view_set_text(texts[1][0], (VMCHAR *) "4");
		xui_text_view_set_text(texts[1][1], (VMCHAR *) "5");
		xui_text_view_set_text(texts[1][2], (VMCHAR *) "6");
		xui_text_view_set_text(texts[2][0], (VMCHAR *) "7");
		xui_text_view_set_text(texts[2][1], (VMCHAR *) "8");
		xui_text_view_set_text(texts[2][2], (VMCHAR *) "9");
		xui_text_view_set_text(texts[3][0], (VMCHAR *) "*");
		xui_text_view_set_text(texts[3][1], (VMCHAR *) "0");
		xui_text_view_set_text(texts[3][2], (VMCHAR *) "#");
		page = xui_init_page((void**) &views, sizeof(views) / sizeof(void *)); // TODO: check
		xui_page_set_background_color(page, white());
		break;
	case VM_EVENT_PAINT:
		/* Graphics library is ready to use, start drawing */
		// vm_thread_sleep(10 * 1000);
		g_timer_id = vm_timer_create_precise(8000, timer_callback, NULL);
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

void handle_touchevt(VM_TOUCH_EVENT event, VMINT x, VMINT y) {
	/* output log to monitor or catcher */
	vm_log_info("touch event=%d, x=%d, y=%d", event, x, y);
}

/* Entry point */
void vm_main(void) {
	xui_lcd_init();
	xui_lcd_backlight_level(100);
	xui_touch_init();
	/* register system events handler */
	vm_pmng_register_system_event_callback(handle_system_event);
	vm_touch_register_event_callback(handle_touchevt);
}
