/*
 * task_sd_card.c
 *
 *  Created on: Feb 26, 2020
 *      Author: stoja
 */

#include "util.h"
#include "fatfs.h"

#include "tasks/task_sd_card.h"

#include <float.h>
#include <stdio.h>
#include <string.h>

extern Disk_drvTypeDef disk;

FATFS EULER_FatFS;
FIL EULER_LOG_FILE;
UINT EULER_bytesSD;

/*
 *  %c	character
 %d	decimal (integer) number (base 10)
 %e	exponential floating-point number
 %f	floating-point number
 %i	integer (base 10)
 %o	octal number (base 8)
 %s	a string of characters
 %u	unsigned decimal (integer) number
 %x	number in hexadecimal (base 16)
 %%	print a percent sign
 \%	print a percent sign
 */

void printLogHeader(char log_buffer[]) {
	sprintf(log_buffer, "Timestamp,Sensor Type,fkfkfkf");
}

void formatLogData(const log_entry_t *const log_entry, char log_buffer[]) {
	switch (log_entry->sens_type) {
	case IMU:
		sprintf(log_buffer, "%ld,%d,%d,%ld,%ld,%ld,%ld,%ld,%ld\r\n",
				log_entry->timestamp, log_entry->sensor_board_id,
				log_entry->sens_type, log_entry->sensor.imu.acc_x,
				log_entry->sensor.imu.acc_y, log_entry->sensor.imu.acc_z,
				log_entry->sensor.imu.gyro_x, log_entry->sensor.imu.gyro_y,
				log_entry->sensor.imu.gyro_z);
		break;
	case BARO:
		sprintf(log_buffer, "%ld,%d,%d,%ld,%ld,%ld\r\n", log_entry->timestamp,
				log_entry->sensor_board_id, log_entry->sens_type,
				log_entry->sensor.baro.pressure,
				log_entry->sensor.baro.temperature,
				log_entry->sensor.baro.timestamp);
		break;
	}
}

uint8_t transformFRESULT(FRESULT res) {
	switch (res) {
	case FR_OK: /* (0) Succeeded */
		return 0;
		break;
	case FR_DISK_ERR: /* (1) A hard error occurred in the low level disk I/O layer */
		return 1;
		break;
	case FR_INT_ERR: /* (2) Assertion failed */
		return 2;
		break;
	case FR_NOT_READY: /* (3) The physical drive cannot work */
		return 3;
		break;
	case FR_NO_FILE: /* (4) Could not find the file */
		return 4;
		break;
	case FR_NO_PATH: /* (5) Could not find the path */
		return 5;
		break;
	case FR_INVALID_NAME: /* (6) The path name format is invalid */
		return 6;
		break;
	case FR_DENIED: /* (7) Access denied due to prohibited access or directory full */
		return 7;
		break;
	case FR_EXIST: /* (8) Access denied due to prohibited access */
		return 8;
		break;
	case FR_INVALID_OBJECT: /* (9) The file/directory object is invalid */
		return 9;
		break;
	case FR_WRITE_PROTECTED: /* (10) The physical drive is write protected */
		return 10;
		break;
	case FR_INVALID_DRIVE: /* (11) The logical drive number is invalid */
		return 11;
		break;
	case FR_NOT_ENABLED: /* (12) The volume has no work area */
		return 12;
		break;
	case FR_NO_FILESYSTEM: /* (13) There is no valid FAT volume */
		return 13;
		break;
	case FR_MKFS_ABORTED: /* (14) The f_mkfs() aborted due to any problem */
		return 14;
		break;
	case FR_TIMEOUT: /* (15) Could not get a grant to access the volume within defined period */
		return 15;
		break;
	case FR_LOCKED: /* (16) The operation is rejected according to the file sharing policy */
		return 16;
		break;
	case FR_NOT_ENOUGH_CORE: /* (17) LFN working buffer could not be allocated */
		return 17;
		break;
	case FR_TOO_MANY_OPEN_FILES: /* (18) Number of open files > _FS_LOCK */
		return 18;
		break;
	case FR_INVALID_PARAMETER:
		return 19;
		break;
	}
	return 20;
}

void vTaskSdCard(void *argument) {
	FRESULT res;
	char EULER_LOG_FILE_NAME[13] = "";
	UsbPrint("[STORAGE TASK] Starting SD Card Task..\r\n");

	resetSDCard: UsbPrint("[STORAGE TASK] Mounting SD card\r\n");
	do {
		res = f_mount(&EULER_FatFS, "", 1);
		if (res != FR_OK) {
			UsbPrint("[STORAGE TASK] Failed mounting SD card: %d\n", res);
			// force sd card to be reinitialized
			disk.is_initialized[0] = 0;
			osDelay(100);
		}
	} while (res != FR_OK);

	if (!EULER_LOG_FILE_NAME[0]) {
		UsbPrint("[STORAGE TASK] Creating file name\r\n");

		unsigned int file_number = 1;

		DIR dj;
		FILINFO fno;
		res = f_findfirst(&dj, &fno, "", "LOG_???.TXT");
		while (res == FR_OK && fno.fname[0]) {
			unsigned int current_file_number = (fno.fname[4] - '0') * 100
					+ (fno.fname[5] - '0') * 10 + (fno.fname[6] - '0');
			if (current_file_number + 1 > file_number) {
				file_number = current_file_number + 1;
			}
			res = f_findnext(&dj, &fno);
		}
		if (res != FR_OK) {
			UsbPrint("[STORAGE TASK] Failed finding first or next file: %d\n",
					res);
			goto resetSDCard;
		}

		strcpy(EULER_LOG_FILE_NAME, "LOG_000.CSV");
		EULER_LOG_FILE_NAME[6] = '0' + file_number % 10;
		EULER_LOG_FILE_NAME[5] = '0' + (file_number / 10) % 10;
		EULER_LOG_FILE_NAME[4] = '0' + (file_number / 100) % 10;

		UsbPrint("[STORAGE TASK] Using file name: %s\n", EULER_LOG_FILE_NAME);

		res = f_closedir(&dj);
		if (res != FR_OK) {
			UsbPrint("[STORAGE TASK] Failed closing directory: %d\n", res);
			goto resetSDCard;
		}
	}

	UsbPrint("[STORAGE TASK] Opening log file\n");
	res = f_open(&EULER_LOG_FILE, EULER_LOG_FILE_NAME,
	FA_OPEN_ALWAYS | FA_WRITE);
	if (res != FR_OK) {
		UsbPrint("[STORAGE TASK] Failed opening log file: %d\n", res);
		goto resetSDCard;
	}

	UsbPrint("[STORAGE TASK] Going to end of file\n");
	res = f_lseek(&EULER_LOG_FILE, f_size(&EULER_LOG_FILE));
	if (res != FR_OK) {
		UsbPrint("[STORAGE TASK] Failed going to end of file: %d\n", res);
		goto resetSDCard;
	}

	int msgCounter = 0;
	char log_buffer[200] = "COL_1,COL_2,COL_3,...\r\n";
	uint32_t num_bytes = 0;
	res = f_write(&EULER_LOG_FILE, log_buffer, strlen(log_buffer),
			&EULER_bytesSD);
	if (res != FR_OK) {
		UsbPrint("[STORAGE TASK] Failed writing to file: %d\n", res);
		goto resetSDCard;
	}
	log_entry_t log_entry = { 0 };
	for (;;) {
		if (osMessageQueueGet(log_queue, &log_entry, NULL,
		osWaitForever) == osOK) {
			formatLogData(&log_entry, log_buffer);
			num_bytes = strlen(log_buffer);
			if (num_bytes > 0) {
				res = f_write(&EULER_LOG_FILE, log_buffer, strlen(log_buffer),
						&EULER_bytesSD);
				if (res != FR_OK) {
					UsbPrint("[STORAGE TASK] Failed writing to file: %d\n",
							res);
					goto resetSDCard;
				}
			}

			msgCounter++;

			if (msgCounter >= SYNC_AFTER_COUNT) {
				msgCounter = 0;

				UsbPrint("[STORAGE TASK] Syncing file..\r\n");
				res = f_sync(&EULER_LOG_FILE);
				if (res != FR_OK) {
					UsbPrint("[STORAGE TASK] Failed syncing file: %d\n", res);
					goto resetSDCard;
				}
			}
		}
	}
}
