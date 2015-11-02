//
#include "vmtype.h" 
#include "vmboard.h"
#include "vmsystem.h"
#include "vmlog.h" 
#include "vmcmd.h" 
#include "vmdcl.h"
#include "vmdcl_gpio.h"
#include "vmthread.h"

#include "ResID.h"
#include "linkit_one_battery.h"


/* ---------------------------------------------------------------------------
* global variables
* ------------------------------------------------------------------------ */

#define COMMAND_PORT  1000
VM_DCL_HANDLE gpio_handle_d2 = VM_DCL_HANDLE_INVALID; 

/* ---------------------------------------------------------------------------
* local variables
* ------------------------------------------------------------------------ */
/*
* system events 
*/
void handle_sysevt(VMINT message, VMINT param);

/*
* entry
*/
void vm_main(void) 
{
    /* register system events handler */
    vm_pmng_register_system_event_callback(handle_sysevt);
}

void pin_d2_init(void)
{
    vm_log_info("Init D2 PIN, need connect a LED to LinkIt ONE");
    gpio_handle_d2 = vm_dcl_open(VM_DCL_GPIO, VM_PIN_D2);
    vm_dcl_control(gpio_handle_d2, VM_DCL_GPIO_COMMAND_SET_MODE_0, NULL);
    vm_dcl_control(gpio_handle_d2,VM_DCL_GPIO_COMMAND_SET_DIRECTION_OUT, NULL);
    vm_dcl_control(gpio_handle_d2,VM_DCL_GPIO_COMMAND_WRITE_HIGH, NULL);
}

void pin_d2_set(VMBOOL is_high)
{
    if(is_high)
        vm_dcl_control(gpio_handle_d2,VM_DCL_GPIO_COMMAND_WRITE_HIGH, NULL);
    else
        vm_dcl_control(gpio_handle_d2,VM_DCL_GPIO_COMMAND_WRITE_LOW, NULL);
}

VMINT32 blinking_thread(VM_THREAD_HANDLE thread_handle, void* user_data)
{
    VM_DCL_HANDLE gpio_handle = VM_DCL_HANDLE_INVALID; 
    
    VMINT i;

    //open gpio PIN D0
    gpio_handle = vm_dcl_open(VM_DCL_GPIO, VM_PIN_D0);

    if (gpio_handle != VM_DCL_HANDLE_INVALID)
    {
        //set pin mode to MODE_0.
        vm_dcl_control(gpio_handle, VM_DCL_GPIO_COMMAND_SET_MODE_0, NULL);

        //set pin direction to OUTPUT.
        vm_dcl_control(gpio_handle,VM_DCL_GPIO_COMMAND_SET_DIRECTION_OUT, NULL);

        for (i = 0; i < 100000; i++)
        {
            if (i % 2 == 0)
            {
                //set pin7 to HIGH.
                vm_dcl_control(gpio_handle,VM_DCL_GPIO_COMMAND_WRITE_HIGH, NULL);  
            }
            else
            {
                //set pin7 to LOW.
                vm_dcl_control(gpio_handle,VM_DCL_GPIO_COMMAND_WRITE_LOW, NULL);  
            }
            vm_thread_sleep(1000);
        }

        //close gpio PIN D0, LED blinking is over.
        vm_dcl_close(gpio_handle);
    }

    return 0;
}


void command_callback(vm_cmd_command_t *param, void *user_data)
{
    vm_log_info("cmd = %s", (char*)param->command_buffer);
    if(strcmp("HIGH",(char*)param->command_buffer) == 0)
    {
        //Do something when receive command: AT+[1000]HIGH
        pin_d2_set(TRUE);
    }
    else if(strcmp("LOW",(char*)param->command_buffer) == 0)
    {
        //Do something when receive command: AT+[1000]LOW
        pin_d2_set(FALSE);
    } 		

    return;
}


void handle_sysevt(VMINT message, VMINT param) 
{
    switch (message) 
    {
    case VM_EVENT_CREATE:
        vm_log_info("Open AT Port & Reg Callback");
        vm_cmd_open_port(COMMAND_PORT, command_callback, NULL);
        pin_d2_init();

        //Create sub thread for Blink PIN D0 in loop
        //Otherwise you need to blink them in a timer callback, never blocking main thread.
        vm_thread_create(blinking_thread, NULL,255);
       
        break;

    case VM_EVENT_QUIT:
        vm_log_info("Exit and Close AT Port");
        vm_cmd_close_port(COMMAND_PORT);
        break;
    }
}

