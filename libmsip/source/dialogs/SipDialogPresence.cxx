
#include<config.h>


#include<libmsip/SipDialogPresence.h>


void SipDialogPresence::setUpStateMachine(){


}

SipDialogPresence::SipDialogPresence(MRef<SipDialogContainer*> dContainer, SipDialogConfig &callConf)
	: SipDialog("SipDialogPresence", dContainer, callConf)
{
	setUpStateMachine();	
}


