#ifndef _SIPMESSAGEDISPATCHER_H
#define _SIPMESSAGEDISPATCHER_H

#include<libmsip/SipSMCommand.h>

#include<list>
#include<libmutil/Mutex.h>
#include<libmutil/MemObject.h>

//#include<libmutil/minilist.h>

using namespace std;

class SipTransaction;
class SipDialog;

class SipMessageDispatcher : public SipSMCommandReceiver{
	public:
		void addTransaction(MRef<SipTransaction*> t);
		void removeTransaction(MRef<SipTransaction*> t);
		void addDialog(MRef<SipDialog*> d);
//		void removeDialog(MRef<SipDialog*> d);
		
//#ifdef DEBUG_OUTPUT
		virtual std::string getMemObjectType() {return "SipMessageDispatcher";}
//#endif
		
		virtual bool handleCommand(const SipSMCommand &cmd);

		/*
		list<MRef<SipDialog*> > *getDialogs() {//return &dialogs;
			list<MRef<SipDialog*> > *l = new list<MRef<SipDialog*> >;
			for (int i=0; i< dialogs.size(); i++)
				l->push_back(dialogs[i]);
			return l;
		
		}*/
		
		list<MRef<SipDialog*> > getDialogs() {//return &dialogs;
			list<MRef<SipDialog*> > l;
			dialogListLock.lock();
			for (int i=0; i< dialogs.size(); i++)
				l.push_back(dialogs[i]);
			dialogListLock.unlock();
			return l;
		}

	private:
//		list<MRef<SipTransaction*> > transactions;
		minilist<MRef<SipTransaction*> > transactions;
//		list<MRef<SipDialog*> > dialogs;
		minilist<MRef<SipDialog*> > dialogs;
		Mutex dialogListLock;
};

#endif
