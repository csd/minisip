/* tcpclient - distributed with @PACKAGE@-@PACKAGE_VERSION@ */
#include<libmnetutil/IP4ServerSocket.h> 
#include<libmutil/trim.h>  //removes whitespace from start and end of strings

#define SERVER_PORT 3333   //This defines the port number that the server will open

using namespace std;

int main(int argc, char **argv){

	//Create a server socket to which clients can connect.
	IP4ServerSocket* ssock = new IP4ServerSocket(SERVER_PORT);
	
	//If we want automatic garbage collecting, we should do this
	//instead:
	//MRef<IP4ServerSocket*> ssock = new IP4ServerSocket(SERVER_PORT);

	string from_client; //This will store the first line of data the client sends to us.
	do{
		char receive_buf[10000];
		
		//Wait for a client to connect
		//If client_sock is already a client, this line
		//will close that connection (since we use the MRef 
		//that will throw away any socket we don't reference
		//any longer). This means that the previous clients socket
		//will be closed.
		MRef<StreamSocket *> client_sock = ssock->accept();
		
		//Read a chunk of data from that client
		int nread = client_sock->read(receive_buf, 10000);
		
		//It is probably not a null-terminated string. This makes
		//sure it is.
		receive_buf[nread]=0;

		//Trim away any leading or trailing whitespace
		from_client= trim( string(receive_buf) );

		//Send some data to the client
		client_sock->write("I read <"+from_client+"> from you. Bye.\n");
	
	//if the client told us to quit, then let's do that.	
	}while(from_client!="server quit");
	
	return 0;
}
