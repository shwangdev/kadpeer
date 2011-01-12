/**
 * @file   kbucket.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Mon Dec  6 10:21:16 2010
 *
 * @brief kademlia algorithm construction of kbucket element
 *
 *
 */

#ifndef __KBUCKET_H_
#define __KBUCKET_H_

#include <vector>
#include <list>
#include "HandleClass.h"
#include "kadid.h"

namespace kad {

    enum KBucketExitCode { SUCCEED, FULL, FAIL };

    class Contact;

    class KBucket:public Object
    {
    public:

        KBucket(const KadID &min, const KadID &max,
                const unsigned short int &kb_K);
        ~KBucket();

        KBucketExitCode AddContact(const Contact &new_contact);

        bool GetContact(const KadID &node_id, Contact *contact);

        void GetContacts(const unsigned short int &count,
                         const std::vector<Contact> &exclude_contacts,
                         std::vector<Contact> *contacts);

        void RemoveContact(const KadID &node_id, const bool &force);

        bool KeyInRange(const KadID &key);

        size_t Size() const;

        Contact LastSeenContact();
        unsigned int last_accessed() const;
        void set_last_accessed(const unsigned int &time_accessed);
        KadID range_min() const;
        KadID range_max() const;

    private:
        unsigned int last_accessed_;
        std::list<Contact> contacts_;
        KadID range_min_, range_max_;
        unsigned short int K_;
    };
}

#endif
