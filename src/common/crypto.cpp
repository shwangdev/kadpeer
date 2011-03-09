/**
 * @file   crypto.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Mar  2 19:33:50 2011
 *
 * @brief
 *
 *
 */
#include "crypto.h"
#include <cryptopp/integer.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/gzip.h>
#include <cryptopp/hex.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include "blah.hpp"
#include <limits>

#include <boost/cstdint.hpp>
namespace crypto {

    CryptoPP::RandomNumberGenerator & GlobalRNG() {
        static CryptoPP::AutoSeededRandomPool rand_pool;
        return rand_pool;
    }

    std::string Crypto::XOROperation(const std::string &first,
                                     const std::string &second) {
        std::string result(first);
        for (unsigned int i = 0; i < result.length(); i++) {
            result[i] = first[i] ^ second[i];
        }
        return result;
    }

    std::string Crypto::Obfuscate(const std::string &first,
                                  const std::string &second,
                                  const obfuscationtype &obt) {
        std::string result;
        if ((first.length() != second.length()) || (first.length() == 0))
            return result;
        switch (obt) {
        case XOR:
            result = XOROperation(first, second);
            break;
        default:
            return result;
        }
        return result;
    }

    std::string Crypto::SecurePassword(const std::string &password,
                                       const int &pin) {
        if ((password == "") || (pin == 0))
            return "";
        byte purpose = 0;
        std::string derived_password;
        std::string salt = "maidsafe_salt";
        unsigned int iter = (pin % 1000)+1000;
        CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA512> pbkdf;
        CryptoPP::SecByteBlock derived(32);
        pbkdf.DeriveKey(derived, derived.size(), purpose,
                        reinterpret_cast<const byte *>(password.data()),
                        password.size(), reinterpret_cast<const byte *>(salt.data()),
                        salt.size(), iter);
        CryptoPP::HexEncoder enc(new CryptoPP::StringSink(derived_password), false);
        enc.Put(derived, derived.size());
        return derived_password;
    }

    template <class T>
    std::string Crypto::HashFunc(const std::string &input,
                                 const std::string &output,
                                 const operationtype &ot,
                                 const bool &hex,
                                 T hash) {
        std::string result;
        switch (ot) {
        case STRING_STRING:
            if (hex) {
                CryptoPP::StringSource(input, true, new CryptoPP::HashFilter(hash,
                                                                             new CryptoPP::HexEncoder(new CryptoPP::StringSink(result), false)));
            } else {
                CryptoPP::StringSource(input, true, new CryptoPP::HashFilter(hash,
                                                                             new CryptoPP::StringSink(result)));
            }
            break;
        case STRING_FILE:
            result = output;
            try {
                if (hex) {
                    CryptoPP::StringSource(input, true, new CryptoPP::HashFilter(hash,
                                                                                 new CryptoPP::HexEncoder(new CryptoPP::FileSink(output.c_str()),
                                                                                                          false)));
                } else {
                    CryptoPP::StringSource(input, true, new CryptoPP::HashFilter(hash,
                                                                                 new CryptoPP::FileSink(output.c_str())));
                }
            }
            catch(const CryptoPP::Exception &e) {
                LOG<< e.what() << std::endl;
                result = "";
            }
            break;
        case FILE_STRING:
            try {
                if (hex) {
                    CryptoPP::FileSource(input.c_str(), true,
                                         new CryptoPP::HashFilter(hash,
                                                                  new CryptoPP::HexEncoder(new CryptoPP::StringSink(result),
                                                                                           false)));
                } else {
                    CryptoPP::FileSource(input.c_str(), true,
                                         new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(result)));
                }
            }
            catch(const CryptoPP::Exception &e) {
                LOG << e.what() << std::endl;
                result = "";
            }
            break;
        case FILE_FILE:
            result = output;
            try {
                if (hex) {
                    CryptoPP::FileSource(input.c_str(), true,
                                         new CryptoPP::HashFilter(hash,
                                                                  new CryptoPP::HexEncoder(new CryptoPP::FileSink(output.c_str()),
                                                                                           false)));
                } else {
                    CryptoPP::FileSource(input.c_str(), true,
                                         new CryptoPP::HashFilter(hash,
                                                                  new CryptoPP::FileSink(output.c_str())));
                }
            }
            catch(const CryptoPP::Exception &e) {
                result = "";
            }
            break;
        }
        return result;
    }

    std::string Crypto::Hash(const std::string &input,
                             const std::string &output,
                             const operationtype &ot,
                             const bool &hex) {
        switch (hash_algorithm_) {
        case SHA_512: {
            CryptoPP::SHA512 hash;
            return HashFunc(input, output, ot, hex, hash);
        }
        case SHA_1: {
            CryptoPP::SHA1 hash;
            return HashFunc(input, output, ot, hex, hash);
        }
//    case SHA_224: {
//      CryptoPP::SHA224 hash;
//      return HashFunc(input, output, ot, hex, hash);
//    }
        case SHA_256: {
            CryptoPP::SHA256 hash;
            return HashFunc(input, output, ot, hex, hash);
        }
        case SHA_384: {
            CryptoPP::SHA384 hash;
            return HashFunc(input, output, ot, hex, hash);
        }
        default: {
            CryptoPP::SHA512 hash;
            return HashFunc(input, output, ot, hex, hash);
        }
        }
    }

    std::string Crypto::SymmEncrypt(const std::string &input,
                                    const std::string &output,
                                    const operationtype &ot,
                                    const std::string &key) {
        if (symm_algorithm_ != AES_256)
            return "";
        CryptoPP::SHA512 hash;
        std::string hashkey = HashFunc(key, "", STRING_STRING, true, hash);
        byte byte_key[AES256_KeySize], byte_iv[AES256_IVSize];
        CryptoPP::StringSource(hashkey.substr(0, AES256_KeySize), true,
                               new CryptoPP::ArraySink(byte_key, sizeof(byte_key)));
        CryptoPP::StringSource(hashkey.substr(AES256_KeySize, AES256_IVSize),
                               true, new CryptoPP::ArraySink(byte_iv, sizeof(byte_iv)));
        std::string result;
        CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptor(byte_key,
                                                                sizeof(byte_key), byte_iv);
        switch (ot) {
        case STRING_STRING:
            CryptoPP::StringSource(input, true,
                                   new CryptoPP::StreamTransformationFilter(encryptor,
                                                                            new CryptoPP::StringSink(result)));
            break;
        case STRING_FILE:
            result = output;
            try {
                CryptoPP::StringSource(input, true,
                                       new CryptoPP::StreamTransformationFilter(encryptor,
                                                                                new CryptoPP::FileSink(output.c_str())));
            }
            catch(const CryptoPP::Exception &e) {
                LOG<< e.what() << std::endl;
                result = "";
            }
            break;
        case FILE_STRING:
            try {
                CryptoPP::FileSource(input.c_str(), true,
                                     new CryptoPP::StreamTransformationFilter(encryptor,
                                                                              new CryptoPP::StringSink(result)));
            }
            catch(const CryptoPP::Exception &e) {
                LOG << e.what() << std::endl;
                result = "";
            }
            break;
        case FILE_FILE:
            result = output;
            try {
                CryptoPP::FileSource(input.c_str(), true,
                                     new CryptoPP::StreamTransformationFilter(encryptor,
                                                                              new CryptoPP::FileSink(output.c_str())));
            }
            catch(const CryptoPP::Exception &e) {
                LOG << e.what() << std::endl;
                result = "";
            }
            break;
        }
        return result;
    }

    std::string Crypto::SymmDecrypt(const std::string &input,
                                    const std::string &output,
                                    const operationtype &ot,
                                    const std::string &key) {
        if (symm_algorithm_ != AES_256)
            return "";
        CryptoPP::SHA512 hash;
        std::string hashkey = HashFunc(key, "", STRING_STRING, true, hash);
        byte byte_key[ AES256_KeySize ], byte_iv[ AES256_IVSize ];
        CryptoPP::StringSource(hashkey.substr(0, AES256_KeySize), true,
                               new CryptoPP::ArraySink(byte_key, sizeof(byte_key)));
        CryptoPP::StringSource(hashkey.substr(AES256_KeySize, AES256_IVSize),
                               true, new CryptoPP::ArraySink(byte_iv, sizeof(byte_iv)));
        std::string result;
        CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryptor(byte_key,
                                                                sizeof(byte_key), byte_iv);
        switch (ot) {
        case STRING_STRING:
            CryptoPP::StringSource(input, true,
                                   new CryptoPP::StreamTransformationFilter(decryptor,
                                                                            new CryptoPP::StringSink(result)));
            break;
        case STRING_FILE:
            result = output;
            try {
                CryptoPP::StringSource(reinterpret_cast<const byte *>(input.c_str()),
                                       input.length(), true,
                                       new CryptoPP::StreamTransformationFilter(decryptor,
                                                                                new CryptoPP::FileSink(output.c_str())));
            }
            catch(const CryptoPP::Exception &e) {
                LOG << e.what() << std::endl;
                result = "";
            }
            break;
        case FILE_STRING:
            try {
                CryptoPP::FileSource(input.c_str(), true,
                                     new CryptoPP::StreamTransformationFilter(decryptor,
                                                                              new CryptoPP::StringSink(result)));
            }
            catch(const CryptoPP::Exception &e) {
                LOG << e.what() << std::endl;
                result = "";
            }
            break;
        case FILE_FILE:
            result = output;
            try {
                CryptoPP::FileSource(input.c_str(), true,
                                     new CryptoPP::StreamTransformationFilter(decryptor,
                                                                              new CryptoPP::FileSink(output.c_str())));
            }
            catch(const CryptoPP::Exception &e) {
                LOG << e.what() << std::endl;
                result = "";
            }
            break;
        }
        return result;
    }

    std::string Crypto::AsymEncrypt(const std::string &input,
                                    const std::string &output,
                                    const std::string &key,
                                    const operationtype &ot) {
        try {
            CryptoPP::StringSource pubkey(key, true);
            CryptoPP::RSAES_OAEP_SHA_Encryptor pub(pubkey);
            CryptoPP::AutoSeededRandomPool rand_pool;
            std::string result;

            switch (ot) {
            case STRING_STRING:
                CryptoPP::StringSource(input, true,
                                       new CryptoPP::PK_EncryptorFilter(rand_pool, pub,
                                                                        new CryptoPP::StringSink(result)));
                break;
            case STRING_FILE:
                result = output;
                CryptoPP::StringSource(input, true,
                                       new CryptoPP::PK_EncryptorFilter(rand_pool, pub,
                                                                        new CryptoPP::FileSink(output.c_str())));
                break;
            case FILE_STRING:
                CryptoPP::FileSource(input.c_str(), true,
                                     new CryptoPP::PK_EncryptorFilter(rand_pool, pub,
                                                                      new CryptoPP::StringSink(result)));
                break;
            case FILE_FILE:
                result = output;
                CryptoPP::FileSource(input.c_str(), true,
                                     new CryptoPP::PK_EncryptorFilter(rand_pool, pub,
                                                                      new CryptoPP::FileSink(output.c_str())));
                break;
            }
            return result;
        }
        catch(const CryptoPP::Exception &e) {
            LOG << e.what() << std::endl;
            return "";
        }
    }

    std::string Crypto::AsymDecrypt(const std::string &input,
                                    const std::string &output,
                                    const std::string &key,
                                    const operationtype &ot) {
        try {
            CryptoPP::StringSource privkey(key, true);
            CryptoPP::RSAES_OAEP_SHA_Decryptor priv(privkey);
            std::string result;
            switch (ot) {
            case STRING_STRING:
                CryptoPP::StringSource(input, true,
                                       new CryptoPP::PK_DecryptorFilter(GlobalRNG(), priv,
                                                                        new CryptoPP::StringSink(result)));
                break;
            case STRING_FILE:
                result = output;
                CryptoPP::StringSource(input, true,
                                       new CryptoPP::PK_DecryptorFilter(GlobalRNG(), priv,
                                                                        new CryptoPP::FileSink(output.c_str())));
                break;
            case FILE_STRING:
                CryptoPP::FileSource(input.c_str(), true,
                                     new CryptoPP::PK_DecryptorFilter(GlobalRNG(), priv,
                                                                      new CryptoPP::StringSink(result)));
                break;
            case FILE_FILE:
                result = output;
                CryptoPP::FileSource(input.c_str(), true,
                                     new CryptoPP::PK_DecryptorFilter(GlobalRNG(), priv,
                                                                      new CryptoPP::FileSink(output.c_str())));
                break;
            }
            return result;
        }
        catch(const CryptoPP::Exception &e) {
            LOG << e.what() << std::endl;
            return "";
        }
    }

    std::string Crypto::AsymSign(const std::string &input,
                                 const std::string &output,
                                 const std::string &key,
                                 const operationtype &ot) {
        try {
            CryptoPP::StringSource privkey(key, true);
            CryptoPP::RSASS<CryptoPP::PKCS1v15, CryptoPP::SHA512>::Signer
                signer(privkey);
            std::string result;
            switch (ot) {
            case STRING_STRING:
                CryptoPP::StringSource(input, true,
                                       new CryptoPP::SignerFilter(GlobalRNG(), signer,
                                                                  new CryptoPP::StringSink(result)));
                break;
            case STRING_FILE:
                result = output;
                CryptoPP::StringSource(input , true,
                                       new CryptoPP::SignerFilter(GlobalRNG(), signer,
                                                                  new CryptoPP::FileSink(output.c_str())));
                break;
            case FILE_STRING:
                CryptoPP::FileSource(input.c_str(), true,
                                     new CryptoPP::SignerFilter(GlobalRNG(), signer,
                                                                new CryptoPP::StringSink(result)));
                break;
            case FILE_FILE:
                result = output;
                CryptoPP::FileSource(input.c_str(), true,
                                     new CryptoPP::SignerFilter(GlobalRNG(), signer,
                                                                new CryptoPP::FileSink(output.c_str())));
                break;
            }
            return result;
        }
        catch(const CryptoPP::Exception &e) {
            LOG << e.what() << std::endl;
            return "";
        }
    }

    bool Crypto::AsymCheckSig(const std::string &input_data,
                              const std::string &input_signature,
                              const std::string &key,
                              const operationtype &ot) {
        try {
            CryptoPP::StringSource pubkey(key, true);

            CryptoPP::RSASS<CryptoPP::PKCS1v15, CryptoPP::SHA512>::Verifier
                verifier(pubkey);
            bool result = false;
            CryptoPP::SecByteBlock *signature;
            CryptoPP::SignatureVerificationFilter *verifierFilter;

            if ((ot == STRING_STRING) || (ot == STRING_FILE)) {
                CryptoPP::StringSource signatureString(input_signature, true);
                if (signatureString.MaxRetrievable() != verifier.SignatureLength())
                    return result;
                signature = new CryptoPP::SecByteBlock(verifier.SignatureLength());
                signatureString.Get(*signature, signature->size());

                verifierFilter = new CryptoPP::SignatureVerificationFilter(verifier);
                verifierFilter->Put(*signature, verifier.SignatureLength());
                CryptoPP::StringSource(input_data, true, verifierFilter);
                result = verifierFilter->GetLastResult();
                delete signature;
                return result;
            } else if ((ot == FILE_FILE) || (ot == FILE_STRING)) {
                CryptoPP::FileSource signatureFile(input_signature.c_str(), true);
                if (signatureFile.MaxRetrievable() != verifier.SignatureLength())
                    return false;
                signature = new CryptoPP::SecByteBlock(verifier.SignatureLength());
                signatureFile.Get(*signature, signature->size());

                verifierFilter = new CryptoPP::SignatureVerificationFilter(verifier);
                verifierFilter->Put(*signature, verifier.SignatureLength());
                CryptoPP::FileSource(input_data.c_str(), true, verifierFilter);
                result = verifierFilter->GetLastResult();
                delete signature;
                return result;
            } else {
                return false;
            }
        }
        catch(const CryptoPP::Exception &e) {
            LOG << e.what() << std::endl;
            return false;
        }
    }

    std::string Crypto::Compress(const std::string &input,
                                 const std::string &output,
                                 const int &compression_level,
                                 const operationtype &ot) {
        if (compression_level < 0 || compression_level > 9)
            return "";
        try {
            std::string result("");
            switch (ot) {
            case STRING_STRING:
                CryptoPP::StringSource(input, true, new CryptoPP::Gzip(
                                           new CryptoPP::StringSink(result), compression_level));
                break;
            case STRING_FILE:
                result = output;
                CryptoPP::StringSource(input, true, new CryptoPP::Gzip(
                                           new CryptoPP::FileSink(output.c_str()), compression_level));
                break;
            case FILE_STRING:
                CryptoPP::FileSource(input.c_str(), true, new CryptoPP::Gzip(
                                         new CryptoPP::StringSink(result), compression_level));
                break;
            case FILE_FILE:
                result = output;
                CryptoPP::FileSource(input.c_str(), true, new CryptoPP::Gzip(
                                         new CryptoPP::FileSink(output.c_str()), compression_level));
                break;
            }
            return result;
        }
        catch(const CryptoPP::Exception &e) {
            LOG << e.what() << std::endl;
            return "";
        }
    }

    std::string Crypto::Uncompress(const std::string &input,
                                   const std::string &output,
                                   const operationtype &ot) {
        try {
            std::string result;
            switch (ot) {
            case STRING_STRING:
                CryptoPP::StringSource(input, true, new CryptoPP::Gunzip(
                                           new CryptoPP::StringSink(result)));
                break;
            case STRING_FILE:
                result = output;
                CryptoPP::StringSource(input, true, new CryptoPP::Gunzip(
                                           new CryptoPP::FileSink(output.c_str())));
                break;
            case FILE_STRING:
                CryptoPP::FileSource(input.c_str(), true, new CryptoPP::Gunzip(
                                         new CryptoPP::StringSink(result)));
                break;
            case FILE_FILE:
                result = output;
                CryptoPP::FileSource(input.c_str(), true, new CryptoPP::Gunzip(
                                         new CryptoPP::FileSink(output.c_str())));
                break;
            }
            return result;
        }
        catch(const CryptoPP::Exception &e) {
            LOG << e.what() << std::endl;
            return "";
        }
    }

    void RsaKeyPair::GenerateKeys(const unsigned int &keySize) {  //NOLINT
        // CryptoPP::AutoSeededRandomPool rand_pool;
        private_key_.clear();
        public_key_.clear();
        CryptoPP::RandomPool rand_pool;
        //std::string seed = base::RandomString(keySize);
        std::string seed = RandomString(keySize);
        rand_pool.IncorporateEntropy(reinterpret_cast<const byte *>(seed.c_str()),
                                     seed.size());

        CryptoPP::RSAES_OAEP_SHA_Decryptor priv(rand_pool, keySize);  // 256 bytes
        CryptoPP::StringSink privKey(private_key_);
        priv.DEREncode(privKey);
        privKey.MessageEnd();

        CryptoPP::RSAES_OAEP_SHA_Encryptor pub(priv);
        CryptoPP::StringSink pubKey(public_key_);
        pub.DEREncode(pubKey);
        pubKey.MessageEnd();
    }

    std::string RandomString(int length) {

        std::string str;
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::Integer rand_num(rng, 32);
        for ( int i = 0 ; i < length ; ++i ) {

            boost::uint32_t num;
            if (!rand_num.IsConvertableToLong()) {

                num = std::numeric_limits<uint32_t>::max() + static_cast<uint32_t>(\
                    rand_num.AbsoluteValue().ConvertToLong());
            } else {

                num = static_cast<uint32_t>(rand_num.AbsoluteValue().ConvertToLong());
            }
            num = num % 122;
            if ( 48 > num )
                num += 48;
            if ( ( 57 < num ) && ( 65 > num ) )
                num += 7;
            if ( ( 90 < num ) && ( 97 > num ) )
                num += 6;
            str += static_cast<char>(num);
            rand_num.Randomize(rng, 32);
        }
        return str;
    }

}  // namespace crypto
