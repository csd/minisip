/*
 Copyright (C) 2006 - 2007 Mikael Magnusson
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2006 - 2007
 *
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#ifdef SWIGPYTHON
%module(directors="1") minisip
#else
%module(directors="1") MinisipModule
#endif

%inline %{
#ifdef SWIGPYTHON
# define MINISIP_BEGIN_ALLOW_THREADS Py_BEGIN_ALLOW_THREADS
# define MINISIP_END_ALLOW_THREADS Py_END_ALLOW_THREADS
#else
# define MINISIP_BEGIN_ALLOW_THREADS
# define MINISIP_END_ALLOW_THREADS
#endif
%}

%include<stl.i>
%include<std_string.i>
%include<std_list.i>
%include<carrays.i>
%include"mtypes.i"

/* Director classes, support for virtual method callback */
%feature("director") GuiProxy;
//%feature("director") Gui;

// Disable LogEntryHandler director, problem with SwigExplicit
// LogEntryHandlerRef.cs(75,137): error CS0117: `Minisip.MinisipModulePINVOKE' does not contain a definition for `LogEntryHandlerRef_getMemObjectTypeSwigExplicitLogEntryHandler'
//%feature("director") LogEntryHandler;

%feature("director") DbgHandler;
//%feature("nodirector") Gui::handleCommand(std::string subsystem, const CommandString& command);
%feature("nodirector") GuiProxy::handleCommand(const CommandString& command);
%feature("nodirector") GuiProxy::handleCommand(std::string subsystem, const CommandString& command);
%feature("nodirector") GuiProxy::setCallback(MRef<CommandReceiver*> cb);
%feature("nodirector") GuiProxy::setConfCallback(MRef<ConfMessageRouter*> cb);

%feature("ref")   MObject "$this->incRefCount();"
%feature("unref") MObject "do{int rc = $this->decRefCount();if(rc<=0){delete $this;}}while(0);"

/*  */
%rename(MinisipMain) Minisip;

/* lock is reserved keyword in c# */
%rename(_lock) lock;
%rename(_unlock) unlock;

/* Exception conflicts with System.Exception */
%rename(MException) Exception;

/* Not exported/defined? */
%ignore SocketServer;
%ignore sipHeaderContactFactory;
%ignore sipHeaderFromFactory;
%ignore sipHeaderToFactory;
%ignore sipHeaderViaFactory;
%ignore sipSipMessageContentFactory;
%ignore readRtpPacket;

%ignore mout;
%ignore mdbg;
%ignore merr;

/* Undefined functions */
%ignore SRtpPacket::SRtpPacket(CryptoContext*, RtpPacket*);
%ignore Bell::loop();
%ignore Bell::timeout(const std::string &);
%ignore SipSoftPhoneConfiguration::installConfigFile(std::string config, std::string address="", bool overwrite=false);


// Ignore MRef<OPType>::operator== in MemObject
%ignore operator ==;
// Ignore MRef<OPType>::operator= in MemObject
%ignore operator =;
// Ignore MRef<OPType>::operator bool() in MemObject
%ignore operator bool;
// Ignore CommandString::operator []
%ignore operator [];
// Ignore SipMessage::operator << in SipMessage.h
%ignore operator <<;

#ifdef SWIGCSHARP
/* Fix problem with overriding methods in multiple inheritance */
%csmethodmodifiers Gui::run() "public virtual"
%csmethodmodifiers Gui::handleCommand(std::string, const CommandString &) "public override"
%csmethodmodifiers Gui::handleCommandResp(std::string, const CommandString &) "public override"
%csmethodmodifiers Gui::handleTimeout(std::string, const CommandString &) "public virtual"
%csmethodmodifiers ConfMessageRouter::handleCommand(std::string, const CommandString &) "public virtual"
%csmethodmodifiers ConfMessageRouter::handleCommandResp(std::string, const CommandString &) "public virtual"
%csmethodmodifiers MediaHandler::handleCommand(std::string, const CommandString &) "public virtual"
%csmethodmodifiers MediaHandler::handleCommandResp(std::string, const CommandString &) "public virtual"
#endif

%array_functions(const char *,CStrings)

namespace std {
class streambuf{
  private:
    streambuf();
};

class stringbuf: public streambuf{
};
};

void setupDefaultSignalHandling();


%{
#include<libmutil/MemObject.h>
#include<libmutil/minilist.h>
#include<libmutil/dbg.h>
#include<libmutil/MessageRouter.h>
#include<libmutil/Thread.h>

#include<libmutil/CommandString.h>

#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipCommandString.h>
#include<libmutil/SipUri.h>

//#include<libminisip/aec/aec.h>
//#include<libminisip/aec/aecfix.h>

//#include<libminisip/ipsec/MsipIpsecAPI.h>


#include<libminisip/signaling/conference/ConfMessageRouter.h>
#include<libminisip/gui/LogEntry.h>
#include<libminisip/Minisip.h>

#include<libminisip/gui/Gui.h>
#include<libminisip/gui/Bell.h>
#include<libminisip/gui/ConsoleDebugger.h>

#include<libminisip/contacts/ContactDb.h>
#include<libminisip/contacts/PhoneBook.h>

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>

//#include<libminisip/sdp/SdpHeaderA.h>
//#include<libminisip/sdp/SdpHeaderM.h>

#include<libminisip/media/MediaCommandString.h>
%}

%include<libmutil/MemObject.h>
%include<libmutil/minilist.h>
%include<libmutil/dbg.h>
%include<libmutil/MessageRouter.h>
//%include<libmutil/Thread.h>

%include<libmutil/CommandString.h>

%include<libmsip/SipDialogConfig.h>
%include<libmsip/SipCommandString.h>
%include<libmutil/SipUri.h>

%include<libminisip/gui/Gui.h>
%include<libminisip/gui/Bell.h>
%include<libminisip/gui/ConsoleDebugger.h>
//%include<libminisip/minisip/LocationDetector.h>
%include<libminisip/gui/LogEntry.h>
%include<libminisip/Minisip.h>

%include<libminisip/contacts/ContactDb.h>
%include<libminisip/contacts/PhoneBook.h>

%include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>

%include<libminisip/media/MediaCommandString.h>

// Smart pointers
%define REF(type)
%template(type ## Ref) MRef<type*>;
%enddef

//%template(Ref) MRef<*>;

//REF(AudioCodec)
//REF(RtpReceiver);
//REF(SdpHeaderA);
//REF(SipTimers);
//REF(SRtpPacket);
//REF(Codec);
//REF(CodecState);
//REF(MediaStreamReceiver);
//REF(MediaStreamSender);
//REF(RtpPacket);
//REF(SdpHeaderM);
//REF(SdpPacket);

REF(ConfBackend);
REF(ConfMessageRouter);
REF(ContactDb);
REF(ContactEntry);
//REF(Gui);
REF(Gui);
//REF(IpProvider);
REF(LogEntryHandler);
REF(LogEntry);
//REF(Media);
//REF(MediaHandler);
REF(Minisip);
//REF(PhoneBookIo);
REF(PhoneBookPerson);
REF(PhoneBook);
//REF(Session);
REF(Sip);
REF(SipIdentity);
REF(SipRegistrar);
REF(SipSoftPhoneConfiguration);
REF(CommandReceiver);
REF(SipStackConfig);

//REF(SipState) State<SipSMCommand, std::string>;
//REF(SipState);

%template(MStrings) minilist<std::string>;
//%template(ConfMembers) minilist<ConfMember>;

%define LIST(type)
%template(type ## List) std::list< type >;
%enddef

//LIST(String);
LIST(SipUri);

//SWIG_STD_LIST_SPECIALIZE(SipIdentityRef, MRef<SipIdentity*>)
//%template(SipIdentityList) std::list< MRef<SipIdentity*> >;

// List of smart pointers
#ifdef SWIGCSHARP

%define REFLIST(type)
SWIG_STD_LIST_SPECIALIZE(type ## Ref, MRef<type*>)
%template(type ## List) std::list< MRef<type*> >;
%enddef

#else

%define REFLIST(type)
%template(type ## List) std::list< MRef<type*> >;
%enddef

#endif

REFLIST(SipIdentity)
REFLIST(PhoneBook)
REFLIST(ContactEntry)
REFLIST(PhoneBookPerson)

//typedef MRef<ContactEntry*> ContactEntryRef;
//LIST(Contact)

%extend CommandString {
   void setKey(std::string key, std::string value) {
	(*self)[key] = value;
   }
}

%inline %{
  char **CreateEmptyArgs() {
       return NULL;
   }
%}

%extend Dbg {
  static Dbg *getOut() {
    return &mout;
  }

  static Dbg *getDbg() {
    return &mdbg;
  }

  static Dbg *getErr() {
    return &merr;
  }
}

%inline %{
  std::streambuf *setOutputBuf(std::streambuf *buf) {
    return std::cout.rdbuf(buf);
  }

  std::streambuf *setErrorBuf(std::streambuf *buf) {
    return std::cerr.rdbuf(buf);
  }
%}


//%rename(GuiProxy::handleCommand) GuiProxy::safe_handleCommand;


%{

struct Gui_handleCommand{
  bool occupied;
  const CommandString * command;
};

%}

%inline %{

class GuiProxy : public Gui{
  public:
    void handleCommand(const CommandString &command);
    CommandString waitCommand();
    void stop();

 protected:
  GuiProxy();

  private:
    Gui *proxy;
    Mutex mutex;
    CondVar cond;
    Gui_handleCommand args;
    bool stopping;
};

%}

%{

GuiProxy::GuiProxy(){
  args.occupied = false;
  stopping = false;
}

void GuiProxy::stop(){
  mutex.lock();
  stopping = true;
  cond.broadcast();
  mutex.unlock();
}

void GuiProxy::handleCommand(const CommandString &command){
//  cerr << "handleCommand enter" << endl;
  mutex.lock();
  while( args.occupied && !stopping){
    cond.wait( mutex );
  }

  if( stopping ){
//    cerr << "handleCommand stopping" << endl;
    mutex.unlock();
    return;
  }

//  cerr << "handleCommand " << command.getString() << endl;
  args.command = &command;
  args.occupied = true;
  cond.broadcast();

  while( args.occupied ){
    cond.wait( mutex );
  }

  mutex.unlock();
//  cerr << "handleCommand exit" << endl;
}

CommandString GuiProxy::waitCommand(){
//  cerr << "waitCommand enter" << endl;
  mutex.lock();
  while( !args.occupied && !stopping ){
    MINISIP_BEGIN_ALLOW_THREADS
    cond.wait( mutex );
    MINISIP_END_ALLOW_THREADS
  }

  if( stopping ){
//    cerr << "waitCommand stopping" << endl;
    mutex.unlock();
    return CommandString("gui", "stop");
  }

//  cerr << "waitCommand " << args.command->getString() << endl;
  CommandString res(*args.command);
  args.occupied = false;
  cond.broadcast();
  mutex.unlock();
//  cerr << "waitCommand exit" << endl;

  return res;
}

%}
