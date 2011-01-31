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
#include <thrift.h>
#include "kademlia_service_types.h"
#include "KademliaService.h"
#include <thrift.h>
#include <transport/TTransportUtils.h>
#include <transport/TSocket.h>
#include <boost/shard_ptr.hpp>
#include <stdint.h>
namespace kad{

const uint32_t kRpcPingTimeout = 3;  // 3secs
const uint32_t kRpcBootstrapTimeout = 7;  // 7secs

    class KadRpcs{
    public:
        KadRpcs();
    private:
        KadRpcs(const KadRpcs&);
        KadRpcs & operator=(const KadRPC&);
        ContactInfo info_;
    };

}

#endif
