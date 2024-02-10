#ifndef _SD_CARD_H_
#define _SD_CARD_H_

#define PIN_NUM_MISO  19
#define PIN_NUM_MOSI  23
#define PIN_NUM_CLK   18
#define PIN_NUM_CS    26

#include "time.h"

#define MOUNT_POINT "/sdcard" 

bool SD_begin();
void SD_write_data_to_log_file(char* file_name, char* data);
bool SD_is_file_exist(char *file_name);


#endif