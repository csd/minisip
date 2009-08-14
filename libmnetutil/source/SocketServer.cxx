/*
  Copyright (C) 2006-2007 Mikael Magnusson

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#include<config.h>

#include<libmnetutil/SocketServer.h>
#include<libmutil/CriticalSection.h>
#include<libmnetutil/NetworkException.h>
#include<libmnetutil/StreamSocket.h>
#include<libmnetutil/IPAddress.h>

#include<algorithm> /* find_if */

#ifdef WIN32
# include<io.h>
# include<fcntl.h>
typedef int socklen_t;
#else
#define closesocket close
#endif

#ifndef SOCKET
# ifdef WIN32
#  define SOCKET uint32_t
# else
#  define SOCKET int32_t
# endif
#endif

using namespace std;

class EqualAddrPort: public std::unary_function< std::pair<const MRef<Socket*>, MRef<InputReadyHandler*> >&, bool>{
	public:
		EqualAddrPort( const IPAddress &theAddress,
			       uint16_t thePort )
				: address( theAddress ), port( thePort ){}

		bool operator() ( pair<const MRef<Socket*>, MRef<InputReadyHandler*> > &pair  ) {
			MRef<Socket *> sock = pair.first;
			if( sock.isNull() )
				return false;

			StreamSocket *ssock =
				dynamic_cast<StreamSocket*>(*sock);

			if( !ssock )
				return false;

			return ssock->matchesPeer( address, port );
		}
	private:
		const IPAddress &address;
		uint16_t port;
};


InputReadyHandler::~InputReadyHandler()
{
}


SocketServer::SocketServer(): fdSignal( -1 ), doStop( false )
{
}

SocketServer::~SocketServer()
{
	stop();
	join();
}

void SocketServer::start()
{
	CriticalSection cs( csMutex );
	if( !thread.isNull() )
		return;
	if (fdSignal < 0)
		createSignalPipe();

	thread = new Thread( this );
}

void SocketServer::stop()
{
	CriticalSection cs( csMutex );

	if( thread.isNull() )
		return;

	doStop = true;
	signal();
}

void SocketServer::join()
{
	CriticalSection cs( csMutex );

	if( thread.isNull() ){
		return;
	}

	thread->join();
	thread = NULL;
}

void SocketServer::addSocket( MRef<Socket*> socket,
			      MRef<InputReadyHandler*> handler )
{
	CriticalSection cs( csMutex );

	sockets[ socket ] = handler;
	signal();
}

void SocketServer::removeSocket( MRef<Socket*> socket )
{
	CriticalSection cs( csMutex );

	Sockets::iterator pos = sockets.find( socket );
	if( pos != sockets.end() ){
		sockets.erase( pos );
		signal();
	}
}

MRef<Socket*> SocketServer::findStreamSocketPeer( const IPAddress &addr,
						  uint16_t port )
{
	CriticalSection cs( csMutex );

	EqualAddrPort pred( addr, port );

	Sockets::iterator iter =
		find_if( sockets.begin(), sockets.end(), pred );

	if( iter == sockets.end() )
		return NULL;

	return iter->first;
}

void SocketServer::signal()
{

	char c = 0;

	if( fdSignal < 0 )
		return;

#ifdef WIN32
 	if( send( fdSignal, &c, sizeof( c ), 0) < 0 ) {
#else
	if( write( fdSignal, &c, sizeof( c )) < 0 ) {
#endif
		cerr << "Write failed " << fdSignal << endl;
		throw NetworkException( errno );
	}
}

int SocketServer::buildFdSet( fd_set *set, int pipeFd )
{
	SOCKET maxFd = -1;
	Sockets::const_iterator i;

	FD_ZERO( set );
	FD_SET( (SOCKET)pipeFd, set );

	maxFd = pipeFd;

	for( i = sockets.begin(); i != sockets.end(); i++ ){
		MRef<Socket*> socket = i->first;
		SOCKET fd = socket->getFd();

		FD_SET( fd, set );
		if( fd > maxFd )
			maxFd = fd;
	}

	return maxFd;
}

#ifdef WIN32
static int createTcpPipe( int32_t fds[2] )
{
	uint32_t sd;
	struct sockaddr_in sa;
	socklen_t sz = sizeof(sa);
	u_long nonBlocking = 1;

	sd = socket( AF_INET, SOCK_STREAM, 0 );
	if( sd < 0 ){
		throw NetworkException( errno );
	}

	memset( &sa, 0, sz );
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");
	sa.sin_family = AF_INET;

	bind( sd, (struct sockaddr *)&sa, sz );

	if( listen( sd, 1 ) < 0 ){
		closesocket( sd );
		throw NetworkException( errno );
	}

	if( getsockname( sd, (struct sockaddr *)&sa, &sz )){
		closesocket( sd );
		closesocket( fds[0] );
		throw GetSockNameFailed( errno );
	}

	fds[0] = socket( AF_INET, SOCK_STREAM, 0 );
	if( fds[0] < 0 ){
		closesocket( sd );
		throw NetworkException( errno );
	}

	if( connect( fds[0], (struct sockaddr *)&sa, sz ) < 0 ){
		closesocket( sd );
		closesocket( fds[0] );
		throw GetSockNameFailed( errno );
	}		

	sz = sizeof( sa );
	fds[1] = accept( sd, (struct sockaddr *)&sa, &sz );

	if( fds[1] < 0 ){
		closesocket( sd );
		closesocket( fds[0] );
		throw NetworkException( errno );
	}

	ioctlsocket( fds[0], FIONBIO, &nonBlocking);
	ioctlsocket( fds[1], FIONBIO, &nonBlocking);

	closesocket( sd );
	return 0;
}
#endif	// WIN32

void SocketServer::createSignalPipe(){
	int32_t pipeFds[2] = {-1,-1};

	if( fdSignal >= 0 ){
		close( fdSignal );
		fdSignal = -1;
	}

#ifdef WIN32
	// Use TCP sockets since Windows pipes don't support select
	if( createTcpPipe( pipeFds )) {
#else
	if( ::pipe( pipeFds ) ){
#endif
		throw Exception( "Can't create pipe" );
	}

	fdSignal = pipeFds[1];
	fdSignalInternal = pipeFds[0];
}

void SocketServer::closeSockets(){
	Sockets::const_iterator i;
	for( i = sockets.begin(); i != sockets.end(); i++ ){
		MRef<Socket*> socket = i->first;
		socket->close();
	}
}

void SocketServer::run()
{
#ifdef DEBUG_OUTPUT
	setThreadName("SocketServer::run");
#endif
	struct timeval timeout;
	fd_set tmpl;
	fd_set set;
	int maxFd = -1;
	Sockets::const_iterator i;

	if (fdSignal < 0)
		createSignalPipe();

	while (!doStop){
		maxFd = buildFdSet( &tmpl, fdSignalInternal );

		int avail;
		do{
			memcpy( &set, &tmpl, sizeof( set ) );

			timeout.tv_sec = 5;
			timeout.tv_usec= 0;
			avail = select( maxFd + 1, &set, NULL, NULL, &timeout );
			if (avail<0){
				Thread::msleep(500);
			}
		} while( avail < 0 );
		if (avail==0){
// 			cerr<< "SipSocketServer::run(): Timeout"<< endl;
		}

		if( FD_ISSET( fdSignalInternal, &set ) ){
			char buf[255];

#ifdef WIN32
			if( recv( fdSignalInternal, buf, sizeof(buf), 0 ) < 0){
#else
			if( read( fdSignalInternal, buf, sizeof(buf) ) < 0){
#endif
				cerr << "Read failed" << endl;
				throw NetworkException( errno );
			}

		}

		for( i = sockets.begin(); i != sockets.end(); i++ ){
			MRef<Socket*> socket = i->first;
			MRef<InputReadyHandler*> handler = i->second;

			if( FD_ISSET( socket->getFd(), &set ) ){
				if( !handler.isNull() ){
					handler->inputReady( socket );
				}
			}
		}
	}

// 	csMutex.lock();

	closesocket( fdSignalInternal );
	closesocket( fdSignal );
	fdSignal = -1;
	doStop = false;

// 	csMutex.unlock();
}

