/**
 * @file   kadrpc.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Fri Jan 28 11:50:44 2011
 *
 * @brief
 *
 *
 */

#ifndef KADRPC_H
#define KADRPC_H
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <stdint.h>
#include "kademlia_service_types.h"
#include "KademliaService.h"
#include <Thrift.h>
#include <transport/TTransportUtils.h>
#include <transport/TSocket.h>
#include <boost/shared_ptr.hpp>
#include <protocol/TProtocol.h>
#include <protocol/TBinaryProtocol.h>
#include <stdint.h>
#include "contact.h"
#include "signed_kadvalue_types.h"
namespace kad{

const uint32_t kRpcPingTimeout = 3;  // 3secs
const uint32_t kRpcBootstrapTimeout = 7;  // 7secs

    class KadRpcs{
    public:
        KadRpcs(boost::shared_ptr<apache::thrift::transport::TTransport> socket);
        void set_info(const ContactInfo & info);

        void FindNode(const std::string &key, const std::string &ip,
                      const uint16_t &port, const std::string & rv_ip,
                      const uint16_t &rv_port,FindResponse *resp);

        void FindValue(const std::string &key, const std::string &ip,
                       const uint16_t &port, const std::string & rv_ip,
                       const uint16_t & rv_port, FindResponse * resp);

        void Ping ( const std::string & ip,
                    const uint16_t & port, const std::string & rv_ip,
                    const uint16_t & rv_port, PingResponse * resp);

        void Store( const std::string & key,const std::string & value,
                    const std::string & ip, const uint16_t & port,
                    const std::string & rv_ip, const uint16_t & rv_port,
                    StoreResponse * resp, const uint32_t & ttl, bool publish);

        void Store ( const std::string & key,const SignedValue & value,
                     const std::string &ip, const uint16_t & port,
                     const std::string &rv_ip, const uint16_t & rv_port,
                     StoreResponse * resp, const uint32_t & ttl, bool publish);
        void Downlist( const std::vector<std::string> downlist,
                       const std::string & ip, const uint16_t & port,
                       const std::string &rv_ip, const uint16_t & rv_port,
                       DownlistResponse *resp);

        void Bootstrap(const std::string & local_id,
                       const std::string & local_ip, const uint16_t & local_port,
                       const std::string & remote_ip, const uint16_t &remote_port,
                       BootstrapResponse *resp);

        void Delete ( const std::string & key, const SignedValue & value,
                      const SignedRequest & sig_req, const std::string ip,
                      const uint16_t & port, const std::string & rv_ip,
                      const uint16_t & rv_port, DeleteResponse * resp);
    private:
        KadRpcs(const KadRpcs&);
        KadRpcs & operator=(const KadRpcs&);
        ContactInfo info_;
        boost::shared_ptr<apache::thrift::transport::TTransport> socket_;
        boost::shared_ptr<apache::thrift::transport::TTransport> transport_;
        boost::shared_ptr<apache::thrift::protocol::TProtocol> protocol_;
    };

}

#endif
