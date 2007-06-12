#include <libmnetutil/Downloader.h>
#include <libmnetutil/HttpDownloader.h>
#include <string>

using namespace std;

MRef<Downloader*> Downloader::create(string const uri) {
	int pos = uri.find("://");
	if (string::npos != pos) {
		string protocol = uri.substr(0, pos);
		if (protocol == "http")
			return MRef<Downloader*>(dynamic_cast<Downloader*>(new HttpDownloader(uri)));
	}
	return MRef<Downloader*>();
}
