#ifndef _SRC_CLUCENE_CLUCENE_CONFIG_H
#define _SRC_CLUCENE_CLUCENE_CONFIG_H 1
 
/* src/CLucene/clucene-config.h. 
*  Generated automatically at end of cmake.
*/

/* Compulsary headers. cmake will fail if these are not found:
 * Eventually we will take these out of StdHeader to simplify it all a bit.
*/
#define _CL_HAVE_ALGORITHM 1
#define _CL_HAVE_FUNCTIONAL  1
#define _CL_HAVE_MAP  1
#define _CL_HAVE_VECTOR  1
#define _CL_HAVE_LIST  1
#define _CL_HAVE_SET  1
#define _CL_HAVE_MATH_H  1 
#define _CL_HAVE_STDARG_H  1
#define _CL_HAVE_STDEXCEPT 1
#define _CL_HAVE_FCNTL_H  1 

#define _CL_HAVE_WCSCPY  1 
#define _CL_HAVE_WCSNCPY  1 
#define _CL_HAVE_WCSCAT  1 
#define _CL_HAVE_WCSCHR  1 
#define _CL_HAVE_WCSSTR  1 
#define _CL_HAVE_WCSLEN  1 
#define _CL_HAVE_WCSCMP  1 
#define _CL_HAVE_WCSNCMP  1 
#define _CL_HAVE_WCSCSPN  1 

/* CMake will look for these headers: */

/* Define to 1 if you have the ANSI C header files. */
#define _CL_STDC_HEADERS  1 

/* Define to 1 if you have the <string.h> header file. */
#define _CL_HAVE_STRING_H  1

/* Define to 1 if you have the <strings.h> header file. */
#define _CL_HAVE_STRINGS_H  1 

/* Define to 1 if you have the <memory.h> header file. */
#define _CL_HAVE_MEMORY_H  1 

/* Define to 1 if you have the <unistd.h> header file. */
#define _CL_HAVE_UNISTD_H  1 

/* Define to 1 if you have the <io.h> header file. */
#define _CL_HAVE_IO_H  1 

/* Define to 1 if you have the <direct.h> header file. */
#define _CL_HAVE_DIRECT_H  1 

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'. */
#define _CL_HAVE_DIRENT_H  1 

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'. */
/* #undef _CL_HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'. */
/* #undef _CL_HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <errno.h> header file. */
#define _CL_HAVE_ERRNO_H  1 

/* Define to 1 if you have the <wchar.h> header file. */
#define _CL_HAVE_WCHAR_H  1 

/* Define to 1 if you have a functioning <wchar.h> header file. */
#define _CL_HAVE_WCTYPE_H   

/* Define to 1 if you have the <ctype.h> header file. */
#define _CL_HAVE_CTYPE_H  1 

/* Define to 1 if you have the <windows.h> header file. */
//#define _CL_HAVE_WINDOWS_H  1

/* Define to 1 if you have the <sys/types.h> header file. */
#define _CL_HAVE_SYS_TYPES_H  1 

/* Define to 1 if you have the <inttypes.h> header file. */
#define _CL_HAVE_INTTYPES_H  1 


/* CMake will look for these symbols: */

/* Define to 1 if the system has the type `float_t'. */
//todo: HACK
#define _CL_HAVE_FLOAT_T  1 

#ifdef __MINGW32__
//typedef float float_t;
#endif


/* Define to 1 if the system has the type `intptr_t'. */
//todo: HACK
#define _CL_HAVE_INTPTR_T  1 

/* Define to 1 if the system has the type `wchar_t'. */
//todo: HACK
#define _CL_HAVE_WCHAR_T  1 



/* CMake will look for these functions: */

/* Define to 1 if you have the `printf' function. */
#define _CL_HAVE_PRINTF  1 

/* Define to 1 if you have the `snprintf' function. */
#define _CL_HAVE_SNPRINTF  1 
 


/* CMake will determine these module specificics */

/* Define if you have POSIX threads libraries and header files. */
#define _CL_HAVE_PTHREAD  1 

/* Define if you have POSIX threads libraries and header files. */
//#define _CL_HAVE_WIN32_THREADS  1

/* define if the compiler supports ISO C++ standard library */
#define _CL_HAVE_STD   


/* Disable multithreading */
/* #undef _CL_DISABLE_MULTITHREADING */


/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef _CL_HAVE_DLFCN_H */


/* Define to 1 if you have the <ext/hash_map> header file. */
#define _CL_HAVE_EXT_HASH_MAP  1 

/* Define to 1 if you have the <ext/hash_set> header file. */
#define _CL_HAVE_EXT_HASH_SET  1 


/* Define to 1 if you have the <functional> header file. */

/* Does not support new float byte<->float conversions */
/* #undef _CL_HAVE_FUNCTIONING_FLOAT_BYTE */

/* Define to 1 if you have the `getpagesize' function. */
/* #undef _CL_HAVE_GETPAGESIZE */

/* Define to 1 if you have the <hash_map> header file. */
#define _CL_HAVE_HASH_MAP

/* Define to 1 if you have the <hash_set> header file. */
#define _CL_HAVE_HASH_SET


/* Define to 1 if you have the `lltoa' function. */
#define _CL_HAVE_LLTOA

/* Define to 1 if you have the `lltow' function. */
//#define _CL_HAVE_LLTOW

/* Define to 1 if long double works and has more range or precision than double. */
/* #undef _CL_HAVE_LONG_DOUBLE */




/* Define to 1 if you have a working `mmap' system call. */
/* #undef _CL_HAVE_MMAP */

/* define if the compiler implements namespaces */
#define _CL_HAVE_NAMESPACES

/* Define if you have the nanosleep function */
/* #undef _CL_HAVE_NANOSLEEP */

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef _CL_HAVE_NDIR_H */

/* Does not support new float byte<->float conversions */
/* #undef _CL_HAVE_NO_FLOAT_BYTE */

/* Does not support try/catch blocks */
/* #undef _CL_HAVE_NO_FUNCTION_TRY_BLOCKS */


/* Define if recursive pthread mutexes are available */
/* #undef _CL_HAVE_PTHREAD_MUTEX_RECURSIVE */



/* Defined if the snprintf overflow test fails */
/* #undef _CL_HAVE_SNPRINTF_BUG */

/* Define to 1 if you have the `snwprintf' function. */
#if defined _WIN32 || defined ANDROID
#define _CL_HAVE_SNWPRINTF
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#define _CL_HAVE_STDINT_H  1 

/* Define to 1 if you have the <stdlib.h> header file. */
#define _CL_HAVE_STDLIB_H  1 

/* define if the compiler supports Standard Template Library */
/* #undef _CL_HAVE_STL */


/* Define to 1 if you have the `strlwr' function. */
#define _CL_HAVE_STRLWR

/* Define to 1 if you have the `strtoll' function. */
#define _CL_HAVE_STRTOLL

/* Define to 1 if you have the `strupr' function. */
#define _CL_HAVE_STRUPR

/* Defined if the swprintf test fails */
/* #undef _CL_HAVE_SWPRINTF_BUG */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define _CL_HAVE_SYS_STAT_H  1 

/* Define to 1 if you have the <sys/timeb.h> header file. */
#ifndef __SYMBIAN32__
#define _CL_HAVE_SYS_TIMEB_H  1 
#endif

/* Define to 1 if you have the <tchar.h> header file. */
#if defined _WIN32 && !defined __SYMBIAN32__
#define _CL_HAVE_TCHAR_H
#endif

/* Define to 1 if you have the `tell' function. */
#define _CL_HAVE_TELL



/* Define to 1 if you have the `vsnwprintf' function. */
#define _CL_HAVE_VSNWPRINTF

/* Define to 1 if you have the `wcscasecmp' function. */
/* #undef _CL_HAVE_WCSCASECMP */

/* Define to 1 if you have the `wcscat' function. */
#define _CL_HAVE_WCSCAT  1 

/* Define to 1 if you have the `wcschr' function. */
#define _CL_HAVE_WCSCHR  1 

/* Define to 1 if you have the `wcscmp' function. */
#define _CL_HAVE_WCSCMP  1 

/* Define to 1 if you have the `wcscpy' function. */
#define _CL_HAVE_WCSCPY  1 

/* Define to 1 if you have the `wcscspn' function. */
#define _CL_HAVE_WCSCSPN  1 

/* Define to 1 if you have the `wcsicmp' function. */
#define _CL_HAVE_WCSICMP

/* Define to 1 if you have the `wcslen' function. */
#define _CL_HAVE_WCSLEN  1 

/* Define to 1 if you have the `wcsncmp' function. */
#define _CL_HAVE_WCSNCMP  1 

/* Define to 1 if you have the `wcsncpy' function. */
#define _CL_HAVE_WCSNCPY  1 

/* Define to 1 if you have the `wcsstr' function. */
#define _CL_HAVE_WCSSTR  1 

/* Define to 1 if you have the `wcstod' function. */
#define _CL_HAVE_WCSTOD

/* Define to 1 if you have the `wcstoll' function. */
//#define _CL_HAVE_WCSTOLL

/* Define to 1 if you have the `wcsupr' function. */
#define _CL_HAVE_WCSUPR


/* Define to 1 if you have the `wprintf' function. */
#define _CL_HAVE_WPRINTF

/* Define to 1 if you have the `_filelength' function. */
#define _CL_HAVE__FILELENGTH

/* How to define a static const in a class */
/* #undef LUCENE_STATIC_CONSTANT_SYNTAX */

/* Name of package */
/* #undef _CL_PACKAGE */

/* Define to the address where bug reports for this package should be sent. */
/* #undef _CL_PACKAGE_BUGREPORT */

/* Define to the full name of this package. */
/* #undef _CL_PACKAGE_NAME */

/* Define to the full name and version of this package. */
/* #undef _CL_PACKAGE_STRING */

/* Define to the one symbol short name of this package. */
/* #undef _CL_PACKAGE_TARNAME */

/* Define to the version of this package. */
/* #undef _CL_PACKAGE_VERSION */

/* Define to the necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef _CL_PTHREAD_CREATE_JOINABLE */

/* The size of a `unsigned char', as computed by sizeof. */
/* #undef _CL_SIZEOF_UNSIGNED_CHAR */

/* The size of a `unsigned int', as computed by sizeof. */
/* #undef _CL_SIZEOF_UNSIGNED_INT */

/* The size of a `unsigned long', as computed by sizeof. */
/* #undef _CL_SIZEOF_UNSIGNED_LONG */

/* The size of a `unsigned long long', as computed by sizeof. */
/* #undef _CL_SIZEOF_UNSIGNED_LONG_LONG */

/* The size of a `unsigned __int64', as computed by sizeof. */
/* #undef _CL_SIZEOF_UNSIGNED___INT64 */

/* Define to 1 if the `S_IS*' macros in <sys/stat.h> do not work properly. */
/* #undef _CL_STAT_MACROS_BROKEN */

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define _CL_TIME_WITH_SYS_TIME  1 

/* Version number of package */
/* #undef _CL_VERSION */

/* Forces into Ascii mode */
/* #undef _ASCII */

/* Conditional Debugging */
/* #undef _CL__CND_DEBUG */

/* debuging option */
/* #undef _DEBUG */

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* If not already defined, then define as a datatype of *exactly* 32 bits. */
/* #undef uint32_t */

/* If not already defined, then define as a datatype of *exactly* 64 bits. */
/* #undef uint64_t */

/* If not already defined, then define as a datatype of *exactly* 8 bits. */
/* #undef uint8_t */

/* Fix for PATH_MAX */
#ifdef __SYMBIAN32__
//#include <sys/cdefs.h>
//#include <sys/syslimits.h>

#define PATH_MAX 256

//#define _LUCENE_NO_NEW_STYLE_CASTS

//#define _CL_DISABLE_NATIVE_EXCEPTIONS

//#define _CLFINALLY() catch(...){ throw; }

#endif

#endif
