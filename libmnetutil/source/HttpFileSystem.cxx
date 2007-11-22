
#include<config.h>

#include<libmnetutil/HttpFileSystem.h>
#include<libmnetutil/HttpDownloader.h>

using namespace std;

class FileString : public File {
public:
	FileString(char *data, int len, MRef<StreamSocket*> ssock);
	virtual void read(void *buf, int32_t count);
	virtual void write(void *buf, int32_t count);
	virtual void seek(int32_t pos );
	virtual int64_t size();
	virtual void flush();

private:
	char *data;
	int64_t len;
	int64_t curpos;
	bool dirty;
	int64_t allocSize;
	MRef<StreamSocket*> ssock;

};

FileString::FileString( char *d, int l, MRef<StreamSocket*> ssock_ ) : data(d), len(l), 
					   curpos(0), dirty(false), 
					   allocSize(len), ssock(ssock_) { }


void FileString::read(void *buf, int32_t count){
	if ( count+curpos > len ){
		//count = len-curpos;
		throw FileException("Read beyond end of file");
	}
	memcpy(buf, &data[curpos], count);
}

void FileString::write(void *buf, int32_t count){
	if (curpos+count > allocSize){
		allocSize =  curpos+count + (curpos+count)/4;
		data = (char*)realloc(data, allocSize ); // alloc 25% more than needed
	}

	memcpy( &data[curpos], buf, count );

	curpos+=count;
	if (curpos>len)
		len = curpos;
}


void FileString::seek(int32_t pos ){
	curpos=pos;
	if (curpos>len)
		curpos=len;
}

int64_t FileString::size(){
	return len;
}

void FileString::flush(){
#warning FileString flush not implemented FIXME
//	httpDl->uploadFile(myPath, data, len);
}

HttpFileSystem::HttpFileSystem(MRef<StreamSocket*> conn_, string prefix_) : 
		prefix(prefix_), conn(conn_) { }

void HttpFileSystem::mkdir( const std::string & name ){

}

MRef<File*> HttpFileSystem::open( const std::string& path, bool createIfNotExist ){
	char *data=NULL;
	int len=0;
	HttpDownloader dl( "www.minisip.org/~erik/data.bin", conn );
	data = dl.getChars(&len);
#warning download data here FIXME;
	return new FileString( data, len, conn);

}


