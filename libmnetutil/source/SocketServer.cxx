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

#ifdef WIN32
# include<io.h>
# include<fcntl.h>
# define write _write
# define pipe(fds) _pipe((fds), 256, O_BINARY)
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
}

void SocketServer::start()
{
	CriticalSection cs( csMutex );
	if( !thread.isNull() )
		return;

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

	if( write( fdSignal, &c, sizeof( c )) < 0 ) {
		cerr << "Write failed " << fdSignal << endl;
		throw NetworkException( errno );
	}
}

int SocketServer::buildFdSet( fd_set *set, int pipeFd )
{
	SOCKET maxFd;
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

void SocketServer::run()
{
	struct timeval timeout;
	fd_set tmpl;
	fd_set set;
	int pipeFds[2] = {-1,-1};
	int maxFd;
	Sockets::const_iterator i;

	csMutex.lock();

	if( fdSignal >= 0 ){
		close( fdSignal );
		fdSignal = -1;
	}

	if( ::pipe( pipeFds ) ){
		throw Exception( "Can't create pipe" );
	}

	fdSignal = pipeFds[1];

	csMutex.unlock();

	while (!doStop){
		maxFd = buildFdSet( &tmpl, pipeFds[0] );

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

		if( FD_ISSET( pipeFds[0], &set ) ){
			char buf[255];

			if( read( pipeFds[0], buf, sizeof(buf) ) < 0){
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

	fdSignal = -1;
	close( pipeFds[0] );
	close( pipeFds[1] );
	doStop = false;

// 	csMutex.unlock();
}
