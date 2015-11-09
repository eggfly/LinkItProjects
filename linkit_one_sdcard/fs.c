/*
 This example code is in public domain.

 This example code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 This sample shows how to create and copy file.
 In this sample, it will create the file test_file.txt in this path: C:\mre\fs_copy_file\,
 the content of this file is "sample of create and copy file.", after that, it will copy this file,
 the new file is in the same folder and the file name of the copies is "copy_of_test_file.txt"
 */
#include "vmtype.h"
#include "vmboard.h"
#include "vmsystem.h"
#include "vmlog.h"
#include "vmdcl.h"
#include "vmdcl_gpio.h"
#include "vmthread.h"
#include "vmfs.h"
#include "ResID.h"
#include <stdio.h>
#include <string.h>

void fs_write_handle_sysevt(VMINT message, VMINT param);

void vm_main(void) {
	/* register system events handler */
	vm_pmng_register_system_event_callback(fs_write_handle_sysevt);
}

void fs_write(void) {
	VMCHAR filename[VM_FS_MAX_PATH_LENGTH] = { 0 };
	VMWCHAR wfilename[VM_FS_MAX_PATH_LENGTH] = { 0 };
	VMWCHAR wfilename_copy[VM_FS_MAX_PATH_LENGTH] = { 0 };
	VM_FS_HANDLE filehandle = -1;
	VMUINT writelen = 0;
	VMINT ret = 0;
	VMCHAR filecontent[100] = { 0 };
	VMUINT readlen = 0;

	vm_log_info("FS_demo_create_and_copy_file -Start");

	VMCHAR letter = vm_fs_get_removable_drive_letter();
	sprintf(filename, "%c:\\%s", letter, "test_file.txt");
	vm_chset_ascii_to_ucs2(wfilename, sizeof(wfilename), filename);

	/* create file */
	if ((filehandle = vm_fs_open(wfilename, VM_FS_MODE_CREATE_ALWAYS_WRITE,
	TRUE)) < 0) {
		vm_log_info("Failed to create file: %s", filename);
		return;
	}

	vm_log_info("Success to create file: %s", filename);

	/* write file */
	strcpy(filename, "SAMPLE of create and READ file.");
	ret = vm_fs_write(filehandle, (void*) filename, 20, &writelen);
	if (ret < 0) {
		vm_log_info("Failed to write file");
		return;
	}
	vm_log_info("Success to write file: %s", filename);

	/* close file */
	vm_fs_close(filehandle);

	/* open the copies */
	if ((filehandle = vm_fs_open(wfilename, VM_FS_MODE_READ, TRUE)) < 0) {
		vm_log_info("Failed to open file.");
		return;
	}

	vm_log_info("Success to open: %s", filename);

	/* read the copies */
	memset(filecontent, 0, sizeof(filecontent));
	ret = vm_fs_read(filehandle, (void*) filecontent, 20, &readlen);
	if (ret < 0) {
		vm_log_info("Failed to read the copies.");
		return;
	} else {
		vm_log_info("Success to read file content: %s", filecontent);
		vm_fs_close(filehandle);
	}

	vm_log_info("FS_demo_create_and_copy_file - End");
	return;
}

void fs_write_handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		/* delay for catch logs */
		vm_thread_sleep(1000);
		vm_log_info("Sample of FS create & copy file - Start.");
		fs_write();

		break;

	case VM_EVENT_QUIT:
		vm_log_info("Sample of FS create & copy file - End.");
		break;
	}
}

