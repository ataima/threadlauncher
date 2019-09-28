/*
 * File:   ThreadClient.h
 * Author: angelo
 *
 * Created on 28 marzo 2019, 14.11
 */

#ifndef THREADCLIENT_H
#define THREADCLIENT_H


#include "Thread.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class caThreadClient
{
protected:
    caThreadStatus mStatus;
    std::thread *mThid;
    std::mutex mMtx;
    std::condition_variable mCond;
    functor reqFunc;
    cleanctor cleanfunc;
    int mIndex;
    unsigned long int mTickCount;
    void *reqParam;
    char mName[32];


    bool CreateThread();
    void WaitForSignal(void);
    int ExecuteClient(void);
    void CondWait(void);
    void CondSignal(void);





public:
    caThreadClient( int index = 0,cleanctor cc=nullptr);
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


    inline std::thread * getThreadId(void)
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

};

#endif /* THREADCLIENT_H */

