/**
 * @file   kadservice.cc
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Fri Jan 28 23:54:54 2011
 *
 * @brief
 *
 *
 */

#include "kadservice.h"
#include "kadrpc.h"
#include <boost/compressed_pair.hpp>
#include "datastore.h"
#include "contact.h"
#include "kademlia_service_types.h"
#include "KademliaService.h"
#include "kademlia_config.h"
#include "crypto.h"
#include "blah.hpp"
#include <cryptopp/cryptlib.h>

namespace kad
{

    static void downlist_ping_cb(const std::string&) {}

    KadService::KadService( /*const NatRpcs &nat_rpcs,*/
        boost::shared_ptr<DataStore> datastore,
        const bool &hasRSAkeys,
        add_contact_function add_cts,
        get_random_contacts_function rand_cts,
        get_contact_function get_ctc,
        get_closestK_function get_kcts,
        ping_function ping) :
        /*nat_rpcs_(nat_rpcs),*/
        pdatastore_(datastore),
        node_joined_(false),
        node_hasRSAkeys_(hasRSAkeys),
        node_info_(),
        /*alternative_store_(NULL),*/
        add_contact_(add_cts),
        get_random_contacts_(rand_cts),
        get_contact_(get_ctc),
        get_closestK_contacts_(get_kcts),
        ping_(ping)
        /*signature_validator_(NULL)*/ {
    }


    KadService::~KadService(){}

    void KadService::Ping(kad::PingResponse& _return, const kad::PingRequest& request)
    {
        if ( ! node_joined_)
        {
            _return.result = kRpcResultFailure;
            return ;
        }
        Contact sender;
        if ( request.ping == "ping" && GetSender(request.sender_info, &sender))
        {
            _return.echo = "pong";
            _return.result = kRpcResultSuccess;
            add_contact_ (sender,false);
        }
        else{
            _return.result = kRpcResultFailure;
        }
        _return.node_id = node_info_.node_id;
    }

    void KadService::FindValue(kad::FindResponse& _return, const kad::FindRequest& request)
    {

        if (!node_joined_) {
            _return.result =kRpcResultFailure;
            return;
        }
        Contact sender;
        if (GetSender(request.sender_info, &sender))
        {
            std::string key = request.key;
            std::vector<std::string> value_str;

            if (pdatastore_->LoadItem(key,&value_str))
            {

                if( node_hasRSAkeys_)
                {
                    for ( int i =0 ; i < value_str.size() ; i++ )
                    {
                        //SignedValue signed_value;
                        _return.values.push_back(value_str[i]);
                    }
                }
                else{
                    for (int i=0 ; i < value_str.size(); i++)
                        _return.values.push_back(value_str[i]);
                }
                _return.result = kRpcResultSuccess;
                add_contact_(sender,false);
            }
            else{
                FindNode(_return,request);
            }

        }
        else
        {
            _return.result= kRpcResultFailure;
        }
        _return.node_id = node_info_.node_id;
    }

    void KadService::FindNode(kad::FindResponse& _return, const kad::FindRequest& request)
    {
        if ( ! node_joined_ )
        {
            _return.result = kRpcResultFailure;
            return ;
        }
        Contact sender;

        if ( GetSender(request.sender_info,&sender) )
        {
            std::vector<Contact> closest_contacts, exclude_contacts;
            kad::KadID key(request.key);

            exclude_contacts.push_back(sender);
            get_closestK_contacts_(key, &closest_contacts, exclude_contacts);

            bool found_node(false);

            for (unsigned int i = 0; i < closest_contacts.size(); ++i) {
                _return.closest_nodes.push_back(closest_contacts[i].ParseToThrift());
                if (key == closest_contacts[i].node_id())
                    found_node = true;
            }

            if (!found_node) {
                Contact key_node;
                if (get_contact_(key, &key_node)) {
                    _return.closest_nodes.push_back(key_node.ParseToThrift());
                }
            }
            _return.result = kRpcResultSuccess;
            add_contact_(sender,false);
        }
        else{
            _return.result = kRpcResultFailure;
        }
    }


    void KadService::Store(kad::StoreResponse& _return, const kad::StoreRequest& request)
    {
        if ( ! node_joined_ )
        {
            _return.result = kRpcResultFailure;
            return;
        }
        Contact sender;

        if ( !CheckStoreRequest(&request, &sender))
        {
            _return.result = kRpcResultFailure;
        }
        //else if (node_hasRSAkeys_){
        //    StoreValueLocal
        //}
        else
        {
            StoreValueLocal(request.key, request.sig_value, sender,
                            request.ttl, request.publish, &_return);

        }
    }


    void KadService::Downlist(kad::DownlistResponse& _return,
                              const kad::DownlistRequest& request)
    {
        if ( ! node_joined_)
        {
            _return.result = kRpcResultFailure;
            return;
        }
        Contact sender;
        if ( GetSender( request.sender_info, & sender))
        {
            for ( int i = 0 ; i < request.downlist.size(); i++ )
            {
                Contact dead_node;
                if ( ! dead_node.ParseFromThrift(request.downlist[i]))
                    continue;
                Contact contact_to_ping;
                _return.result = kRpcResultSuccess;

                if (get_contact_(dead_node.node_id(),&contact_to_ping))
                {
                    ping_( dead_node,boost::bind(&downlist_ping_cb, _1));
                }
            }
            add_contact_(sender,false);
        }
        else
        {
            _return.result = kRpcResultFailure;
        }
    }

    void KadService::Bootstrap(kad::BootstrapResponse& _return,
                               const kad::BootstrapRequest& request)
    {
        if( !node_joined_ )
        {
            _return.result = kRpcResultFailure;
            return;
        }
        if (static_cast<NodeType>(request.node_type == CLIENT)) {
            _return.bootstrap_id = node_info_.node_id;
            _return.newcomer_ext_ip = request.newcomer_ext_ip;
            _return.newcomer_ext_port = request.newcomer_ext_port;
            _return.result = kRpcResultSuccess ;
            return;
        }

        /*
        Contact newcomer;
        */
    }

    void KadService::Delete(kad::DeleteResponse& _return, const kad::DeleteRequest& request)
    {

        if ( ! node_joined_ /*||  ! node_hasRSAkeys_*/ )
        {
            _return.result = kRpcResultFailure;
            return;
        }

        _return.node_id = node_info_.node_id;
        std::vector<std::string> values_str;
        if (!pdatastore_->LoadItem(request.key, &values_str)) {
            _return.result = kRpcResultFailure;
            return;
        }

        Contact sender;
        if (pdatastore_->MarkForDeletion(request.key,
                                         request.value.value+request.value.value_signature,
                                         request.signed_request.signer_id + \
                                         request.signed_request.public_key +\
                                         request.signed_request.signed_public_key +\
                                         request.signed_request.signed_request) &&
            GetSender(request.sender_info, &sender))
        {
            add_contact_(sender,false);
            _return.result = kRpcResultSuccess;
            return;
        }

        _return.result = kRpcResultFailure;

    }


    void KadService::Update(kad::UpdateResponse& _return, const kad::UpdateRequest& request)
    {
        _return.node_id = node_info_.node_id;
        _return.result = kRpcResultFailure;

        if ( !node_joined_ /* || ! node_hasRSAkeys_*/)
        {
            return;
        }
        std::vector<std::string> values_str;
        if (!pdatastore_->LoadItem(request.key, &values_str)) {
            LOG  << "KadService::Update - Didn't find key" << std::endl;
            return;
        }

        bool found(false);
        std::string ser_sv(request.old_value.value + request.old_value.value_signature);
        for (size_t n = 0; n < values_str.size() && !found; ++n) {
            if (ser_sv == values_str[n]) {
                found = true;
            }
        }
        if( !found)
        {
            LOG <<"KadService::Update - Didn't find value" << std::endl;
            return ;
        }

        crypto::Crypto cobj;
        if (!cobj.AsymCheckSig(request.new_value.value,
                               request.new_value.value_signature,
                               request.request.public_key,
                               crypto::STRING_STRING)) {

            LOG << "KadService::Update - New value doesn't validate" <<std::endl;
            return;
        }


        /**
         *  ToDO , check if old value is validate
         *
         */

        bool new_hashable(request.key ==
                          cobj.Hash(request.new_value.value +\
                                    request.new_value.value_signature,
                                    "", crypto::STRING_STRING, false));
        Contact sender;
        if (!pdatastore_->UpdateItem(request.key,
                                     request.old_value.value+\
                                     request.old_value.value_signature,
                                     request.new_value.value+\
                                     request.new_value.value_signature,
                                     request.ttl, new_hashable)) {

            LOG << "KadService::Update - Failed UpdateItem" << std::endl;
            return;
        }

        if ( GetSender( request.sender_info, & sender))
        {
            add_contact_(sender,false);
            _return.result = kRpcResultSuccess;
        }
        else
        {
            LOG<< " KadService::Update - Failed to add_contact_ " << std::endl;
        }
    }


    bool KadService::GetSender(const ContactInfo &sender_info, Contact *sender) {

        if ( sender->ParseFromThrift(sender_info) && NULL != sender )
            return true;
        else
            return false;
    }

    bool KadService::CheckStoreRequest(const StoreRequest *request, Contact *sender)
    {
        if ( node_hasRSAkeys_ )
        {
            if (!request->__isset.signed_request || ! request->__isset.sig_value)
                return false;
        }
        else{
            if ( !request->__isset.value )
                return false;
        }
        return GetSender(request->sender_info,sender);
    }

    void KadService::StoreValueLocal(const std::string &key,
                                     const std::string &value,
                                     Contact sender,
                                     const boost::int32_t &ttl,
                                     const bool &publish,
                                     StoreResponse *response)
    {
        bool result;
        if (publish) {
            result = pdatastore_->StoreItem(key, value, ttl, false);
        }
        else {
            std::string ser_del_request;
            result = pdatastore_->RefreshItem(key, value, &ser_del_request);
            if (!result && ser_del_request.empty()) {

                result = pdatastore_->StoreItem(key, value, ttl, false);
            } else if (!result && !ser_del_request.empty()) {

                SignedRequest req = response->signed_request;
                //req->ParseFromString(ser_del_request);
            }
        }

        if ( result)
        {
            response->result = kRpcResultSuccess;
            add_contact_(sender,false);
        }
        else
        {
            response->result = kRpcResultFailure;
        }
    }


    void KadService::StoreValueLocal(const std::string &key,
                                     const SignedValue &value,
                                     Contact sender,
                                     const boost::int32_t &ttl,
                                     const bool &publish,
                                     StoreResponse *response)
    {

        bool result, hashable;
        std::string ser_value = value.value + value.value_signature;
        if (publish) {

            if (CanStoreSignedValueHashable(key, ser_value, &hashable))
            {
                result = pdatastore_->StoreItem(key, ser_value, ttl, hashable);
                if ( ! result)
                {
                    LOG<<"Store Item Failed !"<<std::endl;
                }
            }
            else {
                LOG << "CanStoreSignedValueHashable Failed!" << std::endl;\
                result = false;
            }
        }
        else{
            std::string ser_del_request;
            result = pdatastore_->RefreshItem(key, ser_value, &ser_del_request);
            if (!result && CanStoreSignedValueHashable(key, ser_value, &hashable) &&
                ser_del_request.empty()) {
                result = pdatastore_->StoreItem(key, ser_value, ttl, hashable);
            } else if (!result && !ser_del_request.empty()) {
                //SignedRequest *req = response->mutable_signed_request();
                //req->ParseFromString(ser_del_request);
                LOG << "Weird Failed." << std::endl;
            }else if( !result)
            {
                LOG << "RefreshItem Failed !"<<std::endl;
            }
        }
        if ( result)
        {
            response->result = kRpcResultSuccess;
            add_contact_(sender,false);
        }
        else
        {
            response->result = kRpcResultFailure;
        }

    }

    bool KadService::CanStoreSignedValueHashable(const std::string &key,
                                                 const std::string &value, bool *hashable)
    {
        std::vector< std::pair<std::string, bool> > attr;
        attr = pdatastore_->LoadKeyAppendableAttr(key);
        *hashable = false;
        if (attr.empty()) {

            crypto::Crypto cobj;
            cobj.set_hash_algorithm(crypto::SHA_512);
            if (key == cobj.Hash(value, "", crypto::STRING_STRING, false))
                *hashable = true;
        } else if (attr.size() == 1) {
            *hashable = attr[0].second;
            if (*hashable && value != attr[0].first)
                return false;
        }
        return true;
    }

}
