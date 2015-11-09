/*
  This example code is in public domain.

  This example code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/*
 This example demonstrates how to use the vm_pwr_reboot() API.
 
 Calling this API will reboot the LinkIt Assist 2502 board.
 */
#include "vmtype.h" 
#include "vmsystem.h"
#include "vmlog.h" 
#include "vmpwr.h"
#include "vmtimer.h"

void reboot_non_precise_timer_callback(VM_TIMER_ID_NON_PRECISE timer_id, void* user_data){
    vm_log_debug("reboot again");
    /* Calls the reboot API */
    vm_pwr_reboot();
}

void reboot_test_main(void){
    VM_TIMER_ID_NON_PRECISE timer_id = 0;
    /* Waits for 10 seconds for the board to initialize itself. */
    timer_id = vm_timer_create_non_precise(10000, (vm_timer_non_precise_callback)reboot_non_precise_timer_callback, NULL);
    if(timer_id < 0){
        vm_log_debug("reboot create non-precise timer fail id=%d", timer_id);
    }
}

/* The callback to be invoked by the system engine. */
void handle_sysevt(VMINT message, VMINT param) {
    switch (message) {
        case VM_EVENT_CREATE:
        vm_log_info("reboot test create");
        reboot_test_main();
        break;

        case VM_EVENT_QUIT:
        vm_log_info("reboot test quit");
        break;
    }
}

/* Entry point */
void vm_main(void){
    /* Registers system event handler */
    vm_pmng_register_system_event_callback(handle_sysevt);
}

