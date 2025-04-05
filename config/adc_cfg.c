#include "asdk_platform.h"
#include "app_adc.h"

#include "adc_cfg.h"
#include "defaults.h"

/* ASDK User Action: Add new ADC pins here */

asdk_mcu_pin_t adc_pins[] = {
    LDR_ADC_PIN,
};

/* ******** CAUTION! Do not edit below code ******** */

asdk_adc_config_t adc_conf = { 
    .pin_count = sizeof(adc_pins) / sizeof(adc_pins[0]), 
    .pin_nums = adc_pins, 
    .enable_interrupt = true, 
    .intr_num = ASDK_EXTI_INTR_CPU_4, 
    .interrupt_priority = 3, 
};
