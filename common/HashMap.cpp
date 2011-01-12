/**
 * @file   HashMap.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Nov 17 00:26:11 2010
 *
 * @brief  HashMap construction
 *
 *
 */

#include "HashMap.h"
#include <cstring>

bool CharArrayEqualFunc::operator()(const char * s1,
				    const char * s2) const
{
    return strcmp(s1,s2) >= 0;
}


size_t CharArrayHashFunc::operator()(const char * s) const
{
    int hashTemp = 0;

    for(unsigned int i = 0; s[i] != 0; i ++){
	if(0 > hashTemp) hashTemp = (hashTemp << 1) + 1;
	else hashTemp = hashTemp << 1;
	hashTemp ^= s[i];
    }
    return (size_t(hashTemp));
}

