#ifndef UDTServerSocket_class
#define UDTServerSocket_class

#include "UDTSocket.h"

class UDTServerSocket : private UDTSocket
{
public:

    UDTServerSocket ( int port );
    UDTServerSocket (){};

    virtual ~UDTServerSocket();

    const UDTServerSocket& operator << ( const std::string& ) const;
    const UDTServerSocket& operator >> ( std::string& ) const;
    void accept ( UDTServerSocket& );
};
#endif
