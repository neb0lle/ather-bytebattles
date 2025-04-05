#ifndef GPIO_CFG_H
#define GPIO_CFG_H

/* ASDK User Action: Define new pins here */

#define USER_LED_1  MCU_PIN_77
#define USER_LED_2  MCU_PIN_47

#define USER_BUTTON MCU_PIN_29

#define IR1_SENSE   MCU_PIN_78

#define RAIN1_SENSE MCU_PIN_32

#define ULTRASONIC_ECHO1   MCU_PIN_68
#define ULTRASONIC_TRIG1   MCU_PIN_67

#define ULTRASONIC_ECHO2 0xFF // To be defined by user, refer ULTRASONIC_ECHO1
#define ULTRASONIC_ECHO3 0xFE // To be defined by user, refer ULTRASONIC_ECHO1
#define ULTRASONIC_ECHO4 0xFD // To be defined by user, refer ULTRASONIC_ECHO1

#endif /* GPIO_CFG_H */
