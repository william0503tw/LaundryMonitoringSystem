#include "SD_Card.h"

#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "device_config.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "utils.h"

//---------------------------------------------------------------------------------------------
// Variable
static const char *TAG = "SD_Card";

sdmmc_card_t *card;
bool is_SD_Card_Initialized = false;

FILE *f;

//---------------------------------------------------------------------------------------------
// Private functions
// functions for data file, each data file will have differnet name, so parameter file name is needed
void _insert_data_file_title(char *file_name) {
    if (is_SD_Card_Initialized) {
        f = fopen(file_name, "a");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for appending");
            return;
        }
        for (int i = 1; i <= BAND_SIZE; i++) {
            if (i == 1) {
                fprintf(f, "Band%d", i);
            } else {
                fprintf(f, ",Band%d", i);
            }
        }
        fprintf(f, "\n");
        fclose(f);
        ESP_LOGI(TAG, "Data file Title Created");
    } else {
        ESP_LOGE(TAG, "SD Card is not initialized");
    }
}
void _create_data_file(char *file_name) {
    if (is_SD_Card_Initialized) {
        f = fopen(file_name, "w");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for writing");
            return;
        }
        fclose(f);
        ESP_LOGI(TAG, "%s File created", file_name);
    } else {
        ESP_LOGE(TAG, "SD Card is not initialized");
    }
}
char *_intToString(int num) {
    // Allocate space for the string (including the null terminator)
    char *result = (char *)malloc(3 * sizeof(char));
    if (!result) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    // Convert integer to string in the desired format
    snprintf(result, 3, "%02d", num);
    return result;
}

// End of functions for data file

void _append_file(char *file_name, char *data) {
    if (is_SD_Card_Initialized) {
        f = fopen(file_name, "a");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for appending");
            return;
        }
        fprintf(f, "%s", data);
        fclose(f);
    } else {
        ESP_LOGE(TAG, "SD Card is not initialized");
    }
}
//---------------------------------------------------------------------------------------------

bool SD_begin() {
    esp_err_t ret;
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {.format_if_mount_failed = true, .max_files = 5, .allocation_unit_size = 16 * 1024};

    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = (gpio_num_t)PIN_NUM_MOSI,
        .miso_io_num = (gpio_num_t)PIN_NUM_MISO,
        .sclk_io_num = (gpio_num_t)PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize((spi_host_device_t)host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return false;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = (gpio_num_t)PIN_NUM_CS;
    slot_config.host_id = (spi_host_device_t)host.slot;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG,
                     "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG,
                     "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
        return false;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    is_SD_Card_Initialized = true;
    return is_SD_Card_Initialized;
}

//---------------------------------------------------------------------------------------------

void SD_write_data_to_log_file(char *file_name, char *data) {
    if (is_SD_Card_Initialized) {
        if (!SD_is_file_exist(file_name)) {
            _create_data_file(file_name);
            _insert_data_file_title(file_name);
        }
        _append_file(file_name, data);
    } else {
        ESP_LOGE(TAG, "SD Card is not initialized");
    }
}

//---------------------------------------------------------------------------------------------

bool SD_is_file_exist(char *file_name) {
    if (is_SD_Card_Initialized) {
        struct stat st;

        if (file_name == NULL) {
            ESP_LOGW(TAG, "File name is NULL");
            return false;
        } else if (stat(file_name, &st) == 0) {
            // file exists
            ESP_LOGI(TAG, "%s File already exists", file_name);
            return true;
        } else {
            ESP_LOGW(TAG, "%s File does not exists create new", file_name);
            return false;
        }
    } else {
        ESP_LOGE(TAG, "SD Card is not initialized");
        return false;
    }
}

void SD_file_delete(char *file_name) {
    if (is_SD_Card_Initialized) {
        if (remove(file_name) == 0) {
            ESP_LOGI(TAG, "Deleted successfully");
        } else {
            ESP_LOGE(TAG, "Unable to delete the file");
        }
    } else {
        ESP_LOGE(TAG, "SD Card is not initialized");
    }
}
