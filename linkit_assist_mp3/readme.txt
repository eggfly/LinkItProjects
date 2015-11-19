This example will play an mp3 file.

In this example, it will play a file with vm_audio_play_open/vm_audio_play_start. The file name is audio.mp3 and is stored on the SD card or the flash storage. During the play back, the volume is set to 4 using vm_audio_set_volume. AT command can be used to control (pause, resume and stop) the playback.

Before you run this example, you need to copy an mp3 file named audio.mp3 into the root directory of the SD card or the flash storage. After launching the example, you can use the AT command in monitor tool to control the flow as follows:

AT+[1000]Test01: Start the playback of the mp3 file
AT+[1000]Test02: Pause 
AT+[1000]Test03: Resume 
AT+[1000]Test04: Stop and close 