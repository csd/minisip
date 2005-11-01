// TFRC RXSEND

#ifndef RXSEND_H
#define RXSEND_H
#include "fbinfo.h"
#include "../TFRC_NET/cdn.h"

class rxsend {
public:
	fbinfo * fbii;
	char fbdata[16];
	rxsend (fbinfo * fbip);
	~rxsend ();
	void sendfbi (void);
};


#endif




