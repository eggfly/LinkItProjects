#include "xui_private.h"
#include "vmmemory.h"
#include "vmgraphic.h"
#include "vmlog.h"

xui_page xui_init_page(void ** views, VMINT view_count) {
#ifdef XUI_DEBUG
	vm_log_debug("xui_init_page");
#endif
	xui_page result;
	result.page = vm_malloc(sizeof(struct _xui_page));
	struct _xui_page * page = (struct _xui_page *) result.page;
	vm_graphic_color_argb_t color;
	color.a = 0;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	page->background_color = color;
	page->views = views;
	page->view_count = view_count;
	return result;
}

void xui_free_page(xui_page page) {
	_xui_free(_xui_page, page, page)
}
