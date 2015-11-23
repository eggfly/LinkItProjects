#ifndef __XUI_H__
#define __XUI_H__

#include "vmtype.h"
#include "vmgraphic.h"

#define MAX_CHAR (256)
// #define MAX_VIEW_COUNT (256)

typedef struct _xui_view xui_view;

// xui_page only need struct declearation, but no define
typedef struct _xui_page xui_page;

typedef xui_view * xui_view_collection;

typedef void (*xui_render_func)(void * view);

struct _xui_view {
	VMINT x, y;
	VMINT width, height;
	VMINT margin_left, margin_top, margin_right, margin_bottom;
	VMINT padding_left, padding_top, padding_right, padding_bottom;
	VMCHAR visibility;
	vm_graphic_color_argb_t background_color;
	xui_render_func render; // hide this function?
};

// void xui_set_visibility(xui_view * view, VMCHAR visibility);
// todo: need validate function ?

typedef struct {
	xui_view view;
	VMCHAR text[MAX_CHAR]; // TODO
} xui_text_view;

void xui_init_text_view(xui_text_view * text_view);
void xui_init_page(xui_page * page, xui_view_collection * views); // TODO: use VARGS instead?
void xui_page_start(xui_page * page);
void xui_page_back();

#endif
