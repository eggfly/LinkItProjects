This example shows how to receive keypad event and keycode from the board.

This example use vm_keypad_register_event_callback() to register keypad handler. When operate the key on the board, the key event and keycode will be sent to application to the keypad handler. 

Upload this example application, and connect Monitor tool for log. On LinkIt Assist 2502,if K0 is pressed, the log shows "key event=2,key code=0".
