/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <thread>
#include "ThreadClient.h"
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <cstring>



caThreadClient::caThreadClient( int index,cleanctor cc)
{
    HERE1();
    mStatus = STOPPED;
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
    InitThread(reqFunc, reqParam, mName) ;
}

caThreadClient::~caThreadClient()
{
    if(getStatus()!=EXITED)
        ReqExit();
    if(mThid!=nullptr)
        delete mThid;

}

bool caThreadClient::InitThread(functor func, void *param, const char *name)
{
    HERE1();
    bool result ;
    reqFunc = func;
    reqParam = param;
    result = CreateThread();
    memcpy(mName, name, 31);
    return result;
}

bool caThreadClient::CreateThread()
{
    HERE1();
    if(mThid==nullptr)
        delete mThid;
    mThid=new std::thread(entry_point,this);
    return mThid==nullptr;
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
        client->WaitForSignal();
        res = client->ExecuteClient();
        client->setStatus(caThreadStatus::EXITED);
        client->finalize_cleanup(res);
#if MOREDEBUG
        std::cerr << "thread " << client->getName() << " EXITED!" << std::endl;
#endif
    }
    if(res)resptr++;
    return (void *)resptr;
}



void caThreadClient::CondWait(void)
{
    HERE1();
    std::unique_lock<std::mutex> lck(mMtx);
    mCond.wait(lck);
}


void  caThreadClient::CondSignal(void)
{
    HERE1();
    mCond.notify_one();
}



void caThreadClient::WaitForSignal(void)
{
    HERE1();
    mStatus = WAIT_SIGNAL;
    CondWait();
    mTickCount++;
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


void caThreadClient::Resume(void)
{
    HERE1();
    CondSignal();
    mStatus = RESUME;
}



void caThreadClient::ReqExit(void)
{
    HERE1();
    CondSignal();
    mStatus = TRY_EXIT;
}