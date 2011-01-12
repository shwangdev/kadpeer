/**
 * @file   TTimer.hh
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Nov 17 00:28:57 2010
 *
 * @brief
 *
 *
 */
#ifndef __TIMER_HH
#define __TIMER_HH

#include <sys/time.h>
#include <sys/resource.h>

#include <unistd.h>

#include <iostream>

// type for different timer-measurment
typedef enum { REAL_TIME, CPU_TIME, USER_TIME, SYSTEM_TIME }  timetype_t;

//
// the timer class
//
class TTimer
{
public:

protected:
    // start/stop times
    double          _start, _stop;

    // temp structs
    struct timeval  _timeval_data;

    struct rusage   _rusage_data;
    // what kind of time we should stop
    timetype_t      _type;


public:


    TTimer ( timetype_t type = CPU_TIME ) : _start(0), _stop(0), _type(type) {}


    TTimer & start    () { _stop = _start = system_time(); return *this; }
    TTimer & stop     () { _stop =          system_time(); return *this; }


    float diff () const { return _stop - _start; }

    double system_time ();

    // set type of time to measure and return *this (for output)
    TTimer & real () { _type = REAL_TIME; return *this; }
    TTimer & cpu  () { _type = CPU_TIME;  return *this; }
    TTimer & user () { _type = USER_TIME; return *this; }
    TTimer & sys  () { _type = SYSTEM_TIME;  return *this; }

    friend std::ostream & operator << ( std::ostream & os, const TTimer & timer );
};

#endif
