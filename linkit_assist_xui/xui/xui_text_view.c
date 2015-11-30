/**
 * xui text view
 * TODO: calculate text size for inside layout gravity
 */

#include "xui_private.h"
#include "vmgraphic_font.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include "vmmemory.h"
#include "vmgraphic.h"
#include "vmlog.h"

static void _render_text_view(struct _xui_view *);
extern vm_graphic_frame_t g_frame[1];

xui_text_view xui_init_text_view() {
#ifdef XUI_DEBUG
	vm_log_debug("xui_init_text_view");
#endif
	xui_text_view result;
	result.view = vm_malloc(sizeof(struct _xui_text_view));
	_init_view(result.view, _render_text_view);
	struct _xui_text_view * p_view = (struct _xui_text_view *) result.view;
	// default text for debug
	xui_text_view_set_text((void *) p_view, (VMCHAR *) "xui_text_view");
	p_view->text_size = 18; // TODO
	// default text color is black
	vm_graphic_color_argb_t color;
	color.a = 255;
	color.r = color.g = color.b = 0;
	p_view->text_color = color;
	p_view->gravity = XUI_TOP_LEFT;
	return result;
}

/**
 * TODO: trim long text on draw
 */
static void _render_text_view(struct _xui_view * view) {
#ifdef XUI_DEBUG
	vm_log_debug("_render_text_view");
#endif
	struct _xui_text_view * text_view = (struct _xui_text_view*) view;
	vm_graphic_set_color(text_view->text_color);
	vm_graphic_set_font_size(text_view->text_size);
	vm_graphic_draw_text(&g_frame[0], text_view->x, text_view->y,
			text_view->text);
}

void xui_text_view_set_text_color(void * view, vm_graphic_color_argb_t color) {
	struct _xui_text_view * p_view = (struct _xui_text_view *) view;
	p_view->text_color = color;
}

void xui_free_text_view(xui_text_view view) {
	_xui_free(_xui_text_view, view, view)
}

void xui_text_view_set_text(void * view, VMSTR str) {
	struct _xui_text_view * p_view = (struct _xui_text_view *) view;
	vm_chset_ascii_to_ucs2(p_view->text, XUI_MAX_CHAR_LENGTH, str);
}

void xui_text_view_set_text_wide(void * view, VMWSTR wstr) {
	struct _xui_text_view * p_view = (struct _xui_text_view *) view;
	vm_wstr_safe_wstrcpy(p_view->text, XUI_MAX_CHAR_LENGTH, wstr);
}

void xui_text_view_set_gravity(void *view, xui_gravity gravity) {
	struct _xui_text_view * p_view = (struct _xui_text_view *) view;
	p_view->gravity = gravity;
}
