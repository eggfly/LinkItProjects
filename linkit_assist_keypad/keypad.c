/*
 This example code is in public domain.

 This example code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 This example shows how to receive keypad event and keycode from the board.

 This example use vm_keypad_register_event_callback() to register keypad handler.
 when operate the key on the board,the key event and keycode will be sended to application
 through this handler. for LINKIT_ONE,background applications will receive key event.
 for LinkIt Assist 2502A or board has LCD,only forground application will receive key event.

 Just run this application,and see the catch log or monitor log or lcd.for LinkIt Assist 2502A,if K0 is pressed,
 this log is "key event=2,key code=0"
 */
#include "vmtype.h" 
#include "vmlog.h"
#include "vmsystem.h"
#include "vmkeypad.h"
#include "vmgraphic.h"
#include "vmgraphic_font.h"
#include "vmdcl.h"
#include "vmdcl_pwm.h"
#include "vmdcl_gpio.h"
#include "vmdcl_kbd.h"
#include "vmchset.h"
#include "stdio.h"

VMCHAR g_buff[256];

vm_graphic_frame_t g_frame;
const vm_graphic_frame_t* g_frame_blt_group[1];
static VMUINT8* g_font_pool;

void key_init(void) {
	VM_DCL_HANDLE kbd_handle;
	VM_DCL_HANDLE gpio_handle;
	vm_dcl_kbd_control_pin_t kbdmap;

	gpio_handle = vm_dcl_open(VM_DCL_GPIO, 16);
	vm_dcl_control(gpio_handle, VM_DCL_GPIO_COMMAND_SET_MODE_1, NULL);
	vm_dcl_close(gpio_handle);

	gpio_handle = vm_dcl_open(VM_DCL_GPIO, 21);
	vm_dcl_control(gpio_handle, VM_DCL_GPIO_COMMAND_SET_MODE_1, NULL);
	vm_dcl_close(gpio_handle);

	gpio_handle = vm_dcl_open(VM_DCL_GPIO, 13);
	vm_dcl_control(gpio_handle, VM_DCL_GPIO_COMMAND_SET_MODE_1, NULL);
	vm_dcl_close(gpio_handle);

	gpio_handle = vm_dcl_open(VM_DCL_GPIO, 19);
	vm_dcl_control(gpio_handle, VM_DCL_GPIO_COMMAND_SET_MODE_1, NULL);
	vm_dcl_close(gpio_handle);

	kbd_handle = vm_dcl_open(VM_DCL_KBD, 0);
	kbdmap.col_map = 0x09;
	kbdmap.row_map = 0x05;
	vm_dcl_control(kbd_handle, VM_DCL_KBD_COMMAND_CONFIG_PIN,
			(void *) (&kbdmap));

	// kbdmap.col_map = 0x04;
	// kbdmap.row_map =0x03;
	// vm_dcl_control(kbd_handle,VM_DCL_KBD_COMMAND_CONFIG_PIN, (void *)(&kbdmap));

	vm_dcl_close(kbd_handle);
}

#if defined(__HDK_LINKIT_ASSIST_2502__)
void lcd_backlight_level(VMUINT32 ulValue) {
	VM_DCL_HANDLE pwm_handle;
	vm_dcl_pwm_set_clock_t pwm_clock;
	vm_dcl_pwm_set_counter_threshold_t pwm_config_adv;
	vm_dcl_config_pin_mode(VM_PIN_P1, VM_DCL_PIN_MODE_PWM);
	pwm_handle = vm_dcl_open(PIN2PWM(VM_PIN_P1),vm_dcl_get_owner_id());
	vm_dcl_control(pwm_handle,VM_PWM_CMD_START,0);
	pwm_config_adv.counter = 100;
	pwm_config_adv.threshold = ulValue;
	pwm_clock.source_clock = 0;
	pwm_clock.source_clock_division =3;
	vm_dcl_control(pwm_handle,VM_PWM_CMD_SET_CLOCK,(void *)(&pwm_clock));
	vm_dcl_control(pwm_handle,VM_PWM_CMD_SET_COUNTER_AND_THRESHOLD,(void *)(&pwm_config_adv));
	vm_dcl_close(pwm_handle);
}
#endif //defined(__HDK_LINKIT_ASSIST_2502__)

static void font_init(void) {
	VM_RESULT result;
	VMUINT32 pool_size;

	result = vm_graphic_get_font_pool_size(0, 0, 0, &pool_size);
	if (VM_IS_SUCCEEDED(result)) {
		g_font_pool = (VMUINT8*) vm_malloc(pool_size);
		if (NULL != g_font_pool) {
			vm_graphic_init_font_pool(g_font_pool, pool_size);
		}
		{
			vm_log_info("allocate font pool memory failed");
		}
	} else {
		vm_log_info("get font pool size failed, result:%d", result);
	}
}

void log_init(void) {
	vm_graphic_color_argb_t color; /* use to set screen and text color */
	vm_graphic_point_t frame_position[1] = { 0, 0 };

	g_frame.width = 240;
	g_frame.height = 240;
	g_frame.color_format = VM_GRAPHIC_COLOR_FORMAT_16_BIT;
	g_frame.buffer = (VMUINT8*) vm_malloc_dma(
			g_frame.width * g_frame.height * 2);
	g_frame.buffer_length = (g_frame.width * g_frame.height * 2);
	g_frame_blt_group[0] = &g_frame;

	/* set color and draw bg*/
	color.a = 255;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	vm_graphic_set_color(color);
	vm_graphic_draw_solid_rectangle(&g_frame, 0, 0, 240, 240);
#if defined(__HDK_LINKIT_ASSIST_2502__)    
	vm_graphic_blt_frame(g_frame_blt_group, frame_position, 1);
#endif //#if defined(__HDK_LINKIT_ASSIST_2502__)    
}

void log_info(VMINT line, VMSTR str) {
	VMWCHAR s[260]; /* string's buffer */
	VMUINT32 size;
	vm_graphic_color_argb_t color; /* use to set screen and text color */
	vm_graphic_point_t frame_position[1] = { 0, 0 };

	vm_chset_ascii_to_ucs2(s, 260, str);
	/* set color and draw bg*/
	color.a = 0;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	vm_graphic_set_color(color);
	vm_graphic_draw_solid_rectangle(&g_frame, 0, line * 20, 240, 20);

	/* set color and draw text*/
	color.a = 0;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	vm_graphic_set_color(color);
	vm_graphic_set_font_size(18);
	vm_graphic_draw_text(&g_frame, 0, line * 20, s);

#if defined(__HDK_LINKIT_ASSIST_2502__)
	/* flush the screen with text data */
	vm_graphic_blt_frame(g_frame_blt_group, frame_position, 1);
#endif //#if defined(__HDK_LINKIT_ASSIST_2502__)    
}

void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		break;
	case VM_EVENT_PAINT:
		log_init();
		log_info(1, "open application success");
		break;
	case VM_EVENT_QUIT:
		break;
	}
}
VMINT handle_keypad_event(VM_KEYPAD_EVENT event, VMINT code) {
	/* output log to monitor or catcher */
	vm_log_info("key event=%d,key code=%d", event, code); /* event value refer to VM_KEYPAD_EVENT */
	sprintf(g_buff, "key event = %d, code=%d", event, code);
	log_info(3, g_buff); /* output log to LCD if have */
	return 0;
}

void vm_main(void) {
	/* register system events handler */
#if defined(__HDK_LINKIT_ASSIST_2502__)    
	lcd_st7789s_init();
	lcd_backlight_level(100);
#endif
	vm_pmng_register_system_event_callback(handle_sysevt);
	vm_log_info("register keypad handler");
	vm_keypad_register_event_callback(handle_keypad_event);
	font_init();
	key_init();
}
