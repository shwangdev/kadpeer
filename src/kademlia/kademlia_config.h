/**
 * @file   kademlia_config.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Tue Dec  7 15:29:58 2010
 *
 * @brief
 *
 *
 */

#ifndef __KADEMLIA_CONFIG_H_
#define __KADEMLIA_CONFIG_H_

#include<boost/function.hpp>
namespace kad
{
    const int KRefreshTime = 3600;
    typedef boost::function<void(const std::string&)> VoidFunctorOneString;
    const std::string kRpcResultFailure("F");
    const std::string kRpcResultSuccess("S");
}

#endif
