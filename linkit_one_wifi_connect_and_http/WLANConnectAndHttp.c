/**
 * Send HTTP request to get DoubanFM's JSON via WLAN
 *
 * This sample connects to HTTP(no secure) to retrieve content and print to vm_log.
 * It calls the API vm_https_register_context_and_callback() to register the callback functions,
 * then set the channel by vm_https_set_channel(), after the channel is established,
 * it will send out the request by vm_https_send_request() and read the response by
 * vm_https_read_content().
 */
#include <string.h>
#include "vmtype.h" 
#include "vmboard.h"
#include "vmsystem.h"
#include "vmlog.h" 
#include "vmcmd.h"
#include "vmdcl.h"
#include "vmdcl_gpio.h"
#include "vmthread.h"
#include "vmwlan.h"

#include "ResID.h"
#include "WLANConnectAndHttp.h"
#include "vmhttps.h"
#include "vmtimer.h"

#include "douban.h"

/* Configurable macros */

#define VMHTTPS_TEST_DELAY 10000    /* delay time after initial WLAN */
#define VMHTTPS_TEST_URL "http://douban.fm/j/mine/playlist?type=n&channel=1&from=mainsite"

#define AP_SSID "AndroidAP1"
#define AP_PASSWORD "adminadmin"
//#define AP_SSID "360wifi-PSSGZF-2.4G"
//#define AP_PASSWORD "homewifi"

#define AP_AUTHORIZE_MODE   VM_WLAN_AUTHORIZE_MODE_WPA2_ONLY_PSK

VMUINT8 g_channel_id;
VMINT g_read_seg_num;

// json
#define JSON_MAX_LENGTH (4096)

char json[JSON_MAX_LENGTH] = { };
int json_pos = 0;

static void append_json(char *buf, int length) {
	vm_log_info("length: %d", length);
	if (length >= 0 && json_pos + length <= JSON_MAX_LENGTH) {
		memcpy(json + json_pos, buf, length);
		json_pos += length;
	}
}

static void wlan_connect_callback(void *user_data,
		vm_wlan_connect_result_t *connect_result) {
	if (VM_WLAN_SUCCESS == connect_result->result) {
		vm_log_info("Connect to AP successfully");
	} else {
		vm_log_info("Connect to AP failed!");
	}
	vm_log_info("AP info shows below:");
	vm_log_info("connect result:%d", connect_result->result);
	vm_log_info("connect cause:%d", connect_result->cause);
	vm_log_info("connect bssid:%s", connect_result->bssid);
	vm_log_info("connect ssid:%s", connect_result->ssid);
	vm_log_info("connect init_by_socket:%d", connect_result->init_by_socket);
	vm_log_info("connect ssid_length:%d", connect_result->ssid_length);
	vm_log_info("connect channel_number:%d", connect_result->channel_number);
	vm_log_info("connect profile_id:%d", connect_result->profile_id);
	vm_log_info("connect authorize_type:%d", connect_result->authorize_type);
	vm_log_info("connect eap_peap_authorize_type:%d",
			connect_result->eap_peap_authorize_type);
	vm_log_info("connect eap_ttls_authorize_type:%d",
			connect_result->eap_ttls_authorize_type);
	vm_log_info("connect connection_type:%d", connect_result->connection_type);
	vm_log_info("connect encry_mode:%d", connect_result->encry_mode);
	vm_log_info("connect passphrase:%s", connect_result->passphrase);
	vm_log_info("connect passphrase_length:%d",
			connect_result->passphrase_length);
	vm_log_info("connect need_login:%d", connect_result->need_login);
	vm_log_info("connect cancel_by_disconnect:%d",
			connect_result->cancel_by_disconnect);
	vm_log_info("connect rssi:%d", connect_result->rssi);
}

static void wlan_connect(void) {
	vm_wlan_ap_info_t ap_info;
	VM_RESULT result;

	strcpy(ap_info.ssid, AP_SSID);
	strcpy(ap_info.password, AP_PASSWORD);
	ap_info.authorize_mode = AP_AUTHORIZE_MODE;
	result = vm_wlan_connect(&ap_info, wlan_connect_callback, NULL);
	vm_log_info("wlan_connect, return result:%d", result);
}

static void wlan_callback(void* user_danta, VM_WLAN_REQUEST_RESULT result_type) {
	vm_log_debug("wlan result:%d", result_type);
	if (VM_WLAN_REQUEST_DONE == result_type) {
		wlan_connect();
	}
}

static void wlan_mode_set(void) {
	VM_RESULT result;
	result = vm_wlan_mode_set(VM_WLAN_MODE_STA, wlan_callback, NULL);
	vm_log_info("mode set return value:%d", result);
}

VMINT32 wlan_init_thread(VM_THREAD_HANDLE thread_handle, void* user_data) {
	wlan_mode_set();
	return 0;
}

static void http_send_request_set_channel_rsp_cb(VMUINT32 req_id,
		VMUINT8 channel_id, VM_HTTPS_RESULT result) {
	VMINT ret = -1;

	ret = vm_https_send_request(0, /* Request ID */
	VM_HTTPS_METHOD_GET, /* HTTP Method Constant */
	VM_HTTPS_OPTION_NO_CACHE, /* HTTP request options */
	VM_HTTPS_DATA_TYPE_BUFFER, /* Reply type (wps_data_type_enum) */
	100, /* bytes of data to be sent in reply at a time. If data is more that this, multiple response would be there */
	(VMUINT8 *) VMHTTPS_TEST_URL, /* The request URL */
	strlen(VMHTTPS_TEST_URL), /* The request URL length */
	NULL, /* The request header */
	0, /* The request header length */
	NULL, 0);

	if (ret != 0) {
		vm_https_unset_channel(channel_id);
	}
}

static void http_unset_channel_rsp_cb(VMUINT8 channel_id,
		VM_HTTPS_RESULT result) {
	vm_log_debug("http_unset_channel_rsp_cb()");
}
static void http_send_release_all_req_rsp_cb(VM_HTTPS_RESULT result) {
	vm_log_debug("http_send_release_all_req_rsp_cb()");
}
static void http_send_termination_ind_cb(void) {
	vm_log_debug("http_send_termination_ind_cb()");
}
static void http_send_read_request_rsp_cb(VMUINT16 request_id,
		VM_HTTPS_RESULT result,
		VMUINT16 status, VMINT32 cause, VM_HTTPS_PROTOCOL protocol,
		VMUINT32 content_length, VMBOOL more, VMSTR content_type,
		VMUINT8 content_type_len, VMSTR new_url, VMUINT32 new_url_len,
		VMSTR reply_header, VMUINT32 reply_header_len,
		VMSTR reply_segment, VMUINT32 reply_segment_len) {
	VMINT ret = -1;
	vm_log_debug("http_send_request_rsp_cb()");
	if (result != 0) {
		vm_https_cancel(request_id);
		vm_https_unset_channel(g_channel_id);
	} else {
		// eggfly
		append_json(reply_segment, reply_segment_len);
		vm_log_debug("reply_content:%s", reply_segment);
		ret = vm_https_read_content(request_id, ++g_read_seg_num, 100);
		if (ret != 0) {
			vm_https_cancel(request_id);
			vm_https_unset_channel(g_channel_id);
		}
	}
}
static void http_send_read_read_content_rsp_cb(VMUINT16 request_id,
		VMUINT8 seq_num, VM_HTTPS_RESULT result, VMBOOL more,
		VMWSTR reply_segment,
		VMUINT32 reply_segment_len) {
	VMINT ret = -1;
	vm_log_debug("reply_content:%s", reply_segment);
	// eggfly
	append_json(reply_segment, reply_segment_len);
	if (more > 0) {
		ret = vm_https_read_content(request_id, /* Request ID */
		++g_read_seg_num, /* Sequence number (for debug purpose) */
		100); /* The suggested segment data length of replied data in the peer buffer of
		 response. 0 means use reply_segment_len in MSG_ID_WPS_HTTP_REQ or
		 read_segment_length in previous request. */
		if (ret != 0) {
			vm_https_cancel(request_id);
			vm_https_unset_channel(g_channel_id);
		}
	} else {
		// eggfly
		vm_log_debug("strlen(): %d", strlen(json));
		char link[1024];
		parse_json(json, link);
		/* don't want to send more requests, so unset channel */
		vm_https_cancel(request_id);
		vm_https_unset_channel(g_channel_id);
		g_channel_id = 0;
		g_read_seg_num = 0;
	}
}
static void http_send_cancel_rsp_cb(VMUINT16 request_id, VM_HTTPS_RESULT result) {
	vm_log_debug("http_send_cancel_rsp_cb()");
}
static void http_send_status_query_rsp_cb(VMUINT8 status) {
	vm_log_debug("http_send_status_query_rsp_cb()");
}

static void http_send_request(VM_TIMER_ID_NON_PRECISE timer_id, void* user_data) {
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	VMINT ret = -1;
	VMINT apn = VM_BEARER_DATA_ACCOUNT_TYPE_WLAN;
	vm_https_callbacks_t callbacks = {
			http_send_request_set_channel_rsp_cb,
			http_unset_channel_rsp_cb,
			http_send_release_all_req_rsp_cb,
			http_send_termination_ind_cb,
			http_send_read_request_rsp_cb,
			http_send_read_read_content_rsp_cb,
			http_send_cancel_rsp_cb,
			http_send_status_query_rsp_cb };
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/

	do {
		vm_timer_delete_non_precise(timer_id);
		ret = vm_https_register_context_and_callback(apn, &callbacks);

		if (ret != 0) {
			break;
		}

		/* set network profile information */
		ret = vm_https_set_channel(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	} while (0);

}

void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		vm_thread_create(wlan_init_thread, NULL, 255);
		vm_timer_create_non_precise(VMHTTPS_TEST_DELAY, http_send_request,
		NULL);
		break;

	case VM_EVENT_QUIT:
		break;
	}
}

void vm_main(void) {
	vm_pmng_register_system_event_callback(handle_sysevt);
}
