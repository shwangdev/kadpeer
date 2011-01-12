/**
 * @file   HashMap.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Aug 18 17:41:53 2010
 *
 * @brief  Hash Map with old head file ext/hash_map
 *
 *
 */

#ifndef HASH_MAP_H
#define HASH_MAP_H
#include <ext/hash_map>
#include <boost/shared_ptr.hpp>
#include <new>

template <class K, class V, class F, class E>
class HashMap : public __gnu_cxx::hash_map<K, V, F, E>
{

};
//Hash
struct CharArrayEqualFunc
{
    bool operator()(const char * s1, const char * s2) const;
};

//Hash Function
struct CharArrayHashFunc
{
    size_t operator()(const char * s) const;
};


// Entry Hash Node
template <class T>
class StringMapEntry
{
public:
    StringMapEntry(char * key,
           const T & value) {
    _key = key;
    _value = value;
    };

    char * getKey() const { return _key; };

    const T & getValue() const { return _value; };

    void setValue(const T & v) { _value = v; };

    ~StringMapEntry() {
    delete [] _key;
    };

private:
    char * _key;
    T _value;
};


//Hash Map
template <class T>
class StringMap {

public:
//
    typedef typename HashMap<const char *, boost::shared_ptr< StringMapEntry<T> >,
                             CharArrayHashFunc, CharArrayEqualFunc>::iterator
                             iterator;

    typedef typename HashMap<const char *, boost::shared_ptr< StringMapEntry<T> >,
                             CharArrayHashFunc, CharArrayEqualFunc>::const_iterator
                             const_iterator;

bool get(const char * key, T & value) const {
    const_iterator it;

    if((it = _map.find(key)) != _map.end()){
        value = (* it).second->getValue();
        return true;
    }

    return false;
}

    boost::shared_ptr< StringMapEntry<T> > get(const char * key) const {
        const_iterator it;

        if((it = _map.find(key)) != _map.end()){
            return (* it).second;
        }

        return boost::shared_ptr< StringMapEntry<T> >();
    }

    bool contains(const char * key) const {
        if(_map.find(key) != _map.end()){
            return true;
        }
        return false;
    }

    void getKeys(std::vector<std::string> & keys) const {
        for(const_iterator it = begin(); it != end(); it ++){
            keys.push_back((* it).second->getKey());
        }
    }
    bool set(const char * key, const T & value) {
        if(contains(key)){
            return false;
        }

        int keyLen = 0;
        for(; key[keyLen] != 0; keyLen ++);

        char * newKey = new char[keyLen + 1];
        for(int i = 0; i < keyLen; i ++) newKey[i] = key[i];
        newKey[keyLen] = '\0';

        boost::shared_ptr< StringMapEntry<T> > p(new StringMapEntry<T>(newKey, value));
        _map[p->getKey()] = p;

        return true;
    }

    bool overwrite(const char * key, const T & value) {
        if(contains(key)){
            iterator it = _map.find(key);
            (* it).second->setValue(value);
            return true;
        } else {
            return set(key, value);
        }
    }


    size_t size() const { return _map.size(); };

    bool empty() const { return _map.empty(); };

    const_iterator begin() const { return _map.begin(); };

    const_iterator end() const { return _map.end(); };

    iterator mbegin() { return _map.begin(); };

    iterator mend() { return _map.end(); };

protected:

    HashMap<const char *, boost::shared_ptr< StringMapEntry<T> >,
            CharArrayHashFunc,
            CharArrayEqualFunc> _map;

};
#endif
