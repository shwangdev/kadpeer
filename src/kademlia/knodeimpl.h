/**
 * @file   knodeimpl.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Thu Mar  3 19:06:00 2011
 *
 * @brief
 *
 *
 */
#ifndef KNODEIMPL_H
#define KNODEIMPL_H

#include "boost/thread.hpp"
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>

#include <Thrift.h>
#include <transport/TTransportUtils.h>
#include <transport/TSocket.h>
#include <protocol/TProtocol.h>
#include <protocol/TBinaryProtocol.h>

#include "kadrpc.h"
#include "datastore.h"
#include "kadroutingtable.h"
#include "kademlia_service_messages_types.h"
#include "contact_info_types.h"
#include "signed_kadvalue_types.h"
#include "kademlia_config.h"
#include "kadservice.h"

namespace kad
{
    class ContactInfo;
    struct LookupContact;

    struct ContactAndTargetKey {
        ContactAndTargetKey() : contact(), target_key(), contacted(false) {
        }
        Contact contact;
        KadID target_key;
        bool contacted;
    };


    bool CompareContact(const ContactAndTargetKey &first,
                        const ContactAndTargetKey &second);

    enum RemoteFindMethod {FIND_NODE, FIND_VALUE, BOOTSTRAP };

    void SortContactList(const KadID &target_key,
                         std::list<Contact> *contact_list);

    void SortLookupContact(const KadID &target_key,
                           std::list<LookupContact> *contact_list);


    inline void dummy_callback(const std::string&) {
    }

    inline void dummy_downlist_callback(DownlistResponse *response) {
        delete response;
    }

    struct DownListCandidate {
        DownListCandidate() : node(), is_down(false) {
        }
        Contact node;
        bool is_down;// flag to mark whether this node is down
    };


    struct DownListData {
        DownListData() : giver(), candidate_list() {
        }
        Contact giver;
        std::list<struct DownListCandidate> candidate_list;
    };


    struct LookupContact {
        LookupContact() : kad_contact(), contacted(false) {
        }
        Contact kad_contact;
        bool contacted;
    };


    struct IterativeLookUpData {

        IterativeLookUpData(const RemoteFindMethod &method,
                            const KadID &key, VoidFunctorOneString callback)
            : method(method), key(key), short_list(), current_alpha(),
              active_contacts(), active_probes(),
              values_found(), dead_ids(), downlist(), downlist_sent(false),
              in_final_iteration(false), is_callbacked(false), wait_for_key(false),
              callback(callback), alternative_value_holder(), sig_values_found() {
        }
        RemoteFindMethod method;
        KadID key;
        std::list<LookupContact> short_list;
        std::list<Contact> current_alpha, active_contacts, active_probes;
        std::list<std::string> values_found, dead_ids;
        std::list<struct DownListData> downlist;
        bool downlist_sent, in_final_iteration, is_callbacked, wait_for_key;
        VoidFunctorOneString callback;
        ContactInfo alternative_value_holder;
        std::list<kad::SignedValue> sig_values_found;
    };

    struct IterativeStoreValueData {

        IterativeStoreValueData(const std::vector<Contact> &close_nodes,
                                const KadID &key, const std::string &value,
                                VoidFunctorOneString callback,
                                const bool &publish_val,
                                const boost::int32_t &timetolive,
                                const SignedValue &svalue,
                                const SignedRequest &sreq)
            : closest_nodes(close_nodes), key(key), value(value), save_nodes(0),
              contacted_nodes(0), index(-1), callback(callback), is_callbacked(false),
              data_type(0), publish(publish_val), ttl(timetolive), sig_value(svalue),
              sig_request(sreq) {
        }

        IterativeStoreValueData(const std::vector<Contact> &close_nodes,
                                const KadID &key, const std::string &value,
                                VoidFunctorOneString callback,
                                const bool &publish_val,
                                const boost::uint32_t &timetolive)
            : closest_nodes(close_nodes), key(key), value(value), save_nodes(0),
              contacted_nodes(0), index(-1), callback(callback), is_callbacked(false),
              data_type(0), publish(publish_val), ttl(timetolive), sig_value(),
              sig_request() {
        }
        std::vector<Contact> closest_nodes;
        KadID key;
        std::string value;
        boost::uint32_t save_nodes, contacted_nodes, index;
        VoidFunctorOneString callback;
        bool is_callbacked;
        int data_type;
        bool publish;
        boost::int32_t ttl;
        SignedValue sig_value;
        SignedRequest sig_request;
    };


    struct IterativeDelValueData {

        IterativeDelValueData(const std::vector<Contact> &close_nodes,
                              const KadID &key, const SignedValue &svalue,
                              const SignedRequest &sreq, VoidFunctorOneString callback)
            : closest_nodes(close_nodes), key(key), del_nodes(0), contacted_nodes(0),
              index(-1), callback(callback), is_callbacked(false), value(svalue),
              sig_request(sreq) {
        }
        std::vector<Contact> closest_nodes;
        KadID key;
        boost::uint32_t del_nodes, contacted_nodes, index;
        VoidFunctorOneString callback;
        bool is_callbacked;
        SignedValue value;
        SignedRequest sig_request;
    };

    struct UpdateValueData {

        UpdateValueData(const KadID &key, const SignedValue &old_value,
                        const SignedValue &new_value, const SignedRequest &sreq,
                        VoidFunctorOneString callback, boost::uint8_t foundnodes)
            : uvd_key(key), uvd_old_value(old_value), uvd_new_value(new_value),
              uvd_request_signature(sreq), uvd_callback(callback), uvd_calledback(0),
              uvd_succeeded(0), retries(0), found_nodes(foundnodes), ttl(0),
              mutex() {
        }
        KadID uvd_key;
        SignedValue uvd_old_value;
        SignedValue uvd_new_value;
        SignedRequest uvd_request_signature;
        VoidFunctorOneString uvd_callback;
        boost::uint8_t uvd_calledback;
        boost::uint8_t uvd_succeeded;
        boost::uint8_t retries;
        boost::uint8_t found_nodes;
        boost::uint32_t ttl;
        boost::mutex mutex;
    };


    struct FindCallbackArgs {

    public:
        explicit FindCallbackArgs(boost::shared_ptr<IterativeLookUpData> data)
            : remote_ctc(), data(data), retry(false) {
        }

        FindCallbackArgs(const FindCallbackArgs &findcbargs)
            : remote_ctc(findcbargs.remote_ctc), data(findcbargs.data),
              retry(findcbargs.retry) {
        }

        FindCallbackArgs &operator=(const FindCallbackArgs &findcbargs) {

            if (this != &findcbargs) {

                remote_ctc = findcbargs.remote_ctc;
                data = findcbargs.data;
                retry = findcbargs.retry;
            }
            return *this;
        }
        Contact remote_ctc;
        boost::shared_ptr<IterativeLookUpData> data;
        bool retry;
    };


    struct StoreCallbackArgs {

        explicit StoreCallbackArgs(boost::shared_ptr<IterativeStoreValueData> data)
            : remote_ctc(), data(data), retry(false) {
        }
        StoreCallbackArgs(const StoreCallbackArgs &storecbargs)
            : remote_ctc(storecbargs.remote_ctc), data(storecbargs.data),
              retry(storecbargs.retry) {
        }
        StoreCallbackArgs &operator=(const StoreCallbackArgs &storecbargs) {

            if (this != &storecbargs) {

                remote_ctc = storecbargs.remote_ctc;
                data = storecbargs.data;
                retry = storecbargs.retry;
            }
            return *this;
        }
        Contact remote_ctc;
        boost::shared_ptr<IterativeStoreValueData> data;
        bool retry;
    };


    struct PingCallbackArgs {

        explicit PingCallbackArgs(VoidFunctorOneString callback)
            : remote_ctc(), callback(callback), retry(false) {
        }
        PingCallbackArgs(const PingCallbackArgs &pingcbargs)
            : remote_ctc(pingcbargs.remote_ctc), callback(pingcbargs.callback),
              retry(pingcbargs.retry){
        }
        PingCallbackArgs &operator=(const PingCallbackArgs &pingcbargs) {

            if (this != &pingcbargs) {

                remote_ctc = pingcbargs.remote_ctc;
                callback = pingcbargs.callback;
                retry = pingcbargs.retry;
            }
            return *this;
        }
        Contact remote_ctc;
        VoidFunctorOneString callback;
        bool retry;
    };



    struct DeleteCallbackArgs {

        explicit DeleteCallbackArgs(boost::shared_ptr<IterativeDelValueData> data)
            : remote_ctc(), data(data), retry(false){
        }
        DeleteCallbackArgs(const DeleteCallbackArgs &delcbargs)
            : remote_ctc(delcbargs.remote_ctc), data(delcbargs.data),
              retry(delcbargs.retry){
        }
        DeleteCallbackArgs &operator=(const DeleteCallbackArgs &delcbargs) {

            if (this != &delcbargs) {

                remote_ctc = delcbargs.remote_ctc;
                data = delcbargs.data;
                retry = delcbargs.retry;
            }
            return *this;
        }
        Contact remote_ctc;
        boost::shared_ptr<IterativeDelValueData> data;
        bool retry;
    };


    struct UpdateCallbackArgs {

        UpdateCallbackArgs()
            : uvd(), retries(0), response(NULL), ct(), contact() {
        }
        UpdateCallbackArgs(const UpdateCallbackArgs &updatecbargs)
            : uvd(updatecbargs.uvd), retries(updatecbargs.retries),
              response(updatecbargs.response),
              ct(updatecbargs.ct), contact(updatecbargs.contact) {
        }
        UpdateCallbackArgs &operator=(const UpdateCallbackArgs &updatecbargs) {

            if (this != &updatecbargs) {

                uvd = updatecbargs.uvd;
                retries = updatecbargs.retries;
                ct = updatecbargs.ct;
                contact = updatecbargs.contact;
                delete response;
                response = updatecbargs.response;
            }
            return *this;
        }
        boost::shared_ptr<UpdateValueData> uvd;
        boost::uint8_t retries;
        UpdateResponse *response;
        ConnectionType ct;
        Contact contact;
    };



    struct BootstrapData {

        BootstrapData() : callback(), bootstrap_ip(), bootstrap_port(0){
        }
        BootstrapData(VoidFunctorOneString cb, const std::string &bs_ip,
                      boost::uint16_t bs_port)
            : callback(cb), bootstrap_ip(bs_ip), bootstrap_port(bs_port){
        }
        BootstrapData(const BootstrapData &data)
            : callback(data.callback), bootstrap_ip(data.bootstrap_ip),
              bootstrap_port(data.bootstrap_port) {
        }
        BootstrapData &operator=(const BootstrapData &data) {

            if (this != &data) {

                callback = data.callback;
                bootstrap_ip = data.bootstrap_ip;
                bootstrap_port = data.bootstrap_port;
            }
            return *this;
        }

        VoidFunctorOneString callback;
        std::string bootstrap_ip;
        boost::uint16_t bootstrap_port;
    };


    struct BootstrapArgs {

        BootstrapArgs() : cached_nodes(), callback(), active_process(0),
                          is_callbacked(false), dir_connected(false) {
        }
        std::vector<Contact> cached_nodes;
        VoidFunctorOneString callback;
        boost::uint16_t active_process;
        bool is_callbacked, dir_connected;
    };


    class KNodeImpl{
    public:
        KNodeImpl(/*rpcprotocol::ChannelManager* channel_manager,
                    transport::TransportHandler *transport_handler,*/

            boost::shared_ptr<apache::thrift::transport::TTransport> socket,
            NodeType type,
            const std::string &private_key, const std::string &public_key,
            const bool &port_forwarded, const bool &use_upnp,
            const boost::uint16_t &k);

        // constructor used to set up parameters k, alpha, and beta for kademlia

        KNodeImpl(/*rpcprotocol::ChannelManager *channel_manager,
                    transport::TransportHandler *transport_handler,*/
            boost::shared_ptr<apache::thrift::transport::TTransport> socket,
            NodeType type,
            const boost::uint16_t &k, const boost::uint16_t &alpha,
            const boost::uint16_t &beta, const boost::uint32_t &refresh_time,
            const std::string &private_key, const std::string &public_key,
            const bool &port_forwarded, const bool &use_upnp);

        virtual ~KNodeImpl();

        // void set_transport_id(const boost::int16_t &transport_id) {

        //     transport_id_ = transport_id;
        // }

        virtual void Join(const KadID &node_id, const std::string &kad_config_file,
                          VoidFunctorOneString callback);
        virtual void Join(const std::string &kad_config_file,
                          VoidFunctorOneString callback);

        // Use this join for the first node in the network

        virtual void Join(const KadID &node_id, const std::string &kad_config_file,
                          const std::string &external_ip,
                          const boost::uint16_t &external_port,
                          VoidFunctorOneString callback);
        virtual void Join(const std::string &kad_config_file,
                          const std::string &external_ip,
                          const boost::uint16_t &external_port,
                          VoidFunctorOneString callback);

        void Leave();
        virtual void StoreValue(const KadID &key, const SignedValue &signed_value,
                                const SignedRequest &signed_request,
                                const boost::int32_t &ttl,
                                VoidFunctorOneString callback);
        virtual void StoreValue(const KadID &key, const std::string &value,
                                const boost::int32_t &ttl,
                                VoidFunctorOneString callback);
        virtual void DeleteValue(const KadID &key, const SignedValue &signed_value,
                                 const SignedRequest &signed_request,
                                 VoidFunctorOneString callback);
        virtual void UpdateValue(const KadID &key, const SignedValue &old_value,
                                 const SignedValue &new_value,
                                 const SignedRequest &signed_request,
                                 boost::uint32_t ttl, VoidFunctorOneString callback);
        virtual void FindValue(const KadID &key, const bool &check_alternative_store,
                               VoidFunctorOneString callback);
        void GetNodeContactDetails(const KadID &node_id,
                                   VoidFunctorOneString callback, const bool &local);
        virtual void FindKClosestNodes(const KadID &node_id,
                                       VoidFunctorOneString callback);
        void GetKNodesFromRoutingTable(const KadID &key,
                                       const std::vector<Contact> &exclude_contacts,
                                       std::vector<Contact> *close_nodes);
        virtual void Ping(const KadID &node_id, VoidFunctorOneString callback);
        virtual void Ping(const Contact &remote, VoidFunctorOneString callback);
        int AddContact(Contact new_contact, const float & rtt, const bool &only_db);
        void RemoveContact(const KadID &node_id);
        bool GetContact(const KadID &id, Contact *contact);
        bool FindValueLocal(const KadID &key, std::vector<std::string> *values);
        bool StoreValueLocal(const KadID &key, const std::string &value,
                             const boost::int32_t &ttl);
        bool RefreshValueLocal(const KadID &key, const std::string &value,
                               const boost::int32_t &ttl);
        bool DelValueLocal(const KadID &key, const SignedValue &value,
                           const SignedRequest &req);
        void GetRandomContacts(const size_t &count,
                               const std::vector<Contact> &exclude_contacts,
                               std::vector<Contact> *contacts);
        void HandleDeadRendezvousServer(const bool &dead_server);

        ConnectionType CheckContactLocalAddress(const KadID &id,
                                                const std::string &ip,
                                                const boost::uint16_t &port,
                                                const std::string &ext_ip);
        // void UpdatePDRTContactToRemote(const KadID &node_id,
        //                                const std::string &host_ip);
        ContactInfo contact_info() const;
        inline KadID node_id() const {

            return (type_ == CLIENT || type_ == CLIENT_PORT_MAPPED)
                ? fake_kClientId_ : node_id_;
        }
        boost::uint32_t KeyLastRefreshTime(const KadID &key,
                                           const std::string &value);
        boost::uint32_t KeyExpireTime(const KadID &key, const std::string &value);
        inline std::string host_ip() const {
            return host_ip_; }
        inline boost::uint16_t host_port() const {
            return host_port_; }
        inline std::string local_host_ip() const {
            return local_host_ip_; }
        inline boost::uint16_t local_host_port() const {
            return local_host_port_; }
        inline std::string rendezvous_ip() const {
            return rv_ip_; }
        inline boost::uint16_t rendezvous_port() const {
            return rv_port_; }
        inline bool is_joined() const {
            return is_joined_; }
        inline KadRpcs* kadrpcs() {
            return &kadrpcs_; }
        bool HasRSAKeys();

        boost::int32_t KeyValueTTL(const KadID &key, const std::string &value) const;

        // inline void set_alternative_store(base::AlternativeStore* alt_store) {

        //     alternative_store_ = alt_store;
        //     if (premote_service_.get() != NULL)
        //         premote_service_->set_alternative_store(alternative_store_);
        //}
        // inline base::AlternativeStore *alternative_store() {

        //     return alternative_store_;
        // }
        // inline void set_signature_validator(base::SignatureValidator *validator) {

        //     signature_validator_ = validator;
        //     if (premote_service_ != 0)
        //         premote_service_->set_signature_validator(signature_validator_);
        // }
        // inline NatType host_nat_type() {
        //     return host_nat_type_; }
        // inline bool recheck_nat_type() {
        //     return recheck_nat_type_; }
    private:

        // friend class test_knodeimpl::TestKNodeImpl_BEH_KNodeImpl_Destroy_Test;
        //   friend class
        //   test_knodeimpl::TestKNodeImpl_BEH_KNodeImpl_Bootstrap_Callback_Test;
        //   friend class
        //   test_knodeimpl::
        //   TestKNodeImpl_BEH_KNodeImpl_Join_Bootstrapping_Iteration_Test;
        // friend class test_knodeimpl::TestKNodeImpl_BEH_KNodeImpl_ExecuteRPCs_Test;
        // friend class test_knodeimpl::TestKNodeImpl_BEH_KNodeImpl_NotJoined_Test;

        KNodeImpl &operator=(const KNodeImpl&);
        KNodeImpl(const KNodeImpl&);
        inline void CallbackWithFailure(VoidFunctorOneString callback);
        void Bootstrap_Callback(const BootstrapResponse *response,
                                BootstrapData data);
        void Bootstrap(const std::string &bootstrap_ip,
                       const boost::uint16_t &bootstrap_port,
                       VoidFunctorOneString callback,
                       const bool &dir_connected);
        void Join_Bootstrapping_Iteration_Client(
            const std::string &result, boost::shared_ptr<struct BootstrapArgs> args,
            const std::string bootstrap_ip, const boost::uint16_t bootstrap_port,
            const std::string local_bs_ip, const boost::uint16_t local_bs_port);
        void Join_Bootstrapping_Iteration(
            const std::string &result, boost::shared_ptr<struct BootstrapArgs> args,
            const std::string bootstrap_ip, const boost::uint16_t bootstrap_port,
            const std::string local_bs_ip, const boost::uint16_t local_bs_port);
        void Join_Bootstrapping(VoidFunctorOneString callback,
                                std::vector<Contact> &cached_nodes,
                                const bool &got_external_address);
        void Join_RefreshNode(VoidFunctorOneString callback,
                              const bool &port_forwarded);
        void SaveBootstrapContacts();  // save the routing table into .kadconfig file
        boost::int16_t LoadBootstrapContacts();
        void RefreshRoutine();
        void StartSearchIteration(const KadID &key, const RemoteFindMethod &method,
                                  VoidFunctorOneString callback);
        void SearchIteration_ExtendShortList(const FindResponse *response,
                                             FindCallbackArgs callback_data);
        void SearchIteration(boost::shared_ptr<IterativeLookUpData> data);
        void FinalIteration(boost::shared_ptr<IterativeLookUpData> data);
        void SendDownlist(boost::shared_ptr<IterativeLookUpData> data);
        void SendFindRpc(Contact remote, boost::shared_ptr<IterativeLookUpData> data,
                         const ConnectionType &conn_type);
        void SearchIteration_CancelActiveProbe(
            Contact sender,
            boost::shared_ptr<IterativeLookUpData> data);

        void SendFinalIteration(boost::shared_ptr<IterativeLookUpData> data);
        void StoreValue_IterativeStoreValue(const StoreResponse *response,
                                            StoreCallbackArgs callback_data);
        void StoreValue_ExecuteStoreRPCs(const std::string &result, const KadID &key,
                                         const std::string &value,
                                         const SignedValue &sig_value,
                                         const SignedRequest &sig_req,
                                         const bool &publish,
                                         const boost::int32_t &ttl,
                                         VoidFunctorOneString callback);
        void DelValue_ExecuteDeleteRPCs(const std::string &result, const KadID &key,
                                        const SignedValue &value,
                                        const SignedRequest &sig_req,
                                        VoidFunctorOneString callback);
        void DelValue_IterativeDeleteValue(const DeleteResponse *response,
                                           DeleteCallbackArgs callback_data);
        void ExecuteUpdateRPCs(const std::string &result,
                               const KadID &key,
                               const SignedValue &old_value,
                               const SignedValue &new_value,
                               const SignedRequest &sig_req,
                               boost::uint32_t ttl,
                               VoidFunctorOneString callback);
        void UpdateValueResponses(boost::shared_ptr<UpdateCallbackArgs> uca);
        void FindNode_GetNode(const std::string &result, const KadID &node_id,
                              VoidFunctorOneString callback);
        void Ping_HandleResult(const PingResponse *response,
                               PingCallbackArgs callback_data);
        void Ping_SendPing(const std::string& result, VoidFunctorOneString callback);
        void ReBootstrapping_Callback(const std::string &result);
        void RegisterKadService();
        void UnRegisterKadService();
        //void UPnPMap(boost::uint16_t host_port);
        //void UnMapUPnP();
        void CheckToInsert(const Contact &new_contact);
        void CheckToInsert_Callback(const std::string &result, KadID id,
                                    Contact new_contact);

        void CheckAddContacts();
        void RefreshValuesRoutine();
        void RefreshValue(const KadID &key, const std::string &value,
                          const boost::int32_t &ttl, VoidFunctorOneString callback);
        void RefreshValueCallback(const std::string &result, const KadID &key,
                                  const std::string &value, const boost::int32_t &ttl,
                                  boost::shared_ptr<boost::uint32_t> refreshes_done,
                                  const boost::uint32_t &total_refreshes);
        //void RecheckNatRoutine();
        //void RecheckNatRoutineJoinCallback(const std::string &result);
        boost::mutex routingtable_mutex_, kadconfig_mutex_, extendshortlist_mutex_,
            joinbootstrapping_mutex_, leave_mutex_, activeprobes_mutex_,
            pendingcts_mutex_;
        //boost::shared_ptr<base::CallLaterTimer> ptimer_;
        //rpcprotocol::ChannelManager *pchannel_manager_;
        //transport::TransportHandler *transport_handler_;
        //boost::int16_t transport_id_;
        //boost::shared_ptr<rpcprotocol::Channel> pservice_channel_;
        boost::shared_ptr<DataStore> pdata_store_;
        //base::AlternativeStore *alternative_store_;
        boost::shared_ptr<KadService> premote_service_;
        KadRpcs kadrpcs_;
        //NatRpcs natrpcs_;
        volatile bool is_joined_;
        boost::shared_ptr<RoutingTable> prouting_table_;
        KadID node_id_, fake_kClientId_;
        std::string host_ip_;
        NodeType type_;
        boost::uint16_t host_port_;
        std::string rv_ip_;
        boost::uint16_t rv_port_;
        std::vector<Contact> bootstrapping_nodes_;
        const boost::uint16_t K_, alpha_, beta_;
        bool refresh_routine_started_;
        boost::filesystem::path kad_config_path_;
        std::string local_host_ip_;
        boost::uint16_t local_host_port_;
        bool stopping_, port_forwarded_, use_upnp_;
        std::list<Contact> contacts_to_add_;
        boost::shared_ptr<boost::thread> addcontacts_routine_;
        boost::condition_variable add_ctc_cond_;
        std::string private_key_, public_key_;

        //NatType host_nat_type_;
        //bool recheck_nat_type_;
        // for UPnP
        //upnp::UpnpIgdClient upnp_;
        //boost::uint16_t upnp_mapped_port_;
        //
        //base::SignatureValidator *signature_validator_;

        std::vector<Contact> exclude_bs_contacts_;

        boost::shared_ptr<apache::thrift::transport::TTransport> socket_;
        boost::shared_ptr<apache::thrift::transport::TTransport> transport_;
        boost::shared_ptr<apache::thrift::protocol::TProtocol> protocol_;

    };


}

#endif
