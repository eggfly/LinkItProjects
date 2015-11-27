#include "xui.h"

#define FRAME_COUNT 1
#define SCREEN_WIDTH 240		/* Set to your display size */
#define SCREEN_HEIGHT 240		/* Set to your display size */

#define XUI_DEBUG

struct _xui_page {
	vm_graphic_color_argb_t background_color;
	void ** views;
	VMINT view_count;
};

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

void _init_view(void * view, _render_func func);

#define _xui_free(inner_type, var, member_name) \
	struct inner_type * _p_view = (struct inner_type *) var.member_name; \
	vm_free(_p_view);
