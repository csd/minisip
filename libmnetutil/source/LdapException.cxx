#include <libmnetutil/LdapException.h>

/**
 * @author	Mikael Svensson
 * @author	Erik Eliasson <eliasson@it.kth.se>
 * @author	Johan Bilien <jobi@via.ecp.fr>
 */

LdapException::LdapException() {
	msg = "LdapException";
};

LdapException::LdapException(std::string msg) {
	this->msg = "LdapException: " + msg;
};

const char* LdapException::what()const throw(){
	return msg.c_str();
}

LdapAttributeNotFoundException::LdapAttributeNotFoundException(std::string message) {
	msg = "LdapAttributeNotFoundException: Could not find attribute " + message;
}
LdapNotConnectedException::LdapNotConnectedException() {
	msg = "LdapNotConnectedException: Not connected to server";
}
LdapUnsupportedException::LdapUnsupportedException(std::string feature) {
	msg = "LdapUnsupportedException: Feature " + feature + " not supported";
}
