#include "contiki.h"
#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"
#include <stdio.h> /* For printf() */

typedef unsigned short USHORT;

//print a unsigned short (as returned from rand) picewise char by char
void
putShort(USHORT in) 
{
	// recursively shift each digit of the int to units from most to least significant
	if (in >= 10)
	{
		putShort(in / 10);
	}
	// isolate unit digit from each number by modulo and add '0' char to turn integer into corresponding ascii char
	putchar((in % 10) + '0');
}

void
putFloat(float in) 
{
	if(in < 0) 
	{
		putchar('-'); // print negative sign if required
		in = -in;
	}	
	
	USHORT integerComponent = (USHORT) in; // truncate float to integer
	float fractionComponent =  (in - integerComponent) * 1000; // take fraction only and promote to integer
	if (fractionComponent - (USHORT)fractionComponent >= 0.5) fractionComponent++; // round

	putShort(integerComponent);
	putchar('.');
	putShort((USHORT) fractionComponent);
}

float getTemperature(void)
{
  	int tempData;

  	// NOTE: You only need to use one of the following
  	// If you run the code in Cooja Simulator, please remove the second one
  	tempData = sht11_sensor.value(SHT11_SENSOR_TEMP_SKYSIM); // For Cooja Sim
  	//tempData = sht11_sensor.value(SHT11_SENSOR_TEMP); // For XM1000 mote

	float d1 = -39.6f;
	float d2 = 0.01f;
  	float temp = d1 + (d2 * tempData); // you need to implement the transfer function here
  	return temp;
}

float getLight(void)
{
  	int lightData = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);

	float V_sensor = 1.5 * lightData / 4096;
	float I = V_sensor/1e5;
  	float light = 0.625 * 1e6 * I * 1000; // you need to implement the transfer function here
  	return light;
}


/*---------------------------------------------------------------------------*/
PROCESS(sensor_reading_process, "Sensor reading process");
AUTOSTART_PROCESSES(&sensor_reading_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_reading_process, ev, data)
{
  static struct etimer timer;
  PROCESS_BEGIN();
  etimer_set(&timer, CLOCK_SECOND); // you need to put the correct timer setting here
  SENSORS_ACTIVATE(light_sensor);
  SENSORS_ACTIVATE(sht11_sensor);

  static int counter = 0;
  while(1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev=PROCESS_EVENT_TIMER);

    float temp = getTemperature();
    float light_lx = getLight();

    printf("%d\n", ++counter); // you should also print the temperature and light intensity here
	printf("Temperature: ");
	putFloat(temp);
	printf(" C");
	putchar('\n');

	printf("Light: ");
	putFloat(light_lx);
	putchar('\n');

	putchar('\n');

    etimer_reset(&timer);
  }
  PROCESS_END();

}
/*---------------------------------------------------------------------------*/

