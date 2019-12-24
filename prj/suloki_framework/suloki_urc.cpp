// suloki_urc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "suloki.h"
#include "handler.h"

class MyAppStateMachine : public Suloki::AppStateMachine
{
public:
	MyAppStateMachine()
	{}
	virtual ~MyAppStateMachine()
	{}
	virtual Suloki::Ret Init(void)
	{
		if (Suloki::ConfigSingleton::Instance().ReadConfig() != Suloki::SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "ConfigSingleton::Instance().ReadConfig error";
			return Suloki::FAIL;
		}
		std::string strLogname;
		//if(ConfigSingleton::Instance().GetConfig(SULOKI_LOGNAME_KEY_CONFIG_BASE, strLogname) != SUCCESS)
		//	strLogname = "log";
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::SULOKI_LOGNAME_KEY_CONFIG_BASE, strLogname);
		strLogname += "_%N.log";
		std::string strLoglevel;
		//if(ConfigSingleton::Instance().GetConfig(SULOKI_LOGLEVEL_KEY_CONFIG_BASE, strLoglevel) != SUCCESS)
		//	strLoglevel = "info";
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::SULOKI_LOGLEVEL_KEY_CONFIG_BASE, strLoglevel);
		boost::log::add_file_log
			(
			boost::log::keywords::open_mode = std::ios::app,
			boost::log::keywords::target = "log",
			boost::log::keywords::file_name = strLogname,//"log_%N.log",                                        //< file name pattern >//
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
		//boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::info);
		Suloki::SetLogLevel(strLoglevel);
		boost::log::add_common_attributes();
		//
		try{
			Suloki::ConfigSingleton::Instance();
			Suloki::EventManagerSingleton::Instance();
			Suloki::UrcSingleton::Instance();
			Suloki::UrcClientHandlerSingleton::Instance();
			Suloki::UrcServerHandlerSingleton::Instance();
			Suloki::UrcTcpServerSingleton::Instance();
		}
		catch (std::exception& e){
			SULOKI_ERROR_LOG_BASEFRAMEWORK << e.what();
			return Suloki::FAIL;
		}
		catch (...){
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "some exception";
			return Suloki::FAIL;
		}
		if (Suloki::UrcSingleton::Instance().Init() != Suloki::SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "Suloki::UrcSingleton::Instance().Init error";
			return Suloki::FAIL;
		}
		if (Suloki::MaintancerSingleton::Instance().Init() != Suloki::SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "Suloki::MaintancerSingleton::Instance().Init() error";
			return Suloki::FAIL;
		}
		//
		;
		Suloki::Global::SetState(Suloki::INIT_GLOBALSTATE_BASEFRAMEWORK);
		//
		SULOKI_INFO_LOG_BASEFRAMEWORK << "suloki_framework init successfully";
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Start(void)
	{
		if (Suloki::UrcSingleton::Instance().Start() != Suloki::SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "Suloki::UrcSingleton::Instance().Start error";
			return Suloki::FAIL;
		}
		std::string strUrcport;
		//if(ConfigSingleton::Instance().GetConfig(SULOKI_URCPORT_KEY_CONFIG_BASE, strUrcport) != SUCCESS)
		//	return FAIL;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::SULOKI_URCPORT_KEY_CONFIG_BASE, strUrcport);
		Suloki::Int urcport = atoi(strUrcport.c_str());
		if (Suloki::UrcTcpServerSingleton::Instance().Start(urcport) != Suloki::SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "Suloki::UrcTcpServerSingleton::Instance().Start error";
			return Suloki::FAIL;
		}
		//if (Suloki::UrcSingleton::Instance().RegModule() != Suloki::SUCCESS)
		//{
		//	SULOKI_ERROR_LOG_BASEFRAMEWORK << "Suloki::UrcSingleton::Instance().RegModule error";
		//	return Suloki::FAIL;
		//}
		if (Suloki::MaintancerSingleton::Instance().Start() != Suloki::SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "Suloki::MaintancerSingleton::Instance().Start() error";
			return Suloki::FAIL;
		}
		//
		;
		Suloki::Global::SetState(Suloki::START_GLOBALSTATE_BASEFRAMEWORK);
		SULOKI_INFO_LOG_BASEFRAMEWORK << "suloki_framework start successfully";
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Run(void)
	{
		std::auto_ptr<suloki::SulokiMessage> msgSmart(new suloki::SulokiMessage());
		if (msgSmart.get() == NULL)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "no memory error";
			return Suloki::FAIL;
		}
		Suloki::SulokiProtoSwrap::MakeBaseMessage(*msgSmart);
		msgSmart->set_businessid(SULOKI_SYSTEM_BISINESSID_PROTO);
		msgSmart->set_messageid(SULOKI_START_MESSAGEID_SYSTEM_PROTO);
		msgSmart->set_messagetype(suloki::SulokiMessage::notice);
		msgSmart->set_sequencenumber(Suloki::IdManagerSingleton::Instance().GetFreeId());
		suloki::SulokiStartMsgBody body;
		body.set_infomation("");
		Suloki::SulokiProtoSwrap::SetBody<suloki::SulokiStartMsgBody>(*msgSmart, body);
		if (Suloki::UrcSingleton::Instance().PostToMainModule(msgSmart) != Suloki::SUCCESS)
		{
			SULOKI_ERROR_LOG_BASEFRAMEWORK << "PostToMainModule error";
			return Suloki::FAIL;
		}
#ifdef SULOKI_WINDOWS_OS_SULOKI
		//::Sleep(1000);
#else
		//usleep(1000*1000);
#endif
		//
		//return AppStateMachine::Run();
		SULOKI_INFO_LOG_BASEFRAMEWORK << "suloki_framework is running";
		std::string strMsg = "input quit to exit; input setlog-x to setup log level(x:trace,debug,info,warning,error,fatal), current level is ";
		std::string strLoglevel;
		strLoglevel.clear();
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::SULOKI_LOGLEVEL_KEY_CONFIG_BASE, strLoglevel);
		std::cout << strMsg << strLoglevel << std::endl;
		std::string str;
		while (true)
		{
			std::cin >> str;
			if (str.compare("quit") == 0)
			{
				SULOKI_INFO_LOG_BASEFRAMEWORK << "urcserver recv \"quit\"";
				break;
			}
			if (str.find("setlog-") == 0)
			{
				str = str.substr(strlen("setlog-"));
				Suloki::SetLogLevel(str);
			}
			strLoglevel.clear();
			Suloki::ConfigSingleton::Instance().GetConfig(Suloki::SULOKI_LOGLEVEL_KEY_CONFIG_BASE, strLoglevel);
			std::cout << strMsg << strLoglevel << std::endl;
		}
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Stop(void)
	{
		SULOKI_INFO_LOG_BASEFRAMEWORK << "suloki_framework will stop";
		//
		Suloki::Global::SetState(Suloki::STOP_GLOBALSTATE_BASEFRAMEWORK);
		;
		Suloki::MaintancerSingleton::Instance().Stop();
		//Suloki::UrcSingleton::Instance().UnregModule()
		;
		Suloki::UrcTcpServerSingleton::Instance().Stop();
		Suloki::UrcSingleton::Instance().Stop();
		return Suloki::SUCCESS;
	}
	virtual Suloki::Ret Clear(void)
	{
		SULOKI_INFO_LOG_BASEFRAMEWORK << "suloki_framework will clear";
		//
		Suloki::Global::SetState(Suloki::CLEAR_GLOBALSTATE_BASEFRAMEWORK);
		;
		Suloki::MaintancerSingleton::Instance().Clear();
		;
		Suloki::UrcTcpServerSingleton::Deinstance();
		Suloki::UrcClientHandlerSingleton::Deinstance();
		Suloki::UrcServerHandlerSingleton::Deinstance();
		Suloki::UrcSingleton::Instance().Clear();
		Suloki::UrcSingleton::Deinstance();
		Suloki::EventManagerSingleton::Deinstance();
		Suloki::ConfigSingleton::Deinstance();
		return Suloki::SUCCESS;
	}
private:
	MyAppStateMachine(MyAppStateMachine& ref) {}
	MyAppStateMachine& operator=(MyAppStateMachine& ref) { return *this; }
private:
};
typedef Suloki::Singleton<MyAppStateMachine> MyAppStateMachineSingleton;
int main(int argc, char* argv[])
{
	if (MyAppStateMachineSingleton::Instance().Init() != 0)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "MyAppStateMachineSingleton::Instance().Init() error";
		Suloki::Sleep(1000);
		return -1;
	}
	if (MyAppStateMachineSingleton::Instance().Start() != 0)
	{
		SULOKI_ERROR_LOG_BASEFRAMEWORK << "MyAppStateMachineSingleton::Instance().Start() error";
		Suloki::Sleep(1000);
		return -1;
	}
	MyAppStateMachineSingleton::Instance().Run();
	MyAppStateMachineSingleton::Instance().Stop();
	MyAppStateMachineSingleton::Instance().Clear();
	return 0;
}

