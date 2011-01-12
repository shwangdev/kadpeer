/**
 * @file   kadroutingtable.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Mon Dec  6 14:29:23 2010
 *
 * @brief
 * @brief  Kademlia Algorithm RoutingTable Construction
 *
 *
 */


#include "kadroutingtable.h"
#include "kadid.h"
#include "kbucket.h"
#include "contact.h"
#include <utility>
#include <string>
#include <map>
#include "TTimer.hh"
#include "kademlia_config.h"


namespace kad{
    RoutingTable::RoutingTable(const KadID &holder_id, const unsigned short int &rt_K)
        : k_buckets_(), bucket_upper_address_(), holder_id_(holder_id),
          bucket_of_holder_(0), brother_bucket_of_holder_(-1),
          address_space_upper_address_(KadID::KMaxID()), K_(rt_K)
    {
        KadID min_range;
        HandlePtr<KBucket> kbucket(new KBucket(min_range,
                                               address_space_upper_address_, K_));
        k_buckets_.push_back(kbucket);
        bucket_upper_address_.insert(std::pair<KadID, unsigned short int>
                                     (address_space_upper_address_, 0));
    }

    RoutingTable::~RoutingTable() {
        k_buckets_.clear();
        bucket_upper_address_.clear();
    }

    unsigned short int  RoutingTable::KBucketIndex(const KadID &key) {
        if (key > address_space_upper_address_)
            return -1;
        std::map<KadID, unsigned short int>::iterator lower_bound_iter =
            bucket_upper_address_.lower_bound(key);
        return (*lower_bound_iter).second;
    }

    std::vector<unsigned short int> RoutingTable::SortBucketsByDistance(
        const KadID &key) {
        std::map<KadID, unsigned short int> distance;
        std::map<KadID, unsigned short int>::iterator iter;
        for (iter = bucket_upper_address_.begin();
             iter != bucket_upper_address_.end(); ++iter) {
            distance.insert(std::pair<KadID, unsigned short int>(((*iter).first ^ key),
                                                                 (*iter).second));
        }
        std::vector<unsigned short int> indices;
        for (std::map<KadID, unsigned short int>::iterator dist_iter = distance.begin();
             dist_iter != distance.end(); ++dist_iter) {
            indices.push_back((*dist_iter).second);
        }
        return indices;
    }

    int RoutingTable::SortContactsByDistance(const KadID &key,
                                             std::vector<Contact> *contacts) {
        size_t number_of_contacts = contacts->size();
        std::map<KadID, Contact> distance;
        for (int i = 0; i < contacts->size(); ++i) {
            distance.insert(std::pair<KadID, Contact> (contacts->at(i).node_id() ^ key,
                                                       contacts->at(i)));
        }
        contacts->clear();
        for (std::map<KadID, Contact>::const_iterator dist_iter = distance.begin();
             dist_iter != distance.end(); ++dist_iter)
            contacts->push_back((*dist_iter).second);
        return contacts->size() == number_of_contacts ? 0 : -1;
    }

    bool RoutingTable::GetContact(const KadID &node_id, Contact *contact) {
        int index = KBucketIndex(node_id);
        if (index < 0)
            return false;
        if (!k_buckets_[index]->GetContact(node_id, contact))
            return false;
        return true;
    }

    void RoutingTable::TouchKBucket(const KadID &node_id) {
        int index = KBucketIndex(node_id);
        if (index < 0)
            return;
        TTimer timer;
        k_buckets_[index]->set_last_accessed(static_cast<unsigned int>(timer.system_time()));
    }

    void RoutingTable::RemoveContact(const KadID &node_id, const bool &force) {
        int index = KBucketIndex(node_id);
        if (index < 0)
            return;
        k_buckets_[index]->RemoveContact(node_id, force);
    }

    void RoutingTable::SplitKbucket(const unsigned short int &index) {
        KadID range_max_kb_left, range_min_kb_right;
        KadID::SplitRange(k_buckets_[index]->range_min(),
                          k_buckets_[index]->range_max(), &range_max_kb_left, &range_min_kb_right);
        HandlePtr<KBucket> kb_left(new KBucket
                                   (k_buckets_[index]->range_min(), range_max_kb_left, K_));
        HandlePtr<KBucket> kb_right(new KBucket
                                    (range_min_kb_right, k_buckets_[index]->range_max(), K_));
        std::vector<Contact> contacts, ex_contacts;
        k_buckets_[index]->GetContacts(K_, ex_contacts, &contacts);
        int clb(0), crb(0);
        for (int i = contacts.size()-1; i > -1; --i) {
            Contact contact = contacts[i];
            KBucketExitCode exitcode;
            if (kb_left->KeyInRange(contact.node_id())) {
                exitcode = kb_left->AddContact(contact);
                ++clb;
            } else {
                exitcode = kb_right->AddContact(contact);
                ++crb;
            }
        }
        k_buckets_.erase(k_buckets_.begin()+index);
        k_buckets_.insert(k_buckets_.begin()+index, kb_left);
        k_buckets_.insert(k_buckets_.begin()+index+1, kb_right);
        bucket_upper_address_.clear();
        for (size_t j = 0; j < k_buckets_.size(); ++j)
            bucket_upper_address_.insert(std::pair<KadID, unsigned short int>
                                         (k_buckets_[j]->range_max(), j));
        if (k_buckets_[index]->KeyInRange(holder_id_)) {
            bucket_of_holder_ = index;
            brother_bucket_of_holder_ = index + 1;
        } else {
            bucket_of_holder_ = index + 1;
            brother_bucket_of_holder_ = index;
        }
    }

    int RoutingTable::AddContact(const Contact &new_contact) {
        short int index = KBucketIndex(new_contact.node_id());
        KBucketExitCode exitcode = FAIL;
        if (index < 0)
            return 3;
        exitcode = k_buckets_[index]->AddContact(new_contact);
        switch (exitcode) {
        case SUCCEED: return 0;
        case FULL: if (K_ > 2 && !k_buckets_[index]->KeyInRange(holder_id_)) {
                if (index == brother_bucket_of_holder_) {
                    if (ForceKAcceptNewPeer(new_contact) != 0) {
                        return 2;
                    } else {
                        return 0;
                    }
                }
                return 2;
            }
            SplitKbucket(index);
            return AddContact(new_contact);
        case FAIL:
        default: return -2;
        }
    }

    void RoutingTable::FindCloseNodes(
        const KadID &key, int count, const std::vector<Contact> &exclude_contacts,
        std::vector<Contact> *close_nodes) {
        int index = KBucketIndex(key);
        if (index < 0)
            return;
        k_buckets_[index]->GetContacts(count, exclude_contacts, close_nodes);
        bool full = (count == static_cast<int>(close_nodes->size()));
        if (full)
            return;
        std::vector<unsigned short int> indices = SortBucketsByDistance(key);
        for (unsigned int index_no = 1; index_no < indices.size(); ++index_no) {
            std::vector<Contact> contacts;
            k_buckets_[indices[index_no]]->GetContacts(K_, exclude_contacts, &contacts);
            if (0 != SortContactsByDistance(key, &contacts))
                continue;
            unsigned int iter(0);
            while (!full && iter < contacts.size()) {
                close_nodes->push_back(contacts[iter]);
                ++iter;
                full = (count == static_cast<int>(close_nodes->size()));
            }
            if (full)
                return;
        }
    }

    void RoutingTable::GetRefreshList(const unsigned short int &start_kbucket,
                                      const bool &force, std::vector<KadID> *ids) {
        TTimer timer ;
        unsigned int curr_time = static_cast<unsigned int> (timer.system_time());
        for (size_t i = start_kbucket; i < k_buckets_.size(); ++i)
            if (force || curr_time-k_buckets_[i]->last_accessed() > KRefreshTime) {
                ids->push_back(KadID(k_buckets_[i]->range_min(),
                                     k_buckets_[i]->range_max()));
            }
    }

    size_t RoutingTable::KbucketSize() const { return k_buckets_.size(); }

    size_t RoutingTable::Size() const {
        size_t size(0);
        std::vector< HandlePtr<KBucket> >::const_iterator it;
        for (it = k_buckets_.begin(); it != k_buckets_.end(); ++it) {
            size += (*it)->Size();
        }
        return size;
    }

    bool RoutingTable::GetContacts(const unsigned short int &index,
                                   const std::vector<Contact> &exclude_contacts,
                                   std::vector<Contact> *contacts) {
        if (index > k_buckets_.size())
            return false;
        contacts->clear();
        k_buckets_[index]->GetContacts(K_, exclude_contacts, contacts);
        return true;
    }

    void RoutingTable::Clear() {
        k_buckets_.clear();
        bucket_upper_address_.clear();
        KadID min_range;
        HandlePtr<KBucket> kbucket(new KBucket(min_range,
                                               address_space_upper_address_, K_));
        k_buckets_.push_back(kbucket);
        bucket_upper_address_.insert(std::pair<KadID, unsigned short int>
                                     (address_space_upper_address_, 0));
    }

    namespace detail {
        struct ForceKEntry {
            Contact contact;
            int score;
        };

        struct ContactWithTargetPeer {
            Contact contact;
            KadID holder_id;
        };

        bool compare_distance(const ContactWithTargetPeer &first,
                              const ContactWithTargetPeer &second) {
            KadID id;
            if (first.contact.node_id() == id)
                return true;
            if (second.contact.node_id() == id)
                return false;
            return KadID::CloserToTarget(first.contact.node_id(),
                                         second.contact.node_id(), first.holder_id);
        }

        bool compare_time(const ContactWithTargetPeer &first,
                          const ContactWithTargetPeer &second) {
            if (first.contact.last_seen() > second.contact.last_seen())
                return true;
            else
                return false;
        }

        bool compare_score(const ForceKEntry &first, const ForceKEntry &second) {
            if (first.score > second.score)
                return true;
            else
                return false;
        }

        bool get_least_useful_contact(std::list<ContactWithTargetPeer> l,
                                      Contact *least_useful_contact) {
            l.sort(compare_distance);
            std::list<ForceKEntry> l_score;
            int d = 1;
            for (std::list<ContactWithTargetPeer>::iterator it = l.begin();
                 it != l.end(); ++it) {
                ForceKEntry entry = {it->contact, d++};
                l_score.push_back(entry);
            }
            l.sort(compare_time);
            int t = 1;
            for (std::list<ContactWithTargetPeer>::iterator it = l.begin();
                 it != l.end(); ++it) {
                for (std::list<ForceKEntry>::iterator it1 = l_score.begin();
                     it1 != l_score.end(); ++it1) {
                    if (it->contact.Equals(it1->contact))
                        it1->score += t++;
                }
            }
            l_score.sort(compare_score);
            if (!l_score.empty()) {
                *least_useful_contact = l_score.front().contact;
                return true;
            } else {
                return false;
            }
        }

    }

    int RoutingTable::ForceKAcceptNewPeer(const Contact &new_contact) {
        int v = K_ - k_buckets_[bucket_of_holder_]->Size();
        if (v == 0) {
            return 1;
        }
        std::vector<Contact> contacts, ex_contacts;
        k_buckets_[brother_bucket_of_holder_]->GetContacts(K_, ex_contacts,
                                                           &contacts);
        std::list<detail::ContactWithTargetPeer> candidates_for_l;
        for (size_t i = 0; i < contacts.size(); ++i) {
            detail::ContactWithTargetPeer entry = {contacts[i], holder_id_};
            candidates_for_l.push_back(entry);
        }
        candidates_for_l.sort(detail::compare_distance);
        std::list<detail::ContactWithTargetPeer>::iterator it;
        it = candidates_for_l.begin();
        advance(it, v - 1);
        if (it == candidates_for_l.end()) {
            return 1;
        }
        if (KadID::CloserToTarget(it->contact.node_id(), new_contact.node_id(),
                                  holder_id_)) {
            return 1;
        }
        std::list<detail::ContactWithTargetPeer> l;
        for (; it != candidates_for_l.end(); ++it)
            l.push_back(*it);
        Contact least_useful_contact;
        if (detail::get_least_useful_contact(l, &least_useful_contact)) {
            k_buckets_[brother_bucket_of_holder_]->RemoveContact(
                least_useful_contact.node_id(), true);
            k_buckets_[brother_bucket_of_holder_]->AddContact(new_contact);
            return 0;
        }
        return -1;
    }

    Contact RoutingTable::GetLastSeenContact(const unsigned short int &kbucket_index) {
        Contact last_seen;
        if (static_cast<size_t>(kbucket_index + 1) > k_buckets_.size())
            return last_seen;
        return k_buckets_[kbucket_index]->LastSeenContact();
    }

    void RoutingTable::GetFurthestContacts(
        const KadID &key, const unsigned short int count,
        const std::vector<Contact> &exclude_contacts,
        std::vector<Contact> *close_nodes)
    {
        if (count < -1 || count == 0) {
            close_nodes->clear();
            return;
        }

        for (size_t n = 0; n < k_buckets_.size(); ++n) {
            k_buckets_[n]->GetContacts(K_, exclude_contacts, close_nodes);
        }

        int a = SortContactsByDistance(key, close_nodes);
        if (a != 0) {
            close_nodes->clear();
            return;
        } else {
            std::reverse(close_nodes->begin(), close_nodes->end());
        }

        if (count > 0 && close_nodes->size() > size_t(count)) {
            close_nodes->resize(count);
        }

    }

}
