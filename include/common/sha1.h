/**
 * @file   sha1.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Aug 18 17:51:53 2010
 *
 * @brief  SHA1 alorithm
 *
 *
 */


#ifndef _SHA1_H_
#define _SHA1_H_
/// add BigInt.h for integration with BigInt
#include "BigInt.h"

class SHA1
{

public:

    SHA1();
    virtual ~SHA1();

    /*
     *  Re-initialize the class
     */
    void Reset();

    /*
     *  Returns the message digest
     */
    bool Result(unsigned *message_digest_array);

    /**
     *
     * @param bint input and output of SHA1 is \a BigInt
     *
     * @return
     */
    bool Result( BigInt::Rossi & bint);
    /*
     *  Provide input to SHA1
     */
    void Input( const unsigned char *message_array,
                unsigned            length);
    void Input( const char  *message_array,
                unsigned    length);
    void Input(unsigned char message_element);
    void Input(char message_element);
    SHA1& operator<<(const char *message_array);
    SHA1& operator<<(const unsigned char *message_array);
    SHA1& operator<<(const char message_element);
    SHA1& operator<<(const unsigned char message_element);

private:

    /*
     *  Process the next 512 bits of the message
     */
    void ProcessMessageBlock();

    /*
     *  Pads the current message block to 512 bits
     */
    void PadMessage();

    /*
     *  Performs a circular left shift operation
     */
    inline unsigned CircularShift(int bits, unsigned word);

    unsigned H[5];                      // Message digest buffers

    unsigned Length_Low;                // Message length in bits
    unsigned Length_High;               // Message length in bits

    unsigned char Message_Block[64];    // 512-bit message blocks
    int Message_Block_Index;            // Index into message block array

    bool Computed;                      // Is the digest computed?
    bool Corrupted;                     // Is the message digest corruped?

};

#endif
