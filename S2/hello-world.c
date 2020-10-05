/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * $Id: hello-world.c,v 1.1 2006/10/02 21:46:46 adamdunkels Exp $
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#define RANDOM_NUMBER_COUNT 3
#define RANDOM_NUMBER_MAX_VALUE 2.5

#include "contiki.h"
#include "lib/random.h"

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

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
	PROCESS_BEGIN();

	printf("Printing rands\n");

	USHORT randomNumbers[RANDOM_NUMBER_COUNT]; //array for retreived random numbers
	float normalisedRandomNumbers[RANDOM_NUMBER_COUNT]; //array for those rands normalised between 0 and 2.5

	float total = 0;

	int i;
	for(i = 0; i < RANDOM_NUMBER_COUNT; i++)
	{
		USHORT nextRand = random_rand();
		float normalisedRand = ((float) nextRand / RANDOM_RAND_MAX ) * RANDOM_NUMBER_MAX_VALUE;
		
		randomNumbers[i] = nextRand;		
		normalisedRandomNumbers[i] = normalisedRand;
		total = total + normalisedRand;

		putShort(nextRand);
		putchar('\n');

		putFloat(normalisedRand);
		putchar('\n');putchar('\n');
	}	

	printf("Printed rands\n");

	printf("Total of random numbers: ");
	putFloat(total);
	putchar('\n');

  	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
