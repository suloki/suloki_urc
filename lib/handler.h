#ifndef _SULOKI_HANDLER_H_
#define  _SULOKI_HANDLER_H_

#include "suloki.h"

namespace Suloki
{

class UrcClientHandler : public Base
{
public:
	UrcClientHandler();
	virtual ~UrcClientHandler();
private:
	UrcClientHandler(UrcClientHandler& ref){}
	UrcClientHandler& operator=(UrcClientHandler& ref){ return *this; }
protected:
};
typedef Singleton<UrcClientHandler> UrcClientHandlerSingleton;

class UrcServerHandler : public Dispatcher<suloki::SulokiMessage, suloki::SulokiContext>
{
public:
	UrcServerHandler();
	virtual ~UrcServerHandler();
protected:
	virtual std::string CalKey(suloki::SulokiMessage& protocolReq);
private:
	UrcServerHandler(UrcServerHandler& ref){}
	UrcServerHandler& operator=(UrcServerHandler& ref){ return *this; }
protected:
};
typedef Singleton<UrcServerHandler> UrcServerHandlerSingleton;

}

#endif