# zephyr_spi_sdcard
Basic read and write operations from/to sd card connected via SPI. Includes test functions to measure performance of sd cards.

## Test setup:
- Discovery Kit: Disco_L475_IoT
- SD Card: 16GB microSD Samsung Class 6
- Zephyr OS: 2.3.0
- Tests include changes from Pull Request: https://github.com/zephyrproject-rtos/zephyr/pull/26319


### Measurement Results
R/W | Performance
| ------------ | -------------| 
Read 1MB |227,5 KByte/s
Write 1MB |75,6 KByte/s
Read 5MB | 227,4 KByte/s
Write 5MB | 79,5 KByte/s

## How to use:
- Connect SD Card to SPI2 on Disco_L475_IoT according to layout from proj.conf
- Check if SPI interface is correctly setup in pinmux.c 
- Connect via Serial Terminal to COM Portto check outout (see proj.conf) or 
 ```bash
 minicom -D /dev/ttyACM<Numbr> -b 921600"
 ```

Sample code to call test functions:
```C
extern struct fs_file_t sd_file;

/**
* Prints information about SD Card like Sector Size
* Manufactor ID, Card size etc.
*/
sd_card_info();

// Start SD Card writing tests
if (test_write_speed(sd_file) != 0) {
    LOG_ERR("Error writing sd card.\n");
}

// Start SD Card reading tests
if (test_read_speed(sd_file) != 0) {
    LOG_ERR("Error reading sd card.\n");
}
```

Output:

```
Listing dir /SD: ...
[FILE] TEST (size = 32845824)
[FILE] IMU.CSV (size = 3584)
[FILE] TEST.LOG (size = 16388096)

----- Testing sd card write performance -----
Test 1: write a file to sd card with size of 1024000 bytes
Performance Result - write time for 1024000 byte: 12895 ms
----- End: testing SD Card write performance: Passed -----

----- Testing sd card read performance -----
Test 2: read a file from sd card with size of 1024000 bytes
Performance Result - read time for 1024000 byte: 4695 ms
----- End: testing SD Card read performance: Passed -----
```

Sources:
- Zephyr OS [SPI API](https://docs.zephyrproject.org/latest/reference/peripherals/spi.html)
- Zephyr OS FAT Sample: [FAT File Sample](https://docs.zephyrproject.org/latest/samples/subsys/fs/fat_fs/README.html)
- Zephyr OS [SDHC](https://docs.zephyrproject.org/latest/reference/storage/disk/sdhc.html#sdhc-api)




