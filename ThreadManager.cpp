/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include "ThreadClient.h"
#include "ThreadManager.h"
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>



caThreadManager *caThreadManager::instance = nullptr;

caThreadManager::caThreadManager()
{
    HERE1();
    max_running=1;
    clients.clear();
    running.clear();
    stopped.clear();
    if (instance != nullptr)
    {
        std::cerr << "caThreadManager::instance already set !!" << std::endl;
    }
    instance = this;
}

caThreadManager::~caThreadManager()
{
    HERE1();
    Reset();
}



bool caThreadManager::AddClient(functor func, void *param,  int index, const char *name)
{
    HERE1();
    bool result = false;
    caThreadClient *client = new caThreadClient(index,caThreadManager::finalize_client);
    if (client != nullptr)
    {
        client->InitThread(func, param, name);
        clients.push_back(client);
        result = true;
    }
    return result;
}


void caThreadManager::pushRunning(int index)
{
    HERE1();
    std::unique_lock<std::mutex> lck(mMtxRun);
    running.push_back(index);
}


void caThreadManager::pushStopped(int index)
{
    HERE1();
    std::unique_lock<std::mutex> lck(mMtxStop);
    stopped.push_back(index);
}



int caThreadManager::GetRunningSize(void)
{
    HERE1();
    int size = 0;
    std::unique_lock<std::mutex> lck(mMtxRun);
    size = (int) running.size();
    return (int)size;
}


int caThreadManager::GetStoppedSize(void)
{
    HERE1();
    int size = 0;
    std::unique_lock<std::mutex> lck(mMtxStop);
    size = (int) stopped.size();
    return size;
}


void caThreadManager::GetStatus(statusThreads &st)
{
    st.clients=clients.size();
    st.running=GetRunningSize();
    st.stopped=GetStoppedSize();
    st.errors=errors;
}


bool caThreadManager::Run(int index)
{
    HERE1();
    bool result = false;
    if(index<clients.size())
    {
        caThreadClient * clientThread =clients.at(index);;
        if(clientThread!=nullptr &&  clientThread->getStatus() == caThreadStatus::WAIT_SIGNAL )
        {
            clientThread->Resume();
            pushRunning(index);
            result=true;
        }
    }
    return result;
}

void  caThreadManager::finalize_client(int index,int error)
{
    caThreadManager *manager=caThreadManager::getInstance();
    if(manager!=nullptr)
    {
        manager->finalize(index,error);
    }
}

void  caThreadManager::finalize( int index, int result)
{
    caThreadClient *  clientThread=clients.at(index);
    pushStopped(index);
    auto crun=GetRunningSize();
    auto cclient=clients.size();
    auto cstop=GetStoppedSize();
    {
        std::unique_lock<std::mutex> lck(mMtxGo);
        if(crun<cclient)
            Run(crun);
    }
    if(cstop==cclient)
    {
        ClientsTerminateSignal();
    }
}




void  caThreadManager::Reset(void)
{
    auto res=false;
    for(auto th: clients)
    {
        th->Reset();
    }
    mMtxRun.lock();
    running.clear();
    mMtxRun.unlock();
    mMtxStop.lock();
    stopped.clear();
    mMtxStop.unlock();
}







void caThreadManager::StartClients(int max_run)
{
    HERE1();
    max_running=max_run;
    do
    {
        statusThreads st;
        GetStatus(st);
        auto i=st.running-st.stopped;
        auto end=st.clients-st.running;
        if(i<max_running && end>0)
        {
            Run(i);
        }
        else
        {
            break;
        }

    }
    while(1);
}





void caThreadManager::ClientsTerminateSignal(void)
{
    mCondEnd.notify_one();
}


void caThreadManager::WaitTerminateClients(void)
{
    std::unique_lock<std::mutex> lck(mMtxEnd);
    mCondEnd.wait(lck);
}

