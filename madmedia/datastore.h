/**
 * @file   datastore.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Thu Jan 13 09:51:32 2011
 *
 * @brief
 *
 *
 */

#ifndef MAIDSAFE_KADEMLIA_DATASTORE_H_
#define MAIDSAFE_KADEMLIA_DATASTORE_H_

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/cstdint.hpp>
#include <boost/thread/mutex.hpp>
#include <string>
#include <vector>
#include <set>
#include <utility>
#include <stdint.h>

namespace kad {

// This class implements physical storage (for data published and fetched via
// the RPCs) for the Kademlia DHT. Boost::multiindex are used

    enum delete_status {
        NOT_DELETED,
        MARKED_FOR_DELETION,
        DELETED
    };

    struct refresh_value {
        std::string key_, value_;
        boost::int32_t ttl_;

        delete_status del_status_;

        refresh_value(const std::string &key, const std::string &value, const
                      int32_t &ttl)
            : key_(key), value_(value), ttl_(ttl), del_status_(NOT_DELETED) {}
        refresh_value(const std::string &key, const std::string &value, const
                      delete_status &del_status)
            : key_(key), value_(value), ttl_(0), del_status_(del_status) {}
    };

    struct key_value_tuple {
        std::string key_, value_, ser_delete_req_;
        boost::uint32_t last_refresh_time_, expire_time_;
        boost::int32_t ttl_;
        bool hashable_;
        delete_status del_status_;

        key_value_tuple(const std::string &key, const std::string &value,
                        const uint32_t &last_refresh_time,
                        const uint32_t &expire_time, const int32_t &ttl,
                        const bool &hashable)
            : key_(key), value_(value), ser_delete_req_(),
              last_refresh_time_(last_refresh_time), expire_time_(expire_time),
              ttl_(ttl), hashable_(hashable), del_status_(NOT_DELETED) {
            if (ttl < 0)
                expire_time_ = 0;
        }
        key_value_tuple(const std::string &key, const std::string &value,
                        const uint32_t &last_refresh_time)
            : key_(key), value_(value), ser_delete_req_(),
              last_refresh_time_(last_refresh_time), expire_time_(0), ttl_(0),
              hashable_(true), del_status_(NOT_DELETED) {}
    };



    struct t_key {};
    struct t_last_refresh_time {};
    struct t_expire_time {};

    typedef boost::multi_index::multi_index_container<
        key_value_tuple,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<t_key>,
                boost::multi_index::composite_key<
                    key_value_tuple,
                    BOOST_MULTI_INDEX_MEMBER(key_value_tuple, std::string, key_),
                    BOOST_MULTI_INDEX_MEMBER(key_value_tuple, std::string, value_)
                    >
                >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<t_last_refresh_time>,
                BOOST_MULTI_INDEX_MEMBER(key_value_tuple, boost::uint32_t,
                                         last_refresh_time_)
                >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<t_expire_time>,
                BOOST_MULTI_INDEX_MEMBER(key_value_tuple, boost::uint32_t,
                                         expire_time_)
                >
            >
    > datastore;

class DataStore {
public:
    // t_refresh = refresh time of key/value pair in seconds

    explicit DataStore(const uint32_t &t_refresh);

    ~DataStore();
    bool Keys(std::set<std::string> *keys);
    // time_to_live is in seconds.

    bool StoreItem(const std::string &key, const std::string &value,
                   const int32_t &time_to_live, const bool &hashable);

    bool LoadItem(const std::string &key, std::vector<std::string> *values);
    bool DeleteKey(const std::string &key);
    bool DeleteItem(const std::string &key, const std::string &value);
    void DeleteExpiredValues();

    uint32_t LastRefreshTime(const std::string &key,
                             const std::string &value);
    uint32_t ExpireTime(const std::string &key, const std::string &value);
    std::vector<refresh_value> ValuesToRefresh();
    int32_t TimeToLive(const std::string &key, const std::string &value);
    void Clear();
    std::vector< std::pair<std::string, bool> > LoadKeyAppendableAttr(
        const std::string &key);
    bool RefreshItem(const std::string &key, const std::string &value,
                     std::string *str_delete_req);
    // If key, value pair does not exist, then it returns false
    bool MarkForDeletion(const std::string &key, const std::string &value,
                         const std::string &ser_del_request);

    // If key, value pair does not exist or its status is not MARKED_FOR_DELETION,
    // then it returns false

    bool MarkAsDeleted(const std::string &key, const std::string &value);

    bool UpdateItem(const std::string &key,
                    const std::string &old_value,
                    const std::string &new_value,
                    const int32_t &time_to_live,
                    const bool &hashable);
    uint32_t t_refresh() const;
private:
    datastore datastore_;
    // refresh time in seconds
    uint32_t t_refresh_;
    boost::mutex mutex_;
};

}
#endif
