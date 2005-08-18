#ifndef SIP_DIALOG_SECURITY_CONFIG
#define SIP_DIALOG_SECURITY_CONFIG

#define KEY_MGMT_METHOD_NULL            0x00
#define KEY_MGMT_METHOD_MIKEY           0x10
#define KEY_MGMT_METHOD_MIKEY_DH        0x11
#define KEY_MGMT_METHOD_MIKEY_PSK       0x12
#define KEY_MGMT_METHOD_MIKEY_PK        0x13


class XMLFileParser;
class certificate_chain;
class ca_db;
class SipIdentity;
class ConfBackend;

class SipDialogSecurityConfig{
	public:
		SipDialogSecurityConfig();

		void useIdentity( MRef<SipIdentity *> );

		bool secured;
		int ka_type;

		bool use_srtp;		
		bool use_ipsec;
		
		MRef<certificate_chain *> cert;
		MRef<ca_db *> cert_db;
		bool psk_enabled;
		unsigned char * psk;
		unsigned int psk_length;
		bool dh_enabled;
		bool check_cert;

		void save( MRef<ConfBackend *> backend );
		void load( MRef<ConfBackend *> backend );
};


#endif
