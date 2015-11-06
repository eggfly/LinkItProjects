/*
 This example code is in public domain.

 This example code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 This example will use bitstream play to play a file named audio.mp3 in the mass storage.

 Firstly we open the bitstream(vm_audio_stream_play_open) so we can put data(vm_audio_stream_play_put_data) to its ring buffer (max
 size is 16K). Next is to play it(vm_audio_stream_play_start) after we put all the mp3 files data to the buffer. To finish it call
 vm_audio_stream_play_finish. During the playing, you can retrieve the play time(vm_audio_stream_play_get_play_time) and stop the
 playback (vm_audio_stream_play_stop/vm_audio_stream_play_close).

 Before run this example, you need to copy an MP3 file named audio.mp3 onto the mass storage. While running this example, you are
 able to send commands through the monitor tool to control the flow:

 AT+[1000]Test01: start play mp3 file
 AT+[1000]Test02: get current play time
 AT+[1000]Test03: stop and close play
 */

#include <stdio.h>
#include "vmtype.h"
#include "vmsystem.h"
#include "vmmemory.h"
#include "vmcmd.h"
#include "vmlog.h"
#include "vmfs.h"
#include "vmchset.h"
#include "vmaudio_stream_play.h"
#include "vmtimer.h"

#define COMMAND_PORT  1000 /* AT command port */
#define MAX_NAME_LEN 260 /* Max length of file name */

VM_AUDIO_HANDLE g_handle = 0; /* Audio player handle */

/* Variable to help getting the audio file data */
VMUINT g_file_size;
VMINT g_file_hdl = -1;
VMINT g_offset = 0;
VMINT g_flag = 0;

/* Timer to help streaming audio data */
VM_TIMER_ID_PRECISE g_timer_id;

/* Put the file data into the stream player buffer */
static void audio_put_data() {
	VMINT ret;
	VMUINT8* data = NULL;
	VMUINT readed;
	VMUINT used_size;
	vm_audio_stream_play_buffer_status_t status;
	vm_audio_stream_play_start_parameters_t param;

	/* Stop the timer if the pointer reaches the end of file */
	if (g_offset >= g_file_size) {
		return;
	}

	/* get the stream player buffer status */
	vm_audio_stream_play_get_buffer_status(g_handle, &status);

	/* put data into the buffer if there is free space */
	if (status.free_buffer_size > 4800) {
		if ((g_file_size - g_offset) < 4800)
			used_size = g_file_size - g_offset;
		else
			used_size = 4800;
	} else {
		return;
	}

	data = (VMUINT8*) vm_calloc(used_size);
	vm_fs_seek(g_file_hdl, g_offset, VM_FS_BASE_BEGINNING);
	vm_fs_read(g_file_hdl, data, used_size, &readed);

	/* put data into the buffer */
	ret = vm_audio_stream_play_put_data(g_handle, data, readed, &used_size);
	if (VM_OK == ret)
		g_offset += used_size;

	vm_free(data);

	/* after putting the data into the buffer, start playing the audio */
	//if(g_flag==0 )
	{
		param.start_time = 0;
		param.audio_path = VM_AUDIO_DEVICE_SPEAKER2;
		param.volume = VM_AUDIO_VOLUME_3;
		ret = vm_audio_stream_play_start(g_handle, &param);
		g_flag = 1;
	}

	if (g_offset == g_file_size) {
		/* no data to play hence stop the playback */
		vm_audio_stream_play_finish(g_handle);
	}
}

/* Stream callback function */
void audio_callback(VM_AUDIO_HANDLE handle, VM_AUDIO_RESULT result,
		void* user_data) {
	switch (result) {
	case VM_AUDIO_DATA_REQUEST:
		/* should put data in this event */
		vm_log_fatal("VM_AUDIO_DATA_REQUEST");
		audio_put_data();
		break;
	default:
		break;
	}
}

/* Open file and prepare to put data */
void audio_open_file() {
	VMINT drv;
	VMWCHAR w_file_name[MAX_NAME_LEN] = { 0 };
	VMCHAR file_name[MAX_NAME_LEN];
	vm_audio_stream_play_config_t audio_cfg;

	drv = vm_fs_get_removable_drive_letter();
	if (drv < 0) {
		vm_log_warn("not find removable driver");
		drv = vm_fs_get_internal_drive_letter();
		if (drv < 0) {
			vm_log_fatal("not find system driver");
			return;
		}
	}
	sprintf(file_name, "%c:\\audio.mp3", drv);
	vm_chset_ascii_to_ucs2(w_file_name, MAX_NAME_LEN, file_name);
	g_file_hdl = vm_fs_open(w_file_name, VM_FS_MODE_READ, TRUE);
	if (g_file_hdl < 0) {
		vm_log_info("vm_file_open error = %d", g_file_hdl);
		return;
	}
	vm_fs_get_size(g_file_hdl, &g_file_size);

	audio_cfg.codec_type = VM_AUDIO_CODEC_MP3;
	if (VM_OK
			!= vm_audio_stream_play_open(&g_handle, &audio_cfg, audio_callback,
					NULL)) {
		vm_log_info("vm_bitstream_audio_open error");
	}
}

/* Timer callback to put audio data */
void audio_put_data_timer_cb(VMINT tid, void* user_data) {
	audio_put_data();
}

/* AT command callback, which will be invoked when you send AT command from the monitor tool */
static void at_callback(vm_cmd_command_t *param, void *user_data) {
	if (strcmp("Test01", (char*) param->command_buffer) == 0) {
		/* start playing when following command is received: AT+[1000]Test01 */
		g_offset = 0;
		audio_open_file();
		g_timer_id = vm_timer_create_precise(1000, audio_put_data_timer_cb,
				NULL);
		audio_put_data();
	} else if (strcmp("Test02", (char*) param->command_buffer) == 0) {
		/* get play time when following command is received: AT+[1000]Test02 */
		if (g_handle != 0) {
			VMUINT time;
			vm_audio_stream_play_get_play_time(g_handle, &time);
			vm_log_info("current time = %d", time);
		}
	} else if (strcmp("Test03", (char*) param->command_buffer) == 0) {
		/* stop and close when following command is received: AT+[1000]Test03 */
		if (g_handle != 0) {
			vm_timer_delete_precise(g_timer_id);
			vm_audio_stream_play_stop(g_handle);
			vm_audio_stream_play_close(g_handle);
		}
	}
}

/* The callback to be invoked by the system engine. */
void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		vm_cmd_open_port(COMMAND_PORT, at_callback, NULL);
		break;
	case VM_EVENT_QUIT:
		vm_cmd_close_port(COMMAND_PORT);
		break;
	}
}

/* Entry point */
void vm_main(void) {
	vm_pmng_register_system_event_callback(handle_sysevt);
}

