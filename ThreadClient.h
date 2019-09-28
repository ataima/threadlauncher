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
    void WaitForSignal();
    int ExecuteClient();
    void CondWait();
    void CondSignal();





public:
    explicit caThreadClient( int index = 0,cleanctor cc=nullptr);
    ~caThreadClient();
    bool InitThread(functor entry, void *param, const char *name);
    void SleepThread(unsigned int delay);
    void Resume();
    void ReqExit();
    void Reset();
    inline void finalize_cleanup(int result)
    {
        if (cleanfunc!=nullptr)
            cleanfunc(mIndex,result);
    }
    inline caThreadStatus getStatus()
    {
        return mStatus;
    }

    inline void setStatus(caThreadStatus m)
    {
        mStatus = m;
    }


    inline std::thread * getThreadId()
    {
        return mThid;
    }

    inline const char *getName()
    {
        return mName;
    }

    inline unsigned long int getTickCount()
    {
        return mTickCount;
    }

    inline size_t getIndex()
    {
        return mIndex;
    }

public:
    static void * entry_point(void *param);

};

#endif /* THREADCLIENT_H */

