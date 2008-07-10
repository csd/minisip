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

#ifndef LIBMNETUTIL_SOCKETSERVER_H
#define LIBMNETUTIL_SOCKETSERVER_H

#include<libmnetutil/libmnetutil_config.h>

#include<libmutil/MemObject.h>
#include<libmutil/Mutex.h>
#include<libmutil/Thread.h>
#include<libmnetutil/Socket.h>

#include<map>

#ifdef WIN32
# include <winsock2.h>
#else
#include<sys/select.h>
#endif

class LIBMNETUTIL_API InputReadyHandler: public virtual MObject {
	public:
		virtual ~InputReadyHandler();
		virtual void inputReady( MRef<Socket*> socket )=0;
};

class LIBMNETUTIL_API SocketServer : public Runnable {
	public:
		SocketServer();
		virtual ~SocketServer();

		/**
		 * Closes all sockets this server references.
		 */
		void closeSockets();

		/**
		 * Creates a thread that runs the "run" method that
		 * listens to all sockets registred to the object.
		 */
		void start();

		/**
		 *
		 * Signals the method running the "run" method to stop.
		 * This will not close the sockets.
		 *
		 */
		void stop();

		/**
		 * Waits until the thread created by "start" that runs the
		 * "run" method has stopped.
		 */
		void join();

		void addSocket( MRef<Socket*> socket, MRef<InputReadyHandler*> handler );
		void removeSocket( MRef<Socket*> socket );

		MRef<Socket*> findStreamSocketPeer( const IPAddress &addr,
						    uint16_t port );

	protected:
		void run();
		void signal();
		int buildFdSet( fd_set *set, int pipeFd );

	private:
		void createSignalPipe();
		typedef std::map< MRef<Socket*>, MRef<InputReadyHandler*> > Sockets;
		Mutex csMutex;
		MRef<Thread *> thread;
		Sockets sockets;

		int fdSignal;		///Socket pair used to wake thread running run()
		int fdSignalInternal;	///from its select. This is typically used to
					///to tell the thread to stop.

		bool doStop;
};
#endif
