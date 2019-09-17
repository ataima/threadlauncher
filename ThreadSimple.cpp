/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <pthread.h>
#include "ThreadSimple.h"
#include <errno.h>
#include <unistd.h>

caThreadSimple::caThreadSimple()
{
    mStatus = STOPPED;
    mThid = nullptr;
    reqFunc = nullptr;
    reqParam = nullptr;
}

caThreadSimple::~caThreadSimple()
{

}

bool caThreadSimple::InitThread(functor entry, void *param)
{
    int ret = -1;
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
                ret = pthread_create(mThid, &tattr,entry, this);
            }
        }
    }
    if (ret == 0)
        return false;
    else
        return true;
}

void caThreadSimple::DestroyThread(void)
{
    if (mThid != nullptr)
    {
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
        pthread_cancel(*mThid);
        delete mThid;
        mThid = nullptr;
    }
}

void caThreadSimple::JoinThread(void)
{
    if (mThid != nullptr)
    {
        pthread_join(*mThid, nullptr);
    }
}

void caThreadSimple::SleepThread(unsigned int delay)
{
    if (delay < 1000)
        usleep(delay * 1000);
    else
    {
        usleep((delay % 1000)*1000);
        sleep(delay / 1000);
    }
}

void *caThreadSimple::entry_point(void *param)
{
    int res=0;
    int * resptr = nullptr;
    caThreadSimple* instance = static_cast<caThreadSimple*> (param);
    if (instance != nullptr)
    {
        pthread_cleanup_push(cleanup_point, instance);
        res = instance->ExecuteClient();
        pthread_cleanup_pop(0);
    }
    if(res)resptr++;
    return (void *)resptr;
}

int caThreadSimple::ExecuteClient(void)
{
    int res=-1;
    if (reqFunc != nullptr)
    {
        mStatus = RUNNING;
        if(reqFunc(reqParam)==nullptr)
            res=0;
    }
    return res;
}

void caThreadSimple::CleanUp(void)
{
    pthread_exit(0);
}

void caThreadSimple::cleanup_point(void *param)
{
    caThreadSimple* instance = static_cast<caThreadSimple*> (param);
    if (instance != nullptr)
    {
        instance->CleanUp();
    }
}

