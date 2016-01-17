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

typedef void(*_render_func)(struct _xui_view * view);

#define XUI_VIEW_MEMBERS \
	VMINT id; \
	VMINT x, y; \
	VMINT width, height; \
	VMBOOL visibility; \
	vm_graphic_color_argb_t background_color; \
	_render_func render; \
	touch_event_callback touch_event_cb; \
	// end of members

struct _xui_view {
	XUI_VIEW_MEMBERS
};

struct _xui_text_view {
	XUI_VIEW_MEMBERS // must be the first
	VMWCHAR text[XUI_MAX_CHAR_LENGTH];
	VMINT text_size;
	vm_graphic_color_argb_t text_color;
	xui_gravity gravity;
};

void _init_view(void * view, _render_func func);

#define _xui_free(p_var) vm_free(p_var);
