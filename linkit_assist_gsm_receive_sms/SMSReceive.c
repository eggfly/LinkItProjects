/*
  This example code is in public domain.

  This example code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/*
 This example demonstrates how to receive an SMS and how to retrieve the content
 and the sender's phone number of an SMS.

 It reads an SMS message using the vm_gsm_sms_read_message() and analyses the
 SMS information inside the sms_read_sms_sample_callback().

 This example requires a SIM card to be installed and an SMS is sent to the board.
 */
#include "vmtype.h" 
#include "vmboard.h"
#include "vmsystem.h"
#include "vmlog.h" 
#include "vmtimer.h"
#include "vmgsm_sms.h"
#include "vmchset.h"
#include "vmgsm_sim.h"
#include "vmgsm_cell.h"

/* Reads SMS callback */
void sms_read_sms_sample_callback(vm_gsm_sms_callback_t* callback_data){
    VMUINT8 content[100];
    VMUINT8 sms_number[50];
    vm_gsm_sms_read_message_data_callback_t* read_msg;
    /* Checks if it is an SMS read event */
    if(callback_data->action == VM_GSM_SMS_ACTION_READ){
        if(callback_data->cause == VM_GSM_SMS_CAUSE_NO_ERROR){
            if(!callback_data->action_data){
                return;
            }
            /* Analyses the data */
            read_msg = (vm_gsm_sms_read_message_data_callback_t*)callback_data->action_data;
            
            /* Gets the content of the SMS message */
            vm_chset_ucs2_to_ascii(content, 100, (VMWSTR)(read_msg->message_data->content_buffer));
            vm_log_debug("msg content is %s.",content);
            
            /* Gets the sender's phone number */
            vm_chset_ucs2_to_ascii(sms_number, 50, (VMWSTR)(read_msg->message_data->number));
            vm_log_debug("show msg number is %s", sms_number);
            
            /* Frees the memory allocated by the malloc() */
            vm_free(read_msg->message_data->content_buffer);
            vm_free(read_msg->message_data);
        }
        else{
            vm_log_debug("read msg failed");
        }
    }
}

/* Reads the SMS */
void sms_read_sms_sample(void){
    VMINT16 message_id;
    vm_gsm_sms_read_message_data_t* message_data = NULL;
    VMWCHAR* content_buff;
    VMINT res;
    VMINT number_count = 0;
    VMBOOL result;
    number_count = vm_gsm_sim_get_card_count();
    vm_log_debug("sms read card count %d", number_count);
    result = vm_gsm_sim_has_card();
    vm_log_debug("sms read sim card result %d", result);
    /* Gets the message ID of the first message in the SMS inbox */
    message_id = vm_gsm_sms_get_message_id(VM_GSM_SMS_BOX_INBOX, 0);
    if(message_id == -1){
        vm_log_debug("sms read message id error");
        return;
    }

    /* Allocates memory for the message data */
    message_data = (vm_gsm_sms_read_message_data_t*)vm_calloc(sizeof(vm_gsm_sms_read_message_data_t));
    if(message_data == NULL){
        vm_log_debug("sms read malloc message data fail");
        return;
    }
    
    /* Allocates memory for the content buffer of the message */
    content_buff = (VMWCHAR*)vm_calloc((500+1)*sizeof(VMWCHAR));
    if(content_buff == NULL){
        vm_free(message_data);
        vm_log_debug("sms read malloc content fail");
        return;

    }
    message_data->content_buffer = content_buff;
    message_data->content_buffer_size = 500;
    
    /* Reads the message */
    res = vm_gsm_sms_read_message(message_id, VM_TRUE, message_data, sms_read_sms_sample_callback, NULL);
    if(res != VM_GSM_SMS_RESULT_OK){
        vm_free(content_buff);
        vm_free(message_data);
        vm_log_debug("register read callback fail");
    }
}

/* A precise timer callback. */
void sms_read_sms_sample_timer_callback(VM_TIMER_ID_PRECISE timer_id, void* user_data){
    /* Checks if the SMS message is ready. */
    if(vm_gsm_sms_is_sms_ready()){
        /* Reads the SMS data */
        sms_read_sms_sample();
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

        /* Creates a precise timer of 10 seconds to wait for the SMS message ready. */
        timer_id = vm_timer_create_precise(10000, sms_read_sms_sample_timer_callback, NULL);
        if(timer_id < 0){
            vm_log_debug("sms read sms create timer fail");
        }
        else{
            vm_log_debug("sms read sms create timer success id = %d", timer_id);
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
