// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBMNETUTIL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBMNETUTIL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBMNETUTIL_EXPORTS
#define LIBMNETUTIL_API __declspec(dllexport)
#else
#define LIBMNETUTIL_API __declspec(dllimport)
#endif

// This class is exported from the libmnetutil.dll
class LIBMNETUTIL_API Clibmnetutil {
public:
	Clibmnetutil(void);
	// TODO: add your methods here.
};

extern LIBMNETUTIL_API int nlibmnetutil;

LIBMNETUTIL_API int fnlibmnetutil(void);
