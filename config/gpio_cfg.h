#ifndef GPIO_CFG_H
#define GPIO_CFG_H

/* ASDK User Action: Define new pins here */

#define USER_LED_1  MCU_PIN_77
#define USER_LED_2  MCU_PIN_47

#define USER_BUTTON MCU_PIN_29

#define IR1_SENSE   MCU_PIN_90

#define RAIN1_SENSE MCU_PIN_73

#define ULTRASONIC_ECHO1   MCU_PIN_54
#define ULTRASONIC_TRIG1   MCU_PIN_78

#define ULTRASONIC_ECHO2 0xFF // To be defined by user, refer ULTRASONIC_ECHO1
#define ULTRASONIC_ECHO3 0xFE // To be defined by user, refer ULTRASONIC_ECHO1
#define ULTRASONIC_ECHO4 0xFD // To be defined by user, refer ULTRASONIC_ECHO1

#define COLOUR_SENSOR_S2    MCU_PIN_17
#define COLOUR_SENSOR_S3    MCU_PIN_14
#define COLOUR_SENSOR       MCU_PIN_71

#endif /* GPIO_CFG_H */
