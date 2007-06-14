#include <libmnetutil/LdapConnection.h>

using namespace std;

int main(int argc, char *argv[])
{
	if( argc != 3 ){
		cerr << "Usage: " << argv[0] << " <ldap server> <base dn>" << endl;
		return 1;
	}

	const char *server_name = argv[1];
	const char *base_dn = argv[2];

	MRef<LdapConnection*> conn = new LdapConnection(server_name);

	std::vector<MRef<LdapEntry*> > entries;
	vector<string> attrs;
	entries = conn->find(base_dn, "(objectclass=*)", attrs);
	
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
