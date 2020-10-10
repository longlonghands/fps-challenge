
#pragma once
#ifndef _TIDY_CONFIG_H
#define _TIDY_CONFIG_H


//
/// Version number  
//

#define TIDY_MAJOR_VERSION    0
#define TIDY_MINOR_VERSION    1
#define TIDY_PATCH_VERSION    0

#define TIDY_AUX_STR_EXP(__A) #__A
#define TIDY_AUX_STR(__A)     TIDY_AUX_STR_EXP(__A)
#define TIDY_VERSION          TIDY_AUX_STR(TIDY_MAJOR_VERSION) "." TIDY_AUX_STR(TIDY_MINOR_VERSION) "." TIDY_AUX_STR(TIDY_PATCH_VERSION)


//
/// Cross platform configuration
//
#if __unix__ // all unices
// Unix    
#define _UNIX
#endif
#if __posix__
// POSIX    
#define _POSIX
#endif
#if __linux__
// linux    
#define _LINUX
#endif
#if __GNUC__
// GCC specific
#define _GNUC
#endif
#if __APPLE__
// Mac OS
#define _APPLE
#endif




//
// Platform Identification
//
#define PLATFORM_OS_FREE_BSD      0x0001
#define PLATFORM_OS_AIX           0x0002
#define PLATFORM_OS_HPUX          0x0003
#define PLATFORM_OS_TRU64         0x0004
#define PLATFORM_OS_LINUX         0x0005
#define PLATFORM_OS_MAC_OS_X      0x0006
#define PLATFORM_OS_NET_BSD       0x0007
#define PLATFORM_OS_OPEN_BSD      0x0008
#define PLATFORM_OS_IRIX          0x0009
#define PLATFORM_OS_SOLARIS       0x000a
#define PLATFORM_OS_QNX           0x000b
#define PLATFORM_OS_VXWORKS       0x000c
#define PLATFORM_OS_CYGWIN        0x000d
#define PLATFORM_OS_UNKNOWN_UNIX  0x00ff
#define PLATFORM_OS_WINDOWS_NT    0x1001
#define PLATFORM_OS_WINDOWS_CE    0x1011
#define PLATFORM_OS_VMS           0x2001


#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS_FAMILY_BSD 1
#define PLATFORM_OS PLATFORMOS_FREE_BSD
#elif defined(_AIX) || defined(__TOS_AIX__)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS PLATFORM_OS_AIX
#elif defined(hpux) || defined(_hpux) || defined(__hpux)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS PLATFORM_OS_HPUX
#elif defined(__digital__) || defined(__osf__)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS PLATFORM_OS_TRU64
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__TOS_LINUX__) || defined(EMSCRIPTEN)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS PLATFORM_OS_LINUX
#elif defined(__APPLE__) || defined(__TOS_MACOS__)
#define PLATFORMOS_FAMILY_UNIX 1
#define PLATFORMOS_FAMILY_BSD 1
#define PLATFORMOS PLATFORMOS_MAC_OS_X
#elif defined(__NetBSD__)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS_FAMILY_BSD 1
#define PLATFORM_OS PLATFORM_OS_NET_BSD
#elif defined(__OpenBSD__)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS_FAMILY_BSD 1
#define PLATFORM_OS PLATFORM_OS_OPEN_BSD
#elif defined(sgi) || defined(__sgi)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS PLATFORM_OS_IRIX
#elif defined(sun) || defined(__sun)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS PLATFORM_OS_SOLARIS
#elif defined(__QNX__)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS PLATFORM_OS_QNX
#elif defined(__CYGWIN__)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS PLATFORM_OS_CYGWIN
#elif defined(PLATFORMVXWORKS)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS PLATFORM_OS_VXWORKS
#elif defined(unix) || defined(__unix) || defined(__unix__)
#define PLATFORM_OS_FAMILY_UNIX 1
#define PLATFORM_OS PLATFORM_OS_UNKNOWN_UNIX
#elif defined(_WIN32_WCE)
#define PLATFORM_OS_FAMILY_WINDOWS 1
#define PLATFORM_OS PLATFORM_OS_WINDOWS_CE
#elif defined(_WIN32) || defined(_WIN64)
#define PLATFORM_OS_FAMILY_WINDOWS 1
#define PLATFORM_OS PLATFORM_OS_WINDOWS_NT
// #define WIN32_LEAN_AND_MEAN 1 redifinition warning
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0601
#endif
#include <sdkddkver.h>
#elif defined(__VMS)
#define PLATFORM_OS_FAMILY_VMS 1
#define PLATFORM_OS PLATFORM_OS_VMS
#endif


#if !defined(PLATFORM_OS)
#error "Unknown Platform."
#endif


#ifndef PLATFORM_OS_FAMILY_UNIX
#define GCC_DIAG_OFF(x)
#define GCC_DIAG_ON(x)
#endif

//
// Hardware Architecture and Byte Order
//
#define PLATFORM_ARCH_ALPHA   0x01
#define PLATFORM_ARCH_IA32    0x02
#define PLATFORM_ARCH_IA64    0x03
#define PLATFORM_ARCH_MIPS    0x04
#define PLATFORM_ARCH_HPPA    0x05
#define PLATFORM_ARCH_PPC     0x06
#define PLATFORM_ARCH_POWER   0x07
#define PLATFORM_ARCH_SPARC   0x08
#define PLATFORM_ARCH_AMD64   0x09
#define PLATFORM_ARCH_ARM     0x0a
#define PLATFORM_ARCH_M68K    0x0b
#define PLATFORM_ARCH_S390    0x0c
#define PLATFORM_ARCH_SH      0x0d
#define PLATFORM_ARCH_NIOS2   0x0e
#define PLATFORM_ARCH_AARCH64 0x0f


#if defined(__ALPHA) || defined(__alpha) || defined(__alpha__) || defined(_M_ALPHA)
#define PLATFORM_ARCH PLATFORM_ARCH_ALPHA
#define PLATFORM_ARCH_LITTLE_ENDIAN 1
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_IX86) || defined(EMSCRIPTEN)
#define PLATFORM_ARCH PLATFORM_ARCH_IA32
#define PLATFORM_ARCH_LITTLE_ENDIAN 1
#elif defined(_IA64) || defined(__IA64__) || defined(__ia64__) || defined(__ia64) || defined(_M_IA64)
#define PLATFORM_ARCH PLATFORM_ARCH_IA64
#if defined(hpux) || defined(_hpux)
#define PLATFORM_ARCH_BIG_ENDIAN 1
#else
#define PLATFORM_ARCH_LITTLE_ENDIAN 1
#endif
#elif defined(__x86_64__) || defined(_M_X64)
#define PLATFORM_ARCH PLATFORM_ARCH_AMD64
#define PLATFORM_ARCH_LITTLE_ENDIAN 1
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__) || defined(_M_MRX000)
#define PLATFORM_ARCH PLATFORM_ARCH_MIPS
#if defined(PLATFORM_OS_FAMILY_WINDOWS)
// Is this OK? Supports windows only little endian??
#define PLATFORM_ARCH_LITTLE_ENDIAN 1
#elif defined(__MIPSEB__) || defined(_MIPSEB) || defined(__MIPSEB)
#define PLATFORM_ARCH_BIG_ENDIAN 1
#elif defined(__MIPSEL__) || defined(_MIPSEL) || defined(__MIPSEL)
#define PLATFORM_ARCH_LITTLE_ENDIAN 1
#else
#error "MIPS but neither MIPSEL nor MIPSEB?"
#endif

#elif defined(__hppa) || defined(__hppa__)
#define PLATFORM_ARCH PLATFORM_ARCH_HPPA
#define PLATFORM_ARCH_BIG_ENDIAN 1
#elif defined(__PPC) || defined(__POWERPC__) || defined(__powerpc) || defined(__PPC__) || \
	defined(__powerpc__) || defined(__ppc__) || defined(__ppc) || defined(_ARCH_PPC) || defined(_M_PPC)
#define PLATFORM_ARCH PLATFORM_ARCH_PPC
#define PLATFORM_ARCH_BIG_ENDIAN 1
#elif defined(_POWER) || defined(_ARCH_PWR) || defined(_ARCH_PWR2) || defined(_ARCH_PWR3) || \
	defined(_ARCH_PWR4) || defined(__THW_RS6000)
#define PLATFORM_ARCH PLATFORM_ARCH_POWER
#define PLATFORM_ARCH_BIG_ENDIAN 1
#elif defined(__sparc__) || defined(__sparc) || defined(sparc)
#define PLATFORM_ARCH PLATFORM_ARCH_SPARC
#define PLATFORM_ARCH_BIG_ENDIAN 1
#elif defined(__arm__) || defined(__arm) || defined(ARM) || defined(_ARM_) || defined(__ARM__) || defined(_M_ARM)
#define PLATFORM_ARCH PLATFORM_ARCH_ARM
#if defined(__ARMEB__)
#define PLATFORM_ARCH_BIG_ENDIAN 1
#else
#define PLATFORM_ARCH_LITTLE_ENDIAN 1
#endif
#elif defined(__arm64__) || defined(__arm64) 
#define PLATFORM_ARCH PLATFORM_ARCH_ARM64
#if defined(__ARMEB__)
#define PLATFORM_ARCH_BIG_ENDIAN 1
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define PLATFORM_ARCH_BIG_ENDIAN 1
#else
#define PLATFORM_ARCH_LITTLE_ENDIAN 1
#endif
#elif defined(__m68k__)
#define PLATFORM_ARCH PLATFORM_ARCH_M68K
#define PLATFORM_ARCH_BIG_ENDIAN 1
#elif defined(__s390__)
#define PLATFORM_ARCH PLATFORM_ARCH_S390
#define PLATFORM_ARCH_BIG_ENDIAN 1
#elif defined(__sh__) || defined(__sh) || defined(SHx) || defined(_SHX_)
#define PLATFORM_ARCH PLATFORM_ARCH_SH
#if defined(__LITTLE_ENDIAN__) || (PLATFORM_OS == PLATFORM_OS_WINDOWS_CE)
#define PLATFORM_ARCH_LITTLE_ENDIAN 1
#else
#define PLATFORM_ARCH_BIG_ENDIAN 1
#endif
#elif defined (nios2) || defined(__nios2) || defined(__nios2__)
#define PLATFORM_ARCH PLATFORM_ARCH_NIOS2
#if defined(__nios2_little_endian) || defined(nios2_little_endian) || defined(__nios2_little_endian__)
#define PLATFORM_ARCH_LITTLE_ENDIAN 1
#else
#define PLATFORM_ARCH_BIG_ENDIAN 1
#endif
#elif defined(__AARCH64EL__)
#define PLATFORM_ARCH PLATFORM_ARCH_AARCH64
#define PLATFORM_ARCH_LITTLE_ENDIAN 1
#elif defined(__AARCH64EB__)
#define PLATFORM_ARCH PLATFORM_ARCH_AARCH64
#define PLATFORM_ARCH_BIG_ENDIAN 1
#endif


#if defined(_MSC_VER)
#define PLATFORM_COMPILER_MSVC  
#define PLATFORM_COMPILER_MSVC_VER  _MSC_VER
#elif defined(__clang__)
#define PLATFORM_COMPILER_CLANG
#elif defined (__GNUC__)
#define PLATFORM_COMPILER_GCC
#elif defined (__MINGW32__) || defined (__MINGW64__)
#define PLATFORM_COMPILER_MINGW
#elif defined (__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__ICL)
#define PLATFORM_COMPILER_INTEL
#elif defined (__SUNPRO_CC)
#define PLATFORM_COMPILER_SUN
#elif defined (__MWERKS__) || defined(__CWCC__)
#define PLATFORM_COMPILER_CODEWARRIOR
#elif defined (__sgi) || defined(sgi)
#define PLATFORM_COMPILER_SGI
#elif defined (__HP_aCC)
#define PLATFORM_COMPILER_HP_ACC
#elif defined (__BORLANDC__) || defined(__CODEGEARC__)
#define PLATFORM_COMPILER_CBUILDER
#elif defined (__DMC__)
#define PLATFORM_COMPILER_DMARS
#elif defined (__HP_aCC)
#define PLATFORM_COMPILER_HP_ACC
#elif (defined (__xlc__) || defined (__xlC__)) && defined(__IBMCPP__)
#define PLATFORM_COMPILER_IBM_XLC // IBM XL C++
#elif defined (__IBMCPP__) && defined(__COMPILER_VER__)
#define PLATFORM_COMPILER_IBM_XLC_ZOS // IBM z/OS C++
#endif


#if !defined(PLATFORM_ARCH)
#error "Unknown Hardware Architecture."
#endif


#if defined(PLATFORM_OS_FAMILY_WINDOWS)
#define PLATFORM_DEFAULT_NEWLINE_CHARS "\r\n"
#else
#define PLATFORM_DEFAULT_NEWLINE_CHARS "\n"
#endif


#ifdef _WIN32
# ifndef _CRT_SECURE_NO_WARNINGS
#	define _CRT_SECURE_NO_WARNINGS
# endif
# ifndef TIDY_SHARED_LIBRARY
#   define TIDY_EXTERN __declspec(dllexport)
# else
#   define TIDY_EXTERN __declspec(dllimport)
# endif
#else
# define TIDY_EXTERN // nothing
#endif

#ifdef _WIN32

// Verify that we're built with the multithreaded 
// versions of the runtime libraries
#if defined(_MSC_VER) && !defined(__MINGW64__) && !defined(_MT) 
#error Must compile with /MD, /MDd, /MT or /MTd
#endif

// Check debug/release settings consistency
#if defined(NDEBUG) && defined(_DEBUG)
#error Inconsistent build settings (check for /MD[d])
#endif

// Unicode Support
#if defined(UNICODE) || defined(_UNICODE)
#define TIDY_UNICODE
#endif

// Disable unnecessary warnings
#if defined(_MSC_VER)
#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable:4251) // ... needs to have dll-interface warning 
#pragma warning(disable:4355) // 'this' : used in base member initializer list
#pragma warning(disable:4996) // VC++ 8.0 deprecation warnings
#pragma warning(disable:4351) // new behavior: elements of array '...' will be default initialized
#pragma warning(disable:4675) // resolved overload was found by argument-dependent lookup
#pragma warning(disable:4275) // non dll-interface class 'std::exception' used as base for dll-interface class 'scy::Exception'
#endif

#endif // WIN32


#ifdef TIDY_DO_ASSERT
#include <assert.h>
#else 
#undef assert
#define assert(X) (void(0))
#endif


#if defined(_MSC_VER)
	#if defined(_WIN64)
		#define TIDY_PTR_IS_64_BIT 1
		typedef signed __int64     IntPtr;
		typedef unsigned __int64   UIntPtr;
	#else
		typedef signed long        IntPtr;
		typedef unsigned long      UIntPtr;
	#endif
	#define TIDY_HAVE_INT64 1
#elif defined(__GNUC__) || defined(__clang__)
typedef signed long            IntPtr;
typedef unsigned long          UIntPtr;
#if defined(__LP64__)
#define TIDY_PTR_IS_64_BIT 1
#define TIDY_LONG_IS_64_BIT 1
typedef signed long        Int64;
typedef unsigned long      UInt64;
#else
typedef signed long long   Int64;
typedef unsigned long long UInt64;
#endif
#define TIDY_HAVE_INT64 1
#endif

//#define TIDY_NO_LOCALE

#endif // _TIDY_CONFIG_H
