/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <pthread.h>
#include "ThreadClient.h"
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <cstring>




pthread_mutex_t caThreadClient::mMtx = PTHREAD_MUTEX_INITIALIZER;


caThreadClient::caThreadClient( size_t index,cleanctor cc)
{
    HERE1();
    mStatus = STOPPED;
    mMode = WAIT_ALWAYS;
    mThid = nullptr;
    reqFunc = nullptr;
    reqParam = nullptr;
    mIndex = index;
    mTickCount = 0;
    mName[0] = '\0';
    cleanfunc=cc;
}

void caThreadClient::Reset(void)
{
    mStatus = STOPPED;
    mMode = WAIT_ALWAYS;
    DestroyThread();
    InitThread(reqFunc, reqParam, mName) ;
}

caThreadClient::~caThreadClient()
{
    HERE1();
    pthread_mutex_destroy(&caThreadClient::mMtx);
}

bool caThreadClient::InitThread(functor func, void *param, const char *name)
{
    HERE1();
    bool result = false;
    if (pthread_cond_init(&mCond, NULL) != 0)
    {
        std::cerr << "Cannot create cond" << std::endl;
    }
    else
    {
        reqFunc = func;
        reqParam = param;
        result = CreateThread();
        memcpy(mName, name, 31);
    }
    return result;
}

bool caThreadClient::CreateThread()
{
    int ret = -1;
    HERE1();
    mThid = new pthread_t;
    if (mThid != nullptr)
    {
        pthread_attr_t tattr;
        ret = pthread_attr_init(&tattr);
        if (ret == 0 || ret == EBUSY)
        {
            ret = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_JOINABLE);
            if (ret == 0)
            {
                ret = pthread_create(mThid, &tattr,entry_point, this);
            }
        }
    }
    if (ret == 0)
        return false;
    else
        return true;
}

void caThreadClient::DestroyThread(void)
{
    HERE1();
    if (mThid != nullptr)
    {
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
        pthread_cancel(*mThid);
        delete mThid;
        mThid = nullptr;
    }
}

void caThreadClient::JoinThread(void)
{
    HERE1();
    if (mThid != nullptr)
    {
        pthread_join(*mThid, nullptr);
    }
}

void caThreadClient::SleepThread(unsigned int delay)
{
    HERE1();
    if (delay < 1000)
        usleep(delay * 1000);
    else
    {
        usleep((delay % 1000)*1000);
        sleep(delay / 1000);
    }
}

void * caThreadClient::entry_point(void *param)
{
    HERE();
    int * resptr=nullptr;
    size_t res = 0;
    caThreadClient* client = static_cast<caThreadClient*> (param);
    if (client != nullptr)
    {
        pthread_cleanup_push(cleanup_point, client);
        if (client->WaitForSignal() == 0)
        {
            if (client->getMode() == caThreadMode::REQ_EXIT)
            {
                res =-1;
                break;
            }
            else
            {
                res = client->ExecuteClient();
            }
        }
        pthread_cleanup_pop(0);
        client->setStatus(caThreadStatus::EXITED);
        client->finalize_cleanup(res);
#if MOREDEBUG
        std::cerr << "thread " << client->getName() << " EXITED!" << std::endl;
#endif
    }
    if(res)resptr++;
    return (void *)resptr;
}

int caThreadClient::Lock(void)
{
    HERE1();
    int ret = pthread_mutex_lock(&caThreadClient::mMtx);
    if (ret != 0)
    {
        std::cerr << "Cannot lock mutex" << std::endl;
    }
#if MOREDEBUG
    else
    {
        std::cerr << "lock mutex index=" << reqParam << std::endl;
    }
#endif
    return ret;
}

int caThreadClient::Unlock(void)
{
    HERE1();
    int ret = pthread_mutex_unlock(&caThreadClient::mMtx);
    if (ret != 0)
    {
        std::cerr << "Cannot unlock mutex" << std::endl;
    }
#if MOREDEBUG
    else
    {
        std::cerr << "unlock mutex index=" << reqParam << std::endl;
    }
#endif
    return ret;
}

int caThreadClient::CondWait(void)
{
    HERE1();
    int ret = pthread_cond_wait(&mCond, &caThreadClient::mMtx);
    if (ret != 0)
    {
        std::cerr << "Cannot waiting condition" << std::endl;
    }
#if MOREDEBUG
    else
    {
        std::cerr << "cond wait pass index=" << reqParam << std::endl;
    }
#endif
    return ret;
}

int caThreadClient::CondSignal(void)
{
    HERE1();
    int ret = pthread_cond_signal(&mCond);
    if (ret != 0)
    {
        std::cerr << "Cannot signalling condition" << std::endl;
    }
#if MOREDEBUG
    else
    {
        std::cerr << "cond signalling pass index=" << reqParam << std::endl;
    }
#endif
    return ret;
}

int caThreadClient::WaitForSignal(void)
{
    HERE1();
    int res = 0;
    if (mMode != caThreadMode::NO_WAIT)
    {
        res = Lock();
        if (res == 0)
        {
            mStatus = WAIT_SIGNAL;
            CondWait();
            if (mMode == caThreadMode::WAIT_ONLY_START)
            {
                mMode = NO_WAIT;
            }
            mTickCount++;
            res = Unlock();
        }
    }
    return res;
}

int caThreadClient::ExecuteClient(void)
{
    HERE1();
    int res=-1;
    if (reqFunc != nullptr)
    {
        mStatus = RUNNING;
        if(reqFunc(reqParam)==nullptr)
            res=0;
    }
    return res;
}

void caThreadClient::cleanup_point(void *param)
{
    HERE();
    caThreadClient* instance = static_cast<caThreadClient*> (param);
    if (instance != nullptr)
    {
        instance->Unlock();
        instance->JoinThread();
    }
}

void caThreadClient::Resume(void)
{
    HERE1();
    if (Lock() == 0)
    {
        if (CondSignal() == 0)
            mStatus = RESUME;
        Unlock();
    }
}

void caThreadClient::ReqExit(void)
{
    HERE1();
    if (Lock() == 0)
    {
        mMode = caThreadMode::REQ_EXIT;
        if (CondSignal() == 0)
            mStatus = TRY_EXIT;
        Unlock();
        std::cerr<<"Force REQ EXIT ID="<<mIndex<<std::endl;
        usleep(2000);
        DestroyThread();
    }
}