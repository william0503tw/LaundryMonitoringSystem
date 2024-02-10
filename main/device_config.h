#ifndef _DEVICE_CONFIG_H_
#define _DEVICE_CONFIG_H_

#define USB_DEBUG_MODE

#define DURATION 2100 // in second

//-------------------- Sound and rotation algorithms --------------------
// Audio Processing configuration
#define SAMPLE_RATE 20480
#define SAMPLE_SIZE 512

#define ROW_COUNT 64
#define BAND_SIZE 64

// FFT
#define freq_perBin_hz  (SAMPLE_RATE / SAMPLE_SIZE) ;
const int bin_perBand ((SAMPLE_SIZE / 2) / BAND_SIZE);

#define MESSAGE_PACKAGE_INIT_SIZE 500
//------------------------------------------------------------------------


//------------------- Pinout configuration --------------------------------
#define battery_pin GPIO_NUM_33
#define buzzer_pin GPIO_NUM_4

#define power_on_pin GPIO_NUM_25
#define usb_detect_pin GPIO_NUM_9

#define IMU_SCL GPIO_NUM_22
#define IMU_SDA GPIO_NUM_21
//------------------------------------------------------------------------


#endif