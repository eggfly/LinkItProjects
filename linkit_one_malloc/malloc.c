/*
 This example code is in public domain.

 This example code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 This example demonstrates how to create a sub-thread and how to communicate
 between the main thread and the sub-thread.
 
 In this example, one sub-thread is created (vm_thread_create()) and the
 communication between the main thread and the sub-thread is established
 (via vm_thread_send_message()). It uses mutex (vm_mutex_lock()/vm_mutex_unlock())
 and signal (vm_signal_wait()) for the global variable protection and thread
 synchronization.

 After launching the example, the following AT commands can be used in the
 Monitor Tool as below:
 AT+[1000]Test01: This creates a sub-thread. If succeeds, it will log the
 "message for creating thread successfully" on the Monitor.
 Then it will wait for the AT command of "AT+[1000]Test02".
 AT+[1000]Test02: It logs "message for waiting signal successfully" on the Monitor
 */

#include "vmtype.h"
#include "vmsystem.h"
#include "vmcmd.h"
#include "vmlog.h"
#include "vmthread.h"

#define COMMAND_PORT  1000 /* AT command port */

/* Message ID for the thread communication. */
#define USER_MSG_1 2000
#define USER_MSG_2 2001
#define USER_MSG_3 2002

VM_THREAD_HANDLE g_thread = 0; /* The sub-thread handle */

/* Signal and mutex for thread synchronization */
VM_SIGNAL_ID g_signal;
vm_mutex_t g_mutex;

VMUINT g_variable = 0x5555; /* The global variable for sharing among multi-threads. */
vm_thread_message_t g_message; /* The message structure */

/* Thread message procedure */
void thread_main(vm_thread_message_t msg)
{
	switch (msg.message_id)
	{
	case USER_MSG_1:
		g_message.message_id = USER_MSG_2;
		g_message.user_data = NULL;
		/* Sends USER_MSG_2 to the main thread. */
		vm_thread_send_message(vm_thread_get_main_handle(), &g_message);
		vm_signal_wait(g_signal); /* Waits for signal. This thread is blocked here. */
		vm_mutex_lock(&g_mutex);
		g_variable = 0x4444;
		vm_mutex_unlock(&g_mutex);
		g_message.message_id = USER_MSG_3;
		g_message.user_data = NULL;
		/* Sends USER_MSG_3 to the main thread. */
		vm_thread_send_message(vm_thread_get_main_handle(), &g_message);
		break;
	default:
		break;
	}

}

/* Thread message procedure */
VMINT32 thread_proc(VM_THREAD_HANDLE thread_handle, void* user_data)
{
	vm_thread_message_t msg;
	while (1)
	{
		vm_thread_get_message(&msg);
		thread_main(msg);
		vm_thread_sleep(10);
		break;
	}
	return 0;
}

// eggfly
char * buf;

/* Create a sub-thread and sends a message to the sub-thread. */
static void thread_create(void)
{
	vm_mutex_lock(&g_mutex);
	g_variable = 0x5555;
	vm_mutex_unlock(&g_mutex);
	g_thread = vm_thread_create(thread_proc, NULL, 126);
	if (g_thread == 0)
			{
		vm_log_info("create thread failed");
		return;
	}
	else
	{
		vm_log_info("create thread successful");
		g_message.message_id = USER_MSG_1;
		g_message.user_data = &g_variable;
		vm_thread_send_message(g_thread, &g_message);
	}
}

/* AT command callback, which will be invoked when you send AT command from monitor tool */
static void at_callback(vm_cmd_command_t* param, void* user_data)
{
	if (strcmp("Test01", (char*) param->command_buffer) == 0) {
		/* create sub thread when receive command: AT+[1000]Test01 */
		thread_create();
	} else if (strcmp("Test02", (char*) param->command_buffer) == 0) {
		/* post signal when receive command: AT+[1000]Test02 */
		vm_signal_post(g_signal);
	} else {
		int size = atoi(param->command_buffer);
		if (size != 0) {
			// eggfly
			// heap size: 479KB +
			vm_log_debug("atoi(): %d", size);
			buf = vm_malloc(size);
			if (buf != NULL) {
				vm_log_debug("malloc ok!");
			} else {
				vm_log_debug("malloc failed!");
			}
		}
	}
}

/* The callback to be invoked by the system engine. */
void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		vm_mutex_init(&g_mutex);
		g_signal = vm_signal_create();
		vm_cmd_open_port(COMMAND_PORT, at_callback, NULL);
		break;
	case USER_MSG_2: /* Receives the message from the sub-thread. */
		vm_log_info("message for creating thread successfully");
		break;
	case USER_MSG_3: /* Receives the message from the sub-thread. */
		vm_log_info("message for waiting signal successfully");
		break;
	case VM_EVENT_QUIT:
		vm_signal_clear(g_signal);
		vm_signal_destroy(g_signal);
		vm_cmd_close_port(COMMAND_PORT);
		break;
	}
}

/* Entry point */
void vm_main(void) {
	/* Registers system event handle */
	vm_pmng_register_system_event_callback(handle_sysevt);
}
