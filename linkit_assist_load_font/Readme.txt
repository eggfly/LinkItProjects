This example sets a Chinese font file and shows a Chinese text.
It initiates the resource to Chinese by API vm_res_init(), then load the text data by API vm_res_get_string(), after that sets the font file path in the phone storage by API vm_graphic_set_font(), at last, draw the text by API vm_graphic_draw_text().

Before run this application, you need to put a Chinese font file to EXTERNAL_FONT_PATH and set the EXTERNAL_FONT_SIZE which can be calculated by CheckMemSize.exe tool.

Modify the macro SCREEN_WIDTH and SCREEN_HEIGHT to fit your display device.
