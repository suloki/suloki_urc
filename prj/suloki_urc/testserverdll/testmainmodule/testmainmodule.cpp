// testmainmodule.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "testmainmodule.h"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> WebsocketServer;
typedef WebsocketServer::message_ptr message_ptr;
const std::string URC_WSPATH = "/local/wsconnection/";
class WsConnect : public BaseRoot
{
public:
	WsConnect(WebsocketServer* server, websocketpp::connection_hdl hdl)
	{
		m_server = server;
		m_hdl = hdl;
	}
	virtual ~WsConnect()
	{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		DEL_MDEBUG(this)
#endif
	}
	WebsocketServer* GetServer(void)
	{
		return m_server;
	}
	websocketpp::connection_hdl GetHdl(void)
	{
		return m_hdl;
	}
private:
	WsConnect(WsConnect& ref) {}
	WsConnect& operator=(WsConnect& ref) { return *this; }
protected:
	WebsocketServer* m_server;
	websocketpp::connection_hdl m_hdl;
};
void OnOpen(WebsocketServer *server, websocketpp::connection_hdl hdl)
{
	boost::shared_ptr<WsConnect> wsconnSmartPtr(new WsConnect(server, hdl));
	if (wsconnSmartPtr.get() == NULL)
		return;
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
	NEW_MDEBUG(wsconnSmartPtr.get(), "")
#endif
	std::stringstream strStream;
	strStream << URC_WSPATH << hdl.lock().get();
	boost::shared_ptr<BaseRoot> baseSmartPtr = boost::static_pointer_cast<BaseRoot>(wsconnSmartPtr);
	if (MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->AddObject(strStream.str(), baseSmartPtr) != Suloki::SUCCESS)
	{
		std::cout << "AddUr error " << __FILE__ << __LINE__ << std::endl;
		return;
	}
	std::cout << "have client connected" << std::endl;
}
void OnClose(WebsocketServer *server, websocketpp::connection_hdl hdl)
{
	std::stringstream strStream;
	strStream << URC_WSPATH << hdl.lock().get();
	boost::shared_ptr<BaseRoot> baseSmartPtr;
	if (MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->DelObject(strStream.str(), baseSmartPtr) != Suloki::SUCCESS)
	{
		std::cout << "DelUr error " << __FILE__ << __LINE__ << std::endl;
		return;
	}
	//boost::shared_ptr<WsConnect> connSmartPtr = boost::dynamic_pointer_cast<WsConnect>(baseSmartPtr);
	//if (connSmartPtr.get() == NULL)
	//	return;
	std::cout << "have client disconnected" << std::endl;
}
void AsyncFunc(SulokiContext* pContextOri, std::auto_ptr<SulokiMessage> msgSmart, SulokiContext& contextNew)//Suloki::Uint pCcontext, Suloki::Uint msgPtr, bool bTimeout)
{
	std::auto_ptr<SulokiContext> contextSmart(pContextOri);
	if(Suloki::Global::GetState() >= Suloki::STOP_GLOBALSTATE_BASEFRAMEWORK)
		return;
	//
	if (!contextNew.has_b())
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "contextNew has not b field";
		return;
	}
	//?????
	if (!contextSmart->has_msgori())
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "contextSmart has not msgori field";
		return;
	}
	if (contextNew.b())
	{
		std::cout << "timeout.msg info:" << contextSmart->msgori() << std::endl;// , businessid:" << contextSmart->msgori().businessid() << "; messageid:" << contextSmart->msgori().messageid() << std::endl;
		return;
	}
	//
	if (msgSmart->businessid() == SULOKI_URC_BISINESSID_PROTO && msgSmart->messageid() == SULOKI_SQL_MESSAGEID_URC_PROTO && msgSmart->messagetype() == SulokiMessage::response)
	{
		std::cout << "async sql query res in AsyncFunc" << std::endl;
		suloki::SulokiSqlResUrcMsgBody resBody;
		if(Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiSqlResUrcMsgBody>(*msgSmart, resBody) != Suloki::SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "GetBody SulokiSqlResUrcMsgBody error";
			return;
		}
		for (int i = 0; i < resBody.sqlres_size(); i++)
		{
			const suloki::SulokiSqlResItemUrcMsgBody& bodyitem = resBody.sqlres(i);
			for (int j = 0; j < bodyitem.sqlresitem_size(); j++)
				;//std::cout << bodyitem.sqlresitem(j) << std::endl;
		}
	}
	//
	if (!contextSmart->has_urname())
	{
		std::cout << "context has not urname field error" << std::endl;
		return;
	}
	std::string wsconnUrName = contextSmart->urname();
	std::cout << "AsyncFunc,wsconnect name:" << wsconnUrName << std::endl;
	std::cout << "AsyncFunc,timeout:" << contextNew.b() << std::endl;
	boost::shared_ptr<BaseRoot> baseSmartPtr;
	if (!(MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetObject(wsconnUrName, baseSmartPtr) == Suloki::SUCCESS && baseSmartPtr.get() != NULL))
	{
		std::cout << "error:ws conn urname have not existed" << std::endl;
		return;
	}
	boost::shared_ptr<WsConnect> connSmartPtr = boost::dynamic_pointer_cast<WsConnect>(baseSmartPtr);
	if (connSmartPtr.get() == NULL)
		return;
	//connSmartPtr->GetServer()->send(connSmartPtr->GetHdl(), "test ok", websocketpp::frame::opcode::text);
	/*
	std::string wsconnUrName;
	{//pop router
		int routerSize = msgSmart->routers_size();
		if (routerSize > 0)
			wsconnUrName = msgSmart->routers(routerSize - 1);
		//????? slow
		std::vector<std::string> nameVec;
		for (Suloki::Int i = 0; i < routerSize - 1; i++)
			nameVec.push_back(msgSmart->routers(i));
		msgSmart->clear_routers();
		for (Suloki::Int i = 0; i < routerSize - 1; i++)
			msgSmart->add_routers(nameVec[i]);
	}
	std::cout << "AsyncFunc,wsconnect name:" << wsconnUrName << std::endl;
	std::cout << "AsyncFunc,timeout:" << contextNew.b() << std::endl;
	boost::shared_ptr<BaseRoot> baseSmartPtr;
	if (!(MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetObject(wsconnUrName, baseSmartPtr) == Suloki::SUCCESS && baseSmartPtr.get() != NULL))
	{
		std::cout << "error:ws conn urname have not existed" << std::endl;
		return;
	}
	boost::shared_ptr<WsConnect> connSmartPtr = boost::dynamic_pointer_cast<WsConnect>(baseSmartPtr);
	if (connSmartPtr.get() == NULL)
		return;
	connSmartPtr->GetServer()->send(connSmartPtr->GetHdl(), "test ok", websocketpp::frame::opcode::text);
	*/
}
void AsyncFuncSub(SulokiContext* pContextOri, std::auto_ptr<SulokiMessage> msgSmart, SulokiContext& contextNew)//Suloki::Uint pCcontext, Suloki::Uint msgPtr, bool bTimeout)
{
	std::auto_ptr<SulokiContext> contextSmart(pContextOri);
	if (Suloki::Global::GetState() >= Suloki::STOP_GLOBALSTATE_BASEFRAMEWORK)
		return;
	//
	if (msgSmart.get() == NULL)
	{
		std::cout << "msgSmart is NULL error" << std::endl;
		return;
	}
	if (msgSmart->messagetype() != SulokiMessage::push)
	{
		std::cout << "is not push msg type error, type:" << msgSmart->messagetype() << std::endl;
		return;
	}
	//
	if (!contextSmart->has_urname())
	{
		std::cout << "context has not urname field error" << std::endl;
		return;
	}
	;
}
void OnMessage(WebsocketServer *server, websocketpp::connection_hdl hdl, message_ptr msg)
{
	/*
	std::string strMsg = msg->get_payload();
	std::cout << strMsg << std::endl;
	//
	std::string strRespon = "receive: ";
	strRespon.append(strMsg);
	//
	server->send(hdl, strRespon, websocketpp::frame::opcode::text);
	*/
	for (Suloki::Int i = 0; i < 1; i++)
	{
		Suloki::Ret ret = Suloki::SUCCESS;
		{//remoted sql query sync
			SulokiMessage req;
			Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
			req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
			req.set_messageid(SULOKI_SQL_MESSAGEID_URC_PROTO);
			req.set_messagetype(SulokiMessage::request);
			req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
			/*
			std::stringstream strStream;
			strStream << URC_WSPATH << hdl.lock().get();
			req.add_routers(strStream.str());
			*/
			suloki::SulokiSqlReqUrcMsgBody body;
			body.set_urckey(Suloki::SULOKI_SQL_NAME_URC_BASE);
			body.set_urcsql("select * from user_v");
			Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiSqlReqUrcMsgBody>(req, body);
			//std::string strMsg;
			//SulokiProtoSwrap::EncodeProtocol<suloki::SulokiOperatorUrcMsgBody>(req, body, strMsg);
			//if (Suloki::UrcSingleton::Instance().GetUr("/remoted/sqldata", req, 1000) != Suloki::SUCCESS)
			//	std::cout << "sync GetUr sql data error" << std::endl;
			//else
			//	std::cout << "sync GetUr sql data ok" << std::endl;
			ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetSqlData("/remoted/sqldata", req, 3000);
			if (ret != Suloki::SUCCESS)
				std::cout << "sync GetUr sql data error, error code:" << ret << std::endl;
			else
			{
				std::cout << "sync GetUr sql data ok" << std::endl;
				suloki::SulokiSqlResUrcMsgBody resBody;
				Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiSqlResUrcMsgBody>(req, resBody);
				for (int i = 0; i < resBody.sqlres_size(); i++)
				{
					const suloki::SulokiSqlResItemUrcMsgBody& bodyitem = resBody.sqlres(i);
					for (int j = 0; j < bodyitem.sqlresitem_size(); j++)
						;//std::cout << bodyitem.sqlresitem(j) << std::endl;
				}
			}
		}
	{//remoted sql query async
		SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_SQL_MESSAGEID_URC_PROTO);
		req.set_messagetype(SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		//
		//std::stringstream strStream;
		//strStream << URC_WSPATH << hdl.lock().get();
		//req.add_routers(strStream.str());
		//
		suloki::SulokiSqlReqUrcMsgBody body;
		body.set_urckey(Suloki::SULOKI_SQL_NAME_URC_BASE);
		body.set_urcsql("select * from user_v");
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiSqlReqUrcMsgBody>(req, body);
		//Suloki::Urc<>::AsyncFunc asyncCb = boost::bind(&MyModuleStateMachine::AsyncFunc, this, 0, _1, _1);
		std::auto_ptr<SulokiContext> contextOriSmart(new SulokiContext);
		if (contextOriSmart.get() == NULL)
			return;
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(contextOriSmart.get(), "")
#endif
			//?????
			//SulokiMessage* pMsgBack = contextOriSmart->mutable_msgori();
			//if (pMsgBack == NULL)
			//	return;
			//Suloki::SulokiProtoSwrap::MakeSimpleCopy(req, *pMsgBack);
		{
			std::stringstream strStream;
			strStream << "businessid:" << req.businessid() << ";messageid:" << req.messageid();
			contextOriSmart->set_msgori(strStream.str());
		}
		std::stringstream strStream;
		strStream << URC_WSPATH << hdl.lock().get();
		contextOriSmart->set_urname(strStream.str());
		ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetSqlData("/remoted/sqldata", req, 3000, AsyncFunc, contextOriSmart);
		if (ret != Suloki::SUCCESS)
			std::cout << "async GetUr sql data error, error code:" << ret << std::endl;
		else
			std::cout << "async GetUr sql data ok" << std::endl;
	}
	//
	{//remoted nosql add sync
		std::string strVal = "nosqldata_val";
		ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->AddNoSqlData("/remoted/nosqldatakey", strVal, false, 5000);
		if (ret != Suloki::SUCCESS)
			std::cout << "sync AddUr nosql data error, error code:" << ret << std::endl;
		else
			std::cout << "sync AddUr nosql data ok" << std::endl;
	}
	{//remoted nosql get sync
		std::string strVal = "";
		ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetNoSqlData("/remoted/nosqldatakey", strVal, 8000);
		if (ret != Suloki::SUCCESS)
			std::cout << "sync GetUr nosql data error, error code:" << ret << std::endl;
		else
			std::cout << "sync GetUr nosql data ok, val=" << strVal << std::endl;
	}
	//
	{//req to a server sync
		SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_SYSTEM_BISINESSID_PROTO);
		req.set_messageid(SULOKI_TEST_MESSAGEID_SYSTEM_PROTO);
		req.set_messagetype(SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		/*
		std::stringstream strStream;
		strStream << URC_WSPATH << hdl.lock().get();
		req.add_routers(strStream.str());
		*/
		suloki::SulokiTestMsgBody body;
		body.set_test("testreq");
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiTestMsgBody>(req, body);
		ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->ReqRes("testgroup", "testserver0", req, 10000);
		if (ret != Suloki::SUCCESS)
			std::cout << "sync ReqRes to service error, error code:" << ret << std::endl;
		else
			std::cout << "sync ReqRes to service ok" << std::endl;
	}
	{//req to a server sync
		SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_SYSTEM_BISINESSID_PROTO);
		req.set_messageid(SULOKI_TEST_MESSAGEID_SYSTEM_PROTO);
		req.set_messagetype(SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		/*
		std::stringstream strStream;
		strStream << URC_WSPATH << hdl.lock().get();
		req.add_routers(strStream.str());
		*/
		suloki::SulokiTestMsgBody body;
		body.set_test("testreq");
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiTestMsgBody>(req, body);
		ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->ReqRes("testgroup", "", req, 10000);
		if (ret != Suloki::SUCCESS)
			std::cout << "sync ReqRes to service error, error code:" << ret << std::endl;
		else
			std::cout << "sync ReqRes to service ok" << std::endl;
	}
	{//sub
		Suloki::Ret ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->Subscribe("/remoted/nosqldatakey", "testsub", 8000);
		if (ret != Suloki::SUCCESS)
			std::cout << "sync Subscribe error, error code:" << ret << std::endl;
		else
			std::cout << "sync Subscribe ok" << std::endl;
		std::string strVal = "nosqldata_val1";
		ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->UpdateNoSqlData("/remoted/nosqldatakey", strVal, 8000);
		if (ret != Suloki::SUCCESS)
			std::cout << "sync UpdateNoSqlData data error, error code:" << ret << std::endl;
		else
			std::cout << "sync UpdateNoSqlData data ok, val=" << strVal << std::endl;
	}
	{//unsub
		Suloki::Ret ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->Unsubscribe("/remoted/nosqldatakey", "testsub", 8000);
		if (ret != Suloki::SUCCESS)
			std::cout << "sync Unsubscribe error, error code:" << ret << std::endl;
		else
			std::cout << "sync Unsubscribe ok" << std::endl;
	}
	{//del
		std::string strVal;
		Suloki::Ret ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->DelNoSqlData("/remoted/nosqldatakey", strVal, 8000);
		if (ret != Suloki::SUCCESS)
			std::cout << "sync DelNoSqlData error, error code:" << ret << std::endl;
		else
			std::cout << "sync DelNoSqlData ok" << std::endl;
	}
	Suloki::Sleep(1000);
	}
	server->send(hdl, "test have completed", websocketpp::frame::opcode::text);
}

class MyModuleStateMachine : public Suloki::AppStateMachine
{
public:
	MyModuleStateMachine()
	{}
	virtual ~MyModuleStateMachine()
	{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		DEL_MDEBUG(this)
#endif
	}
	virtual Suloki::Ret Init(void)
	{
#ifdef SULOKI_WINDOWS_OS_SULOKI
		Suloki::ConfigSingleton::Instance().SetConfig(Suloki::SULOKI_LOGNAME_KEY_CONFIG_BASE, "testmainmodule");
		//Suloki::ConfigSingleton::Instance().SetConfig(Suloki::SULOKI_LOGLEVEL_KEY_CONFIG_BASE, Suloki::LOG_INFO_LEVEL);
		try{
			boost::property_tree::ptree root;
			boost::property_tree::read_json<boost::property_tree::ptree>("config", root);
			std::string logLevel = root.get<std::string>(Suloki::SULOKI_LOGLEVEL_KEY_CONFIG_BASE);
			Suloki::ConfigSingleton::Instance().SetConfig(Suloki::SULOKI_LOGLEVEL_KEY_CONFIG_BASE, logLevel);
		}
		catch (boost::property_tree::ptree_error pt){
			//SULOKI_ERROR_LOG_BASEFRAMEWORK << "read config error";
			std::cout << "read config error" << std::endl;
			return Suloki::FAIL;
		}
		catch (Suloki::Exception e){
			//SULOKI_ERROR_LOG_BASEFRAMEWORK << "read config error," << e.what();
			std::cout << "read config error," << e.what() << std::endl;
			return Suloki::FAIL;
		}
		//
		if (Suloki::AppStateMachine::Init() != 0)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "AppStateMachine::Init error";
			return Suloki::FAIL;
		}
#endif
		m_serverSmart = std::auto_ptr<WebsocketServer>(new WebsocketServer());
		if(m_serverSmart.get() == NULL)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "create WebsocketServer error";
			return Suloki::FAIL;
		}
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(m_serverSmart.get(), "")
#endif
		;
		Suloki::Global::SetState(Suloki::INIT_GLOBALSTATE_BASEFRAMEWORK);
		//
		SULOKI_INFO_LOG_BASEFRAMEWORK << "testmainmodule init successfully";
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Start(void)
	{
		if (Suloki::AppStateMachine::Start() != 0)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "AppStateMachine::Start() error";
			return 1;
		}
		//
		try {
			// Set logging settings
			m_serverSmart->set_access_channels(websocketpp::log::alevel::all);
			m_serverSmart->clear_access_channels(websocketpp::log::alevel::frame_payload);

			// Initialize ASIO
			m_serverSmart->init_asio();

			// Register our open handler
			m_serverSmart->set_open_handler(bind(&OnOpen, m_serverSmart.get(), ::_1));

			// Register our close handler
			m_serverSmart->set_close_handler(bind(&OnClose, m_serverSmart.get(), ::_1));

			// Register our message handler
			m_serverSmart->set_message_handler(bind(&OnMessage, m_serverSmart.get(), ::_1, ::_2));

			//Listen on port 2152
			m_serverSmart->listen(9002);

			//Start the server accept loop
			m_serverSmart->start_accept();
		}
		catch (websocketpp::exception const & e) {
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "websocketpp::exception, " << e.what();
			exit(1);
		}
		catch (...) {
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "websocketpp other exception";
			exit(1);
		}
		;
		m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&MyModuleStateMachine::FuncThread, this)));
		if (m_threadRunnedSmart.get() == NULL)
			return Suloki::FAIL;
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		NEW_MDEBUG(m_threadRunnedSmart.get(), "")
#endif
		;
		Suloki::Global::SetState(Suloki::START_GLOBALSTATE_BASEFRAMEWORK);
		SULOKI_INFO_LOG_BASEFRAMEWORK << "testmainmodule start successfully";
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Run(void)
	{
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Stop(void)
	{
		SULOKI_INFO_LOG_BASEFRAMEWORK << "testmainmodule will stop";
		//
		Suloki::Global::SetState(Suloki::STOP_GLOBALSTATE_BASEFRAMEWORK);
		;
		m_serverSmart->stop();
		//m_serverSmart.reset(NULL);
//#ifdef SULOKI_WINDOWS_OS_SULOKI
		if (m_threadRunnedSmart.get() != NULL)
			m_threadRunnedSmart->join();
//#endif
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		DEL_MDEBUG(m_serverSmart.get())
		m_serverSmart.reset(NULL);
#endif
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		DEL_MDEBUG(m_threadRunnedSmart.get())
		m_threadRunnedSmart.reset(NULL);
#endif
		;
		return AppStateMachine::Stop();
	}
	virtual Suloki::Ret Clear(void)
	{
		SULOKI_INFO_LOG_BASEFRAMEWORK << "testmainmodule will clear";
		//
		Suloki::Global::SetState(Suloki::CLEAR_GLOBALSTATE_BASEFRAMEWORK);
		;
		Suloki::IdManagerSingleton::Deinstance();
		;
		return AppStateMachine::Clear();
	}
protected:
	void FuncThread(void)
	{
		// Create a server endpoint
		//server echo_server;

		try {
			//Start the ASIO io_service run loop
			m_serverSmart->run();
		}
		catch (websocketpp::exception const & e) {
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "websocketpp::exception, " << e.what();
			exit(1);
		}
		catch (...) {
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "websocketpp other exception";
			exit(1);
		}
	}
private:
	MyModuleStateMachine(MyModuleStateMachine& ref) {}
	MyModuleStateMachine& operator=(MyModuleStateMachine& ref) { return *this; }
private:
	std::auto_ptr<boost::thread> m_threadRunnedSmart;
	std::auto_ptr<WebsocketServer> m_serverSmart;
};
typedef Suloki::Singleton<MyModuleStateMachine> MyModuleStateMachineSingleton;

class MyDispatcher : public Suloki::Dispatcher<SulokiMessage, SulokiContext>
{
public:
	MyDispatcher()
	{
		//std::stringstream sStream;
		//sStream << XHMOMSDK_SYSTEM_BUSINESSID_PROTOCOL << "_" << XHMOMSDK_TEST_MESSAGEID_PROTOCOL;
		//m_syncMsgHandlerMap[sStream.str()] = HandlerFunctor(this, &MyHandler::Handler_SYSTEM_TEST1);
		//m_syncMsgHandlerMap["test"] = HandlerFunctor(this, &MyDispatcher::Handler_SYSTEM_TEST1);
		{
			std::stringstream sStream;
			sStream << SULOKI_SYSTEM_BISINESSID_PROTO << "_" << SULOKI_START_MESSAGEID_SYSTEM_PROTO;
			m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_System_Start);
		}
		{
			std::stringstream sStream;
			sStream << SULOKI_SYSTEM_BISINESSID_PROTO << "_" << SULOKI_TEST_MESSAGEID_SYSTEM_PROTO;
			m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_System_Test);
		}
		{
			std::stringstream sStream;
			sStream << SULOKI_URC_BISINESSID_PROTO << "_" << SULOKI_CONNTOURCSERVEROK_MESSAGEID_URC_PROTO;
			m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_Urc_Connok);
		}
		{
			std::stringstream sStream;
			sStream << SULOKI_URC_BISINESSID_PROTO << "_" << SULOKI_UPDATE_MESSAGEID_URC_PROTO;
			m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_Urc_Update);
		}
	}
	virtual ~MyDispatcher()
	{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		DEL_MDEBUG(this)
#endif
	}
	//void SetSulokiUrcModuleInterface(SulokiUrcModuleInterface* pSulokiUrcModuleInterface)
	//{
	//	m_pSulokiUrcModuleInterface = pSulokiUrcModuleInterface;
	//}
protected:
	virtual std::string CalKey(SulokiMessage& msg)
	{
		std::stringstream sStream;
		sStream << msg.businessid() << "_" << msg.messageid();
		return sStream.str();// protocolReq.m_msgKey;
	}
	virtual Suloki::Ret HandleUnmatched(SulokiMessage& msg, SulokiContext& context)
	{
		std::cout << "some msg have not handler, businessid:" << msg.businessid() << ";messageid:" << msg.messageid() << std::endl;
		return Suloki::FAIL;
	}
protected:
	/*
	static void AsyncFunc(SulokiContext* pContextOri, std::auto_ptr<SulokiMessage> msgSmart, SulokiContext& contextNew)//Suloki::Uint pCcontext, Suloki::Uint msgPtr, bool bTimeout)
	{
		std::auto_ptr<SulokiContext> contextSmart(pContextOri);
		if(Suloki::Global::GetState() >= Suloki::STOP_GLOBALSTATE_BASEFRAMEWORK)
			return;
		if (!contextNew.has_b())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "contextNew has not b field";
			return;
		}
		std::cout << "AsyncFunc,timeout:" << contextNew.b() << std::endl;
	}
	*/
	//Suloki::Ret Handler_SYSTEM_TEST1(SulokiMessage& protocolReq, SulokiContext& paraIn, SulokiMessage& protocolRes, SulokiContext& paraOut)
	//{
	//	std::cout << "Handler_SYSTEM_TEST1" << std::endl;
	//	return Suloki::SUCCESS;
	//}
	Suloki::Ret Handler_System_Start(SulokiMessage& msg, SulokiContext& context)
	{
		//std::cout << "recv system start msg" << std::endl;
		return Suloki::SUCCESS;
	}
	Suloki::Ret Handler_System_Test(SulokiMessage& msg, SulokiContext& context)
	{
		std::cout << "recv system test msg" << std::endl;
		if (!context.has_b())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "context has not b field";
			return Suloki::FAIL;
		}
		if (context.b() && msg.messagetype() == SulokiMessage::request)
		{
			suloki::SulokiTestMsgBody body;
			if(Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiTestMsgBody>(msg, body) != Suloki::SUCCESS)
			{
				SULOKI_ERROR_LOG_BASEFRAMEWORK << "GetBody SulokiTestMsgBody error";
				return Suloki::FAIL;
			}
			//
			SulokiMessage res;
			Suloki::SulokiProtoSwrap::MakeResMessage(msg, res);
			res.set_errorcode(Suloki::SUCCESS);
			suloki::SulokiTestMsgBody resBody;
			resBody.set_test("testres");
			Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiTestMsgBody>(res, resBody);
			//
			if (!context.has_urname())
			{
				std::cout << "context has not urname field error" << std::endl;
				return Suloki::FAIL;
			}
			boost::shared_ptr<BaseRoot> baseSmartPtr;
			if (!(MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetObject(context.urname(), baseSmartPtr) == Suloki::SUCCESS && baseSmartPtr.get() != NULL))
				return Suloki::FAIL;
			boost::shared_ptr<Suloki::UrcTcpConnection> connSmartPtr = boost::dynamic_pointer_cast<Suloki::UrcTcpConnection>(baseSmartPtr);
			if (connSmartPtr.get() == NULL)
				return Suloki::FAIL;
			connSmartPtr->WriteAsync(res);// strResMsg.c_str(), strResMsg.length());
			return Suloki::SUCCESS;
		}
		return Suloki::FAIL;
	}
	Suloki::Ret Handler_Urc_Connok(SulokiMessage& msg, SulokiContext& context)
	{
		//std::cout << "recv urc conn ok msg" << std::endl;
		//Suloki::Ret ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->Subscribe(Suloki::SULOKI_SQL_NAME_URC_BASE, "sqlsub", 8000);
		//if (ret != Suloki::SUCCESS)
		//	std::cout << "sync Subscribe " << Suloki::SULOKI_SQL_NAME_URC_BASE << " error, error code:" << ret << std::endl;
		//else
		//	std::cout << "sync Subscribe " << Suloki::SULOKI_SQL_NAME_URC_BASE << " ok" << std::endl;
		return Suloki::SUCCESS;
	}
	Suloki::Ret Handler_Urc_Update(SulokiMessage& msg, SulokiContext& context)
	{
		std::cout << "recv urc update msg" << std::endl;
		if (msg.messagetype() == SulokiMessage::push)
		{
			std::cout << "recv urc update push msg,";
			if (msg.has_urckey())
				std::cout << "urc name:" << msg.urckey();
			if (msg.has_urcval())
				std::cout << ",urc val:" << msg.urcval();
			std::cout << std::endl;
		}
		return Suloki::SUCCESS;
	}
protected:
	//SulokiUrcModuleInterface* m_pSulokiUrcModuleInterface;
};
class MySulokiHandleModule : public SulokiHandleModuleInterface
{
public:
	//explicit MySulokiHandleModule(std::string moduleName){}
	MySulokiHandleModule(){}
	virtual ~MySulokiHandleModule()
	{
#ifdef SULOKI_MEMALLOCATOR_DEBUG_BASEFRAMEWORK
		DEL_MDEBUG(this)
#endif
	}
	//
	virtual SulokiRet Init(SULOKI_IN SulokiUrcModuleInterface* pSulokiUrcModuleInterface, SULOKI_IN std::string groupName, SULOKI_IN std::string serviceName, SULOKI_IN std::string strModuleName, SULOKI_IN std::string strConfig)
	{
		//MySulokiUrcModuleInterface::SetSulokiUrcModuleInterface(pSulokiUrcModuleInterface);
		//m_sulokiUrcModuleInterfaceSmart = sulokiUrcModuleInterfaceSmart;
		//m_pSulokiUrcModuleInterface = pSulokiUrcModuleInterface;
		//m_dispatcher.SetSulokiUrcModuleInterface(m_pSulokiUrcModuleInterface);
		return Suloki::SUCCESS;
	}
	virtual SulokiRet Start(void){ return Suloki::SUCCESS; }
	virtual SulokiRet TestValid(SULOKI_IN const SulokiMessage& msg){ return Suloki::SUCCESS; }
	virtual SulokiRet Handle(SULOKI_IN std::auto_ptr< SulokiMessage > msgSmart, SULOKI_IN SulokiContext& context)
	{
		return m_dispatcher.Handle(*msgSmart, context);
	}
	virtual SulokiRet Stop(void){ return Suloki::SUCCESS; }
	virtual SulokiRet Clear(void){ return Suloki::SUCCESS; }
	//
protected:
	//MySulokiHandleModule(){}
	//explicit MySulokiHandleModule(std::string modulePath){}// throw(Suloki::Exception){}
private:
	MySulokiHandleModule(MySulokiHandleModule& ref) {}
	MySulokiHandleModule& operator=(MySulokiHandleModule& ref) { return *this; }
protected:
	MyDispatcher m_dispatcher;
	//std::auto_ptr<SulokiUrcModuleInterface> m_sulokiUrcModuleInterfaceSmart;
	//SulokiUrcModuleInterface* m_pSulokiUrcModuleInterface;
};
typedef Suloki::Singleton<MySulokiHandleModule> MySulokiHandleModuleSingleton;

TESTMAINMODULE_API SulokiRet Init(SULOKI_IN SulokiUrcModuleInterface* pSulokiUrcModuleInterface, SULOKI_IN std::string groupName, SULOKI_IN std::string serviceName, SULOKI_IN std::string strModuleName, SULOKI_IN std::string strConfig)
{
	MySulokiUrcModuleInterface::SetSulokiUrcModuleInterface(pSulokiUrcModuleInterface);
	if (MyModuleStateMachineSingleton::Instance().Init() != 0)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "MyModuleStateMachineSingleton::Instance().Init() error";
		return -1;
	}
	//return SULOKI_SUCCESS;
	return MySulokiHandleModuleSingleton::Instance().Init(pSulokiUrcModuleInterface, groupName, serviceName, strModuleName, strConfig);
}
TESTMAINMODULE_API SulokiRet Start(void)
{
	if (MyModuleStateMachineSingleton::Instance().Start() != 0)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "MyModuleStateMachineSingleton::Instance().Start() error";
		return -1;
	}
	return MySulokiHandleModuleSingleton::Instance().Start();
}
TESTMAINMODULE_API SulokiRet TestValid(SULOKI_IN const SulokiMessage& msg)
{
	return MySulokiHandleModuleSingleton::Instance().TestValid(msg);
}
TESTMAINMODULE_API SulokiRet Handle(SULOKI_IN std::auto_ptr< SulokiMessage > msgSmart, SULOKI_IN SulokiContext& context)
{
	return MySulokiHandleModuleSingleton::Instance().Handle(msgSmart, context);
}
TESTMAINMODULE_API SulokiRet Stop(void)
{
	MySulokiHandleModuleSingleton::Instance().Stop();
	MyModuleStateMachineSingleton::Instance().Stop();
	return Suloki::SUCCESS;
}
TESTMAINMODULE_API SulokiRet Clear(void)
{
	MySulokiHandleModuleSingleton::Instance().Clear();
	MySulokiHandleModuleSingleton::Deinstance();
	MyModuleStateMachineSingleton::Instance().Clear();
	MyModuleStateMachineSingleton::Deinstance();
	return Suloki::SUCCESS;
}

/*
// This is an example of an exported variable
TESTMAINMODULE_API int ntestmainmodule=0;

// This is an example of an exported function.
TESTMAINMODULE_API int fntestmainmodule(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see testmainmodule.h for the class definition
Ctestmainmodule::Ctestmainmodule()
{
	return;
}
*/
