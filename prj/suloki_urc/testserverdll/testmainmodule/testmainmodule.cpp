// testmainmodule.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "testmainmodule.h"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

class MySulokiUrcModuleInterface
{
public:
	static void SetSulokiUrcModuleInterface(SulokiUrcModuleInterface* pSulokiUrcModuleInterface)
	{
		m_pSulokiUrcModuleInterface = pSulokiUrcModuleInterface;
	}
	static inline SulokiUrcModuleInterface* GetSulokiUrcModuleInterface(void)
	{
		return m_pSulokiUrcModuleInterface;
	}
private:
	MySulokiUrcModuleInterface(MySulokiUrcModuleInterface& ref) {}
	MySulokiUrcModuleInterface& operator=(MySulokiUrcModuleInterface& ref) { return *this; }
protected:
	static SulokiUrcModuleInterface* m_pSulokiUrcModuleInterface;
};
SulokiUrcModuleInterface* MySulokiUrcModuleInterface::m_pSulokiUrcModuleInterface = NULL;

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
	virtual ~WsConnect(){}
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
	boost::shared_ptr<WsConnect> wsconnSmartPtr;
	std::stringstream strStream;
	strStream << URC_WSPATH << hdl.lock().get();
	boost::shared_ptr<BaseRoot> baseSmartPtr = boost::static_pointer_cast<BaseRoot>(wsconnSmartPtr);
	if (MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->DelObject(strStream.str(), baseSmartPtr) != Suloki::SUCCESS)
	{
		std::cout << "DelUr error " << __FILE__ << __LINE__ << std::endl;
		return;
	}
	std::cout << "have client disconnected" << std::endl;
}
void AsyncFunc(suloki::SulokiContext* pContextOri, std::auto_ptr<suloki::SulokiMessage> msgSmart, suloki::SulokiContext& contextNew)//Suloki::Uint pCcontext, Suloki::Uint msgPtr, bool bTimeout)
{
	std::auto_ptr<suloki::SulokiContext> contextSmart(pContextOri);
	//
	if (!contextNew.has_b())
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "contextNew has not b field";
		return;
	}
	if (!contextSmart->has_msgori())
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "contextSmart has not msgori field";
		return;
	}
	if (contextNew.b())
	{
		std::cout << "timeout, businessid:" << contextSmart->msgori().businessid() << ";messageid:" << contextSmart->msgori().messageid() << std::endl;
		return;
	}
	//
	if (msgSmart->businessid() == SULOKI_URC_BISINESSID_PROTO && msgSmart->messageid() == SULOKI_SQL_MESSAGEID_URC_PROTO && msgSmart->messagetype() == suloki::SulokiMessage::response)
	{
		std::cout << "async sql query res in AsyncFunc" << std::endl;
		suloki::SulokiSqlResUrcMsgBody resBody;
		Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiSqlResUrcMsgBody>(*msgSmart, resBody);
		for (int i = 0; i < resBody.sqlres_size(); i++)
		{
			const suloki::SulokiSqlResItemUrcMsgBody& bodyitem = resBody.sqlres(i);
			for (int j = 0; j < bodyitem.sqlresitem_size(); j++)
				std::cout << bodyitem.sqlresitem(j) << std::endl;
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
	connSmartPtr->GetServer()->send(connSmartPtr->GetHdl(), "test ok", websocketpp::frame::opcode::text);
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
	{//remoted sql query sync
		suloki::SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_SQL_MESSAGEID_URC_PROTO);
		req.set_messagetype(suloki::SulokiMessage::request);
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
		if (MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetSqlData("/remoted/sqldata", req, 1000) != Suloki::SUCCESS)
			std::cout << "sync GetUr sql data error" << std::endl;
		else
		{
			std::cout << "sync GetUr sql data ok" << std::endl;
			suloki::SulokiSqlResUrcMsgBody resBody;
			Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiSqlResUrcMsgBody>(req, resBody);
			for (int i = 0; i < resBody.sqlres_size(); i++)
			{
				const suloki::SulokiSqlResItemUrcMsgBody& bodyitem = resBody.sqlres(i);
				for (int j = 0; j < bodyitem.sqlresitem_size(); j++)
					std::cout << bodyitem.sqlresitem(j) << std::endl;
			}
		}
	}
	{//remoted sql query async
		suloki::SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_URC_BISINESSID_PROTO);
		req.set_messageid(SULOKI_SQL_MESSAGEID_URC_PROTO);
		req.set_messagetype(suloki::SulokiMessage::request);
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
		//Suloki::Urc<>::AsyncFunc asyncCb = boost::bind(&MyAppStateMachine::AsyncFunc, this, 0, _1, _1);
		std::auto_ptr<suloki::SulokiContext> contextOriSmart(new suloki::SulokiContext);
		if (contextOriSmart.get() == NULL)
			return;
		suloki::SulokiMessage* pMsgBack = contextOriSmart->mutable_msgori();
		if (pMsgBack == NULL)
			return;
		Suloki::SulokiProtoSwrap::MakeSimpleCopy(req, *pMsgBack);
		std::stringstream strStream;
		strStream << URC_WSPATH << hdl.lock().get();
		contextOriSmart->set_urname(strStream.str());
		if (MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetSqlData("/remoted/sqldata", req, 1000, AsyncFunc, contextOriSmart) != Suloki::SUCCESS)
			std::cout << "async GetUr sql data error" << std::endl;
		else
			std::cout << "async GetUr sql data ok" << std::endl;
	}
	//
	{//remoted nosql add sync
		std::string strVal = "nosqldata_val";
		if (MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->AddNoSqlData("/remoted/nosqldatakey", strVal, false, 1000) != Suloki::SUCCESS)
			std::cout << "sync AddUr nosql data error" << std::endl;
		else
			std::cout << "sync AddUr nosql data ok" << std::endl;
	}
	{//remoted nosql add sync
		std::string strVal = "";
		if (MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetNoSqlData("/remoted/nosqldatakey", strVal, 1000) != Suloki::SUCCESS)
			std::cout << "sync GetUr nosql data error" << std::endl;
		else
			std::cout << "sync GetUr nosql data ok, val=" << strVal << std::endl;
	}
	//
	{//req to a server sync
		suloki::SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_SYSTEM_BISINESSID_PROTO);
		req.set_messageid(SULOKI_TEST_MESSAGEID_SYSTEM_PROTO);
		req.set_messagetype(suloki::SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		/*
		std::stringstream strStream;
		strStream << URC_WSPATH << hdl.lock().get();
		req.add_routers(strStream.str());
		*/
		suloki::SulokiTestMsgBody body;
		body.set_test("testreq");
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiTestMsgBody>(req, body);
		if (MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->ReqRes("testgroup", "testserver0", req, 1000) != Suloki::SUCCESS)
			std::cout << "sync ReqRes to service error" << std::endl;
		else
			std::cout << "sync ReqRes to service ok" << std::endl;
	}
	{//req to a server sync
		suloki::SulokiMessage req;
		Suloki::SulokiProtoSwrap::MakeBaseMessage(req);
		req.set_businessid(SULOKI_SYSTEM_BISINESSID_PROTO);
		req.set_messageid(SULOKI_TEST_MESSAGEID_SYSTEM_PROTO);
		req.set_messagetype(suloki::SulokiMessage::request);
		req.set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		/*
		std::stringstream strStream;
		strStream << URC_WSPATH << hdl.lock().get();
		req.add_routers(strStream.str());
		*/
		suloki::SulokiTestMsgBody body;
		body.set_test("testreq");
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiTestMsgBody>(req, body);
		if (MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->ReqRes("testgroup", "", req, 1000) != Suloki::SUCCESS)
			std::cout << "sync ReqRes to service error" << std::endl;
		else
			std::cout << "sync ReqRes to service ok" << std::endl;
	}
}

class MyAppStateMachine : public Suloki::AppStateMachine
{
public:
	MyAppStateMachine()
	{}
	virtual ~MyAppStateMachine()
	{}
	virtual Suloki::Ret Init(void)
	{
		Suloki::ConfigSingleton::Instance().SetConfig(Suloki::SULOKI_LOGNAME_KEY_CONFIG_BASE, "testmainmodule");
		//Suloki::ConfigSingleton::Instance().SetConfig(Suloki::SULOKI_LOGLEVEL_KEY_CONFIG_BASE, Suloki::LOG_INFO_LEVEL);
		try{
			boost::property_tree::ptree root;
			boost::property_tree::read_json<boost::property_tree::ptree>("config", root);
			std::string logLevel = root.get<std::string>(Suloki::SULOKI_LOGLEVEL_KEY_CONFIG_BASE);
			Suloki::ConfigSingleton::Instance().SetConfig(Suloki::SULOKI_LOGLEVEL_KEY_CONFIG_BASE, logLevel);
		}
		catch (boost::property_tree::ptree_error pt){
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "read config error";
			return Suloki::FAIL;
		}
		catch (Suloki::Exception e){
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "read config error," << e.what();
			return Suloki::FAIL;
		}
		//
		if (Suloki::AppStateMachine::Init() != 0)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "AppStateMachine::Init error";
			return 1;
		}
		//
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
		;
		m_threadRunnedSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&MyAppStateMachine::FuncThread, this)));
		if (m_threadRunnedSmart.get() == NULL)
			return Suloki::FAIL;
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
		m_server.stop();
		if (m_threadRunnedSmart.get() != NULL)
			m_threadRunnedSmart->join();
		;
		return AppStateMachine::Stop();
	}
	virtual Suloki::Ret Clear(void)
	{
		SULOKI_INFO_LOG_BASEFRAMEWORK << "testmainmodule will clear";
		//
		Suloki::Global::SetState(Suloki::CLEAR_GLOBALSTATE_BASEFRAMEWORK);
		;

		;
		return AppStateMachine::Clear();
	}
protected:
	void FuncThread(void)
	{
		// Create a server endpoint
		//server echo_server;

		try {
			// Set logging settings
			m_server.set_access_channels(websocketpp::log::alevel::all);
			m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

			// Initialize ASIO
			m_server.init_asio();

			// Register our open handler
			m_server.set_open_handler(bind(&OnOpen, &m_server, ::_1));

			// Register our close handler
			m_server.set_close_handler(bind(&OnClose, &m_server, ::_1));

			// Register our message handler
			m_server.set_message_handler(bind(&OnMessage, &m_server, ::_1, ::_2));

			//Listen on port 2152
			m_server.listen(9002);

			//Start the server accept loop
			m_server.start_accept();

			//Start the ASIO io_service run loop
			m_server.run();
		}
		catch (websocketpp::exception const & e) {
			std::cout << e.what() << std::endl;
		}
		catch (...) {
			std::cout << "other exception" << std::endl;
		}
	}
private:
	MyAppStateMachine(MyAppStateMachine& ref) {}
	MyAppStateMachine& operator=(MyAppStateMachine& ref) { return *this; }
private:
	std::auto_ptr<boost::thread> m_threadRunnedSmart;
	WebsocketServer m_server;
};
typedef Suloki::Singleton<MyAppStateMachine> MyAppStateMachineSingleton;

class MyDispatcher : public Suloki::Dispatcher<suloki::SulokiMessage, suloki::SulokiContext>
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
	}
	virtual ~MyDispatcher()
	{}
	//void SetSulokiUrcModuleInterface(SulokiUrcModuleInterface* pSulokiUrcModuleInterface)
	//{
	//	m_pSulokiUrcModuleInterface = pSulokiUrcModuleInterface;
	//}
protected:
	virtual std::string CalKey(suloki::SulokiMessage& msg)
	{
		std::stringstream sStream;
		sStream << msg.businessid() << "_" << msg.messageid();
		return sStream.str();// protocolReq.m_msgKey;
	}
	virtual Suloki::Ret HandleUnmatched(suloki::SulokiMessage& msg, suloki::SulokiContext& context)
	{
		std::cout << "some msg have not handler, businessid:" << msg.businessid() << ";messageid:" << msg.messageid();
		return Suloki::FAIL;
	}
protected:
	static void AsyncFunc(suloki::SulokiContext* pContextOri, std::auto_ptr<suloki::SulokiMessage> msgSmart, suloki::SulokiContext& contextNew)//Suloki::Uint pCcontext, Suloki::Uint msgPtr, bool bTimeout)
	{
		if (!contextNew.has_b())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "contextNew has not b field";
			return;
		}
		std::auto_ptr<suloki::SulokiContext> contextSmart(pContextOri);
		std::cout << "AsyncFunc,timeout:" << contextNew.b() << std::endl;
	}
	//Suloki::Ret Handler_SYSTEM_TEST1(suloki::SulokiMessage& protocolReq, suloki::SulokiContext& paraIn, suloki::SulokiMessage& protocolRes, suloki::SulokiContext& paraOut)
	//{
	//	std::cout << "Handler_SYSTEM_TEST1" << std::endl;
	//	return Suloki::SUCCESS;
	//}
	Suloki::Ret Handler_System_Start(suloki::SulokiMessage& msg, suloki::SulokiContext& context)
	{
		std::cout << "recv system start msg" << std::endl;
		return Suloki::SUCCESS;
	}
	Suloki::Ret Handler_System_Test(suloki::SulokiMessage& msg, suloki::SulokiContext& context)
	{
		std::cout << "recv system test msg" << std::endl;
		if (!context.has_b())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "context has not b field";
			return Suloki::FAIL;
		}
		if (context.b() && msg.messagetype() == suloki::SulokiMessage::request)
		{
			suloki::SulokiTestMsgBody body;
			Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiTestMsgBody>(msg, body);
			//
			suloki::SulokiMessage res;
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
protected:
	//SulokiUrcModuleInterface* m_pSulokiUrcModuleInterface;
};
class MySulokiHandleModule : public SulokiHandleModuleInterface
{
public:
	//explicit MySulokiHandleModule(std::string moduleName){}
	MySulokiHandleModule(){}
	virtual ~MySulokiHandleModule(){}
	//
	virtual SulokiRet Init(SULOKI_IN SulokiUrcModuleInterface* pSulokiUrcModuleInterface, SULOKI_IN std::string groupName, SULOKI_IN std::string serviceName, SULOKI_IN std::string strModuleName, SULOKI_IN std::string strConfig)
	{
		MySulokiUrcModuleInterface::SetSulokiUrcModuleInterface(pSulokiUrcModuleInterface);
		//m_sulokiUrcModuleInterfaceSmart = sulokiUrcModuleInterfaceSmart;
		//m_pSulokiUrcModuleInterface = pSulokiUrcModuleInterface;
		//m_dispatcher.SetSulokiUrcModuleInterface(m_pSulokiUrcModuleInterface);
		return Suloki::SUCCESS;
	}
	virtual SulokiRet Start(void){ return Suloki::SUCCESS; }
	virtual SulokiRet TestValid(SULOKI_IN const suloki::SulokiMessage& msg){ return Suloki::SUCCESS; }
	virtual SulokiRet Handle(SULOKI_IN std::auto_ptr< suloki::SulokiMessage > msgSmart, SULOKI_IN suloki::SulokiContext& context)
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
	if (MyAppStateMachineSingleton::Instance().Init() != 0)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "MyAppStateMachineSingleton::Instance().Init() error";
		return -1;
	}
	//return SULOKI_SUCCESS;
	return MySulokiHandleModuleSingleton::Instance().Init(pSulokiUrcModuleInterface, groupName, serviceName, strModuleName, strConfig);
}
TESTMAINMODULE_API SulokiRet Start(void)
{
	if (MyAppStateMachineSingleton::Instance().Start() != 0)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "MyAppStateMachineSingleton::Instance().Start() error";
		return -1;
	}
	return MySulokiHandleModuleSingleton::Instance().Start();
}
TESTMAINMODULE_API SulokiRet TestValid(SULOKI_IN const suloki::SulokiMessage& msg)
{
	return MySulokiHandleModuleSingleton::Instance().TestValid(msg);
}
TESTMAINMODULE_API SulokiRet Handle(SULOKI_IN std::auto_ptr< suloki::SulokiMessage > msgSmart, SULOKI_IN suloki::SulokiContext& context)
{
	return MySulokiHandleModuleSingleton::Instance().Handle(msgSmart, context);
}
TESTMAINMODULE_API SulokiRet Stop(void)
{
	MyAppStateMachineSingleton::Instance().Stop();
	return MySulokiHandleModuleSingleton::Instance().Stop();
}
TESTMAINMODULE_API SulokiRet Clear(void)
{
	MyAppStateMachineSingleton::Instance().Clear();
	return MySulokiHandleModuleSingleton::Instance().Clear();
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
