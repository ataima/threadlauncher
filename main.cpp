#include <stdio.h>
#include <pthread.h>
#include "Thread.h"
#include "ThreadClient.h"
#include "ThreadSimple.h"
#include "ThreadManager.h"
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <time.h>


static struct timespec start, finish;
#define MAX_CLIENT 64
static unsigned int counters[MAX_CLIENT];

unsigned long int getelapsedtime(void)
{
    long seconds = finish.tv_sec - start.tv_sec;
    long ns = finish.tv_nsec - start.tv_nsec;

    if (start.tv_nsec > finish.tv_nsec)   // clock underflow
    {
        --seconds;
        ns += 1000000000;
    }
    unsigned long int result = (seconds * 1000000000) + ns;
    return result;
}

void * func_client(void *param)
{
    int index = (int) (unsigned long int) (param);
    clock_gettime(CLOCK_REALTIME, &finish);
    unsigned long int total = getelapsedtime();
    if (index < MAX_CLIENT)
    {
        std::cout << total << " ) >>exec func client id=" << index << " (" << counters[index] <<
                  ")" << std::endl;
        counters[index]++;
    }
    else
        std::cout << total << " ) >>exec func client id=" << index << " HUH" << std::endl;
    auto randtime=rand()&0xffff;
    usleep(100000+randtime);
    return nullptr;
}

static int curr_task = 0;



int main(void)
{
    int i, c, average, mintick, maxtick;
    char names[64];
    caThreadManager manager;
    for (i = 0; i < MAX_CLIENT; i++)
    {
        sprintf(names, "exec client %d", i);
        counters[i] = 0;
        manager.AddClient(func_client, (void *) (unsigned long int)i,i, names);
    }

    clock_gettime(CLOCK_REALTIME, &start);

    sleep(1);
    manager.StartClients(16);
    manager.WaitTerminateClients();
    manager.Reset();
    manager.StartClients(32);
    manager.WaitTerminateClients();
    manager.Reset();
    manager.StartClients(64);
    manager.WaitTerminateClients();
    manager.Reset();
    manager.StartClients(128);
    manager.WaitTerminateClients();
    mintick = 10000000;
    maxtick = average = 0;
    for (i = 0; i < MAX_CLIENT; i++)
    {
        //std::cerr << "COUNTER[" << i << "]=" << counters[i] << std::endl;
        average += counters[i];
        if (counters[i] < mintick)
        {
            mintick = counters[i];
        }
        if (counters[i] > maxtick)
        {
            maxtick = counters[i];
        }
    }
    std::cerr << "TOTAL TICK CLIENTS: " << average << std::endl;
    average /= MAX_CLIENT;
    std::cerr << "AVERAGE TICK X CLIENTS: " << average << " ( " << 10000.0 / average << " ms )" << std::endl;
    std::cerr << "MIN TICK  CLIENT: " << mintick << std::endl;
    std::cerr << "MAX TICK  CLIENT: " << maxtick << std::endl;

    return 0;
}

