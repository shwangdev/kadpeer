/**
 * @file   contact.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Dec  1 17:04:07 2010
 *
 * @brief  Kademlia Algorithm construction contact
 *
 *
 */

#include "contact.h"
#include "blah.hpp"
#include <exception>
#include <sstream>

#include "contact_info_types.h"

namespace kad{

    Contact::Contact():node_id_(BigInt::Rossi(0)),
                       host_ip_(),host_port_(0),
                       local_ip_(),local_port_(0),
                       rendezvous_ip_(),rendezvous_port_(0){}


    Contact::Contact(const BigInt::Rossi &node_id, const std::string &host_ip,
                     const unsigned short int &host_port, const std::string &local_ip,
                     const unsigned short int &local_port, const std::string &rendezvous_ip,
                     const unsigned short int &rendezvous_port):node_id_(node_id),
                                                                host_ip_(host_ip),host_port_(host_port),
                                                                local_ip_(local_ip),local_port_(local_port),
                                                                rendezvous_ip_(rendezvous_ip),rendezvous_port_(rendezvous_port){}

    Contact::Contact(const BigInt::Rossi &node_id, const std::string &host_ip,
                     const unsigned short int &host_port):node_id_(node_id),
                                                          host_ip_(host_ip),host_port_(host_port),
                                                          local_ip_(),local_port_(0),
                                                          rendezvous_ip_(),rendezvous_port_(){}

    Contact::Contact(const BigInt::Rossi &node_id, const std::string &host_ip,
                     const unsigned short int &host_port, const std::string &local_ip,
                     const unsigned short int &local_port):node_id_(node_id),
                                                           host_ip_(host_ip),host_port_(host_port),
                                                           local_ip_(local_ip),local_port_(local_port),
                                                           rendezvous_ip_(),rendezvous_port_(){}

    Contact::Contact(const KadID &node_id, const std::string &host_ip,
                     const unsigned short int &host_port, const std::string &local_ip,
                     const unsigned short int &local_port, const std::string &rendezvous_ip,
                     const unsigned short int &rendezvous_port):node_id_(node_id),
                                                                host_ip_(host_ip),host_port_(host_port),
                                                                local_ip_(local_ip),local_port_(local_port),
                                                                rendezvous_ip_(rendezvous_ip),rendezvous_port_(rendezvous_port){}

    Contact::Contact(const KadID &node_id, const std::string &host_ip,
                     const unsigned short int &host_port):node_id_(node_id),
                                                          host_ip_(host_ip),host_port_(host_port),
                                                          local_ip_(),local_port_(0),
                                                          rendezvous_ip_(),rendezvous_port_(0){}

    Contact::Contact(const KadID &node_id, const std::string &host_ip,
                     const unsigned short int &host_port, const std::string &local_ip,
                     const unsigned short int &local_port):node_id_(node_id),
                                                           host_ip_(host_ip),host_port_(host_port),
                                                           local_ip_(local_ip),local_port_(local_port),
                                                           rendezvous_ip_(),rendezvous_port_(0){}


    Contact::Contact( const ContactInfo &contact_info)
        :        node_id_(contact_info.node_id),
                 host_ip_(contact_info.ip),host_port_(contact_info.port),
                 local_ip_(contact_info.local_ip),local_port_(contact_info.local_port),
                 rendezvous_ip_(contact_info.rendezvous_ip),rendezvous_port_(contact_info.rendezvous_port)
    {
    }

    std::string Contact::DebugString() const
    {
        std::ostringstream os;
        os<<"Serialise id   :"<<node_id_<<std::endl
          <<"host_ip        :"<<host_ip_<<std::endl
          <<"host port      :"<<host_port_<<std::endl
          <<"local ip       :"<<local_ip_<<std::endl
          <<"local port     :"<<local_port_<<std::endl
          <<"rendezvous ip  :"<<rendezvous_ip_<<std::endl
          <<"rendezvous port:"<<rendezvous_port_<<std::endl;
        return os.str();
    }

}

