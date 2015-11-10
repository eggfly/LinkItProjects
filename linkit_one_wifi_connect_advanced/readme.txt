This example connects to an AP with SSID and password.

It set the device to station mode first by API vm_wlan_mode_set(), after set mode successfully, it will call vm_wlan_connect() to connect the AP, then print the AP information in the callback function.

Modify the macro AP_SSID, AP_PASSWORD and AP_AUTHORIZE_MODE to match your Wi-Fi AP.
