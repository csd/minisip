#include <libmnetutil/Downloader.h>
#include <libmnetutil/HttpDownloader.h>
#include <libmnetutil/LdapDownloader.h>
#include <string>

MRef<Downloader*> Downloader::create(std::string const uri) {
	int pos = uri.find("://");
	if (std::string::npos != pos) {
		std::string protocol = uri.substr(0, pos);
		if (protocol == "http")
			return MRef<Downloader*>(dynamic_cast<Downloader*>(new HttpDownloader(uri)));
		else if (protocol == "ldap")
			return MRef<Downloader*>(dynamic_cast<Downloader*>(new LdapDownloader(uri)));
	}
	return MRef<Downloader*>();
}
