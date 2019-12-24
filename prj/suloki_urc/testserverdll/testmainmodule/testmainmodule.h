// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TESTMAINMODULE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TESTMAINMODULE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SULOKI_WINDOWS_OS_SULOKI
#ifdef TESTMAINMODULE_EXPORTS
#define TESTMAINMODULE_API __declspec(dllexport)
#else
#define TESTMAINMODULE_API __declspec(dllimport)
#endif
#else
#define TESTMAINMODULE_API
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
	TESTMAINMODULE_API SulokiRet Init(SULOKI_IN SulokiUrcModuleInterface* pSulokiUrcModuleInterface, SULOKI_IN std::string groupName, SULOKI_IN std::string serviceName, SULOKI_IN std::string strModuleName, SULOKI_IN std::string strConfig);
	TESTMAINMODULE_API SulokiRet Start(void);
	TESTMAINMODULE_API SulokiRet TestValid(SULOKI_IN const suloki::SulokiMessage& msg);
	TESTMAINMODULE_API SulokiRet Handle(SULOKI_IN std::auto_ptr< suloki::SulokiMessage > msgSmart, SULOKI_IN suloki::SulokiContext& context);
	TESTMAINMODULE_API SulokiRet Stop(void);
	TESTMAINMODULE_API SulokiRet Clear(void);

#ifdef __cplusplus
}
#endif


/*
// This class is exported from the testmainmodule.dll
class TESTMAINMODULE_API Ctestmainmodule {
public:
	Ctestmainmodule(void);
	// TODO: add your methods here.
};

extern TESTMAINMODULE_API int ntestmainmodule;

TESTMAINMODULE_API int fntestmainmodule(void);
*/
