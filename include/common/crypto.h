/**
 * @file   crypto.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Mar  2 19:34:21 2011
 *
 * @brief
 *
 *
 */

#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <string>

namespace crypto {

    const int AES256_KeySize = 32;  // size in bytes
    const int AES256_IVSize = 16;   // in bytes
/**
 * Types of operation regarding source and destination
 */
    enum operationtype {FILE_FILE, /**< file source, file destination >*/
                        STRING_FILE, /**< string source, file destination >*/
                        FILE_STRING, /**< file source, string destination >*/
                        STRING_STRING /**< string source, string destination >*/
    };
/**
 * Types of Hash handled.
 */
    enum hashtype {SHA_512, SHA_1, /*SHA_224, */SHA_256, SHA_384};
/**
 * Types of symmetric encryption.
 */
    enum symmtype {AES_256};
/**
 * Types of symmetric obfuscation.
 */
    enum obfuscationtype {XOR};

/**
 * @class Crypto
 * Object with the following cryptographic operations: Hash a string, AES
 * encryption/decryption, RSA encryption/decrytion, and sign and
 * verify a signature.
 */


    std::string RandomString(int length) ;

    class Crypto {
    public:
        Crypto() : hash_algorithm_(SHA_512), symm_algorithm_(AES_256) {}
        /**
         * Objuscate a string with another one.
         * @param first string to be obfuscated.
         * @param second string used to obfuscate the first one.
         * @param obt type of objuscation, for example XOR
         * @return The objuscated string
         */
        std::string Obfuscate(const std::string &first, const std::string &second,
                              const obfuscationtype &obt);
        /**
         * Creates a secure password using the Password-Based Key Derivation Function
         * (PBKDF) version 2 algorithm.
         * @param password password
         * @param pin number from which the number of iterations is derived.
         * @return The derived key
         */
        std::string SecurePassword(const std::string &password, const int &pin);
        inline void set_hash_algorithm(hashtype type) {hash_algorithm_ = type;}
        inline hashtype hash_algorithm() const {return hash_algorithm_;}
        /**
         * Hash function. It returns an empty string if the input from a file
         * could not be read or cannot write the output to a file.
         * @param input string or path to file that is going to be hashed
         * @param output path where the result is going to be written, if
         * the result is going to be returned as a string, then it is ignored
         * @param ot type of operation
         * @param hex sets how the result is going to be retured, True for encoded,
         * False for decoded.
         * @return the result of the hash function or the path of the file where the
         * result is written or an empty string
         */
        std::string Hash(const std::string &input, const std::string &output,
                         const operationtype &ot, const bool &hex);
        inline void set_symm_algorithm(const symmtype &type) {symm_algorithm_ = type;}
        inline symmtype symm_algorithm() const {return symm_algorithm_;}
        /**
         * Performs a symetric encrytion of data. It returns an empty string if the input
         * from a file could not be read or cannot write the output to a file.
         * @param input string or path to file that is going to be encrypted
         * @param output path where the result is going to be written, if
         * the result is going to be returned as a string, then it is ignored
         * @param ot type of operation
         * @param key key used to encrypt
         * @return the encrypted data or the path of the file where the result is
         * written or an empty string
         */
        std::string SymmEncrypt(const std::string &input, const std::string &output,
                                const operationtype &ot, const std::string &key);
        /**
         * Tries to decrypt symmetric encrypted data.  It returns an empty string if
         * the input from a file could not be read or cannot write the output to a
         * file.
         * @param input string or path to file that is going to be decrypted
         * @param output path where the result is going to be written, if
         * the result is going to be returned as a string, then it is ignored
         * @param ot type of operation
         * @param key key used to decrypt
         * @return the decrypted data or the path of the file where the result is
         * written or an empty string
         */
        std::string SymmDecrypt(const std::string &input, const std::string &output,
                                const operationtype &ot, const std::string &key);
        /**
         * Encrypts data with a public key using RSA algorithm.  It returns an empty
         * string if the string passed as a public key is not valid.  It also
         * returns an empty string if the input from a file could not be read or
         * cannot write the output to a file.
         * @param input string or path to file that is going to be encrypted
         * @param output path where the result is going to be written, if
         * the result is going to be returned as a string, then it is ignored
         * @param key public key used to encrypt
         * @param ot type of operation
         * @return the encrypted data or the path of the file where the result is
         * written or an empty string
         */
        std::string AsymEncrypt(const std::string &input, const std::string &output,
                                const std::string &key, const operationtype &ot);
        /**
         * Decrypts data with a private key using RSA algorithm.  It returns an empty
         * string if the string passed as a private key is not valid.  It also
         * returns an empty string if the input from a file could not be read or
         * cannot write the output to a file.
         * @param input string or path to file that is going to be decrypted
         * @param output path where the result is going to be written, if
         * the result is going to be returned as a string, then it is ignored
         * @param key private key used to decrypt
         * @param ot type of operation
         * @return the encrypted data or the path of the file where the result is
         * written or an empty string
         */
        std::string AsymDecrypt(const std::string &input, const std::string &output,
                                const std::string &key, const operationtype &ot);
        // AsymSign -- key is a private key.  Dependinging on the operation type, the
        // function returns either the 512 bit signature or the path to output file
        // which contains the signature.
        /**
         * Signs data with a private key.  It returns the 512 bit signature.  It
         * returns an empty string if the input from a file could not be read or
         * cannot write the output to a file or the key is not a valid private key.
         * @param input string or path to file that is going to be signed
         * @param output path where the result is going to be written, if
         * the result is going to be returned as a string, then it is ignored
         * @param key private key used to decrypt
         * @param ot type of operation
         * @return the signature of the data or the path of the file where the result is
         * written or an empty string
         */
        std::string AsymSign(const std::string &input, const std::string &output,
                             const std::string &key, const operationtype &ot);
        /**
         * Verifies the signature of some data signed with a public key using
         * the correspondent public key.
         * @param input_data string or path to file of the original data that was
         * signed
         * @param input_signature string or path to file of the signature to be
         * verified
         * @param key public key
         * @param ot type of operation
         * @return True if the validation was correct, false otherwise
         */
        bool AsymCheckSig(const std::string &input_data,
                          const std::string &input_signature, const std::string &key,
                          const operationtype &ot);
        /**
         * Compress a string or a file using gzip.  Compression level must be
         * between 0 and 9 inclusive or function returns "".  It also returns an
         * empty string if input from a file could not be read or cannot write the
         * output to a file.
         * @param input string or path of file to be compressed
         * @param output path where the result is going to be written, if
         * the result is going to be returned as a string, then it is ignored
         * @param compression_level level of compression
         * @param ot operation type
         * @return the compressed data or path to the file containing it or an
         * empty string
         */
        std::string Compress(const std::string &input, const std::string &output,
                             const int &compression_level, const operationtype &ot);
        /**
         * Uncompressed a string or a file using gzip. It returns an
         * empty string if input from a file could not be read or cannot write the
         * output to a file.
         * @param input string or path of file to be uncompressed
         * @param output path where the result is going to be written, if
         * the result is going to be returned as a string, then it is ignored
         * @param ot operation type
         * @return the uncompressed data or path to the file containing it or an
         * empty string
         */
        std::string Uncompress(const std::string &input, const std::string &output,
                               const operationtype &ot);
    private:
        /**
         * XOR obfuscation operation.
         */
        std::string XOROperation(const std::string &first,
                                 const std::string &second);
        template <class T>
	    std::string HashFunc(const std::string &input, const std::string &output,
                             const operationtype &ot, const bool &hex, T hash);
        hashtype hash_algorithm_;
        symmtype symm_algorithm_;
    };

/**
 * @class RsaKeyPair
 * Object that generates and holds a RSA key pair (private and public keys) of
 * lenght given by the user.
 */
    class RsaKeyPair {
    public:
        RsaKeyPair() : public_key_(""), private_key_("") {}
        inline std::string public_key() const {return public_key_;}
        inline std::string private_key() const {return private_key_;}
        inline void set_public_key(std::string publickey) {
            public_key_ = publickey;
        }
        inline void set_private_key(std::string privatekey) {
            private_key_ = privatekey;
        }
        /**
         * Clears the keys and sets them as empty strings.
         */
        inline void ClearKeys() {private_key_ = public_key_ = "";}
        /**
         * Generates a pair of RSA keys of given size.
         * @param keySize size in bits of the keys
         */
        void GenerateKeys(const unsigned int &keySize);
    private:
        std::string public_key_;
        std::string private_key_;
    };

}
#endif

