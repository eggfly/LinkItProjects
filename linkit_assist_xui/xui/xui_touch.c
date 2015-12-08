#include "xui_private.h"
#include "tp_goodix_gt9xx.h"
#include "vmtouch.h"

void xui_touch_init(void) {
	tp_gt9xx_init();
}

void xui_handle_touch(xui_page page, VM_TOUCH_EVENT event, VMINT x, VMINT y) {
	struct _xui_page * p_page = (struct _xui_page *) page;
	int i;
	for (i = p_page->view_count - 1; i >= 0; i--) {
		void * raw_p_view = p_page->views[i];
		struct _xui_view * p_view = (struct _xui_view *) raw_p_view;
		if (x >= p_view->x && x <= p_view->x + p_view->width
			&& y >= p_view->y && y <= p_view->y + p_view->height) {
			// and more check like visibility?
			if (p_view->touch_event_cb) {
				p_view->touch_event_cb(raw_p_view, event, x, y);
				break;
			}
		}
	}
}
