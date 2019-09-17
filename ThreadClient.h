/*
 * File:   ThreadClient.h
 * Author: angelo
 *
 * Created on 28 marzo 2019, 14.11
 */

#ifndef THREADCLIENT_H
#define THREADCLIENT_H


#include "Thread.h"

class caThreadClient
{
protected:
    caThreadStatus mStatus;
    caThreadMode mMode;
    pthread_t *mThid;
    static pthread_mutex_t mMtx;
    pthread_cond_t mCond;
    functor reqFunc;
    cleanctor cleanfunc;
    size_t mIndex;
    unsigned long int mTickCount;
    void *reqParam;
    char mName[32];


    bool CreateThread();
    int WaitForSignal(void);
    int ExecuteClient(void);
    int Lock(void);
    int Unlock(void);
    int CondWait(void);
    int CondSignal(void);

    void DestroyThread(void);
    void JoinThread(void);



public:
    caThreadClient( size_t index = 0,cleanctor cc=nullptr);
    ~caThreadClient();
    bool InitThread(functor entry, void *param, const char *name);
    void SleepThread(unsigned int delay);
    void Resume(void);
    void ReqExit(void);
    void Reset(void);
    inline void finalize_cleanup(int result)
    {
        if (cleanfunc!=nullptr)
            cleanfunc(mIndex,result);
    }
    inline caThreadStatus getStatus(void)
    {
        return mStatus;
    }

    inline void setStatus(caThreadStatus m)
    {
        mStatus = m;
    }

    inline caThreadMode getMode(void)
    {
        return mMode;
    }

    inline pthread_t * getThreadId(void)
    {
        return mThid;
    }

    inline const char *getName(void)
    {
        return mName;
    }

    inline int getTickCount(void)
    {
        return mTickCount;
    }

    inline size_t getIndex(void)
    {
        return mIndex;
    }

public:
    static void * entry_point(void *param);
    static void cleanup_point(void *param);

};

#endif /* THREADCLIENT_H */

