/**
 * @file   TThread.hh
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Aug 18 17:48:48 2010
 *
 * @brief  virtual Thread wapper for theadpool
 *
 *
 */

#ifndef __TTHREAD_HH
#define __TTHREAD_HH

#include <stdio.h>
#include <pthread.h>


class TThread
{
protected:
    // thread-specific things
    pthread_t       _thread_id;
    pthread_attr_t  _thread_attr;

    // is the thread running or not
    bool            _running;
    int             _thread_no;

public:
    TThread ( const int thread_no = -1 );
    virtual ~TThread ();
    int thread_no () const { return _thread_no; }
    int proc_no   () const { return _thread_no; }

    void set_thread_no ( const int n );


    bool on_proc ( const int p ) const { return ((p == -1) || (_thread_no == -1) || (p == _thread_no)); }

    void reset_running () { _running = false; }


    virtual void run () = 0;

    void create ( const bool detached = false, const bool sscope = false );

    // detach thread
    void detach ();

    // synchronise with thread (wait until finished)
    void join   ();

    // request cancellation of thread
    void cancel ();

protected:

    void exit   ();

    // put thread to sleep for <sec> seconds
    void sleep  ( const double sec );
};


class TMutex
{
protected:
    // the mutex itself and the mutex-attr
    pthread_mutex_t      _mutex;
    pthread_mutexattr_t  _mutex_attr;

public:

    TMutex ()
    {
        pthread_mutexattr_init( & _mutex_attr );
        pthread_mutex_init( & _mutex, & _mutex_attr );
    }

    ~TMutex ()
    {
        pthread_mutex_destroy( & _mutex );
        pthread_mutexattr_destroy( & _mutex_attr );
    }


    // lock and unlock mutex (return 0 on success)
    int lock    () { return pthread_mutex_lock(   & _mutex ); }
    int unlock  () { return pthread_mutex_unlock( & _mutex ); }

    // return true if mutex is locked, otherwise false
    bool is_locked ()
    {
        if ( pthread_mutex_trylock( & _mutex ) != 0 )
            return true;
        else
        {
            unlock();
            return false;
        }// else
    }
};


class TCondition : public TMutex
{
protected:

    pthread_cond_t  _cond;

public:

    TCondition  () { pthread_cond_init(    & _cond, NULL ); }
    ~TCondition () { pthread_cond_destroy( & _cond ); }

    void wait      () { pthread_cond_wait( & _cond, & _mutex ); }
    void signal    () { pthread_cond_signal( & _cond ); }
    void broadcast () { pthread_cond_broadcast( & _cond ); }
};

#endif  // __TTHREAD_HH
