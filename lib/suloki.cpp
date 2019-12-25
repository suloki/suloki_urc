#include "suloki.h"
#include "suloki_interface.h"
#include "handler.h"

namespace Suloki
{

//extern __thread Int g_logId = -1;
//tls_id
#ifdef SULOKI_WINDOWS_OS_SULOKI
	__declspec(thread) Int g_logId = -1;
#else
	__thread Int g_logId = -1;
#endif
Log g_log[MAXNUM_THREAD];
Int g_logIdFree = 0;
Int GetLogId(void)
{
	Int id = SULOKI_INCREMENT_GLOBAL(&g_logIdFree);
	if(id >= MAXNUM_THREAD)
	{
		std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << " too much thread error" << std::endl;
		return -1;
	}
	return id;
}
Log* GetLogPtr(void)
{
	if(g_logId < 0)
		g_logId = GetLogId();
	if(g_logId < 0)
		return NULL;
	return &g_log[g_logId];
}
void SetLogLevel(std::string logLevel)
{
	//std::cout << logLevel << std::endl;
	if(logLevel.compare("trace") == 0)
	{
		boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::trace);
		ConfigSingleton::Instance().SetConfig(SULOKI_LOGLEVEL_KEY_CONFIG_BASE, "trace");
	}
	else
	if(logLevel.compare("debug") == 0)
	{
		boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::debug);
		ConfigSingleton::Instance().SetConfig(SULOKI_LOGLEVEL_KEY_CONFIG_BASE, "debug");
	}
	else
	if(logLevel.compare("info") == 0)
	{
		boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::info);
		ConfigSingleton::Instance().SetConfig(SULOKI_LOGLEVEL_KEY_CONFIG_BASE, "info");
	}
	else
	if(logLevel.compare("warning") == 0)
	{
		boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::warning);
		ConfigSingleton::Instance().SetConfig(SULOKI_LOGLEVEL_KEY_CONFIG_BASE, "warning");
	}
	else
	if(logLevel.compare("error") == 0)
	{
		boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::error);
		ConfigSingleton::Instance().SetConfig(SULOKI_LOGLEVEL_KEY_CONFIG_BASE, "error");
	}
	else
	if(logLevel.compare("fatal") == 0)
	{
		boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::fatal);
		ConfigSingleton::Instance().SetConfig(SULOKI_LOGLEVEL_KEY_CONFIG_BASE, "fatal");
	}
}

Int Global::m_state = ZERO_GLOBALSTATE_BASEFRAMEWORK;

void Sleep(Int mSec)
{
#ifdef SULOKI_WINDOWS_OS_SULOKI
	::Sleep(mSec);
#else
	usleep(1000*mSec);
#endif
}

#ifdef SULOKI_OPENSOURCE_VERSION_GLOBAL
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

Config::Config()
{
	SetDefault();
}
Ret Config::ReadConfig(void)
{
	SetDefault();
	//
	try{
		boost::property_tree::ptree root;
		boost::property_tree::read_json<boost::property_tree::ptree>("config", root);
		std::string logName = root.get<std::string>(SULOKI_LOGNAME_KEY_CONFIG_BASE);
		ConfigSingleton::Instance().SetConfig(SULOKI_LOGNAME_KEY_CONFIG_BASE, logName);
		std::string logLevel = root.get<std::string>(SULOKI_LOGLEVEL_KEY_CONFIG_BASE);
		ConfigSingleton::Instance().SetConfig(SULOKI_LOGLEVEL_KEY_CONFIG_BASE, logLevel);
		std::string urcIp = root.get<std::string>(SULOKI_URCIP_KEY_CONFIG_BASE);
		ConfigSingleton::Instance().SetConfig(SULOKI_URCIP_KEY_CONFIG_BASE, urcIp);
		std::string urcPort = root.get<std::string>(SULOKI_URCPORT_KEY_CONFIG_BASE);
		ConfigSingleton::Instance().SetConfig(SULOKI_URCPORT_KEY_CONFIG_BASE, urcPort);
		std::string urcServer = root.get<std::string>(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE);
		ConfigSingleton::Instance().SetConfig(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE, urcServer);
		std::string urcThreadnum = root.get<std::string>(SULOKI_URCTHREADNUM_KEY_CONFIG_BASE);
		ConfigSingleton::Instance().SetConfig(SULOKI_URCTHREADNUM_KEY_CONFIG_BASE, urcThreadnum);
		std::string groupName = root.get<std::string>(SULOKI_GROUPNAME_KEY_CONFIG_BASE);
		ConfigSingleton::Instance().SetConfig(SULOKI_GROUPNAME_KEY_CONFIG_BASE, groupName);
		std::string serverName = root.get<std::string>(SULOKI_SERVERNAME_KEY_CONFIG_BASE);
		ConfigSingleton::Instance().SetConfig(SULOKI_SERVERNAME_KEY_CONFIG_BASE, serverName);
		boost::property_tree::ptree itemModuleinfo = root.get_child(SULOKI_MODULEINFO_KEY_CONFIG_BASE);
		boost::property_tree::ptree::iterator iterModuleinfo = itemModuleinfo.begin();
		while (iterModuleinfo != itemModuleinfo.end())
		{
			Module module;
			module.m_name = iterModuleinfo->second.get<std::string>("name");
			module.m_path = iterModuleinfo->second.get<std::string>("path");
			module.m_config = iterModuleinfo->second.get<std::string>("config");
			{
				WriteLock lock(m_rwLock);
				m_doduleVector.push_back(module);
			}
			iterModuleinfo++;

		}
		//
		//std::string moduleName = root.get<std::string>(SULOKI_MODULENAME_KEY_CONFIG_BASE);
		//ConfigSingleton::Instance().SetConfig(SULOKI_MODULENAME_KEY_CONFIG_BASE, moduleName);
		//std::string moduleScript = root.get<std::string>(SULOKI_MODULESCRIPT_KEY_CONFIG_BASE);
		//ConfigSingleton::Instance().SetConfig(SULOKI_MODULESCRIPT_KEY_CONFIG_BASE, moduleScript);
		//boost::property_tree::ptree item = root.get_child(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE);
		//boost::property_tree::ptree::iterator iter = item.begin();
		//if(iter != item.end())
		//{
		//	std::string address = iter->second.get<std::string>(SULOKI_ADDRESS_KEY_CONFIG_BASE);
		//	ConfigSingleton::Instance().SetConfig(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE, address);
		//	iter++;
		//}
		//if(iter != item.end())
		//{
		//	std::string address = iter->second.get<std::string>(SULOKI_ADDRESS_KEY_CONFIG_BASE);
		//	std::cout << address << std::endl;
		//	iter++;
		//}
		//if(iter != item.end())
		//	throw Exception("much address");
	}catch(boost::property_tree::ptree_error pt){
		//SULOKI_ERROR_LOG_BASEFRAMEWORK << "read config error";
		std::cout << "read config error" << std::endl;
		return FAIL;
	}catch(Exception e){
		//SULOKI_ERROR_LOG_BASEFRAMEWORK << "read config error," << e.what();
		std::cout << "read config error," << e.what() << std::endl;
		return FAIL;
	}
	return SUCCESS;
}
Ret Config::SetConfig(std::string key, std::string val)
{
	if(!(key.length() > 0 && val.length() > 0))
		return INVALIDPARA_ERRORCODE;
	WriteLock lock(m_rwLock);
	m_configMap[key] = val;
	return SUCCESS;
}
Ret Config::GetConfig(std::string key, std::string& val)
{
	if(!(key.length() > 0))
		return INVALIDPARA_ERRORCODE;
	ReadLock lock(m_rwLock);
	std::map<std::string, std::string>::iterator iter = m_configMap.find(key);
	if(iter == m_configMap.end())
	{
		//SULOKI_ERROR_LOG_BASEFRAMEWORK << "config " << key << " not existed error";
		std::cout << "config " << key << " not existed error" << std::endl;
		return FAIL;
	}
	val = iter->second;
	return SUCCESS;
}
void Config::SetDefault(void)
{
	{
		WriteLock lock(m_rwLock);
		m_configMap.clear();
		m_doduleVector.clear();
	}
	SetConfig(SULOKI_LOGNAME_KEY_CONFIG_BASE, "log");
	SetConfig(SULOKI_LOGLEVEL_KEY_CONFIG_BASE, "info");
	SetConfig(SULOKI_URCPORT_KEY_CONFIG_BASE, "8000");
}

IdManager::IdManager() :m_idFree(0), m_bCanfree(false), m_maxId(-1)
{}
IdManager::IdManager(Int maxId, bool bCanfree) throw() : m_idFree(0), m_bCanfree(false), m_maxId(-1)
{
	if (maxId > 0)
		m_maxId = maxId;
	else
		throw Exception("maxId invalid");
	m_bCanfree = bCanfree;
}
IdManager::~IdManager()
{}
Int IdManager::GetFreeId(void)
{
	std::auto_ptr<Int> idSmart = m_idQueue.Pop();
	if(idSmart.get() != NULL)
		return *idSmart;
	if (m_maxId > 0 && m_idFree >= m_maxId)
		return -1;
	Int id = SULOKI_INCREMENT_GLOBAL(&m_idFree);
	id--;
	return id;
}
Ret IdManager::FreeId(Int id)
{
	if(id < 0)
		return INVALIDPARA_ERRORCODE;
	if (!m_bCanfree)
		return INVALIDSTATE_ERRORCODE;
	std::auto_ptr<Int> idSmart(new Int(id));
	if(idSmart.get() == NULL)
	{
		SULOKI_FATAL_LOG_BASEFRAMEWORK << "no memory";
		return NOMEMORY_ERRORCODE;
	}
	m_idQueue.Push(idSmart);
	return SUCCESS;
}

ThreadPool::ThreadPool() :m_bInited(false), m_bCleared(false), m_worker(m_ioService), m_refNumber(0), m_threadNumber(1)
{}
ThreadPool::~ThreadPool()
{
	Clear();
}
Ret ThreadPool::Init(Int threadNum)
{
	if(threadNum < 1 || threadNum > MAXNUM_THREAD)
		return INVALIDPARA_ERRORCODE;
	if (!m_bInited)
	{
		m_bInited = true;
		//m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&ThreadPool::Run, this)));
		//if(m_threadRunnedSmart.get() == NULL)
		//	return FAIL;
		for (Int i = 0; i < threadNum; ++i)
			m_threads.create_thread(boost::bind(&boost::asio::io_service::run, &m_ioService));
		m_threadNumber = threadNum;
	}
	else
		return INVALIDSTATE_ERRORCODE;
	return SUCCESS;
}
//void ThreadPool::Run(void)
//{
//	m_ioService.run();
//}
Ret ThreadPool::Clear(void)
{
	if (!m_bCleared)
	{
		m_bCleared = true;
		//if(m_threadRunnedSmart.get() != NULL)
		//{
		//	m_threadRunnedSmart->interrupt();
		//	m_threadRunnedSmart->join();
		//}
		m_ioService.stop();
		m_threads.join_all();
	}
	return SUCCESS;
}

EventManager::EventManager()throw()
{
	m_idManagerSmart = std::auto_ptr<IdManager>(new IdManager(MAXNUM_THREAD, true));
	if (m_idManagerSmart.get() == NULL)
		throw Exception("no memory");
	for (Int i = 0; i < MAXNUM_THREAD + 2; i++)
	{
		m_ready[i] = false;
		m_bTimeout[i] = false;
	}
	//
	for (Int i = 0; i < MAXNUM_THREAD + 2; i++)
	{
		m_timer[i] = new boost::asio::steady_timer(m_ioService);
		if (m_timer[i] == NULL)
			throw Exception("no memory1");
	}
	m_timerForever = new boost::asio::steady_timer(m_ioService);
	if (m_timerForever == NULL)
		throw Exception("no memory2");
	//
	m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&EventManager::Run, this)));
	if (m_threadRunnedSmart.get() == NULL)
		throw Exception("no memory3");
}
EventManager::~EventManager()
{
	for (Int i = 0; i < MAXNUM_THREAD + 2; i++)
	{
		if (m_timer[i] != NULL)
			delete m_timer[i];
	}
	if (m_timerForever != NULL)
		delete m_timerForever;
	m_ioService.stop();
	if (m_threadRunnedSmart.get() != NULL)
		m_threadRunnedSmart->join();
}
void EventManager::FreeEvent(Int id)
{
	if (!(id >= 0 && id < MAXNUM_THREAD))
		return;
	m_idManagerSmart->FreeId(id);
}
Int EventManager::GetFreeEvent(void)
{
	return m_idManagerSmart->GetFreeId();
}
Ret EventManager::Wait(Int id, std::string strUrcKey, Int timeout, std::auto_ptr<suloki::SulokiMessage>& msgSmart)
{
	if (!(id >= 0 && id < MAXNUM_THREAD))
		return INVALIDPARA_ERRORCODE;
	bool bTimeout = false;
	m_timer[id]->expires_from_now(std::chrono::milliseconds(timeout));
	m_timer[id]->async_wait(std::bind(&EventManager::MyTimeout, this, id, strUrcKey));
	{
		boost::unique_lock<boost::mutex> lock(m_mutex[id]);
		m_ready[id] = false;
		m_bTimeout[id] = false;
		m_msgSmart[id].reset(NULL);
		while (!m_ready[id])
			m_cond[id].wait(lock);
		bTimeout = m_bTimeout[id];
		msgSmart = m_msgSmart[id];
	}
	if (bTimeout)
	{
		m_idManagerSmart->FreeId(id);
		return TIMEOUT_ERRORCODE;
	}
	m_idManagerSmart->FreeId(id);
	return SUCCESS;
}
Ret EventManager::Notify(Int id, std::auto_ptr<suloki::SulokiMessage> msgSmart)
{
	if (!(id >= 0 && id < MAXNUM_THREAD && msgSmart.get() != NULL && msgSmart->messagetype() == suloki::SulokiMessage::response))
		return INVALIDPARA_ERRORCODE;
	//
	//std::stringstream strStream;
	//strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msgSmart->businessid << "_" << msgSmart->messageid << "_" << msgSmart->sequencenumber;
	//
	//boost::function<void(Int, std::string, std::auto_ptr<suloki::SulokiMessage>, bool)> func;
	//if (UrcSingleton::Instance().DelUr_Urcsys(strStream.str(), func) != SUCCESS)
	//	return URC_UNEXISTED;
	//
	boost::unique_lock<boost::mutex> lock(m_mutex[id]);
	m_ready[id] = true;
	m_msgSmart[id] = msgSmart;
	m_cond[id].notify_one();
	return SUCCESS;
}
Ret EventManager::AsyncTimer(Int timeout, std::string strUrcKey)
{
	//std::auto_ptr<boost::asio::steady_timer> timerSmart(new boost::asio::steady_timer(m_ioService));
	//if (timerSmart.get() == NULL)
	//	return FAIL;
	boost::asio::steady_timer* pTimer = new boost::asio::steady_timer(m_ioService);
	if (pTimer == NULL)
		return FAIL;
	pTimer->expires_from_now(std::chrono::milliseconds(timeout));
	pTimer->async_wait(std::bind(&EventManager::MyAsyncTimeout, this, (Uint)pTimer, strUrcKey));
	return SUCCESS;
}
void EventManager::MyAsyncTimeout(Uint timerPtr, std::string strUrcKey)
{
	std::auto_ptr<boost::asio::steady_timer> timerSmart((boost::asio::steady_timer*)timerPtr);
	//boost::function<void(Uint, bool)> func;
	Urc::AsyncNewFunc func;
	if (UrcSingleton::Instance().DelUr_Urcsys(strUrcKey, func) != SUCCESS)
		return;
	//std::cout << "async timeout, urc name:" << strUrcKey << ";timeout:" << true << std::endl;
	//suloki::SulokiMessage* pMsg = new suloki::SulokiMessage();
	//func(NULL, true);
	suloki::SulokiContext context;
	context.set_b(true);
	std::auto_ptr<suloki::SulokiMessage> msgSmart;
	func(msgSmart, context);
}
void EventManager::MyTimeout(Int id, std::string strUrcKey)
{
	if (id < 0)
	{
		if (!(Global::GetState() >= STOP_GLOBALSTATE_BASEFRAMEWORK))
		{
			m_timerForever->expires_from_now(std::chrono::milliseconds(1000));
			m_timerForever->async_wait(std::bind(&EventManager::MyTimeout, this, -1, strUrcKey));
		}
		//std::cout << "timeout for ever" << std::endl;
	}
	else
	{
		if (!(id >= 0 && id < MAXNUM_THREAD))
			return;
		//
		//boost::function<void(Int, std::string, std::auto_ptr<suloki::SulokiMessage>, bool)> func;
		//boost::function<void(Uint, bool)> func;
		Urc::AsyncNewFunc func;
		if (UrcSingleton::Instance().DelUr_Urcsys<Urc::AsyncNewFunc>(strUrcKey, func) != SUCCESS)
			return;
		//
		boost::unique_lock<boost::mutex> lock(m_mutex[id]);
		m_ready[id] = true;
		m_bTimeout[id] = true;
		m_cond[id].notify_one();
	}
}
void EventManager::Run(void)
{
	m_timerForever->expires_from_now(std::chrono::milliseconds(1000));
	m_timerForever->async_wait(std::bind(&EventManager::MyTimeout, this, -1, ""));
	//std::cout << "pre" << std::endl;
	m_ioService.run();
	//std::cout << "late" << std::endl;
}

void UrcTcpConnection::HandleException(const boost::system::error_code& error)
{
	if(m_bServer)
	{
		SULOKI_INFO_LOG_BASEFRAMEWORK << "server net conn exception, conn's name:" << m_userData.m_strName;
		boost::shared_ptr<UrcTcpConnection> connSmartPtr;
		UrcSingleton::Instance().DelUr_Urcsys< boost::shared_ptr<UrcTcpConnection> >(m_userData.m_strName, connSmartPtr);
		//
		std::string strGroup;
		ConfigSingleton::Instance().GetConfig(SULOKI_GROUPNAME_KEY_CONFIG_BASE, strGroup);
		if (strGroup == "urcgroup" && m_userData.m_strServiceStateUrName.length() > 0)
		{
			SULOKI_INFO_LOG_BASEFRAMEWORK << "server net conn exception, service's name:" << m_userData.m_strServiceStateUrName;
			std::string strVal;
			UrcSingleton::Instance().DelNoSqlData(m_userData.m_strServiceStateUrName, strVal);
			m_userData.m_strServiceStateUrName = "";
		}
	}
	else
	{
		SULOKI_INFO_LOG_BASEFRAMEWORK << "client net conn exception, conn's name:" << m_userData.m_strName;
		boost::shared_ptr<UrcTcpConnection> connSmartPtr;
		UrcSingleton::Instance().DelUr_Urcsys< boost::shared_ptr<UrcTcpConnection> >(m_userData.m_strName, connSmartPtr);
	}
}
void UrcTcpConnection::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred)
{
	//if(m_bServer)
	//{
	//	std::cout << "server send ok, len:" << bytes_transferred << std::endl;
	//}
	//else
	//{
	//	std::cout << "client send ok, len:" << bytes_transferred << std::endl;
	//}
}
void UrcTcpConnection::HandleRead(std::string& strMsg)
{
	if (m_bServer)
	{
		//std::cout << "server recv ok, msg:" << strMsg << std::endl;
		//WriteAsync(strMsg.c_str(), strMsg.length());
		std::auto_ptr<suloki::SulokiMessage> msgSmart(new suloki::SulokiMessage());
		if (msgSmart.get() == NULL)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory";
			return;
		}
		if (SulokiProtoSwrap::DecodeProtocol(strMsg.c_str(), strMsg.length(), *msgSmart) != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "DecodeProtocol error";
			return;
		}
		/*
		int routerSize = msgSmart->routers_size();
		assert(routerSize > 0);
		const std::string moduleNameFrom = msgSmart->routers(routerSize - 1);
		//????? slow
		std::vector<std::string> nameVec;
		for (Suloki::Int i = 0; i < routerSize - 1; i++)
		nameVec.push_back(msgSmart->routers(i));
		msgSmart->clear_routers();
		for (Suloki::Int i = 0; i < routerSize - 1; i++)
		msgSmart->add_routers(nameVec[i]);
		//
		msgSmart->add_routers(m_userData.m_strName);
		std::string groupName;
		ConfigSingleton::Instance().GetConfig(SULOKI_GROUPNAME_KEY_CONFIG_BASE, groupName);
		std::string serverName;
		ConfigSingleton::Instance().GetConfig(SULOKI_SERVERNAME_KEY_CONFIG_BASE, serverName);
		*/
		//push router
		msgSmart->add_routers(m_userData.m_strName);
		//
		if ((msgSmart->messageid() == SULOKI_REGSERVICE_MESSAGEID_URC_PROTO || msgSmart->messageid() == SULOKI_UNREGSERVICE_MESSAGEID_URC_PROTO)
			&& msgSmart->messagetype() == suloki::SulokiMessage::request && msgSmart->businessid() == SULOKI_URC_BISINESSID_PROTO)
		{
			suloki::SulokiOperatorUrcMsgBody body;
			Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiOperatorUrcMsgBody>(*msgSmart, body);
			//
			suloki::SulokiMessage res;
			Suloki::SulokiProtoSwrap::MakeResMessage(*msgSmart, res);
			suloki::SulokiOperatorUrcMsgBody resBody;
			//
			if (!msgSmart->has_urckey())
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "has_urckey false error";
				return;
			}
			std::string urName = msgSmart->urckey();
			urName.erase(0, Suloki::SULOKI_REMOTED_RESOURCE_URC_BASE.length());
			urName.insert(0, Suloki::SULOKI_LOCAL_RESOURCE_URC_BASE);
			//std::cout << urName << std::endl;
			if (!msgSmart->has_urcval() || !msgSmart->has_dir())
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "has_urcval false or has_dir error";
				return;
			}
			std::string& strVal = (std::string&)msgSmart->urcval();
			Suloki::Ret ret = SUCCESS;
			if (msgSmart->messageid() == SULOKI_REGSERVICE_MESSAGEID_URC_PROTO)
			{
				ret = UrcSingleton::Instance().AddNoSqlData(urName, strVal, msgSmart->dir());
				if (ret == SUCCESS)
					m_userData.m_strServiceStateUrName = urName;
			}
			else
			{
				ret = UrcSingleton::Instance().DelNoSqlData(urName, strVal);
				if (ret == SUCCESS)
					m_userData.m_strServiceStateUrName = "";
			}
			res.set_errorcode(ret);
			//
			Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(res, resBody);
			WriteAsync(res);// strResMsg.c_str(), strResMsg.length());
			return;
		}
		//
		suloki::SulokiContext context;
		context.set_urname(m_userData.m_strName);
		context.set_b(m_bServer);
		UrcSingleton::Instance().Post("main", msgSmart, context);
	}
	else
	{
		//std::cout << "client recv ok, msg:" << strMsg << std::endl;
		std::auto_ptr<suloki::SulokiMessage> msgSmart(new suloki::SulokiMessage());
		if (msgSmart.get() == NULL)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory";
			return;
		}
		if (SulokiProtoSwrap::DecodeProtocol(strMsg.c_str(), strMsg.length(), *msgSmart) != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "DecodeProtocol error";
			return;
		}
		if (msgSmart->messagetype() == suloki::SulokiMessage::response)
		{
			std::stringstream strStream;
			if (msgSmart->routers_size() > 0)//msgSmart->routers(msgSmart->routers_size() - 1) << "_" <<
				strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msgSmart->routers(msgSmart->routers_size() - 1) << "_" << msgSmart->businessid() << "_" << msgSmart->messageid() << "_" << msgSmart->sequencenumber();
			else
				strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msgSmart->businessid() << "_" << msgSmart->messageid() << "_" << msgSmart->sequencenumber();
			//boost::function<void(Uint, bool)> func;
			Urc::AsyncNewFunc func;
			if (UrcSingleton::Instance().DelUr_Urcsys<Urc::AsyncNewFunc>(strStream.str(), func) == SUCCESS)
			{
				suloki::SulokiContext context;
				context.set_b(false);
				//func(msgSmart, context);
				UrcSingleton::Instance().PostFuncRes(func, msgSmart, context);
				return;
			}
		}
		suloki::SulokiContext context;
		context.set_urname(m_userData.m_strName);
		context.set_b(m_bServer);
		UrcSingleton::Instance().Post("main", msgSmart, context);
	}
}
UrcTcpServer::~UrcTcpServer()
{
	std::vector<std::string> nameVector;
	if (UrcSingleton::Instance().GetUrDir_Urcsys(SULOKI_CONNECTPATH_NET_URC_BASE, nameVector) == SUCCESS)
	{
		for(std::vector<std::string>::iterator iter=nameVector.begin();iter!=nameVector.end();iter++)
		{
			SULOKI_INFO_LOG_BASEFRAMEWORK << "TcpServer del resource, name:" << *iter;
			boost::shared_ptr<BaseRoot> baseSmartPtr; //boost::shared_ptr<UrcTcpConnection> connSmartPtr;
			UrcSingleton::Instance().DelUr_Urcsys< boost::shared_ptr<BaseRoot> >(*iter, baseSmartPtr);
		}
	}
	if (UrcSingleton::Instance().GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS)
	{
		for(std::vector<std::string>::iterator iter=nameVector.begin();iter!=nameVector.end();iter++)
		{
			SULOKI_INFO_LOG_BASEFRAMEWORK << "TcpServer del resource1, name:" << *iter;
			boost::shared_ptr<BaseRoot> baseSmartPtr; //boost::shared_ptr<UrcTcpConnection> connSmartPtr;
			UrcSingleton::Instance().DelUr_Urcsys< boost::shared_ptr<BaseRoot> >(*iter, baseSmartPtr);
		}
	}
	if (UrcSingleton::Instance().GetUrDir_Urcsys(SULOKI_CONNECT2SERVICEPATH_NET_URC_BASE, nameVector) == SUCCESS)
	{
		for (std::vector<std::string>::iterator iter = nameVector.begin(); iter != nameVector.end(); iter++)
		{
			SULOKI_INFO_LOG_BASEFRAMEWORK << "TcpServer del resource1, name:" << *iter;
			boost::shared_ptr<BaseRoot> baseSmartPtr; //boost::shared_ptr<UrcTcpConnection> connSmartPtr;
			UrcSingleton::Instance().DelUr_Urcsys< boost::shared_ptr<BaseRoot> >(*iter, baseSmartPtr);
		}
	}
}
Ret UrcTcpServer::Start(Int port)
{
	SULOKI_INFO_LOG_BASEFRAMEWORK << "urc port acceptted:" << port;
	m_ioServiceSmart = std::auto_ptr<boost::asio::io_service>(new boost::asio::io_service());
	m_acceptorSmart = std::auto_ptr<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor(*m_ioServiceSmart, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)));
	Accept();
	m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&UrcTcpServer::Run, this)));
	//?????
	std::string strUrcServerAddr;
	if (!(ConfigSingleton::Instance().GetConfig(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE, strUrcServerAddr) == SUCCESS && strUrcServerAddr.find("_1._1._1._1:") != std::string::npos))
	{
		SULOKI_INFO_LOG_BASEFRAMEWORK << "mean this urc is child";
		size_t pos = strUrcServerAddr.find(":");
		if (pos == std::string::npos)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "address invalid";
			return FAIL;
		}
		std::string urcServerIp = strUrcServerAddr.substr(0, pos);
		std::string urcServerPort = strUrcServerAddr.substr(pos+1);
		//
		std::stringstream strStream;
		strStream << SULOKI_CONNECT2URCPATH_NET_URC_BASE << 0;
		if(UrcSingleton::Instance().ConnAndReg(strStream.str(), urcServerIp, atoi(urcServerPort.c_str())) != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "ConnAndReg error ";// << __FILE__ << __LINE__;
			return FAIL;
		}
		//std::vector<std::string> nameVector;
		//UrcSingleton::Instance().GetUrDir(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector);
		SULOKI_INFO_LOG_BASEFRAMEWORK << "urc connect to root urc server ok, ip:" << urcServerIp << ";port:" << urcServerPort;
	}
	else
	{
		UrcSingleton::Instance().SetRootFlag();
		SULOKI_INFO_LOG_BASEFRAMEWORK << "mean this urc is root server";	
	}
	return SUCCESS;
}
void UrcTcpServer::HandleAccept(std::string strConnname, boost::shared_ptr<UrcTcpConnection> connSmartPtr)
{
	boost::shared_ptr<BaseRoot> baseSmartPtr = boost::static_pointer_cast<BaseRoot>(connSmartPtr);
	if (UrcSingleton::Instance().AddUrIn<boost::shared_ptr<BaseRoot>, BaseRoot>(strConnname, baseSmartPtr, SULOKI_OBJECT_TYPE_URC_BASE, 0, true, 0, Loki::Type2Type< boost::shared_ptr<BaseRoot> >()) != SUCCESS)
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUr error";
	else
		SULOKI_INFO_LOG_BASEFRAMEWORK << "HandleAccept AddUr ok.strConnname:" << strConnname;
}

Ret Urc::Init(void)
{
	std::string strUrcthreadnum;
	ConfigSingleton::Instance().GetConfig(SULOKI_URCTHREADNUM_KEY_CONFIG_BASE, strUrcthreadnum);
	Int urcthreadnum = atoi(strUrcthreadnum.c_str());
	if (SetThreadNum(urcthreadnum) != SUCCESS)
		return FAIL;
	std::vector<Config::Module> moduleVector;
	ConfigSingleton::Instance().GetConfig<Config::Module>(SULOKI_MODULEINFO_KEY_CONFIG_BASE, moduleVector);
	if (moduleVector.size() != 1)
		return FAIL;
	//
	for (std::vector<Config::Module>::iterator iter = moduleVector.begin(); iter != moduleVector.end(); iter++)
	{
		std::string strUrName = SULOKI_URCSYS_RESOURCE_URC_BASE + "module/" + iter->m_name;
		boost::shared_ptr< SulokiHandleModuleInterface > handlerSmartPtr(NULL);
		/*
		if (iter->m_name != "main" || m_mainHandlerSmartPtr.get() == NULL)
			return FAIL;
		handlerSmartPtr = m_mainHandlerSmartPtr;
		*/
		if (iter->m_name != "main")
			return FAIL;
		handlerSmartPtr = boost::shared_ptr< SulokiHandleModuleInterface >(new SulokiCppHandleModule(iter->m_path));
		if (handlerSmartPtr.get() == NULL)
			return FAIL;
		m_mainHandlerSmartPtr = handlerSmartPtr;
		//
		if (handlerSmartPtr.get() == NULL)
			return FAIL;
		std::string strGroup;
		ConfigSingleton::Instance().GetConfig(SULOKI_GROUPNAME_KEY_CONFIG_BASE, strGroup);
		std::string strServer;
		ConfigSingleton::Instance().GetConfig(SULOKI_SERVERNAME_KEY_CONFIG_BASE, strServer);
		if (handlerSmartPtr->Init((SulokiUrcModuleInterface*)this, strGroup, strServer, iter->m_name, iter->m_config) != SUCCESS)
			return FAIL;
		boost::shared_ptr<Module> moduleSmartPtr(new Module(iter->m_name, handlerSmartPtr, iter->m_config));
		if (moduleSmartPtr.get() == NULL)
			return FAIL;
		//if (AddUrIn<boost::shared_ptr<Module>, Module>(strUrName, moduleSmartPtr, SULOKI_OBJECT_TYPE_URC_BASE, 0, true, 0, Loki::Type2Type< boost::shared_ptr<Module> >()) != SUCCESS)
		boost::shared_ptr<BaseRoot> baseSmartPtr = boost::static_pointer_cast<BaseRoot>(moduleSmartPtr);
		if (UrcSingleton::Instance().AddUrIn(strUrName, baseSmartPtr, SULOKI_OBJECT_TYPE_URC_BASE, 0, true, 0, Loki::Type2Type< boost::shared_ptr<BaseRoot> >()) != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUr error ";// << __FILE__ << __LINE__ << std::endl;
			return FAIL;
		}
	}
	//
	return SUCCESS;
}
Ret Urc::Start(void)
{
	std::vector<Config::Module> moduleVector;
	ConfigSingleton::Instance().GetConfig<Config::Module>(SULOKI_MODULEINFO_KEY_CONFIG_BASE, moduleVector);
	if (moduleVector.size() == 0)
		return FAIL;
	//
	for (std::vector<Config::Module>::iterator iter = moduleVector.begin(); iter != moduleVector.end(); iter++)
	{
		std::string strUrName = SULOKI_URCSYS_RESOURCE_URC_BASE + "module/" + iter->m_name;
		//boost::shared_ptr<Module> moduleSmartPtr;
		//if (GetUr<boost::shared_ptr<Module>>(strUrName, moduleSmartPtr) != Suloki::SUCCESS)
		//	return Suloki::FAIL;
		boost::shared_ptr<BaseRoot> baseSmartPtr;
		if (!(Suloki::UrcSingleton::Instance().GetUr(strUrName, baseSmartPtr) == Suloki::SUCCESS && baseSmartPtr.get() != NULL))
			return Suloki::FAIL;
		boost::shared_ptr<Module> moduleSmartPtr = boost::dynamic_pointer_cast<Module>(baseSmartPtr);
		if (moduleSmartPtr.get() == NULL)
			return FAIL;
		if (moduleSmartPtr->m_handlerSmartPtr->Start() != SUCCESS)
			return FAIL;
	}
	return SUCCESS;
}
Ret Urc::Stop(void)
{
	std::vector<Config::Module> moduleVector;
	ConfigSingleton::Instance().GetConfig<Config::Module>(SULOKI_MODULEINFO_KEY_CONFIG_BASE, moduleVector);
	if (moduleVector.size() == 0)
		return FAIL;
	//
	for (std::vector<Config::Module>::iterator iter = moduleVector.begin(); iter != moduleVector.end(); iter++)
	{
		std::string strUrName = SULOKI_URCSYS_RESOURCE_URC_BASE + "module/" + iter->m_name;
		//boost::shared_ptr<Module> moduleSmartPtr;
		//if (GetUr<boost::shared_ptr<Module>>(strUrName, moduleSmartPtr) != Suloki::SUCCESS)
		//	return Suloki::FAIL;
		boost::shared_ptr<BaseRoot> baseSmartPtr;
		if (!(Suloki::UrcSingleton::Instance().GetUr(strUrName, baseSmartPtr) == Suloki::SUCCESS && baseSmartPtr.get() != NULL))
			return Suloki::FAIL;
		boost::shared_ptr<Module> moduleSmartPtr = boost::dynamic_pointer_cast<Module>(baseSmartPtr);
		if (moduleSmartPtr.get() == NULL)
			return FAIL;
		if (moduleSmartPtr->m_handlerSmartPtr->Stop() != SUCCESS)
			return FAIL;
	}
	return SUCCESS;
}
Ret Urc::Clear(void)
{
	std::vector<Config::Module> moduleVector;
	ConfigSingleton::Instance().GetConfig<Config::Module>(SULOKI_MODULEINFO_KEY_CONFIG_BASE, moduleVector);
	if (moduleVector.size() == 0)
		return FAIL;
	//
	for (std::vector<Config::Module>::iterator iter = moduleVector.begin(); iter != moduleVector.end(); iter++)
	{
		std::string strUrName = SULOKI_URCSYS_RESOURCE_URC_BASE + "module/" + iter->m_name;
		//boost::shared_ptr<Module> moduleSmartPtr;
		//if (GetUr<boost::shared_ptr<Module>>(strUrName, moduleSmartPtr) != Suloki::SUCCESS)
		//	return Suloki::FAIL;
		boost::shared_ptr<BaseRoot> baseSmartPtr;
		if (!(Suloki::UrcSingleton::Instance().GetUr(strUrName, baseSmartPtr) == Suloki::SUCCESS && baseSmartPtr.get() != NULL))
			return Suloki::FAIL;
		boost::shared_ptr<Module> moduleSmartPtr = boost::dynamic_pointer_cast<Module>(baseSmartPtr);
		if (moduleSmartPtr.get() == NULL)
			return FAIL;
		if (moduleSmartPtr->m_handlerSmartPtr->Clear() != SUCCESS)
			return FAIL;
	}
	return SUCCESS;
}
Ret Urc::ConnAndReg(std::string strUrName, std::string ip, Uint port)
{
	if (m_bRoot)
		return FAIL;
	//connect
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip), port);
	boost::shared_ptr<UrcTcpConnection> connSmartPtr = UrcTcpServerSingleton::Instance().CreateClientConnection();
	if (connSmartPtr.get() == NULL)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory";
		return FAIL;
	}
	try{
		connSmartPtr->GetSocketRef().connect(ep);
		connSmartPtr->ReadAsync();
	}
	catch (boost::system::system_error e){
		SULOKI_ERROR_LOG_BASEFRAMEWORK << e.code();
		return FAIL;
	}
	//std::stringstream strStream;
	//strStream << SULOKI_CONNECT2URCPATH_NET_URC_BASE << 0;
	UrcTcpConnection::UserData userData(strUrName);// strStream.str());
	connSmartPtr->SetUserData(userData);
	boost::shared_ptr<BaseRoot> baseSmartPtr = boost::static_pointer_cast<BaseRoot>(connSmartPtr);
	if (UrcSingleton::Instance().AddUrIn<boost::shared_ptr<BaseRoot>, BaseRoot>(strUrName, baseSmartPtr, SULOKI_OBJECT_TYPE_URC_BASE, 0, true, 0, Loki::Type2Type< boost::shared_ptr<BaseRoot> >()) != SUCCESS)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUr error ";// << __FILE__ << __LINE__;
		return FAIL;
	}
	//reg
	{
		std::string strGroup;
		ConfigSingleton::Instance().GetConfig(SULOKI_GROUPNAME_KEY_CONFIG_BASE, strGroup);
		std::string strServer;
		ConfigSingleton::Instance().GetConfig(SULOKI_SERVERNAME_KEY_CONFIG_BASE, strServer);
		std::string strUrName = SULOKI_REMOTED_RESOURCE_URC_BASE + SULOKI_SERVICES_PATHNAME_URC_BASE + strGroup + "/" + strServer;
		//
		std::string strIp;
		ConfigSingleton::Instance().GetConfig(SULOKI_URCIP_KEY_CONFIG_BASE, strIp);
		std::string strPort;
		ConfigSingleton::Instance().GetConfig(SULOKI_URCPORT_KEY_CONFIG_BASE, strPort);
		suloki::SulokiServiceStateUrcMsgBody state;
		state.set_ip(strIp);
		state.set_port(atoi(strPort.c_str()));
		state.set_busydegree(0);
		std::string strBody;
		if (!state.SerializeToString(&strBody))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "SerializeToString error";
			return URC_ENCODEBODYPROTO_ERRORCODE;
		}
		suloki::SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_REGSERVICE_MESSAGEID_URC_PROTO);
		req.set_messagetype(suloki::SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		req.set_urckey(strUrName);
		req.set_urcval(strBody);
		req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
		req.set_attrib(0);
		req.set_dir(true);
		suloki::SulokiOperatorUrcMsgBody body;
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
		//
		std::string strMsg;
		SulokiProtoSwrap::EncodeProtocol(req, strMsg);
		//
		Int id = EventManagerSingleton::Instance().GetFreeEvent();
		if (id < 0)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "get GetFreeEvent fail";
			return FAIL;
		}
		std::stringstream strStream;
		if (req.routers_size() > 0)//tVal.routers(tVal.routers_size() - 1) << "_" <<
			strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << req.routers(req.routers_size() - 1) << "_" << req.businessid() << "_" << req.messageid() << "_" << req.sequencenumber();
		else
			strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << req.businessid() << "_" << req.messageid() << "_" << req.sequencenumber();
		//boost::function<void(Int, Uint, bool)> func = boost::bind(&Urc::FuncRes, this, id, _1, _1);
		//suloki::SulokiContext context;
		//context.set_id(id);
		std::auto_ptr<suloki::SulokiContext> contextSmart(new suloki::SulokiContext());
		if (contextSmart.get() == NULL)
			return FAIL;
		contextSmart->set_id(id);
		suloki::SulokiMessage* pMsgBack = contextSmart->mutable_msgori();
		if (pMsgBack == NULL)
			return FAIL;
		SulokiProtoSwrap::MakeSimpleCopy(req, *pMsgBack);
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
		if (EventManagerSingleton::Instance().Wait(id, strStream.str(), 1000, resSmart) != SUCCESS)
			return TIMEOUT_ERRORCODE;
		//req = *resSmart;
		if (!resSmart->has_errorcode())
			return FAIL;
		return resSmart->errorcode();
	}
	return SUCCESS;
}
SulokiRet Urc::ReqRes(SULOKI_IN std::string strGroupName, SULOKI_IN std::string strServiceName, SULOKI_INOUT suloki::SulokiMessage& req, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart)
{
	if (!(strGroupName.length() > 0 && req.messagetype() == suloki::SulokiMessage::request))
		return INVALIDPARA_ERRORCODE;
	if (strServiceName == "")
	{
		std::vector<std::string> nameVector;
		if (GetUrDir_Urcsys(SULOKI_CONNECT2SERVICEPATH_NET_URC_BASE, nameVector) == SUCCESS)
		{
			std::stringstream strStream;
			strStream << "/" << strGroupName << "_";
			for (std::vector<std::string>::iterator iter = nameVector.begin(); iter != nameVector.end(); iter++)
			{
				size_t pos = iter->find(strStream.str());
				size_t pos_ = iter->find('_', pos);
				if (pos != std::string::npos)
				{
					strServiceName = iter->substr(pos_ + 1);
				}
			}
		}
		if (strServiceName == "")
		{
			std::stringstream strUrStream;
			strUrStream << SULOKI_REMOTED_RESOURCE_URC_BASE + SULOKI_SERVICES_PATHNAME_URC_BASE + strGroupName + "/";
			suloki::SulokiMessage req;
			Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
			req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
			req.set_messageid(SULOKI_OBTAINSTATES_MESSAGEID_URC_PROTO);
			req.set_messagetype(suloki::SulokiMessage::request);
			req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
			req.set_urckey(strUrStream.str());
			//req.set_urcval(data);
			//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
			//req.set_attrib(0);
			//req.set_dir(bDir);
			//suloki::SulokiOperatorUrcMsgBody body;
			//Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
			if (ReqresMsgToUrcserver(strUrStream.str(), req, timeout) != SUCCESS)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "can't obtaib service's state info in group from urc server";
				return FAIL;
			}
			suloki::SulokiGroupStateUrcMsgBody body;
			if (SulokiProtoSwrap::GetBody<suloki::SulokiGroupStateUrcMsgBody>(req, body) != SUCCESS)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "GetBody error";
				return FAIL;
			}
			Int serviceNum = body.stateres_size();
			Int minBusy = 256;
			Int which = -1;
			for (int i = 0; i < serviceNum; i++)
			{
				if (!body.stateres(i).has_busydegree())// || !body.stateres(i).has_urname())
				{
					SULOKI_ERROR_LOG_BASEFRAMEWORK << "no busyDegree field error";
					return FAIL;
				}
				//if (body.stateres(i).urname().find(strGroupName+std::string("_")) != std::string::npos)
				//	continue;
				if (body.stateres(i).busydegree() < minBusy)
				{
					minBusy = body.stateres(i).busydegree();
					which = i;
				}
			}
			if (which == -1)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "obtain states error";
				return FAIL;
			}
			if (!body.stateres(which).has_urname())
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "no urName field error";
				return FAIL;
			}
			strServiceName = body.stateres(which).urname();
			size_t pos = strServiceName.rfind('/');
			if (pos == std::string::npos)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "can't find service name error";
				return FAIL;
			}
			strServiceName = strServiceName.substr(pos + 1);
		}
	}
	std::stringstream strStream;
	strStream << SULOKI_CONNECT2SERVICEPATH_NET_URC_BASE << strGroupName << "_" << strServiceName;
	boost::shared_ptr<BaseRoot> baseSmartPtr;
	if (!(GetUr_Urcsys(strStream.str(), baseSmartPtr) == SUCCESS && baseSmartPtr.get() != NULL))
	{
		std::stringstream strUrStream;
		strUrStream << SULOKI_REMOTED_RESOURCE_URC_BASE + SULOKI_SERVICES_PATHNAME_URC_BASE + strGroupName + "/" + strServiceName;
		suloki::SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_GET_MESSAGEID_URC_PROTO);
		req.set_messagetype(suloki::SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		req.set_urckey(strUrStream.str());
		//req.set_urcval(data);
		req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
		//req.set_attrib(0);
		//req.set_dir(bDir);
		suloki::SulokiOperatorUrcMsgBody body;
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
		if (ReqresMsgToUrcserver(strUrStream.str(), req, timeout) != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "can't obtaib service's state info from urc server";
			return FAIL;
		}
		if (!req.has_urcval())
			return INVALIDPARA_ERRORCODE;
		std::string strState = req.urcval();
		suloki::SulokiServiceStateUrcMsgBody state;
		if (!state.ParseFromString(strState))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "ParseFromString error";
			return URC_DECODEBODYPROTO_ERRORCODE;
		}
		if (!(state.has_ip() && state.has_port()))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "service state invalid error";
			return URC_DECODEBODYPROTO_ERRORCODE;
		}
		if (ConnAndReg(strStream.str(), state.ip(), state.port()) != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
			return FAIL;
		}
	}
	if (asyncCb == NULL)
		return ReqresMsgToUrcserver(SULOKI_REMOTED_RESOURCE_URC_BASE, req, timeout, strStream.str());
	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
	return ReqresMsgToUrcserver(SULOKI_REMOTED_RESOURCE_URC_BASE, req, timeout, asyncCbNew, strStream.str());
}
SulokiRet Urc::Notify(SULOKI_IN std::string strGroupName, SULOKI_IN std::string strServiceName, SULOKI_IN suloki::SulokiMessage& notice)
{
	if (notice.messagetype() != suloki::SulokiMessage::notice)
		return INVALIDPARA_ERRORCODE;
	std::stringstream strStream;
	strStream << SULOKI_CONNECT2SERVICEPATH_NET_URC_BASE << strGroupName << "_" << strServiceName;
	boost::shared_ptr<BaseRoot> baseSmartPtr;
	if (!(GetUr_Urcsys(strStream.str(), baseSmartPtr) == SUCCESS && baseSmartPtr.get() != NULL))
	{
		std::stringstream strUrStream;
		strUrStream << SULOKI_REMOTED_RESOURCE_URC_BASE + SULOKI_SERVICES_PATHNAME_URC_BASE + strGroupName + "/" + strServiceName;
		suloki::SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_GET_MESSAGEID_URC_PROTO);
		req.set_messagetype(suloki::SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		req.set_urckey(strUrStream.str());
		//req.set_urcval(data);
		req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
		//req.set_attrib(0);
		//req.set_dir(bDir);
		suloki::SulokiOperatorUrcMsgBody body;
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
		if (ReqresMsgToUrcserver(strUrStream.str(), req, 1000) != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "can't obtaib service's state info from urc server";
			return FAIL;
		}
		if (!req.has_urcval())
			return INVALIDPARA_ERRORCODE;
		std::string strState = req.urcval();
		suloki::SulokiServiceStateUrcMsgBody state;
		if (!state.ParseFromString(strState))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "ParseFromString error";
			return URC_DECODEBODYPROTO_ERRORCODE;
		}
		if (!(state.has_ip() && state.has_port()))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "service state invalid error";
			return URC_DECODEBODYPROTO_ERRORCODE;
		}
		if (ConnAndReg(strStream.str(), state.ip(), state.port()) != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
			return FAIL;
		}
	}
	return NotifyMsgToUrcserver(SULOKI_REMOTED_RESOURCE_URC_BASE, notice, strStream.str());
}
SulokiRet Urc::PostToMainModule(SULOKI_IN std::auto_ptr<suloki::SulokiMessage> msgSmart)
{
	//return PostToMainModuleUnvirtual(msgSmart);
	suloki::SulokiContext context;
	Post("main", msgSmart, context);
	return SUCCESS;
}
SulokiRet Urc::AddObject(SULOKI_IN std::string strUrName, SULOKI_IN boost::shared_ptr<BaseRoot>& baseSmartPtr)
{
	//return AddObjectUnvirtual(strUrName, baseSmartPtr);
	return AddUr(strUrName, baseSmartPtr, SULOKI_OBJECT_TYPE_URC_BASE, 0, true, 0);
}
SulokiRet Urc::DelObject(SULOKI_IN std::string strUrName, SULOKI_OUT boost::shared_ptr<BaseRoot>& baseSmartPtr)
{
	//return DelObjectUnvirtual(strUrName, baseSmartPtr);
	return DelUr(strUrName, baseSmartPtr);
}
SulokiRet Urc::GetObject(SULOKI_IN std::string strUrName, SULOKI_OUT boost::shared_ptr<BaseRoot>& baseSmartPtr)
{
	//return GetObjectUnvirtual(strUrName, baseSmartPtr);
	return GetUr(strUrName, baseSmartPtr);
}
SulokiRet Urc::GetSqlData(SULOKI_IN std::string strUrName, SULOKI_INOUT suloki::SulokiMessage& msg, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart)
{
	//return GetSqlDataUnvirtual(strUrName, msg, timeout);
	//return GetUr(strUrName, msg, timeout);
	if (asyncCb == NULL)
		return GetUr(strUrName, msg, timeout);
	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
	return GetUr(strUrName, msg, timeout, asyncCbNew);
}
//virtual SulokiRet GetSqlDataAsync(std::string strUrName, suloki::SulokiMessage& msg, long timeout, SulokiAsyncCb asyncCb, std::auto_ptr<suloki::SulokiContext> contextOriSmart)
//{
//	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
//	//return GetSqlDataAsyncUnvirtual(strUrName, msg, timeout, asyncCbNew);
//	return GetUr(strUrName, msg, timeout, asyncCbNew);
//}
SulokiRet Urc::AddNoSqlData(SULOKI_IN std::string strUrName, SULOKI_IN std::string& data, SULOKI_IN bool bDir, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart)
{
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
	{
		return AddUr(strUrName, data, SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, bDir);
	}
	suloki::SulokiMessage req;
	Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
	req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
	req.set_messageid(SULOKI_ADD_MESSAGEID_URC_PROTO);
	req.set_messagetype(suloki::SulokiMessage::request);
	req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
	req.set_urckey(strUrName);
	req.set_urcval(data);
	req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
	req.set_attrib(0);
	req.set_dir(bDir);
	suloki::SulokiOperatorUrcMsgBody body;
	Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
	if (asyncCb == NULL)
		return ReqresMsgToUrcserver(strUrName, req, timeout);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, bDir, timeout);
	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
	return ReqresMsgToUrcserver(strUrName, req, timeout, asyncCbNew);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, bDir, timeout, asyncCbNew);
}
SulokiRet Urc::DelNoSqlData(SULOKI_IN std::string strUrName, SULOKI_OUT std::string& data, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart)
{
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
	{
		return DelUr(strUrName, data);
	}
	suloki::SulokiMessage req;
	Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
	req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
	req.set_messageid(SULOKI_DEL_MESSAGEID_URC_PROTO);
	req.set_messagetype(suloki::SulokiMessage::request);
	req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
	req.set_urckey(strUrName);
	//req.set_urcval(data);
	//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
	//req.set_attrib(0);
	//req.set_dir(bDir);
	suloki::SulokiOperatorUrcMsgBody body;
	Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
	if (asyncCb == NULL)
		return ReqresMsgToUrcserver(strUrName, req, timeout); //return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, false, timeout);
	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
	return ReqresMsgToUrcserver(strUrName, req, timeout, asyncCbNew);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, false, timeout, asyncCbNew);
}
SulokiRet Urc::UpdateNoSqlData(SULOKI_IN std::string strUrName, SULOKI_INOUT std::string& data, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart)
{
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
	{
		std::string strValOri;
		Ret ret = UpdateUr(strUrName, data, strValOri);
		data = strValOri;
		return ret;
	}
	suloki::SulokiMessage req;
	Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
	req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
	req.set_messageid(SULOKI_UPDATE_MESSAGEID_URC_PROTO);
	req.set_messagetype(suloki::SulokiMessage::request);
	req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
	req.set_urckey(strUrName);
	req.set_urcval(data);
	//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
	//req.set_attrib(0);
	//req.set_dir(bDir);
	suloki::SulokiOperatorUrcMsgBody body;
	Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
	if (asyncCb == NULL)
		return ReqresMsgToUrcserver(strUrName, req, timeout); //return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, false, timeout);
	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
	return ReqresMsgToUrcserver(strUrName, req, timeout, asyncCbNew);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, false, timeout, asyncCbNew);
}
SulokiRet Urc::GetNoSqlData(SULOKI_IN std::string strUrName, SULOKI_OUT std::string& data, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<suloki::SulokiContext> contextOriSmart)
{
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
	{
		return GetUr(strUrName, data);
	}
	suloki::SulokiMessage req;
	Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
	req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
	req.set_messageid(SULOKI_GET_MESSAGEID_URC_PROTO);
	req.set_messagetype(suloki::SulokiMessage::request);
	req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
	req.set_urckey(strUrName);
	req.set_urcval(data);
	//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
	//req.set_attrib(0);
	//req.set_dir(bDir);
	suloki::SulokiOperatorUrcMsgBody body;
	Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
	if (asyncCb == NULL)
	{
		Ret ret = ReqresMsgToUrcserver(strUrName, req, timeout); //AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, false, timeout);
		if (!req.has_urcval())
			return FAIL;
		data = req.urcval();
		return ret;
	}
	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
	Ret ret = ReqresMsgToUrcserver(strUrName, req, timeout, asyncCbNew); //AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, false, timeout, asyncCbNew);
	if (!req.has_urcval())
		return FAIL;
	data = req.urcval();
	return ret;
}
SulokiRet Urc::GetUrDirectory(SULOKI_IN std::string strUrPath, SULOKI_OUT std::vector<std::string>& nameVector)
{
	if (strUrPath.find(SULOKI_LOCAL_RESOURCE_URC_BASE) != 0)
		return FAIL;
	return GetUrDir(strUrPath, nameVector);
}
Ret Urc::ReqresMsgToUrcserver(std::string strUrName, suloki::SulokiMessage& msg, Int timeout, std::string strObjName)
{
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
		return URC_INVALIDPATH_ERRORCODE;
	{//sync
		if (strObjName == "")
		{
			std::vector<std::string> nameVector;
			if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
			{
				std::string strUrcServerAddr;
				ConfigSingleton::Instance().GetConfig(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE, strUrcServerAddr);
				size_t pos = strUrcServerAddr.find(":");
				std::string urcServerIp = strUrcServerAddr.substr(0, pos);
				std::string urcServerPort = strUrcServerAddr.substr(pos + 1);
				std::stringstream strStream;
				strStream << SULOKI_CONNECT2URCPATH_NET_URC_BASE << 0;
				if (ConnAndReg(strStream.str(), urcServerIp, atoi(urcServerPort.c_str())) != SUCCESS)
				{
					SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
					return FAIL;
				}
				std::vector<std::string> nameVector;
				if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
				{
					SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
					return FAIL;
				}
			}
			strObjName = nameVector[0];
		}
		//boost::shared_ptr<UrcTcpConnection> connSmartPtr;
		//if (!(GetUr_Urcsys(nameVector[0], connSmartPtr) == SUCCESS && connSmartPtr.get() != NULL))
		//{
		//	std::cout << "get connect to urc server fail" << std::endl;
		//	return FAIL;
		//}
		boost::shared_ptr<BaseRoot> baseSmartPtr;
		if (!(GetUr_Urcsys(strObjName, baseSmartPtr) == SUCCESS && baseSmartPtr.get() != NULL))
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
		SulokiProtoSwrap::EncodeProtocol(msg, strMsg);
		//
		Int id = EventManagerSingleton::Instance().GetFreeEvent();
		if (id < 0)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "get GetFreeEvent fail";
			return FAIL;
		}
		std::stringstream strStream;
		if (msg.routers_size() > 0)//tVal.routers(tVal.routers_size() - 1) << "_" <<
			strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msg.routers(msg.routers_size() - 1) << "_" << msg.businessid() << "_" << msg.messageid() << "_" << msg.sequencenumber();
		else
			strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msg.businessid() << "_" << msg.messageid() << "_" << msg.sequencenumber();
		//boost::function<void(Int, Uint, bool)> func = boost::bind(&Urc::FuncRes, this, id, _1, _1);
		//suloki::SulokiContext context;
		//context.set_id(id);
		std::auto_ptr<suloki::SulokiContext> contextSmart(new suloki::SulokiContext());
		if (contextSmart.get() == NULL)
			return FAIL;
		contextSmart->set_id(id);
		suloki::SulokiMessage* pMsgBack = contextSmart->mutable_msgori();
		if (pMsgBack == NULL)
			return FAIL;
		SulokiProtoSwrap::MakeSimpleCopy(msg, *pMsgBack);
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
		msg = *resSmart;
		if (!resSmart->has_errorcode())
			return FAIL;
		return resSmart->errorcode();
	}
	return URC_INVALIDPATH_ERRORCODE;
}
Ret Urc::ReqresMsgToUrcserver(std::string strUrName, suloki::SulokiMessage& msg, Int timeout, AsyncNewFunc asyncCb, std::string strObjName)
{
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
		return URC_INVALIDPATH_ERRORCODE;
	{//async
		if (strObjName == "")
		{
			std::vector<std::string> nameVector;
			if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
			{
				std::string strUrcServerAddr;
				ConfigSingleton::Instance().GetConfig(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE, strUrcServerAddr);
				size_t pos = strUrcServerAddr.find(":");
				std::string urcServerIp = strUrcServerAddr.substr(0, pos);
				std::string urcServerPort = strUrcServerAddr.substr(pos + 1);
				std::stringstream strStream;
				strStream << SULOKI_CONNECT2URCPATH_NET_URC_BASE << 0;
				if (ConnAndReg(strStream.str(), urcServerIp, atoi(urcServerPort.c_str())) != SUCCESS)
				{
					SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
					return FAIL;
				}
				std::vector<std::string> nameVector;
				if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
				{
					SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
					return FAIL;
				}
			}
			strObjName = nameVector[0];
		}
		//boost::shared_ptr<UrcTcpConnection> connSmartPtr;
		//if (!(GetUr_Urcsys(nameVector[0], connSmartPtr) == SUCCESS && connSmartPtr.get() != NULL))
		//{
		//	SULOKI_ERROR_LOG_BASEFRAMEWORK << "get connect to urc server fail";
		//	return FAIL;
		//}
		boost::shared_ptr<BaseRoot> baseSmartPtr;
		if (!(GetUr_Urcsys(strObjName, baseSmartPtr) == SUCCESS && baseSmartPtr.get() != NULL))
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
		SulokiProtoSwrap::EncodeProtocol(msg, strMsg);
		//
		std::stringstream strStream;
		if (msg.routers_size() > 0)//tVal.routers(tVal.routers_size() - 1) << "_" <<
			strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msg.routers(msg.routers_size() - 1) << "_" << msg.businessid() << "_" << msg.messageid() << "_" << msg.sequencenumber();
		else
			strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msg.businessid() << "_" << msg.messageid() << "_" << msg.sequencenumber();
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
Ret Urc::NotifyMsgToUrcserver(std::string strUrName, suloki::SulokiMessage& msg, std::string strObjName)
{
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
		return URC_INVALIDPATH_ERRORCODE;
	{//sync
		if (strObjName == "")
		{
			std::vector<std::string> nameVector;
			if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
			{
				std::string strUrcServerAddr;
				ConfigSingleton::Instance().GetConfig(SULOKI_URCSERVERADDR_KEY_CONFIG_BASE, strUrcServerAddr);
				size_t pos = strUrcServerAddr.find(":");
				std::string urcServerIp = strUrcServerAddr.substr(0, pos);
				std::string urcServerPort = strUrcServerAddr.substr(pos + 1);
				std::stringstream strStream;
				strStream << SULOKI_CONNECT2URCPATH_NET_URC_BASE << 0;
				if (ConnAndReg(strStream.str(), urcServerIp, atoi(urcServerPort.c_str())) != SUCCESS)
				{
					SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
					return FAIL;
				}
				std::vector<std::string> nameVector;
				if (!(GetUrDir_Urcsys(SULOKI_CONNECT2URCPATH_NET_URC_BASE, nameVector) == SUCCESS && nameVector.size() == 1))
				{
					SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server";
					return FAIL;
				}
			}
			strObjName = nameVector[0];
		}
		//boost::shared_ptr<UrcTcpConnection> connSmartPtr;
		//if (!(GetUr_Urcsys(nameVector[0], connSmartPtr) == SUCCESS && connSmartPtr.get() != NULL))
		//{
		//	std::cout << "get connect to urc server fail" << std::endl;
		//	return FAIL;
		//}
		boost::shared_ptr<BaseRoot> baseSmartPtr;
		if (!(GetUr_Urcsys(strObjName, baseSmartPtr) == SUCCESS && baseSmartPtr.get() != NULL))
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
		SulokiProtoSwrap::EncodeProtocol(msg, strMsg);
		//
		connSmartPtr->WriteAsync(strMsg.c_str(), strMsg.length());
		return SUCCESS;
	}
	return URC_INVALIDPATH_ERRORCODE;
}

Maintancer::Maintancer()
{}
Maintancer::~Maintancer()
{}
Ret Maintancer::Init(void)
{
	return SUCCESS;
}
Ret Maintancer::Start(void)
{
	m_timerSmart = std::auto_ptr<boost::asio::steady_timer>(new boost::asio::steady_timer(m_ioService));
	if (m_timerSmart.get() == NULL)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "new m_timerSmart error";
		return FAIL;
	}
	m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&Maintancer::Run, this)));
	if (m_threadRunnedSmart.get() == NULL)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "new m_threadRunnedSmart error";
		return FAIL;
	}
	return SUCCESS;
}
Ret Maintancer::Stop(void)
{
	if (m_timerSmart.get() != NULL)
		m_timerSmart.reset(NULL);
	m_ioService.stop();
	if (m_threadRunnedSmart.get() != NULL)
		m_threadRunnedSmart->join();
	return SUCCESS;
}
Ret Maintancer::Clear(void)
{
	return SUCCESS;
}
void Maintancer::MyTimeout(void)
{
	//????? urc server have not report state
	if (!UrcSingleton::Instance().GetRootFlag())
	{
		std::string strGroup;
		ConfigSingleton::Instance().GetConfig(SULOKI_GROUPNAME_KEY_CONFIG_BASE, strGroup);
		std::string strServer;
		ConfigSingleton::Instance().GetConfig(SULOKI_SERVERNAME_KEY_CONFIG_BASE, strServer);
		std::string strUrName = SULOKI_REMOTED_RESOURCE_URC_BASE + SULOKI_SERVICES_PATHNAME_URC_BASE + strGroup + "/" + strServer;
		std::string strIp;
		ConfigSingleton::Instance().GetConfig(SULOKI_URCIP_KEY_CONFIG_BASE, strIp);
		std::string strPort;
		ConfigSingleton::Instance().GetConfig(SULOKI_URCPORT_KEY_CONFIG_BASE, strPort);
		suloki::SulokiServiceStateUrcMsgBody state;
		state.set_ip(strIp);
		state.set_port(atoi(strPort.c_str()));
		Float fBusy = UrcSingleton::Instance().GetBusydegree();
		state.set_busydegree((Int)(255 * fBusy));
		std::string strBody;
		if (!state.SerializeToString(&strBody))
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "SerializeToString error";
		}
		else
		{
			suloki::SulokiMessage req;
			Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
			req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
			req.set_messageid(SULOKI_UPDATE_MESSAGEID_URC_PROTO);
			req.set_messagetype(suloki::SulokiMessage::request);
			req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
			req.set_urckey(strUrName);
			req.set_urcval(strBody);
			req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
			req.set_attrib(0);
			req.set_dir(true);
			suloki::SulokiOperatorUrcMsgBody body;
			Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
			UrcSingleton::Instance().ReqresMsgToUrcserver(strUrName, req, 1000);
		}
	}
	m_timerSmart->expires_from_now(std::chrono::milliseconds(5000));
	m_timerSmart->async_wait(std::bind(&Maintancer::MyTimeout, this));
}
void Maintancer::Run(void)
{
	m_timerSmart->expires_from_now(std::chrono::milliseconds(5000));
	m_timerSmart->async_wait(std::bind(&Maintancer::MyTimeout, this));
	//std::cout << "pre" << std::endl;
	m_ioService.run();
	//std::cout << "late" << std::endl;
}

AppStateMachine::AppStateMachine()
{}
AppStateMachine::~AppStateMachine()
{}
Ret AppStateMachine::Init(void)
{
	/*
	if(ConfigSingleton::Instance().ReadConfig() != SUCCESS)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "ConfigSingleton::Instance().ReadConfig error";
		return FAIL;
	}
	*/
	std::string strLogname;
	//if(ConfigSingleton::Instance().GetConfig(SULOKI_LOGNAME_KEY_CONFIG_BASE, strLogname) != SUCCESS)
	//	strLogname = "log";
	ConfigSingleton::Instance().GetConfig(SULOKI_LOGNAME_KEY_CONFIG_BASE, strLogname);
	strLogname += "_%N.log";
	std::string strLoglevel;
	//if(ConfigSingleton::Instance().GetConfig(SULOKI_LOGLEVEL_KEY_CONFIG_BASE, strLoglevel) != SUCCESS)
	//	strLoglevel = "info";
	ConfigSingleton::Instance().GetConfig(SULOKI_LOGLEVEL_KEY_CONFIG_BASE, strLoglevel);
	auto skinSmartPtr = boost::log::add_file_log
		(
		boost::log::keywords::open_mode = std::ios::app,
		boost::log::keywords::target = "log",
		boost::log::keywords::file_name = (std::string)"./log/" + strLogname,//"log_%N.log",                                        //< file name pattern >//
		boost::log::keywords::rotation_size = 1024 * 1024 * 10,                                   //< rotate files every 10 MiB... >//
		boost::log::keywords::max_size = 1024 * 1024 * 1000,
		boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0), //< ...or at midnight >//
		//keywords::format = //"[%TimeStamp%]: %Message%"                                 //< log record format >//
		//	expr::stream
		//	<< " <" << boost::log::trivial::severity
		//	<< ">:" << expr::smessage
		//(
		//	expr::stream
		//	<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d, %H:%M:%S.%f")
		//<< " [" << expr::format_date_time< attrs::timer::value_type >("Uptime", "%O:%M:%S")
		//<< "] [" << expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
		//<< "] <" << expr::attr< severity_level >("Severity")
		//	<< "> " << expr::message
		//)
		boost::log::keywords::format = boost::log::expressions::stream
		<< "{" << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d, %H:%M:%S.%f")
		//<< "][" << expr::format_date_time< attrs::timer::value_type >("Uptime", "%O:%M:%S")
		//<< "][" << expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
		<< "}[" << boost::log::expressions::attr< boost::log::trivial::severity_level >("Severity")
		<< "]" << boost::log::expressions::message
		);
	skinSmartPtr->locked_backend()->auto_flush(true);
	boost::log::core::get()->add_sink(skinSmartPtr);
	//boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::info);
	SetLogLevel(strLoglevel);
	boost::log::add_common_attributes();
	//std::cout << "log init" << std::endl;
	//
	try{
		ConfigSingleton::Instance();
		/*
		EventManagerSingleton::Instance();
		UrcSingleton::Instance();
		UrcClientHandlerSingleton::Instance();
		UrcServerHandlerSingleton::Instance();
		UrcTcpServerSingleton::Instance();
		*/
	}catch(std::exception& e){
		SULOKI_ERROR_LOG_BASEFRAMEWORK << e.what();
		return FAIL;
	}catch(...){
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "some exception";
		return FAIL;
	}
	/*
	if(UrcSingleton::Instance().Init() != SUCCESS)
		return FAIL;
	*/
	//std::string strUrcthreadnum;
	//ConfigSingleton::Instance().GetConfig(SULOKI_URCTHREADNUM_KEY_CONFIG_BASE, strUrcthreadnum);
	//Int urcthreadnum = atoi(strUrcthreadnum.c_str());
	//if (UrcSingleton::Instance().SetThreadNum(urcthreadnum) != SUCCESS)
	//	return FAIL;
	//std::vector<Config::Module> moduleVector;
	//ConfigSingleton::Instance().GetConfig<Config::Module>(SULOKI_MODULEINFO_KEY_CONFIG_BASE, moduleVector);
	return SUCCESS;
}
Ret AppStateMachine::Start(void)
{
	/*if (UrcSingleton::Instance().Init() != SUCCESS)
		return FAIL;
	std::string strUrcthreadnum;
	ConfigSingleton::Instance().GetConfig(SULOKI_URCTHREADNUM_KEY_CONFIG_BASE, strUrcthreadnum);
	Int urcthreadnum = atoi(strUrcthreadnum.c_str());
	if (UrcSingleton::Instance().SetThreadNum(urcthreadnum) != SUCCESS)
		return FAIL;*/
	/*
	if(UrcSingleton::Instance().Start() != SUCCESS)
		return FAIL;
	std::string strUrcport;
	//if(ConfigSingleton::Instance().GetConfig(SULOKI_URCPORT_KEY_CONFIG_BASE, strUrcport) != SUCCESS)
	//	return FAIL;
	ConfigSingleton::Instance().GetConfig(SULOKI_URCPORT_KEY_CONFIG_BASE, strUrcport);
	Int urcport = atoi(strUrcport.c_str());
	if(UrcTcpServerSingleton::Instance().Start(urcport) != SUCCESS)
		return FAIL;
	*/
	return SUCCESS;
}
Ret AppStateMachine::Run(void)
{
	const std::string strMsg = "input quit to exit";
	std::cout << strMsg << std::endl;
	std::string str;
	while (true)
	{
		std::cin >> str;
		if (str.compare("quit") == 0)
			break;
		std::cout << strMsg << std::endl;
	}
	return SUCCESS;
}
Ret AppStateMachine::Stop(void)
{
	/*
	UrcTcpServerSingleton::Instance().Stop();
	UrcSingleton::Instance().Stop();
	*/
	return SUCCESS;
}
Ret AppStateMachine::Clear(void)
{
	/*
	UrcTcpServerSingleton::Deinstance();
	UrcClientHandlerSingleton::Deinstance();
	UrcServerHandlerSingleton::Deinstance();
	UrcSingleton::Instance().Clear();
	UrcSingleton::Deinstance();
	EventManagerSingleton::Deinstance();
	*/
	ConfigSingleton::Deinstance();
	return SUCCESS;
}

}
