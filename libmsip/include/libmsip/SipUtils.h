#ifndef _SIPUTILS_H
#define _SIPUTILS_H

#include<string>

class SipUtils{
	public:

		static bool startsWith(std::string line, std::string part);

		static int findEndOfHeader(const std::string &s, int &startIndex);
		
	
};


#endif
