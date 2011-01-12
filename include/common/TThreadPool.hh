/**
 * @file   TThreadPool.hh
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Aug 18 17:50:05 2010
 *
 * @brief  ThreadPool implement
 *
 *
 */

#ifndef __TTHREADPOOL_HH
#define __TTHREADPOOL_HH

#include "sll.hh"
#include "TThread.hh"
typedef unsigned int uint;


#define NO_PROC ((uint) -1)

class TPoolThr;

class TThreadPool
{
    friend class TPoolThr;

public:

    // inline class for Thread Job
    class TJob
    {
    protected:

        const uint  _job_no;

        TMutex     _sync_mutex;

    public:
        // constructor
        TJob ( const uint n = NO_PROC ) : _job_no(n) {}

        virtual ~TJob ()
        {
            if ( _sync_mutex.is_locked() )
                std::cerr << "(TJob) destructor : job is still running!" << std::endl;
        }

        // running method
        virtual void run ( void * ptr ) = 0;

        // access processor-number
        uint job_no () const { return _job_no; }

        // and mutex
        void lock   () { _sync_mutex.lock(); }
        void unlock () { _sync_mutex.unlock(); }

        // compare if given proc-no is local one
        bool on_proc ( const uint p ) const
        { return ((p == NO_PROC) || (_job_no == NO_PROC) || (p == _job_no)); }
    };

protected:
    // maximum degree of parallelism
    uint               _max_parallel;

    // array of threads, handled by pool
    TPoolThr **        _threads;

    // list of idle threads
    sll< TPoolThr * >  _idle_threads;

    // condition for synchronisation of idle list
    TCondition         _idle_cond;

public:
    ///////////////////////////////////////////////
    //
    // constructor and destructor
    //

    TThreadPool ( const uint max_p );

    ~TThreadPool ();

    ///////////////////////////////////////////////
    //
    // access local variables
    //

    uint max_parallel () const { return _max_parallel; }

    ///////////////////////////////////////////////
    //
    // run, stop and synch with job
    //

    void  run  ( TJob * job, void * ptr = NULL, const bool del = false );
    void  sync ( TJob * job );

    void  sync_all ();

protected:

    // return idle thread form pool
    TPoolThr * get_idle ();

    // insert idle thread into pool
    void append_idle ( TPoolThr * t );
};

// init global thread_pool
void tp_init ( const uint p );

// run job
void tp_run ( TThreadPool::TJob * job, void * ptr = NULL, const bool del = false );

// synchronise with specific job
void tp_sync ( TThreadPool::TJob * job );

// synchronise with all jobs
void tp_sync_all ();

// finish thread pool
void tp_done ();

#endif  // __TTHREADPOOL_HH
