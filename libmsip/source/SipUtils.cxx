#include<libmsip/SipUtils.h>
#include<ctype.h>
#include<stdint.h>


bool SipUtils::startsWith(std::string line, std::string part){
	if (part.length() > line.length())
		return false;
	for (uint32_t i=0; i< part.length(); i++){
		if ( toupper(part[i]) != toupper(line[i]) )
			return false;
	}
	return true;
}

