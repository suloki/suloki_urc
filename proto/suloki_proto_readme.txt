message SulokiMessage
{
	required string protocolVersion = 1;//protocol version
	required sint32 businessId = 2;//it is a good idea that every moduleIo use a special businessId
	required sint32 messageId = 3;//like a command
	enum MessageType{
		request = 0;
		response = 1;//match with request,and have same sequenceNumber field
		notice = 2;
	}
	required MessageType messageType = 4;
	required sint64 sequenceNumber = 5;
	optional uint64 handle = 6;//maybe socket or window handle,and so on
	optional string token = 7;
	optional string dateTime = 8;
	optional string optionSign = 9;
	optional string optionCompress = 10;
	optional string optionEncrypt = 11;
	optional bytes  messageBody = 12;
	repeated string routers = 13;//maybe record routers path,used by suloki current
	optional uint32 errorCode = 14;//only for response
	optional bytes  errorInfo = 15;//only for response
	repeated bytes  reserved = 16;
}

businessId(<1024, mean suloki business;user can use >= 1024 businessid):
businessId == 0, mean system business
businessId == 1, mean urc business
businessId == 2，mean isolation business,only use by isolation framework

businessId == 0, system business:
messageId == 0, mean test msg, notice no body
messageId == 1, mean heartbeat, notice no body
messageId == 2, mean init info to module, req body SulokiInitMsgBody,res body none
messageId == 3, mean start info to module, req body SulokiStartMsgBody,res body none
messageId == 4, mean stop info to module, req body SulokiStopMsgBody,res body none
messageId == 5, mean clear info to module, req body SulokiClearMsgBody,res body none
messageId == 6, mean timeout，used suloki inside

businessId == 1, urc business:
messageId == 0, mean add urc, req body SulokiOperatorUrcMsgBody, res body SulokiOperatorUrcMsgBody
messageId == 1, mean del urc, req body SulokiOperatorUrcMsgBody, res body SulokiOperatorUrcMsgBody
messageId == 2, mean update urc, req body SulokiOperatorUrcMsgBody, res body SulokiOperatorUrcMsgBody
messageId == 3, mean get urc, req body SulokiOperatorUrcMsgBody, res body SulokiOperatorUrcMsgBody

messageId == 1024, mean sql urc, req body SulokiSqlReqUrcMsgBody, res body SulokiSqlResUrcMsgBody

/////////////////////////////////////////////////////

businessId == 0, system business:
messageId == 0, mean test msg, notice no body
messageId == 1, mean heartbeat, notice no body
messageId == 2, mean init info to module, req body SulokiInitMsgBody,res body none
messageId == 3, mean start info to module, req body SulokiStartMsgBody,res body none
messageId == 4, mean stop info to module, req body SulokiStopMsgBody,res body none
messageId == 5, mean clear info to module, req body SulokiClearMsgBody,res body none
messageId == 6, mean suloki_esf start success
messageId == 7, mean timeout，used suloki inside
messageId == 8, mean start test progress，包括功能和性能

businessId == 1, event business:
messageId == 0, mean reg recv event to framework, req body SulokiRegRecvEventBody,res none
messageId == 1, mean unreg recv event to framework, req body SulokiRegRecvEventBody,res none

businessId == 3，timer business
messageId == 0, mean set timer, req body SulokiSetTimerBody,res body none
messageId == 1, mean cancel timer, req body SulokiSetTimerBody,res body none
messageId == 2, mean timer notice, req body SulokiSetTimerBody,res body none

businessId == 4，cache business
messageId == 0, mean get cache, req body SulokiCacheBody,res body SulokiCacheBody
messageId == 1, mean set cache, req body SulokiCacheBody,res body none
messageId == 2, mean del cache, req body SulokiCacheBody,res body none

businessId == 5，urc business

message SulokiInitMsgBody
{
	required string infomation= 1;
}
message SulokiStartMsgBody
{
	required string infomation= 1;
}
message SulokiStopMsgBody
{
	required string infomation= 1;
}
message SulokiClearMsgBody
{
	required string infomation= 1;
}

enum EventMode{
	ONETIME = 0;
	REPEATED = 1;
}
enum EventRight{
	EXCLUSIVE = 0;
	SHARED = 1;
}
enum EventPolicy{
	BROADCAST = 0;
	ROTATION = 1;
	SEQUENCE = 2;//only EXCLUSIVE
}
message SulokiRegRecvEventBody
{
	//patternKey is a module name event from which or a virtual key knowned by sender and recver
	required string patternKey = 1;
	required sint32 businessId = 2;
	required sint32 messageId = 3;
	optional sint64 sequenceNumber = 4;
	required EventMode mode = 5;
	optional sint32 timeout = 6;
	required EventRight right = 7;
	optional EventPolicy policy = 8;
	required string callback = 9;
	optional uint64 userdata = 10;
}
message SulokiSetTimerBody
{
	required string patternKey = 1;
	required EventMode mode = 2;
	optional sint32 timeout = 3;
}
message SulokiCacheBody
{
	required string key = 1;
	optional bytes  val = 2;
}

