/*
 * File:   ThreanManager.h
 * Author: angelo
 *
 * Created on 28 marzo 2019, 16.28
 */

#ifndef THREANMANAGER_H
#define THREANMANAGER_H


#include <vector>
#include "Thread.h"
#include "ThreadClient.h"


class caThreadManager;

typedef std::vector<caThreadClient *> thArray;


typedef struct tag_status_thread
{
    size_t clients;
    size_t running;
    size_t stopped;
    int errors;
} statusThreads;

class caThreadManager
{
private:
    thArray clients;
    std::vector<int> running;
    std::vector<int> stopped;
    int max_running;
    int errors;
    std::mutex mMtxRun;
    std::mutex mMtxStop;
    std::mutex mMtxEnd;
    std::mutex mMtxGo;
    std::condition_variable mCondEnd;
    static caThreadManager *instance;
private:
    bool Run(int index);
    void pushRunning(int index);
    void pushStopped(int index);
    int  GetRunningSize();
    int  GetStoppedSize();
    void ClientsTerminateSignal();
    void finalize(int index,int result);
public:
    caThreadManager();
    ~caThreadManager();
    bool AddClient(functor func,void *param, int index, const char *name);
    void GetStatus(statusThreads &st);
    void StartClients(int max_run);
    void Reset();
    void WaitTerminateClients();
    inline bool haveErrors()
    {
        return (errors!=0);
    }

    inline static caThreadManager * getInstance()
    {
        return instance;
    }


public:
    static void finalize_client(int index,int result);
};



#endif /* THREANMANAGER_H */

