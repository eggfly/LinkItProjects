#ifndef __XUI_H__
#define __XUI_H__

#include "vmtype.h"
#include "vmgraphic.h"

#define MAX_CHAR (256)
// #define MAX_VIEW_COUNT (256)

typedef void (*xui_render_func)(void * view);

typedef struct {
	VMINT x, y;
	VMINT width, height;
	VMINT margin_left, margin_top, margin_right, margin_bottom;
	VMINT padding_left, padding_top, padding_right, padding_bottom;
	VMCHAR visibility;
	vm_graphic_color_argb_t background_color;
	xui_render_func render; // hide this function?
} xui_view;

typedef xui_view * xui_view_collection;
typedef struct {
	xui_view_collection * collection;
} xui_page;

// void xui_set_visibility(xui_view * view, VMCHAR visibility);
// todo: need validate function ?

typedef struct {
	xui_view view;
	VMCHAR text[MAX_CHAR]; // TODO
	VMINT text_size;
	vm_graphic_color_argb_t text_color;
} xui_text_view;

void xui_init();
void xui_init_text_view(xui_text_view * text_view);
void xui_init_page(xui_page * page, xui_view_collection * views,
		VMINT view_count); // TODO: use VARGS instead?
//void xui_page_start(xui_page * page);
//void xui_page_back();
void xui_validate();
#endif
