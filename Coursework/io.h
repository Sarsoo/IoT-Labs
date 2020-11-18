#ifndef _IO_GUARD
#define _IO_GUARD

#include "dev/light-sensor.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"

void 
initIO()
{
    SENSORS_ACTIVATE(light_sensor);
    SENSORS_ACTIVATE(button_sensor);
    leds_off(LEDS_ALL);
}

// get float from light sensor including transfer function
float
getLight(void)
{
    int lightData = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);

    float V_sensor = 1.5 * lightData / 4096;
    float I = V_sensor/1e5;
    float light = 0.625 * 1e6 * I * 1000;
    return light;
}

#endif
