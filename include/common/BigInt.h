/**
 * @file   BigInt.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Aug 18 17:40:08 2010
 *
 * @brief  BigInt for Kadpeer P2P framework
 *
 *
 */

#ifndef   __BIG_INT_H__
#define   __BIG_INT_H__

// include section
#include <cassert>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <iomanip>
#include <utility>
#include <climits>
#include <algorithm>
#include <stdexcept>
#include <limits>


// ========================================
// BOOST
// --------------------------
// #define BIG_INT_USES_BOOST
// --------------------------
#ifdef BIG_INT_USES_BOOST
#include <boost/static_assert.hpp>
#endif
// ========================================
// ========== FUNCTION_NAME MACROS (BEGIN) =========


#ifdef FUNCTION_NAME
#undef FUNCTION_NAME
#endif

#ifdef UNDECORATED_FUNCTION_NAME
#undef UNDECORATED_FUNCTION_NAME
#endif

// --------------------------------------

// --------------------------------------
#if defined _MSC_VER    // Windows
// #define FUNCTION_NAME __FUNCSIG__  // too long and not friendly representation
#define FUNCTION_NAME               __FUNCTION__
#define UNDECORATED_FUNCTION_NAME   __FUNCTION__
#elif defined __GNUC__  // g++ GNU C++
#define FUNCTION_NAME               __PRETTY_FUNCTION__
#define UNDECORATED_FUNCTION_NAME   __FUNCTION__
#elif defined __HP_aCC  // aCC on HP-UX
#define FUNCTION_NAME               __PRETTY_FUNCTION__
#define UNDECORATED_FUNCTION_NAME   __FUNCTION__
#elif defined __xlC__   // xlC on IBM AIX
#define FUNCTION_NAME               __FUNCTION__
#define UNDECORATED_FUNCTION_NAME   __func__
#elif defined __SUNPRO_CC   // SUN CC
#define FUNCTION_NAME               BOOST_CURRENT_FUNCTION  // Must be compiled with option  -features=extensions
#define UNDECORATED_FUNCTION_NAME   __func__                // Must be compiled with option  -features=extensions
#elif defined __INTEL_COMPILER  // Intel C/C++
#define FUNCTION_NAME               __PRETTY_FUNCTION__
#define UNDECORATED_FUNCTION_NAME   __FUNCTION__
#else
#define FUNCTION_NAME               "Unable to detect FUNCTION_NAME"
#define UNDECORATED_FUNCTION_NAME   "Unable to detect UNDECORATED_FUNCTION_NAME"
#endif

// ========== FUNCTION_NAME MACROS (END) ===========
// =================================================


// ===========================================
// ========== DISPLAY MACROS (BEGIN) =========

#define PRE_MSG(s, p)       s << p << "[" <<  __FILE__ << ", #" << std::dec << std::setw(3) << __LINE__ << "; " << UNDECORATED_FUNCTION_NAME << "] " << std::flush
#define SCREEN_MSG(s,p,x,t) { std::ostringstream oo_ss_ss; PRE_MSG(oo_ss_ss,p) << x << t << std::endl; s << std::flush << oo_ss_ss.str() << std::flush; }
#define FATAL_MSG(s, t)     SCREEN_MSG (s, "FATAL   ", "", t)
#define ERR_MSG(s, t)       SCREEN_MSG (s, "ERROR   ", "", t)
#define WARN_MSG(s, t)      SCREEN_MSG (s, "WARNING ", "", t)
#define INFO_MSG(s, t)      SCREEN_MSG (s, "INFO    ", "", t)
#define SUCCESS_MSG(s, t)   SCREEN_MSG (s, "SUCCESS ", "", t)

// ===========================================
// ========= ASSERTION MACROS (BEGIN) ========
#define ASSERTION(x)     assert(x);             \
    if (!(x))                                   \
    {                                           \
        std::cerr   << "["                      \
                    << __FILE__                 \
                    << ", "                     \
                    << __LINE__                 \
                    << "] assert("              \
                    << #x                       \
                    << ") not working"          \
                    << std::endl                \
                    << std::flush;              \
        abort();                                \
    }
// ========== ASSERTION MACROS (END) =========
// ===========================================




namespace  BigInt
{
    // -------------------
    // FORWARD DECLARATION
    // -------------------
    class Rossi;


    // --------
    // TYPEDEFS
    // --------
    typedef unsigned long Ulong;


    // ----------------
    // STATIC CONSTANTS
    // ----------------
    static const std::size_t BIG_INT_MAJOR_VERSION = 4;
    static const std::size_t BIG_INT_MINOR_VERSION = 0;

    static const std::size_t DEC_DIGIT          = 10;
    static const std::size_t HEX_DIGIT          = 16;
    static const std::size_t NIBBLE_BIT         = 4;
    static const std::size_t ULONG_HEX_DIGITS   = ((sizeof (Ulong) * CHAR_BIT)/NIBBLE_BIT);


    static const Ulong MAX_UNIT_VALUE   = (ULONG_MAX >> 2);
    static const Ulong ULONG_MSB        = static_cast<Ulong>(1) << (sizeof(Ulong) * CHAR_BIT - 1);

    static const Ulong BASE1        = 10;
    static const Ulong BASE2        = 1000000000;  // // BASE1 ** (BASE1 - 1)
    static const Ulong SUB_BASE2    = BASE2 - 1;  // 999999999
    static const Ulong OVER_BASE2   = 0xc0000000;  // OVER_BASE2 > SUB_BASE2


    // -----------------------------------------
#ifdef BIG_INT_USES_BOOST
    BOOST_STATIC_ASSERT (SUB_BASE2 == 999999999);
    BOOST_STATIC_ASSERT (!(BASE2 >= MAX_UNIT_VALUE));
    BOOST_STATIC_ASSERT (BASE1 * (BASE2/BASE1 + 1) < MAX_UNIT_VALUE);
    BOOST_STATIC_ASSERT (!(BASE2 != (SUB_BASE2 + 1)));
    BOOST_STATIC_ASSERT (OVER_BASE2 > SUB_BASE2);

    BOOST_STATIC_ASSERT(
        (sizeof(Ulong) == 4) && ((ULONG_MSB == static_cast<Ulong>(0x80000000)))
        ||
        (sizeof(Ulong) == 8) && ((ULONG_MSB == ((static_cast<Ulong>(0x80000000) << 31) << 1)))
        );
#endif
    // -----------------------------------------
    // Public Methods
    static void assertCheck();
    static void showVersion(std::ostream& o_stream);

    // -----------------------------------
    template <typename T>
    static std::string toString(const T& t)
    {
        std::ostringstream oss;
        oss << t;
        return oss.str();
    }

    // -------------------------------
    template<typename T, std::size_t N>
    static std::size_t getArrayElems(const T(&)[N])
    {
        return N;
    }

    // -------------------------------
    template<typename T, std::size_t N>
    static std::size_t getArrayElems(T(&)[N])
    {
        return N;
    }

    // -------------------------------
    template<typename T, std::size_t N>
    static std::size_t getArraySizeInBytes(const T(&a)[N])
    {
        return getArrayElems(a) * sizeof (T);
    }

    // -------------------------------
    template<typename T, std::size_t N>
    static std::size_t getArraySizeInBytes(T(&a)[N])
    {
        return getArrayElems(a) * sizeof (T);
    }


    // -------------------------------
    template<typename T, std::size_t N>
    static std::vector<T> array2vector(const T(&a)[N])
    {
        return std::vector<T> (a, a + getArrayElems (a));
    }

    // --------------------------------
    template<typename T, std::size_t N>
    static std::vector<T> array2vector(T(&a)[N])
    {
        return std::vector<T> (a, a + getArrayElems (a));
    }

    // ------------------------------------
    template<typename T>
    static std::vector<T> elem2vector(const T& e)
    {
        return std::vector<T> (&e, &e + 1);
    }

    // ------------------------------------
    template<typename T>
    static std::vector<T> elem2vector(T& e)
    {
        return std::vector<T> (&e, &e + 1);
    }

    // ---------------------------------------------------------
    template<typename K, typename T, std::size_t N>
    static std::map<K, T> array2map(const std::pair<K,T>(&a)[N])
    {
        return std::map<K, T> (a, a + getArrayElems (a));
    }

    // ---------------------------------------------------------
    template<typename K, typename T, std::size_t N>
    static std::map<K, T> array2map(std::pair<K,T>(&a)[N])
    {
        return std::map<K, T> (a, a + getArrayElems (a));
    }

    // ------------------------------------
    template<typename K, typename T>
    static std::map<K, T> pair2map(const std::pair<K,T>& e)
    {
        return std::map<K, T> (&e, &e + 1);
    }

// ==============
    class BaseBigInt
    // ==============
    {

    public:
        // --- Public Methods
        std::size_t getUnitsSize () const;
        void        maximize ();

    protected :
        // --- Protected Data Members ---
        std::vector<Ulong>                  m_units;
        static std::map<char, std::size_t>  ms_hex2dec;

        // --- Protected Methods
        void showUnits (std::ostream& o_stream) const;
        virtual ~BaseBigInt () = 0;


    public :
        // --- Public Methods ---
        bool  isEmpty () const;

    };

// ==============
    class Vin : public BaseBigInt
    // ==============
    {
    private :
        // -------
        // FRIENDS
        // -------
        friend std::ostream& operator<< (std::ostream& o_os, const Vin& i_ins);


    private :
        // --- Private Data Members ---
        static Ulong s_carry;

        // --- Private Methods ---
        static Ulong    addUnits(Ulong n1, Ulong n2);
        bool            initViaString (const std::string& i_arg, std::size_t i_digitBase);

    public :
        // ----------------------
        // --- Public Methods ---
        // ----------------------
        // --- Constructors ---
        explicit Vin ();
        explicit Vin (Ulong i_unit);
        explicit Vin (const std::string& i_arg, std::size_t i_digitBase);
        explicit Vin (const Rossi& i_arg);

        // --- Aux methods  ---
        Ulong toUlong () const;
        // operator Ulong () const;

        // --- Show functions ---
        std::string toStrHex (const std::string& i_prefix = "") const;
        std::string toStr0xHex () const;
        std::string toStrDec (const std::string& i_prefix = "") const;


        // --- General purpose mathematical methods ---
        Vin operator+ (const Vin& i_arg) const;
        Vin operator* (Ulong i_arg) const;


        // --- Comparison operators ---
        bool operator== (const Vin& i_arg) const;
        bool operator!= (const Vin& i_arg) const;
        bool operator<  (const Vin& i_arg) const;
        bool operator>  (const Vin& i_arg) const;
        bool operator<= (const Vin& i_arg) const;
        bool operator>= (const Vin& i_arg) const;


        // ---- Service methods ---
        void showUnits(std::ostream& o_stream) const;

    };


// ============================
    class Rossi : public BaseBigInt
    // ============================
    {
    private :
        // -------
        // FRIENDS
        // -------
        friend std::ostream& operator<< (std::ostream& o_os, const Rossi& i_ins);

    private :
        // --- Private Methods ---
        void  resizeUnits (std::size_t i_size);
        void  truncateUnits ();
        void  smartTruncateUnits ();
        bool  backUnitIsNull () const;
        bool  initViaString (const std::string& i_arg, std::size_t i_digitBase);


    public :
        // ----------------------
        // --- Public Methods ---
        // ----------------------
        // --- Constructors ---
        explicit Rossi ();
        explicit Rossi (Ulong i_unit);
        explicit Rossi (const std::string& i_arg, std::size_t i_digitBase);
        explicit Rossi (const Vin& i_arg);
        explicit Rossi (const std::size_t i_noOfUnits, Ulong i_internalUnits, Ulong i_backUnit);


        // --- Aux methods ---
        Ulong toUlong () const;
        // operator Ulong () const;


        // --- General purpose mathematical methods ---

        // Rossi& operator= (Ulong i_arg);
        Rossi  operator+ (const Rossi& i_arg);
        Rossi  operator+ (Ulong i_arg);
        Rossi  operator* (Rossi i_arg) const;
        Rossi  operator* (Ulong i_arg) const;
        //  Rossi& Rossi::operator*= (Rossi i_arg);
        Rossi  operator/ (const Rossi& i_arg) const;
        Rossi  operator% (const Rossi& i_arg) const;
        Rossi  divide(const Rossi& i_dividend, const Rossi& i_divisor, Rossi* o_remainder) const;
        Rossi& operator+= (const Rossi& i_arg);
        Rossi  operator++ (int); // Post increment operator
        Rossi& operator++ ();    // Pre increment operator
        Rossi  operator-  (const Rossi& i_arg);
        Rossi  operator-  ();
        Rossi  operator-- (int); // Post decrement operator
        Rossi& operator-- ();    // Pre decrement operator
        Rossi& operator-= (const Rossi& i_arg);
        Rossi  sqrt();


        // --- Bitwise boolean operators ---
        Rossi  operator&   (const Rossi& i_arg);
        Rossi  operator|   (const Rossi& i_arg);
        Rossi  operator^   (const Rossi& i_arg);
        Rossi& operator&=  (const Rossi& i_arg);
        Rossi& operator|=  (const Rossi& i_arg);
        Rossi& operator^=  (const Rossi& i_arg);
        Rossi  operator~   ();
        Rossi  operator>>  (std::size_t i_shift);
        Rossi  operator<<  (std::size_t  i_shift);
        Rossi& operator>>= (std::size_t  i_shift);
        Rossi& operator<<= (std::size_t  i_shift);
        // --- Comparison operators ---
        bool operator== (const Rossi& i_arg) const;
        bool operator!= (const Rossi& i_arg) const;
        bool operator<  (const Rossi& i_arg) const;
        bool operator>  (const Rossi& i_arg) const;
        bool operator<= (const Rossi& i_arg) const;
        bool operator>= (const Rossi& i_arg) const;

        // --- Log fucntion
        friend int rossiLog( const Rossi  & bint);
        // --- Show functions ---
        std::string toStrHex (const std::string& i_prefix = "") const;
        std::string toStr0xHex () const;
        std::string toStrDec (const std::string& i_prefix = "") const;

        // ---- Service methods ---
        void showUnits(std::ostream& o_stream) const;
    };
}; // class BigInt

#endif    // __BIG_INT_H__
