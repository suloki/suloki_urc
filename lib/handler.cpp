#include "handler.h"

namespace Suloki
{

UrcClientHandler::UrcClientHandler()
{}
UrcClientHandler::~UrcClientHandler()
{}

UrcServerHandler::UrcServerHandler()
{}
UrcServerHandler::~UrcServerHandler()
{}
std::string UrcServerHandler::CalKey(suloki::SulokiMessage& protocolReq)
{
	std::stringstream strStream;
	strStream << protocolReq.businessid() << "_" << protocolReq.messageid() << "_" << protocolReq.messagetype();
	return strStream.str();
}

}