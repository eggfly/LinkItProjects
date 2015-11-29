#include "xui_private.h"
#include "vmmemory.h"
#include "vmgraphic.h"
#include "vmgraphic_font.h"
#include "vmlog.h"

/* Use one frame temporary */
vm_graphic_frame_t g_frame[1] = { };
static VMUINT8* g_font_pool;

/* For frame blt */
const static vm_graphic_frame_t* g_frame_group[FRAME_COUNT] = { };

static void _font_init(void) {
#ifdef XUI_DEBUG
	vm_log_debug("_font_init");
#endif
	VM_RESULT result;
	VMUINT32 pool_size;

	result = vm_graphic_get_font_pool_size(0, 0, 0, &pool_size);
	if (VM_IS_SUCCEEDED(result)) {
		g_font_pool = (VMUINT8*) vm_malloc(pool_size);
		if (NULL != g_font_pool) {
			vm_graphic_init_font_pool(g_font_pool, pool_size);
		} else {
			vm_log_info("allocate font pool memory failed");
		}
	} else {
		vm_log_info("get font pool size failed, result:%d", result);
	}
}

/* Allocate memory for a single frame */
static VMBOOL _allocate_frame(vm_graphic_frame_t *frame) {
#ifdef XUI_DEBUG
	vm_log_debug("_allocate_frame");
#endif
	if (frame == NULL) {
		return VM_FALSE;
	}
	/* We use 16-bit color, 2 bytes per pixel */
	frame->color_format = VM_GRAPHIC_COLOR_FORMAT_32_BIT;
	frame->width = SCREEN_WIDTH;
	frame->height = SCREEN_HEIGHT;
	frame->buffer_length = SCREEN_WIDTH * SCREEN_HEIGHT * 4;
	frame->buffer = vm_malloc_dma(frame->buffer_length);
	if (frame->buffer == NULL) {
		return VM_FALSE;
	}
	return VM_TRUE;
}

void xui_init() {
#ifdef XUI_DEBUG
	vm_log_debug("xui_init");
#endif
	// TODO two buffer refresh
	_allocate_frame(&g_frame[0]);
	g_frame_group[0] = &g_frame[0];
	_font_init();
}

static void _free_frame(vm_graphic_frame_t * frame);

void xui_release() {
#ifdef XUI_DEBUG
	vm_log_debug("xui_release");
#endif
	// TODO release in a for loop
	_free_frame(&g_frame[0]);
	g_frame_group[0] = NULL;
}

void _init_view(void * view, _render_func func) {
#ifdef XUI_DEBUG
	vm_log_debug("_init_view");
	vm_log_debug("_render_func pointer: %d", func);
#endif
	struct _xui_view * p = (struct _xui_view *) view;
	p->render = func;
	p->x = p->y = 0;
	p->width = p->height = 80; // TODO
	p->visibility = 1;
	vm_graphic_color_argb_t color;
	// transparent
	color.a = color.r = color.g = color.b = 0;
	p->background_color = color;
}

void xui_validate(xui_page page) {
#ifdef XUI_DEBUG
	vm_log_debug("xui_validate");
#endif
	struct _xui_page * p_page = (struct _xui_page *) page.page;
	int i;
	void ** views = p_page->views;
	int view_count = p_page->view_count;
#ifdef XUI_DEBUG
	vm_log_debug("_xui_page->views ** : %d", views);
	vm_log_debug("view_count: %d", view_count);
#endif
	// draw background
	vm_graphic_set_color(p_page->background_color);
	vm_graphic_draw_solid_rectangle(&g_frame[0], 0, 0, SCREEN_WIDTH,
	SCREEN_HEIGHT);

	for (i = 0; i < view_count; i++) {
		void * view = views[i];
		xui_view * p_view = (xui_view*) view;
		void * p_internal_view = p_view->view;
		struct _xui_view * this = (struct _xui_view *) p_internal_view;
#ifdef XUI_DEBUG
		vm_log_debug("_xui_page->views[0] * : %d", p_view);
		vm_log_debug("_xui_view *: %d", this);
		vm_log_debug("_xui_view->width: %d", this->width);
		vm_log_debug("_xui_view->height: %d", this->height);
		vm_log_debug("_xui_view->visibility: %d", this->visibility);
		vm_log_debug("_xui_view->render pointer: %d", this->render);
#endif
		if (this->visibility) {
			// draw view background
			vm_graphic_set_color(this->background_color);
			vm_graphic_draw_solid_rectangle(&g_frame[0], this->x, this->y,
					this->width, this->height);
			// view sub type's render logic (virtual function)
			this->render(this);
		}
	}
	vm_graphic_point_t positions[FRAME_COUNT] = { };
	/* composite and display */
	vm_graphic_blt_frame(g_frame_group, positions, FRAME_COUNT);
	vm_log_debug("xui_validate finish");
}

/* Free one frame */
static void _free_frame(vm_graphic_frame_t *frame) {
	if (NULL != frame->buffer) {
		vm_free(frame->buffer);
		frame->buffer = NULL;
	}
	frame->buffer_length = 0;
}

void xui_view_set_x(void * view, VMINT value) {
	struct _xui_view * p_view = (struct _xui_view *) view;
	p_view->x = value;
}

void xui_view_set_y(void * view, VMINT value) {
	struct _xui_view * p_view = (struct _xui_view *) view;
	p_view->y = value;
}

void xui_view_set_visibility(void * view, VMBOOL visibility) {
	struct _xui_view * p_view = (struct _xui_view *) view;
	p_view->visibility = visibility;
}

void xui_view_set_background_color(void * view, vm_graphic_color_argb_t color) {
	struct _xui_view * p_view = (struct _xui_view *) view;
	p_view->background_color = color;
}
