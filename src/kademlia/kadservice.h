#ifndef KADSERVICE_H
#define KADSERVICE_H

#include <memory>
#include <string>
#include <vector>
#include "contact.h"
#include "KademliaService.h"
#include "kademlia_service_messages_types.h"
#include "signed_kadvalue_types.h"
#include "kademlia_config.h"

namespace kad{

    class DataStore;
    class Contact;
    class KadId;
    typedef boost::function< int(Contact, const float&, const bool&) > add_contact_function;
    typedef boost::function< void(const int&, const std::vector<Contact>&,std::vector<Contact>*) > get_random_contacts_function;
    typedef boost::function< bool(const std::string&, Contact*) > get_contact_function;

    typedef boost::function< void(const std::string&, std::vector<Contact>*, const std::vector<Contact>&) > get_closestK_function;
    typedef boost::function< void(const Contact&, VoidFunctorOneString) > ping_function;
    class KadService : public KademliaServiceIf
    {

    public:
        KadService();
        ~KadService ();
        void Ping(kad::PingResponse& _return, const kad::PingRequest& request) ;
        void FindValue(kad::FindResponse& _return, const kad::FindRequest& request) ;
        void FindNode(kad::FindResponse& _return, const kad::FindRequest& request) ;
        void Store(kad::StoreResponse& _return, const kad::StoreRequest& request) ;
        void Downlist(kad::DownlistResponse& _return, const kad::DownlistRequest& request) ;
        void Bootstrap(kad::BootstrapResponse& _return, const kad::BootstrapRequest& request) ;
        void Delete(kad::DeleteResponse& _return, const kad::DeleteRequest& request) ;
        void Update(kad::UpdateResponse& _return, const kad::UpdateRequest& request) ;
        inline void set_node_joined(const bool &joined) {
            node_joined_ = joined;
        }
        inline void set_node_info(const ContactInfo &info) {
            node_info_ = info;
        }

        // inline void set_alternative_store(base::AlternativeStore* alt_store) {
        //     alternative_store_ = alt_store;
        // }
        // inline void set_signature_validator(base::SignatureValidator *sig_validator) {
        //     signature_validator_ = sig_validator;
        // }

    private:
        bool GetSender(const ContactInfo &sender_info, Contact *sender);
        void StoreValueLocal(const std::string &key,
                             const std::string &value, Contact sender, const boost::int32_t &ttl,
                             const bool &publish, StoreResponse *response,
                             rpcprotocol::Controller *ctrl);
        void StoreValueLocal(const std::string &key,
                             const SignedValue &value, Contact sender, const boost::int32_t &ttl,
                             const bool &publish, StoreResponse *response,
                             rpcprotocol::Controller *ctrl);
        bool CanStoreSignedValueHashable(const std::string &key,
                                         const std::string &value, bool *hashable);
        NatRpcs nat_rpcs_;
        boost::shared_ptr<DataStore> pdatastore_;
        bool node_joined_, node_hasRSAkeys_;
        ContactInfo node_info_;
        //base::AlternativeStore *alternative_store_;
        add_contact_function add_contact_;
        get_random_contacts_function get_random_contacts_;
        get_contact_function get_contact_;
        get_closestK_function get_closestK_contacts_;
        ping_function ping_;
        //base::SignatureValidator *signature_validator_;
        /**
         *  Disable copy construction of KadService
         */
        KadService(const KadService&);
        KadService& operator=(const KadService&);
    };
}
#endif
