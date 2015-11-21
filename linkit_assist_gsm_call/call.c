/*
 This example code is in public domain.

 This example code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 This example demonstrates how to make and receive call using vm_gsm_tel_call_actions().

 This example make voice call to a recipient's mobile phone number, which can be modified
 in vm_make_voice_call(). For example, replace XXXXXXXXX with a valid phone number in your country
 like 4085261899.

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
#include "vmgsm_tel.h"

/* tel action callback */
void vm_gsm_tel_call_actions_cb(vm_gsm_tel_call_actions_callback_data_t *data) {
	vm_log_info("vm_gsm_tel_call_actions_cb start");
	if (data == NULL) {
		vm_log_info("vm_gsm_tel_call_actions_cb return");
		return;
	}

	/* tel action type */
	switch (data->type_action) {
	case VM_GSM_TEL_CALL_ACTION_NO:
		vm_log_debug("No action.");
		break;
	case VM_GSM_TEL_CALL_ACTION_DIAL:
		vm_log_debug("Dial action");
		break;
	case VM_GSM_TEL_CALL_ACTION_ACCEPT:
		vm_log_debug("Accept action");
		break;
	case VM_GSM_TEL_CALL_ACTION_HOLD:
		vm_log_debug("Hold action");
		break;
	case VM_GSM_TEL_CALL_ACTION_END_SINGLE:
		vm_log_debug("End single action");
		break;
	}

	/* tel action result */
	switch (data->data_act_rsp.result_info.result) {
	case VM_GSM_TEL_OK:
		vm_log_debug("OK.");
		break;
	case VM_GSM_TEL_ERROR_UNSPECIFIED_ERROR:
		vm_log_debug("Unspecified error.");
		break;
	case VM_GSM_TEL_ERROR_CANCEL:
		vm_log_debug("Cancel.");
		break;
	case VM_GSM_TEL_ERROR_USER_ABORT:
		vm_log_debug("User abort.");
		break;
	case VM_GSM_TEL_ERROR_ACTION_NOT_ALLOWED:
		vm_log_debug("Action not allowed.");
		break;
	case VM_GSM_TEL_ERROR_NOT_ALLOWED_TO_DIAL:
		vm_log_debug("Not allowed to dial.");
		break;
	case VM_GSM_TEL_ERROR_NOT_ALLOWED_TO_ACCEPT:
		vm_log_debug("Not allowed to accept.");
		break;
	case VM_GSM_TEL_ERROR_NO_ACTIVE_CALL:
		vm_log_debug("No active call.");
		break;
	case VM_GSM_TEL_ERROR_CALLED_PARTY_BUSY:
		vm_log_debug("Called party busy.");
		break;
	case VM_GSM_TEL_ERROR_NETWORK_NOT_AVAILABLE:
		vm_log_debug("Network not available.");
		break;
	case VM_GSM_TEL_ERROR_CALLED_PARTY_NOT_ANSWERED:
		vm_log_debug("Called party not answered.");
		break;
	case VM_GSM_TEL_ERROR_SOS_NUMBER_ONLY:
		vm_log_debug("SOS number only.");
		break;
	case VM_GSM_TEL_ERROR_INVALID_NUMBER:
		vm_log_debug("Invalid number.");
		break;
	case VM_GSM_TEL_ERROR_EMPTY_NUMBER:
		vm_log_debug("Empty number.");
		break;
	case VM_GSM_TEL_ERROR_SOS_CALL_EXISTS:
		vm_log_debug("Exist SOS call.");
		break;
	case VM_GSM_TEL_ERROR_PREFERRED_MODE_PROHIBIT:
		vm_log_debug("Preferred mode prohibit.");
		break;
	case VM_GSM_TEL_ERROR_FLIGHT_MODE_PROHIBIT:
		vm_log_debug("Flight mode prohibit.");
		break;
	case VM_GSM_TEL_ERROR_LOW_BATTERY:
		vm_log_debug("Low battery.");
		break;
	case VM_GSM_TEL_ERROR_UCM_BUSY:
		vm_log_debug("Busy.");
		break;
	case VM_GSM_TEL_ERROR_VT_FALLBACK:
		vm_log_debug("VT fallback.");
		break;
	}

	vm_log_info("vm_gsm_tel_call_actions_cb end");
}

/* voice call events call back function */
static void vm_gsm_tel_call_listener_cb(vm_gsm_tel_call_listener_data_t* data) {
	// eggfly mod
//	vm_gsm_tel_set_output_device(VM_GSM_TEL_DEVICE_LOUDSPK);
//	vm_gsm_tel_set_volume(VM_AUDIO_VOLUME_6);

	vm_log_info("vm_gsm_tel_call_listener_cb start");
	if (data == NULL) {
		vm_log_info("vm_gsm_tel_call_listener_cb return");
		return;
	}

	/* External call incoming */
	if (data->call_type == VM_GSM_TEL_INDICATION_INCOMING_CALL) {
		static VMUINT16 i = 1;
		vm_gsm_tel_call_info_t* call_info = (vm_gsm_tel_call_info_t*) data->data;
		vm_log_debug("Incoming number is %s", (char* )call_info->num_uri);

		vm_gsm_tel_single_call_action_request_t req;
		// vm_gsm_tel_id_info_t tel_id;
		vm_gsm_tel_call_actions_data_t data;
		VM_RESULT ret;
		req.action_id.sim = VM_GSM_TEL_CALL_SIM_1;
		req.action_id.call_id = 1;
		req.action_id.group_id = 1;

		data.action = VM_GSM_TEL_CALL_ACTION_ACCEPT; /* set tel call action to accept calls */
		data.data_action = (void*) &req;
		data.user_data = (void*) NULL;
		data.callback = vm_gsm_tel_call_actions_cb;

		ret = vm_gsm_tel_call_actions(&data);
		if (VM_IS_SUCCEEDED(ret)) {
			vm_log_info("incoming success");
		} else {
			vm_log_info("incoming failed");
		}
	} else if (data->call_type == VM_GSM_TEL_INDICATION_OUTGOING_CALL) {
		vm_gsm_tel_call_info_t* call_info = (vm_gsm_tel_call_info_t*) data->data;
		vm_log_info("Outgoing, calling number is %s",
				(char* )call_info->num_uri);
	} else if (data->call_type == VM_GSM_TEL_INDICATION_CONNECTED) {
		vm_log_info((char* )"connect success");
	} else if (data->call_type == VM_GSM_TEL_INDICATION_CALL_ENDED) {
		vm_log_info((char* )"call end");
	}

	vm_log_info("vm_gsm_tel_call_listener_cb end");
}

/* make voice call */
void vm_make_voice_call() {
	char *num = "18601065423"; /* dial number*/
	VMINT number_count = 0;
	VM_GSM_SIM_ID sim_id;
	VMBOOL res;
	number_count = vm_gsm_sim_get_card_count();
	sim_id = vm_gsm_sim_get_active_sim_card();
	res = vm_gsm_sim_has_card();
	vm_log_debug(
			"sms read card count is %d, active sime card is %d, sim card result %d",
			number_count, sim_id, res);

	VM_RESULT result;
	vm_gsm_tel_call_actions_data_t vm_gsm_tel_call_action_data;
	vm_gsm_tel_dial_action_request_t vm_gsm_tel_dial_action_request;

	vm_gsm_tel_dial_action_request.sim = VM_GSM_TEL_CALL_SIM_1;
	vm_chset_ascii_to_ucs2((VMWSTR) vm_gsm_tel_dial_action_request.num_uri,
	VM_GSM_TEL_MAX_NUMBER_LENGTH, (VMSTR) num);
	vm_gsm_tel_dial_action_request.module_id = 0;
	vm_gsm_tel_dial_action_request.is_ip_dial = VM_FALSE;
	vm_gsm_tel_dial_action_request.phonebook_data = NULL;

	vm_gsm_tel_call_action_data.action = VM_GSM_TEL_CALL_ACTION_DIAL; /* set action to dial voice call */
	vm_gsm_tel_call_action_data.data_action =
			(void *) &vm_gsm_tel_dial_action_request;
	vm_gsm_tel_call_action_data.callback = vm_gsm_tel_call_actions_cb;

	result = vm_gsm_tel_call_actions(&vm_gsm_tel_call_action_data);
	if (VM_IS_SUCCEEDED(result))
		vm_log_info("make voice call success");
		else
		vm_log_info("make voice call failed");
}

/* The callback to be invoked by the system engine. */
void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		vm_log_info("make voice call");
		vm_make_voice_call();
		break;

	case VM_EVENT_QUIT:
		vm_log_debug("voice call quit");
		break;
	}
}

/* Entry point */
void vm_main(void) {
	/* Registers system event handler */
	vm_pmng_register_system_event_callback(handle_sysevt);

	vm_log_info("call back reg start");
	vm_gsm_tel_call_reg_listener(vm_gsm_tel_call_listener_cb); /* register listener call back function */
	vm_log_info("call back reg end");
}

