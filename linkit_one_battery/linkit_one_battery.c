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
 * local variables
 * ------------------------------------------------------------------------ */
/*
 * system events
 */
void handle_sysevt(VMINT message, VMINT param);

/*
 * entry
 */
void vm_main(void) {
	/* register system events handler */
	vm_pmng_register_system_event_callback(handle_sysevt);
}

void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		while (1) {
			VMINT battery_level = 0;
			battery_level = vm_pwr_get_battery_level();
			vm_log_info("[Battery GATT] battery_level:%d", battery_level);
			vm_thread_sleep(1000);
		}
		break;
	case VM_EVENT_QUIT:
		break;
	}
}

