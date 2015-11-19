/*
  This example code is in public domain.

  This example code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/*
  This example shows how to read a digital pin.
  
  In this example, it will read the status from the pin READ_PIN, and write the same
  status to the pin WRITE_PIN.
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
    #define READ_PIN   VM_PIN_D7
    #define WRITE_PIN  VM_PIN_D8
#elif defined(__HDK_LINKIT_ASSIST_2502__)
    #define READ_PIN   VM_PIN_P7
    #define WRITE_PIN  VM_PIN_P8
#else
    #error ¡° Board not support¡±
#endif

void gpio_input_demo_handle_sysevt(VMINT message, VMINT param);

/* Entry point */
void vm_main(void) 
{
    /* Registers system event handler */
    vm_pmng_register_system_event_callback(gpio_input_demo_handle_sysevt);
}

VMINT32 gpio_input_demo_read_thread(VM_THREAD_HANDLE thread_handle, void* user_data)
{
    VM_DCL_HANDLE gpio_handle_d7 = VM_DCL_HANDLE_INVALID;
    VM_DCL_HANDLE gpio_handle_d8 = VM_DCL_HANDLE_INVALID;
    vm_dcl_gpio_control_level_status_t data;
    VMINT i;

    vm_log_info("[GPIO]gpio_input_demo_read_thread - START.");

    /* Opens GPIO PIN READ_PIN */
    gpio_handle_d7 = vm_dcl_open(VM_DCL_GPIO, READ_PIN);

    if (gpio_handle_d7 != VM_DCL_HANDLE_INVALID)
    {
        /* Sets the pin mode to MODE_0 */
        vm_dcl_control(gpio_handle_d7, VM_DCL_GPIO_COMMAND_SET_MODE_0, NULL);

        /* Sets the pin direction to INPUT */
        vm_dcl_control(gpio_handle_d7, VM_DCL_GPIO_COMMAND_SET_DIRECTION_IN, NULL);

        /* Sets the pin to pull-low status */
        vm_dcl_control(gpio_handle_d7, VM_DCL_GPIO_COMMAND_ENABLE_PULL, NULL);
        vm_dcl_control(gpio_handle_d7, VM_DCL_GPIO_COMMAND_SET_PULL_LOW, NULL);
    }
    else
    {
        vm_log_info("Failed to open READ_PIN.");
        return 0;
    }

    /* Opens GPIO PIN WRITE_PIN */
    gpio_handle_d8 = vm_dcl_open(VM_DCL_GPIO, WRITE_PIN);
    if (gpio_handle_d8 != VM_DCL_HANDLE_INVALID)
    {
        /* Sets the pin mode to MODE_0 */
        vm_dcl_control(gpio_handle_d8, VM_DCL_GPIO_COMMAND_SET_MODE_0, NULL);

        /* Sets the pin direction to OUTPUT */
        vm_dcl_control(gpio_handle_d8, VM_DCL_GPIO_COMMAND_SET_DIRECTION_OUT, NULL);
    }
    else
    {
        vm_log_info("Failed to open WRITE_PIN.");
        return 0;
    }

    /* Reads the status from READ_PIN, and outputs the status to WRITE_PIN.  */
    for (i = 0; i < 120; i++)
    {
        /* Reads the status from READ_PIN */
        vm_dcl_control(gpio_handle_d7, VM_DCL_GPIO_COMMAND_READ, (void*)&data);
        vm_log_info("digital read READ_PIN, result = %d;", data.level_status);

        if (data.level_status == 1)
        {
            /* Sets WRITE_PIN to HIGH */
            vm_dcl_control(gpio_handle_d8, VM_DCL_GPIO_COMMAND_WRITE_HIGH, NULL);
        }
        else
        {
            /* Sets WRITE_PIN to LOW */
            vm_dcl_control(gpio_handle_d8, VM_DCL_GPIO_COMMAND_WRITE_LOW, NULL);
        }
        vm_thread_sleep(1000);
    }

    /* Closes GPIO READ_PIN & WRITE_PIN */
    vm_dcl_close(gpio_handle_d7);
    vm_dcl_close(gpio_handle_d8);

    vm_log_info("[GPIO]gpio_input_demo_read_thread - END.");

    return 0;
}

void gpio_input_demo_handle_sysevt(VMINT message, VMINT param)
{
    switch (message)
    {
    case VM_EVENT_CREATE:
        vm_log_info("Sample of GPIO input - Start.");
        /* Creates a sub-thread with the priority of 255 */
        vm_thread_create(gpio_input_demo_read_thread, NULL, 255);

        break;

    case VM_EVENT_QUIT:
        vm_log_info("Sample of GPIO input - End.");
        break;
    }
}

