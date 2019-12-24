// urchandler.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "urchandler.h"
#include "mysql.h"

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


class MyAppStateMachine : public Suloki::AppStateMachine
{
public:
	MyAppStateMachine()
	{}
	virtual ~MyAppStateMachine()
	{}
	virtual Suloki::Ret Init(void)
	{
		Suloki::ConfigSingleton::Instance().SetConfig(Suloki::SULOKI_LOGNAME_KEY_CONFIG_BASE, "urchandler");
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
		SULOKI_INFO_LOG_BASEFRAMEWORK << "urchandler init successfully";
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
		Suloki::Global::SetState(Suloki::START_GLOBALSTATE_BASEFRAMEWORK);
		SULOKI_INFO_LOG_BASEFRAMEWORK << "urchandler start successfully";
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Run(void)
	{
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Stop(void)
	{
		SULOKI_INFO_LOG_BASEFRAMEWORK << "urchandler will stop";
		//
		Suloki::Global::SetState(Suloki::STOP_GLOBALSTATE_BASEFRAMEWORK);
		;
		//
		;
		return AppStateMachine::Stop();
	}
	virtual Suloki::Ret Clear(void)
	{
		SULOKI_INFO_LOG_BASEFRAMEWORK << "urchandler will clear";
		//
		Suloki::Global::SetState(Suloki::CLEAR_GLOBALSTATE_BASEFRAMEWORK);
		;

		;
		return AppStateMachine::Clear();
	}
private:
	MyAppStateMachine(MyAppStateMachine& ref) {}
	MyAppStateMachine& operator=(MyAppStateMachine& ref) { return *this; }
private:
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
			sStream << SULOKI_URC_BISINESSID_PROTO << "_" << SULOKI_SQL_MESSAGEID_URC_PROTO;
			m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_Urc_Sql);
		}
		{
			std::stringstream sStream;
			sStream << SULOKI_URC_BISINESSID_PROTO << "_" << SULOKI_ADD_MESSAGEID_URC_PROTO;
			m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_Urc_Add);
		}
		{
			std::stringstream sStream;
			sStream << SULOKI_URC_BISINESSID_PROTO << "_" << SULOKI_DEL_MESSAGEID_URC_PROTO;
			m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_Urc_Del);
		}
		{
			std::stringstream sStream;
			sStream << SULOKI_URC_BISINESSID_PROTO << "_" << SULOKI_UPDATE_MESSAGEID_URC_PROTO;
			m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_Urc_Update);
		}
		{
			std::stringstream sStream;
			sStream << SULOKI_URC_BISINESSID_PROTO << "_" << SULOKI_GET_MESSAGEID_URC_PROTO;
			m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_Urc_Get);
		}
		{
			std::stringstream sStream;
			sStream << SULOKI_URC_BISINESSID_PROTO << "_" << SULOKI_OBTAINSTATES_MESSAGEID_URC_PROTO;
			m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_Urc_ObtainStates);
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
		return Suloki::FAIL;
	}
protected:
	//Suloki::Ret Handler_SYSTEM_TEST1(suloki::SulokiMessage& protocolReq, suloki::SulokiContext& paraIn, suloki::SulokiMessage& protocolRes, suloki::SulokiContext& paraOut)
	//{
	//	std::cout << "Handler_SYSTEM_TEST1" << std::endl;
	//	return Suloki::SUCCESS;
	//}
	Suloki::Ret Handler_System_Start(suloki::SulokiMessage& msg, suloki::SulokiContext& context)
	{
		//std::cout << "recv system start msg" << std::endl;
		return Suloki::SUCCESS;
	}
	Suloki::Ret Handler_Urc_Sql(suloki::SulokiMessage& msg, suloki::SulokiContext& context)
	{
		if (!context.has_b())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "context has not b field";
			return Suloki::FAIL;
		}
		if (context.b() && msg.messagetype() == suloki::SulokiMessage::request)
		{
			suloki::SulokiSqlReqUrcMsgBody body;
			Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiSqlReqUrcMsgBody>(msg, body);
			//
			suloki::SulokiMessage res;
			Suloki::SulokiProtoSwrap::MakeResMessage(msg, res);
			suloki::SulokiSqlResUrcMsgBody resBody;
			//?????
			//exe sql
			//????? MYSQL is thread safed?
			{
				MYSQL mysql;
				mysql_init(&mysql);
				if (!mysql_real_connect(&mysql, "127.0.0.1", "root", "root", "sesystem", 3306, NULL, 0))
				{
					std::cout << "mysql_real_connect error" << std::endl;
					return -1;
				}
				if (mysql_query(&mysql, body.urcsql().c_str()) != 0)
				{
					mysql_close(&mysql);
					std::cout << "mysql_query error" << std::endl;
					return -1;
				}
				MYSQL_RES* pRes = NULL;
				pRes = mysql_store_result(&mysql);
				if (pRes == NULL)
				{
					mysql_close(&mysql);
					std::cout << "mysql_store_result error" << std::endl;
					return -1;
				}
				MYSQL_ROW row;
				while ((row = mysql_fetch_row(pRes)) != NULL)
				{
					unsigned int fields = mysql_num_fields(pRes);
					for (unsigned int field = 0; field < fields; field++)
					{
						//std::cout << row[field] << std::endl;
						resBody.add_sqlres()->add_sqlresitem(row[field], strlen(row[field]));
					}
				}
				mysql_free_result(pRes);
				mysql_close(&mysql);
			}
			//
			//suloki::SulokiMessage res;
			//SulokiProtoSwrap::MakeBaseMessage(res);
			//res.set_businessid(msg.businessid());
			//res.set_messageid(msg.messageid());
			//res.set_messagetype(suloki::SulokiMessage::response);
			//res.set_sequencenumber(msg.sequencenumber());
			//res.set_errorcode(Suloki::SUCCESS);
			//for (Suloki::Int i = 0; i < msg.routers_size(); i++)
			//	res.add_routers(msg.routers(i));
			//SulokiProtoSwrap::MakeResMessage(msg, res);
			//suloki::SulokiSqlResUrcMsgBody resBody;
			//std::string strResMsg;
			//if (SulokiProtoSwrap::EncodeProtocol<suloki::SulokiSqlResUrcMsgBody>(res, resBody, strResMsg) != SULOKI_SUCCESS)
			//{
			//	std::cout << "EncodeProtocol error" << std::endl;
			//	return Suloki::FAIL;
			//}
			res.set_errorcode(Suloki::SUCCESS);
			if (Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiSqlResUrcMsgBody>(res, resBody) != Suloki::SUCCESS)
			{
				std::cout << "SetBody error" << std::endl;
				return Suloki::FAIL;
			}
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
	Suloki::Ret Handler_Urc_Add(suloki::SulokiMessage& msg, suloki::SulokiContext& context)
	{
		if (!context.has_b())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "context has not b field";
			return Suloki::FAIL;
		}
		if (context.b() && msg.messagetype() == suloki::SulokiMessage::request)
		{
			suloki::SulokiOperatorUrcMsgBody body;
			Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiOperatorUrcMsgBody>(msg, body);
			//
			suloki::SulokiMessage res;
			Suloki::SulokiProtoSwrap::MakeResMessage(msg, res);
			suloki::SulokiOperatorUrcMsgBody resBody;
			//
			if (!msg.has_urckey())
				return Suloki::FAIL;
			std::string urName = msg.urckey();
			urName.erase(0, Suloki::SULOKI_REMOTED_RESOURCE_URC_BASE.length());
			urName.insert(0, Suloki::SULOKI_LOCAL_RESOURCE_URC_BASE);
			//std::cout << urName << std::endl;
			if (!msg.has_urcval() || !msg.has_dir())
				return Suloki::FAIL;
			std::string& strVal = (std::string&)msg.urcval();
			Suloki::Ret ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->AddNoSqlData(urName, strVal, msg.dir());
			res.set_errorcode(ret);
			//
			Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(res, resBody);
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
	Suloki::Ret Handler_Urc_Del(suloki::SulokiMessage& msg, suloki::SulokiContext& context)
	{
		if (!context.has_b())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "context has not b field";
			return Suloki::FAIL;
		}
		if (context.b() && msg.messagetype() == suloki::SulokiMessage::request)
		{
			suloki::SulokiOperatorUrcMsgBody body;
			Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiOperatorUrcMsgBody>(msg, body);
			//
			suloki::SulokiMessage res;
			Suloki::SulokiProtoSwrap::MakeResMessage(msg, res);
			suloki::SulokiOperatorUrcMsgBody resBody;
			//
			if (!msg.has_urckey())
				return Suloki::FAIL;
			std::string urName = msg.urckey();
			urName.erase(0, Suloki::SULOKI_REMOTED_RESOURCE_URC_BASE.length());
			urName.insert(0, Suloki::SULOKI_LOCAL_RESOURCE_URC_BASE);
			//std::cout << urName << std::endl;
			std::string strVal;
			Suloki::Ret ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->DelNoSqlData(urName, strVal);
			res.set_errorcode(ret);
			res.set_urcval(strVal);
			//
			Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(res, resBody);
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
	Suloki::Ret Handler_Urc_Update(suloki::SulokiMessage& msg, suloki::SulokiContext& context)
	{
		if (!context.has_b())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "context has not b field";
			return Suloki::FAIL;
		}
		if (context.b() && msg.messagetype() == suloki::SulokiMessage::request)
		{
			suloki::SulokiOperatorUrcMsgBody body;
			Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiOperatorUrcMsgBody>(msg, body);
			//
			suloki::SulokiMessage res;
			Suloki::SulokiProtoSwrap::MakeResMessage(msg, res);
			suloki::SulokiOperatorUrcMsgBody resBody;
			//
			if (!msg.has_urckey())
				return Suloki::FAIL;
			std::string urName = msg.urckey();
			urName.erase(0, Suloki::SULOKI_REMOTED_RESOURCE_URC_BASE.length());
			urName.insert(0, Suloki::SULOKI_LOCAL_RESOURCE_URC_BASE);
			//std::cout << urName << std::endl;
			std::string strVal = msg.urcval();
			Suloki::Ret ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->UpdateNoSqlData(urName, strVal);
			res.set_errorcode(ret);
			res.set_urcval(strVal);
			//
			Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(res, resBody);
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
			//std::cout << "update ur name:" << urName << std::endl;
			return Suloki::SUCCESS;
		}
		return Suloki::FAIL;
	}
	Suloki::Ret Handler_Urc_Get(suloki::SulokiMessage& msg, suloki::SulokiContext& context)
	{
		if (!context.has_b())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "context has not b field";
			return Suloki::FAIL;
		}
		if (context.b() && msg.messagetype() == suloki::SulokiMessage::request)
		{
			suloki::SulokiOperatorUrcMsgBody body;
			Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiOperatorUrcMsgBody>(msg, body);
			//
			suloki::SulokiMessage res;
			Suloki::SulokiProtoSwrap::MakeResMessage(msg, res);
			suloki::SulokiOperatorUrcMsgBody resBody;
			//
			if (!msg.has_urckey())
				return Suloki::FAIL;
			std::string urName = msg.urckey();
			urName.erase(0, Suloki::SULOKI_REMOTED_RESOURCE_URC_BASE.length());
			urName.insert(0, Suloki::SULOKI_LOCAL_RESOURCE_URC_BASE);
			//std::cout << urName << std::endl;
			std::string strVal;
			Suloki::Ret ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetNoSqlData(urName, strVal);
			res.set_errorcode(ret);
			res.set_urcval(strVal);
			//std::cout << "get nosql val:" << strVal << std::endl;
			//
			Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiOperatorUrcMsgBody>(res, resBody);
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
	Suloki::Ret Handler_Urc_ObtainStates(suloki::SulokiMessage& msg, suloki::SulokiContext& context)
	{
		if (!context.has_b())
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "context has not b field";
			return Suloki::FAIL;
		}
		if (context.b() && msg.messagetype() == suloki::SulokiMessage::request)
		{
			suloki::SulokiOperatorUrcMsgBody body;
			Suloki::SulokiProtoSwrap::GetBody<suloki::SulokiOperatorUrcMsgBody>(msg, body);
			//
			suloki::SulokiMessage res;
			Suloki::SulokiProtoSwrap::MakeResMessage(msg, res);
			suloki::SulokiGroupStateUrcMsgBody resBody;
			//
			if (!msg.has_urckey())
				return Suloki::FAIL;
			std::string urName = msg.urckey();
			urName.erase(0, Suloki::SULOKI_REMOTED_RESOURCE_URC_BASE.length());
			urName.insert(0, Suloki::SULOKI_LOCAL_RESOURCE_URC_BASE);
			//std::cout << urName << std::endl;
			std::vector<std::string> nameVector;
			Suloki::Ret ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetUrDirectory(urName, nameVector);
			if (ret != Suloki::SUCCESS)
			{
				std::cout << "GetSulokiUrcModuleInterface error" << std::endl;
				return Suloki::FAIL;
			}
			for (std::vector<std::string>::iterator iter = nameVector.begin(); iter != nameVector.end();iter++)
			{
				std::string strVal;
				ret = MySulokiUrcModuleInterface::GetSulokiUrcModuleInterface()->GetNoSqlData(*iter, strVal);
				if (ret != Suloki::SUCCESS)
				{
					std::cout << "GetNoSqlData error" << std::endl;
					return Suloki::FAIL;
				}
				suloki::SulokiServiceStateUrcMsgBody state;
				if (!state.ParseFromString(strVal))
				{
					std::cout << "ParseFromString error" << std::endl;
					return Suloki::FAIL;
				}
				std::string urName = *iter;
				urName.erase(0, Suloki::SULOKI_LOCAL_RESOURCE_URC_BASE.length());
				urName.insert(0, Suloki::SULOKI_REMOTED_RESOURCE_URC_BASE);
				suloki::SulokiServiceStateUrcMsgBody* pSulokiServiceStateUrcMsgBody = resBody.add_stateres();
				if (pSulokiServiceStateUrcMsgBody == NULL)
				{
					std::cout << "no memory error" << std::endl;
					return Suloki::FAIL;
				}
				pSulokiServiceStateUrcMsgBody->set_ip(state.ip());
				pSulokiServiceStateUrcMsgBody->set_port(state.port());
				pSulokiServiceStateUrcMsgBody->set_busydegree(state.busydegree());
				pSulokiServiceStateUrcMsgBody->set_urname(urName);
			}
			res.set_errorcode(ret);
			//res.set_urcval(strVal);
			//std::cout << "get nosql val:" << strVal << std::endl;
			//
			Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiGroupStateUrcMsgBody>(res, resBody);
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
	//SulokiUrcModuleInterface* m_pSulokiUrcModuleInterface;
	//std::auto_ptr<SulokiUrcModuleInterface> m_sulokiUrcModuleInterfaceSmart;
};
typedef Suloki::Singleton<MySulokiHandleModule> MySulokiHandleModuleSingleton;

URCHANDLER_API SulokiRet Init(SULOKI_IN SulokiUrcModuleInterface* pSulokiUrcModuleInterface, SULOKI_IN std::string groupName, SULOKI_IN std::string serviceName, SULOKI_IN std::string strModuleName, SULOKI_IN std::string strConfig)
{
	if (MyAppStateMachineSingleton::Instance().Init() != 0)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "MyAppStateMachineSingleton::Instance().Init() error";
		return -1;
	}
	return MySulokiHandleModuleSingleton::Instance().Init(pSulokiUrcModuleInterface, groupName, serviceName, strModuleName, strConfig);
}
URCHANDLER_API SulokiRet Start(void)
{
	if (MyAppStateMachineSingleton::Instance().Start() != 0)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "MyAppStateMachineSingleton::Instance().Start() error";
		return -1;
	}
	return MySulokiHandleModuleSingleton::Instance().Start();
}
URCHANDLER_API SulokiRet TestValid(SULOKI_IN const suloki::SulokiMessage& msg)
{
	return MySulokiHandleModuleSingleton::Instance().TestValid(msg);
}
URCHANDLER_API SulokiRet Handle(SULOKI_IN std::auto_ptr< suloki::SulokiMessage > msgSmart, SULOKI_IN suloki::SulokiContext& context)
{
	return MySulokiHandleModuleSingleton::Instance().Handle(msgSmart, context);
}
URCHANDLER_API SulokiRet Stop(void)
{
	MyAppStateMachineSingleton::Instance().Stop();
	return MySulokiHandleModuleSingleton::Instance().Stop();
}
URCHANDLER_API SulokiRet Clear(void)
{
	MyAppStateMachineSingleton::Instance().Clear();
	return MySulokiHandleModuleSingleton::Instance().Clear();
}

/*
// This is an example of an exported variable
URCHANDLER_API int nurchandler=0;

// This is an example of an exported function.
URCHANDLER_API int fnurchandler(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see urchandler.h for the class definition
Curchandler::Curchandler()
{
	return;
}
*/