//
// Created by Filip Dej on 13.03.2018.
//
#include <stdio.h>
#include <time.h>
#include <sys/times.h>

#ifndef SYSOPY_TIMES_H
#define SYSOPY_TIMES_H

struct TimeMes{
    clock_t realTime;
    clock_t systemTime;
    clock_t userTime;
};

struct TimeMes startTime(){
    struct timespec realTime;
    struct tms otherTimes;
    times(&otherTimes);

    struct TimeMes myTime;
    clock_gettime(CLOCK_REALTIME, &realTime);
    myTime.realTime = realTime.tv_nsec + realTime.tv_sec*1000000000; //conversion to nanoseconds
    myTime.userTime = otherTimes.tms_utime;
    myTime.systemTime = otherTimes.tms_stime;
    return myTime;
}

void endTime(struct TimeMes * myTime){
    struct timespec realTime;
    struct tms otherTimes;
    times(&otherTimes);

    clock_gettime(CLOCK_REALTIME, &realTime);
    myTime->realTime = (realTime.tv_nsec + realTime.tv_sec*1000000000) - myTime->realTime;
    myTime->userTime = otherTimes.tms_utime - myTime->userTime;
    myTime->systemTime = otherTimes.tms_stime - myTime->systemTime;

    printf("real time: %f\n", (float) (myTime->realTime)/1000000); //  nanoseconds
    printf("user time: %f\n", (float)((myTime->userTime) *1000000.0/ CLOCKS_PER_SEC));//  nanoseconds
    printf("system time: %f\n", (float)((myTime->systemTime) *1000000.0/ CLOCKS_PER_SEC)); //  nanoseconds

}

#endif //SYSOPY_TIMES_H
