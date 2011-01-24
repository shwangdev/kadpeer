/**
 * @file   TThreadPool.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Mon Nov 15 22:32:26 2010
 *
 * @brief  Threadpool construction
 *
 *
 */
#include <pthread.h>
#include "TThreadPool.hh"

#define THR_SEQUENTIAL  0

TThreadPool * thread_pool = NULL;

class TPoolThr : public TThread
{
protected:

    TThreadPool       * _pool;
    TThreadPool::TJob * _job;
    void              * _data_ptr;
    bool                _del_job;
    TCondition          _work_cond;
    bool                _end;
    TMutex              _del_mutex;

public:

    TPoolThr ( const int n, TThreadPool * p )
        : TThread(n), _pool(p), _job(NULL), _data_ptr(NULL), _del_job(false), _end(false)
	{}

    ~TPoolThr () {}

    void run ()
	{
	    _del_mutex.lock();

	    while ( ! _end )
	    {

            _pool->append_idle( this );

            _work_cond.lock();

            while (( _job == NULL ) && ! _end )
                _work_cond.wait();

            _work_cond.unlock();

            if ( _job != NULL )
            {
                _job->run( _data_ptr );
                _job->unlock();

                if ( _del_job )
                    delete _job;

                _work_cond.lock();
                _job      = NULL;
                _data_ptr = NULL;
                _work_cond.unlock();
            }
	    }

	    _del_mutex.unlock();
	}


    void run_job  ( TThreadPool::TJob * j, void * p, const bool del = false )
	{
	    _work_cond.lock();

	    _job      = j;
	    _data_ptr = p;
	    _del_job  = del;

	    _work_cond.signal();
	    _work_cond.unlock();
	}


    TMutex & del_mutex  ()
	{
	    return _del_mutex;
	}


    void quit ()
	{
	    _work_cond.lock();

	    _end      = true;
	    _job      = NULL;
	    _data_ptr = NULL;

	    _work_cond.signal();
	    _work_cond.unlock();
	}
};



TThreadPool::TThreadPool ( const uint max_p )
{

    _max_parallel = max_p;

    _threads = new TPoolThr*[ _max_parallel ];

    if ( _threads == NULL )
    {
        _max_parallel = 0;
        std::cerr << "(TThreadPool) TThreadPool : could not allocate thread array" << std::endl;
    }

    for ( uint i = 0; i < _max_parallel; i++ )
    {
        _threads[i] = new TPoolThr( i, this );

        if ( _threads == NULL )
            std::cerr << "(TThreadPool) TThreadPool : could not allocate thread" << std::endl;
        else
            _threads[i]->create( true, true );
    }

    // tell the scheduling system, how many threads to expect
    // (commented out since not needed on most systems)
//     if ( pthread_setconcurrency( _max_parallel + pthread_getconcurrency() ) != 0 )
//         std::cerr << "(TThreadPool) TThreadPool : pthread_setconcurrency ("
//                   << strerror( status ) << ")" << std::endl;
}

TThreadPool::~TThreadPool ()
{
    // wait till all threads have finished
    sync_all();

    // finish all thread
    for ( uint i = 0; i < _max_parallel; i++ )
        _threads[i]->quit();

    // cancel and delete all threads (not really safe !)
    for ( uint i = 0; i < _max_parallel; i++ )
    {
        _threads[i]->del_mutex().lock();
        delete _threads[i];
    }// for

    delete[] _threads;
}


void
TThreadPool::run ( TThreadPool::TJob * job, void * ptr, const bool del )
{
    if ( job == NULL )
        return;

#if THR_SEQUENTIAL == 1

    job->run( ptr );

    if ( del )
        delete job;

#else

    TPoolThr * thr = get_idle();

    job->lock();

    thr->run_job( job, ptr, del );
#endif
}


void
TThreadPool::sync ( TJob * job )
{
    if ( job == NULL )
        return;

    job->lock();
    job->unlock();
}


void
TThreadPool::sync_all ()
{
    while ( true )
    {
        _idle_cond.lock();

        if ( _idle_threads.size() < _max_parallel )
            _idle_cond.wait();
        else
        {
            _idle_cond.unlock();
            break;
        }

        _idle_cond.unlock();
    }
}

//
TPoolThr *
TThreadPool::get_idle ()
{
    while ( true )
    {

        _idle_cond.lock();

        while ( _idle_threads.size() == 0 )
            _idle_cond.wait();

        if ( _idle_threads.size() > 0 )
        {
            TPoolThr * t = _idle_threads.behead();

            _idle_cond.unlock();

            return t;
        }

        _idle_cond.unlock();
    }
}


void
TThreadPool::append_idle ( TPoolThr * t )
{

    _idle_cond.lock();

    for ( sll< TPoolThr * >::iterator iter = _idle_threads.first(); ! iter.eol(); ++iter )
    {
        if ( iter() == t )
        {
            _idle_cond.unlock();
            return;
        }
    }

    _idle_threads.push_back( t );

    _idle_cond.signal();

    _idle_cond.unlock();
}


void
tp_init ( const uint p )
{
    if ( thread_pool != NULL )
        delete thread_pool;

    if ((thread_pool = new TThreadPool( p )) == NULL)
        std::cerr << "(init_thread_pool) could not allocate thread pool" << std::endl;
}


void
tp_run ( TThreadPool::TJob * job, void * ptr, const bool del )
{
    if ( job == NULL )
        return;

    thread_pool->run( job, ptr, del );
}


void
tp_sync ( TThreadPool::TJob * job )
{
    thread_pool->sync( job );
}


void
tp_sync_all ()
{
    thread_pool->sync_all();
}


void
tp_done ()
{
    delete thread_pool;
}
