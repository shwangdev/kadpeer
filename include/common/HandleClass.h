/**
 * @file   HandleClass.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Tue Aug 17 20:29:26 2010
 *
 * @brief  Virtual class for counting pointer
 *
 *
 */
#ifndef _HANDLE_H
#define _HANDLE_H
#include<iostream>


class Object
{
public:
    void addReference();
    void removeReference();
protected:
    Object();
    Object(const Object &ob);
    Object &operator=(const Object &ob);
    virtual ~Object()=0;
private:
    unsigned int refCount;
};

inline Object::Object()
    :refCount(0){}
inline Object::Object(const Object &)
    :refCount(0){}

inline Object& Object::operator=(const Object & ob)
{
    return *this;
}

inline Object::~Object(){}

inline void Object::addReference()
{
    ++refCount;
}
inline void Object::removeReference()
{
    if( 0 == --refCount ) delete this;
}


template<class T>
class HandlePtr
{
public:
    explicit HandlePtr(T* realPtr = 0);
    HandlePtr(const HandlePtr& rhs);
    ~HandlePtr();
    HandlePtr& operator=(const HandlePtr& rhs);

    T* operator->() const;
    T& operator*() const;

    void clear() {
        *this = HandlePtr<T>(0);
    };

private:
    T *pointee;
    void init() {
        if(pointee != 0) pointee->addReference();
    }
};

template<typename T>
inline bool operator == (const HandlePtr<T> & pointer, const HandlePtr<T> & other)
{
    return pointer.operator->() == other.operator->();
}

template<typename T>
inline bool operator != (const HandlePtr<T> & pointer, const HandlePtr<T> & other)
{
    return pointer.operator->() != other.operator->();
}

template<typename T>
inline bool operator == (const HandlePtr<T> & pointer, const T * other)
{
    return pointer.operator->() == other;
}

template<typename T>
inline bool operator == (const T * other, const HandlePtr<T> & pointer)
{
    return pointer.operator->() == other;
}

template<typename T>
inline bool operator != (const HandlePtr<T> & pointer, const T * other)
{
    return pointer.operator->() != other;
}

template<typename T>
inline bool operator != (const T * other, const HandlePtr<T> & pointer)
{
    return pointer.operator->() != other;
}

template<class T>
HandlePtr<T>::HandlePtr(T* realPtr)
    : pointee(realPtr)
{
    init();
}

template<class T>
HandlePtr<T>::HandlePtr(const HandlePtr& rhs)
    : pointee(rhs.pointee)
{
    init();
}

template<class T>
HandlePtr<T>::~HandlePtr()
{
    if(pointee != 0) pointee->removeReference();
}

template<class T>
HandlePtr<T>& HandlePtr<T>::operator=(const HandlePtr& rhs)
{
    if (pointee != rhs.pointee) {
        if(pointee != 0) pointee->removeReference();
        pointee = rhs.pointee;
        init();
    }
    return *this;
}

template<class T>
T* HandlePtr<T>::operator->() const
{ return pointee; }

template<class T>
T& HandlePtr<T>::operator*() const
{ return *(pointee); }

#endif
