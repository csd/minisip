#include <libmnetutil/Downloader.h>
#include <libmnetutil/HttpDownloader.h>

#ifdef ENABLE_LDAP
#include <libmnetutil/LdapDownloader.h>
#endif

#include <string>

MRef<Downloader*> Downloader::create(std::string const uri) {
	size_t pos = uri.find("://");
	if (std::string::npos != pos) {
		std::string protocol = uri.substr(0, pos);
		if (protocol == "http")
			return MRef<Downloader*>(dynamic_cast<Downloader*>(new HttpDownloader(uri)));
#ifdef ENABLE_LDAP
		else if (protocol == "ldap")
			return MRef<Downloader*>(dynamic_cast<Downloader*>(new LdapDownloader(uri)));
#endif
	}
	return MRef<Downloader*>();
}
