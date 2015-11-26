#include "xui_private.h"
#include "vmmemory.h"
#include "vmgraphic.h"
#include "vmlog.h"

/* Use one frame temporary */
static vm_graphic_frame_t g_frame[1] = { };

/* For frame blt */
const static vm_graphic_frame_t* g_frame_group[FRAME_COUNT] = { };

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
	vm_log_debug("xui_init");
	_allocate_frame(&g_frame[0]);
	g_frame_group[0] = &g_frame[0];
}

static void _init_view(void * view, _render_func func) {
	struct _xui_view * p = (struct _xui_view *) view;
	vm_log_debug("_render_func pointer: %d", func);
	p->render = func;
	p->x = p->y = 0;
	p->width = p->height = 80; // TODO
	p->visibility = 1;
}

VMWCHAR g_wstr[260];

void _render_text_view(struct _xui_view * view) {
	vm_log_debug("_render_text_view");
	struct _xui_text_view * text_view = (struct _xui_text_view*) view;
	vm_graphic_set_color(text_view->text_color);
	vm_graphic_set_font_size(text_view->text_size);
	char * str = "text";
	vm_chset_ascii_to_ucs2(g_wstr, 260, str);
	text_view->text = g_wstr;
	vm_graphic_draw_text(&g_frame, text_view->x, text_view->y, text_view->text);
}

xui_text_view xui_init_text_view() {
	vm_log_debug("xui_init_text_view");
	xui_text_view result;
	result.view = vm_malloc(sizeof(struct _xui_text_view));
	_init_view(result.view, _render_text_view);
	struct _xui_text_view * p_view = (struct _xui_text_view *) result.view;
	vm_log_debug("struct _xui_text_view * : %d", p_view);
	p_view->text_size = 10; // TODO
	vm_graphic_color_argb_t * color = &p_view->text_color;
	color->a = 255;
	color->r = color->g = color->b = 0;
}

void xui_validate(xui_page page) {
	vm_log_debug("xui_validate");
	struct _xui_page * p_page = (struct _xui_page *) page.page;
	vm_log_debug("xui_page.page : %d", page.page);
	vm_log_debug("xui_page.page : %d", p_page);
	int i;
	void ** views = p_page->views;
	int view_count = p_page->view_count;
	vm_log_debug("_xui_page->views ** : %d", views);
	vm_log_debug("view_count: %d", view_count);
	vm_log_debug("1");
	for (i = 0; i < view_count; i++) {
		void * view = views[i];
		xui_view * p_view = (xui_view*) view;
		vm_log_debug("_xui_page->views[0] * : %d", p_view); // right
		void * p_internal_view = p_view->view;
		struct _xui_text_view * this = (struct _xui_text_view *) p_internal_view;
		vm_log_debug("_xui_text_view *: %d", this); // right
		vm_log_debug("_xui_text_view->render pointer: %d", this->render);
		//this->render(this);
		vm_log_debug("7");
	}
	vm_log_debug("8");
	vm_graphic_point_t positions[FRAME_COUNT] = { };
	/* composite and display */
	//vm_graphic_blt_frame(g_frame_group, positions, FRAME_COUNT);
}

xui_page xui_init_page(void ** views, VMINT view_count) {
	vm_log_debug("xui_init_page");
	xui_page result;
	result.page = vm_malloc(sizeof(struct _xui_page));
	struct _xui_page * page = (struct _xui_page *) result.page;
	page->views = views;
	page->view_count = view_count;
	return result;
}
