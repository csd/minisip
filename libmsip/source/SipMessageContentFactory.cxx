
#include<libmsip/SipMessageContentFactory.h>

#include<config.h>


void SMCFCollection::addFactory(string contentType, SipMessageContentFactoryFuncPtr f){
	factories[contentType] = f;
}

SipMessageContentFactoryFuncPtr SMCFCollection::getFactory(const string contentType){
	return factories[contentType];
}


