/**
 * @file   kadid.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Thu Nov 25 14:56:33 2010
 *
 * @brief  Kademlia Algorithm construction Kad ID
 *
 *
 */

#ifndef __KADID_H__
#define __KADID_H__

#include<string>
#include "common/BigInt.h"

namespace kad{

    class KadID{
    public:
        //enum KadIdType { kaxId, kRandomId };
        /**
         * Default KadID constructor.
         *
         */
        KadID();

        /**
         * KadID constructor with std string
         *
         */

        KadID( std::string raw );

        /**
         * KadID copy constructor copy
         *
         */
        KadID( BigInt::Rossi raw);

        KadID( const KadID & other );

        KadID( const KadID & min, const KadID & max);

        /**
         * SplitRange split min and max to ( min , max1) and ( min1 , max)
         *
         * @param min_id  min id
         * @param max_id  max id
         * @param min_id1 ( min + max)/2 +1;
         * @param max_id1 ( min + max)/2
         */
        static void SplitRange ( const KadID & min_id, const KadID & max_id , KadID * min_id1 , KadID * max_id1);
        /**
         * CloserToTarget , compare id1 and id2 with target
         *
         * @param id1
         * @param id2
         * @param target
         *
         * @return whether id1 is closer to target
         */
        static bool CloserToTarget(KadID  id1, KadID id2, const KadID &target );

        static const KadID KMaxID();
        static const KadID KMinID();

        KadID & operator = ( const KadID & other );
        bool operator == ( const KadID & other );
        bool operator != ( const KadID & other );
        bool operator <  ( const KadID & other );
        bool operator >  ( const KadID & other );
        bool operator <= ( const KadID & other );
        bool operator >= ( const KadID & other );
        //friend KadID operator + ( const KadID & ls,const KadID & other );
        KadID operator + ( const KadID & other );
        KadID operator - ( const KadID & other );
        KadID operator / ( const int & rs );
        KadID operator ^ ( const KadID & other );
        KadID operator >> ( const int shift);

        friend bool operator <= (const KadID & ls, const KadID & rs);
        friend bool operator >= (const KadID & ls, const KadID & rs);
        friend bool operator == (const KadID & ls, const KadID & rs);
        friend bool operator <  (const KadID & ls, const KadID & rs);
        friend bool operator >  (const KadID & ls, const KadID & rs);
        friend KadID operator ^ ( const KadID & ls, const KadID & rs);
        friend bool operator != (const KadID & ls, const KadID & rs);

        friend std::ostream& operator << ( std::ostream &os , const KadID & id );

        BigInt::Rossi getRawID();
    private:
        BigInt::Rossi raw_id;
    };
}

#endif
