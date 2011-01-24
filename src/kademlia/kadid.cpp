/**
 * @file   kadid.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Sun Nov 28 15:52:18 2010
 *
 * @brief  Kademlia Algorithm construction kad ID implement.
 *
 *
 */

#include "kadid.h"
#include "common/TTimer.hh"
#include "common/sha1.h"

#ifdef OLD_SSTREAM
#include <strstream.h>
#else
#include <sstream>
#endif

namespace kad{
    // Default KadID construction
    KadID::KadID()
    {
        std::ostringstream raw;
        TTimer timer;
        long  tmp_time=timer.current();
        raw << tmp_time;
        SHA1 sha1;
        sha1.Reset();
        sha1 << raw.str().c_str();
        sha1.Result(raw_id);
    }

    KadID::KadID( std::string raw)
    {
        SHA1 sha1;
        sha1.Reset();
        sha1<<raw.c_str();
        sha1.Result(raw_id);
    }

    KadID::KadID ( BigInt::Rossi raw)
    {
        this->raw_id = raw;
    }

    KadID::KadID( const KadID & other )
    {
        if ( this->raw_id != other.raw_id )
        {
            this->raw_id = other.raw_id;
        }
        else
            return;
    }

    KadID::KadID( const KadID & min , const KadID & max)
    {
        BigInt::Rossi _min(min.raw_id), _max(max.raw_id);
        this->raw_id = ( _min + _max) >> 1;
    }

    const KadID KadID::KMaxID()
    {
        std::string raw(160,'1');
        return KadID(BigInt::Rossi(raw,10));
    }

    const KadID KadID::KMinID()
    {
        return KadID(BigInt::Rossi(0));
    }

    KadID & KadID::operator = ( const KadID & other )
    {
        this->raw_id = other.raw_id;
        return *this;
    }
    bool KadID::operator == ( const KadID & other )
    {
        return this->raw_id == other.raw_id;
    }

    bool KadID::operator < ( const KadID & other )
    {
        return this->raw_id < other.raw_id;
    }

    bool KadID::operator > ( const KadID & other )
    {
        return this->raw_id > other.raw_id;
    }

    bool KadID::operator <= ( const KadID & other )
    {
        return this->raw_id <= other.raw_id;
    }

    bool KadID::operator >= ( const KadID & other )
    {
        return this->raw_id >=  other.raw_id;
    }

    bool KadID::operator != ( const KadID & other )
    {
        return this->raw_id != other.raw_id;
    }

    std::ostream &operator<< ( std::ostream & os , const KadID & id)
    {
        os << id.raw_id;
        return os;
    }

    KadID KadID::operator ^ ( const KadID & other )
    {
        BigInt::Rossi ret;
        ret = this->raw_id ^ other.raw_id;
        return KadID(ret);
    }

    KadID operator ^ ( const KadID & ls, const KadID & rs)
    {
        BigInt::Rossi ret(ls.raw_id);
        ret = ret ^ rs.raw_id;
        return KadID(ret);
    }
    bool KadID::CloserToTarget( KadID id1, KadID id2,  const KadID & target )
    {
        KadID lh = id1 ^ target;
        KadID rh = id2 ^ target;
        return lh < rh;
    }

    bool operator < ( const KadID & ls , const KadID & rs)
    {
        return static_cast<bool> ( ls.raw_id < rs.raw_id);
    }

    bool operator > ( const KadID & ls , const KadID & rs)
    {
        return static_cast<bool> ( ls.raw_id > rs.raw_id);
    }

    bool operator <= ( const KadID & ls, const KadID & rs)
    {
        return static_cast<bool> ( !(ls > rs));
    }

    bool operator >= ( const KadID & ls, const KadID & rs )
    {
        return  static_cast<bool> ( !( ls<rs));
    }

    bool operator == ( const KadID & ls, const KadID & rs )
    {
        return static_cast<bool> ( ls.raw_id == rs.raw_id );
    }

    BigInt::Rossi KadID::getRawID()
    {
        return this->raw_id;
    }
/*
    KadID operator + ( const KadID &ls, const KadID & other )
    {
        KadID ret;
        ret.raw_id = ls.raw_id + other.raw_id;
        return ret;
    }
*/

    KadID KadID::operator + ( const KadID &other )
    {
        KadID ret;
        ret.raw_id = this->raw_id + other.raw_id ;
        return ret;
    }
    KadID KadID::operator - ( const KadID & rs )
    {
        KadID ret;
        ret.raw_id = this->raw_id - rs.raw_id;
        return ret;
    }

    KadID KadID::operator >> ( const int shift )
    {
        KadID ret;
        ret.raw_id = this->raw_id >> shift;
        return ret;
    }
    void KadID::SplitRange( const KadID &min, const KadID & max , KadID * min1, KadID * max1 )
    {
        assert( NULL != min1 && NULL != max1 );
        KadID min_(min),max_(max);
        * max1 = (min_ >> 1)+ (max_ >> 1);
        * min1 = ( *max1 ) + BigInt::Rossi(1);
        return ;
    }
}
