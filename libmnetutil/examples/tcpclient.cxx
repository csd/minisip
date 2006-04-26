/* tcpclient - distributed with @PACKAGE@-@PACKAGE_VERSION@ */
#include<libmnetutil/TCPSocket.h> 
#include<libmnetutil/NetworkException.h> //ConnectFailed && HostNotFound
#include<libmutil/trim.h>  //removes whitespace from start and end of strings

using namespace std;

int main(int argc, char **argv){

	if (argc!=4){
		cerr << "Usage: tcpclient <serveraddress> <serverport> <texttosend>\nExample:  ./tcpclient localhost 3333 \"Hello world\"."<<endl;
		return 1;
	}
	

	TCPSocket *sock;
	try{
		//Create a TCP connection to the server (address and port given
		//as arguments to the application).
		sock = new TCPSocket(argv[1], atoi(argv[2]));
	}catch(ConnectFailed &){
		cerr << "Sorry, I could not connect to port "<< argv[2] << " on the server." << endl;
		return -3;
	
	}catch(HostNotFound &){
		cerr << "Sorry, the server <"<< argv[1]<<"> could not be found"<< endl;
		return -3;
	}
	
	//Send some data to the client
	sock->write(argv[3]);

	//Read some data from the server
	char buf[10000];
	int nread = sock->read(buf, 10000);
	buf[nread]=0;

	//Print what we read to the screen
	cerr << "Received from server: "<< buf<<endl;
	
	sock->close();
	
	return 0;
}
