#ifndef _STDINT_COMPAT_H_
#define _STDINT_COMPAT_H_

// Compatiblity for older compilers

#if defined(_MSC_VER)
#ifdef _WIN32

    #if (_MSC_VER >= 1600)

        // VS2010 and newer can use this header
        #include <cstdint>

    #elif (_MSC_VER < 1600)

        // Then define the things we may need.
		
        typedef __int64            int64_t;
        typedef __int32            int32_t;
        typedef __int16            int16_t;
        typedef __int8             int8_t;

        typedef unsigned __int64   uint64_t;
        typedef unsigned __int32   uint32_t;
        typedef unsigned __int16   uint16_t;
        typedef unsigned __int8    uint8_t;

        const int32_t	INT32_MAX	= 0x7FFFFFFF;
        const uint32_t	UINT32_MAX	= 0xFFFFFFFF;

        const int16_t   INT16_NAX	= 0x7FFF;
        const uint16_t  UINT16_MAX	= 0xFFFF;

    #endif // _MSC_VER

#else 

    #include <cstdint>
    #include <climit>

#endif // _WIN32
#else

    #include <cstdint>
    #include <climits>

#endif // _MSC_VER


#endif // _STDINT_COMPAT_H_
