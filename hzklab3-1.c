/*
 * hzklab3-1.c
 *
 *  Created on: Feb 16, 2016
 *      Author: zhqk6
 */
#ifndef MODULE
#define MODULE
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include<linux/module.h>
#include<linux/kernel.h>
#include<asm/io.h>
#include<rtai.h>
#include<rtai_sched.h>
#include<rtai_sem.h>
#include <linux/delay.h>
#include "ece4220lab3.h"

MODULE_LICENSE("GPL");

SEM sem;
static RT_TASK mytask;
RTIME period;

static void rt_process(int t)
{
unsigned long *pbdr,*pbddr,*ptr;
ptr = (unsigned long*)__ioremap(0x80840000,4096,0);
pbdr = ptr+1;
pbddr = ptr+5;
*pbddr|= 0xE0;    //set pbddr to 11100000
while(1){
	*pbdr&=0x00;   //initialize pbdr to 00000000
	while(1-check_button()){   // if the button is not pressed, check_button returns 0
		*pbdr&=0x00;
		*pbdr|=0x80;   // the red one, pbdr is 10000000
		rt_sleep(500*period);
		*pbdr&=0x00;
		*pbdr|=0x20;   // the green one, pbdr is 00100000
		rt_sleep(500*period);
		*pbdr&=0x00;
	}
	*pbdr|=0x40;      // the yellow one, pbdr is 01000000
	rt_sleep(500*period);
	clear_button();
}
}

int init_module(void){
	rt_set_periodic_mode();
    period = start_rt_timer(nano2count(1000000));
    rt_task_init(&mytask,rt_process,0,256,0,0,0);
    rt_task_resume(&mytask);
    return 0;
}

void cleanup_module(void){
	rt_task_delete(&mytask);
	stop_rt_timer();
}
