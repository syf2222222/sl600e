/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
/*  Porting by Michael Vysotsky <michaelvy@hotmail.com> August 2011   */

#define SYS_ARCH_GLOBALS



/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"

#include "arch/sys_arch.h"

//////////////////////////////////////////////
#include "os_cfg_app.h"
//////////////////////////////////////////////
/*----------------------------------------------------------------------------*/
/*                      DEFINITIONS                                           */
/*----------------------------------------------------------------------------*/
#define LWIP_ARCH_TICK_PER_MS       (1000/OS_CFG_TICK_RATE_HZ)

#if defined(LWIP_SOCKET_SET_ERRNO) && defined(LWIP_PROVIDE_ERRNO)
int errno;
#endif

   

/*----------------------------------------------------------------------------*/
/*                      VARIABLES                                             */
/*----------------------------------------------------------------------------*/
static OS_MEM StackMem;

const void * const pvNullPointer = (mem_ptr_t*)0xffffffff;
//#pragma data_alignment=4
__align(4)
CPU_STK       LwIP_Task_Stk[LWIP_TASK_MAX*LWIP_STK_SIZE];
//#pragma data_alignment=4
__align(4)
CPU_INT08U    LwIP_task_priopity_stask[LWIP_TASK_MAX];
OS_TCB        LwIP_task_TCB[LWIP_TASK_MAX];


/*----------------------------------------------------------------------------*/
/*                      PROTOTYPES                                            */
/*----------------------------------------------------------------------------*/
/*--------------------Creates an empty mailbox.-------------------------------*/
  

err_t sys_mbox_new( sys_mbox_t *mbox, int size)
{
  OS_ERR       ucErr;
      
  OSQCreate(mbox,"LWIP quiue", size, &ucErr); 
  LWIP_ASSERT( "OSQCreate ", ucErr == OS_ERR_NONE );
  
  if( ucErr == OS_ERR_NONE){ 
    return 0; 
  }
  return -1;
}

/*-----------------------------------------------------------------------------------*/
/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void
sys_mbox_free(sys_mbox_t * mbox)
{
    OS_ERR     ucErr;
    LWIP_ASSERT( "sys_mbox_free ", mbox != SYS_MBOX_NULL );      
        
    OSQFlush(mbox,& ucErr);
    
    OSQDel(mbox, OS_OPT_DEL_ALWAYS, &ucErr);
    LWIP_ASSERT( "OSQDel ", ucErr == OS_ERR_NONE );
}

/*-----------------------------------------------------------------------------------
 *   Posts the "msg" to the mailbox.
 */
void
sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  OS_ERR     ucErr;
  CPU_INT08U  i=0; 
	if( msg == NULL ) msg = (void*)&pvNullPointer;
  /* try 10 times */
  while(i<10){
    OSQPost(mbox, msg,0,OS_OPT_POST_ALL,&ucErr);
    if(ucErr == OS_ERR_NONE)
      break;
    i++;
    OSTimeDly(5,OS_OPT_TIME_DLY,&ucErr);
  }
  LWIP_ASSERT( "sys_mbox_post error!\n", i !=10 );  
}

/* Try to post the "msg" to the mailbox. */
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  OS_ERR     ucErr;
  if(msg == NULL ) msg = (void*)&pvNullPointer;  
  OSQPost(mbox, msg,0,OS_OPT_POST_ALL,&ucErr);    
  if(ucErr != OS_ERR_NONE){
    return ERR_MEM;
  }
  return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP. 
*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{ 
  OS_ERR	ucErr;
  OS_MSG_SIZE   msg_size;
  CPU_TS        ucos_timeout;  
	void	*temp;
  CPU_TS        in_timeout = timeout/LWIP_ARCH_TICK_PER_MS;
	
	
  if(timeout && in_timeout == 0)
    in_timeout = 1;
//void  *OSQPend (OS_Q         *p_q,       //��Ϣ����ָ��
//                OS_TICK       timeout,   //�ȴ����ޣ���λ��ʱ�ӽ��ģ�
//                OS_OPT        opt,       //ѡ��
//                OS_MSG_SIZE  *p_msg_size,//������Ϣ��С����λ���ֽڣ�
//                CPU_TS       *p_ts,      //��ȡ�ȵ���Ϣʱ��ʱ���
//                OS_ERR       *p_err)     //���ش�������	
  temp  = OSQPend (mbox,in_timeout,OS_OPT_PEND_BLOCKING,&msg_size, 
                          &ucos_timeout,&ucErr);
	
	if(msg != NULL)
	{
		if( temp == (void*)&pvNullPointer )
		{
			*msg = NULL;
		}
		else
		{
			*msg = temp;
		}
	}
//	printf("\r\nmbox_fetch02");
	if ( ucErr == OS_ERR_TIMEOUT )  			//if ( ucErr == OS_ERR_TIMEOUT ) 
      ucos_timeout = SYS_ARCH_TIMEOUT;  
  return ucos_timeout; 
}
#if 0
u32_t sys_arch_mbox_fetch(sys_mbox_t mbox, void **msg, u32_t timeout)
{
	u8_t	ucErr;
	u32_t	ucos_timeout, timeout_new;
	void	*temp;
	
	if(timeout != 0)
	{
		ucos_timeout = (timeout * OS_TICKS_PER_SEC)/1000; /* convert to timetick */
		
		if(ucos_timeout < 1)
		{
			ucos_timeout = 1;
		}
		else if(ucos_timeout > 600)	/* ucOS only support u16_t timeout */
		{
			ucos_timeout = 600;			
		}
	}
	else 
	{
		ucos_timeout = 0;
	}

	timeout = OSTimeGet();			
	temp = OSQPend( mbox->pQ, (u16_t)ucos_timeout, &ucErr );
	if(msg != NULL)
	{
		if( temp == (void*)&pvNullPointer )
		{
			*msg = NULL;
		}
		else
		{
			*msg = temp;
		}
	}   
	
	if ( ucErr == OS_ERR_TIMEOUT ) 
	{
		timeout = SYS_ARCH_TIMEOUT;
	}
	else
	{
		LWIP_ASSERT( "OSQPend ", ucErr == OS_ERR_NONE );	
		timeout_new = OSTimeGet();
		if (timeout_new>timeout) 
		{
			timeout_new = timeout_new - timeout;
		}
		else 
		{
			timeout_new = 0xffffffff - timeout + timeout_new;
		}
		timeout = timeout_new * 1000 / OS_TICKS_PER_SEC + 1;  /* convert to milisecond */
	}
	
	return timeout; 
}
#endif
/*-----------------------------------------------------------------------------------*/
/** 
  * Check if an mbox is valid/allocated: 
  * @param sys_mbox_t *mbox pointer mail box
  * @return 1 for valid, 0 for invalid 
  */ 
int sys_mbox_valid(sys_mbox_t *mbox)
{
  if(mbox->NamePtr)  
    return (strcmp(mbox->NamePtr,"?Q"))? 1:0;
  else
    return 0;
}
/** 
  * Set an mbox invalid so that sys_mbox_valid returns 0 
  */      
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  if(sys_mbox_valid(mbox))
    sys_mbox_free(mbox);
}
/*
 *  Creates and returns a new semaphore. The "count" argument specifies
 *  the initial state of the semaphore. TBD finish and test
 */

err_t sys_sem_new(sys_sem_t * sem, u8_t count)
{  
  OS_ERR	ucErr;
  OSSemCreate (sem,"LWIP Sem",count,&ucErr);
  if(ucErr != OS_ERR_NONE ){
    LWIP_ASSERT("OSSemCreate ",ucErr == OS_ERR_NONE );
    return -1;    
  }
  return 0;
}
/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that lwIP implements a function with a similar name,
  sys_sem_wait(), that uses the sys_arch_sem_wait() function.
*/
u32_t
sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{ 
  OS_ERR	ucErr;
  CPU_TS        ucos_timeout;
  CPU_TS        in_timeout = timeout/LWIP_ARCH_TICK_PER_MS;
	
  if(timeout && in_timeout == 0)
    in_timeout = 1;
	
//	if(in_timeout==0) in_timeout=30000;
	
//	printf("\r\nsys_arch_sem_wait0,in_timeout=%d",in_timeout);
  OSSemPend (sem,in_timeout,OS_OPT_PEND_BLOCKING,&ucos_timeout,&ucErr);
    /*  only when timeout! */
//	printf("\r\nsys_arch_sem_wait1");
  if(ucErr == OS_ERR_TIMEOUT)
      ucos_timeout = SYS_ARCH_TIMEOUT;	
  return ucos_timeout;
}

/*
 *       Signals a semaphore
 */

void
sys_sem_signal(sys_sem_t *sem)
{
  OS_ERR	ucErr;  
  OSSemPost(sem,OS_OPT_POST_ALL,&ucErr);
  LWIP_ASSERT("OSSemPost ",ucErr == OS_ERR_NONE );  
}

/*
 *      Deallocates a semaphore
 */
void
sys_sem_free(sys_sem_t *sem)
{
    OS_ERR     ucErr;
    OSSemDel(sem, OS_OPT_DEL_ALWAYS, &ucErr );
    LWIP_ASSERT( "OSSemDel ", ucErr == OS_ERR_NONE );
}
int sys_sem_valid(sys_sem_t *sem)
{
  if(sem->NamePtr)
    return (strcmp(sem->NamePtr,"?SEM"))? 1:0;
  else
    return 0;
}

/** Set a semaphore invalid so that sys_sem_valid returns 0 */
void sys_sem_set_invalid(sys_sem_t *sem)
{
  if(sys_sem_valid(sem))
    sys_sem_free(sem);
}
/*-----------------------------------------------------------------------------------*/
/*            memory interface                                                       */
/*-----------------------------------------------------------------------------------*/

/**
 * Zero the heap and initialize start, end and lowest-free
 */
//void mem_init(void)
//{

//}

/*
 * Initialize sys arch
 */
void
sys_init(void)
{
  OS_ERR ucErr;
  memset(LwIP_task_priopity_stask,0,sizeof(LwIP_task_priopity_stask));
  /* init mem used by sys_mbox_t, use ucosII functions */
  OSMemCreate(&StackMem,"LWIP TASK STK",(void*)LwIP_Task_Stk,LWIP_TASK_MAX,LWIP_STK_SIZE*sizeof(CPU_STK),&ucErr);
  LWIP_ASSERT( "sys_init: failed OSMemCreate STK", ucErr == OS_ERR_NONE );
}
/*-----------------------------------------------------------------------------------*/
                                      /* Mutexes*/
/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
#if LWIP_COMPAT_MUTEX == 0

//	OSMutexCreate (&dm9000_sem_lock, //�����ź���ָ��
//                 "dm9000_sem_lock",  //ȡ�ź���������
//                  &err);   //���ش�������
		
////						�����ź���ָ��	//��ʱʱ�䣨���ģ� //ѡ��	 //ʱ���-����ȡ //���ش�������
//	OSMutexPend (&dm9000_sem_lock, 0, 	OS_OPT_PEND_BLOCKING, 0, &err); 
////						�����ź���ָ��	 	//ѡ��	 			//���ش�������	
//	OSMutexPost (&dm9000_sem_lock, OS_OPT_POST_1, &err); 
									 

/* Create a new mutex*/
err_t sys_mutex_new(sys_mutex_t *mutex) {
OS_ERR     ucErr;
	OSMutexCreate (mutex, "mutex", &ucErr); 
	return ucErr;
}
/*-----------------------------------------------------------------------------------*/
/* Deallocate a mutex*/
void sys_mutex_free(sys_mutex_t *mutex)
{
OS_ERR     ucErr;
  ////						�����ź���ָ��	 	//ѡ��	 			//���ش�������	
//	OSMutexPost (mutex, OS_OPT_POST_1, &ucErr); 
//	OSMutexDel() ɾ��һ���������ź���
	OSMutexDel (mutex,OS_OPT_POST_NONE, &ucErr);//
}
/*-----------------------------------------------------------------------------------*/
/* Lock a mutex*/
void sys_mutex_lock(sys_mutex_t *mutex)
{
	OS_ERR     ucErr;
//						�����ź���ָ��	//��ʱʱ�䣨���ģ� //ѡ��	 //ʱ���-����ȡ //���ش�������
	OSMutexPend (mutex, 0, 	OS_OPT_PEND_BLOCKING, 0, &ucErr); 
}

/*-----------------------------------------------------------------------------------*/
/* Unlock a mutex*/
void sys_mutex_unlock(sys_mutex_t *mutex)
{
	OS_ERR     ucErr;
  ////						�����ź���ָ��	 	//ѡ��	 			//���ش�������	
	OSMutexPost (mutex, OS_OPT_POST_NONE, &ucErr); 
}
#endif /*LWIP_COMPAT_MUTEX*/


/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
// TBD 
/*-----------------------------------------------------------------------------------*/
/*
  Starts a new thread with priority "prio" that will begin its execution in the
  function "thread()". The "arg" argument will be passed as an argument to the
  thread() function. The id of the new thread is returned. Both the id and
  the priority are system dependent.
*/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	 int i; 
	  OS_ERR      ucErr;
    CPU_INT08U  ubPrio = LWIP_TASK_START_PRIO;
   CPU_STK * task_stk ;
    int tsk_prio ;
	arg = arg;
   
    if(prio){
      ubPrio +=(prio-1);
      for(i=0; i<LWIP_TASK_MAX; ++i)
        if(LwIP_task_priopity_stask[i] == ubPrio)
          break;
      if(i == LWIP_TASK_MAX){
        for(i=0; i<LWIP_TASK_MAX; ++i)
          if(LwIP_task_priopity_stask[i]==0){
            LwIP_task_priopity_stask[i] = ubPrio;
            break;
          }
        if(i == LWIP_TASK_MAX){
          LWIP_ASSERT( "sys_thread_new: there is no space for priority", 0 );
          return (-1);
        }        
      }else
        prio = 0;
    }
  /* Search for a suitable priority */     
    if(!prio){
      ubPrio = LWIP_TASK_START_PRIO;
      while(ubPrio < (LWIP_TASK_START_PRIO+LWIP_TASK_MAX)){ 
        for(i=0; i<LWIP_TASK_MAX; ++i)
          if(LwIP_task_priopity_stask[i] == ubPrio){
            ++ubPrio;
            break;
          }
        if(i == LWIP_TASK_MAX)
          break;
      }
      if(ubPrio < (LWIP_TASK_START_PRIO+LWIP_TASK_MAX))
        for(i=0; i<LWIP_TASK_MAX; ++i)
          if(LwIP_task_priopity_stask[i]==0){
            LwIP_task_priopity_stask[i] = ubPrio;
            break;
          }
      if(ubPrio >= (LWIP_TASK_START_PRIO+LWIP_TASK_MAX) || i == LWIP_TASK_MAX){
        LWIP_ASSERT( "sys_thread_new: there is no free priority", 0 );
        return (-1);
      }
    }
    if(stacksize > LWIP_STK_SIZE || !stacksize)   
        stacksize = LWIP_STK_SIZE;
  /* get Stack from pool */
   // CPU_STK * task_stk = OSMemGet( &StackMem, &ucErr );
		task_stk = OSMemGet( &StackMem, &ucErr );
    if(ucErr != OS_ERR_NONE){
      LWIP_ASSERT( "sys_thread_new: impossible to get a stack", 0 );
      return (-1);
    } 
    //int 
		tsk_prio = ubPrio-LWIP_TASK_START_PRIO;
    OSTaskCreate(&LwIP_task_TCB[tsk_prio],
                 (CPU_CHAR  *)name,
                 (OS_TASK_PTR)thread, 
                 (void      *)0,
                 (OS_PRIO    )ubPrio,
                 (CPU_STK   *)&task_stk[0],
                 (CPU_STK    )(stacksize/10),
                 (CPU_STK_SIZE)stacksize,
                 (OS_MSG_QTY )0,
                 (OS_TICK    )0,
                 (void      *)0,
                 (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR    *)&ucErr);    
								// printf("\r\n ubPrio:%d  stacksize:%d  tsk_prio:%d \r\n ",ubPrio,stacksize,tsk_prio);
    
    return ubPrio;
}

/*
  This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions. An embedded system which supports ISR-based drivers might
  want to implement this function by disabling interrupts. Task-based systems
  might want to implement this by using a mutex or disabling tasking. This
  function should support recursive calls from the same task or interrupt. In
  other words, sys_arch_protect() could be called while already protected. In
  that case the return value indicates that it is already protected.

  sys_arch_protect() is only required if your port is supporting an operating
  system.

  Note: This function is based on FreeRTOS API, because no equivalent CMSIS-RTOS
        API is available
*/
sys_prot_t sys_arch_protect(void)
{
	CPU_SR_ALLOC(); 
//	CPU_SR  cpu_sr = (CPU_SR)0
	CPU_CRITICAL_ENTER();
	(void)cpu_sr;//�������
  return (sys_prot_t)1;
}


/*
  This optional function does a "fast" set of critical region protection to the
  value specified by pval. See the documentation for sys_arch_protect() for
  more information. This function is only required if your port is supporting
  an operating system.

  Note: This function is based on FreeRTOS API, because no equivalent CMSIS-RTOS
        API is available
*/
void sys_arch_unprotect(sys_prot_t pval)
{
	CPU_SR_ALLOC(); 
//  ( void ) pval;
//  osMutexRelease(lwip_sys_mutex);
	CPU_CRITICAL_EXIT();
}
/**
 * Sleep for some ms. Timeouts are NOT processed while sleeping.
 *
 * @param ms number of milliseconds to sleep
 */
//void
//sys_msleep(u32_t ms)
//{
//  OS_ERR      ucErr;  
//  OSTimeDly(ms,OS_OPT_TIME_DLY,&ucErr);  
//}

