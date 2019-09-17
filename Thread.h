/*
 * File:   Thread.h
 * Author: angelo
 *
 * Created on 29 marzo 2019, 9.31
 */

#ifndef THREAD_H
#define THREAD_H


typedef void * (*functor)(void *);
typedef void (*cleanctor)(size_t, int);

typedef enum tag_thread_status
{
    STOPPED,
    WAIT_SIGNAL,
    RUNNING,
    RESUME,
    TRY_EXIT,
    EXITED
} caThreadStatus;

typedef enum tag_thread_mode
{
    WAIT_ALWAYS,
    NO_WAIT,
    WAIT_ONLY_START,
    REQ_EXIT
} caThreadMode;

typedef enum tag_thread_schedule_mode
{
    ROUND_ROBIN,
    MORE_INCR,
    MODE_USER,
} caSchedulerPriorityMode;


#define MOREDEBUG 0

#if MOREDEBUG
#define HERE() std::cerr<<__func__<< "  : "<<__LINE__ <<std::endl;
#define HERE1() std::cerr<<mName<<" : "<<__func__<< "  : "<<__LINE__ << \
              "  status="<<this->mStatus<<"  mode="<<this->mMode<<"  index="<<(int)(unsigned long int)reqParam<<std::endl;
#else
#define HERE()
#define HERE1()
#endif

#endif /* THREAD_H */

