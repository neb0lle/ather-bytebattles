/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK includes */
#include "asdk_error.h"

/* Application specific includes */
#include "app_adc.h"
#include "adc_cfg.h"

/* Debug Print includes */
#include "debug_print.h"

#define ADC_PIN_SIZE    4U

static asdk_errorcode_t status;

volatile bool adc_complete;

asdk_adc_callback_t adc_callback_params;

uint32_t sensor_raw_value[ADC_PIN_SIZE];

uint8_t ADC_Index = 0;

static uint32_t LDR_brightness_lvl;

void adc_callback(asdk_adc_callback_t params)
{
    if (ASDK_ADC_CALLBACK_REASON_CONVERSION_COMPLETE == params.callback_reason)
    {
        adc_complete = true;

        adc_callback_params.adc_pin = params.adc_pin;
        adc_callback_params.callback_reason = params.callback_reason;
    }
}

void app_adc_init()
{
    status = asdk_adc_init(&adc_conf);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_SUCCESS);

    status = asdk_adc_install_callback(adc_callback);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_SUCCESS);
}

void app_adc_start_conversion()
{
    /* Trigger ADC conversion */

    status = asdk_adc_start_conversion_non_blocking(adc_pins[ADC_Index]);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_SUCCESS);
}

void app_adc_iteration()
{   
    /* Handle sequence of conversion and also reads the converted values */

    if (adc_complete)
    {
        adc_complete = false;

        status = asdk_adc_read_conversion_value_non_blocking(adc_callback_params.adc_pin, &sensor_raw_value[ADC_Index]);
        ASDK_DEV_ERROR_ASSERT(status, ASDK_SUCCESS);

        ADC_Index++;

        if(ADC_Index >= adc_conf.pin_count)
        {
            ADC_Index = 0;
        }

        status = asdk_adc_start_conversion_non_blocking(adc_pins[ADC_Index]);
        ASDK_DEV_ERROR_ASSERT(status, ASDK_SUCCESS);
    }
}

uint32_t app_get_adc_value(asdk_mcu_pin_t p_Pin)
{
    uint32_t Value = 0;
    uint8_t Idx = 0;

    for( ; adc_conf.pin_count; Idx++)
    {
        if(p_Pin == adc_pins[Idx])
        {
            Value = sensor_raw_value[Idx];
            break;
        }
    }

    return Value;
}

void app_ldr_iteration()
{
    LDR_brightness_lvl = app_get_adc_value(LDR_ADC_PIN);

    if (LDR_brightness_lvl > 3000)
    {
        /* Take action */
    }
}
