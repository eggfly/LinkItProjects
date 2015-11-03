/*
  This example code is in public domain.

  This example code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/*
  This example shows how to get the current and the neighboring cell information.

  In this example, it will outputs the cell information in the Monitor tool.
  Before launching this example, a SIM card needs to be inserted and the GSM
  antenna needs to be attached to the GSM ANT. After launching the example,
  the AT commands can be used through the Monitor tool to control the flow as
  below:
  AT+[1000]Test01: opens the cell.
  AT+[1000]Test02: shows the current cell information.
  AT+[1000]Test03: shows the total number of the neighboring cells and all cell info.
  AT+[1000]Test04: stops the cell.
*/
#include "vmtype.h"
#include "vmsystem.h"
#include "vmcmd.h"
#include "vmlog.h"
#include "vmgsm_cell.h"

#define COMMAND_PORT  1000 /* AT command port */

vm_gsm_cell_info_t g_info; /* cell information data */

/* AT command callback, which will be called after the AT command is sent from the Monitor tool */
static void at_callback(vm_cmd_command_t* param, void* user_data)
{
  VMINT result;

  if(strcmp("Test01",(char*)param->command_buffer) == 0)
  {
     /* Opens the cell when receiving AT command: AT+[1000]Test01 */
     result = vm_gsm_cell_open();
     vm_log_info("open result = %d",result);
  }
  else if(strcmp("Test02",(char*)param->command_buffer) == 0)
  {
    /* Gets the current cell info when receiving AT command: AT+[1000]Test02 */
    vm_gsm_cell_get_current_cell_info(&g_info);
    vm_log_info("ar=%d, bsic=%d, rxlev=%d, mcc=%d, mnc=%d, lac=%d, ci=%d", g_info.arfcn, g_info.bsic, g_info.rxlev, g_info.mcc, g_info.mnc, g_info.lac, g_info.ci);
  }
  else if(strcmp("Test03",(char*)param->command_buffer) == 0)
  {
    /* Gets the neighboring cell info when receiving AT command: AT+[1000]Test03 */
    int i, count;
    count = vm_gsm_cell_get_neighbor_number();
    vm_log_info("neighbor count=%d", count);
    for(i=0; i<count; i++)
    {
      vm_gsm_cell_get_neighbor_cell_info(&g_info, i);
      vm_log_info("ar=%d, bsic=%d, rxlev=%d, mcc=%d, mnc=%d, lac=%d, ci=%d", g_info.arfcn, g_info.bsic, g_info.rxlev, g_info.mcc, g_info.mnc, g_info.lac, g_info.ci);
    }
  }
  else if(strcmp("Test04",(char*)param->command_buffer) == 0)
  {
    /* Closes the cell when receiving AT command: AT+[1000]Test04 */
    vm_gsm_cell_close();
  }
}

/* The callback to be invoked by the system engine. */
void handle_sysevt(VMINT message, VMINT param) {
  switch (message) {
  case VM_EVENT_CREATE:
    vm_cmd_open_port(COMMAND_PORT, at_callback, NULL);
    break;
  case VM_EVENT_CELL_INFO_CHANGE:
	/* After opening the cell, this event will occur when the cell info changes.
	 * The new data of the cell info can be obtained from here. */    
    vm_gsm_cell_get_current_cell_info(&g_info);
    vm_log_info("ar=%d, bsic=%d, rxlev=%d, mcc=%d, mnc=%d, lac=%d, ci=%d", g_info.arfcn, g_info.bsic, g_info.rxlev, g_info.mcc, g_info.mnc, g_info.lac, g_info.ci);
    break;
  case VM_EVENT_QUIT:
    vm_cmd_close_port(COMMAND_PORT);
    break;
  }
}

/* Entry point */
void vm_main(void) {
  /* Registers system event handler */
  vm_pmng_register_system_event_callback(handle_sysevt);
}
