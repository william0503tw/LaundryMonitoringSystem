#ifndef _FEATURE_PROVIDER_H_
#define _FEATURE_PROVIDER_H_

#include <cstdint>

#include "device_config.h"

void generate_feature_one_slice(int32_t input_audio_sample[], float* band_result);
bool feature_provider_begin();
float moving_average_filter(float input);
void _dc_remove(int32_t input[], int size);

#endif