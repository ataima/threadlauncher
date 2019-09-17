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
typedef int (*functor_nexttask)(caThreadManager *instance);
typedef struct tag_status_thread
{
    size_t clients;
    size_t running;
    size_t stopped;
} statusThreads;

class caThreadManager
{
private:
    thArray clients;
    thArray running;
    thArray stopped;
    size_t max_running;
    std::vector<size_t> errors;
    static pthread_mutex_t mMtxClients;
    static pthread_mutex_t mMtxRun;
    static pthread_mutex_t mMtxStop;
    static caThreadManager *instance;
private:
    bool Run(size_t index);
    size_t GetClientsSize(void);
    size_t GetRunningSize(void);
    size_t GetStoppedSize(void);
public:
    caThreadManager();
    ~caThreadManager();
    bool AddClient(functor func,void *param, size_t index, const char *name);

    void GetStatus(statusThreads &st);
    void StartClients(size_t max_run);
    void StopClients(void);
    bool Reset(void);

    inline bool haveErrors(void)
    {
        return !errors.empty();
    }

    inline static caThreadManager * getInstance(void)
    {
        return instance;
    }

    void lockRunning(void);
    void unlockRunning(void);
    void lockStopped(void);
    void unlockStopped(void);
    void lockClients(void);
    void unlockClients(void);

    void finalize(size_t index,int result);
public:
    static void finalize_client(size_t index,int result);
};



#endif /* THREANMANAGER_H */

