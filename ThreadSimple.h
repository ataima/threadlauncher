/*
 * File:   ThreadScheduler.h
 * Author: angelo
 *
 * Created on 29 marzo 2019, 9.29
 */

#ifndef THREADSCHEDULER_H
#define THREADSCHEDULER_H



#include "Thread.h"

class caThreadSimple
{
private:
    caThreadStatus mStatus;
    pthread_t *mThid;
    functor reqFunc;
    void *reqParam;

    int ExecuteClient(void);
    void CleanUp(void);

public:
    caThreadSimple();
    ~caThreadSimple();
    bool InitThread(functor entry, void *param);
    void DestroyThread(void);
    void JoinThread(void);
    void SleepThread(unsigned int delay);

    caThreadStatus getStatus(void)
    {
        return mStatus;
    }

    pthread_t * getThreadId(void)
    {
        return mThid;
    }
public:
    static void * entry_point(void *param);
    static void cleanup_point(void *param);

};


#endif /* THREADSCHEDULER_H */

