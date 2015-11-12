This example demonstrates how to create a sub-thread and how to communicate between the main thread and the sub-thread.

In this example, one sub-thread is created (vm_thread_create()) and the communication between the main thread and the sub-thread is established (via vm_thread_send_message()). It uses mutex (vm_mutex_lock()/vm_mutex_unlock()) and signal (vm_signal_wait()) for the global variable protection and thread synchronization.

After launching the example, the following AT commands can be used in the Monitor Tool as below:

AT+[1000]Test01: This creates a sub-thread. If succeeds, it will log the "message for creating thread successfully" on the Monitor. Then it will wait for the AT command of "AT+[1000]Test02".

AT+[1000]Test02: It logs "message for waiting signal successfully" on the Monitor
