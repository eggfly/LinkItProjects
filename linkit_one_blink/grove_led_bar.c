#include "grove_led_bar.h"
#include "vmboard.h"
#include "vmdcl.h"
#include "vmdcl_gpio.h"
#include "vmthread.h"

#define write_high(handle)	vm_dcl_control((handle), VM_DCL_GPIO_COMMAND_WRITE_HIGH, NULL)
#define write_low(handle)	vm_dcl_control((handle), VM_DCL_GPIO_COMMAND_WRITE_LOW, NULL)
#define read(handle, data)	vm_dcl_control((handle), VM_DCL_GPIO_COMMAND_READ, (void*) &(data))

static VM_DCL_HANDLE gpio_handle_data = VM_DCL_HANDLE_INVALID;
static VM_DCL_HANDLE gpio_handle_clock = VM_DCL_HANDLE_INVALID;

static void _init() {
	if (gpio_handle_data == VM_DCL_HANDLE_INVALID) {
		gpio_handle_data = vm_dcl_open(VM_DCL_GPIO, PIN_DATA);
	}
	if (gpio_handle_clock == VM_DCL_HANDLE_INVALID) {
		gpio_handle_clock = vm_dcl_open(VM_DCL_GPIO, PIN_CLOCK);
	}
	if (gpio_handle_data != VM_DCL_HANDLE_INVALID
			&& gpio_handle_clock != VM_DCL_HANDLE_INVALID) {
		/* Sets the pin mode to MODE_0 */
		vm_dcl_control(gpio_handle_data, VM_DCL_GPIO_COMMAND_SET_MODE_0, NULL);
		vm_dcl_control(gpio_handle_clock, VM_DCL_GPIO_COMMAND_SET_MODE_0, NULL);

		/* Sets the pin direction to OUTPUT */
		vm_dcl_control(gpio_handle_data, VM_DCL_GPIO_COMMAND_SET_DIRECTION_OUT,
		NULL);
		vm_dcl_control(gpio_handle_clock, VM_DCL_GPIO_COMMAND_SET_DIRECTION_OUT,
		NULL);
	}
}

static void _try_init() {
	if (gpio_handle_data == VM_DCL_HANDLE_INVALID
			|| gpio_handle_clock == VM_DCL_HANDLE_INVALID) {
		_init();
	}
}

static void _latch_data() {
	_try_init();
	vm_dcl_control(gpio_handle_data, VM_DCL_GPIO_COMMAND_WRITE_LOW, NULL);
	vm_thread_sleep(10);
	unsigned char i;
	for (i = 0; i < 4; i++) {
		write_high(gpio_handle_data);
		write_low(gpio_handle_data);
	}
}

static void _send_data(unsigned int data) {
	_try_init();
	unsigned char i;
	for (i = 0; i < 16; i++) {
		unsigned int state = (data & 0x8000);
		if (state) {
			write_high(gpio_handle_data);
		} else {
			write_low(gpio_handle_data);
		}

		vm_dcl_gpio_control_level_status_t d;
		read(gpio_handle_clock, d);

		if (d.level_status) {
			write_low(gpio_handle_clock);
		} else {
			write_high(gpio_handle_clock);
		}

		data <<= 1;
	}
}

void led_bar_set_state(led_bar_state_t * state) {
	_send_data(GLB_CMDMODE);
	unsigned char i;
	for (i = 0; i < 10; i++) {
		_send_data(state->brightness[i]);
	}
	// Two extra empty bits for padding the command to the correct length
	_send_data(0x00);
	_send_data(0x00);
	_latch_data();
}
