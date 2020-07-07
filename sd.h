/**
  **************************************************************************
  * @author 	Emin Yagmahan
  * @date 		07.02.2020
  * @file 		../include/Logging/sd.h
  * @brief 		Allows to read and write from/to a SD Card. SD Card is connected
  *             via SPI at 315kHz Init Speed and 20MHz during operation.
  *             write and read tests are performed to calculate deviations in
  *             sd card performance.
  * @version	V 2.0
  **************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SD_H_
#define __SD_H_

/*-------------------------------------------------------------------------*
 * Includes
 *-------------------------------------------------------------------------*/
#include <fs/fs.h>
#include <ff.h>
#include <logging/log.h>
#include <stdio.h>
#include <zephyr.h>
#include <fs/fs.h>

/*-------------------------------------------------------------------------*
 * Defines
 *-------------------------------------------------------------------------*/
#define TEST_FILE_SIZE 1024
#define KILO 1000

/*-------------------------------------------------------------------------*
 * Prototype Functions
 *-------------------------------------------------------------------------*/
int lsdir(const char *path);
int test_write_speed(struct fs_file_t file);
int test_read_speed(struct fs_file_t file);
void sd_card_info(void);

#endif
