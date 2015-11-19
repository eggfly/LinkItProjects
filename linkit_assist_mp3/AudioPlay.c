/*
 This example code is in public domain.

 This example code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 This example will play an mp3 file

 In this example, it will play a file with vm_audio_play_open/vm_audio_play_start. The file name is audio.mp3
 is stored on the SD card or the flash storage.
 During the play back, the volume is set to 4 using vm_audio_set_volume.
 AT command can be used to control (pause, resume and stop) the playback.
 
 Before you run this example, you need to copy an mp3 file named audio.mp3 into the root directory of the
 SD card or the flash storage. After launching the example, you can use the AT command in monitor tool
 to control the flow as follows:
 AT+[1000]Test01: Start the playback of the mp3 file
 AT+[1000]Test02: Pause
 AT+[1000]Test03: Resume
 AT+[1000]Test04: Stop and close
 */

#include <stdio.h>
#include <string.h>
#include "vmtype.h"
#include "vmsystem.h"
#include "vmlog.h" 
#include "vmcmd.h" 
#include "vmfs.h"
#include "vmchset.h"
#include "vmaudio_play.h"

#define COMMAND_PORT 1000 /* AT command port */
#define MAX_NAME_LEN 260  /* Max length of file name */

static VMINT g_handle = -1; /* The handle of play */
static VMINT g_interrupt_handle = 0; /* The handle of interrupt */

/* The callback function when playing. */
void audio_play_callback(VM_AUDIO_HANDLE handle, VM_AUDIO_RESULT result,
		void * userdata) {
	switch (result) {
	case VM_AUDIO_RESULT_END_OF_FILE:
		/* When the end of file is reached, it needs to stop and close the handle */
		vm_audio_play_stop(g_handle);
		vm_audio_play_close(g_handle);
		g_handle = -1;
		break;
	case VM_AUDIO_RESULT_INTERRUPT:
		/* The playback is terminated by another application, for example an incoming call */
		vm_audio_play_stop(g_handle);
		vm_audio_play_close(g_handle);
		g_handle = -1;
		break;
	default:
		break;
	}
}

/* Play the audio file. */
void audio_play() {
	VMINT drv;
	VMWCHAR w_file_name[MAX_NAME_LEN] = { 0 };
	VMCHAR file_name[MAX_NAME_LEN];
	vm_audio_play_parameters_t play_parameters;
	VM_AUDIO_VOLUME volume;

	/* get file path */
	drv = vm_fs_get_removable_drive_letter();
	if (drv < 0) {
		drv = vm_fs_get_internal_drive_letter();
		if (drv < 0) {
			vm_log_fatal("not find driver");
			return;
		}
	}
	sprintf(file_name, (VMSTR) "%c:\\audio.mp3", drv);
	vm_chset_ascii_to_ucs2(w_file_name, MAX_NAME_LEN, file_name);

	/* set play parameters */
	memset(&play_parameters, 0, sizeof(vm_audio_play_parameters_t));
	play_parameters.filename = w_file_name;
	play_parameters.reserved = 0; /* no use, set to 0 */
	play_parameters.format = VM_AUDIO_FORMAT_MP3; /* file format */
	play_parameters.output_path = VM_AUDIO_DEVICE_SPEAKER_BOTH; /* set device to output */
	play_parameters.async_mode = 0;
	play_parameters.callback = audio_play_callback;
	play_parameters.user_data = NULL;
	g_handle = vm_audio_play_open(&play_parameters);
	if (g_handle >= VM_OK) {
		vm_log_info("open success");
	} else {
		vm_log_error("open failed");
	}
	/* start to play */
	vm_audio_play_start(g_handle);
	/* set volume */
	vm_audio_set_volume(VM_AUDIO_VOLUME_6);
	/* register interrupt callback */
	g_interrupt_handle = vm_audio_register_interrupt_callback(
			audio_play_callback, NULL);
}

/* AT command callback, which will be invoked when you send an AT command from the monitor tool */
void at_callback(vm_cmd_command_t *param, void *user_data) {
	VMBOOL is_play;

	if (strcmp("Test01", (char*) param->command_buffer) == 0) {
		/* start playing when the following command is received: AT+[1000]Test01 */
		audio_play();
	} else if (strcmp("Test02", (char*) param->command_buffer) == 0) {
		/* pause when the following command is received: AT+[1000]Test02 */
		if (g_handle >= 0) {
			vm_audio_play_pause(g_handle);
		}
	} else if (strcmp("Test03", (char*) param->command_buffer) == 0) {
		/* resume when the following command is received: AT+[1000]Test03 */
		if (g_handle >= 0) {
			vm_audio_play_resume(g_handle);
		}
	} else if (strcmp("Test04", (char*) param->command_buffer) == 0) {
		/* stop and close when the following command is received: AT+[1000]Test04 */
		if (g_handle >= 0) {
			vm_audio_play_stop(g_handle);
			vm_audio_play_close(g_handle);
			if (g_interrupt_handle != 0) {
				vm_audio_clear_interrupt_callback(g_interrupt_handle);
			}
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

