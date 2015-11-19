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

#if defined(__HDK_LINKIT_ONE_V1__)
#define BLINK_PIN_1   VM_PIN_D0
#define BLINK_PIN_2   VM_PIN_D1
#elif defined(__HDK_LINKIT_ASSIST_2502__)
#define BLINK_PIN_1   VM_PIN_P0
#define BLINK_PIN_2   VM_PIN_P1
#else
#error ¡° Board not support¡±
#endif

void gpio_output_demo_handle_sysevt(VMINT message, VMINT param);

/* Entry point */
void vm_main(void) {
	/* Registers system event handler */
	vm_pmng_register_system_event_callback(gpio_output_demo_handle_sysevt);
}

VMINT32 gpio_output_demo_blinking_thread(VM_THREAD_HANDLE thread_handle,
		void* user_data) {
	VM_DCL_HANDLE gpio_handle_1 = VM_DCL_HANDLE_INVALID;
	VM_DCL_HANDLE gpio_handle_2 = VM_DCL_HANDLE_INVALID;
	VMINT i;

	vm_log_info("gpio_output_demo_blinking_thread - Start.");

	/* Opens GPIO pin BLINK_PIN_1 */
	gpio_handle_1 = vm_dcl_open(VM_DCL_GPIO, BLINK_PIN_1);
	gpio_handle_2 = vm_dcl_open(VM_DCL_GPIO, BLINK_PIN_2);

	if (gpio_handle_1 != VM_DCL_HANDLE_INVALID
			&& gpio_handle_2 != VM_DCL_HANDLE_INVALID) {
		/* Sets the pin mode to MODE_0 */
		vm_dcl_control(gpio_handle_1, VM_DCL_GPIO_COMMAND_SET_MODE_0, NULL);
		vm_dcl_control(gpio_handle_2, VM_DCL_GPIO_COMMAND_SET_MODE_0, NULL);

		/* Sets the pin direction to OUTPUT */
		vm_dcl_control(gpio_handle_1, VM_DCL_GPIO_COMMAND_SET_DIRECTION_OUT,
		NULL);
		vm_dcl_control(gpio_handle_2, VM_DCL_GPIO_COMMAND_SET_DIRECTION_OUT,
		NULL);

		for (i = 0; i < 120; i++) {
			if (i % 2 == 0) {
				/* Sets BLINK_PIN_1 to HIGH */
				vm_dcl_control(gpio_handle_1, VM_DCL_GPIO_COMMAND_WRITE_HIGH,
				NULL);
				vm_dcl_control(gpio_handle_2, VM_DCL_GPIO_COMMAND_WRITE_HIGH,
				NULL);
			} else {
				/* Sets BLINK_PIN_1 to LOW */
				vm_dcl_control(gpio_handle_1, VM_DCL_GPIO_COMMAND_WRITE_LOW,
				NULL);
				vm_dcl_control(gpio_handle_2, VM_DCL_GPIO_COMMAND_WRITE_LOW,
				NULL);
			}
			vm_thread_sleep(1000);
		}

		/* Closes GPIO pin BLINK_PIN_1 */
		vm_dcl_close(gpio_handle_1);
		vm_dcl_close(gpio_handle_2);
	}
	vm_log_info("gpio_output_demo_blinking_thread - End.");

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

