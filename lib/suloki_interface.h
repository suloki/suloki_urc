#ifndef _SULOKI_ESF_INTERFACE_H_
#define _SULOKI_ESF_INTERFACE_H_

#ifdef SULOKI_WINDOWS_OS_SULOKI
#define SULOKI_STDCALL __stdcall
#else
#define SULOKI_STDCALL
#endif

#define SULOKI_IN
#define SULOKI_OUT
#define SULOKI_INOUT

#include "suloki.pb.h"

//about proto buffer define
//business id
const long SULOKI_SYSTEM_BISINESSID_PROTO = 0;//some system level bussiness
const long SULOKI_URC_BISINESSID_PROTO = 1;//about urc bussiness
const long SULOKI_USER_BISINESSID_PROTO = 1024;//user define business id must >= SULOKI_USER_BISINESSID_PROTO
//message id
//SULOKI_SYSTEM_BISINESSID_PROTO
const long SULOKI_TEST_MESSAGEID_SYSTEM_PROTO = 0;
const long SULOKI_HEARTBEAT_MESSAGEID_SYSTEM_PROTO = 1;
const long SULOKI_INIT_MESSAGEID_SYSTEM_PROTO = 2;
const long SULOKI_START_MESSAGEID_SYSTEM_PROTO = 3;
const long SULOKI_STOP_MESSAGEID_SYSTEM_PROTO = 4;
const long SULOKI_CLEAR_MESSAGEID_SYSTEM_PROTO = 5;
const long SULOKI_TIMEOUT_MESSAGEID_SYSTEM_PROTO = 6;
const long SULOKI_TOKEN_MESSAGEID_SYSTEM_PROTO = 7;//auth
//SULOKI_URC_BISINESSID_PROTO
const long SULOKI_ADD_MESSAGEID_URC_PROTO = 0;
const long SULOKI_DEL_MESSAGEID_URC_PROTO = 1;
const long SULOKI_UPDATE_MESSAGEID_URC_PROTO = 2;
const long SULOKI_GET_MESSAGEID_URC_PROTO = 3;
const long SULOKI_REGSERVICE_MESSAGEID_URC_PROTO = 4;//used system inside
const long SULOKI_UNREGSERVICE_MESSAGEID_URC_PROTO = 5;//used system inside
const long SULOKI_OBTAINSTATES_MESSAGEID_URC_PROTO = 6;//used system inside, get all service's state in group
const long SULOKI_SQL_MESSAGEID_URC_PROTO = 1024;

typedef long SulokiRet;

//urc obj base class
class BaseRoot
{
public:
	BaseRoot(){}
	virtual ~BaseRoot(){}
private:
	BaseRoot(BaseRoot& ref){}
	BaseRoot& operator=(BaseRoot& ref){ return *this; }
};
//urc module interface
typedef void(SULOKI_STDCALL *SulokiAsyncCb)(SULOKI_IN suloki::SulokiContext* pContextOri, SULOKI_IN std::auto_ptr<suloki::SulokiMessage> msgSmart, SULOKI_IN suloki::SulokiContext& contextNew);//Suloki::Uint pCcontext, Suloki::Uint msgPtr, bool bTimeout)
class SulokiUrcModuleInterface
{
public:
	virtual ~SulokiUrcModuleInterface(){}
	//
	virtual SulokiRet ReqRes(SULOKI_IN std::string strGroupName, SULOKI_IN std::string strServiceName, SULOKI_INOUT suloki::SulokiMessage& req, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL)) = 0;
	//Response
	virtual SulokiRet Notify(SULOKI_IN std::string strGroupName, SULOKI_IN std::string strServiceName, SULOKI_IN suloki::SulokiMessage& notice) = 0;
	//
	virtual SulokiRet PostToMainModule(SULOKI_IN std::auto_ptr<suloki::SulokiMessage> msgSmart) = 0;
	virtual SulokiRet AddObject(SULOKI_IN std::string strUrName, SULOKI_IN boost::shared_ptr<BaseRoot>& baseSmartPtr) = 0;
	virtual SulokiRet DelObject(SULOKI_IN std::string strUrName, SULOKI_OUT boost::shared_ptr<BaseRoot>& baseSmartPtr) = 0;
	virtual SulokiRet GetObject(SULOKI_IN std::string strUrName, SULOKI_OUT boost::shared_ptr<BaseRoot>& baseSmartPtr) = 0;
	virtual SulokiRet GetSqlData(SULOKI_IN std::string strUrName, SULOKI_INOUT suloki::SulokiMessage& msg, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL)) = 0;
	virtual SulokiRet AddNoSqlData(SULOKI_IN std::string strUrName, SULOKI_IN std::string& data, SULOKI_IN bool bDir = false, SULOKI_IN long timeout = 0, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL)) = 0;
	virtual SulokiRet DelNoSqlData(SULOKI_IN std::string strUrName, SULOKI_OUT std::string& data, SULOKI_IN long timeout = 0, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL)) = 0;
	virtual SulokiRet UpdateNoSqlData(SULOKI_IN std::string strUrName, SULOKI_INOUT std::string& data, SULOKI_IN long timeout = 0, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL)) = 0;
	virtual SulokiRet GetNoSqlData(SULOKI_IN std::string strUrName, SULOKI_OUT std::string& data, SULOKI_IN long timeout = 0, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL)) = 0;
	virtual SulokiRet GetUrDirectory(SULOKI_IN std::string strUrPath, SULOKI_OUT std::vector<std::string>& nameVector) = 0;// , long timeout = 0, SulokiAsyncCb asyncCb = NULL, std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL)) = 0;
protected:
	SulokiUrcModuleInterface(){}
private:
	SulokiUrcModuleInterface(SulokiUrcModuleInterface& ref) {}
	SulokiUrcModuleInterface& operator=(SulokiUrcModuleInterface& ref) { return *this; }
protected:
};

//handle module interface,dll/so
typedef SulokiRet(SULOKI_STDCALL *IhInit)(SULOKI_IN SulokiUrcModuleInterface* pSulokiUrcModuleInterface, SULOKI_IN std::string groupName, SULOKI_IN std::string serviceName, SULOKI_IN std::string strModuleName, SULOKI_IN std::string strConfig);
typedef SulokiRet(SULOKI_STDCALL *IhStart)(void);
typedef SulokiRet(SULOKI_STDCALL *IhTestValid)(SULOKI_IN const suloki::SulokiMessage& msg);
typedef SulokiRet(SULOKI_STDCALL *IhHandle)(SULOKI_IN std::auto_ptr< suloki::SulokiMessage > msgSmart, SULOKI_IN suloki::SulokiContext& context);
typedef SulokiRet(SULOKI_STDCALL *IhStop)(void);
typedef SulokiRet(SULOKI_STDCALL *IhClear)(void);
class SulokiHandleModuleInterface
{
public:
	virtual ~SulokiHandleModuleInterface(){}
	//
	virtual SulokiRet Init(SULOKI_IN SulokiUrcModuleInterface* pSulokiUrcModuleInterface, SULOKI_IN std::string groupName, SULOKI_IN std::string serviceName, SULOKI_IN std::string strModuleName, SULOKI_IN std::string strConfig) = 0;
	virtual SulokiRet Start(void) = 0;
	virtual SulokiRet TestValid(SULOKI_IN const suloki::SulokiMessage& msg) = 0;
	virtual SulokiRet Handle(SULOKI_IN std::auto_ptr< suloki::SulokiMessage > msgSmart, SULOKI_IN suloki::SulokiContext& context) = 0;
	virtual SulokiRet Stop(void) = 0;
	virtual SulokiRet Clear(void) = 0;
	//
protected:
	SulokiHandleModuleInterface(){}
	//explicit SulokiHandleModuleInterface(std::string modulePath){}// throw(Suloki::Exception){}
private:
	SulokiHandleModuleInterface(SulokiHandleModuleInterface& ref) {}
	SulokiHandleModuleInterface& operator=(SulokiHandleModuleInterface& ref) { return *this; }
protected:
};

#endif