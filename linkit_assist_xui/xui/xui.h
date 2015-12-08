#ifndef __XUI_H__
#define __XUI_H__

#include "vmtype.h"
#include "vmgraphic.h"
#include "vmtouch.h"

#define XUI_MAX_CHAR_LENGTH 260

typedef void * xui_page;

// TODO give them as a public base type to use
typedef void * xui_view;
typedef void(*touch_event_callback)(xui_view view, VM_TOUCH_EVENT event, VMINT x, VMINT y);

typedef void * xui_text_view;

typedef enum {
	XUI_TOP_LEFT,
	XUI_TOP_CENTER,
	XUI_TOP_RIGHT,
	XUI_CENTER_LEFT,
	XUI_CENTER,
	XUI_CENTER_RIGHT,
	XUI_BOTTOM_LEFT,
	XUI_BOTTOM_CENTER,
	XUI_BOTTOM_RIGHT,
} xui_gravity;

void xui_init();
void xui_release();
void xui_lcd_init();
void xui_lcd_backlight_level(VMUINT32 ulValue);

void xui_touch_init();

VMINT xui_view_get_x(xui_view view);
VMINT xui_view_get_y(xui_view view);
VMINT xui_view_get_width(xui_view view);
VMINT xui_view_get_height(xui_view view);

void xui_view_set_visibility(xui_view view, VMBOOL visibility);
void xui_view_set_x(xui_view view, VMINT value);
void xui_view_set_y(xui_view view, VMINT value);
void xui_view_set_width(xui_view view, VMINT value);
void xui_view_set_height(xui_view view, VMINT value);
void xui_view_set_background_color(xui_view view,
	vm_graphic_color_argb_t color);
void xui_view_set_touch_event_callback(xui_view view, touch_event_callback callback);

xui_text_view xui_init_text_view();
void xui_text_view_set_text(xui_text_view view, VMSTR str);
void xui_text_view_set_text_wide(xui_text_view view, VMWSTR wstr);
void xui_text_view_set_text_color(xui_text_view view,
	vm_graphic_color_argb_t color);
void xui_text_view_set_gravity(xui_text_view view, xui_gravity gravity);
void xui_free_text_view(xui_text_view view);

xui_page xui_init_page(xui_view * views, VMINT view_count);
void xui_page_set_background_color(xui_page page,
	vm_graphic_color_argb_t color);
void xui_free_page(xui_page page);

// void xui_set_visibility(xui_view * view, VMCHAR visibility);
// todo: need validate function ?

//void xui_page_start(xui_page * page);
//void xui_page_back();
void xui_validate(xui_page page); // todo: frame handler?
#endif
