#ifndef UDTClientSocket_class
#define UDTClientSocket_class

#include "UDTSocket.h"

class UDTClientSocket : private UDTSocket
{
public:

    UDTClientSocket ( std::string host, int port );
    virtual ~UDTClientSocket(){};

    const UDTClientSocket& operator << ( const std::string& ) const;
    const UDTClientSocket& operator >> ( std::string& ) const;
};

#endif
