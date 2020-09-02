/*
 * task_sd_card.c
 *
 *  Created on: Feb 26, 2020
 *      Author: stoja
 */

#include "fatfs.h"

#include "tasks/task_sd_card.h"
#include "Util/util.h"
#include "Flash/w25qxx.h"

#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Disk_drvTypeDef disk;
extern logging_mode_e logging_mode;

FATFS EULER_FatFS;
FIL EULER_LOG_FILE;
UINT EULER_bytesSD;

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
	// Try everything forever;
	for (;;) {
		osDelay(100);
		flight_phase_detection_t local_flight_phase;
		FRESULT res;
		char EULER_LOG_FILE_NAME[13] = "";
		UsbPrint("[STORAGE TASK] Starting SD Card Task..\n");
		local_flight_phase.flight_phase = IDLE;
		resetSDCard: //UsbPrint("[STORAGE TASK] Mounting SD card\n");
		do {
			EULER_LOG_FILE_NAME[0] = 0;
			res = f_mount(&EULER_FatFS, "", 1);
			if (res != FR_OK) {
				UsbPrint("[STORAGE TASK] Failed mounting SD card: %d\n", res);
				// force sd card to be reinitialized
				disk.is_initialized[0] = 0;
				HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
				osDelay(10);
			}
		} while (res != FR_OK);

		logToNewFile: if (!EULER_LOG_FILE_NAME[0]) {
			UsbPrint("[STORAGE TASK] Creating file name\n");

			unsigned int file_number = 1;

			DIR dj;
			FILINFO fno;
			res = f_findfirst(&dj, &fno, "", "LOG_???.CSV");
			while (res == FR_OK && fno.fname[0]) {
				unsigned int current_file_number = (fno.fname[4] - '0') * 100
						+ (fno.fname[5] - '0') * 10 + (fno.fname[6] - '0');
				if (current_file_number + 1 > file_number) {
					file_number = current_file_number + 1;
				}
				res = f_findnext(&dj, &fno);
			}
			if (res != FR_OK) {
				UsbPrint(
						"[STORAGE TASK] Failed finding first or next file: %d\n",
						res);
				goto resetSDCard;
			}

			strcpy(EULER_LOG_FILE_NAME, "LOG_000.CSV");
			EULER_LOG_FILE_NAME[6] = '0' + file_number % 10;
			EULER_LOG_FILE_NAME[5] = '0' + (file_number / 10) % 10;
			EULER_LOG_FILE_NAME[4] = '0' + (file_number / 100) % 10;

			UsbPrint("[STORAGE TASK] Using file name: %s\n",
					EULER_LOG_FILE_NAME);

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
		switch (logging_mode) {
		case DIRECT: {
			volatile int32_t msgCounter = 0;
			char log_header[32] = "Timestamp;Log Entry Type;Data\n";
			uint32_t num_bytes = 0;
			res = f_write(&EULER_LOG_FILE, log_header, strlen(log_header),
					&EULER_bytesSD);
			if (res != FR_OK) {
				UsbPrint("[STORAGE TASK] Failed writing to file: %d\n", res);
				goto resetSDCard;
			}
			log_entry_t log_entry = { 0 };
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
			for (;;) {
				if (osMessageQueueGet(log_queue, &log_entry, NULL,
				osWaitForever) == osOK) {
					num_bytes = strlen(log_entry.str);
					if (num_bytes > 0) {
						res = f_write(&EULER_LOG_FILE, log_entry.str, num_bytes,
								&EULER_bytesSD);
						if (res != FR_OK) {
							UsbPrint(
									"[STORAGE TASK] Failed writing to file: %d\n",
									res);
							goto resetSDCard;
						}
					}
					msgCounter++;
					if (msgCounter >= SYNC_AFTER_COUNT) {
						msgCounter = 0;
						//UsbPrint("[STORAGE TASK] Syncing file..\n");
						HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
						res = f_sync(&EULER_LOG_FILE);
						if (res != FR_OK) {
							UsbPrint("[STORAGE TASK] Failed syncing file: %d\n",
									res);
							goto resetSDCard;
						}
						// if the rocket landed, create a new file and write to that one
						if (ReadMutex(&fsm_mutex,
								&global_flight_phase_detection,
								&local_flight_phase,
								sizeof(global_flight_phase_detection)) == osOK
								&& local_flight_phase.flight_phase
										== RECOVERY) {
							f_close(&EULER_LOG_FILE);
							// "clean" current file name
							EULER_LOG_FILE_NAME[0] = 0;
							goto logToNewFile;
						}
					}
				}
			}
		}
			break;
		case FLASHY: {
			uint16_t sector_id = -1;
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			/* TODO [nemanja] - free this thing!!! if we jump with goto we're doomed */
			uint8_t *logf_buffer_read = (uint8_t*) calloc(4096,
					sizeof(uint8_t));
			for (;;) {
				if (osMessageQueueGet(logf_sector_queue, &sector_id, NULL,
				osWaitForever) == osOK) {
					UsbPrint("[STORAGE TASK] Logging sector: %hu\n", sector_id);
					W25qxx_ReadSector(logf_buffer_read, sector_id, 0, 4096);
					uint16_t logf_buffer_read_idx = 0;
					uint16_t log_elem_size = 0;
					char print_buf[200] = { 0 };
					while (logf_buffer_read_idx < 4096) {
						uint8_t log_type_uint8_t =
								logf_buffer_read[logf_buffer_read_idx++];
						log_entry_type_e log_type =
								(log_entry_type_e) log_type_uint8_t;
						switch (log_type) {
						case SENSOR: {
							log_elem_size = sizeof(sensor_log_elem_t);
							sensor_log_elem_t sensor_log;
							memcpy(&sensor_log,
									&logf_buffer_read[logf_buffer_read_idx],
									log_elem_size);
							snprintf(print_buf + strlen(print_buf), 200,
									"[FLASH]: Sector#: %hu, Page#: %hu; Sensor data: %lu;%d;%hi,%d,",
									sector_id, logf_buffer_read_idx / 256,
									sensor_log.ts, SENSOR,
									sensor_log.sensor_board_id,
									sensor_log.sens_type);
							switch (sensor_log.sens_type) {
							case BARO: {
								baro_data_t *baro_data_ptr =
										(baro_data_t*) &sensor_log.sensor_data.baro;
								snprintf(print_buf + strlen(print_buf), 200,
										"P: %ld,T: %ld,Ts: %lu\n",
										baro_data_ptr->pressure,
										baro_data_ptr->temperature,
										baro_data_ptr->ts);
							}
								break;
							case IMU: {
								imu_data_t *imu_data_ptr =
										(imu_data_t*) &sensor_log.sensor_data.imu;
								snprintf(print_buf + strlen(print_buf), 200,
										"Ax: %hd, Ay: %hd, Az: %hd,Gx: %hd,Gy: %hd,Gz: %hd,Ts: %lu\n",
										imu_data_ptr->acc_x,
										imu_data_ptr->acc_y,
										imu_data_ptr->acc_z,
										imu_data_ptr->gyro_x,
										imu_data_ptr->gyro_y,
										imu_data_ptr->gyro_z, imu_data_ptr->ts);
							}
								break;
							case BATTERY: {
								battery_data_t *battery_data_ptr =
										(battery_data_t*) &sensor_log.sensor_data.bat;
								snprintf(print_buf + strlen(print_buf), 200,
										"%hd,%hd,%hd,%hd\n",
										battery_data_ptr->battery,
										battery_data_ptr->consumption,
										battery_data_ptr->current,
										battery_data_ptr->supply);
								break;
							}
							default: {
								snprintf(print_buf + strlen(print_buf), 200,
										"[FLASH] Sensor type not recognized!\n");
								break;
							}
							}
						}
							break;
						case MSG: {
							/* first elem of log msg is its size */
							log_elem_size =
									logf_buffer_read[logf_buffer_read_idx++];
							char str_log[LOG_BUFFER_LEN];
							memcpy(&str_log,
									&logf_buffer_read[logf_buffer_read_idx],
									log_elem_size);
							snprintf(print_buf + strlen(print_buf), 200,
									"[FLASH]: Sector#: %hu, Page#: %hu; Strlen: %d, Msg: %s\n",
									sector_id, logf_buffer_read_idx / 256,
									log_elem_size, str_log);
						}
							break;
						default: {
							logf_buffer_read_idx = 4096;
							snprintf(print_buf + strlen(print_buf), 200,
									"[FLASH] Log type not recognized while reading!\n");
						}
						}
						logf_buffer_read_idx += log_elem_size;
						UsbPrint(print_buf);
						print_buf[0] = 0;
					}
				}
			}
		}
			break;
		default:
			break;
		}

	}
}
