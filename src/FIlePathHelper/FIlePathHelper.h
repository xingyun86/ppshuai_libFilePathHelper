// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FILEPATHHELPER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FILEPATHHELPER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FILEPATHHELPER_EXPORTS
#define FILEPATHHELPER_API __declspec(dllexport)
#else
#define FILEPATHHELPER_API __declspec(dllimport)
#endif

// This class is exported from the FIlePathHelper.dll
//class FILEPATHHELPER_API CFIlePathHelper {
//public:
//	CFIlePathHelper(void);
//	// TODO: add your methods here.
//};

//extern FILEPATHHELPER_API int nFIlePathHelper;

//FILEPATHHELPER_API int fnFIlePathHelper(void);

FILEPATHHELPER_API bool GetSystemRecycleBinPathA(CHAR * wPath, int nSize);
FILEPATHHELPER_API bool GetSystemRecycleBinPathW(WCHAR * wPath, int nSize);
