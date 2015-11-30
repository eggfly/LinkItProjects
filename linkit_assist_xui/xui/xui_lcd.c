#include "xui_private.h"
#include "lcd_sitronix_st7789s.h"
#include "vmlog.h"
#include "vmdcl.h"
#include "vmdcl_gpio.h"
#include "vmdcl_pwm.h"

#define BACKLIGHT_PIN VM_PIN_P1

void xui_lcd_st7789s_init() {
	lcd_st7789s_init();
}

void xui_lcd_backlight_level(VMUINT32 ulValue) {
	VM_DCL_HANDLE pwm_handle;
	vm_dcl_pwm_set_clock_t pwm_clock;
	vm_dcl_pwm_set_counter_threshold_t pwm_config_adv;

	vm_dcl_config_pin_mode(BACKLIGHT_PIN, VM_DCL_PIN_MODE_PWM);

	pwm_handle = vm_dcl_open(PIN2PWM(BACKLIGHT_PIN), vm_dcl_get_owner_id());
	vm_dcl_control(pwm_handle, VM_PWM_CMD_START, 0);
	pwm_config_adv.counter = 100;
	pwm_config_adv.threshold = ulValue;
	pwm_clock.source_clock = 0;
	pwm_clock.source_clock_division = 3;
	vm_dcl_control(pwm_handle, VM_PWM_CMD_SET_CLOCK, (void *) (&pwm_clock));
	vm_dcl_control(pwm_handle, VM_PWM_CMD_SET_COUNTER_AND_THRESHOLD,
			(void *) (&pwm_config_adv));
	vm_dcl_close(pwm_handle);
}
