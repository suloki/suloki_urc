#ifndef _SULOKI_H_
#define  _SULOKI_H_

#define SULOKI_OPENSOURCE_VERSION_GLOBAL
//#define SULOKI_FREE_VERSION_GLOBAL
//#define SULOKI_WINDOWS_OS_SULOKI
//must 64 bit,32bit maybe error
//#define SULOKI_64_PLATFORM_SULOKI
//test code
//#define SULOKI_DEBUG_BASEFRAMEWORK

#include <stdio.h>
#include <iostream>
#include <setjmp.h>
#include <list>
#include <memory> 
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <assert.h>
#include <math.h>
#include <sstream>
#include <time.h>
#include <exception>
#include <ctime>

#ifdef SULOKI_WINDOWS_OS_SULOKI
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <dlfcn.h>
#endif

//loki
#define LOKI_USE_REFERENCE
#include "TypeManip.h"
#include "./Reference/functor.h"

//boost
//#include <boost/coroutine2/all.hpp>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
//
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/common.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio/steady_timer.hpp>

#include "suloki_interface.h"

//namespace logging=boost::log;
//namespace src = boost::log::sources;
//namespace sinks = boost::log::sinks;
//namespace keywords = boost::log::keywords;
//namespace expr = boost::log::expressions;
//namespace attrs = boost::log::attributes;
#define SULOKI_TRACE_LOG_BASEFRAMEWORK if(Suloki::GetLogPtr()!=NULL)BOOST_LOG_SEV(*Suloki::GetLogPtr(), boost::log::trivial::severity_level::trace) << "<" << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << ">: "
#define SULOKI_DEBUG_LOG_BASEFRAMEWORK if(Suloki::GetLogPtr()!=NULL)BOOST_LOG_SEV(*Suloki::GetLogPtr(), boost::log::trivial::severity_level::debug) << "<" << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << ">: "
#define SULOKI_INFO_LOG_BASEFRAMEWORK if(Suloki::GetLogPtr()!=NULL)BOOST_LOG_SEV(*Suloki::GetLogPtr(), boost::log::trivial::severity_level::info) <<  "<" <<__FILE__ << " " << __LINE__ << " " << __FUNCTION__ << ">: "
#define SULOKI_WARN_LOG_BASEFRAMEWORK if(Suloki::GetLogPtr()!=NULL)BOOST_LOG_SEV(*Suloki::GetLogPtr(), boost::log::trivial::severity_level::warning) << "<" << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << ">: "
#define SULOKI_ERROR_LOG_BASEFRAMEWORK if(Suloki::GetLogPtr()!=NULL)BOOST_LOG_SEV(*Suloki::GetLogPtr(), boost::log::trivial::severity_level::error) << "<" << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << ">: "
#define SULOKI_FATAL_LOG_BASEFRAMEWORK if(Suloki::GetLogPtr()!=NULL)BOOST_LOG_SEV(*Suloki::GetLogPtr(), boost::log::trivial::severity_level::fatal) << "<" << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << ">: "

//#define SULOKI_INCREMENT_GLOBAL(pInt) __sync_add_and_fetch(pInt, 1)//__sync_fetch_and_add(pInt, 1)+1
//#define SULOKI_DECREMENT_GLOBAL(pInt) __sync_sub_and_fetch(pInt, 1)//__sync_fetch_and_sub(pInt, 1)-1
//atom
#ifdef SULOKI_WINDOWS_OS_SULOKI
#define SULOKI_CAS_GLOBAL( destination, exchange, comparand ) ( InterlockedCompareExchangePointer( (PVOID volatile *)destination, (PVOID)exchange, (PVOID)comparand ) != comparand ? true : false )
#else
//#define CAS( destination, exchange, comparand ) ( __sync_val_compare_and_swap( destination, comparand, exchange ) != comparand ? true : false )
#define SULOKI_CAS_GLOBAL( destination, exchange, comparand ) ( !__sync_bool_compare_and_swap( destination, comparand, exchange ) )
#endif
#ifdef SULOKI_WINDOWS_OS_SULOKI
#ifdef SULOKI_64_PLATFORM_SULOKI
#define SULOKI_INCREMENT_GLOBAL(pInt) InterlockedIncrement64(pInt)
#define SULOKI_DECREMENT_GLOBAL(pInt) InterlockedDecrement64(pInt)
#define SULOKI_INCREMENT64_GLOBAL(pInt) InterlockedIncrement64(pInt)
#define SULOKI_DECREMENT64_GLOBAL(pInt) InterlockedDecrement64(pInt)
#else
#define SULOKI_INCREMENT_GLOBAL(pInt) InterlockedIncrement((LONG volatile *)pInt)
#define SULOKI_DECREMENT_GLOBAL(pInt) InterlockedDecrement((LONG volatile *)pInt)
#define SULOKI_INCREMENT64_GLOBAL(pInt) InterlockedIncrement64((LONGLONG volatile *)pInt)
#define SULOKI_DECREMENT64_GLOBAL(pInt) InterlockedDecrement64((LONGLONG volatile *)pInt)
#endif
#else
#define SULOKI_INCREMENT_GLOBAL(pInt) __sync_fetch_and_add(pInt, 1)+1
#define SULOKI_DECREMENT_GLOBAL(pInt) __sync_fetch_and_sub(pInt, 1)-1
#define SULOKI_INCREMENT64_GLOBAL(pInt) __sync_fetch_and_add(pInt, 1)+1
#define SULOKI_DECREMENT64_GLOBAL(pInt) __sync_fetch_and_sub(pInt, 1)-1
#endif

namespace Suloki
{

//base simple type
#ifdef SULOKI_64_PLATFORM_SULOKI
typedef long long  Int;
typedef unsigned long long Uint;
typedef double Float;
#else
typedef long  Int;
typedef unsigned long Uint;
typedef float  Float;
#endif
typedef long long  Int64;
typedef long  Int32;
typedef short  Int16;
typedef char   Int8;
typedef unsigned long long Uint64;
typedef unsigned long Uint32;
typedef unsigned short Uint16;
typedef unsigned char  Uint8;
typedef double Float64;
typedef float  Float32;

typedef Int Ret;
//error code
const Ret SUCCESS = 0;
const Ret FAIL = 1;
//normal
const Ret INVALIDPARA_ERRORCODE = 2;
const Ret INVALIDSTATE_ERRORCODE = 3;
const Ret NOMEMORY_ERRORCODE = 4;
const Ret NOTHREAD_ERRORCODE = 5;
const Ret TIMEOUT_ERRORCODE = 6;
//about urc
const Ret URC_NAMETOOLONG_ERRORCODE = 10;
const Ret URC_EXISTED_ERRORCODE = 11;
const Ret URC_UNEXISTED_ERRORCODE = 12;
const Ret URC_NOTDIR_ERRORCODE = 13;
const Ret URC_CANNOTSUB_ERRORCODE = 14;
const Ret URC_SUBEXISTED_ERRORCODE = 15;
const Ret URC_SUBUNEXISTED_ERRORCODE = 16;
const Ret URC_INVALIDPATH_ERRORCODE = 17;//must be local/machine/remoted
const Ret URC_DECODEPROTO_ERRORCODE = 18;
const Ret URC_ENCODEPROTO_ERRORCODE = 19;
const Ret URC_DECODEBODYPROTO_ERRORCODE = 20;
const Ret URC_ENCODEBODYPROTO_ERRORCODE = 21;

const Int MAXNUM_THREAD = 256;
const Int MAXNUM_NETCONN = 10240;

typedef boost::shared_mutex RwLockPolicy;
typedef boost::unique_lock<RwLockPolicy>   WriteLock;
typedef boost::shared_lock<RwLockPolicy>   ReadLock;

typedef boost::log::sources::severity_logger<boost::log::trivial::severity_level> Log;
const Int MIN_LOGLEVEL = 0;
const Int MAX_LOGLEVEL = 5;
Log* GetLogPtr(void);
const std::string LOG_TRACE_LEVEL = "trace";
const std::string LOG_DEBUG_LEVEL = "debug";
const std::string LOG_INFO_LEVEL = "info";
const std::string LOG_WARN_LEVEL = "warning";
const std::string LOG_ERROR_LEVEL = "error";
const std::string LOG_FATAL_LEVEL = "fatal";
void SetLogLevel(std::string logLevel);

struct Exception : public std::exception
{
	explicit Exception(std::string strError):m_strError(strError)
	{}
	const char* what() const throw()
	{
		return m_strError.c_str();
	}
	std::string m_strError;
};

//global
const Int ZERO_GLOBALSTATE_BASEFRAMEWORK = 0;
const Int INIT_GLOBALSTATE_BASEFRAMEWORK = 1;
const Int START_GLOBALSTATE_BASEFRAMEWORK = 2;
const Int STOP_GLOBALSTATE_BASEFRAMEWORK = 3;
const Int CLEAR_GLOBALSTATE_BASEFRAMEWORK = 4;
class Global
{
public:
	inline static Ret SetState(Int state)
	{
		if(!(state >= INIT_GLOBALSTATE_BASEFRAMEWORK && state <= CLEAR_GLOBALSTATE_BASEFRAMEWORK))
			return INVALIDPARA_ERRORCODE;
		m_state = state;
		return SUCCESS;
	}
	inline static Int GetState(void)
	{
		return m_state;
	}
private:
	Global(){}
	~Global(){}
	Global(Global& ref){}
	Global& operator=(Global& ref){return *this;}
private:
	static Int m_state;//volatile
};

template < typename T >
class Singleton
{
public:
	inline static T& Instance(void) throw()
	{
		if (m_pInstance == NULL)
		{
			WriteLock lock(m_rwLock);
			if (m_pInstance == NULL)
			{
				m_pInstance = new T();
				if (m_pInstance == NULL)
					throw std::bad_alloc();//std::exception();
			}
		}
		return *m_pInstance;
	}
	inline static void Deinstance(void)
	{
		if (m_pInstance != NULL)
		{
			WriteLock lock(m_rwLock);
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
private:
	Singleton() {}
	~Singleton() {}
	Singleton(Singleton& ref) {}
	Singleton& operator=(Singleton& ref) { return *this; }
private:
	static RwLockPolicy m_rwLock;
	static T* m_pInstance;
};
template < typename T >
RwLockPolicy Singleton< T >::m_rwLock;
template < typename T >
T* Singleton< T >::m_pInstance = NULL;

void Sleep(Int mSec);

#ifdef SULOKI_OPENSOURCE_VERSION_GLOBAL
#else
#ifdef SULOKI_FREE_VERSION_GLOBAL
#else
#endif
#endif

#ifdef SULOKI_OPENSOURCE_VERSION_GLOBAL
class Base
{
public:
	Base(){}
	virtual ~Base(){}
private:
	Base(Base& ref){}
	Base& operator=(Base& ref){return *this;}
};
#else
#ifdef SULOKI_FREE_VERSION_GLOBAL
#else
#endif
#endif

#ifdef SULOKI_OPENSOURCE_VERSION_GLOBAL
#else
#ifdef SULOKI_FREE_VERSION_GLOBAL
#else
#endif
#endif

const std::string SULOKI_LOGNAME_KEY_CONFIG_BASE = "logname";//must exist
const std::string SULOKI_LOGLEVEL_KEY_CONFIG_BASE = "loglevel";//must exist
const std::string SULOKI_URCIP_KEY_CONFIG_BASE = "urcip";//must exist
const std::string SULOKI_URCPORT_KEY_CONFIG_BASE = "urcport";//must exist
const std::string SULOKI_URCSERVERADDR_KEY_CONFIG_BASE = "urcserveraddr";
const std::string SULOKI_URCTHREADNUM_KEY_CONFIG_BASE = "urcthreadnum";
const std::string SULOKI_GROUPNAME_KEY_CONFIG_BASE = "groupname";
const std::string SULOKI_SERVERNAME_KEY_CONFIG_BASE = "servername";
const std::string SULOKI_MODULEINFO_KEY_CONFIG_BASE = "moduleinfo";
//const std::string SULOKI_MODULENAME_KEY_CONFIG_BASE = "modulename";
//const std::string SULOKI_MODULESCRIPT_KEY_CONFIG_BASE = "modulescipt";
//const std::string SULOKI_ADDRESS_KEY_CONFIG_BASE = "address";
//const std::string SULOKI_URCSERVERADDR_BACK_KEY_CONFIG_BASE = "urcserveraddr_back";
//const std::string SULOKI_URCSERVERADDR_ALL_KEY_CONFIG_BASE = "/config/urcserveraddr_all";
class Config : public Base
{
public:
	class Module
	{
	public:
		std::string m_name;
		std::string m_path;
		std::string m_config;
	};
public:
	Config();
	virtual ~Config(){}
	virtual Ret ReadConfig(void);
	Ret SetConfig(std::string key, std::string val);
	Ret GetConfig(std::string key, std::string& val);
	template< typename T >
	Ret GetConfig(std::string key, std::vector<T>& tVector)
	{
		tVector.clear();
		ReadLock lock(m_rwLock);
		tVector = m_doduleVector;
		return SUCCESS;
	}
protected:
	void SetDefault(void);
private:
	Config(Config& ref){}
	Config& operator=(Config& ref){return *this;}
protected:
	RwLockPolicy m_rwLock;
	std::map<std::string, std::string> m_configMap;
	std::vector<Module> m_doduleVector;
};
typedef Singleton<Config> ConfigSingleton;

//proto
class SulokiProtoSwrap
{
public:
	static int MakeBaseMessage(suloki::SulokiMessage& proto)
	{
		proto.set_protocolversion("2.0.0.0");
		proto.set_businessid(SULOKI_SYSTEM_BISINESSID_PROTO);
		proto.set_messageid(SULOKI_TEST_MESSAGEID_SYSTEM_PROTO);
		proto.set_messagetype(suloki::SulokiMessage::notice);
		proto.set_sequencenumber(0);
		return SUCCESS;
	}
	static int MakeSimpleCopy(suloki::SulokiMessage& ori, suloki::SulokiMessage& back)
	{
		back.set_protocolversion(ori.protocolversion());
		back.set_businessid(ori.businessid());
		back.set_messageid(ori.messageid());
		back.set_messagetype(suloki::SulokiMessage::response);
		back.set_sequencenumber(ori.sequencenumber());
		return SUCCESS;
	}
	static int MakeResMessage(suloki::SulokiMessage& req, suloki::SulokiMessage& res)
	{
		res.set_protocolversion(req.protocolversion());
		res.set_businessid(req.businessid());
		res.set_messageid(req.messageid());
		res.set_messagetype(suloki::SulokiMessage::response);
		res.set_sequencenumber(req.sequencenumber());
		if (req.has_token())
			res.set_token(req.token());
		if (req.has_errorcode())
			res.set_errorcode(SUCCESS);
		if (req.has_urckey())
			res.set_urckey(req.urckey());
		//????? slow
		for (int i = 0; i < req.routers_size(); i++)
			res.add_routers(req.routers(i));
		return SUCCESS;
	}
	template <typename Tbody>
	static int SetBody(suloki::SulokiMessage& proto, Tbody& body)
	{
		std::string strBody;
		if (!body.SerializeToString(&strBody))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "SerializeToString error";
			return URC_ENCODEBODYPROTO_ERRORCODE;
		}
		proto.clear_messagebody();
		proto.set_messagebody(strBody.c_str(), strBody.length());
		return SUCCESS;
	}
	template <typename Tbody>
	static int GetBody(suloki::SulokiMessage& proto, Tbody& body)
	{
		if (!proto.has_messagebody())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no body error";
			return URC_DECODEBODYPROTO_ERRORCODE;
		}
		body.Clear();
		std::string strBody = proto.messagebody();
		if (!body.ParseFromString(strBody))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "ParseFromString error";
			return URC_DECODEBODYPROTO_ERRORCODE;
		}
		return SUCCESS;
	}
	template <typename Tbody>
	int DecodeBody(const char* pMsg, int msgLen, Tbody& body)
	{
		if (!body.ParseFromArray(pMsg, msgLen))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "ParseFromArray error";
			return URC_DECODEBODYPROTO_ERRORCODE;
		}
		return SUCCESS;
	}
	template <typename Tbody>
	int EncodeBody(const Tbody& body, std::string& strBody)
	{
		strBody.clear();
		if (!body.SerializeToString(&strBody))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "SerializeToString error";
			return URC_ENCODEBODYPROTO_ERRORCODE;
		}
		return SUCCESS;
	}
	template <typename Tbody>
	static int EncodeProtocol(suloki::SulokiMessage& proto, Tbody& body, std::string& strMsg)
	{
		strMsg.clear();
		if (!body.SerializeToString(&strMsg))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "SerializeToString error";
			return URC_ENCODEBODYPROTO_ERRORCODE;
		}
		proto.set_messagebody(strMsg.c_str(), strMsg.length());
		strMsg.clear();
		if (!proto.SerializeToString(&strMsg))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "SerializeToString error";
			return URC_ENCODEPROTO_ERRORCODE;
		}
		return SUCCESS;
	}
	static int EncodeProtocol(suloki::SulokiMessage& proto, std::string& strMsg)
	{
		strMsg.clear();
		if (!proto.SerializeToString(&strMsg))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "SerializeToString error";
			return URC_ENCODEPROTO_ERRORCODE;
		}
		return SUCCESS;
	}
	template <typename Tbody>
	static int DecodeProtocol(const char* pMsg, int msgLen, suloki::SulokiMessage& proto, Tbody& body)
	{
		if (pMsg == NULL || msgLen <= 0)
			return FAIL;
		proto.Clear();
		if (!proto.ParseFromArray(pMsg, msgLen))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "ParseFromArray error";
			return URC_DECODEPROTO_ERRORCODE;
		}
		if (!proto.has_messagebody())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no body error";
			return URC_DECODEBODYPROTO_ERRORCODE;
		}
		body.Clear();
		std::string strBody = proto.messagebody();
		if (!body.ParseFromArray(strBody.c_str(), strBody.length()))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "ParseFromArray error";
			return URC_DECODEBODYPROTO_ERRORCODE;
		}
		return SUCCESS;
	}
	static int DecodeProtocol(const char* pMsg, int msgLen, suloki::SulokiMessage& proto)
	{
		if (pMsg == NULL || msgLen <= 0)
			return URC_DECODEPROTO_ERRORCODE;
		proto.Clear();
		if (!proto.ParseFromArray(pMsg, msgLen))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "ParseFromArray error";
			return URC_DECODEPROTO_ERRORCODE;
		}
		return SUCCESS;
	}
	static int DecodeProtocol(const char* pMsg, int msgLen, std::auto_ptr<suloki::SulokiMessage>& protoSmart)
	{
		protoSmart = std::auto_ptr<suloki::SulokiMessage>(new suloki::SulokiMessage);
		if (protoSmart.get() == NULL)
			return NOMEMORY_ERRORCODE;
		if (pMsg == NULL || msgLen <= 0)
			return URC_DECODEPROTO_ERRORCODE;
		protoSmart->Clear();
		if (!protoSmart->ParseFromArray(pMsg, msgLen))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "ParseFromArray error";
			return URC_DECODEPROTO_ERRORCODE;
		}
		return SUCCESS;
	}
};

//queue
template <typename T>
class Queue : public Base
{
public:
	Queue() throw()
	{
		m_pQueue = new boost::lockfree::queue<T*, boost::lockfree::fixed_sized<false> >(0);
		if(m_pQueue == NULL)
			throw Exception("new boost::lockfree::queue exception");
#ifdef SULOKI_DEBUG_BASEFRAMEWORK
		if (m_pQueue->is_lock_free())
			std::cout << "lock free queue" << std::endl;
#endif
		m_size = 0;
	}
	virtual ~Queue()
	{
		Clear();
		if(m_pQueue == NULL)
		{
			delete m_pQueue;
			m_pQueue = NULL;
		}
	}
	inline Ret Push(std::auto_ptr<T> tSmart)// throw()
	{
		if (tSmart.get() == NULL)
			return INVALIDPARA_ERRORCODE;
		T* pItem = tSmart.get();
		bool bRes = m_pQueue->push(pItem);
		if (bRes)
		{
			SULOKI_INCREMENT_GLOBAL(&m_size);
			tSmart.release();
			return SUCCESS;
		}
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "boost::lockfree::queue push error";
		return FAIL;
	}
	inline std::auto_ptr<T> Pop(void)// throw()
	{
		T* pItem = NULL;
		bool bRes = m_pQueue->pop(pItem);
		if (bRes && pItem != NULL)
		{
			SULOKI_DECREMENT_GLOBAL(&m_size);
			return std::auto_ptr<T>(pItem);
		}
		return std::auto_ptr<T>(NULL);
	}
	inline void Clear(void)
	{
		while (true)
		{
			std::auto_ptr<T> tSmart = Pop();
			if (tSmart.get() == NULL)
				break;
		}
	}
	inline Int GetSize(void)
	{
		return m_size;
	}
private:
	Queue(Queue& ref) {}
	Queue& operator=(Queue& ref) { return *this; }
private:
	boost::lockfree::queue<T*, boost::lockfree::fixed_sized<false> > *m_pQueue;
	volatile Int m_size;
};

class IdManager : public Base
{
public:
	IdManager();
	//explicit IdManager(Int maxId)throw();
	IdManager(Int maxId, bool bCanfree)throw();
	virtual ~IdManager();
	Int GetFreeId(void);
	Ret FreeId(Int id);
private:
	IdManager(IdManager& ref) {}
	IdManager& operator=(IdManager& ref) { return *this; }
private:
	Queue<Int> m_idQueue;
	volatile Int m_idFree;
	bool m_bCanfree;
	Int m_maxId;
};
//?????
//default can't FreeId and increase increase
typedef Singleton<IdManager> IdManagerSingleton;

class ThreadPool : public Base
{
public:
	ThreadPool();
	virtual ~ThreadPool();
	Ret Init(Int threadNum);
	//template<typename F, typename...Args>
	//Ret Post(F &&f, Args&&...args)
	//{
	//	if(!m_bInited)
	//		return INVALIDSTATE_ERRORCODE;
	//	m_ioService.post(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	//	return SUCCESS;
	//}
	template<typename F>
	Ret Post(F func)
	{
		if(!m_bInited)
			return INVALIDSTATE_ERRORCODE;
		m_ioService.post(func);
		return SUCCESS;
	}
	void IncreseRef(void)
	{
		SULOKI_INCREMENT_GLOBAL(&m_refNumber);
	}
	void DecreseRef(void)
	{
		SULOKI_DECREMENT_GLOBAL(&m_refNumber);
	}
	Float GetThreadpoolBusydegree(void)
	{
		Float busyDegree = m_refNumber / (Float)m_threadNumber;
		if (busyDegree > 1.0f)
			busyDegree = 1.0f;
		return busyDegree;
	}
protected:
	//void Run(void);
	Ret Clear(void);
private:
	ThreadPool(ThreadPool& ref) :m_worker(m_ioService){}
	ThreadPool& operator=(ThreadPool& ref){ return *this; }
protected:
	bool m_bInited;
	bool m_bCleared;
	boost::asio::io_service m_ioService;
	boost::asio::io_service::work m_worker;
	boost::thread_group m_threads;
	//std::auto_ptr<boost::thread> m_threadRunnedSmart;
	volatile Int m_refNumber;
	Int m_threadNumber;
};

//double dispatch
template<typename Tprotocol, typename Tpara>
class Dispatcher : public Base
{
protected:
	typedef Loki::Functor<Int, TYPELIST_4(Tprotocol&, Tpara&, Tprotocol&, Tpara&)> HandlerFunctor;
	typedef Loki::Functor<Int, TYPELIST_2(Tprotocol&, Tpara&)> HandlerFunctorSimple;
public:
	Dispatcher(){}
	virtual ~Dispatcher(){}
	Ret Handle(Tprotocol& protocolReq, Tpara& paraIn, Tprotocol& protocolRes, Tpara& paraOut)
	{
		std::string strKey = CalKey(protocolReq);
		typename std::map< std::string, HandlerFunctor >::iterator iter = m_syncMsgHandlerMap.find(strKey);
		if (iter != m_syncMsgHandlerMap.end())
			return iter->second(protocolReq, paraIn, protocolRes, paraOut);
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "can't find handler, dispatch key:" << strKey;
		return FAIL;
	}
	Ret Handle(Tprotocol& protocolReq, Tpara& paraIn)
	{
		std::string strKey = CalKey(protocolReq);
		typename std::map< std::string, HandlerFunctorSimple >::iterator iter = m_syncMsgHandlerMapSimple.find(strKey);
		if (iter != m_syncMsgHandlerMapSimple.end())
			return iter->second(protocolReq, paraIn);
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "can't find handler simple, dispatch key:" << strKey;
		return HandleUnmatched(protocolReq, paraIn);
	}
protected:
	virtual std::string CalKey(Tprotocol& protocolReq)
	{
		return "";
	}
	virtual Ret HandleUnmatched(Tprotocol& protocolReq, Tpara& paraIn)
	{
		return FAIL;
	}
private:
	Dispatcher(Dispatcher& ref) {}
	Dispatcher& operator=(Dispatcher& ref) { return *this; }
protected:
	//????? no lock, must init in constructor
	std::map< std::string, HandlerFunctor > m_syncMsgHandlerMap;
	std::map< std::string, HandlerFunctorSimple > m_syncMsgHandlerMapSimple;
};

//Use inside by urc
class EventManager : public Base
{
public:
	EventManager()throw();
	virtual ~EventManager();
	void FreeEvent(Int id);
	Int GetFreeEvent(void);
	Ret Wait(Int id, std::string strUrcKey, Int timeout, std::auto_ptr<suloki::SulokiMessage>& msgSmart);
	Ret Notify(Int id, std::auto_ptr<suloki::SulokiMessage> msgSmart);
	Ret AsyncTimer(Int timeout, std::string strUrcKey);
protected:
	void MyAsyncTimeout(Uint timerPtr, std::string strUrcKey);
protected:
	void MyTimeout(Int id, std::string strUrcKey);
	void Run(void);
private:
	EventManager(EventManager& ref) {}
	EventManager& operator=(EventManager& ref) { return *this; }
protected:
	boost::condition_variable_any m_cond[MAXNUM_THREAD+2];
	boost::mutex m_mutex[MAXNUM_THREAD+2];
	bool m_ready[MAXNUM_THREAD+2];
	bool m_bTimeout[MAXNUM_THREAD + 2];
	std::auto_ptr<suloki::SulokiMessage> m_msgSmart[MAXNUM_THREAD + 2];
	std::auto_ptr<IdManager> m_idManagerSmart;
	//
	boost::asio::io_service m_ioService;
	boost::asio::steady_timer* m_timer[MAXNUM_THREAD + 2];
	std::auto_ptr<boost::thread> m_threadRunnedSmart;
	boost::asio::steady_timer* m_timerForever;
};
typedef Singleton<EventManager> EventManagerSingleton;

//????? net have not try exception
//net used by urc
class UrcTcpConnection : public BaseRoot, public boost::enable_shared_from_this<UrcTcpConnection>
{
	enum{MAXLEN_PROTOCOLHEAD=16, MAXLEN_BUFRECVED=8192};
	const std::string PROTOCOL_FLAG_VER = "flg_1.0_";
public:
	class UserData
	{
	public:
		UserData(){}
		explicit UserData(std::string strName)
		{
			m_strName = strName;
		}
		//this conn obj ur name
		std::string m_strName;
		std::string m_strServiceStateUrName;
	};
public:
	static boost::shared_ptr<UrcTcpConnection> Create(boost::asio::io_service& io_service, bool bServer = true)
	{
		boost::shared_ptr<UrcTcpConnection> connSmartPtr(new UrcTcpConnection(io_service));
		connSmartPtr->m_bServer = bServer;
		return connSmartPtr;
	}
protected:
	explicit UrcTcpConnection(boost::asio::io_service& io_service):m_socket(io_service),m_bServer(true)
	{
#ifdef SULOKI_DEBUG_BASEFRAMEWORK
		m_idTested = IdManagerTestedSingleton::Instance().GetFreeId();
		std::cout << "UrcTcpConnection, m_idTested:" << m_idTested << std::endl;
#endif
	}
public:
	virtual ~UrcTcpConnection()
	{
#ifdef SULOKI_DEBUG_BASEFRAMEWORK
		std::cout << "~UrcTcpConnection, m_idTested:" << m_idTested << std::endl;
#endif
	}
	boost::asio::ip::tcp::socket& GetSocketRef()
	{
		return m_socket;
	}
	void SetUserData(UserData userData)
	{
		m_userData = userData;
	}
	UserData GetUserData(void)
	{
		return m_userData;
	}
	void Cancel(void)
	{
		m_socket.cancel();
	}
	Ret WriteAsync(suloki::SulokiMessage& msg)//const char* pData, int dataLen)
	{
		if (m_bServer)
		{//pop router
			int routerSize = msg.routers_size();
			//????? slow
			std::vector<std::string> nameVec;
			for (Suloki::Int i = 0; i < routerSize - 1; i++)
				nameVec.push_back(msg.routers(i));
			msg.clear_routers();
			for (Suloki::Int i = 0; i < routerSize - 1; i++)
				msg.add_routers(nameVec[i]);
		}
		std::string strMsg;
		if (SulokiProtoSwrap::EncodeProtocol(msg, strMsg) != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "EncodeProtocol error";
			return Suloki::FAIL;
		}
		const char* pData = strMsg.c_str();
		int dataLen = strMsg.length();
		//if (!(pData != NULL && dataLen>0 && dataLen<99999999))
		//	return INVALIDPARA_ERRORCODE;
		char cFlag[MAXLEN_PROTOCOLHEAD + 2];
		sprintf(cFlag, "%s%08d", PROTOCOL_FLAG_VER.c_str(), dataLen);
		std::string strProtocol(cFlag, 16);
		strProtocol.append(pData, dataLen);
		boost::asio::async_write(m_socket, boost::asio::buffer(strProtocol.c_str(), strProtocol.length()),
			boost::bind(&UrcTcpConnection::handle_write, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
#ifdef SULOKI_DEBUG_BASEFRAMEWORK
		if (m_bServer)
			std::cout << "server send:" << std::string(pData, dataLen) << std::endl;
		else
			std::cout << "client send:" << std::string(pData, dataLen) << std::endl;
#endif
		return SUCCESS;
	}
	Ret WriteAsync(const char* pData, int dataLen)
	{
		if(!(pData!=NULL && dataLen>0 && dataLen<99999999))
			return INVALIDPARA_ERRORCODE;
		char cFlag[MAXLEN_PROTOCOLHEAD+2];
		sprintf(cFlag, "%s%08d", PROTOCOL_FLAG_VER.c_str(), dataLen);
		std::string strProtocol(cFlag, 16);
		strProtocol.append(pData, dataLen);
		boost::asio::async_write(m_socket, boost::asio::buffer(strProtocol.c_str(), strProtocol.length()),
			boost::bind(&UrcTcpConnection::handle_write, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
#ifdef SULOKI_DEBUG_BASEFRAMEWORK
		if(m_bServer)
			std::cout << "server send:" << std::string(pData, dataLen) << std::endl;
		else
			std::cout << "client send:" << std::string(pData, dataLen) << std::endl;
#endif
		return SUCCESS;
	}
	Ret ReadAsync(void)
	{
		m_socket.async_read_some(boost::asio::buffer(m_buffer, MAXLEN_BUFRECVED),
			boost::bind(&UrcTcpConnection::handle_read, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
		return SUCCESS;
	}
protected:
	virtual void HandleException(const boost::system::error_code& error);
	virtual void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);
	virtual void HandleRead(std::string& strMsg);
	void handle_write(const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (!error)
		{
			HandleWrite(error, bytes_transferred);
		}
		else
		{
			HandleException(error);
		}
	}
	void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (!error)
		{
			m_strData.append(m_buffer, bytes_transferred);
			if(m_strData.length() >= MAXLEN_PROTOCOLHEAD)
			{
				if(memcmp(m_strData.c_str(), PROTOCOL_FLAG_VER.c_str(), 8) != 0)
				{
#ifdef SULOKI_DEBUG_BASEFRAMEWORK
					std::cout << "recv bad protocol head,cancel will connect" << std::endl;
#endif
					m_socket.async_read_some(boost::asio::buffer(m_buffer, MAXLEN_BUFRECVED),
						boost::bind(&UrcTcpConnection::handle_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
					Cancel();
					return;
				}
				std::string strLen = m_strData.substr(8,8);
				int msgLen = atoi(strLen.c_str());
				if(m_strData.length() >= 16 + msgLen)
				{
					std::string strMsg = m_strData.substr(16,msgLen);
					if(m_strData.length() == 16 + msgLen)
						m_strData.clear();
					else
						m_strData = m_strData.substr(16 + msgLen);
					HandleRead(strMsg);
				}
			}
			//
			m_socket.async_read_some(boost::asio::buffer(m_buffer, MAXLEN_BUFRECVED),
				boost::bind(&UrcTcpConnection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			HandleException(error);
		}
	}
private:
	//UrcTcpConnection(UrcTcpConnection& ref) {}
	//UrcTcpConnection& operator=(UrcTcpConnection& ref) { return *this; }
protected:
	boost::asio::ip::tcp::socket m_socket;
	char m_buffer[MAXLEN_BUFRECVED+2];
	std::string m_strData;
	bool m_bServer;
	//
	UserData m_userData;
	//
#ifdef SULOKI_DEBUG_BASEFRAMEWORK
	Int m_idTested;
#endif
};
const std::string SULOKI_CONNECTPATH_NET_URC_BASE = "/urcsys/connection/";
const std::string SULOKI_CONNECT2URCPATH_NET_URC_BASE = "/urcsys/connection2urc/";
const std::string SULOKI_CONNECT2SERVICEPATH_NET_URC_BASE = "/urcsys/connection2service/";
class UrcTcpServer
{
//public:
	//const std::string CONNECT_PATH = "/local/connection/";
	//const std::string CONNECT2URC_PATH = "/local/connection2urc/";
public:
	UrcTcpServer()
	{}
	virtual ~UrcTcpServer();
	Ret Start(Int port);
	Ret Stop(void)
	{
		if(m_ioServiceSmart.get() != NULL)
			m_ioServiceSmart->stop();
		if(m_threadRunnedSmart.get() != NULL)
			m_threadRunnedSmart->join();
		return SUCCESS;
	}
	boost::shared_ptr<UrcTcpConnection> CreateClientConnection(void)
	{
		boost::shared_ptr<UrcTcpConnection> connSmartPtr = UrcTcpConnection::Create(m_acceptorSmart->get_io_service(), false);
		return connSmartPtr;
	}
protected:
	void Accept()
	{
		boost::shared_ptr<UrcTcpConnection> connSmartPtr = UrcTcpConnection::Create(m_acceptorSmart->get_io_service());
		m_acceptorSmart->async_accept(connSmartPtr->GetSocketRef(),
			boost::bind(&UrcTcpServer::handle_accept, this, connSmartPtr, boost::asio::placeholders::error));
	}
	virtual void HandleAccept(std::string strConnname, boost::shared_ptr<UrcTcpConnection> connSmartPtr);
	void handle_accept(boost::shared_ptr<UrcTcpConnection> connSmartPtr, const boost::system::error_code& error)
	{
		if (!error)
		{
			Int id = m_idManager.GetFreeId();
			if(id >= 0)
			{
				std::stringstream strStream;
				strStream << SULOKI_CONNECTPATH_NET_URC_BASE << id;
				UrcTcpConnection::UserData userData(strStream.str());
				connSmartPtr->SetUserData(userData);
				//if(MyUrcSingleton::Instance().AddUr(userData.m_strName, connSmartPtr, SULOKI_OBJECT_TYPE_URC_BASE, 0, true) != SUCCESS)
				//	std::cout << "AddUr error" << std::endl;
				//else
				connSmartPtr->ReadAsync();
				HandleAccept(userData.m_strName, connSmartPtr);
				//connSmartPtr->WriteAsync("asd", 3); 
				//m_connection = connSmartPtr;
			}
#ifdef SULOKI_DEBUG_BASEFRAMEWORK
			else
				std::cout << "no free id" << std::endl;
#endif
		}
		Accept();
	}
	void Run(void)
	{
#ifdef SULOKI_DEBUG_BASEFRAMEWORK
		std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << " start" << std::endl;
#endif
		m_ioServiceSmart->run();
#ifdef SULOKI_DEBUG_BASEFRAMEWORK
		std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << " end" << std::endl;
#endif
	}
private:
	UrcTcpServer(UrcTcpServer& ref) {}
	UrcTcpServer& operator=(UrcTcpServer& ref) { return *this; }
protected:
	std::auto_ptr<boost::asio::io_service> m_ioServiceSmart;
	std::auto_ptr<boost::thread> m_threadRunnedSmart;
	std::auto_ptr<boost::asio::ip::tcp::acceptor> m_acceptorSmart;
	IdManager m_idManager;
};
typedef Singleton<UrcTcpServer> UrcTcpServerSingleton;

const std::string SULOKI_LOCAL_RESOURCE_URC_BASE = "/local/";//same progass 
const std::string SULOKI_MACHINE_RESOURCE_URC_BASE = "/machine/";//same machine
const std::string SULOKI_REMOTED_RESOURCE_URC_BASE = "/remoted/";//remoted urc server,mean root global urc server
const std::string SULOKI_URCSYS_RESOURCE_URC_BASE = "/urcsys/";//this is urc reserved type,local
//
const std::string SULOKI_SERVICES_PATHNAME_URC_BASE = "services/";
//
const std::string SULOKI_SQL_NAME_URC_BASE = "/remoted/sqldata";
//urc type
const Int SULOKI_SQLDATA_TYPE_URC_BASE = 1;
const Int SULOKI_NOSQLDATA_TYPE_URC_BASE = 2;
const Int SULOKI_MSGQUEUE_TYPE_URC_BASE = 3;
const Int SULOKI_DIR_TYPE_URC_BASE = 4;
const Int SULOKI_OBJECT_TYPE_URC_BASE = 5;
const Int SULOKI_EVENT_TYPE_URC_BASE = 6;
//urc attrib
//const Int SULOKI_CACHE_GLOBAL_ATTR_URC_BASE = 1;//this bit is 0 mean not cache
//SULOKI_MSGQUEUE_TYPE_URC_BASE attribute
const Int SULOKI_EXCLUSIVE_MSGQUEUE_ATTR_URC_BASE = 2;//this bit is 0 mean shared
const Int SULOKI_SERIAL_MSGQUEUE_ATTR_URC_BASE = 4;//this bit is 0 mean parallel when SULOKI_EXCLUSIVE_MSGQUEUE_ATTR_URC_BASE have setted
const Int SULOKI_BROADCAST_MSGQUEUE_ATTR_URC_BASE = 8;//this bit is valid when SULOKI_EXCLUSIVE_MSGQUEUE_ATTR_URC_BASE have not setted
const Int SULOKI_BUSYDEGREE_MSGQUEUE_ATTR_URC_BASE = 16;//this bit is 0 mean one bu one when SULOKI_BROADCAST_MSGQUEUE_ATTR_URC_BASE have not setted
//
//const std::string SULOKI_SQLDIR_NAME_URC_BASE = "/remoted/sqldata/";
//const std::string SULOKI_SQLVIEWDIR_NAME_URC_BASE = "/remoted/sqldata/sqlview/";
//const std::string SULOKI_SQLQUERY_NAME_URC_BASE = "/remoted/sqldata/sqlquery";
//
#ifdef SULOKI_WINDOWS_OS_SULOKI
#define LOCAL_GETPROCADDR(myFunc, tFunc, nameFunc, bFlag) myFunc = (tFunc)GetProcAddress(m_hinstLib, nameFunc);\
	if (myFunc == NULL && bFlag)\
				{\
		std::stringstream sStream; sStream << "GetProcAddress " << nameFunc << " error" << std::endl; \
		throw Suloki::Exception(sStream.str());\
				}
#else
#define LOCAL_GETPROCADDR(myFunc, tFunc, nameFunc, bFlag) myFunc = (tFunc)dlsym(m_hinstLib, nameFunc);\
	szError = dlerror();\
	if ((myFunc == NULL || szError != NULL) && bFlag)\
				{\
		std::stringstream sStream; sStream << "dlsym " << nameFunc << " error" << std::endl; throw Suloki::Exception(sStream.str());\
				}
#endif
class SulokiCppHandleModule : public SulokiHandleModuleInterface
{
public:
	explicit SulokiCppHandleModule(std::string modulePath)throw(Exception)
	{
		//m_moduleType = BaseModuleInterface::CPPHMODULE;
		//
		m_bInit = false;
		m_MyInit = NULL;
		m_MyStart = NULL;
		m_MyTestValid = NULL;
		m_MyHandle = NULL;
		m_MyStop = NULL;
		m_MyClear = NULL;
		//
#ifdef SULOKI_WINDOWS_OS_SULOKI
		m_hinstLib = LoadLibrary(TEXT(modulePath.c_str()));
#else
		m_hinstLib = dlopen(modulePath.c_str(), RTLD_LAZY);
#endif
		if (m_hinstLib == NULL)
		{
			std::stringstream sStream;
#ifdef SULOKI_WINDOWS_OS_SULOKI
			sStream << "LoadLibrary " << modulePath << " error" << std::endl;
#else
			sStream << "LoadLibrary " << modulePath << " error:" << dlerror() << std::endl;
#endif
			throw Exception(sStream.str());
		}
		else
		{
			char* szError = NULL;
			//m_MyInit = (MyInit)GetProcAddress(m_hinstLib, "sfw_Init");
			//if (m_MyInit == NULL)
			//{
			//	std::stringstream sStream;sStream << "GetProcAddress " << "sfw_Init" << " error" << std::endl;throw Suloki::BaseException(sStream.str());
			//}
			LOCAL_GETPROCADDR(m_MyInit, IhInit, "Init", true);
			LOCAL_GETPROCADDR(m_MyStart, IhStart, "Start", true);
			LOCAL_GETPROCADDR(m_MyTestValid, IhTestValid, "TestValid", true);
			LOCAL_GETPROCADDR(m_MyHandle, IhHandle, "Handle", true);
			LOCAL_GETPROCADDR(m_MyStop, IhStop, "Stop", true);
			LOCAL_GETPROCADDR(m_MyClear, IhClear, "Clear", true);
			m_bInit = true;
		}
	}
	virtual ~SulokiCppHandleModule()
	{
		if (m_hinstLib != NULL)
#ifdef SULOKI_WINDOWS_OS_SULOKI
			FreeLibrary(m_hinstLib);
#else
			dlclose(m_hinstLib);
#endif
	}
	//
	virtual SulokiRet Init(SULOKI_IN SulokiUrcModuleInterface* pSulokiUrcModuleInterface, SULOKI_IN std::string groupName, SULOKI_IN std::string serviceName, SULOKI_IN std::string strModuleName, SULOKI_IN std::string strConfig)
	{
		if (!m_bInit)
			return INVALIDPARA_ERRORCODE;
		return (m_MyInit)(pSulokiUrcModuleInterface, groupName, serviceName, strModuleName, strConfig);
	}
	virtual SulokiRet Start(void)
	{
		if (!m_bInit)
			return INVALIDPARA_ERRORCODE;
		return (m_MyStart)();
	}
	virtual SulokiRet TestValid(SULOKI_IN const suloki::SulokiMessage& msg)
	{
		if (!m_bInit)
			return INVALIDPARA_ERRORCODE;
		return (m_MyTestValid)(msg);
	}
	virtual SulokiRet Handle(SULOKI_IN std::auto_ptr< suloki::SulokiMessage > msgSmart, SULOKI_IN suloki::SulokiContext& context)
	{
		if (!m_bInit)
			return INVALIDPARA_ERRORCODE;
		return (m_MyHandle)(msgSmart, context);
	}
	virtual SulokiRet Stop(void)
	{
		if (!m_bInit)
			return INVALIDPARA_ERRORCODE;
		return (m_MyStop)();
	}
	virtual SulokiRet Clear(void)
	{
		if (!m_bInit)
			return INVALIDPARA_ERRORCODE;
		return (m_MyClear)();
	}
	//
protected:
	//SulokiCppHandleModule(){}
	//explicit SulokiCppHandleModule(std::string modulePath){}// throw(Suloki::Exception){}
private:
	SulokiCppHandleModule(SulokiCppHandleModule& ref) {}
	SulokiCppHandleModule& operator=(SulokiCppHandleModule& ref) { return *this; }
protected:
	bool m_bInit;
#ifdef SULOKI_WINDOWS_OS_SULOKI
	HINSTANCE m_hinstLib;
#else
	void* m_hinstLib;
#endif
	IhInit m_MyInit;
	IhStart m_MyStart;
	IhTestValid m_MyTestValid;
	IhHandle m_MyHandle;
	IhStop m_MyStop;
	IhClear m_MyClear;
};
class Urc : public SulokiUrcModuleInterface
{
public:
	typedef Loki::Functor<void, TYPELIST_3(suloki::SulokiContext*, std::auto_ptr<suloki::SulokiMessage>, suloki::SulokiContext&)> AsyncFunc;
	typedef Loki::Functor<void, TYPELIST_2(std::auto_ptr<suloki::SulokiMessage>, suloki::SulokiContext&)> AsyncNewFunc;
private:
	enum{MAXLEN_NAME=64};
	class Module : public BaseRoot
	{
	public:
		Module(std::string name, boost::shared_ptr< SulokiHandleModuleInterface > handlerSmartPtr, std::string config)
		{
			m_name = name;
			m_handlerSmartPtr = handlerSmartPtr;
			m_config = config;
		}
		std::string m_name;
		boost::shared_ptr< SulokiHandleModuleInterface > m_handlerSmartPtr;
		std::string m_config;
	};
	class InfoSubscribed
	{
	public:
		InfoSubscribed():m_ref(0)
		{}
		AsyncNewFunc m_cb;
		Queue<suloki::SulokiMessage> m_queue;
		Int m_ref;
	};
	class Swaper
	{
	public:
		explicit Swaper(Int type, Int attrib):m_currentSubscriber(0)
		{m_type=type;m_attrib=attrib;}
		virtual ~Swaper()
		{
			for(std::map< std::string, InfoSubscribed* >::iterator iter=m_subscriberMap.begin();iter!=m_subscriberMap.end();iter++)
			{
				InfoSubscribed* pInfoSubscribed = (InfoSubscribed*)(iter->second);
				delete pInfoSubscribed;
			}
		}
		Int m_type;
		Int m_attrib;
		Int m_currentSubscriber;
		std::map< std::string, InfoSubscribed* > m_subscriberMap;
	}; 
	class SwaperDir : public Swaper
	{
	public:
		explicit SwaperDir(Int type):Swaper(type, 0)
		{}
		virtual ~SwaperDir()
		{}
		std::string m_strIndex;
	};
	template <typename T>
	class SwaperObject : public Swaper
	{
	public:
		SwaperObject(Int type, Int attrib, bool bDir, T val):Swaper(type, attrib)
		{m_val = val;m_bDir=bDir;}
		virtual ~SwaperObject()
		{}
		bool m_bDir;
		T m_val;
	};
public:
	Urc() :m_bRoot(false)
	{
		//boost::shared_ptr< SulokiHandleModuleInterface > handleSmart((SulokiHandleModuleInterface*)(new SulokiHandleModuleDefault("")));
		//m_handlerSmartPtr = handleSmart;
		m_mainHandlerSmartPtr = boost::shared_ptr< SulokiHandleModuleInterface >(NULL);
		//?????
	}
	virtual ~Urc()
	{
		WriteLock lock(m_rwLock);
		for(std::map<std::string, void*>::iterator iter=m_urcMap.begin();iter!=m_urcMap.end();iter++)
		{
			//????? free
			;
		}
	}
public://statemachine interface
	virtual Ret Init(void);
	virtual Ret Start(void);
	virtual Ret Stop(void);
	virtual Ret Clear(void);
public://
	Ret ConnAndReg(std::string strUrName, std::string ip, Uint port);
public://SulokiUrcModuleInterface
	virtual SulokiRet ReqRes(SULOKI_IN std::string strGroupName, SULOKI_IN std::string strServiceName, SULOKI_INOUT suloki::SulokiMessage& req, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL));
	virtual SulokiRet Notify(SULOKI_IN std::string strGroupName, SULOKI_IN std::string strServiceName, SULOKI_IN suloki::SulokiMessage& notice);
	virtual SulokiRet PostToMainModule(SULOKI_IN std::auto_ptr<suloki::SulokiMessage> msgSmart);
	virtual SulokiRet AddObject(SULOKI_IN std::string strUrName, SULOKI_IN boost::shared_ptr<BaseRoot>& baseSmartPtr);
	virtual SulokiRet DelObject(SULOKI_IN std::string strUrName, SULOKI_OUT boost::shared_ptr<BaseRoot>& baseSmartPtr);
	virtual SulokiRet GetObject(SULOKI_IN std::string strUrName, SULOKI_OUT boost::shared_ptr<BaseRoot>& baseSmartPtr);
	virtual SulokiRet GetSqlData(SULOKI_IN std::string strUrName, SULOKI_INOUT suloki::SulokiMessage& msg, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL));
	virtual SulokiRet AddNoSqlData(SULOKI_IN std::string strUrName, SULOKI_IN std::string& data, SULOKI_IN bool bDir = false, SULOKI_IN long timeout = 0, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL));
	virtual SulokiRet DelNoSqlData(SULOKI_IN std::string strUrName, SULOKI_OUT std::string& data, SULOKI_IN long timeout = 0, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL));
	virtual SulokiRet UpdateNoSqlData(SULOKI_IN std::string strUrName, SULOKI_INOUT std::string& data, SULOKI_IN long timeout = 0, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL));
	virtual SulokiRet GetNoSqlData(SULOKI_IN std::string strUrName, SULOKI_OUT std::string& data, SULOKI_IN long timeout = 0, SULOKI_IN SulokiAsyncCb asyncCb = NULL, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart = std::auto_ptr<suloki::SulokiContext>(NULL));
	virtual SulokiRet GetUrDirectory(SULOKI_IN std::string strUrPath, SULOKI_OUT std::vector<std::string>& nameVector);
protected:
	Ret SetThreadNum(Int threadNum)
	{
		return m_threadPool.Init(threadNum);
	}
	Ret SetRootFlag(void)
	{
		m_bRoot = true;
		return SUCCESS;
	}
public:
	Ret SetMainHandler(boost::shared_ptr< SulokiHandleModuleInterface > handlerSmartPtr)
	{
		m_mainHandlerSmartPtr = handlerSmartPtr;
		return SUCCESS;
	}
	Float GetBusydegree(void)
	{
		return m_threadPool.GetThreadpoolBusydegree();
	}
	bool GetRootFlag(void)
	{
		return m_bRoot;
	}
public://urc interface
	//advanced interface
	Ret ReqresMsgToUrcserver(std::string strUrName, suloki::SulokiMessage& msg, Int timeout, std::string strObjName = "");
	Ret ReqresMsgToUrcserver(std::string strUrName, suloki::SulokiMessage& msg, Int timeout, AsyncNewFunc asyncCb, std::string strObjName = "");
	Ret NotifyMsgToUrcserver(std::string strUrName, suloki::SulokiMessage& msg, std::string strObjName = "");
	//sync
	template <typename T>
	Ret AddUr(std::string strUrName, T& tVal, Int type, Int attrib = 0, bool bDir = false, Int timeout = 0)
	{
		if(!(strUrName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if(!(strUrName.length() > 0 && strUrName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		return AddUrIn(strUrName, tVal, type, attrib, bDir, timeout, Loki::Type2Type<T>());
	}
	//async
	template <typename T>
	Ret AddUr(std::string strUrName, T& tVal, Int type, Int attrib, bool bDir, Int timeout, AsyncNewFunc asyncCb)
	{
		if (!(strUrName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if (!(strUrName.length() > 0 && strUrName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		return AddUrIn(strUrName, tVal, type, attrib, bDir, timeout, asyncCb, Loki::Type2Type<T>());
	}
protected:
	//for sync call
	void FuncRes(suloki::SulokiContext* pContextOri, std::auto_ptr<suloki::SulokiMessage> msgSmart, suloki::SulokiContext& contextNew)
	{
		std::auto_ptr<suloki::SulokiContext> contextSmart(pContextOri);
		if (!contextSmart->has_id())
		{
			SULOKI_INFO_LOG_BASEFRAMEWORK << "contextSmart has not id field error";
			return;
		}
		EventManagerSingleton::Instance().Notify(contextSmart->id(), msgSmart);
	}
	//void FuncRes(Int eventId, Uint msgPtr, bool bTimeout)//std::auto_ptr<suloki::SulokiMessage> msgSmart)
	//{
	//	suloki::SulokiMessage* pTprotocol = (suloki::SulokiMessage*)msgPtr;
	//	std::auto_ptr<suloki::SulokiMessage> msgSmart(pTprotocol);
	//	EventManagerSingleton::Instance().Notify(eventId, msgSmart);
	//}
protected:
	template <typename T>
	Ret AddUrIn(std::string strUrName, T& tVal, Int type, Int attrib, bool bDir, Int timeout, Loki::Type2Type< std::string >)
	{
		if(!(type == SULOKI_NOSQLDATA_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		if (!(strUrName.find(SULOKI_LOCAL_RESOURCE_URC_BASE) == 0 || strUrName.find(SULOKI_URCSYS_RESOURCE_URC_BASE) == 0))
			return URC_INVALIDPATH_ERRORCODE;
		return AddUrInIn(strUrName, tVal, type, attrib, bDir);
	}
	template <typename T, typename Tptr>
	Ret AddUrIn(std::string strUrName, T& tVal, Int type, Int attrib, bool bDir, Int timeout, Loki::Type2Type< boost::shared_ptr<Tptr> >)
	{
		if(!(type == SULOKI_OBJECT_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		if (!(strUrName.find(SULOKI_LOCAL_RESOURCE_URC_BASE) == 0 || strUrName.find(SULOKI_URCSYS_RESOURCE_URC_BASE) == 0))
			return URC_INVALIDPATH_ERRORCODE;
		return AddUrInIn(strUrName, tVal, type, attrib, bDir);
	}
	template <typename T>//, typename Tpcb>
	Ret AddUrIn(std::string strUrName, T& tVal, Int type, Int attrib, bool bDir, Int timeout, Loki::Type2Type< AsyncNewFunc >)
	{
		if(!(type == SULOKI_EVENT_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		if (!(strUrName.find(SULOKI_LOCAL_RESOURCE_URC_BASE) == 0 || strUrName.find(SULOKI_URCSYS_RESOURCE_URC_BASE) == 0))
			return URC_INVALIDPATH_ERRORCODE;
		return AddUrInIn(strUrName, tVal, type, attrib, bDir);
	}
	//
	template <typename T>
	Ret AddUrIn(std::string strUrName, T& tVal, Int type, Int attrib, bool bDir, Loki::Type2Type< Queue<suloki::SulokiMessage> >)
	{
		if(!(type == SULOKI_MSGQUEUE_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		//boost::shared_ptr< Queue<suloki::SulokiMessage> > queueSmartPtr(new Queue<suloki::SulokiMessage>);
		//if(queueSmartPtr.get() == NULL)
		//	return NOMEMORY;
		return AddUrInIn(strUrName, tVal, type, attrib, bDir);
	}
	//
	template <typename T>
	Ret AddUrInIn(std::string strUrName, T& tVal, Int type, Int attrib, bool bDir)
	{
		std::auto_ptr< SwaperObject<T> > objSmart(new SwaperObject<T>(type, attrib, bDir, tVal));
		if(objSmart.get() == NULL)
			return NOMEMORY_ERRORCODE;
		std::auto_ptr< SwaperDir > objDirSmart;
		std::string strPath;
		char cName[MAXLEN_NAME+2];
		if(bDir)
		{
			size_t pos = strUrName.rfind('/');
			strPath = strUrName.substr(0, pos+1);
			//std::cout << strPath << std::endl;
			objDirSmart = std::auto_ptr< SwaperDir >(new SwaperDir(SULOKI_DIR_TYPE_URC_BASE));
			memset(cName, 0, MAXLEN_NAME+2);
			//std::string strLastName = strUrName.substr(pos+1);
			//std::cout << strLastName << std::endl;
			sprintf(cName, "%s", strUrName.c_str());
			objDirSmart->m_strIndex.append(cName, MAXLEN_NAME);
		}
		WriteLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if(iter != m_urcMap.end())
			return URC_EXISTED_ERRORCODE;
		m_urcMap.insert(std::pair<std::string, void*>(strUrName, (void*)objSmart.get()));
		objSmart.release();
		if(bDir)
		{
			std::map<std::string, void*>::iterator iter = m_urcMap.find(strPath);
			if(iter != m_urcMap.end())
			{
				std::string& strIndex = ((SwaperDir*)(iter->second))->m_strIndex;
				//char name[MAXLEN_NAME+2];
				memset(cName, 0, MAXLEN_NAME+2);
				//std::string strLastName = strUrName.substr(pos+1);
				sprintf(cName, "%s", strUrName.c_str());
				strIndex.append(cName, MAXLEN_NAME);
			}
			else
			{
				m_urcMap.insert(std::pair<std::string, void*>(strPath, (void*)objDirSmart.get()));
				objDirSmart.release();
			}
		}
		return SUCCESS;
	}
public:
	template <typename T>
	Ret DelUr(std::string strUrName, T& tVal)
	{
		if(!(strUrName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if(!(strUrName.length() > 0 && strUrName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		if (strUrName.find(SULOKI_LOCAL_RESOURCE_URC_BASE) == 0)
		{
			//strUrName = strUrName.substr(SULOKI_LOCAL_RESOURCE_URC_BASE.length()-1);
			//WriteLock lock(m_rwLock);
			//std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
			//if(iter == m_urcMap.end())
			//	return URC_UNEXISTED;
			return DelUrIn(strUrName, tVal, Loki::Type2Type<T>());
		}
		else
		if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) == 0)
		{
			//strUrName = strUrName.substr(SULOKI_REMOTED_RESOURCE_URC_BASE.length()-1);
		}
		return URC_INVALIDPATH_ERRORCODE;
	}
protected:
	template <typename T>
	Ret DelUr_Urcsys(std::string strUrName, T& tVal)
	{
		if (!(strUrName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if (!(strUrName.length() > 0 && strUrName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		if (strUrName.find(SULOKI_URCSYS_RESOURCE_URC_BASE) == 0)
		{
			//strUrName = strUrName.substr(SULOKI_LOCAL_RESOURCE_URC_BASE.length()-1);
			//WriteLock lock(m_rwLock);
			//std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
			//if (iter == m_urcMap.end())
			//	return URC_UNEXISTED;
			return DelUrIn(strUrName, tVal, Loki::Type2Type<T>());
		}
		return URC_INVALIDPATH_ERRORCODE;
	}
protected:
	template <typename T>
	Ret DelUrIn(std::string strUrName, T& tVal, Loki::Type2Type< std::string >)
	{
		WriteLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if (iter == m_urcMap.end())
			return URC_UNEXISTED_ERRORCODE;
		Int type = ((Swaper*)(iter->second))->m_type;
		if(!(type == SULOKI_NOSQLDATA_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		return DelUrInIn(iter, strUrName, tVal, type);
	}
	template <typename T, typename Tptr>
	Ret DelUrIn(std::string strUrName, T& tVal, Loki::Type2Type< boost::shared_ptr<Tptr> >)
	{
		WriteLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if (iter == m_urcMap.end())
			return URC_UNEXISTED_ERRORCODE;
		Int type = ((Swaper*)(iter->second))->m_type;
		if(!(type == SULOKI_OBJECT_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		return DelUrInIn(iter, strUrName, tVal, type);
	}
	template <typename T>//, typename Tpcb>
	Ret DelUrIn(std::string strUrName, T& tVal, Loki::Type2Type< AsyncNewFunc >)
	{
		WriteLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if (iter == m_urcMap.end())
			return URC_UNEXISTED_ERRORCODE;
		Int type = ((Swaper*)(iter->second))->m_type;
		if(!(type == SULOKI_EVENT_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		return DelUrInIn(iter, strUrName, tVal, type);
	}
	//
	template <typename T>
	Ret DelUrIn(std::string strUrName, T& tVal, Loki::Type2Type< Queue<suloki::SulokiMessage> >)
	{
		//if(!(type == SULOKI_MSGQUEUE_TYPE_URC_BASE))
		//	return INVALIDPARA;
		//return DelUrInIn(iter, strUrName, tVal, type);
		WriteLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if (iter == m_urcMap.end())
			return URC_UNEXISTED_ERRORCODE;
		Int type = ((Swaper*)(iter->second))->m_type;
		if (!(type == SULOKI_MSGQUEUE_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		return DelUrInIn(iter, strUrName, tVal, type);
	}
	//
	template <typename T>
	Ret DelUrInIn(std::map<std::string, void*>::iterator iter, std::string strUrName, T& tVal, Int type)
	{
		std::auto_ptr< SwaperObject<T> > objSmart((SwaperObject<T>*)(iter->second));
		tVal = objSmart->m_val;
		m_urcMap.erase(iter);
		if(objSmart->m_bDir)
		{
			std::string strPath;
			size_t pos = strUrName.rfind('/');
			strPath = strUrName.substr(0, pos+1);
			std::map<std::string, void*>::iterator iter = m_urcMap.find(strPath);
			if(iter != m_urcMap.end())
			{
				std::string& strIndex = ((SwaperDir*)(iter->second))->m_strIndex;
				size_t pos = strIndex.find(strUrName);
				if(pos != std::string::npos)
				{
					strIndex.erase(pos, MAXLEN_NAME);
					if(strIndex.length() == 0)
					{
						std::auto_ptr< SwaperDir > objDirSmart((SwaperDir*)(iter->second));
						//tVal = objSmart->m_val;
						m_urcMap.erase(iter);
					}
				}
			}
		}
		return SUCCESS;
	}
public:
	template <typename T>
	Ret UpdateUr(std::string strUrName, T& tVal, T& tValOld)
	{
		if(!(strUrName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if(!(strUrName.length() > 0 && strUrName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		if(strUrName.find(SULOKI_LOCAL_RESOURCE_URC_BASE) == 0)
		{
			//strUrName = strUrName.substr(SULOKI_LOCAL_RESOURCE_URC_BASE.length()-1);
			//WriteLock lock(m_rwLock);
			//std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
			//if(iter == m_urcMap.end())
			//	return URC_UNEXISTED;
			return UpdateUrIn(strUrName, tVal, tValOld, Loki::Type2Type<T>());
		}
		else
		if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) == 0)
		{
			//strUrName = strUrName.substr(SULOKI_REMOTED_RESOURCE_URC_BASE.length()-1);
		}
		return URC_INVALIDPATH_ERRORCODE;
	}
protected:
	template <typename T>
	Ret UpdateUrIn(std::string strUrName, T& tVal, T& tValOld, Loki::Type2Type< std::string >)
	{
		WriteLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if (iter == m_urcMap.end())
			return URC_UNEXISTED_ERRORCODE;
		Int type = ((Swaper*)(iter->second))->m_type;
		if(!(type == SULOKI_NOSQLDATA_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		return UpdateUrInIn(iter, strUrName, tVal, tValOld, type);
	}
	template <typename T>
	Ret UpdateUrInIn(std::map<std::string, void*>::iterator iter, std::string strUrName, T& tVal, T& tValOld, Int type)
	{
		SwaperObject<T>* pSwaperObject = ((SwaperObject<T>*)(iter->second));
		tValOld = pSwaperObject->m_val;
		pSwaperObject->m_val = tVal;
		return SUCCESS;
	}
public:
	template <typename T>
	Ret GetUr(std::string strUrName, T& tVal, Int timeout = 0)
	{
		if(!(strUrName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if(!(strUrName.length() > 0 && strUrName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		//if(strUrName.find(SULOKI_LOCAL_RESOURCE_URC_BASE) == 0)
		//{
			//strUrName = strUrName.substr(SULOKI_LOCAL_RESOURCE_URC_BASE.length()-1);
			/*ReadLock lock(m_rwLock);
			std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
			if(iter == m_urcMap.end())
				return URC_UNEXISTED;*/
			//return GetUrIn(iter, strUrName, tVal, ((Swaper*)(iter->second))->m_type, Loki::Type2Type<T>());
		//}
		//else
		//if (strUrName == SULOKI_SQL_NAME_URC_BASE)
		//{
			//strUrName = strUrName.substr(SULOKI_REMOTED_RESOURCE_URC_BASE.length()-1);
		//	return GetUrIn(strUrName, tVal, timeout, asyncCb, Loki::Type2Type<T>());
		//}
		//return URC_INVALIDPATH;
		return GetUrIn(strUrName, tVal, timeout, Loki::Type2Type<T>());
	}
	template <typename T>
	Ret GetUr(std::string strUrName, T& tVal, Int timeout, AsyncNewFunc asyncCb)
	{
		if (!(strUrName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if (!(strUrName.length() > 0 && strUrName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		return GetUrIn(strUrName, tVal, timeout, asyncCb, Loki::Type2Type<T>());
	}
protected:
	template <typename T>
	Ret GetUr_Urcsys(std::string strUrName, T& tVal)
	{
		if (!(strUrName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if (!(strUrName.length() > 0 && strUrName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		if (strUrName.find(SULOKI_URCSYS_RESOURCE_URC_BASE) == 0)
		{
			//strUrName = strUrName.substr(SULOKI_LOCAL_RESOURCE_URC_BASE.length()-1);
			//ReadLock lock(m_rwLock);
			//std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
			//if (iter == m_urcMap.end())
			//	return URC_UNEXISTED;
			//return GetUrIn(iter, strUrName, tVal, ((Swaper*)(iter->second))->m_type, Loki::Type2Type<T>());
			return GetUrIn(strUrName, tVal, 0, Loki::Type2Type<T>());
		}
		return URC_INVALIDPATH_ERRORCODE;
	}
protected:
	template <typename T>
	Ret GetUrIn(std::string strUrName, T& tVal, Int timeout, Loki::Type2Type< suloki::SulokiMessage >)
	{
		if (strUrName != SULOKI_SQL_NAME_URC_BASE)
			return INVALIDPARA_ERRORCODE;
		//if (!(type == SULOKI_SQLDATA_TYPE_URC_BASE && timeout > 0))
		//	return INVALIDPARA;
		//if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
		//	return URC_INVALIDPATH;
		{//sync
			//std::cout << "sql sync" << std::endl;
			std::vector<std::string> nameVector;
			if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
				return FAIL;
			}
			//boost::shared_ptr<UrcTcpConnection> connSmartPtr;
			//if (!(GetUr_Urcsys(nameVector[0], connSmartPtr) == SUCCESS && connSmartPtr.get() != NULL))
			//{
			//	SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server fail";
			//	return FAIL;
			//}
			boost::shared_ptr<BaseRoot> baseSmartPtr;
			if (!(GetUr_Urcsys(nameVector[0], baseSmartPtr) == SUCCESS && baseSmartPtr.get() != NULL))
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server fail";
				return FAIL;
			}
			boost::shared_ptr<UrcTcpConnection> connSmartPtr = boost::dynamic_pointer_cast<UrcTcpConnection>(baseSmartPtr);
			if (connSmartPtr.get() == NULL)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server1 fail";
				return FAIL;
			}
			std::string strMsg;
			SulokiProtoSwrap::EncodeProtocol(tVal, strMsg);
			//
			Int id = EventManagerSingleton::Instance().GetFreeEvent();
			if (id < 0)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "get GetFreeEvent fail";
				return FAIL;
			}
			std::stringstream strStream;
			if (tVal.routers_size() > 0)//tVal.routers(tVal.routers_size() - 1) << "_" <<
				strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << tVal.routers(tVal.routers_size() - 1) << "_" << tVal.businessid() << "_" << tVal.messageid() << "_" << tVal.sequencenumber();
			else
				strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << tVal.businessid() << "_" << tVal.messageid() << "_" << tVal.sequencenumber();
			//boost::function<void(Int, Uint, bool)> func = boost::bind(&Urc::FuncRes, this, id, _1, _1);
			//suloki::SulokiContext context;
			std::auto_ptr<suloki::SulokiContext> contextSmart(new suloki::SulokiContext());
			if (contextSmart.get() == NULL)
				return FAIL;
			//context.set_id(id);
			contextSmart->set_id(id);
			suloki::SulokiMessage* pMsgBack = contextSmart->mutable_msgori();
			if (pMsgBack == NULL)
				return FAIL;
			SulokiProtoSwrap::MakeSimpleCopy(tVal, *pMsgBack);
			AsyncNewFunc func(BindFirst(AsyncFunc(this, &Urc::FuncRes), contextSmart.release()));
			if (AddUrIn(strStream.str(), func, SULOKI_EVENT_TYPE_URC_BASE, 0, false, -1, Loki::Type2Type<AsyncNewFunc>()) != SUCCESS)
			{
				EventManagerSingleton::Instance().FreeEvent(id);
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUrIn fail";
				return FAIL;
			}
			//
			connSmartPtr->WriteAsync(strMsg.c_str(), strMsg.length());
			std::auto_ptr<suloki::SulokiMessage> resSmart;
			if (EventManagerSingleton::Instance().Wait(id, strStream.str(), timeout, resSmart) != SUCCESS)
				return TIMEOUT_ERRORCODE;
			tVal = *resSmart;
			if (!resSmart->has_errorcode())
				return FAIL;
			return resSmart->errorcode();
		}
		return URC_INVALIDPATH_ERRORCODE;
	}
	template <typename T>
	Ret GetUrIn(std::string strUrName, T& tVal, Int timeout, AsyncNewFunc asyncCb, Loki::Type2Type< suloki::SulokiMessage >)
	{
		if (strUrName != SULOKI_SQL_NAME_URC_BASE)
			return INVALIDPARA_ERRORCODE;
		//if (!(type == SULOKI_SQLDATA_TYPE_URC_BASE && timeout > 0))
		//	return INVALIDPARA;
		//if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
		//	return URC_INVALIDPATH;
		{//async
			//std::cout << "sql async" << std::endl;
			std::vector<std::string> nameVector;
			if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
				return FAIL;
			}
			//boost::shared_ptr<UrcTcpConnection> connSmartPtr;
			//if (!(GetUr_Urcsys(nameVector[0], connSmartPtr) == SUCCESS && connSmartPtr.get() != NULL))
			//{
			//	SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server fail";
			//	return FAIL;
			//}
			boost::shared_ptr<BaseRoot> baseSmartPtr;
			if (!(GetUr_Urcsys(nameVector[0], baseSmartPtr) == SUCCESS && baseSmartPtr.get() != NULL))
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server fail";
				return FAIL;
			}
			boost::shared_ptr<UrcTcpConnection> connSmartPtr = boost::dynamic_pointer_cast<UrcTcpConnection>(baseSmartPtr);
			if (connSmartPtr.get() == NULL)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server1 fail";
				return FAIL;
			}
			std::string strMsg;
			SulokiProtoSwrap::EncodeProtocol(tVal, strMsg);
			//
			std::stringstream strStream;
			if (tVal.routers_size() > 0)//tVal.routers(tVal.routers_size() - 1) << "_" <<
				strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << tVal.routers(tVal.routers_size() - 1) << "_" << tVal.businessid() << "_" << tVal.messageid() << "_" << tVal.sequencenumber();
			else
				strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << tVal.businessid() << "_" << tVal.messageid() << "_" << tVal.sequencenumber();
			//boost::function<void(Int, Uint, bool)> func = boost::bind(&Urc::FuncRes, this, id, _1, _1);
			if (AddUrIn(strStream.str(), asyncCb, SULOKI_EVENT_TYPE_URC_BASE, 0, false, -1, Loki::Type2Type<AsyncNewFunc>()) != SUCCESS)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUrIn fail";
				return FAIL;
			}
			//
			if (EventManagerSingleton::Instance().AsyncTimer(timeout, strStream.str()) != SUCCESS)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "AsyncTimer fail";
				//boost::function<void(Uint, bool)> func;
				AsyncNewFunc func;
				if (DelUr_Urcsys(strStream.str(), func) != SUCCESS)
				{
					SULOKI_ERROR_LOG_BASEFRAMEWORK << "DelUr_Urcsys fail, urc name:" << strStream.str();
				}
				return FAIL;
			}
			//
			connSmartPtr->WriteAsync(strMsg.c_str(), strMsg.length());
			return SUCCESS;
		}
		return URC_INVALIDPATH_ERRORCODE;
	}
	template <typename T>
	Ret GetUrIn(std::string strUrName, T& tVal, Int timeout, Loki::Type2Type< std::string >)
	{
		ReadLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if (iter == m_urcMap.end())
			return URC_UNEXISTED_ERRORCODE;
		if (!(((Swaper*)(iter->second))->m_type == SULOKI_NOSQLDATA_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		//return GetUrInIn(iter, strUrName, tVal, type);
		//std::auto_ptr< SwaperObject<T> > objSmart((SwaperObject<T>*)(iter->second));
		tVal = ((SwaperObject<T>*)(iter->second))->m_val;
		return SUCCESS;
	}
	template <typename T, typename Tptr>
	Ret GetUrIn(std::string strUrName, T& tVal, Int timeout, Loki::Type2Type< boost::shared_ptr<Tptr> >)
	{
		ReadLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if (iter == m_urcMap.end())
			return URC_UNEXISTED_ERRORCODE;
		if (!(((Swaper*)(iter->second))->m_type == SULOKI_OBJECT_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		//return GetUrInIn(iter, strUrName, tVal, type);
		//std::auto_ptr< SwaperObject<T> > objSmart((SwaperObject<T>*)(iter->second));
		tVal = ((SwaperObject<T>*)(iter->second))->m_val;
		return SUCCESS;
	}
	template <typename T>//, typename Tpcb>
	Ret GetUrIn(std::string strUrName, T& tVal, Int timeout, Loki::Type2Type< AsyncNewFunc >)
	{
		ReadLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if (iter == m_urcMap.end())
			return URC_UNEXISTED_ERRORCODE;
		if (!(((Swaper*)(iter->second))->m_type == SULOKI_EVENT_TYPE_URC_BASE))
			return INVALIDPARA_ERRORCODE;
		//return GetUrInIn(iter, strUrName, tVal, type);
		//std::auto_ptr< SwaperObject<T> > objSmart((SwaperObject<T>*)(iter->second));
		tVal = ((SwaperObject<T>*)(iter->second))->m_val;
		return SUCCESS;
	}
	//template <typename T>
	//Ret GetUrInIn(std::map<std::string, void*>::iterator iter, std::string strUrName, T& tVal, Int type)
	//{
	//	//std::auto_ptr< SwaperObject<T> > objSmart((SwaperObject<T>*)(iter->second));
	//	tVal = ((SwaperObject<T>*)(iter->second))->m_val;
	//	return SUCCESS;
	//}
public:
	/*
	Ret SubscribeUr(std::string strUrName, std::string strModuleName, AsyncNewFunc func)
	{
		if(!(strUrName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if(!(strUrName.length() > 0 && strUrName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		if(!(strModuleName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if(!(strModuleName.length() > 0 && strModuleName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		//ReadLock lock(m_rwLock);
		WriteLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if(iter == m_urcMap.end())
			return URC_UNEXISTED_ERRORCODE;
		Swaper* pSwaper = (Swaper*)(iter->second);
		if(!(pSwaper->m_type == SULOKI_MSGQUEUE_TYPE_URC_BASE))
			return URC_CANNOTSUB_ERRORCODE;
		std::map< std::string, InfoSubscribed* >::iterator iterSub = pSwaper->m_subscriberMap.find(strModuleName);
		if(iterSub != pSwaper->m_subscriberMap.end())
			return URC_SUBEXISTED_ERRORCODE;
		std::auto_ptr<InfoSubscribed> subSmart(new InfoSubscribed());
		if (subSmart.get() == NULL)
			return NOMEMORY_ERRORCODE;
		subSmart->m_cb = func;
		pSwaper->m_subscriberMap.insert(std::pair< std::string, InfoSubscribed*  >(strModuleName, subSmart.release()));
		return SUCCESS;
	}
	Ret UnsubscribeUr(std::string strUrName, std::string strModuleName)
	{
		if(!(strUrName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if(!(strUrName.length() > 0 && strUrName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		if(!(strModuleName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if(!(strModuleName.length() > 0 && strModuleName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		//ReadLock lock(m_rwLock);
		WriteLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if(iter == m_urcMap.end())
			return URC_UNEXISTED_ERRORCODE;
		Swaper* pSwaper = (Swaper*)(iter->second);
		if(!(pSwaper->m_type == SULOKI_MSGQUEUE_TYPE_URC_BASE))
			return URC_CANNOTSUB_ERRORCODE;
		std::map< std::string, InfoSubscribed* >::iterator iterSub = pSwaper->m_subscriberMap.find(strModuleName);
		if(iterSub == pSwaper->m_subscriberMap.end())
			return URC_SUBUNEXISTED_ERRORCODE;
		std::auto_ptr<InfoSubscribed> subSmart(iterSub->second);
		pSwaper->m_subscriberMap.erase(iterSub);
		return SUCCESS;
	}
	Ret PushMsg(std::string strUrName, std::string strModuleName, std::auto_ptr<suloki::SulokiMessage> msgSmartPtr)
	{
		if (msgSmartPtr.get() == NULL)
			return INVALIDPARA_ERRORCODE;
		if (!(strUrName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if (!(strUrName.length() > 0 && strUrName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		if (!(strModuleName.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if (!(strModuleName.length() > 0 && strModuleName.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		//ReadLock lock(m_rwLock);
		WriteLock lock(m_rwLock);
		std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrName);
		if (iter == m_urcMap.end())
			return URC_UNEXISTED_ERRORCODE;
		Swaper* pSwaper = (Swaper*)(iter->second);
		if (!(pSwaper->m_type == SULOKI_MSGQUEUE_TYPE_URC_BASE))
			return URC_CANNOTSUB_ERRORCODE;
		std::map< std::string, InfoSubscribed* >::iterator iterSub = pSwaper->m_subscriberMap.find(strModuleName);
		if (iterSub == pSwaper->m_subscriberMap.end())
			return URC_SUBUNEXISTED_ERRORCODE;
		InfoSubscribed* pSub = iterSub->second;
		pSub->m_queue.Push(msgSmartPtr);
		;
		return SUCCESS;
	}
	*/
public:
	//template <typename T>
	Ret GetUrDir(std::string strUrPath, std::vector<std::string>& nameVector)//T& tVal, 
	{
		if(!(strUrPath.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if(!(strUrPath.length() > 0 && strUrPath.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		if(strUrPath.at(strUrPath.length()-1) != '/')
			return INVALIDPARA_ERRORCODE;
		//
		nameVector.clear();
		//
		if(strUrPath.find(SULOKI_LOCAL_RESOURCE_URC_BASE) == 0)
		{
			//strUrPath = strUrPath.substr(SULOKI_LOCAL_RESOURCE_URC_BASE.length()-1);
			std::string strIndex;
			{
				ReadLock lock(m_rwLock);
				std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrPath);
				if(iter == m_urcMap.end())
					return URC_UNEXISTED_ERRORCODE;
				//std::auto_ptr< SwaperObject<T> > objSmart((SwaperObject<T>*)(iter->second));
				if(((Swaper*)(iter->second))->m_type != SULOKI_DIR_TYPE_URC_BASE)
					return URC_NOTDIR_ERRORCODE;
				strIndex = ((SwaperDir*)(iter->second))->m_strIndex;
			}
			Int numInx = strIndex.length()/MAXLEN_NAME;
			for(Int i=0;i<numInx;i++)
			{
				nameVector.push_back(strIndex.c_str()+MAXLEN_NAME*i);
			}
			return SUCCESS;
		}
		else
		if (strUrPath.find(SULOKI_REMOTED_RESOURCE_URC_BASE) == 0)
		{
			//strUrPath = strUrPath.substr(SULOKI_REMOTED_RESOURCE_URC_BASE.length()-1);
		}
		return URC_INVALIDPATH_ERRORCODE;
	}
protected:
	Ret GetUrDir_Urcsys(std::string strUrPath, std::vector<std::string>& nameVector)//T& tVal, 
	{
		if (!(strUrPath.length() <= MAXLEN_NAME))
			return URC_NAMETOOLONG_ERRORCODE;
		if (!(strUrPath.length() > 0 && strUrPath.length() <= MAXLEN_NAME))
			return INVALIDPARA_ERRORCODE;
		if (strUrPath.at(strUrPath.length() - 1) != '/')
			return INVALIDPARA_ERRORCODE;
		//
		nameVector.clear();
		//
		if (strUrPath.find(SULOKI_URCSYS_RESOURCE_URC_BASE) == 0)
		{
			//strUrPath = strUrPath.substr(SULOKI_LOCAL_RESOURCE_URC_BASE.length()-1);
			std::string strIndex;
			{
				ReadLock lock(m_rwLock);
				std::map<std::string, void*>::iterator iter = m_urcMap.find(strUrPath);
				if (iter == m_urcMap.end())
					return URC_UNEXISTED_ERRORCODE;
				//std::auto_ptr< SwaperObject<T> > objSmart((SwaperObject<T>*)(iter->second));
				if (((Swaper*)(iter->second))->m_type != SULOKI_DIR_TYPE_URC_BASE)
					return URC_NOTDIR_ERRORCODE;
				strIndex = ((SwaperDir*)(iter->second))->m_strIndex;
			}
			Int numInx = strIndex.length() / MAXLEN_NAME;
			for (Int i = 0; i<numInx; i++)
			{
				nameVector.push_back(strIndex.c_str() + MAXLEN_NAME*i);
			}
			return SUCCESS;
		}
		return URC_INVALIDPATH_ERRORCODE;
	}
protected:
	//template<typename F, typename...Args>
	//Ret Post(F &&f, Args&&...args)
	//{
	//	m_threadPool.Post(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	//	return SUCCESS;
	//}
	void Handle(std::string strModuleName, suloki::SulokiMessage* pMsg, suloki::SulokiContext& context)
	{
		std::auto_ptr< suloki::SulokiMessage > msgSmart(pMsg);
		m_threadPool.IncreseRef();
		if (strModuleName == "main")
		{
			m_mainHandlerSmartPtr->Handle(msgSmart, context);
			m_threadPool.DecreseRef();
			return;
		}
		;
		m_threadPool.DecreseRef();
	}
	Ret Post(std::string strModuleName, std::auto_ptr< suloki::SulokiMessage > msgSmart, suloki::SulokiContext& context)
	{
		m_threadPool.Post(boost::bind(&Urc::Handle, this, strModuleName, msgSmart.release(), context));
		return SUCCESS;
	}
	void HandleFuncRes(Urc::AsyncNewFunc func, suloki::SulokiMessage* pMsg, suloki::SulokiContext& context)
	{
		std::auto_ptr< suloki::SulokiMessage > msgSmart(pMsg);
		m_threadPool.IncreseRef();
		func(msgSmart, context);
		m_threadPool.DecreseRef();
	}
	Ret PostFuncRes(Urc::AsyncNewFunc func, std::auto_ptr< suloki::SulokiMessage > msgSmart, suloki::SulokiContext& context)
	{
		m_threadPool.Post(boost::bind(&Urc::HandleFuncRes, this, func, msgSmart.release(), context));
		return SUCCESS;
	}
private:
	Urc(Urc& ref) {}
	Urc& operator=(Urc& ref) { return *this; }
private:
	bool m_bRoot;
	//
	ThreadPool m_threadPool;
	boost::shared_ptr< SulokiHandleModuleInterface > m_mainHandlerSmartPtr;
	//
	RwLockPolicy m_rwLock;
	std::map<std::string, void*> m_urcMap;
	//
	friend class EventManager;
	friend class UrcTcpConnection;
	friend class UrcTcpServer;
};
typedef Singleton< Urc > UrcSingleton;

class Maintancer
{
public:
	Maintancer();
	virtual ~Maintancer();
	virtual Ret Init(void);
	virtual Ret Start(void);
	virtual Ret Stop(void);
	virtual Ret Clear(void);
private:
	Maintancer(Maintancer& ref) {}
	Maintancer& operator=(Maintancer& ref) { return *this; }
protected:
	void MyTimeout(void);
	void Run(void);
protected:
	std::auto_ptr<boost::thread> m_threadRunnedSmart;
	boost::asio::io_service m_ioService;
	std::auto_ptr<boost::asio::steady_timer> m_timerSmart;
};
typedef Singleton< Maintancer > MaintancerSingleton;

class AppStateMachine
{
public:
	AppStateMachine();
	virtual ~AppStateMachine();
	virtual Ret Init(void);
	virtual Ret Start(void);
	virtual Ret Run(void);
	virtual Ret Stop(void);
	virtual Ret Clear(void);
private:
	AppStateMachine(AppStateMachine& ref) {}
	AppStateMachine& operator=(AppStateMachine& ref) { return *this; }
private:
};

}

#endif
