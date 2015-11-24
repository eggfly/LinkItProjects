#include "xui.h"

void xui_init_page(xui_page * page, xui_view_collection * views) {

}

static void _init_view(xui_view * view, xui_render_func func) {
	view->render = func;
	view->x = view->y = 0;
	view->width = view->height = 0;
	view->margin_left = view->margin_top = view->margin_right =
			view->margin_bottom = 0;
	view->padding_left = view->padding_top = view->padding_right =
			view->padding_bottom = 0;
	view->visibility = 1;
	view->background_color.a = 0;
}

static void _render_text_view(void * view) {
	xui_text_view * textview = (xui_text_view*) view;
	// TODO render
}

void xui_init_text_view(xui_text_view * text_view) {
	_init_view(&text_view->view, _render_text_view);
	text_view->text_size = 10; // todo
	vm_graphic_color_argb_t * color = &text_view->text_color;
	color->a = 255;
	color->r = color->g = color->b = 0;
}
