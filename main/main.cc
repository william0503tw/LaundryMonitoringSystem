#include <Arduino.h>
#include <Quectel_BC260.h>
#include <stdio.h>

#define SERIAL_PORT Serial1
QuectelBC260 quectel = QuectelBC260(false);
char IMEI[16];

extern "C"{
void app_main(void);
}

char* formatTimeStr(const struct tm timeStruct) {
    // Allocate memory for the result string
    // Example format: "Year: 2023, Month: 04, Day: 05, Hour: 12, Minute: 01, Second: 59"
    char* result = (char*) malloc(100 * sizeof(char));  // Adjust size as needed
    if (result == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Format the time into the allocated string using the input struct tm
    sprintf(result, "Year: %d, Month: %02d, Day: %02d, Hour: %02d, Minute: %02d, Second: %02d",
            timeStruct.tm_year + 1900,  // tm_year is years since 1900
            timeStruct.tm_mon + 1,      // tm_mon is months since January (0-11), so add 1 for 1-12
            timeStruct.tm_mday,         // tm_mday is day of the month (1-31)
            timeStruct.tm_hour,         // tm_hour is hours since midnight (0-23)
            timeStruct.tm_min,          // tm_min is minutes after the hour (0-59)
            timeStruct.tm_sec);         // tm_sec is seconds after the minute (0-60, including leap second)

    return result;
}

void app_main(void) {
    quectel.begin(&SERIAL_PORT);
    quectel.setDeepSleep();
    quectel.getData();

    printf("IMEI: %s\n", quectel.getIMEI());
    printf("Firmware version: %s\n", quectel.getFirmwareVersion());
    printf("Timezone: %d\n", quectel.engineeringData.timezone);

    while(1){
        printf("Time: %s\n", formatTimeStr(quectel.getStruct_DateAndTime()));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    

    while(1);
}
