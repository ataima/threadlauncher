/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <pthread.h>
#include "ThreadClient.h"
#include "ThreadManager.h"
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <set>

pthread_mutex_t caThreadManager::mMtxClients = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t caThreadManager::mMtxRun = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t caThreadManager::mMtxStop = PTHREAD_MUTEX_INITIALIZER;



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
    errors.clear();
    pthread_mutex_init(&mMtxClients,nullptr);
    pthread_mutex_init(&mMtxRun,nullptr);
    pthread_mutex_init(&mMtxStop,nullptr);
    instance = this;
}

caThreadManager::~caThreadManager()
{
    HERE1();
    StopClients();
    lockClients();
    for(auto th: clients)
    {
        delete th;
    }
    clients.clear();
    running.clear();
    stopped.clear();
    errors.clear();
    unlockClients();
    pthread_mutex_destroy(&mMtxClients);
    pthread_mutex_destroy(&mMtxRun);
    pthread_mutex_destroy(&mMtxStop);
}

bool caThreadManager::AddClient(functor func, void *param,  size_t index, const char *name)
{
    HERE1();
    bool result = true;
    caThreadClient *client = new caThreadClient(index,caThreadManager::finalize_client);
    if (client != nullptr)
    {
        lockClients();
        clients.push_back(client);
        unlockClients();
        client->InitThread(func, param, name);
        while (client->getStatus() != caThreadStatus::WAIT_SIGNAL)
        {
            usleep(100);
        }
        result = false;
    }
    return result;
}

bool caThreadManager::Run(size_t index)
{
    HERE1();
    bool result = false;
    caThreadClient * clientThread =nullptr;
    //auto num_clients=GetClientsSize();
    //auto num_running=GetRunningSize();
    //if(num_running<num_clients)
    //{
    lockClients();
    clientThread = clients.at(index);
    unlockClients();
    lockRunning();
    running.push_back(clientThread);
    unlockRunning();
    //}
    if(clientThread!=nullptr &&  clientThread->getStatus() == caThreadStatus::WAIT_SIGNAL )
    {
        clientThread->Resume();
        result=true;
    }
    return result;
}

void  caThreadManager::finalize_client(size_t index,int error)
{
    caThreadManager *manager=caThreadManager::getInstance();
    if(manager!=nullptr)
    {
        manager->finalize(index,error);
    }
}

void  caThreadManager::finalize( size_t index, int result)
{
    caThreadClient * clientThread =nullptr;
    // save error
    // save to stopped thread
    lockRunning();
    errors.push_back(result);
    clientThread =running.at(index);
    unlockRunning();
    lockStopped();
    stopped.push_back(clientThread);
    unlockStopped();
    //std::cerr<<"FINALIZE CLIENT = "<<index<<" "<<stopped.size()<<":"<<running.size()<<std::endl;
    // check more thread
    auto crun=GetRunningSize();
    auto cclient=GetClientsSize();
    if(crun<cclient)
        Run(crun);
}


void caThreadManager::StopClients(void)
{
    lockRunning();
    thArray::iterator it = running.begin();
    thArray::iterator _end = running.end();
    while (it != _end)
    {
        if (*it != nullptr)
        {
            if((*it)->getStatus()!=EXITED)
                (*it)->ReqExit();
            *it = nullptr;
        }
        it++;
    }
    unlockRunning();
}


bool caThreadManager::Reset(void)
{
    auto res=false;
    statusThreads st;
    GetStatus(st);
    if(st.running==st.clients && st.stopped==st.clients)
    {
        lockClients();
        lockRunning();
        lockStopped();
        running.clear();
        errors.clear();
        stopped.clear();
        for(auto th: clients)
        {
            th->Reset();
        }
        unlockStopped();
        unlockRunning();
        unlockClients();
        res=true;
    }
    return res;
}

size_t caThreadManager::GetClientsSize(void)
{
    HERE1();
    int size = 0;
    lockClients();
    size = (int) clients.size();
    unlockClients();
    return size;
}

size_t caThreadManager::GetRunningSize(void)
{
    HERE1();
    int size = 0;
    lockRunning();
    size = (int) running.size();
    unlockRunning();
    return size;
}


size_t caThreadManager::GetStoppedSize(void)
{
    HERE1();
    int size = 0;
    lockStopped();
    size = (int) stopped.size();
    unlockStopped();
    return size;
}


void caThreadManager::GetStatus(statusThreads &st)
{
    st.clients=GetClientsSize();
    st.running=GetRunningSize();
    st.stopped=GetStoppedSize();
}


void caThreadManager::StartClients(size_t max_run)
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



void  caThreadManager::lockRunning(void)
{
    auto res=-1;
    do
    {
        res=pthread_mutex_trylock(&mMtxRun);
        if(res)
        {
            std::cerr<<"LOCK RUNNING ERROR"<<std::endl;
            break;
        }
    }
    while(res!=0);
}

void  caThreadManager::unlockRunning(void)
{
    auto res=-1;
    do
    {
        res=pthread_mutex_unlock(&mMtxRun);
        if(res)
        {
            std::cerr<<"UNLOCK RUNNING ERROR"<<std::endl;
            break;
        }
    }
    while(res!=0);
}

void  caThreadManager::lockStopped(void)
{
    auto res=-1;
    do
    {
        res=pthread_mutex_trylock(&mMtxStop);
        if(res)
        {
            std::cerr<<"LOCK STOPPED ERROR"<<std::endl;
            break;
        }
    }
    while(res!=0);
}

void  caThreadManager::unlockStopped(void)
{
    auto res=-1;
    do
    {
        res=pthread_mutex_unlock(&mMtxStop);
        if(res)
        {
            std::cerr<<"UNLOCK STOPPED ERROR"<<std::endl;
            break;
        }
    }
    while(res!=0);
}

void  caThreadManager::lockClients(void)
{
    auto res=-1;
    do
    {
        res=pthread_mutex_trylock(&mMtxClients);
        if(res)
        {
            std::cerr<<"LOCK CLIENTS ERROR"<<std::endl;
            break;
        }
    }
    while(res!=0);
}

void  caThreadManager::unlockClients(void)
{
    auto res=-1;

    do
    {
        res=pthread_mutex_unlock(&mMtxClients);
        if(res)
        {
            std::cerr<<"UNLOCK CLIENTS ERROR"<<std::endl;
            break;
        }
    }
    while(res!=0);
}



