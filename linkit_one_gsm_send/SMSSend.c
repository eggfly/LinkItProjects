/*
  This example code is in public domain.

  This example code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/*
  This example demonstrates how to send an SMS message using vm_gsm_sms_send().

  This example sends an SMS message to a recipient's mobile number, which can be
  modified in sms_send_sms_test(), for example, replacing the "+8610086" with a
  valid mobile phone number.

  This example requires a SIM card installed on the board.
 */
#include "vmtype.h" 
#include "vmboard.h"
#include "vmsystem.h"
#include "vmlog.h" 
#include "vmtimer.h"
#include "vmgsm_sms.h"
#include "vmgsm_sim.h"
#include "vmgsm_cell.h"
/* The callback of sending SMS, for checking if an SMS is sent successfully. */
void sms_send_callback(vm_gsm_sms_callback_t* callback_data){
    if(callback_data->action == VM_GSM_SMS_ACTION_SEND){
        vm_log_debug("send sms callback, result = %d", callback_data->result);
    }
}

/* Sends SMS */
void sms_send_sms_test(void){
    VMWCHAR content[100];
    VMWCHAR num[100];
    VMINT res = 0;
    VMINT number_count = 0;
    VMBOOL result;
    number_count = vm_gsm_sim_get_card_count();
    vm_log_debug("sms read card count %d", number_count);
    result = vm_gsm_sim_has_card();
    vm_log_debug("sms read sim card result %d", result);
    /* Sets SMS content */
    vm_chset_ascii_to_ucs2(content, 100*2, "hello, LINKIT SMS!");
    
    /* Sets recipient's mobile number */
    vm_chset_ascii_to_ucs2(num, 100*2, "+8618601065423");
    res = vm_gsm_sms_send(num, content, sms_send_callback, NULL);
    if(res != 0){
        vm_log_debug("sms send fail!");
    }
}

/* A precise timer callback. */
void sms_send_timer_callback(VM_TIMER_ID_PRECISE timer_id, void* user_data){
    /* Checks if the SMS message is ready. */
    if(vm_gsm_sms_is_sms_ready()){
        /* Sends SMS */
        sms_send_sms_test();
        
        /* Deletes the precise timer */
        vm_timer_delete_precise(timer_id);
    }
    else{
        vm_log_debug("sms is not ready");
    }
}

/* The callback to be invoked by the system engine. */
void handle_sysevt(VMINT message, VMINT param) {
    /* Defines a precise timer. */
    VM_TIMER_ID_PRECISE timer_id = 0;
    switch (message) {
        case VM_EVENT_CREATE:
        /* Creates a precise timer of 5 seconds to wait for the SMS message ready. */
        timer_id = vm_timer_create_precise(5000, sms_send_timer_callback, NULL);
        if(timer_id < 0){
            vm_log_debug("send sms timer fail");
        }
        else{
            vm_log_debug("send sms create timer success id = %d", timer_id);
        }
        break;

        case VM_EVENT_QUIT:
        vm_log_debug("sms send quit");
        break;
    }
}

/* Entry point */
void vm_main(void){
    /* Registers system event handler */
    vm_pmng_register_system_event_callback(handle_sysevt);
}
