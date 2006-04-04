
#include<libminisip/sip/SipDialogSecurityConfig.h>

#include<libminisip/configbackend/ConfBackend.h>
#include<libmutil/cert.h>
#include<libmutil/XMLParser.h>
#include<libmutil/itoa.h>
#include<libmsip/SipDialogConfig.h>


SipDialogSecurityConfig::SipDialogSecurityConfig():
	secured(false),
	ka_type(0),
	use_srtp(false),		
	use_ipsec(false),
	cert(NULL),
	cert_db(NULL),
	psk_enabled(false),
	psk(NULL),
	psk_length(0),
	dh_enabled(false),
	check_cert(false)
{
}

void SipDialogSecurityConfig::save( MRef<ConfBackend *> backend ){

        backend->save("secured", secured?string("yes"): string("no"));

	backend->save("use_srtp", use_srtp?string("yes"): string("no"));
	backend->save("use_ipsec", use_ipsec?string("yes"): string("no"));

        backend->save("psk_enabled", psk_enabled?string("yes"): string("no"));
        backend->save("dh_enabled", dh_enabled?string("yes"): string("no"));

        char * pskString = new char[psk_length+1];
        memcpy( pskString, psk, psk_length );
        pskString[psk_length] = '\0';
        backend->save("psk", pskString);
        delete [] pskString;

        string kaTypeString;
        switch( ka_type ){
                case KEY_MGMT_METHOD_MIKEY_DH:
                        kaTypeString = "dh";
                        break;
                case KEY_MGMT_METHOD_MIKEY_PSK:
                        kaTypeString = "psk";
                        break;
                case KEY_MGMT_METHOD_MIKEY_PK:
                        kaTypeString = "pk";
        }

        backend->save("ka_type", kaTypeString);
 
	/***********************************************************
         * Certificate settings
         ***********************************************************/

        /* Update the certificate part of the configuration file */
        cert->lock();
        cert->init_index();
        MRef<certificate *> certItem = cert->get_next();

        /* The first element is the personal certificate, the next ones
         * are saved as certificate_chain */
        if( !certItem.isNull() ){
                backend->save("certificate",certItem->get_file());
                backend->save("private_key",certItem->get_pk_file());
                certItem = cert->get_next();
        }

        uint32_t i = 0;

 	while( !certItem.isNull() ){
                backend->save("certificate_chain["+itoa(i)+"]",
                                certItem->get_file() );
                i++;
                certItem = cert->get_next();
        }

        cert->unlock();

 	/* CA database saved in the config file */
        uint32_t iFile = 0;
        uint32_t iDir  = 0;
        cert_db->lock();
        cert_db->init_index();
        ca_db_item * caDbItem = cert_db->get_next();

        while( caDbItem != NULL ){
                switch( caDbItem->type ){
                        case CERT_DB_ITEM_TYPE_FILE:
                                backend->save("ca_file["+itoa(iFile)+"]",
                                        caDbItem->item);
                                iFile ++;
                                break;
                        case CERT_DB_ITEM_TYPE_DIR:
                                backend->save("ca_dir["+itoa(iDir)+"]",
                                        caDbItem->item);
                                iDir ++;
                                break;
                }

                caDbItem = cert_db->get_next();
        }
        
	cert_db->unlock();
}

void SipDialogSecurityConfig::load( MRef<ConfBackend *> backend ){

	secured = backend->loadString("secured","no")=="yes";
	use_srtp = backend->loadString("use_srtp","no")=="yes";		
	use_ipsec = backend->loadString("use_ipsec","no")=="yes";
	
	dh_enabled   = backend->loadString("dh_enabled","no")=="yes";
	psk_enabled  = backend->loadString("psk_enabled","no")=="yes";
	check_cert   = backend->loadString("check_cert","no")=="yes";

	if( backend->loadString("ka_type", "psk") == "psk" )
		ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
	
	else if( backend->loadString("ka_type", "psk") == "dh" )
		ka_type = KEY_MGMT_METHOD_MIKEY_DH;


	else if( backend->loadString("ka_type", "psk") == "pk" )
		ka_type = KEY_MGMT_METHOD_MIKEY_PK;

	else{
		ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
#ifdef DEBUG_OUTPUT
		merr << "Invalid KA type in config file, default to PSK"<<end;
#endif
	}

	string pskString = backend->loadString("psk","Unspecified PSK");
	psk_length = (int)pskString.size();
	psk = new unsigned char[psk_length];

	memcpy( psk, pskString.c_str(), psk_length );

	/****************************************************************
	 * Certificate settings 
	 ****************************************************************/

	string certFile = backend->loadString("certificate","");
	string privateKeyFile = backend->loadString("private_key","");
	
	cert = new certificate_chain();

	if( certFile != "" ){
		certificate * cert=NULL;

		try{
			cert = new certificate( certFile );
			this->cert->add_certificate( cert );
		}
		catch( certificate_exception & ){
			merr << "Could not open the given certificate " << certFile <<end;
		}

		if( privateKeyFile != "" ){

			try{
				cert->set_pk( privateKeyFile );
			}
			catch( certificate_exception_pkey & ){
				merr << "The given private key " << privateKeyFile << " does not match the certificate"<<end;
			}

			catch( certificate_exception &){
				merr << "Could not open the given private key "<< privateKeyFile << end;
			}
		}
	}

	uint32_t iCertFile = 0;
	certFile = backend->loadString("certificate_chain[0]","");

	while( certFile != "" ){
		try{
			certificate * cert = new certificate( certFile );
			this->cert->add_certificate( cert );
		}
		catch( certificate_exception &){
			merr << "Could not open the given certificate" << end;
		}
		iCertFile ++;
		certFile = backend->loadString("certificate_chain["+itoa(iCertFile)+"]","");

	}

	cert_db = new ca_db();
	iCertFile = 0;
	certFile = backend->loadString("ca_file[0]","");

	while( certFile != ""){
		try{
			cert_db->add_file( certFile );
		}
		catch( certificate_exception &){
			merr << "Could not open the CA certificate" << end;
		}
		iCertFile ++;
		certFile = backend->loadString("ca_file["+itoa(iCertFile)+"]","");

	}
	iCertFile = 0;

	certFile = backend->loadString("ca_dir[0]","");

	while( certFile != ""){
		try{
			cert_db->add_directory( certFile );
		}
		catch( certificate_exception &){
			merr << "Could not open the CA certificate directory " << certFile << end;
		}
		iCertFile ++;
		certFile = backend->loadString("ca_dir["+itoa(iCertFile)+"]","");
	}
}
			

void SipDialogSecurityConfig::useIdentity( MRef<SipIdentity *> identity ){
	identity->lock();
	secured = identity->securitySupport;
	identity->unlock();
}
