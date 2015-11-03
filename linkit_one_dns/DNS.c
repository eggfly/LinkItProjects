/*
 This exmple code is in public domain.

 This sample code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 This example gets the IP address of labs.mediatek.com

 It calls the API vm_dns_get_host_by_name() to get the host address and print the
 result to log in callback function.

 Please config the APN information according to your SIM card first before run this example.
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

#include "ResID.h"
#include "DNS.h"
#include "vmdns.h"
#include "vmtimer.h"

/* Configurable macros */
#define CUST_APN   "uninet" /*"cmwap"*/         /* The APN of your test SIM */
#define USING_PROXY VM_TRUE         /* Whether your SIM uses proxy */
#define PROXY_ADDRESS   "10.0.0.172"    /* The proxy address */
#define PROXY_PORT  80              /* The proxy port */
#define HOST "labs.mediatek.com"    /* The host that needs to query the ip */
#define DNS_START_TIMER 60000   /* Start 60 seconds later after bootup */

/* ---------------------------------------------------------------------------
 * global variables
 * ------------------------------------------------------------------------ */
static VM_DNS_HANDLE g_handle;

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
static VM_RESULT get_host_callback(VM_DNS_HANDLE handle,
		vm_dns_result_t* result, void *user_data) {
	vm_log_info("dns result:%d.%d.%d.%d", (result->address[0]) & 0xFF,
			((result->address[0]) & 0xFF00) >> 8,
			((result->address[0]) & 0xFF0000) >> 16,
			((result->address[0]) & 0xFF000000) >> 24);
	return VM_SUCCESS;
}

void set_cust_apn(void) {
	vm_gsm_gprs_apn_info_t apn_info;

	memset(&apn_info, 0, sizeof(apn_info));
	strcpy(apn_info.apn, CUST_APN);
	strcpy(apn_info.proxy_address, PROXY_ADDRESS);
	apn_info.proxy_port = PROXY_PORT;
	apn_info.using_proxy = USING_PROXY;
	vm_gsm_gprs_set_customized_apn_info(&apn_info);
}

static void get_host(VM_TIMER_ID_NON_PRECISE timer_id, void* user_data) {
	vm_dns_result_t result;

	vm_timer_delete_non_precise(timer_id);
	g_handle = vm_dns_get_host_by_name(
			VM_BEARER_DATA_ACCOUNT_TYPE_GPRS_CUSTOMIZED_APN, HOST, &result,
			get_host_callback, NULL);

}

void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		vm_timer_create_non_precise(DNS_START_TIMER, get_host, NULL);
		break;

	case VM_EVENT_QUIT:
		break;
	}
}

