// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the URCHANDLER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// URCHANDLER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SULOKI_WINDOWS_OS_SULOKI
#ifdef URCHANDLER_EXPORTS
#define URCHANDLER_API __declspec(dllexport)
#else
#define URCHANDLER_API __declspec(dllimport)
#endif
#else
#define URCHANDLER_API
#endif

#include <memory>
#include <iostream>
#include "suloki.pb.h"
#include "suloki.h"
#include "suloki_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif
	URCHANDLER_API SulokiRet Init(SULOKI_IN SulokiUrcModuleInterface* pSulokiUrcModuleInterface, SULOKI_IN std::string groupName, SULOKI_IN std::string serviceName, SULOKI_IN std::string strModuleName, SULOKI_IN std::string strConfig);
	URCHANDLER_API SulokiRet Start(void);
	URCHANDLER_API SulokiRet TestValid(SULOKI_IN const SulokiMessage& msg);
	URCHANDLER_API SulokiRet Handle(SULOKI_IN std::auto_ptr< SulokiMessage > msgSmart, SULOKI_IN SulokiContext& context);
	URCHANDLER_API SulokiRet Stop(void);
	URCHANDLER_API SulokiRet Clear(void);

#ifdef __cplusplus
}
#endif

/*
// This class is exported from the urchandler.dll
class URCHANDLER_API Curchandler {
public:
	Curchandler(void);
	// TODO: add your methods here.
};

extern URCHANDLER_API int nurchandler;

URCHANDLER_API int fnurchandler(void);
*/
