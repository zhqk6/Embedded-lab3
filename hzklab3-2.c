/*
 * hzklab3-2.c
 *
 *  Created on: Feb 23, 2016
 *      Author: zhqk6
 */

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_sem.h>
#include "ece4220lab3.h"

MODULE_LICENSE("GPL");

SEM sem;
static RT_TASK mytask1;
static RT_TASK mytask2;
static RT_TASK mytask3;
RTIME period;
unsigned long *pbdr,*pbddr,*ptr;

static void task1(int t){
ptr = (unsigned long*)__ioremap(0x80840000,4096,0);
pbdr = ptr+1;
pbddr = ptr+5;
*pbddr|= 0xE0;    //set pbddr to 11100000
rt_sem_wait(&sem);
while(check_button()){   // if button is pressed, the loop begins
	rt_sem_signal(&sem);
	*pbdr&=0x00;   //initialize pbdr to 00000000
	*pbdr|=0x80;   // green
	rt_sleep(300*period);
	*pbdr&=0x00;
}
clear_button();
rt_sem_delete(&sem);
rt_task_wait_period();
//Release the semaphore
}
static void task2(int t){
	ptr = (unsigned long*)__ioremap(0x80840000,4096,0);
	pbdr = ptr+1;
	pbddr = ptr+5;
	*pbddr|= 0xE0;
	rt_sem_wait(&sem);
	while(1){
	rt_sem_signal(&sem);
		*pbdr&=0x00;   //initialize pbdr to 00000000
		*pbdr|=0x40;   //yellow
		rt_sleep(300*period);
		*pbdr&=0x00;
	}
	rt_sem_delete(&sem);
	rt_task_wait_period();
	//Release the semaphore
	}
static void task3(int t){
	ptr = (unsigned long*)__ioremap(0x80840000,4096,0);
	pbdr = ptr+1;
	pbddr = ptr+5;
	*pbddr|= 0xE0;
	rt_sem_wait(&sem);
	while(1){
	rt_sem_signal(&sem);
	*pbdr&=0x00;   //initialize pbdr to 00000000
	*pbdr|=0x20;   // red
	rt_sleep(300*period);
	*pbdr&=0x00;

}
rt_sem_delete(&sem);
//Release the semaphore
rt_task_wait_period();
}


int init_module(void){
	rt_set_periodic_mode();
    period = start_rt_timer(nano2count(1000000));
    rt_sem_init(&sem,1);
    rt_task_init(&mytask1,task1,0,256,0,0,0);
    rt_task_init(&mytask2,task2,0,256,1,0,0);
    rt_task_init(&mytask3,task3,0,256,2,0,0);
    rt_task_make_periodic(&mytask1,rt_get_time()+0*period,1000*period);
    rt_task_make_periodic(&mytask2,rt_get_time()+1000*period,1000*period);
    rt_task_make_periodic(&mytask3,rt_get_time()+1000*period,1000*period);
	rt_task_resume(&mytask1);
	rt_task_resume(&mytask2);
	rt_task_resume(&mytask3);
    return 0;
}

void cleanup_module(void){
	rt_task_delete(&mytask1);
	rt_task_delete(&mytask2);
	rt_task_delete(&mytask3);
    stop_rt_timer();
}
