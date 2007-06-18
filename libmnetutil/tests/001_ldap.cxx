#include <libmnetutil/LdapConnection.h>
#include <libmnetutil/LdapCredentials.h>
#include <libmnetutil/LdapUrl.h>

using namespace std;

int main(int argc, char *argv[])
{
	if( argc != 2 ){
		cerr << "Usage: " << argv[0] << " ldap://<host>/<dn>[?[<attributes>][?[<scope>][?[<filter>]]]]" << endl;
		return 1;
	}

	const char *url_str = argv[1];

	LdapUrl url(url_str);

	url.printDebug();

	MRef<LdapCredentials*> cred = new LdapCredentials("","");
	MRef<LdapConnection*> conn = new LdapConnection(url.getHost(), url.getPort(), cred);

	std::vector<MRef<LdapEntry*> > entries;
	vector<string> attrs = url.getAttributes();
	string label = attrs[0];

	entries = conn->find(url.getDn(), url.getFilter(), attrs);
	
	std::vector<MRef<LdapEntry*> >::iterator i;
	for (i = entries.begin(); i != entries.end(); i++) {
		MRef<LdapEntry*> entry = *i;

		vector<string> attrs = entry->getAttrNames();
		vector<string>::iterator j;

		for (j = attrs.begin(); j != attrs.end(); j++) {
			const string &attr_name = *j;

			vector<string> values = entry->getAttrValuesStrings(attr_name);
			vector<string>::iterator k;

			for (k = values.begin(); k != values.end(); k++) {
				const string &value = *k;
				cout << attr_name << ":" << value << endl;
			}
		}
	}
	
	return 0;
}
