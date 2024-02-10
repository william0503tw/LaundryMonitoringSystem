#include "feature_provider.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cstdint>

#include "esp_dsp.h"
#include "esp_system.h"
#include "esp_timer.h"

// Window coefficients
float _wind[SAMPLE_SIZE];

// working complex array
float result[SAMPLE_SIZE * 2];

float* FFT_result = &result[0];

void _findMinMax(float* array, int size, float* min, float* max) {
    *min = *max = array[0];
    for (int i = 1; i < size; ++i) {
        if (array[i] < *min) *min = array[i];
        if (array[i] > *max) *max = array[i];
    }
}

void _normalize(float* array, int size) {
    float min, max;
    _findMinMax(array, size, &min, &max);
    for (int i = 0; i < size; ++i) {
        array[i] = (array[i] - min) / (max - min) * 1024;
    }
}

bool feature_provider_begin() {
    esp_err_t ret;
    ret = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    if (ret != ESP_OK) {
        ESP_LOGE("feature_provider_begin", "Not possible to initialize FFT. Error = %i", ret);
        return false;
    } else {
        ESP_LOGI("feature_provider_begin", "Start Feature Provider");
        return true;
    }
}

void generate_feature_one_slice(int32_t input_audio_sample[], float band_result[]) {
    _dc_remove(input_audio_sample, SAMPLE_SIZE);

    for (int i = 0; i < SAMPLE_SIZE; i++) {
        result[i * 2 + 0] = static_cast<float>(input_audio_sample[i]);  // Real
        result[i * 2 + 1] = 0;                                          // Img
    }

    // FFT
    dsps_fft2r_fc32_ae32(result, SAMPLE_SIZE);

    // Bit reverse
    dsps_bit_rev_fc32(result, SAMPLE_SIZE);

    // Separate
    dsps_cplx2reC_fc32(result, SAMPLE_SIZE);

    for (int i = 0; i < (SAMPLE_SIZE / 2); i++) {
        FFT_result[i] = sqrtf((FFT_result[i * 2 + 0] * FFT_result[i * 2 + 0] + FFT_result[i * 2 + 1] * FFT_result[i * 2 + 1]));
        //printf("%d: %.2f\n",i, FFT_result[i]);
    }

    double temp = 0;
    int count = 0;
    temp += FFT_result[0];
    for (int i = 1; i < (SAMPLE_SIZE >> 1); i++) {
        if (i % bin_perBand != 0) {
            temp += FFT_result[i];
        } else {
            band_result[count] = temp / bin_perBand;
            temp = 0;
            count++;
            if (i != 0) temp += FFT_result[i];
        }
    }
    band_result[count] = temp / bin_perBand;

    _normalize(band_result, BAND_SIZE);
}

void _dc_remove(int32_t input[], int size) {
    int64_t mean = 0;
    for (int i = 0; i < size; i++) {
        mean += input[i];
    }
    mean /= size;

    for (int i = 0; i < size; i++) {
        input[i] -= mean;
    }
}
