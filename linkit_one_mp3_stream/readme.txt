This example will use bitstream play to play a file named audio.mp3 in the mass storage. 

Firstly we open the bitstream(vm_audio_stream_play_open) so we can put data(vm_audio_stream_play_put_data) to its ring buffer (max size is 16K). Next is to play it(vm_audio_stream_play_start) after we put all the mp3 files data to the buffer. To finish it call vm_audio_stream_play_finish. During the playing, you can retrieve the play time(vm_audio_stream_play_get_play_time) and stop the playback (vm_audio_stream_play_stop/vm_audio_stream_play_close). 

Before run this example, you need to copy an MP3 file named audio.mp3 onto the mass storage. While running this example, you are able to send commands through the monitor tool to control the flow:

AT+[1000]Test01: start play mp3 file
AT+[1000]Test02: get current play time
AT+[1000]Test03: stop and close play
