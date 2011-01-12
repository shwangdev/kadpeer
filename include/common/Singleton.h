/**
 * @file   Singleton.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Aug 18 17:46:17 2010
 *
 * @brief  Singleton desin pattern
 *
 *
 */

#ifndef __SINGLETON_H_
#define __SINGLETON_H_

#include <boost/thread/mutex.hpp>
#include <memory>

typedef boost::mutex::scoped_lock scoped_lock;

template <class T>
class Singleton {
public:
    static inline T* instance();
    Singleton(void) {}
    ~Singleton(void) {}
    Singleton(const Singleton&) {}
    Singleton & operator=(const Singleton &) {}
    static std::auto_ptr<T> instance_;
    static boost::mutex mutex_;
};

template <class T>
std::auto_ptr<T> Singleton<T>::instance_;

template <class T>
boost::mutex Singleton<T>::mutex_;

template <class T> inline T* Singleton<T>::instance() {
    scoped_lock guard(mutex_);
    if (0 == instance_.get()) {
        if (0 == instance_.get()) {
            instance_.reset(new T);
        }
    }
    return instance_.get();
}
#endif
