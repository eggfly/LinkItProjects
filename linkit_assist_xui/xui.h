#ifndef __XUI_H__
#define __XUI_H__

#include "vmtype.h"
#include "vmgraphic.h"

typedef struct {
	void * page;
} xui_page;

typedef struct {
	void * view;
} xui_view;

typedef struct {
	void * view;
} xui_text_view;

void xui_init(); // todo: frame handler
xui_text_view xui_init_text_view();
xui_page xui_init_page(void ** views, VMINT view_count);

// void xui_set_visibility(xui_view * view, VMCHAR visibility);
// todo: need validate function ?

//void xui_page_start(xui_page * page);
//void xui_page_back();
void xui_validate(xui_page page); // todo: frame handler?

#endif
