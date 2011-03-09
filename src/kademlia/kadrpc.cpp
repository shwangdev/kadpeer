/**
 * @file   kadrpc.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Fri Jan 28 11:51:06 2011
 *
 * @brief
 *
 *
 */

#include "kadrpc.h"
#include "blah.hpp"
//#include ""
namespace kad
{
    KadRpcs::KadRpcs(boost::shared_ptr<apache::thrift::transport::TTransport> socket):
        info_()
    {
        socket_ = socket;
        transport_ = boost::shared_ptr<apache::thrift::transport::TTransport>( new apache::thrift::transport::TBufferedTransport(socket_));
        protocol_  = boost::shared_ptr<apache::thrift::protocol::TProtocol>( new apache::thrift::protocol::TBinaryProtocol(transport_));
    }

    void KadRpcs::set_info(const ContactInfo & info)
    {
        info_= info;
    }

    void KadRpcs::FindNode(const std::string & key, const std::string &ip,
                           const uint16_t &port, const std::string & rv_ip,
                           const uint16_t &rv_port,FindResponse * resp)
    {
        KademliaServiceClient client(protocol_);
        FindRequest args;
        try{
            transport_->open();

            if ( resp->requester_ext_addr != "" )
            {
                args.is_boostrap=true;
            }
            args.key = key;
            //ContactInfo *sender_info = &args.sender_info;
            args.sender_info = info_;
            client.FindNode(*resp, args);
            transport_->close();
        }catch(apache::thrift::TException & ex)
        {
            LOG<<"FindNode Error"<<std::endl;
        }
    }

    void KadRpcs::FindValue( const std::string &key, const std::string &ip,
                             const uint16_t & port, const std::string & rv_ip,
                             const uint16_t & rv_port, FindResponse * resp)
    {
        FindRequest args;
        KademliaServiceClient client(protocol_);
        try{
            transport_->open();
            args.key = key;
            args.sender_info = info_;
            client.FindValue(*resp, args);
            transport_->close();
        }catch(apache::thrift::TException & ex)
        {
            LOG<<"FindValue Error"<<std::endl;
        }
    }

    void KadRpcs::Ping ( const std::string &ip,
                         const uint16_t & port,
                         const std::string & rv_ip,
                         const uint16_t & rv_port,
                         PingResponse * resp)
    {
        PingRequest args;
        KademliaServiceClient client( protocol_);
        try{
            transport_->open();
            args.sender_info= info_;
            args.ping = "ping";
            client.Ping(*resp, args);
            transport_->close();
        }catch(apache::thrift::TException & ex)
        {
            LOG<<"Ping Error"<<std::endl;
        }
    }


    void KadRpcs::Store( const std::string & key, const std::string &value,
                         const std::string & ip, const uint16_t & port,
                         const std::string & rv_ip, const uint16_t & rv_port,
                         StoreResponse * resp, const uint32_t & ttl, bool publish)
    {
        KademliaServiceClient client( protocol_);
        StoreRequest args;
        try{
            transport_->open();
            args.key=key;
            args.value = value;
            args.ttl = ttl;
            args.publish = publish;
            args.sender_info = info_;
            client.Store(*resp, args);
            transport_->close();
        }catch(apache::thrift::TException & ex)
        {
            LOG<<"Error Store"<<std::endl;
        }
    }


    void KadRpcs::Store ( const std::string & key,const SignedValue & value,
                     const std::string &ip, const uint16_t & port,
                 const std::string &rv_ip, const uint16_t & rv_port,
                 StoreResponse * resp, const uint32_t & ttl, bool publish)
    {
        KademliaServiceClient client( protocol_ );
        StoreRequest args;
        try{
            transport_->open();
            args.key = key;
            args.value = value.value;
            args.ttl = ttl;
            args.publish = publish;
            args.sender_info = info_;
            client.Store(*resp, args);
            transport_->close();
        }catch(apache::thrift::TException & ex)
        {
            LOG<<"Store Error"<<std::endl;
        }
    }


    void KadRpcs::Downlist( const std::vector<kad::ContactInfo> downlist,
                   const std::string & ip, const uint16_t & port,
                   const std::string &rv_ip, const uint16_t & rv_port,
                   DownlistResponse *resp)
    {
        KademliaServiceClient client( protocol_);
        DownlistRequest args;
        try{
            transport_->open();
            for ( int i =0 ; i  < downlist.size() ; i ++ )
                args.downlist.push_back(downlist[i]);
            args.sender_info = info_;
            client.Downlist(*resp,args);
            transport_->close();
        }catch(apache::thrift::TException & ex)
        {
            LOG<<"Downlist Error"<<std::endl;
        }
    }



    void KadRpcs::Bootstrap(const std::string & local_id,
                            const std::string & local_ip, const uint16_t & local_port,
                            const std::string & remote_ip, const uint16_t &remote_port,
                            BootstrapResponse *resp)
    {
        KademliaServiceClient client( protocol_);
        BootstrapRequest args;
        try{
            transport_->open();
            args.newcomer_id = local_id;
            args.newcomer_local_ip = local_ip;
            args.newcomer_local_port = local_port;
            client.Bootstrap(*resp, args);
            transport_->close();
        }catch(apache::thrift::TException & ex)
        {
            LOG<<"Bootstrap Error"<<std::endl;
        }
    }


    void KadRpcs::Delete ( const std::string & key, const SignedValue & value,
                  const SignedRequest & sig_req, const std::string ip,
                  const uint16_t & port, const std::string & rv_ip,
                  const uint16_t & rv_port, DeleteResponse * resp)
    {
        KademliaServiceClient client( protocol_);
        DeleteRequest args;
        try{
            transport_->open();
            args.key = key;
            args.value = value;
            args.sender_info = info_;
            args.signed_request = sig_req;
            client.Delete( * resp, args);
            transport_->close();
        }catch( apache::thrift::TException &ex)
        {
            LOG<<"Delete Error"<<std::endl;
        }
    }


}
