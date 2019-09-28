/*
 * File:   Thread.h
 * Author: angelo
 *
 * Created on 29 marzo 2019, 9.31
 */

#ifndef THREAD_H
#define THREAD_H


typedef void * (*functor)(void *);
typedef void (*cleanctor)(int, int);

typedef enum tag_thread_status
{
    STOPPED,
    WAIT_SIGNAL,
    RUNNING,
    RESUME,
    TRY_EXIT,
    EXITED
} caThreadStatus;




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

