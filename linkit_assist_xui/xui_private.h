#include "xui.h"

#define FRAME_COUNT 1
#define SCREEN_WIDTH 240		/* Set to your display size */
#define SCREEN_HEIGHT 240		/* Set to your display size */

struct _xui_page {
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
	VMWSTR text;
	VMINT text_size;
	vm_graphic_color_argb_t text_color;
	XUI_VIEW_MEMBERS // must be the first
};
