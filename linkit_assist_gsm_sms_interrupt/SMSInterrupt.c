/*
  This example code is in public domain.

  This example code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/*
  This example demonstrates how to set up callback functions to handle the new
  SMS messages with vm_gsm_sms_set_interrupt_event_handler().

  This example registers a callback function to receive SMS event with
  vm_gsm_sms_set_interrupt_event_handler(), and prints out the new message
  content on the Monitor tool.

  This example requires a SIM card installed on the board.
*/
#include "vmtype.h" 
#include "vmsystem.h"
#include "vmlog.h" 
#include "vmgsm_sms.h"
#include "vmchset.h"
#include "vmtimer.h"
#include <stdio.h>
extern int strlen(char* s);

VMINT sms_new_message_interrupt_proc(vm_gsm_sms_event_t* event_data){
    vm_gsm_sms_event_new_sms_t * event_new_message_ptr;
    vm_gsm_sms_new_message_t * new_message_ptr = NULL;
    VM_RESULT res = 0;
    VMUINT8 content[100];
    /* Checks if this event is for new SMS message. */
    if(event_data->event_id == VM_GSM_SMS_EVENT_ID_SMS_NEW_MESSAGE){
        /* Gets the event info. */
        event_new_message_ptr = (vm_gsm_sms_event_new_sms_t *)event_data->event_info;

        /* Gets the message data. */
        new_message_ptr  =  event_new_message_ptr->message_data;

        /* Converts the message content to ASCII. */
        vm_chset_ucs2_to_ascii((VMSTR)content, 100, (VMWSTR)event_new_message_ptr->content);
        vm_log_debug("sms new message interrupt content = %s %d", (VMSTR)content, strlen(content));

        /* Prints the Phone Number and the SMS Center Number*/
        vm_log_debug("sms new message interrupt number = %s", (VMWSTR)new_message_ptr->number);
        vm_log_debug("sms new message interrupt center number = %s", (VMWSTR)new_message_ptr->sms_center_number);

        return 1;
    }
    else{
        vm_log_debug("sms new message interrupt number not NEW MESSAGE ID");
        return 0;
    }
}

/* To be called by the timer callback to set up notification for new SMS message. */
static void test_sms_set_interrupt_event_handler(void){
    VM_RESULT res;

    /* Sets up callback to handle the new SMS message. */
    res = vm_gsm_sms_set_interrupt_event_handler(VM_GSM_SMS_EVENT_ID_SMS_NEW_MESSAGE, sms_new_message_interrupt_proc, NULL);
    if(res != VM_GSM_SMS_RESULT_OK){
        vm_log_debug("set interrupt fail");
        return ;
    }
    vm_log_debug("Please send sms to this device");
}

/* A precise timer callback. */
void sms_interrupt_timer_callback(VM_TIMER_ID_PRECISE timer_id, void* user_data){
    /* Checks if the SMS message is ready. */
    if(vm_gsm_sms_is_sms_ready()){
        vm_log_debug("sms is ready");
        test_sms_set_interrupt_event_handler();
    }
    else{
        vm_log_debug("sms is not ready");
        test_sms_set_interrupt_event_handler();
    }

    /* Deletes the timer. */
    vm_timer_delete_precise(timer_id);
}

/* The callback to be invoked by the system engine. */
void handle_sysevt(VMINT message, VMINT param) {
	/* Defines a precise timer. */
    VM_TIMER_ID_PRECISE timer_id = 0;
    switch (message) {
        case VM_EVENT_CREATE:
        /* Creates a precise timer of 5 seconds with sms_interrupt_timer_callback(). */
        timer_id = vm_timer_create_precise(5000, (vm_timer_precise_callback)sms_interrupt_timer_callback, NULL);
        if(timer_id < 0){
            vm_log_debug("sms interrupt create timer fail");
        }
        else{
            vm_log_debug("sms interrupt create timer success id = %d", timer_id);
        }
        break;

        case VM_EVENT_QUIT:
        break;
    }
}

/* Entry point */
void vm_main(void){
    /* Registers system event handler */
    vm_pmng_register_system_event_callback(handle_sysevt);
}
