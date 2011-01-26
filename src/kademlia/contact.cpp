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

    // bool ContactInfo::has_attr(char *name )
    // {
    //     if ( NULL == name )
    //     {
    //         LOG <<" Checking NULL Attribute" << std::endl;

    //         return false;
    //     }

    //     TiXmlAttribute * pAttr= element->FirstAttribute();

    //     while( pAttr )
    //     {
    //         if ( 0 == strcmp(name , pAttr->Name() ))
    //         {
    //             if ( NULL == pAttr->Value() || 0 == pAttr->Value() )
    //             {
    //                 return false;
    //             }
    //             return true;
    //         }
    //         pAttr = pAttr->Next();
    //     }
    //     return false;
    // }

    // bool ContactInfo::has_attr( std::string name )
    // {
    //     return has_attr(name.c_str());
    // }

    // void  ContactInfo::set_host_ip(char * ip)
    // {
    //     try {
    //         if ( NULL != ip )
    //             element->SetAttribute("host_ip",ip);
    //         else
    //             LOG<<"NULL host IP"<<std::endl;
    //     }catch(...)
    //     {
    //         LOG<<"Set Host IP Fail !"<<std::endl;
    //     }
    // }

    // void ContactInfo::set_host_ip( std::string ip )
    // {
    //     set_host_ip( ip.c_str());
    // }

    // void ContactInfo::set_host_port( unsigned short port)
    // {
    //     try{
    //         element->SetAttribute("host_port",port);
    //     }catch(...)
    //     {
    //         LOG<<" Set Host Port Fail !"<<std::endl;
    //     }
    // }

    // void ContactInfo::set_local_ip( char *ip )
    // {
    //     if ( NULL == ip )
    //     {
    //         LOG<<" NULL Local IP !"<<std::endl;
    //     }
    //     try{
    //         element->SetAttribute( "local_ip" , ip );
    //     }catch(...)
    //     {
    //         LOG<<" Set Local IP Fail !"<<std::endl;
    //     }
    // }

    // void ContactInfo::set_local_ip( std::string ip )
    // {
    //     set_local_ip( ip.c_str());
    // }

    // void ContactInfo::set_local_port( unsigned short port )
    // {
    //     try{
    //         element->SetAttribute( "local_port", port);
    //     }catch(...)
    //     {
    //         LOG<<" Set Local Port Fail !"<<std::endl;
    //     }
    // }

    // void ContactInfo::set_rendezvous_ip ( char * ip )
    // {
    //     if ( NULL == ip )
    //     {
    //         LOG<<" NULL Rendezvous IP"<<std::endl;
    //     }
    //     try{
    //         element->SetAttribute( "rendezvous_ip" , ip );
    //     }catch(...)
    //     {
    //         LOG<<" Set Rendezvous IP Fail !"<<std::endl;
    //     }
    // }

    // void ContactInfo::set_rendezvous_ip ( std::string ip )
    // {
    //     set_rendezvous_ip( ip.c_str() );
    // }

    // void ContactInfo::set_rendezvous_port( unsigned short port )
    // {
    //     try{
    //         element->SetAttribute( "rendezvous_port", port);
    //     }catch(...)
    //     {
    //         LOG<<"Set Rendezvous Port Fail !"<<std::endl;
    //     }
    // }

    // void ContactInfo::set_id( char * id)
    // {
    //     if ( NULL == id)
    //     {
    //         LOG<<"NULL ID"<<std::endl;
    //     }
    //     try{
    //         element->SetAttribute( "id", id);
    //     }catch(...)
    //     {
    //         LOG<<"Set ID Fail !"<<std::endl;
    //     }
    // }

    // void ContactInfo::set_id( std::string ip)
    // {
    //     set_id(ip.c_str());
    // }

    // void ContactInfo::set_id ( KadID id )
    // {
    //     std::ostringstream os;
    //     os<<id;
    //     set_id( os.str());
    // }

    // void ContactInfo::set_id ( BigInt::Rossi id )
    // {
    //     std::ostringstream os;
    //     os<<id;
    //     set_id( os.str() );
    // }

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


    // Contact::Contact( const ContactInfo &contact_info)
    //     :        node_id_(BigInt::Rossi(0)),
    //              host_ip_(),host_port_(0),
    //              local_ip_(),local_port_(0),
    //              rendezvous_ip_(),rendezvous_port_(0)
    // {
    //     TiXmlAttribute *pAttr = contact_info.element->FirstAttribute();
    //     try
    //     {
    //         while ( pAttr )
    //         {
    //             if ( 0 == strcmp( pAttr->Name(), "id" ) ){
    //                 node_id_ = BigInt::Rossi( std::string( pAttr->Value() ) ,10 );
    //             }
    //             else if ( 0 == strcmp ( pAttr->Name() , "host_ip")){
    //                 host_ip_ = pAttr->Value();
    //             }
    //             else if ( 0 == strcmp ( pAttr->Name() , "host_port")){
    //                 if ( TIXML_SUCCESS != pAttr->QueryIntValue( (int * ) &host_port_))
    //                     throw std::exception();
    //             }
    //             else if ( 0 == strcmp ( pAttr->Name() , "local_ip")){
    //                 local_ip_ = pAttr->Value();
    //             }
    //             else if ( 0 == strcmp ( pAttr->Name() , "local_port")){
    //                 if ( TIXML_SUCCESS  != pAttr->QueryIntValue((int *)&local_port_))
    //                     throw std::exception();
    //             }
    //             else if ( 0 == strcmp ( pAttr->Name() , "rendezvous_ip")) {
    //                 rendezvous_ip_ = pAttr->Value();
    //             }
    //             else if ( 0 == strcmp ( pAttr->Name() , "rendezvous_port") ) {
    //                 if ( TIXML_SUCCESS != pAttr->QueryIntValue((int *) &rendezvous_port_))
    //                     throw std::exception();
    //             }
    //             else
    //             {
    //                 LOG<<"Unknown Type of Attribute"<<std::endl;
    //                 throw std::exception();
    //             }
    //             pAttr = pAttr->Next();
    //         }
    //     }catch(...)
    //     {
    //         LOG<<" contact_info translated to Contact Error!"<<std::endl;
    //     }
    // }

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

    // bool Contact::SerialiseToXmlElement(ContactInfo *serialised_output)
    // {
    //     if ( NULL == serialised_output )
    //     {
    //         LOG<<"NULL ContactInfo"<<std::endl;
    //         return false;
    //     }

    //     serialised_output->set_id(node_id_);

    //     if ( NULL != host_ip_.c_str())
    //         serialised_output->set_host_ip(host_ip_);
    //     if ( 0 != host_port_ )
    //         serialised_output->set_host_port(host_port_);
    //     if ( NULL != local_ip_.c_str())
    //         serialised_output->set_local_ip( local_ip_);
    //     if ( 0 != local_port_ )
    //         serialised_output->set_local_port( local_port_);
    //     if ( NULL != rendezvous_ip_.c_str() )
    //         serialised_output->set_rendezvous_ip(rendezvous_ip_);
    //     if ( 0 != rendezvous_port_ )
    //         serialised_output->set_rendezvous_port(rendezvous_port_);

    //     return true;
    // }

    // bool Contact::ParseFromXmlElement(ContactInfo &data)
    // {
    //     TiXmlAttribute * pAttr = data.element->FirstAttribute();

    //     while ( pAttr )
    //     {
    //         if ( 0 == strcmp( pAttr->Name(), "id"))
    //         {
    //             node_id_  = KadID(BigInt::Rossi( std::string(pAttr->Value()), 10 ));
    //         }
    //         else if ( 0 == strcmp ( pAttr->Name(), "host_ip"))
    //         {
    //             host_ip_ = pAttr->Value();
    //         }
    //         else if ( 0 == strcmp ( pAttr->Name(), "host_port"))
    //         {
    //             if ( TIXML_SUCCESS != pAttr->QueryIntValue( (int *)&host_port_))
    //                 throw std::exception();
    //         }
    //         else if ( 0 == strcmp ( pAttr->Name(), "local_ip"))
    //         {
    //             local_ip_ = pAttr->Value();
    //         }
    //         else if ( 0 == strcmp( pAttr->Name(), "local_port"))
    //         {
    //             if ( TIXML_SUCCESS != pAttr->QueryIntValue((int*) & local_port_))
    //                 throw std::exception();
    //         }
    //         else if ( 0 == strcmp ( pAttr->Name(), "rendezvous_ip"))
    //         {
    //             rendezvous_ip_ = pAttr->Value();
    //         }
    //         else if ( 0 == strcmp( pAttr->Name(),"rendezvous_port"))
    //         {
    //             if ( TIXML_SUCCESS != pAttr->QueryIntValue((int*)&rendezvous_port_))
    //                 throw std::exception();
    //         }
    //         else
    //         {
    //             LOG<<"Unknown Attribute!"<<std::endl;
    //             throw std::exception();
    //         }
    //         pAttr = pAttr->Next();
    //     }
    //     return true;
    // }

}

