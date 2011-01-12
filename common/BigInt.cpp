
#include "BigInt.h"
#include <exception>

namespace BigInt{
// =================
// Constants
// =================
    static const BigInt::Rossi RossiZero (0);
    static const BigInt::Rossi RossiOne  (1);
    static const BigInt::Rossi RossiTwo  (2);
// static const BigInt::Rossi RossiTen  = BigInt::Rossi(BigInt::toString(std::numeric_limits<BigInt::Ulong>::max()), BigInt::DEC_DIGIT) + RossiOne;

    static const BigInt::Vin VinZero (0);
    static const BigInt::Vin VinOne  (1);


    static const std::pair<char, std::size_t> s_hex2dec[] =
    {
        std::make_pair ('0', 0),
        std::make_pair ('1', 1),
        std::make_pair ('2', 2),
        std::make_pair ('3', 3),
        std::make_pair ('4', 4),
        std::make_pair ('5', 5),
        std::make_pair ('6', 6),
        std::make_pair ('7', 7),
        std::make_pair ('8', 8),
        std::make_pair ('9', 9),
        std::make_pair ('a', 10),
        std::make_pair ('A', 10),
        std::make_pair ('b', 11),
        std::make_pair ('B', 11),
        std::make_pair ('c', 12),
        std::make_pair ('C', 12),
        std::make_pair ('d', 13),
        std::make_pair ('D', 13),
        std::make_pair ('e', 14),
        std::make_pair ('E', 14),
        std::make_pair ('f', 15),
        std::make_pair ('F', 15)
    };

    std::map<char, std::size_t> BigInt::BaseBigInt::ms_hex2dec (array2map(s_hex2dec));

// =================
// Macros
// =================

//#define SET_TEST_NAME(outstream) BigInt::Test::setTestName(outstream, FUNCTION_NAME)

#define BIGINT_TEST_COMPUTE_BINARY_OP(outstream, type, args, op)	\
    type z = args.first op args.second;                             \
    outstream << args.first                                         \
    << " "                                                          \
    << #op                                                          \
    << " "                                                          \
    << args.second                                                  \
    << " = "                                                        \
    << z                                                            \
    << std::endl

#define BIGINT_TEST_COMPUTE_BINARY_ULONG_OP(outstream, type, args, op)	\
    type z = args.first op args.second;                                 \
    outstream << args.first                                             \
    << " "                                                              \
    << #op                                                              \
    << " "                                                              \
    << std::hex                                                         \
    << std::showbase                                                    \
    << args.second                                                      \
    << std::dec                                                         \
    << " = "                                                            \
    << z                                                                \
    << std::endl

#define BIGINT_TEST_COMPARE_BINARY_OP(outstream, args, op)	\
    const bool z = args.first op args.second;               \
    outstream << args.first                                 \
    << " "                                                  \
    << #op                                                  \
    << " "                                                  \
    << args.second                                          \
    << " = "                                                \
    << (z ? "TRUE" : "FALSE")                               \
    << std::endl

#define BIGINT_TEST_COMPUTE_UNARY_OP(outstream, args, op)	\
    outstream << args.first;                                \
    args.first op args.second;                              \
    outstream << ""                                         \
    << " "                                                  \
    << #op                                                  \
    << " "                                                  \
    << args.second                                          \
    << " = "                                                \
    << args.first                                           \
    << std::endl


#define ROSSI_TEST_COMPUTE_BINARY_OP(outstream, args, op)          BIGINT_TEST_COMPUTE_BINARY_OP(outstream, BigInt::Rossi, args, op)
#define ROSSI_TEST_COMPUTE_BINARY_ULONG_OP(outstream, args, op)    BIGINT_TEST_COMPUTE_BINARY_ULONG_OP(outstream, BigInt::Rossi, args, op)

#define VIN_TEST_COMPUTE_BINARY_OP(outstream, args, op)            BIGINT_TEST_COMPUTE_BINARY_OP(outstream, BigInt::Vin, args, op)
#define VIN_TEST_COMPUTE_BINARY_ULONG_OP(outstream, args, op)      BIGINT_TEST_COMPUTE_BINARY_ULONG_OP(outstream, BigInt::Vin, args, op)

// ==================================
    void showVersion(std::ostream& o_stream)
    {
        std::ostringstream oss;
        oss << std::endl

            << "BigInt - C++ class for computation with arbitrary large integers"
            << std::endl

            << "Version "
            << BIG_INT_MAJOR_VERSION
            << "."
            << BIG_INT_MINOR_VERSION
            << std::endl

            << "Compiled in "
            << (sizeof(BigInt::Ulong) * CHAR_BIT)
            << "-bit mode"
            << std::endl

            << std::endl

            << "Copyright (C) 1995       William A. Rossi - class BigInt::Rossi"
            << std::endl
            << "Copyright (C) 1999-2010  Alex Vinokur - class BigInt, class BigInt::BaseBigInt, class BigInt::Vin, upgrading class BigInt::Rossi, class BigInt::Run, class BigInt::Test, class BigInt::TestVin, class BigInt::TestRossi"
            << std::endl

            << std::endl

            << "This is free software; see the source for copying conditions."
            << std::endl
            << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."
            << std::endl

            << std::endl;

        o_stream << std::flush << oss.str() << std::flush;
    }

// ==========================================
    void BigInt::BaseBigInt::showUnits(std::ostream& o_stream) const
    {
        std::ostringstream oss;

        const std::string::size_type widthSpaces = static_cast<std::string::size_type>(BigInt::toString(m_units.size()).size());

        oss << "--- Units: BEGIN ---"
            << std::endl;
        for (std::size_t i = 0; i < m_units.size(); i++)
        {
            oss << "m_units["
                << i
                << "]"
                << std::string (widthSpaces, ' ')
                << ": "
                << std::hex
                << std::showbase
                << m_units[i]
                << std::dec
                << " = "
                << m_units[i]
                << std::endl;
        }
        oss << "---- Units: END ----"
            << std::endl;


        o_stream << std::flush << oss.str() << std::flush;
    }

// ================
// class BigInt::BaseBigInt
// ================
// ------------------
    bool BigInt::BaseBigInt::isEmpty() const
    {
        return m_units.empty();
    }



// ------------------
    void BigInt::BaseBigInt::maximize()
    {
        m_units.clear();
        std::vector<BigInt::Ulong>::size_type unitsSize = 0;

        const BigInt::Ulong maxUlong = std::numeric_limits<BigInt::Ulong>::max();

        while (true)
        {
            unitsSize = m_units.size();
            try
            {
                m_units.push_back(maxUlong);
            }
            catch(...)
            {
                // Do nothing
                break;
            }
        }
    }



// ------------------
    std::size_t BigInt::BaseBigInt::getUnitsSize() const
    {
        return m_units.size();
    }


// ------------------
    BigInt::BaseBigInt::~BaseBigInt()
    {
    }



// =================
// class BigInt::Vin
// =================
// ----------------------------
    BigInt::Ulong BigInt::Vin::s_carry = 0;
// ----------------------------
    BigInt::Ulong BigInt::Vin::addUnits(BigInt::Ulong n1, BigInt::Ulong n2)
    {
        n1 += (n2 + s_carry);
        s_carry = (OVER_BASE2 & n1) || (n1 > SUB_BASE2);
        return (s_carry ? (n1 - BASE2) : n1);
    }

// ------------------
// Constructor-0
    BigInt::Vin::Vin ()
    {
    }

// ------------------
// Constructor-1
    BigInt::Vin::Vin (BigInt::Ulong i_unit)
    {
        if (!(i_unit < BASE2))
        {
            const std::size_t witdh1 = 19;

            std::ostringstream ossErr;
            ossErr  << ""
                    << "BigInt::Ulong value too big: "
                    << std::endl

                    << std::setw (witdh1)
                    << std::left
                    << "i_unit"
                    << std::right
                    << " = "
                    << std::dec
                    << i_unit
                    << ", "
                    << std::hex
                    << std::showbase
                    << i_unit
                    << std::dec
                    << std::endl

                    << std::setw (witdh1)
                    << std::left
                    << "BASE2"
                    << std::right
                    << " = "
                    << BASE2
                    << ", "
                    << std::hex
                    << std::showbase
                    << BASE2
                    << std::dec
                    << std::endl

                    << std::setw (witdh1)
                    << std::left
                    << "MAX_UNIT_VALUE"
                    << std::right
                    << " = "
                    << MAX_UNIT_VALUE
                    << ", "
                    << std::hex
                    << std::showbase
                    << MAX_UNIT_VALUE
                    << std::dec
                    << std::endl
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());

        }
        ASSERTION (i_unit < BASE2);


        // ---------------------
        try
        {
            m_units.reserve(m_units.size() + 1);
        }
        catch(...)
        {
            std::ostringstream ossThrow;
            std::ostringstream ossErr;
            ossErr  << ""
                    << "Unable to to do m_units.reserve("
                    << (m_units.size() + 1)
                    << ")"
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
        }


        // ---------------------
        try
        {
            m_units.push_back (i_unit);
        }
        catch(...)
        {
            std::ostringstream ossThrow;
            std::ostringstream ossErr;
            ossErr  << ""
                    << "Unable to to do m_units.push_back()"
                    << "; "
                    << "m_units.size() = "
                    << m_units.size()
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
        }

    }



// ------------------
// Constructor-2
    BigInt::Vin::Vin (const std::string& i_arg, std::size_t i_digitBase)
    {
        bool rc = initViaString (i_arg, i_digitBase);
        ASSERTION (rc);
    }

// ------------------
// Constructor-3
    BigInt::Vin::Vin (const BigInt::Rossi& i_arg)
    {
        const std::string str (i_arg.toStrHex ());
        bool rc = initViaString (str, HEX_DIGIT);
        ASSERTION (rc);
    }






// ------------------
    BigInt::Ulong BigInt::Vin::toUlong () const
    {
        ASSERTION (!m_units.empty());
        if (m_units.size() > 1)
        {
            std::ostringstream ossErr;
            ossErr  << ""
                    << "BigInt::Ulong can't be obtained: m_units.size() too big"
                    << std::endl
                    << "m_units.size() = "
                    << m_units.size()
                    << std::endl
                    << "Here m_units.size() must be = 1"
                    << std::endl
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ASSERTION (m_units.size() == 1);
        }
        ASSERTION (m_units.size() == 1);
        return m_units.front();
    }

// ------------------
// BigInt::Vin::operator BigInt::Ulong () const
// {
//  return toUlong();
// }


// ------------------
    bool BigInt::Vin::initViaString (const std::string& i_arg, std::size_t i_digitBase)
    {
        ASSERTION (m_units.empty());
        ASSERTION ((i_digitBase == BigInt::HEX_DIGIT) || (i_digitBase == BigInt::DEC_DIGIT));

        m_units.push_back(0);

        for (std::size_t i = 0; i < i_arg.size(); i++)
        {
            switch (i_digitBase)
            {
            case DEC_DIGIT:
                if (!isdigit (i_arg[i]))
                {
                    std::ostringstream ossErr;
                    ossErr  << ""
                            << "std::string contains non-decimal digit"
                            << std::endl
                            << "std::string = "
                            << i_arg
                            << std::endl
                            << i
                            << "-th char = "
                            << i_arg[i]
                            << std::endl
                            << std::endl;

                    ERR_MSG (std::cerr, ossErr.str());

                    ASSERTION (0);
                    return false; // unused
                }
                break;

            case HEX_DIGIT:
                if (!isxdigit (i_arg[i]))
                {
                    std::ostringstream ossErr;
                    ossErr  << ""
                            << "std::string contains non-hexadecimal digit"
                            << std::endl
                            << "std::string = "
                            << i_arg
                            << std::endl
                            << i
                            << "-th char = "
                            << i_arg[i]
                            << std::endl
                            << std::endl;

                    ERR_MSG (std::cerr, ossErr.str());
                    ASSERTION (0);
                    return false; // unused
                }
                break;

            default:
                ASSERTION (0);
                return false;
                break; // unused
            }  // switch (i_digitBase)
        } // for (std::size_t i = 0; i < i_arg.size(); i++)

        for (std::size_t i = 0; i < i_arg.size(); i++)
        {
            *this = (*this) * static_cast<BigInt::Ulong>(i_digitBase)
                    +
                    BigInt::Vin (static_cast<BigInt::Ulong>(ms_hex2dec[i_arg[i]]));
        }
        return true;
    }



// ------------------
    BigInt::Vin BigInt::Vin::operator+ (const BigInt::Vin& i_arg) const
    {
        BigInt::Vin ret;
        const std::size_t maxSize (std::max (m_units.size (), i_arg.m_units.size ()));

        std::vector<BigInt::Ulong> u1 (m_units);
        std::vector<BigInt::Ulong> u2 (i_arg.m_units);


        // ---------------------
        try
        {
            u1.reserve(maxSize);
        }
        catch(...)
        {
            std::ostringstream ossThrow;
            std::ostringstream ossErr;
            ossErr  << ""
                    << "Unable to to do u1.reserve("
                    << maxSize
                    << ")"
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
        }

        // ---------------------
        try
        {
            u2.reserve(maxSize);
        }
        catch(...)
        {
            std::ostringstream ossThrow;
            std::ostringstream ossErr;
            ossErr  << ""
                    << "Unable to to do u2.reserve("
                    << maxSize
                    << ")"
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
        }


        // ---------------------
        try
        {
            ret.m_units.reserve(maxSize + 1);
        }
        catch(...)
        {
            std::ostringstream ossThrow;
            std::ostringstream ossErr;
            ossErr  << ""
                    << "Unable to to do ret.m_units.reserve("
                    << (maxSize + 1)
                    << ")"
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
        }

        // ---------------------
        try
        {
            u1.resize(maxSize);
        }
        catch(...)
        {
            std::ostringstream ossThrow;
            std::ostringstream ossErr;
            ossErr  << ""
                    << "Unable to to do u1.resize("
                    << maxSize
                    << ")"
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
        }

        // ---------------------
        try
        {
            u2.resize(maxSize);
        }
        catch(...)
        {
            std::ostringstream ossThrow;
            std::ostringstream ossErr;
            ossErr  << ""
                    << "Unable to to do u2.resize("
                    << maxSize
                    << ")"
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
        }

        // ---------------------
        try
        {
            ret.m_units.resize(maxSize);
        }
        catch(...)
        {
            std::ostringstream ossThrow;
            std::ostringstream ossErr;
            ossErr  << ""
                    << "Unable to to do ret.m_units.resize("
                    << maxSize
                    << ")"
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
        }



        s_carry = 0;
        std::transform (&u1[0], &u1[0] + maxSize, &u2[0], &ret.m_units[0], BigInt::Vin::addUnits);

        if (s_carry)
        {
            try
            {
                ret.m_units.push_back (s_carry);
            }
            catch(...)
            {
                std::ostringstream ossThrow;
                std::ostringstream ossErr;
                ossErr  << ""
                        << "Unable to to do ret.m_units.push_back()"
                        << "; "
                        << "ret.m_units.size() = "
                        << ret.m_units.size()
                        << std::endl;

                ERR_MSG (std::cerr, ossErr.str());
                ERR_MSG (ossThrow, ossErr.str());
                throw std::runtime_error (ossThrow.str().c_str());
            }

        }

        return ret;

    }


// ------------------
    BigInt::Vin BigInt::Vin::operator* (BigInt::Ulong i_arg) const
    {
        BigInt::Vin ret (0);
        for (std::size_t i = 0; i < i_arg; i++)
        {
            ret = ret + (*this);
        }
        return ret;
    }


// ------------------
    bool BigInt::Vin::operator< (const BigInt::Vin& i_arg) const
    {
        if (m_units.size() < i_arg.m_units.size())
        {
            return true;
        }
        if (i_arg.m_units.size() < m_units.size())
        {
            return false;
        }

        ASSERTION (m_units.size() == i_arg.m_units.size());
        for (std::size_t i = (m_units.size() - 1); i > 0; i--)
        {
            if (m_units[i] < i_arg.m_units[i])
            {
                return true;
            }

            if (i_arg.m_units[i] < m_units[i])
            {
                return false;
            }
        }
        return (m_units[0] < i_arg.m_units[0]);
    }


// ------------------
    bool BigInt::Vin::operator<= (const BigInt::Vin& i_arg) const
    {
        if (*this < i_arg)
        {
            return true;
        }

        if (*this == i_arg)
        {
            return true;
        }

        return false;
    }



// ------------------
    bool BigInt::Vin::operator> (const BigInt::Vin& i_arg) const
    {
        return (!(*this <= i_arg));
    }


// ------------------
    bool BigInt::Vin::operator>= (const BigInt::Vin& i_arg) const
    {
        return (!(*this < i_arg));
    }

// ------------------
    bool BigInt::Vin::operator== (const BigInt::Vin& i_arg) const
    {
        if (*this < i_arg)
        {
            return false;
        }

        if (i_arg < *this)
        {
            return false;
        }

        return true;
    }

// ------------------
    bool BigInt::Vin::operator!= (const BigInt::Vin& i_arg) const
    {
        return (!(*this == i_arg));
    }


// ------------------
    std::string BigInt::Vin::toStrHex (const std::string& i_prefix) const
    {
        std::ostringstream oss;

        BigInt::Rossi rossi (toStrDec(), BigInt::DEC_DIGIT);

        oss << i_prefix
            << rossi.toStrHex();

        return oss.str();
    }


// ------------------
    std::string BigInt::Vin::toStr0xHex () const
    {
        return toStrHex("0x");
    }


// ------------------
    std::string BigInt::Vin::toStrDec (const std::string& i_prefix) const
    {
        std::ostringstream oss;

        if (isEmpty ())
        {
            return "isEmpty";
        }

        oss << i_prefix;

        for (std::size_t i = (m_units.size() - 1); i; --i)
        {
            oss << m_units [i]
                << std::setw (BigInt::BASE1 - 1)
                << std::setfill ('0');
        }
        oss << m_units [0];

        return oss.str();
    }


// ------------------
    std::ostream& operator<< (std::ostream& o_os, const BigInt::Vin& i_ins)
    {
        return o_os << i_ins.toStrDec();
    }


// ==========================================
    void BigInt::Vin::showUnits(std::ostream& o_stream) const
    {
        std::ostringstream oss;

        BigInt::Rossi rossi(*this);

        oss << std::endl;
        oss << ""
            << "BigInt::Vin value"
            << ": "
            << rossi.toStr0xHex()
            << " = "
            << this->toStrDec()
            << std::endl;

        BigInt::BaseBigInt::showUnits(oss);

        oss << std::endl;

        o_stream << std::flush << oss.str() << std::flush;
    }


// ===================
// class BigInt::Rossi
// ===================


// ------------------
// Constructor-0
    BigInt::Rossi::Rossi ()
    {
        ASSERTION(isEmpty());
    }


// ------------------
// Constructor-1
    BigInt::Rossi::Rossi (BigInt::Ulong i_unit)
    {
        ASSERTION (m_units.empty());
        m_units.push_back (i_unit);
    }


// ------------------
// Constructor-2
    BigInt::Rossi::Rossi (const std::string& i_arg, std::size_t i_digitBase)
    {
        const bool rc = initViaString (i_arg, i_digitBase);
        ASSERTION (rc);
    }



// ------------------
// Constructor-3
    BigInt::Rossi::Rossi (const BigInt::Vin& i_arg)
    {
        std::ostringstream oss;
        oss << i_arg;
        const bool rc = initViaString (oss.str(), DEC_DIGIT);
        ASSERTION (rc);
    }


// ------------------
// Constructor-4
    BigInt::Rossi::Rossi (const std::size_t i_noOfUnits, BigInt::Ulong i_internalUnits, BigInt::Ulong i_backUnit)
    {
        ASSERTION (m_units.empty());

        try
        {
            m_units.resize (i_noOfUnits, i_internalUnits);
        }
        catch(...)
        {
            std::ostringstream ossThrow;
            std::ostringstream ossErr;
            ossErr  << ""
                    << "Unable to to do m_units.resize("
                    << i_noOfUnits
                    << ")"
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
        }

        m_units.back() = i_backUnit;
    }



// ------------------
    BigInt::Ulong BigInt::Rossi::toUlong () const
    {
        ASSERTION (!m_units.empty());
        if (m_units.size() > 1)
        {
            std::ostringstream ossErr;
            ossErr  << ""
                    << "BigInt::Ulong can't be obtained: m_units.size() too big"
                    << std::endl
                    << "m_units.size() = "
                    << m_units.size()
                    << std::endl
                    << "Hex value = "
                    << toStrHex()
                    << std::endl
                    << "Here m_units.size() must be = 1"
                    << std::endl
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());

            ASSERTION (m_units.size() == 1);
        }
        ASSERTION (m_units.size() == 1);
        return m_units.front();
    }

// ------------------
// BigInt::Rossi::operator BigInt::Ulong () const
// {
//  return toUlong();
// }


// ------------------
    bool BigInt::Rossi::initViaString (const std::string& i_arg, std::size_t i_digitBase)
    {
        ASSERTION (m_units.empty());
        ASSERTION ((i_digitBase == BigInt::HEX_DIGIT) || (i_digitBase == BigInt::DEC_DIGIT));

        m_units.push_back(0);

        for (std::size_t i = 0; i < i_arg.size(); i++)
        {
            switch (i_digitBase)
            {
            case DEC_DIGIT:
                if (!isdigit (i_arg[i]))
                {
                    std::ostringstream ossErr;
                    ossErr  << ""
                            << "std::string contains non-decimal digit"
                            << std::endl
                            << "std::string = "
                            << i_arg
                            << std::endl
                            << i
                            << "-th char = "
                            << i_arg[i]
                            << std::endl
                            << std::endl;

                    ERR_MSG (std::cerr, ossErr.str());

                    ASSERTION (0);
                    return false; // unused
                }
                break;

            case HEX_DIGIT:
                if (!isxdigit (i_arg[i]))
                {
                    std::ostringstream ossErr;
                    ossErr  << ""
                            << "std::string contains non-hexadecimal digit"
                            << std::endl
                            << "std::string = "
                            << i_arg
                            << std::endl
                            << i
                            << "-th char = "
                            << i_arg[i]
                            << std::endl
                            << std::endl;

                    ERR_MSG (std::cerr, ossErr.str());
                    ASSERTION (0);
                    return false; // unused
                }
                break;

            default:
                ASSERTION (0);
                return false;
                break; // unused

            }
        }

        for (std::size_t i = 0; i < i_arg.size(); i++)
        {
            *this = (*this) * static_cast<BigInt::Ulong>(i_digitBase)
                    +
                    BigInt::Rossi (static_cast<BigInt::Ulong>(ms_hex2dec[i_arg[i]]));
        }
        return true;
    }


// ------------------
    void BigInt::Rossi::resizeUnits(std::size_t i_size)
    {
        m_units.resize(i_size);
    }

// ------------------
    void BigInt::Rossi::truncateUnits()
    {
        while ((m_units.size() > 1) && (m_units.back() == 0))
        {
            m_units.pop_back();
        }
        ASSERTION(!isEmpty());

    }


// ------------------
    void BigInt::Rossi::smartTruncateUnits()
    {
        if (backUnitIsNull ())
        {
            truncateUnits ();
        }
    }

// ------------------
    bool BigInt::Rossi::backUnitIsNull () const
    {
        ASSERTION (!m_units.empty());
        if (m_units.size() == 1)
        {
            return false;
        }
        return (m_units.back() == 0);
    }



// ------------------
// BigInt::Rossi& BigInt::Rossi::operator= (BigInt::Ulong i_arg)
// {
//  *this = BigInt::Rossi (i_arg);
//  return *this;
// }

// ------------------
    BigInt::Rossi BigInt::Rossi::operator+ (const BigInt::Rossi& i_arg)
    {
        BigInt::Rossi ret (0);
        BigInt::Rossi arg (i_arg);
        BigInt::Ulong carry = 0;

        const std::size_t maxSize (std::max (getUnitsSize(), arg.getUnitsSize()));

        resizeUnits (maxSize + 1);
        arg.resizeUnits (maxSize + 1);
        ret.resizeUnits (maxSize + 1);

        for (std::size_t i = 0; i < m_units.size(); i++)
        {
            ret.m_units[i] = m_units[i] + arg.m_units[i] + carry;
            if (carry == 0)
            {
                carry = ((ret.m_units[i] < m_units[i] || ret.m_units[i] < arg.m_units[i]) ? 1 : 0);
            }
            else
            {
                carry = ((ret.m_units[i] <= m_units[i] || ret.m_units[i] <= arg.m_units[i]) ? 1 : 0);
            }
        }

        smartTruncateUnits();
        ret.smartTruncateUnits();

        return ret;

    }

// ------------------
    BigInt::Rossi BigInt::Rossi::operator+ (BigInt::Ulong i_arg)
    {
        return (*this + BigInt::Rossi (i_arg));
    }


// ------------------
    bool BigInt::Rossi::operator< (const BigInt::Rossi& i_arg) const
    {
        if (m_units.size() < i_arg.m_units.size())
        {
            return true;
        }

        if (i_arg.m_units.size() < m_units.size())
        {
            return false;
        }

        ASSERTION (m_units.size() == i_arg.m_units.size());
        for (std::size_t i = (m_units.size() - 1); i > 0; i--)
        {
            if (m_units[i] < i_arg.m_units[i])
            {
                return true;
            }

            if (i_arg.m_units[i] < m_units[i])
            {
                return false;
            }
        }
        return (m_units[0] < i_arg.m_units[0]);
    }


// ------------------
    bool BigInt::Rossi::operator<= (const BigInt::Rossi& i_arg) const
    {
        if (*this < i_arg)
        {
            return true;
        }

        if (*this == i_arg)
        {
            return true;
        }

        return false;
    }



// ------------------
    bool BigInt::Rossi::operator> (const BigInt::Rossi& i_arg) const
    {
        return (!(*this <= i_arg));
    }


// ------------------
    bool BigInt::Rossi::operator>= (const BigInt::Rossi& i_arg) const
    {
        return (!(*this < i_arg));
    }

// ------------------
    bool BigInt::Rossi::operator== (const BigInt::Rossi& i_arg) const
    {
        if (*this < i_arg)
        {
            return false;
        }

        if (i_arg < *this)
        {
            return false;
        }

        return true;
    }


// ------------------
    bool BigInt::Rossi::operator!= (const BigInt::Rossi& i_arg) const
    {
        return (!(*this == i_arg));
    }



// ------------------
    BigInt::Rossi BigInt::Rossi::operator/ (const BigInt::Rossi& i_arg) const
    {
        return divide(*this, i_arg, NULL);
    }


// ------------------
    BigInt::Rossi BigInt::Rossi::operator% (const BigInt::Rossi& i_arg) const
    {
        BigInt::Rossi ret;
        divide(*this, i_arg, &ret);
        return ret;
    }



// ------------------
    BigInt::Rossi BigInt::Rossi::operator>> (std::size_t i_shift)
    {
        BigInt::Rossi tmp;
        BigInt::Rossi ret;

        tmp = *this;

        ret.resizeUnits (m_units.size());

        ASSERTION (ret.getUnitsSize() == tmp.getUnitsSize());

        for (std::size_t i = 0; i < i_shift; i++)
        {
            ret.m_units.back() = (tmp.m_units.back() >> 1);

            for (std::size_t j1 = tmp.m_units.size(); j1 ; j1--)
            {
                const std::size_t j = j1 -1;
                ret.m_units[j] = (tmp.m_units[j] >> 1);

                if ((tmp.m_units[j + 1] & 1) != 0)
                {
                    ret.m_units[j] |= ULONG_MSB;      // Set MSB bit
                }
            }
            tmp = ret;
        }

        smartTruncateUnits();
        ret.smartTruncateUnits();

        return ret;
    }


// ------------------
    BigInt::Rossi BigInt::Rossi::operator<< (std::size_t i_shift)
    {
        BigInt::Rossi tmp;
        BigInt::Rossi ret;

        tmp = *this;

        ret.resizeUnits (m_units.size() + 1);

        ASSERTION ((ret.getUnitsSize() +1 ) == tmp.getUnitsSize());

        for (std::size_t i = 0; i < i_shift; i++)
        {
            ret.m_units.front() = (tmp.m_units.front() << 1);
            for (std::size_t j = 1; j < tmp.m_units.size(); j++)
            {
                ret.m_units[j] = (tmp.m_units[j] << 1);
                if ((tmp.m_units[j-1] & ULONG_MSB) != 0)
                {
                    ret.m_units[j] |= 1;     // Set MSB bit
                }
            }
            if ((tmp.m_units.back() & ULONG_MSB) != 0)
            {
                ret.m_units.back() |= 1;   // Set MSB bit
            }
            tmp = ret;
        }

        smartTruncateUnits();
        ret.smartTruncateUnits();

        return ret;
    }



// ------------------
    BigInt::Rossi& BigInt::Rossi::operator>>= (std::size_t i_shift)
    {
        BigInt::Ulong carry;
        m_units.push_back(0);

        for (std::size_t i = 0; i < i_shift; i++)
        {
            carry = m_units.back() & 1;
            m_units.back() >>= 1;

            for (std::size_t j1 = m_units.size(); j1; j1--)
            {
                const std::size_t j = j1 -1;
                if (carry)
                {
                    carry = m_units[j] & 1;
                    m_units[j] >>= 1;
                    m_units[j] |= ULONG_MSB;
                }
                else
                {
                    carry = m_units[j] & 1;
                    m_units[j] >>= 1;
                }
            }
        }

        smartTruncateUnits();

        return *this;
    }



// ------------------
    BigInt::Rossi& BigInt::Rossi::operator<<= (std::size_t i_shift)
    {
        BigInt::Ulong carry;

        const std::size_t pushBackSize (i_shift/(sizeof (std::size_t) * CHAR_BIT) + 1);

        for (std::size_t i = 0; i < (pushBackSize + 1); i++)
        {
            try
            {
                m_units.push_back(0);
            }
            catch(...)
            {
                std::ostringstream ossThrow;
                std::ostringstream ossErr;
                ossErr  << ""
                        << "Unable to to do m_units.push_back()"
                        << "; "
                        << "m_units.size() = "
                        << m_units.size()
                        << std::endl;

                ERR_MSG (std::cerr, ossErr.str());
                ERR_MSG (ossThrow, ossErr.str());
                throw std::runtime_error (ossThrow.str().c_str());
            }
        }

        for (std::size_t i = 0; i < i_shift; i++)
        {
            carry = m_units.front() & ULONG_MSB;
            m_units.front() <<= 1;

            for (std::size_t j = 1; j < m_units.size(); j++)
            {
                if (carry)
                {
                    carry = m_units[j] & ULONG_MSB;
                    m_units[j] <<= 1;
                    m_units[j] |= 1;
                }
                else
                {
                    carry = m_units[j] & ULONG_MSB;
                    m_units[j] <<= 1;
                }
            }
        }

        smartTruncateUnits();

        return *this;
    }



// ------------------
    BigInt::Rossi& BigInt::Rossi::operator+=(const BigInt::Rossi& i_arg)
    {
        BigInt::Ulong carry = 0;
        BigInt::Ulong prevDigit;
        BigInt::Rossi arg (i_arg);

        const std::size_t maxSize (std::max (getUnitsSize(), arg.getUnitsSize()));

        resizeUnits (maxSize + 1);
        arg.resizeUnits (maxSize + 1);

        for (std::size_t i = 0; i < m_units.size(); i++)
        {
            prevDigit = m_units[i];
            m_units[i] = m_units[i] + arg.m_units[i] + carry;
            if (carry == 0)
            {
                carry = ((m_units[i] < prevDigit || m_units[i] < arg.m_units[i]) ? 1 : 0);
            }
            else
            {
                carry = ((m_units[i] <= prevDigit || m_units[i] <= arg.m_units[i]) ? 1 : 0);
            }
        }

        smartTruncateUnits();
        return *this;
    }


// ------------------
    BigInt::Rossi& BigInt::Rossi::operator++()  // Pre Increment operator -- faster than add
    {
        m_units.push_back(0);

        m_units.front()++;
        for (std::size_t i = 1; i < m_units.size(); i++)
        {
            if (m_units[i-1])
            {
                break;
            }
            m_units[i]++;
        }

        smartTruncateUnits();

        return *this;
    }


// ------------------
    BigInt::Rossi BigInt::Rossi::operator++ (int)  // Post Increment operator -- faster than add
    {
        BigInt::Rossi tmp (*this);
        ++*this;
        return tmp;
    }




// ------------------
    BigInt::Rossi BigInt::Rossi::operator- ()  // Negates a number
    {
        BigInt::Rossi ret;
        ret.resizeUnits(m_units.size() + 1);

        for (std::size_t i = 0; i < m_units.size(); i++)
        {
            ret.m_units[i] = ~m_units[i];
        }

        ret = ret + RossiOne;

        smartTruncateUnits();
        ret.smartTruncateUnits();

        return ret;
    }



// ------------------
    BigInt::Rossi BigInt::Rossi::operator-(const BigInt::Rossi& i_arg)
    {
        BigInt::Rossi ret (RossiZero);
        BigInt::Rossi arg (i_arg);
        BigInt::Ulong borrow = 0;

        const std::size_t maxSize (std::max (getUnitsSize(), arg.getUnitsSize()));

        resizeUnits (maxSize + 1);
        arg.resizeUnits (maxSize + 1);
        ret.resizeUnits (maxSize + 1);

        if (*this < arg)
        {
            std::ostringstream ossErr;
            ossErr  << ""
                    << "minuend < subtracter"
                    << std::endl
                    << "minuend    = "
                    << this->toStrHex()
                    << "subtracter = "
                    << arg.toStrHex()
                    << std::endl
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ASSERTION (0);
        }

        for (std::size_t i = 0; i < m_units.size(); i++)
        {
            ret.m_units[i] = m_units[i] - arg.m_units[i] - borrow;

            if (borrow == 0)
            {
                borrow = ((m_units[i] < arg.m_units[i]) ? 1 : 0);
            }
            else
            {
                borrow = ((m_units[i] <= arg.m_units[i]) ? 1 : 0);
            }
        }

        smartTruncateUnits();
        ret.smartTruncateUnits();

        return ret;
    }


// ------------------
    BigInt::Rossi& BigInt::Rossi::operator-= (const BigInt::Rossi& i_arg)
    {
        BigInt::Ulong borrow = 0;
        BigInt::Rossi arg (i_arg);
        BigInt::Ulong prevDigit;

        const std::size_t maxSize (std::max (getUnitsSize(), arg.getUnitsSize()));
        resizeUnits (maxSize + 1);
        arg.resizeUnits (maxSize + 1);

        if (*this < arg)
        {
            std::ostringstream ossErr;
            ossErr  << ""
                    << "minuend < subtracter"
                    << std::endl
                    << "minuend    = "
                    << this->toStrHex()
                    << "subtracter = "
                    << arg.toStrHex()
                    << std::endl
                    << std::endl;

            ERR_MSG (std::cerr, ossErr.str());
            ASSERTION (0);
        }

        for (std::size_t i = 0; i < m_units.size(); i++)
        {
            prevDigit = m_units[i];
            m_units[i] = m_units[i] - arg.m_units[i] - borrow;

            if (borrow == 0)
            {
                borrow = ((prevDigit < arg.m_units[i]) ? 1 : 0);
            }
            else
            {
                borrow = ((prevDigit <= arg.m_units[i]) ? 1 : 0);
            }
        }

        smartTruncateUnits();

        return *this;
    }




// ------------------
    BigInt::Rossi& BigInt::Rossi::operator--()  // Pre Decrement operator -- faster than add
    {
        m_units.front()--;
        for (std::size_t i = 1; i < m_units.size(); i++)
        {
            if (m_units[i-1] != ULONG_MAX)
            {
                break;
            }
            ASSERTION (m_units[i-1] < ULONG_MAX);

            m_units[i]--;
        }

        smartTruncateUnits();
        return *this;
    }


// ------------------
    BigInt::Rossi BigInt::Rossi::operator-- (int)  // Post Decrement operator -- faster than add
    {
        BigInt::Rossi tmp (*this);
        --*this;
        return tmp;
    }



// ------------------
    BigInt::Rossi BigInt::Rossi::operator& (const BigInt::Rossi& i_arg)
    {
        const std::size_t maxSize (std::max (getUnitsSize (), i_arg.getUnitsSize ()));

        BigInt::Rossi ret;
        BigInt::Rossi arg (i_arg);

        ret.resizeUnits(maxSize);
        arg.resizeUnits(maxSize);
        resizeUnits(maxSize);

        for (std::size_t i1 = m_units.size(); i1; i1--)
        {
            const std::size_t i = i1 - 1;
            ret.m_units[i] = m_units[i] & arg.m_units[i];
        }

        smartTruncateUnits();
        ret.smartTruncateUnits();

        return ret;
    }


// ------------------
    BigInt::Rossi BigInt::Rossi::operator| (const BigInt::Rossi& i_arg)
    {
        const std::size_t maxSize (std::max (getUnitsSize (), i_arg.getUnitsSize ()));

        BigInt::Rossi ret;
        BigInt::Rossi arg (i_arg);

        ret.resizeUnits(maxSize);
        arg.resizeUnits(maxSize);
        resizeUnits(maxSize);

        for (std::size_t i1 = m_units.size(); i1; i1--)
        {
            const std::size_t i = i1 - 1;
            ret.m_units[i] = m_units[i] | arg.m_units[i];
        }


        smartTruncateUnits();
        ret.smartTruncateUnits();

        return ret;
    }



// ------------------
    BigInt::Rossi BigInt::Rossi::operator^ (const BigInt::Rossi& i_arg)
    {
        const std::size_t maxSize (std::max (getUnitsSize (), i_arg.getUnitsSize ()));

        BigInt::Rossi ret;
        BigInt::Rossi arg (i_arg);

        ret.resizeUnits(maxSize);
        arg.resizeUnits(maxSize);
        resizeUnits(maxSize);

        for (std::size_t i1 = m_units.size(); i1; i1--)
        {
            const std::size_t i = i1 - 1;
            ret.m_units[i] = m_units[i] ^ arg.m_units[i];
        }


        smartTruncateUnits();
        ret.smartTruncateUnits();

        return ret;

    }


// ------------------
    BigInt::Rossi BigInt::Rossi::operator~ ()
    {
        BigInt::Rossi ret;

        ret.resizeUnits(getUnitsSize());

        for (std::size_t i1 = m_units.size(); i1; i1--)
        {
            const std::size_t i = i1 - 1;
            ret.m_units[i] = ~m_units[i];
        }


        smartTruncateUnits();
        ret.smartTruncateUnits();

        return ret;

    }



// ------------------
    BigInt::Rossi& BigInt::Rossi::operator&= (const BigInt::Rossi& i_arg)
    {
        const std::size_t maxSize (std::max  (getUnitsSize (), i_arg.getUnitsSize ()));

        BigInt::Rossi arg (i_arg);

        arg.resizeUnits(maxSize);
        resizeUnits(maxSize);

        for (std::size_t i1 = m_units.size(); i1; i1--)
        {
            const std::size_t i = i1 - 1;
            m_units[i] = m_units[i] & arg.m_units[i];
        }

        smartTruncateUnits();

        return *this;

    }


// ------------------
    BigInt::Rossi& BigInt::Rossi::operator|=(const BigInt::Rossi& i_arg)
    {
        const std::size_t maxSize (std::max (getUnitsSize (), i_arg.getUnitsSize ()));

        BigInt::Rossi arg (i_arg);

        arg.resizeUnits(maxSize);
        resizeUnits(maxSize);

        for (std::size_t i1 = m_units.size(); i1; i1--)
        {
            const std::size_t i = i1 - 1;
            m_units[i] = m_units[i] | arg.m_units[i];
        }

        smartTruncateUnits();

        return *this;

    }


// ------------------
    BigInt::Rossi& BigInt::Rossi::operator^=(const BigInt::Rossi& i_arg)
    {
        const std::size_t maxSize (std::max  (getUnitsSize (), i_arg.getUnitsSize ()));

        BigInt::Rossi arg (i_arg);

        arg.resizeUnits(maxSize);
        resizeUnits(maxSize);

        for (std::size_t i1 = m_units.size(); i1; i1--)
        {
            const std::size_t i = i1 - 1;
            m_units[i] = m_units[i] ^ arg.m_units[i];
        }

        smartTruncateUnits();
        return *this;

    }




// ------------------
    BigInt::Rossi BigInt::Rossi::operator* (BigInt::Rossi i_arg) const
    {
        BigInt::Rossi tmp;
        BigInt::Rossi ret;


        tmp = *this;
        ret = RossiZero;
        // ret.resizeUnits (getUnitsSize () + i_arg.getUnitsSize ());

        do
        {
            if ((i_arg.m_units.front() & 1) != 0)
            {
                ret += tmp;
            }
            i_arg >>= 1;
            tmp <<= 1;
        } while (i_arg != RossiZero);

        const_cast<BigInt::Rossi*>(this)->smartTruncateUnits();
        ret.smartTruncateUnits();

        return ret;
    }


// ------------------
    BigInt::Rossi BigInt::Rossi::operator* (BigInt::Ulong i_arg) const
    {
        return ((*this) * BigInt::Rossi (i_arg));
    }



// ------------------
    BigInt::Rossi BigInt::Rossi::divide (
        const BigInt::Rossi& i_dividend,
        const BigInt::Rossi& i_divisor,
        BigInt::Rossi*       o_remainder
        ) const
    {
        BigInt::Rossi dividend (i_dividend);
        BigInt::Rossi divisor (i_divisor);


        long shiftcnt (0);

        // --- Check for attempt to divide by zero ---
        if (divisor == RossiZero)
        {
            ERR_MSG (std::cerr, "divisor == Zero");
            ASSERTION (0 && "divisor == Zero");

            shiftcnt = 1 / shiftcnt;  // Force a divide by zero exception. (shiftcnt=0)
        }

        BigInt::Rossi quotient (RossiZero);

        quotient.resizeUnits (dividend.getUnitsSize ());

        if (o_remainder != NULL)
        {
            o_remainder->resizeUnits (dividend.getUnitsSize ());
        }


        // --- Left shift divisor until it is greater than or equal to dividend ---
        // while ( divisor < dividend && ((divisor.m_units.back() & ULONG_MSB) == 0))
        while ( divisor < dividend)
        {
            divisor <<= 1;
            shiftcnt++;
        }

        if (divisor > dividend)      // If divisor is greater than dividend, right shift divisor
        {
            divisor >>= 1;
            shiftcnt--;
        }

        if (shiftcnt >= 0)
        {
            for(long i = 0; i <= shiftcnt; i++)
            {
                if ( divisor <= dividend)  // If divisor is greater than dividend, then the bit is a 1
                {
                    dividend -= divisor;     // Subtract divisor from dividend
                    divisor  >>= 1;          // Right shift divisor
                    quotient <<= 1;          // Left shift quotient
                    quotient++;              // Increment quotient           // Increment quotient
                }
                else
                {
                    divisor >>= 1;             // Bit is 0, right shift divisor, left shift quotient
                    quotient <<= 1;
                }
            }
        }

        BigInt::Rossi remainder (dividend);
        remainder.smartTruncateUnits ();

        if (o_remainder != NULL)
        {
            *o_remainder = remainder;
            o_remainder->smartTruncateUnits ();
        }

        quotient.smartTruncateUnits ();

        ASSERTION ((quotient * i_divisor + remainder) == i_dividend);
        return quotient;
    }



// ------------------
    BigInt::Rossi BigInt::Rossi::sqrt()		// Returns the square root of this
    {
        BigInt::Rossi ret;
        BigInt::Rossi tmp;
        BigInt::Rossi delta;

        BigInt::Rossi mask (RossiTwo);

        tmp = *this;
        mask = -mask;

        std::size_t i = 0;
        ret = tmp;
        for (i = 0; ret != RossiZero; ret >>= 1, i++)
        {
            // Do nothing
        }

        ret = tmp;
        for (std::size_t j = 0; j < std::size_t(i >> 1); ret >>= 1, j++)
        {
            // Do nothing
        }

        do
        {
            // -----------------------------------------------
            // We are really performing the fuction:
            //	 delta = (tmp/ret - ret) / 2;
            //   below, but since these are unsigned numbers,
            //   we MUST do the subtraction last in order for
            //   the ret = ret + delta;  equation to work properly.
            // -----------------------------------------------

            delta = ( tmp >> BigInt::Ulong(1)) / ret - (ret >> BigInt::Ulong(1));
            ret = ret + delta;
        } while ((delta &= mask) != RossiZero);

        return ret;
    }



// ------------------
    std::string BigInt::Rossi::toStrHex (const std::string& i_prefix) const
    {
        const std::size_t HEX_SETW = sizeof(BigInt::Ulong) * 2;
        std::ostringstream oss;

        if (m_units.empty())
        {
            oss << i_prefix
                << std::hex
                << static_cast<BigInt::Ulong>(0)
                << std::dec;

            return oss.str();
        }

        ASSERTION (!m_units.empty());

        // --------------
        oss << i_prefix
            << std::hex
            << m_units.back();

        for (std::size_t i1 = (m_units.size() - 1); i1; i1--)
        {
            const std::size_t i = i1 - 1;
            oss << std::setw (HEX_SETW)
                << std::setfill ('0')
                << m_units [i];
        }
        oss << std::dec;
        return oss.str();
    }


// ------------------
    std::string BigInt::Rossi::toStr0xHex () const
    {
        return toStrHex("0x");
    }


// ------------------
    std::string BigInt::Rossi::toStrDec (const std::string& i_prefix) const
    {
        std::ostringstream oss;

        BigInt::Vin vin (toStrHex(), BigInt::HEX_DIGIT);

        oss << vin;

        return oss.str();
    }


// ==========================================
    void BigInt::Rossi::showUnits(std::ostream& o_stream) const
    {
        std::ostringstream oss;

        oss << std::endl;
        oss << ""
            << "BigInt::Rossi value"
            << ": "
            << this->toStr0xHex()
            << " = "
            << this->toStrDec()
            << std::endl;


        BigInt::BaseBigInt::showUnits(oss);

        oss << std::endl;

        o_stream << std::flush << oss.str() << std::flush;
    }



// ------------------
    std::ostream& operator<< (std::ostream& o_os, const BigInt::Rossi& i_ins)
    {
        return o_os << i_ins.toStrDec();
        //return o_os << i_ins.toStr0xHex();
    }

    int rossiLog( const BigInt::Rossi & bint)
    {
        BigInt::Rossi tmp(bint);
        try {
            if ( tmp <= BigInt::Rossi( 0 ))
                throw std::exception();
            int count=0;
            while ( tmp > BigInt::Rossi( 0 ))
            {
                tmp>>=1;
                count++;
            }
            return count-1;
        }catch (...)
        {
            std::cerr<<"Log operator error"<<std::endl;
            exit(1);
        }
    }
}
