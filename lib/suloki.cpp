#include "suloki.h"
#include "suloki_interface.h"
#include "handler.h"

#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
typedef boost::shared_mutex RwLockPolicy1;
typedef boost::unique_lock<RwLockPolicy1>   WriteLock1;
typedef boost::shared_lock<RwLockPolicy1>   ReadLock1;
static RwLockPolicy1 g_lockMdebug;
static std::map<long long, std::string> g_mapMdebug;
void New_Mdebug(void* pTr, std::string str)
{
	if (MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface() != NULL)
	{
		MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->NewMdebug(pTr, str);
		return;
	}
	WriteLock1 lock(g_lockMdebug);
	g_mapMdebug.insert(std::pair<long long, std::string>((long long)pTr, str));
}
void Del_Mdebug(void* pTr)
{
	if (MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface() != NULL)
	{
		MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->DelMdebug(pTr);
		return;
	}
	WriteLock1 lock(g_lockMdebug);
	std::map<long long, std::string>::iterator iter = g_mapMdebug.find((long long)pTr);
	if (iter != g_mapMdebug.end())
		g_mapMdebug.erase(iter);
}
void Print_Mdebug(void)
{
	SULOKI_FATAL_LOG_BASEFRAMEWORK << "print memory debug info start";
	{
		WriteLock1 lock(g_lockMdebug);
		for (std::map<long long, std::string>::iterator iter = g_mapMdebug.begin(); iter != g_mapMdebug.end(); iter++)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "memory leak:" << iter->second;
		}
	}
	SULOKI_FATAL_LOG_BASEFRAMEWORK << "print memory debug info end";
}
SulokiMessage::SulokiMessage()
{}
SulokiMessage::~SulokiMessage()
{
	Del_Mdebug(this);
}
SulokiContext::SulokiContext()
{}
SulokiContext::~SulokiContext()
{
	Del_Mdebug(this);
}
#else
SulokiMessage::SulokiMessage()
{}
SulokiMessage::~SulokiMessage()
{}
SulokiContext::SulokiContext()
{}
SulokiContext::~SulokiContext()
{}
#endif

SulokiUrcModuleInterface* MySulokiUrcModuleInterface::m_pSulokiUrcModuleInterface = NULL;

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
{
	m_idQueue.Clear();
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(this)
#endif
}
Int IdManager::GetFreeId(void)
{
	std::auto_ptr<Int> idSmart = m_idQueue.Pop();
	if (idSmart.get() != NULL)
	{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		DEL_MDEBUG(idSmart.get());
#endif
		return *idSmart;
	}
	if (m_maxId > 0 && m_idFree >= m_maxId)
		return -1;
	Int id = SULOKI_INCREMENT_GLOBAL(&m_idFree);
	id--;
	return id;
}
Ret IdManager::FreeId(Int id, std::string strMdebug)
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
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	std::stringstream strStream;
	strStream << strMdebug << "_" << id;
	strMdebug = strStream.str();
	NEW_MDEBUG(idSmart.get(), strMdebug);
#endif
	m_idQueue.Push(idSmart);
	return SUCCESS;
}

ThreadPool::ThreadPool() :m_bInited(false), m_bCleared(false), m_worker(m_ioService), m_refNumber(0), m_threadNumber(1)
{}
ThreadPool::~ThreadPool()
{
	Clear();
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(this)
#endif
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
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	NEW_MDEBUG(m_idManagerSmart.get(), "");
#endif
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
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(m_timer[i], "");
#endif
	}
	m_timerForever = new boost::asio::steady_timer(m_ioService);
	if (m_timerForever == NULL)
		throw Exception("no memory2");
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	NEW_MDEBUG(m_timerForever, "");
#endif
	//
	m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&EventManager::Run, this)));
	if (m_threadRunnedSmart.get() == NULL)
		throw Exception("no memory3");
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	NEW_MDEBUG(m_threadRunnedSmart.get(), "");
#endif
}
EventManager::~EventManager()
{
	if (m_timerForever != NULL)
	{
		m_timerForever->cancel();
	}
	for (Int i = 0; i < MAXNUM_THREAD + 2; i++)
	{
		if (m_timer[i] != NULL)
			m_timer[i]->cancel();
	}
	m_ioService.stop();
	if (m_threadRunnedSmart.get() != NULL)
		m_threadRunnedSmart->join();
	//?????
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	if (m_timerForever != NULL)
	{
		delete m_timerForever;
		DEL_MDEBUG(m_timerForever)
	}
#endif
	for (Int i = 0; i < MAXNUM_THREAD + 2; i++)
	{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		if (m_timer[i] != NULL)
		{
			delete m_timer[i];
			DEL_MDEBUG(m_timer[i])
		}
#endif
	}
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(m_threadRunnedSmart.get())
	m_threadRunnedSmart.reset(NULL);
#endif
	//
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(this)
#endif
}
void EventManager::FreeEvent(Int id)
{
	if (!(id >= 0 && id < MAXNUM_THREAD))
		return;
	m_idManagerSmart->FreeId(id, __FUNCTION__);
}
Int EventManager::GetFreeEvent(void)
{
	return m_idManagerSmart->GetFreeId();
}
Ret EventManager::Wait(Int id, std::string strUrcKey, Int timeout, std::auto_ptr<SulokiMessage>& msgSmart)
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
		m_idManagerSmart->FreeId(id, __FUNCTION__);
		return TIMEOUT_ERRORCODE;
	}
	m_idManagerSmart->FreeId(id, __FUNCTION__);
	return SUCCESS;
}
Ret EventManager::Notify(Int id, std::auto_ptr<SulokiMessage> msgSmart)
{
	if (!(id >= 0 && id < MAXNUM_THREAD && msgSmart.get() != NULL && msgSmart->messagetype() == SulokiMessage::response))
		return INVALIDPARA_ERRORCODE;
	//
	//std::stringstream strStream;
	//strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << msgSmart->businessid << "_" << msgSmart->messageid << "_" << msgSmart->sequencenumber;
	//
	//boost::function<void(Int, std::string, std::auto_ptr<SulokiMessage>, bool)> func;
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
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	NEW_MDEBUG(pTimer, strUrcKey);
#endif
	pTimer->expires_from_now(std::chrono::milliseconds(timeout));
	pTimer->async_wait(std::bind(&EventManager::MyAsyncTimeout, this, (Uint)pTimer, strUrcKey));
	return SUCCESS;
}
void EventManager::MyAsyncTimeout(Uint timerPtr, std::string strUrcKey)
{
	std::auto_ptr<boost::asio::steady_timer> timerSmart((boost::asio::steady_timer*)timerPtr);
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(timerSmart.get());
#endif
	//boost::function<void(Uint, bool)> func;
	Urc::AsyncNewFunc func;
	if (UrcSingleton::Instance().DelUr_Urcsys(strUrcKey, func) != SUCCESS)
		return;
	//std::cout << "async timeout, urc name:" << strUrcKey << ";timeout:" << true << std::endl;
	//SulokiMessage* pMsg = new SulokiMessage();
	//func(NULL, true);
	SulokiContext context;
	context.set_b(true);
	std::auto_ptr<SulokiMessage> msgSmart;
	func(msgSmart, context);
}
void EventManager::MyTimeout(Int id, std::string strUrcKey)
{
	if(Global::GetState() >= STOP_GLOBALSTATE_BASEFRAMEWORK)
		return;
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
		//boost::function<void(Int, std::string, std::auto_ptr<SulokiMessage>, bool)> func;
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
		//
		for (std::set<std::string>::iterator iter = m_userData.m_subSet.begin(); iter != m_userData.m_subSet.end(); iter++)
		{
			size_t pos = iter->find("___");
			if (pos != std::string::npos)
			{
				std::string strUrName = iter->substr(0, pos);
				pos += strlen("___");
				std::string strVal = iter->substr(pos, iter->length()-pos);
				UrcSingleton::Instance().UnsubscribeUr(strUrName, strVal);
			}
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
		std::auto_ptr<SulokiMessage> msgSmart(new SulokiMessage());
		if (msgSmart.get() == NULL)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory";
			return;
		}
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(msgSmart.get(), "");
#endif
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
		if (msgSmart->messagetype() == SulokiMessage::request && msgSmart->businessid() == SULOKI_URC_BISINESSID_PROTO)
		{
			if (msgSmart->messageid() == SULOKI_REGSERVICE_MESSAGEID_URC_PROTO || msgSmart->messageid() == SULOKI_UNREGSERVICE_MESSAGEID_URC_PROTO)
			{
				suloki::SulokiOperatorUrcMsgBody body;
				if (Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiOperatorUrcMsgBody>(*msgSmart, body) != SUCCESS)
				{
					SULOKI_ERROR_LOG_BASEFRAMEWORK << "GetBody SulokiOperatorUrcMsgBody error";
					return;
				}
				//
				SulokiMessage res;
				Suloki::SulokiProtoSwrap::MakeResMessage(*msgSmart, res);
				suloki::SulokiOperatorUrcMsgBody resBody;
				;
				Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(res, resBody);
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
					else
					{
						SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddNoSqlData error, urname:" << urName;
					}
				}
				else
				{
					ret = UrcSingleton::Instance().DelNoSqlData(urName, strVal);
					if (ret == SUCCESS)
						m_userData.m_strServiceStateUrName = "";
					else
					{
						SULOKI_ERROR_LOG_BASEFRAMEWORK << "DelNoSqlData error, urname:" << urName;
					}
				}
				res.set_errorcode(ret);
				//
				//Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(res, resBody);
				WriteAsync(res);// strResMsg.c_str(), strResMsg.length());
				return;
			}
			else
			if (msgSmart->messageid() == SULOKI_SUBSCRIBE_MESSAGEID_URC_PROTO || msgSmart->messageid() == SULOKI_UNSUBSCRIBE_MESSAGEID_URC_PROTO)
			{
				//suloki::SulokiOperatorUrcMsgBody body;
				//if (Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiOperatorUrcMsgBody>(*msgSmart, body) != SUCCESS)
				//{
				//	SULOKI_ERROR_LOG_BASEFRAMEWORK << "GetBody SulokiOperatorUrcMsgBody error";
				//	return;
				//}
				//
				SulokiMessage res;
				Suloki::SulokiProtoSwrap::MakeResMessage(*msgSmart, res);
				//suloki::SulokiOperatorUrcMsgBody resBody;
				//;
				//Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(res, resBody);
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
				if (!msgSmart->has_urcval())
				{
					SULOKI_ERROR_LOG_BASEFRAMEWORK << "has_urcval false error";
					return;
				}
				std::string strVal = (std::string&)msgSmart->urcval();
				strVal.insert(0, m_userData.m_strServiceStateUrName);
				Suloki::Ret ret = SUCCESS;
				if (msgSmart->messageid() == SULOKI_SUBSCRIBE_MESSAGEID_URC_PROTO)
				{
					std::auto_ptr<SulokiContext> contextOriSmart(new SulokiContext());
					if (contextOriSmart.get() == NULL)
					{
						SULOKI_ERROR_LOG_BASEFRAMEWORK << "new SulokiContext error";
						return;
					}
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
					NEW_MDEBUG(contextOriSmart.get(), "");
#endif
					contextOriSmart->set_urname(m_userData.m_strName);
					ret = UrcSingleton::Instance().SubscribeUr(urName, strVal, Urc::FuncSub, contextOriSmart);
					if (ret != SUCCESS)
					{
						SULOKI_ERROR_LOG_BASEFRAMEWORK << "SubscribeUr error, urname:" << urName << "subname:" << strVal;
					}
					else
					{
						std::stringstream strStream;
						strStream << urName << "___" << strVal;
						m_userData.m_subSet.insert(strStream.str());
					}
				}
				else
				{
					ret = UrcSingleton::Instance().UnsubscribeUr(urName, strVal);
					if (ret != SUCCESS)
					{
						SULOKI_ERROR_LOG_BASEFRAMEWORK << "UnsubscribeUr error, urname:" << urName << "subname:" << strVal;
					}
					else
					{
						std::stringstream strStream;
						strStream << urName << "___" << strVal;
						std::set<std::string>::iterator iter = m_userData.m_subSet.find(strStream.str());
						if (iter != m_userData.m_subSet.end())
							m_userData.m_subSet.erase(iter);
					}
				}
				res.set_errorcode(ret);
				//
				//Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(res, resBody);
				WriteAsync(res);// strResMsg.c_str(), strResMsg.length());
				return;
			}
		}
		//
		SulokiContext context;
		context.set_urname(m_userData.m_strName);
		context.set_b(m_bServer);
		UrcSingleton::Instance().Post("main", msgSmart, context);
	}
	else
	{
		//std::cout << "client recv ok, msg:" << strMsg << std::endl;
		std::auto_ptr<SulokiMessage> msgSmart(new SulokiMessage());
		if (msgSmart.get() == NULL)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory";
			return;
		}
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(msgSmart.get(), "");
#endif
		if (SulokiProtoSwrap::DecodeProtocol(strMsg.c_str(), strMsg.length(), *msgSmart) != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "DecodeProtocol error";
			return;
		}
		if (msgSmart->messagetype() == SulokiMessage::response)
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
				SulokiContext context;
				context.set_b(false);
				//func(msgSmart, context);
				UrcSingleton::Instance().PostFuncRes(func, msgSmart, context);
				return;
			}
		}
		SulokiContext context;
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
	//????? about sql data rc
	{
		std::string strSqlUrName = SULOKI_SQL_NAME_URC_BASE;
		strSqlUrName.erase(0, Suloki::SULOKI_REMOTED_RESOURCE_URC_BASE.length());
		strSqlUrName.insert(0, Suloki::SULOKI_LOCAL_RESOURCE_URC_BASE);
		std::string stdVal = "sql";
		UrcSingleton::Instance().DelUr(strSqlUrName, stdVal);
	}
	//
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(m_threadRunnedSmart.get())
	m_threadRunnedSmart.reset(NULL);
#endif
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(m_acceptorSmart.get())
	m_acceptorSmart.reset(NULL);
#endif
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(m_ioServiceSmart.get())
	m_ioServiceSmart.reset(NULL);
#endif
	//
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(this)
#endif
}
Ret UrcTcpServer::Start(Int port)
{
	SULOKI_INFO_LOG_BASEFRAMEWORK << "urc port acceptted:" << port;
	m_ioServiceSmart = std::auto_ptr<boost::asio::io_service>(new boost::asio::io_service());
	if (m_ioServiceSmart.get() == NULL)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory";
		return FAIL;
	}
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	NEW_MDEBUG(m_ioServiceSmart.get(), "");
#endif
	m_acceptorSmart = std::auto_ptr<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor(*m_ioServiceSmart, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)));
	if (m_acceptorSmart.get() == NULL)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory";
		return FAIL;
	}
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	NEW_MDEBUG(m_acceptorSmart.get(), "");
#endif
	Accept();
	m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&UrcTcpServer::Run, this)));
	if (m_threadRunnedSmart.get() == NULL)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory";
		return FAIL;
	}
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	NEW_MDEBUG(m_threadRunnedSmart.get(), "");
#endif
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
		//????? about sql data rc
		std::string strSqlUrName = SULOKI_SQL_NAME_URC_BASE;
		strSqlUrName.erase(0, Suloki::SULOKI_REMOTED_RESOURCE_URC_BASE.length());
		strSqlUrName.insert(0, Suloki::SULOKI_LOCAL_RESOURCE_URC_BASE);
		std::string stdVal = "sql";
		if (UrcSingleton::Instance().AddUr(strSqlUrName, stdVal, SULOKI_NOSQLDATA_TYPE_URC_BASE) != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUr error, urname:" << strSqlUrName;
			return FAIL;
		}
		//
		SULOKI_INFO_LOG_BASEFRAMEWORK << "mean this urc is root server";	
	}
	return SUCCESS;
}
void UrcTcpServer::HandleAccept(std::string strConnname, boost::shared_ptr<UrcTcpConnection> connSmartPtr)
{
	boost::shared_ptr<BaseRoot> baseSmartPtr = boost::static_pointer_cast<BaseRoot>(connSmartPtr);
	if (UrcSingleton::Instance().AddUrIn<boost::shared_ptr<BaseRoot>, BaseRoot>(strConnname, baseSmartPtr, SULOKI_OBJECT_TYPE_URC_BASE, 0, true, 0, Loki::Type2Type< boost::shared_ptr<BaseRoot> >()) != SUCCESS)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUr error";
	}
	else
	{
		SULOKI_INFO_LOG_BASEFRAMEWORK << "HandleAccept AddUr ok.strConnname:" << strConnname;
	}
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
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(handlerSmartPtr.get(), "");
#endif
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
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(moduleSmartPtr.get(), "");
#endif
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
		//?????
		{
			boost::shared_ptr<BaseRoot> baseSmartPtr; //boost::shared_ptr<UrcTcpConnection> connSmartPtr;
			UrcSingleton::Instance().DelUr_Urcsys< boost::shared_ptr<BaseRoot> >(strUrName, baseSmartPtr);
		}
	}
	return SUCCESS;
}
Ret Urc::ConnAndReg(std::string strUrName, std::string ip, Uint port)
{
	if (m_bRoot)
		return FAIL;
	Ret ret = SUCCESS;
	//connect
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip), port);
	boost::shared_ptr<UrcTcpConnection> connSmartPtr = UrcTcpServerSingleton::Instance().CreateClientConnection();
	if (connSmartPtr.get() == NULL)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "create net conn fail";
		return NET_CREATECONNFAIL_ERRORCODE;
	}
	try{
		connSmartPtr->GetSocketRef().connect(ep);
		connSmartPtr->ReadAsync();
	}
	catch (boost::system::system_error e){
		SULOKI_ERROR_LOG_BASEFRAMEWORK << e.code();
		return NET_EXCEPTION_ERRORCODE;
	}
	//std::stringstream strStream;
	//strStream << SULOKI_CONNECT2URCPATH_NET_URC_BASE << 0;
	UrcTcpConnection::UserData userData(strUrName);// strStream.str());
	connSmartPtr->SetUserData(userData);
	boost::shared_ptr<BaseRoot> baseSmartPtr = boost::static_pointer_cast<BaseRoot>(connSmartPtr);
	ret = UrcSingleton::Instance().AddUrIn<boost::shared_ptr<BaseRoot>, BaseRoot>(strUrName, baseSmartPtr, SULOKI_OBJECT_TYPE_URC_BASE, 0, true, 0, Loki::Type2Type< boost::shared_ptr<BaseRoot> >());
	if (ret != SUCCESS)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUr error, error code:" << ret;// << __FILE__ << __LINE__;
		return ret;
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
		SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_REGSERVICE_MESSAGEID_URC_PROTO);
		req.set_messagetype(SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		req.set_urckey(strUrName);
		req.set_urcval(strBody);
		req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
		req.set_attrib(0);
		req.set_dir(true);
		suloki::SulokiOperatorUrcMsgBody body;
		;
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
		//
		std::string strMsg;
		SulokiProtoSwrap::EncodeProtocol(req, strMsg);
		//
		Int id = EventManagerSingleton::Instance().GetFreeEvent();
		if (id < 0)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "get GetFreeEvent fail";
			return URC_NOEVENT_ERRORCODE;
		}
		std::stringstream strStream;
		if (req.routers_size() > 0)//tVal.routers(tVal.routers_size() - 1) << "_" <<
			strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << req.routers(req.routers_size() - 1) << "_" << req.businessid() << "_" << req.messageid() << "_" << req.sequencenumber();
		else
			strStream << SULOKI_URCSYS_RESOURCE_URC_BASE << "response/" << req.businessid() << "_" << req.messageid() << "_" << req.sequencenumber();
		//boost::function<void(Int, Uint, bool)> func = boost::bind(&Urc::FuncRes, this, id, _1, _1);
		//SulokiContext context;
		//context.set_id(id);
		std::auto_ptr<SulokiContext> contextSmart(new SulokiContext());
		if (contextSmart.get() == NULL)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory";
			return NOMEMORY_ERRORCODE;
		}
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(contextSmart.get(), "");
#endif
		contextSmart->set_id(id);
		//?????
		//SulokiMessage* pMsgBack = contextSmart->mutable_msgori();
		//if (pMsgBack == NULL)
		//{
		//	SULOKI_ERROR_LOG_BASEFRAMEWORK << "mutable_msgori error,maybe no memory";
		//	return NOMEMORY_ERRORCODE;
		//}
		//SulokiProtoSwrap::MakeSimpleCopy(req, *pMsgBack);
		AsyncNewFunc func(BindFirst(AsyncFunc(this, &Urc::FuncRes), contextSmart.release()));
		ret = AddUrIn(strStream.str(), func, SULOKI_EVENT_TYPE_URC_BASE, 0, false, -1, Loki::Type2Type<AsyncNewFunc>());
		if (ret != SUCCESS)
		{
			EventManagerSingleton::Instance().FreeEvent(id);
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUrIn fail, error code:" << ret;
			return ret;
		}
		//
		connSmartPtr->WriteAsync(strMsg.c_str(), strMsg.length());
		std::auto_ptr<SulokiMessage> resSmart;
		ret = EventManagerSingleton::Instance().Wait(id, strStream.str(), 1000, resSmart);
		if (ret != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "event Wait fail, error code:" << ret;
			return ret;
		}
		//req = *resSmart;
		if (!resSmart->has_errorcode())
			return FAIL;
		std::auto_ptr<SulokiMessage> msgSmart(new SulokiMessage());
		if (msgSmart.get() == NULL)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory";
		}
		else
		{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
			NEW_MDEBUG(msgSmart.get(), "");
#endif
			Suloki::SulokiProtoSwrap::MakeBaseMessage(*msgSmart);
			msgSmart->set_businessid(SULOKI_URC_BISINESSID_PROTO);
			msgSmart->set_messageid(SULOKI_CONNTOURCSERVEROK_MESSAGEID_URC_PROTO);
			msgSmart->set_messagetype(SulokiMessage::notice);
			msgSmart->set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
			//msgSmart->set_urckey(strUrName);
			//msgSmart->set_urcval(strSubName);
			PostToMainModule(msgSmart);
		}
		return resSmart->errorcode();
	}
	return SUCCESS;
}
SulokiRet Urc::ReqRes(SULOKI_IN std::string strGroupName, SULOKI_IN std::string strServiceName, SULOKI_INOUT SulokiMessage& req, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<SulokiContext> contextOriSmart)
{
	if (!(strGroupName.length() > 0 && req.messagetype() == SulokiMessage::request))
		return INVALIDPARA_ERRORCODE;
	if (strGroupName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	if (strServiceName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	Ret ret = SUCCESS;
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
			SulokiMessage req;
			Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
			req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
			req.set_messageid(SULOKI_OBTAINSTATES_MESSAGEID_URC_PROTO);
			req.set_messagetype(SulokiMessage::request);
			req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
			req.set_urckey(strUrStream.str());
			//req.set_urcval(data);
			//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
			//req.set_attrib(0);
			//req.set_dir(bDir);
			suloki::SulokiOperatorUrcMsgBody stateBody;
			Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, stateBody);
			ret = ReqresMsgToUrcserver(strUrStream.str(), req, timeout);
			if (ret != SUCCESS)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "can't obtaib service's state info in group from urc server, error code:" << ret;
				return ret;
			}
			suloki::SulokiGroupStateUrcMsgBody body;
			ret = SulokiProtoSwrap::GetBody<suloki::SulokiGroupStateUrcMsgBody>(req, body);
			if (ret != SUCCESS)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "GetBody SulokiGroupStateUrcMsgBody error, error code:" << ret;
				return ret;
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
		SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_GET_MESSAGEID_URC_PROTO);
		req.set_messagetype(SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		req.set_urckey(strUrStream.str());
		//req.set_urcval(data);
		req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
		//req.set_attrib(0);
		//req.set_dir(bDir);
		suloki::SulokiOperatorUrcMsgBody body;
		;
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
		ret = ReqresMsgToUrcserver(strUrStream.str(), req, timeout);
		if (ret != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "can't obtaib service's state info from urc server, error code:" << ret;
			return ret;
		}
		if (!req.has_urcval())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "urcval field have not existed error";
			return INVALIDPARA_ERRORCODE;
		}
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
		ret = ConnAndReg(strStream.str(), state.ip(), state.port());
		if (ret != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server, error code:" << ret;
			return ret;
		}
	}
	if (asyncCb == NULL)
		return ReqresMsgToUrcserver(SULOKI_REMOTED_RESOURCE_URC_BASE, req, timeout, strStream.str());
	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
	return ReqresMsgToUrcserver(SULOKI_REMOTED_RESOURCE_URC_BASE, req, timeout, asyncCbNew, strStream.str());
}
SulokiRet Urc::Notify(SULOKI_IN std::string strGroupName, SULOKI_IN std::string strServiceName, SULOKI_IN SulokiMessage& notice)
{
	if (notice.messagetype() != SulokiMessage::notice)
		return INVALIDPARA_ERRORCODE;
	if (strGroupName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	if (strServiceName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	Ret ret = SUCCESS;
	std::stringstream strStream;
	strStream << SULOKI_CONNECT2SERVICEPATH_NET_URC_BASE << strGroupName << "_" << strServiceName;
	boost::shared_ptr<BaseRoot> baseSmartPtr;
	if (!(GetUr_Urcsys(strStream.str(), baseSmartPtr) == SUCCESS && baseSmartPtr.get() != NULL))
	{
		std::stringstream strUrStream;
		strUrStream << SULOKI_REMOTED_RESOURCE_URC_BASE + SULOKI_SERVICES_PATHNAME_URC_BASE + strGroupName + "/" + strServiceName;
		SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_GET_MESSAGEID_URC_PROTO);
		req.set_messagetype(SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		req.set_urckey(strUrStream.str());
		//req.set_urcval(data);
		req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
		//req.set_attrib(0);
		//req.set_dir(bDir);
		suloki::SulokiOperatorUrcMsgBody body;
		;
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
		ret = ReqresMsgToUrcserver(strUrStream.str(), req, 1000);
		if (ret != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "can't obtaib service's state info from urc server, error code:" << ret;
			return ret;
		}
		if (!req.has_urcval())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "urcval field have not existed error";
			return INVALIDPARA_ERRORCODE;
		}
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
		ret = ConnAndReg(strStream.str(), state.ip(), state.port());
		if (ret != SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no connect to urc server, error code:" << ret;
			return ret;
		}
	}
	return NotifyMsgToUrcserver(SULOKI_REMOTED_RESOURCE_URC_BASE, notice, strStream.str());
}
SulokiRet Urc::PostToMainModule(SULOKI_IN std::auto_ptr<SulokiMessage> msgSmart)
{
	//return PostToMainModuleUnvirtual(msgSmart);
	SulokiContext context;
	Post("main", msgSmart, context);
	return SUCCESS;
}
SulokiRet Urc::AddObject(SULOKI_IN std::string strUrName, SULOKI_IN boost::shared_ptr<BaseRoot>& baseSmartPtr)
{
	if (strUrName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	//return AddObjectUnvirtual(strUrName, baseSmartPtr);
	return AddUr(strUrName, baseSmartPtr, SULOKI_OBJECT_TYPE_URC_BASE, 0, true, 0);
}
SulokiRet Urc::DelObject(SULOKI_IN std::string strUrName, SULOKI_OUT boost::shared_ptr<BaseRoot>& baseSmartPtr)
{
	if (strUrName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	//return DelObjectUnvirtual(strUrName, baseSmartPtr);
	return DelUr(strUrName, baseSmartPtr);
}
SulokiRet Urc::GetObject(SULOKI_IN std::string strUrName, SULOKI_OUT boost::shared_ptr<BaseRoot>& baseSmartPtr)
{
	if (strUrName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	//return GetObjectUnvirtual(strUrName, baseSmartPtr);
	return GetUr(strUrName, baseSmartPtr);
}
SulokiRet Urc::GetSqlData(SULOKI_IN std::string strUrName, SULOKI_INOUT SulokiMessage& msg, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<SulokiContext> contextOriSmart)
{
	if (strUrName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	//return GetSqlDataUnvirtual(strUrName, msg, timeout);
	//return GetUr(strUrName, msg, timeout);
	if (asyncCb == NULL)
		return GetUr(strUrName, msg, timeout);
	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
	return GetUr(strUrName, msg, timeout, asyncCbNew);
}
//virtual SulokiRet GetSqlDataAsync(std::string strUrName, SulokiMessage& msg, long timeout, SulokiAsyncCb asyncCb, std::auto_ptr<SulokiContext> contextOriSmart)
//{
//	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
//	//return GetSqlDataAsyncUnvirtual(strUrName, msg, timeout, asyncCbNew);
//	return GetUr(strUrName, msg, timeout, asyncCbNew);
//}
SulokiRet Urc::AddNoSqlData(SULOKI_IN std::string strUrName, SULOKI_IN std::string& data, SULOKI_IN bool bDir, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<SulokiContext> contextOriSmart, bool bProxy)
{
	if (strUrName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	if (!bProxy)
	{
		if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
		{
			return AddUr(strUrName, data, SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, bDir);
		}
		SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_ADD_MESSAGEID_URC_PROTO);
		req.set_messagetype(SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		req.set_urckey(strUrName);
		req.set_urcval(data);
		req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
		req.set_attrib(0);
		req.set_dir(bDir);
		suloki::SulokiOperatorUrcMsgBody body;
		;
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
		if (asyncCb == NULL)
			return ReqresMsgToUrcserver(strUrName, req, timeout);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, bDir, timeout);
		AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
		return ReqresMsgToUrcserver(strUrName, req, timeout, asyncCbNew);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, bDir, timeout, asyncCbNew);
	}
	/*
	if (!(asyncCb != NULL && contextOriSmart.get() != NULL))
		return INVALIDPARA_ERRORCODE;
	if (!(strUrName.find(SULOKI_LOCAL_RESOURCE_URC_BASE) == 0 || strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) == 0))
		return INVALIDPARA_ERRORCODE;
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
	{
		//AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
		AsyncFunc asyncCbNew = AsyncFunc(asyncCb);
		return SubscribeUr(strUrName, strSubName, asyncCbNew, contextOriSmart);
	}
	std::string strProxyUrName = SULOKI_URCSYS_RESOURCE_URC_BASE + SULOKI_PROXYS_PATHNAME_URC_BASE + strUrName + "_" + strSubName;
	{
		boost::shared_ptr<BaseRoot> baseSmartPtr;
		if (GetUr_Urcsys(strProxyUrName, baseSmartPtr) == SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "sub have existed error";
			return FAIL;
		}
	}
	boost::shared_ptr<SubProxy> subProxySmartPtr(new SubProxy(strUrName, strSubName, asyncCb, contextOriSmart));
	if (subProxySmartPtr.get() == NULL)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "new SubProxy error";
		return FAIL;
	}
	boost::shared_ptr<BaseRoot> baseSmartPtr = boost::static_pointer_cast<BaseRoot>(subProxySmartPtr);
	if (UrcSingleton::Instance().AddUrIn<boost::shared_ptr<BaseRoot>, BaseRoot>(strProxyUrName, baseSmartPtr, SULOKI_OBJECT_TYPE_URC_BASE, 0, true, 0, Loki::Type2Type< boost::shared_ptr<BaseRoot> >()) != SUCCESS)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUr error";
		return FAIL;
	}
	SulokiMessage req;
	Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
	req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
	req.set_messageid(SULOKI_SUBSCRIBE_MESSAGEID_URC_PROTO);
	req.set_messagetype(SulokiMessage::request);
	req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
	req.set_urckey(strUrName);
	req.set_urcval(strSubName);
	//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
	//req.set_attrib(0);
	//req.set_dir(bDir);
	//suloki::SulokiOperatorUrcMsgBody body;
	//;
	//Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
	Ret ret = ReqresMsgToUrcserver(strUrName, req, timeout);
	if (ret != SUCCESS)
	{
		boost::shared_ptr<SubProxy> subProxySmartPtr;
		UrcSingleton::Instance().DelUr_Urcsys< boost::shared_ptr<SubProxy> >(strProxyUrName, subProxySmartPtr);
		return ret;
	}
	*/
	return FAIL;
}
SulokiRet Urc::DelNoSqlData(SULOKI_IN std::string strUrName, SULOKI_OUT std::string& data, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<SulokiContext> contextOriSmart, bool bProxy)
{
	if (strUrName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	if (!bProxy)
	{
		if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
		{
			return DelUr(strUrName, data);
		}
		SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_DEL_MESSAGEID_URC_PROTO);
		req.set_messagetype(SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		req.set_urckey(strUrName);
		//req.set_urcval(data);
		//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
		//req.set_attrib(0);
		//req.set_dir(bDir);
		suloki::SulokiOperatorUrcMsgBody body;
		;
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
		if (asyncCb == NULL)
			return ReqresMsgToUrcserver(strUrName, req, timeout); //return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, false, timeout);
		AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
		return ReqresMsgToUrcserver(strUrName, req, timeout, asyncCbNew);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, false, timeout, asyncCbNew);
	}
	/*
	if (!(strUrName.find(SULOKI_LOCAL_RESOURCE_URC_BASE) == 0 || strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) == 0))
		return INVALIDPARA_ERRORCODE;
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
	{
		return UnsubscribeUr(strUrName, strSubName);
	}
	std::string strProxyUrName = SULOKI_URCSYS_RESOURCE_URC_BASE + SULOKI_PROXYS_PATHNAME_URC_BASE + strUrName + "_" + strSubName;
	boost::shared_ptr<SubProxy> subProxySmartPtr;
	UrcSingleton::Instance().DelUr_Urcsys< boost::shared_ptr<SubProxy> >(strProxyUrName, subProxySmartPtr);
	//
	SulokiMessage req;
	Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
	req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
	req.set_messageid(SULOKI_UNSUBSCRIBE_MESSAGEID_URC_PROTO);
	req.set_messagetype(SulokiMessage::request);
	req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
	req.set_urckey(strUrName);
	req.set_urcval(strSubName);
	//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
	//req.set_attrib(0);
	//req.set_dir(bDir);
	//suloki::SulokiOperatorUrcMsgBody body;
	//;
	//Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
	return ReqresMsgToUrcserver(strUrName, req, timeout);
	*/
	return FAIL;
}
SulokiRet Urc::UpdateNoSqlData(SULOKI_IN std::string strUrName, SULOKI_INOUT std::string& data, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<SulokiContext> contextOriSmart)
{
	if (strUrName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
	{
		std::string strValOri;
		Int typeOld = 0;
		Ret ret = UpdateUr(strUrName, data, strValOri, typeOld);
		data = strValOri;
		if (ret == SUCCESS && typeOld == SULOKI_NOSQLDATA_TYPE_URC_BASE)
		{
			std::auto_ptr<SulokiMessage> msgSmart(new SulokiMessage());
			if (msgSmart.get() == NULL)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory, push sub will fail, strUrName:" << strUrName;
			}
			else
			{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
				NEW_MDEBUG(msgSmart.get(), "");
#endif
				Suloki::SulokiProtoSwrap::MakeBaseMessage(*msgSmart);
				msgSmart->set_businessid(SULOKI_URC_BISINESSID_PROTO);
				msgSmart->set_messageid(SULOKI_UPDATE_MESSAGEID_URC_PROTO);
				msgSmart->set_messagetype(SulokiMessage::push);
				msgSmart->set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
				msgSmart->set_urckey(strUrName);
				msgSmart->set_urcval(data);
				msgSmart->set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
				//req.set_attrib(0);
				//req.set_dir(bDir);
				suloki::SulokiOperatorUrcMsgBody body;
				;
				Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(*msgSmart, body);
				PostSub(strUrName, msgSmart);
			}
		}
		return ret;
	}
	SulokiMessage req;
	Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
	req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
	req.set_messageid(SULOKI_UPDATE_MESSAGEID_URC_PROTO);
	req.set_messagetype(SulokiMessage::request);
	req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
	req.set_urckey(strUrName);
	req.set_urcval(data);
	//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
	//req.set_attrib(0);
	//req.set_dir(bDir);
	suloki::SulokiOperatorUrcMsgBody body;
	;
	Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
	if (asyncCb == NULL)
		return ReqresMsgToUrcserver(strUrName, req, timeout); //return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, false, timeout);
	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
	return ReqresMsgToUrcserver(strUrName, req, timeout, asyncCbNew);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, false, timeout, asyncCbNew);
}
SulokiRet Urc::GetNoSqlData(SULOKI_IN std::string strUrName, SULOKI_OUT std::string& data, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<SulokiContext> contextOriSmart, bool bProxy)
{
	if (strUrName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	if (!bProxy)
	{
		if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
		{
			return GetUr(strUrName, data);
		}
		SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_GET_MESSAGEID_URC_PROTO);
		req.set_messagetype(SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		req.set_urckey(strUrName);
		req.set_urcval(data);
		//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
		//req.set_attrib(0);
		//req.set_dir(bDir);
		suloki::SulokiOperatorUrcMsgBody body;
		;
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
	/*
	;
	*/
	return FAIL;
}
SulokiRet Urc::GetUrDirectory(SULOKI_IN std::string strUrPath, SULOKI_OUT std::vector<std::string>& nameVector)
{
	if (strUrPath.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	if (strUrPath.find(SULOKI_LOCAL_RESOURCE_URC_BASE) != 0)
		return FAIL;
	return GetUrDir(strUrPath, nameVector);
}
SulokiRet Urc::Subscribe(SULOKI_IN std::string strUrName, SULOKI_IN std::string strSubName, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<SulokiContext> contextOriSmart)
{
	if (strUrName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	if (strSubName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
	{
		AsyncFunc asyncCbNew = AsyncFunc(FuncSubLocal);
		std::auto_ptr<SulokiContext> contextSmart(new SulokiContext());
		if (contextSmart.get() == NULL)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory";
			return NOMEMORY_ERRORCODE;
		}
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(contextSmart.get(), "");
#endif
		return SubscribeUr(strUrName, strSubName, asyncCbNew, contextSmart);
	}
	SulokiMessage req;
	Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
	req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
	req.set_messageid(SULOKI_SUBSCRIBE_MESSAGEID_URC_PROTO);
	req.set_messagetype(SulokiMessage::request);
	req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
	req.set_urckey(strUrName);
	req.set_urcval(strSubName);
	//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
	//req.set_attrib(0);
	//req.set_dir(bDir);
	//suloki::SulokiOperatorUrcMsgBody body;
	//;
	//Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
	if (asyncCb == NULL)
		return ReqresMsgToUrcserver(strUrName, req, timeout);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, bDir, timeout);
	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
	return ReqresMsgToUrcserver(strUrName, req, timeout, asyncCbNew);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, bDir, timeout, asyncCbNew);
}
SulokiRet Urc::Unsubscribe(SULOKI_IN std::string strUrName, SULOKI_IN std::string strSubName, SULOKI_IN long timeout, SULOKI_IN SulokiAsyncCb asyncCb, SULOKI_IN std::auto_ptr<SulokiContext> contextOriSmart)
{
	if (strUrName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	if (strSubName.find(SULOKI_STR_DEVIDED_SUB_URC_BASE) != std::string::npos)
		return INVALIDPARA_ERRORCODE;
	if (strUrName.find(SULOKI_REMOTED_RESOURCE_URC_BASE) != 0)
	{
		return UnsubscribeUr(strUrName, strSubName);
	}
	SulokiMessage req;
	Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
	req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
	req.set_messageid(SULOKI_UNSUBSCRIBE_MESSAGEID_URC_PROTO);
	req.set_messagetype(SulokiMessage::request);
	req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
	req.set_urckey(strUrName);
	req.set_urcval(strSubName);
	//req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
	//req.set_attrib(0);
	//req.set_dir(bDir);
	//suloki::SulokiOperatorUrcMsgBody body;
	//;
	//Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
	if (asyncCb == NULL)
		return ReqresMsgToUrcserver(strUrName, req, timeout);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, bDir, timeout);
	AsyncNewFunc asyncCbNew = AsyncNewFunc(BindFirst(AsyncFunc(asyncCb), contextOriSmart.release()));
	return ReqresMsgToUrcserver(strUrName, req, timeout, asyncCbNew);//return AddUr(strUrName, req, Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE, 0, bDir, timeout, asyncCbNew);
}
void Urc::NewMdebug(void* pTr, std::string str)
{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	New_Mdebug(pTr, str);
#endif
}
void Urc::DelMdebug(void* pTr)
{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	Del_Mdebug(pTr);
#endif
}
Ret Urc::ReqresMsgToUrcserver(std::string strUrName, SulokiMessage& msg, Int timeout, std::string strObjName)
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
		//SulokiContext context;
		//context.set_id(id);
		std::auto_ptr<SulokiContext> contextSmart(new SulokiContext());
		if (contextSmart.get() == NULL)
			return FAIL;
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(contextSmart.get(), "");
#endif
		contextSmart->set_id(id);
		//?????
		//SulokiMessage* pMsgBack = contextSmart->mutable_msgori();
		//if (pMsgBack == NULL)
		//	return FAIL;
		//SulokiProtoSwrap::MakeSimpleCopy(msg, *pMsgBack);
		AsyncNewFunc func(BindFirst(AsyncFunc(this, &Urc::FuncRes), contextSmart.release()));
		if (AddUrIn(strStream.str(), func, SULOKI_EVENT_TYPE_URC_BASE, 0, false, -1, Loki::Type2Type<AsyncNewFunc>()) != SUCCESS)
		{
			EventManagerSingleton::Instance().FreeEvent(id);
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "AddUrIn fail";
			return FAIL;
		}
		//
		connSmartPtr->WriteAsync(strMsg.c_str(), strMsg.length());
		std::auto_ptr<SulokiMessage> resSmart;
		if (EventManagerSingleton::Instance().Wait(id, strStream.str(), timeout, resSmart) != SUCCESS)
			return TIMEOUT_ERRORCODE;
		msg = *resSmart;
		if (!resSmart->has_errorcode())
			return FAIL;
		return resSmart->errorcode();
	}
	return URC_INVALIDPATH_ERRORCODE;
}
Ret Urc::ReqresMsgToUrcserver(std::string strUrName, SulokiMessage& msg, Int timeout, AsyncNewFunc asyncCb, std::string strObjName)
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
Ret Urc::NotifyMsgToUrcserver(std::string strUrName, SulokiMessage& msg, std::string strObjName)
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
void Urc::FuncSub(SulokiContext* pContextOri, std::auto_ptr<SulokiMessage> msgSmart, SulokiContext& contextNew)
{
	std::auto_ptr<SulokiContext> contextSmart(pContextOri);
	if (Suloki::Global::GetState() >= Suloki::STOP_GLOBALSTATE_BASEFRAMEWORK)
		return;
	if (!contextSmart->has_urname())
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "no urName field error";
		return;
	}
	if (msgSmart.get() == NULL)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "msgSmart is empty";
		return;
	}
	boost::shared_ptr<BaseRoot> baseSmartPtr;
	if (!(UrcSingleton::Instance().GetObject(contextSmart->urname(), baseSmartPtr) == Suloki::SUCCESS && baseSmartPtr.get() != NULL))
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "GetObject error, ur name:" << contextSmart->urname();
		return;
	}
	boost::shared_ptr<Suloki::UrcTcpConnection> connSmartPtr = boost::dynamic_pointer_cast<Suloki::UrcTcpConnection>(baseSmartPtr);
	if (connSmartPtr.get() == NULL)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "dynamic_pointer_cast to Suloki::UrcTcpConnection error";
		return;
	}
	if (msgSmart->has_urckey())
	{
		std::string strUrName = msgSmart->urckey();
		strUrName.erase(0, Suloki::SULOKI_LOCAL_RESOURCE_URC_BASE.length());
		strUrName.insert(0, Suloki::SULOKI_REMOTED_RESOURCE_URC_BASE);
		msgSmart->set_urckey(strUrName);
	}
	connSmartPtr->WriteAsync(*msgSmart);
}
void Urc::FuncSubLocal(SulokiContext* pContextOri, std::auto_ptr<SulokiMessage> msgSmart, SulokiContext& contextNew)
{
	std::auto_ptr<SulokiContext> contextSmart(pContextOri);
	if (Suloki::Global::GetState() >= Suloki::STOP_GLOBALSTATE_BASEFRAMEWORK)
		return;
	UrcSingleton::Instance().PostToMainModule(msgSmart);
}

Maintancer::Maintancer() :m_busyDegree(-1.0f)
{}
Maintancer::~Maintancer()
{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(this)
#endif
}
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
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	NEW_MDEBUG(m_timerSmart.get(), "");
#endif
	m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&Maintancer::Run, this)));
	if (m_threadRunnedSmart.get() == NULL)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "new m_threadRunnedSmart error";
		return FAIL;
	}
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	NEW_MDEBUG(m_threadRunnedSmart.get(), "");
#endif
	return SUCCESS;
}
Ret Maintancer::Stop(void)
{
	if (m_timerSmart.get() != NULL)
	{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		DEL_MDEBUG(m_timerSmart.get());
#endif
		m_timerSmart.reset(NULL);
	}
	m_ioService.stop();
	if (m_threadRunnedSmart.get() != NULL)
		m_threadRunnedSmart->join();
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(m_threadRunnedSmart.get());
#endif
	return SUCCESS;
}
Ret Maintancer::Clear(void)
{
	return SUCCESS;
}
void Maintancer::MyTimeout(void)
{
	if(Global::GetState() >= STOP_GLOBALSTATE_BASEFRAMEWORK)
		return;
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
		if (fabs(fBusy - m_busyDegree) > 0.0001f)
		{//busy degree have changed
			m_busyDegree = fBusy;
			//
			SulokiMessage req;
			Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
			req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
			req.set_messageid(SULOKI_UPDATE_MESSAGEID_URC_PROTO);
			req.set_messagetype(SulokiMessage::request);
			req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
			req.set_urckey(strUrName);
			req.set_urcval(strBody);
			req.set_type(Suloki::SULOKI_NOSQLDATA_TYPE_URC_BASE);
			req.set_attrib(0);
			req.set_dir(true);
			suloki::SulokiOperatorUrcMsgBody body;
			;
			Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(req, body);
			Ret ret = UrcSingleton::Instance().ReqresMsgToUrcserver(strUrName, req, 4000);
			if(ret != SUCCESS)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "update state error, error code:" << ret;
			}
			if(Global::GetState() >= STOP_GLOBALSTATE_BASEFRAMEWORK)
				return;
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
{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	DEL_MDEBUG(this)
#endif
}
Ret AppStateMachine::Init(void)
{
	/*
	if(ConfigSingleton::Instance().ReadConfig() != SUCCESS)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "ConfigSingleton::Instance().ReadConfig error";
		return FAIL;
	}
	*/
#ifdef SULOKI_WINDOWS_OS_SULOKI
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
#endif
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
