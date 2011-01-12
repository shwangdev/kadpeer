/**
 * @file   kbucket.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Mon Dec  6 10:43:05 2010
 *
 * @brief Kbucket construction
 *
 *
 */

#include "kbucket.h"
#include <string>
#include "contact.h"

namespace kad
{
    KBucket::KBucket(const KadID &min, const KadID &max,
                     const unsigned short int &kb_K)
        :last_accessed_(0), contacts_(), range_min_(min), range_max_(max),
         K_(kb_K),Object(){}

    KBucket::~KBucket() {
        contacts_.clear();
    }

    bool KBucket::KeyInRange(const KadID &key) {
        return static_cast<bool>((range_min_ <= key) && (key <= range_max_));
    }

    size_t KBucket::Size() const { return contacts_.size(); }

    unsigned int KBucket::last_accessed() const { return last_accessed_; }

    void KBucket::set_last_accessed(const unsigned int &time_accessed) {
        last_accessed_  = time_accessed;
    }

    KBucketExitCode KBucket::AddContact(const Contact &new_contact) {
        Contact new_contact_local(new_contact);
        int position(-1), i(0);

        for (std::list<Contact>::const_iterator it = contacts_.begin();
             it != contacts_.end() && position == -1; ++it)
        {
            Contact current_element = *it;
            if (new_contact_local.Equals(current_element))
                position = i;
            ++i;
        }
        if (position != -1) {
            std::list<Contact>::iterator it = contacts_.begin();
            std::advance(it, position);
            contacts_.erase(it);
        }

        if (contacts_.size() == K_)
            return FULL;

        contacts_.push_front(new_contact_local);
        return SUCCEED;
    }

    void KBucket::RemoveContact(const KadID &node_id, const bool &force = true ) {
        int position(-1), i(0);
        for (std::list<Contact>::const_iterator it = contacts_.begin();
             it != contacts_.end(); ++it)
        {
            if (it->node_id() == node_id)
            {
                position = i;
            }
            ++i;
        }

        if (position != -1) {
            std::list<Contact>::iterator it = contacts_.begin();
            std::advance(it, position);
            Contact current_element = *it;
            //  current_element.IncreaseFailed_RPC();
            contacts_.erase(it);
            // if (current_element.failed_rpc() <= kFailedRpc && !force) {
            //     std::list<Contact>::iterator new_it = contacts_.begin();
            //     std::advance(new_it, position);
            //    contacts_.insert(new_it, current_element);
            //}
        }
    }

    bool KBucket::GetContact(const KadID &node_id, Contact *contact) {
        bool result = false;
        for (std::list<Contact>::const_iterator it = contacts_.begin();
             it != contacts_.end() && !result; ++it) {
            if (it->node_id() == node_id) {
                *contact = (*it);
                result = true;
            }
        }
        return result;
    }

    void KBucket::GetContacts(const unsigned short int &count,
                              const std::vector<Contact> &exclude_contacts,
                              std::vector<Contact> *contacts) {
        bool insert;
        unsigned short int i(0);
        for (std::list<Contact>::iterator it = contacts_.begin();
             it != contacts_.end() && i < count; ++it) {
            insert = true;
            for (std::vector<Contact>::const_iterator it1 = exclude_contacts.begin();
                 it1 != exclude_contacts.end() && insert; ++it1) {
                if (it->node_id() == it1->node_id())
                    insert = false;
            }
            if (insert) {
                contacts->push_back(*it);
                ++i;
            }
        }
    }

    KadID KBucket::range_min() const { return range_min_; }

    KadID KBucket::range_max() const { return range_max_; }

    Contact KBucket::LastSeenContact() {
        if (contacts_.empty()) {
            Contact empty;
            return empty;
        }
        return contacts_.back();
    }
}
