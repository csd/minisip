#include<config.h>

#include<libmutil/MessageRouter.h>
#include<string>

using namespace std;

/* 
 * The MessageRouter internal state is a collection of 
 * (subsystem name, CommandReceiver object reference)
 * tuples. Such a tuple is called a "Route".
 *
 * Todo: Plugin slots:
 *  - Before a message is routed
 *  - After the message has been delivered
 *  This can be used by a logging/monitoring plugin.
 *  
 */


SubsystemNotFoundException::SubsystemNotFoundException(const char *m):Exception(m){

}

/**
 * Tuple containing subsystem name and reference to the subsystem interface object.
 */
class Route{
	public:
		string ssname;
		MRef<CommandReceiver*> dest;
};

class MessageRouterInternal{
	public:
		list<Route> subsystems;
};


MessageRouter::MessageRouter(){
	internal = new MessageRouterInternal;
}

MessageRouter::MessageRouter(const MessageRouter &mr){
	internal = new MessageRouterInternal(*mr.internal);
}

MessageRouter::~MessageRouter(){
	delete internal;
}

bool MessageRouter::hasSubsystem(string name){
	list<Route>::iterator i;
	for (i=internal->subsystems.begin(); i!=internal->subsystems.end(); i++){
		if ((*i).ssname==name){
			return true;
		}
	}
	return false;
}

bool MessageRouter::addSubsystem(string name, MRef<CommandReceiver*> rcvr){
	if (hasSubsystem(name)){
		return false;
	}else{
		Route r;
		r.ssname = name;
		r.dest = rcvr;
		internal->subsystems.push_back(r);
		return true;
	}
}

void MessageRouter::handleCommand(string subsystem, const CommandString &cmd){
#ifdef DEBUG_OUTPUT
	mdbg("messagerouter") << "MessageRouter:  To:"<<subsystem<<" Command:"<<cmd.getString()<<endl;
#endif
	list<Route>::iterator i;
	for (i=internal->subsystems.begin(); i!=internal->subsystems.end(); i++){
		if ((*i).ssname==subsystem){
			(*i).dest->handleCommand(subsystem, cmd);
			return;
		}
	}
	throw new SubsystemNotFoundException(subsystem.c_str());
}

CommandString MessageRouter::handleCommandResp(string subsystem, const CommandString &cmd){
#ifdef DEBUG_OUTPUT
	mdbg("messagerouter") << "MessageRouter:  To(request):"<<subsystem<<" Command:"<< cmd.getString()<< endl;
#endif
	list<Route>::iterator i;
	for (i=internal->subsystems.begin(); i!=internal->subsystems.end(); i++){
		if ((*i).ssname==subsystem){
			CommandString ret =(*i).dest->handleCommandResp(subsystem, cmd);
#ifdef DEBUG_OUTPUT
			mdbg("messagerouter") << "MessageRouter:  Response from:"<<subsystem<<" Command:"<<ret.getString()<<endl;
#endif
			return ret;
		}
	}
	throw new SubsystemNotFoundException(subsystem.c_str());
	CommandString dummy("","");
	return dummy; //not reached - avoids compiler warning
}

void MessageRouter::clear(){
	internal->subsystems.clear();
}

