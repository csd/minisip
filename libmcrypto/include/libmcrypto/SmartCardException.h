#include <libmutil/Exception.h>

class SmartCardException : public Exception{
public:
	SmartCardException(const char * message);
	virtual ~SmartCardException() throw(){};


};

