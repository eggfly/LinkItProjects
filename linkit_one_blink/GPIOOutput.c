/*
 This example code is in public domain.

 This example code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 This example shows how to write the status to a digital pin.

 In this example, it will write to the BLINK_PIN_1 and switch the status between
 HIGH and LOW alternately every 1 second.
 */

#include "vmtype.h"
#include "vmboard.h"
#include "vmsystem.h"
#include "vmlog.h"
#include "vmdcl.h"
#include "vmdcl_gpio.h"
#include "vmthread.h"
#include "ResID.h"
#include "grove_led_bar.h"

void gpio_output_demo_handle_sysevt(VMINT message, VMINT param);

/* Entry point */
void vm_main(void) {
	/* Registers system event handler */
	vm_pmng_register_system_event_callback(gpio_output_demo_handle_sysevt);
}

VMINT32 gpio_output_demo_blinking_thread(VM_THREAD_HANDLE thread_handle,
		void* user_data) {
	vm_log_info("gpio_output_demo_blinking_thread - Start.");

	unsigned int i, j;
	led_bar_state_t state = { };
	while (1) {
		for (i = 0; i < 10; i++) {
			for (j = 0; j < 63; j++) {
				state.brightness[i] += 2;
				led_bar_set_state(&state);
			}
			vm_thread_sleep(100);
		}
		for (i = 0; i < 10; i++) {
			for (j = 0; j < 63; j++) {
				state.brightness[i] -= 2;
				led_bar_set_state(&state);
			}
			vm_thread_sleep(100);
		}
	}

	return 0;
}

void gpio_output_demo_handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		vm_log_info("Sample of GPIO output - Start.");
		/* Creates a sub-thread for blinking the pin BLINK_PIN_1 with the priority of 255. */
		vm_thread_create(gpio_output_demo_blinking_thread, NULL, 255);

		break;

	case VM_EVENT_QUIT:
		vm_log_info("Sample of GPIO output - End.");
		break;
	}
}

