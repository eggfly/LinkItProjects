/*
 This example code is in public domain.

 This example code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 This example demonstrates how to configure the BT service for notifications,
 how to receive the notifications from a smart phone and how to analyse the
 notification content.

 This example starts with a 5 second timer first. Then it tries to start the BT
 service (vm_bt_cm_switch_on()), set the BT name (vm_bt_cm_set_host_name()),
 set the BT visibility (vm_bt_cm_set_visibility()), register the notification
 event callback (vm_bt_ns_register_notification_callback()) and the BT status
 change callback (vm_bt_ns_register_connection_status_change_callback()).
 
 If LinkIt Assist 2502 is connected with a smart phone and the the smart phone
 has the notification, such as new SMS message or a missed call, the LinkIt
 Assist 2502 will receive the notification in the callback.


 To use this example, modify macro the BT_NAME to change the device name to
 something you like, upload the example and pair the device with an iOS or 
 Android smart phone. For an iOS smart phone, download the "Mediatek SmartDevice"
 from the App store. For an Android smart phone, install the supplied APK file
 in SDK’s custom folder. If the smart phone has notification such as new
 SMS message or missed call, the device will receive the notification in the
 callback function.
 */

#include "vmtype.h" 
#include "vmsystem.h"
#include "vmlog.h" 
#include "vmtimer.h"
#include "vmbt_ns.h"
#include "vmbt_cm.h"
#include <string.h>

/* BT device name of this example */
#define BT_NAME "BT_NS"
/* BT service handle */
VMINT g_btcm_hdl = -1;
/* The timer ID */
VMINT g_tid_switch = -1;

/* BT service callback */
static void app_btcm_cb(VMUINT evt, void * param, void * user_data) {
	VMINT ret;
	vm_log_debug("BTCM app_btcm_cb evt[0x%x]", evt);
	switch (evt) {
	case VM_BT_CM_EVENT_ACTIVATE: {
		vm_bt_cm_device_info_t dev_info = { 0 };
		vm_bt_cm_activate_t *active = (vm_bt_cm_activate_t *) param;
		vm_log_debug("BTCM VM_BT_CM_EVENT_ACTIVATE hdl[%d] rst[%d]",
				active->handle, active->result);
		/* Sets the BT host name, so that other BT devices can search and see this device. */
		ret = vm_bt_cm_set_host_name((VMUINT8*) BT_NAME);
		ret = vm_bt_cm_get_host_device_info(&dev_info);
		vm_log_debug(
				"BTCM vm_bt_cm_get_host_device_info[%s][0x%02x:%02x:%02x:%02x:%02x:%02x]",
				dev_info.name, ((dev_info.device_address.nap & 0xff00) >> 8),
				(dev_info.device_address.nap & 0x00ff),
				(dev_info.device_address.uap),
				((dev_info.device_address.lap & 0xff0000) >> 16),
				((dev_info.device_address.lap & 0x00ff00) >> 8),
				(dev_info.device_address.lap & 0x0000ff));
		/* Sets the BT device as visible for other BT devices to scan for. */
		ret = vm_bt_cm_set_visibility(VM_BT_CM_VISIBILITY_ON);
		vm_log_debug("BTCM vm_bt_cm_set_visibility ret[%d]", ret);
		break;
	}

	case VM_BT_CM_EVENT_DEACTIVATE: {
		ret = vm_bt_cm_exit(g_btcm_hdl);
		g_btcm_hdl = -1;
		vm_log_debug("BTCM vm_bt_cm_exit [%d]", ret);
		break;
	}

	case VM_BT_CM_EVENT_INQUIRY_IND: {
		vm_bt_cm_inquiry_indication_t *ind =
				(vm_bt_cm_inquiry_indication_t *) param;
		vm_log_debug("BTCM VM_BT_CM_EVENT_INQUIRY_IND hdl[%d] num[%d]",
				ind->handle, ind->discovered_device_number);
		break;
	}

	case VM_BT_CM_EVENT_SET_VISIBILITY: {
		vm_bt_cm_device_info_t dev_info = { 0 };
		vm_bt_cm_set_visibility_t *visi = (vm_bt_cm_set_visibility_t *) param;
		vm_log_debug("BTCM VM_BT_CM_EVENT_SET_VISIBILITY hdl[%d] rst[%d]",
				visi->handle, visi->result);
		break;
	}

	default: {
		break;
	}
	}
}

/* Initializes the BT service and enables the BT if necessary. */
static void app_btcm_start(void) {
	VMINT ret;
	g_btcm_hdl = vm_bt_cm_init(app_btcm_cb,
			VM_BT_CM_EVENT_ACTIVATE | VM_BT_CM_EVENT_DEACTIVATE
					| VM_BT_CM_EVENT_INQUIRY_IND
					| VM_BT_CM_EVENT_INQUIRY_COMPLETE
					| VM_BT_CM_EVENT_SET_VISIBILITY | VM_BT_CM_EVENT_SET_NAME,
			NULL);
	vm_log_debug("BTCM vm_btcm_init [%d]", g_btcm_hdl);

	ret = vm_bt_cm_get_power_status();
	vm_log_debug("BTCM vm_btcm_get_power_status[%d]", ret);
	if (VM_BT_CM_POWER_OFF == ret) {
		/* Turns on the BT if not turned on yet */
		ret = vm_bt_cm_switch_on();
		vm_log_debug("BTCM vm_btcm_switch_on[%d]", ret);
	} else if (VM_BT_CM_POWER_ON == ret) { /* If the BT is already on */
		vm_bt_cm_device_info_t dev_info = { 0 };
		/* Sets the BT host name */
		ret = vm_bt_cm_set_host_name((VMUINT8*) BT_NAME);
		ret = vm_bt_cm_get_host_device_info(&dev_info);
		vm_log_debug(
				"BTCM vm_bt_cm_get_host_device_info[%s][0x%02x:%02x:%02x:%02x:%02x:%02x]",
				dev_info.name, ((dev_info.device_address.nap & 0xff00) >> 8),
				(dev_info.device_address.nap & 0x00ff),
				(dev_info.device_address.uap),
				((dev_info.device_address.lap & 0xff0000) >> 16),
				((dev_info.device_address.lap & 0x00ff00) >> 8),
				(dev_info.device_address.lap & 0x0000ff));
		/* Sets the BT visibility for other BT devices to scan for. */
		ret = vm_bt_cm_set_visibility(VM_BT_CM_VISIBILITY_ON);
		vm_log_debug("BTCM vm_bt_cm_set_visibility ret[%d]", ret);
	}
}

static void app_timer_cb(VMINT tid, void* user_data) {
	vm_log_debug("BTCM app_timer_cb[%d][%d]", tid, g_tid_switch);
	if (tid == g_tid_switch) {
		/* Starts the BT */
		app_btcm_start();
		/* Deletes the timer */
		vm_timer_delete_precise(g_tid_switch);
		g_tid_switch = -1;
	}
}

/* Handles the received notifications from the smart phone */
VMINT btns_test_notification_callback(
		vm_bt_ns_notification_data_t* parameter_ptr, void* user_data) {
	VMUINT8 content[100];
	/* Shows the BT address */
	vm_log_debug("status bt_addr.lap = %d", parameter_ptr->bt_address.lap);
	vm_log_debug("status bt_addr.lap = %d", parameter_ptr->bt_address.uap);
	vm_log_debug("status bt_addr.lap = %d", parameter_ptr->bt_address.nap);
	/* Shows the content */
	vm_chset_ucs2_to_ascii((VMSTR) content, 100,
			(VMWSTR) parameter_ptr->message->sender);
	vm_log_debug("refresh cb sender = %s", (VMSTR )content);
	/* Shows the sender's number */
	memset(content, 0, 100);
	vm_chset_ucs2_to_ascii((VMSTR) content, 100,
			(VMWSTR) parameter_ptr->message->sender_number);
	vm_log_debug("refresh cb sender number = %s", (VMSTR )content);
	/* Shows the content sender's content */
	memset(content, 0, 100);
	vm_chset_ucs2_to_ascii((VMSTR) content, 100,
			(VMWSTR) parameter_ptr->message->content);
	vm_log_debug("refresh cb sender content = %s", (VMSTR )content);
	memset(content, 0, 100);
	vm_chset_ucs2_to_ascii((VMSTR) content, 100,
			(VMWSTR) parameter_ptr->message->title);
	vm_log_debug("refresh cb sender title = %s", (VMSTR )content);
	vm_log_debug(" missed_call_count = %d",
			parameter_ptr->message->missed_call_count);
	vm_log_debug("refresh cb sender source = %d",
			parameter_ptr->message->source);
	vm_log_debug(" app id = %d", parameter_ptr->message->app_id);
	return 1;
}

/* Handles the notifications when the BT device is connected or disconnected */
VMINT btns_test_connection_status_change_callback(
		vm_bt_ns_connection_status_t* parameter_ptr, void* user_data) {
	vm_log_debug("status is_connected = %d", parameter_ptr->is_connected);
	vm_log_debug("status bt_addr.lap = %d", parameter_ptr->bt_address.lap);
	vm_log_debug("status bt_addr.lap = %d", parameter_ptr->bt_address.uap);
	vm_log_debug("status bt_addr.lap = %d", parameter_ptr->bt_address.nap);
	return 1;
}

/* The callback to be invoked by the system engine. */
void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		/* Waits 5 seconds to start the BT process as the device initialization needs time. */
		g_tid_switch = vm_timer_create_precise(5000, app_timer_cb, NULL);
		break;

	case VM_EVENT_QUIT:
		break;
	}
}

/* Entry point */
void vm_main(void) {
	VMINT i = 0;
	/* Registers system event handler */
	vm_pmng_register_system_event_callback(handle_sysevt);

	/* Registers the notification event callback */
	i = vm_bt_ns_register_notification_callback(btns_test_notification_callback,
			NULL);
	vm_log_debug("refresh_callback handle = %d", i);

	/* Registers the BT connection status change callback */
	i = vm_bt_ns_register_connection_status_change_callback(
			btns_test_connection_status_change_callback, NULL);
	vm_log_debug("connect status change handle = %d", i);
}

