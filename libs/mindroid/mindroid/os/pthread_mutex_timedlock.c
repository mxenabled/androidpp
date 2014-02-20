//
//  pthread_mutex_timedlock.c
//  cocos2dx
//
//  Created by Saul Howard on 11/5/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>

int pthread_mutex_timedlock (pthread_mutex_t *mutex, struct timespec *timeout)
{
    struct timeval timenow;
    struct timespec sleepytime;
    int retcode;
    
    /* This is just to avoid a completely busy wait */
    sleepytime.tv_sec = 0;
    sleepytime.tv_nsec = 10000000; /* 10ms */
    
    while ((retcode = pthread_mutex_trylock (mutex)) == EBUSY) {
        gettimeofday (&timenow, NULL);
        
        if (timenow.tv_sec >= timeout->tv_sec &&
            (timenow.tv_usec * 1000) >= timeout->tv_nsec) {
            return ETIMEDOUT;
        }
        
        nanosleep (&sleepytime, NULL);
    }
    
    return retcode;
}