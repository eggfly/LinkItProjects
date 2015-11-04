/**
 * Send HTTP request to get DoubanFM's JSON via WLAN
 *
 * This sample connects to HTTP(no secure) to retrieve content and print to vm_log.
 * It calls the API vm_https_register_context_and_callback() to register the callback functions,
 * then set the channel by vm_https_set_channel(), after the channel is established,
 * it will send out the request by vm_https_send_request() and read the response by
 * vm_https_read_content().
 */