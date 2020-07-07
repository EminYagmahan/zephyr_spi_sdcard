/**
  **************************************************************************
  * @author 	Emin Yagmahan
  * @date 		07.02.2020
  * @file 		../src/Logging/sd.c
  * @brief 		Allows to read and write from/to a SD Card. SD Card is connected
  *             via SPI at 315kHz Init Speed and 20MHz during operation.
  *             write and read tests are performed to calculate deviations in
  *             sd card performance.
  * @version	V 2.0
  **************************************************************************
  */
/*-------------------------------------------------------------------------*
 * Includes
 *-------------------------------------------------------------------------*/
#include <logging/log.h>
#include <disk/disk_access.h>
#include "sd.h"


/*-------------------------------------------------------------------------*
 * Definitions
 *-------------------------------------------------------------------------*/
LOG_MODULE_REGISTER(sd_card_info);

char buf_test[TEST_FILE_SIZE];
static FATFS fat_fs;

/*
*  Note the fatfs library is able to mount only strings inside _VOLUME_STRS
*  in ffconf.h
*/
static const char disk_mount_pt[] = "/SD:";
struct fs_file_t sd_file;

/* mounting info */
static struct fs_mount_t mp = {
        .type = FS_FATFS,
        .mnt_point = disk_mount_pt,
        .fs_data = &fat_fs,
};


/*-------------------------------------------------------------------------*
 * Functions
 *-------------------------------------------------------------------------*/
/**
 * @brief lists directories and files of mounted file system
 * @param path Path of mount point
 * @retval 0 Success
 * @retval -ERRNO errno code if error
 * @return  If function returns 0, mounted file system could be read, listed and closed.
 */
int lsdir(const char *path) {
    int res;
    struct fs_dir_t dirp;
    static struct fs_dirent entry;

    /* Verify fs_opendir() */
    res = fs_opendir(&dirp, path);
    if (res) {
        printk("Error opening dir %s [%d]\n", path, res);
        return res;
    }

    printk("Listing dir %s ...\n", path);
    for (;;) {
        /* Verify fs_readdir() */
        res = fs_readdir(&dirp, &entry);

        /* entry.name[0] == 0 means end-of-dir */
        if (res || entry.name[0] == 0) {
            break;
        }

        if (entry.type == FS_DIR_ENTRY_DIR) {
            printk("[DIR ] %s\n", entry.name);
        } else {
            printk("[FILE] %s (size = %zu)\n",
                   entry.name, entry.size);
        }
    }
    /* Verify fs_closedir() */
    fs_closedir(&dirp);

    return res;
}

/**
 * @brief Test function to determine single block write speed on sd card.
 *        1024 KB are written to sd card.
 * @param file Used file object for sd card read and write test
 * @retval 0 Success. Test passed.
 * @retval -ERRNO errno code if error. Test failed.
 * @return  If function returns 0, created testfile could be written on sd card
 * @note printed time includes time for FS operations. Avg. Speed 79,9 KBytes/s.
 * @todo compare measured speed with avg speed + offset.
 */
int test_write_speed(struct fs_file_t file) {

    u64_t total_file_size = TEST_FILE_SIZE * KILO;

    /* Create dummy char file with custom file size */
    memset(buf_test, 'X', TEST_FILE_SIZE);

    printk("\r\n ----- Testing sd card write performance -----\r\n");
    printk("Test 1: write a file to sd card with size of %llu bytes\r\n", total_file_size);

    /* capture initial time stamp */
    s64_t time_stamp = k_uptime_get();

    // Open File. If file already exists, it will be overwritten
    if (fs_open(&file, "/SD:/test.log") != 0) {
        printk("FS_OPEN failed!\n");
        return -1;
    }

    /* Add 1000x 1024 Byte Junk to end of file and write. */
    for (int i = 0; i < KILO; ++i) {
        /* Search for End of File */
        if (fs_seek(&file, 0, FS_SEEK_END) != 0) {
            printk("Test 1 Error: FS_SEEK failed!\n");
            return -1;
        }

        if (fs_write(&file, buf_test, TEST_FILE_SIZE) != TEST_FILE_SIZE) {
            fs_close(&file);
            printk("Test 1 Error: Write error\n");
            return -1;
        }
    }
    /* flush cached information of a written file. */
    if (fs_sync(&file) != 0) {
        printk("Test 1 Error: Sync error\n");
        return -1;
    }
    /* close an open file. */
    fs_close(&file);

    /* Calculate time difference and write speed */
    s64_t milliseconds_spent = k_uptime_delta(&time_stamp);
    printk("Performance Result - write time for %d byte: %llu ms\r\n", \
    (TEST_FILE_SIZE * KILO), milliseconds_spent);
    printk("----- End: testing SD Card write performance: Passed -----\r\n");
    return 0;

}

/**
 * @brief Test function to determine single block read speed on sd card.
 *        1024 KB are read from sd card.
 * @param file Used file object for sd card read and write test
 * @retval 0 Success. Test passed.
 * @retval -ERRNO errno code if error. Test failed.
 * @return  If function returns 0, testfile could be read from sd card
 * @note printed time includes time for FS operations. Avg. Speed 79,9 KBytes/s.
 * @todo compare measured speed with avg speed + offset.
 */
int test_read_speed(struct fs_file_t file) {

    u64_t total_file_size = TEST_FILE_SIZE * KILO;
    memset(buf_test, 0, TEST_FILE_SIZE);


    printk("\r\n ----- Testing sd card read performance -----\r\n");
    printk("Test 2: read a file from sd card with size of %llu bytes\r\n", total_file_size);

    /* capture initial time stamp */
    s64_t time_stamp = k_uptime_get();

    /* Open File. If file already exists, it will be overwritten */
    if (fs_open(&file, "/SD:/TEST") != 0) {
        printk("Test 2 Error: FS_OPEN failed!\n");
        return -1;
    }

    /* Read 1000x 1024 Byte Junk from beginning of file */
    for (int i = 0; i < KILO; ++i) {

        if (fs_seek(&file, 0, FS_SEEK_SET) != 0) {
            printk("Test 2 Error: FS_SEEK failed!\n");
            return -1;
        }
        if (fs_read(&file, buf_test, TEST_FILE_SIZE) != TEST_FILE_SIZE) {
            fs_close(&file);
            printk("Test 2 Error: Read error!\n");
            return -1;
        }
    }
    /* close an open file. */
    fs_close(&file);

    /* Calculate time difference and read speed */
    s64_t milliseconds_spent = k_uptime_delta(&time_stamp);
    printk("Performance Result - read time for %d byte: %llu ms\r\n", \
    (TEST_FILE_SIZE * KILO), milliseconds_spent);
    printk("----- End: testing SD Card read performance: Passed -----\r\n");
    return 0;
}

/**
 * @brief Prints SD Card Vendor informations
 */
void sd_card_info() {
    /* raw disk i/o */
    static const char *disk_pdrv = "SD";
    u64_t memory_size_mb;
    u32_t block_count;
    u32_t block_size;

    do {
        if (disk_access_init(disk_pdrv) != 0) {
            LOG_ERR("Storage init ERROR!");
            break;
        }

        if (disk_access_ioctl(disk_pdrv,
                              DISK_IOCTL_GET_SECTOR_COUNT, &block_count)) {
            LOG_ERR("Unable to get sector count");
            break;
        }
        LOG_INF("Block count %u", block_count);

        if (disk_access_ioctl(disk_pdrv,
                              DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) {
            LOG_ERR("Unable to get sector size");
            break;
        }
        LOG_INF("Sector size %u\n", block_size);

        memory_size_mb = (u64_t) block_count * block_size;
        LOG_INF("Memory Size(MB) %u\n", (u32_t) memory_size_mb >> 20);
    } while (0);
    int res = fs_mount(&mp);

    if (res == FR_OK) {
        LOG_INF("Disk mounted.\n");
        lsdir(mp.mnt_point);
    } else {
        LOG_ERR("Error mounting disk.\n");
    }
}