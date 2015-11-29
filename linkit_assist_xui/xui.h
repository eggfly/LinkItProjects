#ifndef __XUI_H__
#define __XUI_H__

#include "vmtype.h"
#include "vmgraphic.h"

// TODO convert to void * typedef
typedef struct {
	void * page;
} xui_page;

// TODO convert to void * typedef
// TODO give them as a public base type to use
typedef struct {
	void * view;
} xui_view;

// TODO convert to void * typedef
typedef struct {
	void * view;
} xui_text_view;

void xui_init();
void xui_release();
void xui_lcd_st7789s_init();
void xui_lcd_backlight_level(VMUINT32 ulValue);

VMINT xui_view_get_x(void * view);
VMINT xui_view_get_y(void * view);
VMINT xui_view_get_width(void * view);
VMINT xui_view_get_height(void * view);

void xui_view_set_visibility(void * view, VMBOOL visibility);
void xui_view_set_x(void * view, VMINT value);
void xui_view_set_y(void * view, VMINT value);
void xui_view_set_width(void * view, VMINT value);
void xui_view_set_height(void * view, VMINT value);
void xui_view_set_background_color(void * view, vm_graphic_color_argb_t color);

xui_text_view xui_init_text_view();
void xui_text_view_set_text_color(void * view, vm_graphic_color_argb_t color);
void xui_free_text_view(xui_text_view view);

xui_page xui_init_page(void ** views, VMINT view_count);
void xui_page_set_background_color(void * page, vm_graphic_color_argb_t color);
void xui_free_page(xui_page page);

// void xui_set_visibility(xui_view * view, VMCHAR visibility);
// todo: need validate function ?

//void xui_page_start(xui_page * page);
//void xui_page_back();
void xui_validate(xui_page page); // todo: frame handler?
#endif
