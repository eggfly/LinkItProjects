#include "xui.h"

#define FRAME_COUNT 1
#define SCREEN_WIDTH 240		/* Set to your display size */
#define SCREEN_HEIGHT 240		/* Set to your display size */

/* Use one frame temporary */
static vm_graphic_frame_t g_frame[1] = { };

/* For frame blt */
static vm_graphic_frame_t* g_frame_group[FRAME_COUNT] = { };

/* Allocate memory for a single frame */
static VMBOOL _allocate_frame(vm_graphic_frame_t *frame) {
	if (frame == NULL) {
		return VM_FALSE;
	}
	/* We use 16-bit color, 2 bytes per pixel */
	frame->color_format = VM_GRAPHIC_COLOR_FORMAT_16_BIT;
	frame->width = SCREEN_WIDTH;
	frame->height = SCREEN_HEIGHT;
	frame->buffer_length = SCREEN_WIDTH * SCREEN_HEIGHT * 2;
	frame->buffer = vm_malloc_dma(frame->buffer_length);
	if (frame->buffer == NULL) {
		return VM_FALSE;
	}
	return VM_TRUE;
}

void xui_init() {
	_allocate_frame(&g_frame[0]);
	g_frame_group[0] = &g_frame[0];
}

static void _init_view(xui_view * view, xui_render_func func) {
	view->render = func;
	view->x = view->y = 0;
	view->width = view->height = 0;
	view->margin_left = view->margin_top = view->margin_right =
			view->margin_bottom = 0;
	view->padding_left = view->padding_top = view->padding_right =
			view->padding_bottom = 0;
	view->visibility = 1;
	view->background_color.a = 0;
}

static void _render_text_view(void * view) {
	xui_text_view * text_view = (xui_text_view*) view;
	VMWCHAR s[260]; /* string's buffer */
	VMCHAR * str = text_view->text;
	vm_chset_ascii_to_ucs2(s, 260, str);
	vm_graphic_set_color(text_view->text_color);
	vm_graphic_set_font_size(text_view->text_size);
	vm_graphic_draw_text(&g_frame, text_view->view.x, text_view->view.y, s);
}

void xui_init_text_view(xui_text_view * text_view) {
	_init_view(&text_view->view, _render_text_view);
	text_view->text_size = 10; // todo
	vm_graphic_color_argb_t * color = &text_view->text_color;
	color->a = 255;
	color->r = color->g = color->b = 0;
}

void xui_validate(xui_page * page) {
	// init with zero
	vm_graphic_point_t positions[FRAME_COUNT] = { };
	/* composite and display */
	vm_graphic_blt_frame(g_frame_group, positions, FRAME_COUNT);
}

struct {
} _xui_page;

void xui_init_page(xui_page * page, xui_view_collection * views,
		VMINT view_count) {

}
