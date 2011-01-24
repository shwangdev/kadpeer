/**
 * @file   kadroutingtable.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Mon Dec  6 14:19:14 2010
 *
 * @brief  kadroutingtable Construction
 *
 *
 */

#ifndef __KAD_ROUTING_TABLE_H__
#define __KAD_ROUTING_TABLE_H__

#include <vector>
#include <list>
#include <map>
#include "common/HandleClass.h"
#include "kadid.h"

namespace kad {

    class Contact;
    class KBucket;

    class RoutingTable {
    public:
        RoutingTable(const KadID &holder_id, const unsigned short int &rt_K);
        ~RoutingTable();

        int AddContact(const Contact &new_contact);

        bool GetContact(const KadID &node_id, Contact *contact);

        void RemoveContact(const KadID &node_id, const bool &force);

        void TouchKBucket(const KadID &node_id);

        void FindCloseNodes(const KadID &key, int count,
                            const std::vector<Contact> &exclude_contacts,
                            std::vector<Contact> *close_nodes);

        void GetRefreshList(const unsigned short int &start_kbucket, const bool &force,
                            std::vector<KadID> *ids);

        bool GetContacts(const unsigned short int &index,
                         const std::vector<Contact> &exclude_contacts,
                         std::vector<Contact> *contacts);
        size_t KbucketSize() const;
        size_t Size() const;
        void Clear();

        unsigned short int  KBucketIndex(const KadID &key);
        Contact GetLastSeenContact(const unsigned short int &kbucket_index);
        void GetFurthestContacts(const KadID &key, const unsigned short int count,
                                 const std::vector<Contact> &exclude_contacts,
                                 std::vector<Contact> *close_nodes);

    private:
        std::vector<unsigned short int> SortBucketsByDistance(const KadID &key);

        int SortContactsByDistance(const KadID &key, std::vector<Contact> *contacts);

        void SplitKbucket(const unsigned short int &index);

        int ForceKAcceptNewPeer(const Contact &new_contact);

        std::vector< HandlePtr<KBucket> > k_buckets_;

        std::map<KadID, unsigned short int> bucket_upper_address_;

        KadID holder_id_;

        unsigned short int bucket_of_holder_, brother_bucket_of_holder_;

        KadID address_space_upper_address_;

        unsigned short int K_;
    };

}
#endif
