#include "xui_private.h"
#include "vmgraphic_font.h"
#include "vmchset.h"
#include "vmmemory.h"
#include "vmgraphic.h"
#include "vmlog.h"

static void _render_text_view(struct _xui_view *);
static VMWCHAR g_wstr[260]; // TODO: temp
extern vm_graphic_frame_t g_frame[1];

xui_text_view xui_init_text_view() {
#ifdef XUI_DEBUG
	vm_log_debug("xui_init_text_view");
#endif
	xui_text_view result;
	result.view = vm_malloc(sizeof(struct _xui_text_view));
	_init_view(result.view, _render_text_view);
	struct _xui_text_view * p_view = (struct _xui_text_view *) result.view;
	p_view->text_size = 18; // TODO
	vm_graphic_color_argb_t * color = &p_view->text_color;
	color->a = 255;
	color->r = color->g = color->b = 255; // TODO
	return result;
}

static void _render_text_view(struct _xui_view * view) {
#ifdef XUI_DEBUG
	vm_log_debug("_render_text_view");
#endif
	struct _xui_text_view * text_view = (struct _xui_text_view*) view;
	vm_graphic_set_color(text_view->text_color);
	vm_graphic_set_font_size(text_view->text_size);
	VMCHAR * str = (VMCHAR *) "text";
	vm_chset_ascii_to_ucs2(g_wstr, 260, str);
	text_view->text = g_wstr;
	vm_graphic_draw_text(&g_frame[0], text_view->x, text_view->y,
			text_view->text);
}

void xui_free_text_view(xui_text_view view) {
	_xui_free(_xui_text_view, view, view)
}
