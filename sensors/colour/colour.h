#ifndef COLOUR_H
#define COLOUR_H

#include <stdint.h>

#define MAP(pulse_dur, no_light, white_light) ((pulse_dur - no_light) * (255 - 0) / (white_light - no_light) + 0)

typedef enum {
    NO_FILTER = 1,
    RED_FILTER,
    GREEN_FILTER,
    BLUE_FILTER,
    MAX_FILTER
} colour_sensor_filter_t;

typedef struct {
    uint16_t clear;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} colour_components_t;

void colour_sensor_init(void);

/*!
    @brief Returns the current set filter
    @return colour_sensor_filter_t - NO_FILTER | RED_FILTER | GREEN_FILTER | BLUE_FILTER
*/
colour_sensor_filter_t get_current_filter();

/*!
    @brief
    This function populates the corresponsing raw values in microseconds for clear, red, green or blue 
    component of the arg based on the current filter set.

    @param colour_components_t

    @note Example: If the current filter is set to RED_FILTER, only the component.red is updated.
*/
void get_colour_sensor_reading(colour_components_t *components);

/*!
    @brief
    This function increments the filter and sets the current filter 
    in the order of declaration of the colour_sensor_filter_t enum
*/
void colour_sensor_iteration(void);

void colour_sensor_callback(uint32_t pin_state);

#endif /* COLOUR_H */