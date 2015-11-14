/*
 This example code is in public domain.

 This example code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 This sample shows how to list the files in a special folder.
 In this sample, it will list all the files under the folder "C:\mre\fs_copy_file\",
 the name of these files will show in logs file.
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

void handle_sysevt(VMINT message, VMINT param);

void vm_main(void)
{
	/* register system events handler */
	vm_pmng_register_system_event_callback(handle_sysevt);
}

void fs_demo_find_files(void)
{
	VMCHAR filename[VM_FS_MAX_PATH_LENGTH] = { 0 };
	VMWCHAR wfilename[VM_FS_MAX_PATH_LENGTH] = { 0 };
	VM_FS_HANDLE filehandle = -1;
	vm_fs_info_ex_t fileinfo;
	VMINT ret = 0;

	vm_log_info("fs_demo_find_files - START");

	sprintf(filename, "%c:\\%s", vm_fs_get_removable_drive_letter(), "*");
	vm_chset_ascii_to_ucs2(wfilename, sizeof(wfilename), filename);

	filehandle = vm_fs_find_first_ex(wfilename, &fileinfo);
	if (filehandle >= 0)
			{
		do
		{
			vm_chset_ucs2_to_ascii(filename, sizeof(filename),
					fileinfo.full_filename);
			vm_log_info("Find out the file : %s", filename);

			/* find the next file */
			ret = vm_fs_find_next_ex(filehandle, &fileinfo);
		} while (0 == ret);

		vm_fs_find_close_ex(filehandle);

	}
	else
	{
		vm_log_info("Failed to find file.");
	}

	vm_log_info("fs_demo_find_files - END");

	return;
}

void handle_sysevt(VMINT message, VMINT param)
{
	switch (message)
	{
	case VM_EVENT_CREATE:
		/* delay for catch logs */
		vm_thread_sleep(8000);
		vm_log_info("Sample of FS list file - Start.");
		fs_demo_find_files();

		break;

	case VM_EVENT_QUIT:
		vm_log_info("Sample of FS list file - End.");
		break;
	}
}

