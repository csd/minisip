/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include"RemoteGui.h"
#include<unistd.h>
//#include<pthread.h>
#include<signal.h>
#include<errno.h>
#include"../../util/trim.h"
#include"../../util/Thread.h"
#include"../../netutil/ServerSocket.h"
#include<stdio.h>

// How?
// A prompt is displayed to the user
// If a message is available, nothing is prompted, only a bell is sounded.
// The user has the commands:
//   help		displays available commands
//   answer		answers current incoming call
//   reject/deny	do not accpt the call
//   call <nr>		calls a remote UA


//#define GUI_INTERRUPT SIGUSR2

//void sigguihandler(int i){
////	cerr << "Gui signal handler: invoked"<< endl;
//}

/*
void RemoteGui::wake_up(){
	if (my_pid>0){
//		cout << "INFO: Gui sending SIGUSR2 to self"<< endl;
		kill(my_pid, GUI_INTERRUPT);
	}else{
		cerr << "WARNING Gui interrupt ignored"<< endl;
	}
}
*/
/*
void RemoteGui::refresh(){
	refresh("");
}

void RemoteGui::refresh(string message){
	cout << "\r"<<message<<"\n"<< prompt<<flush;
	wake_up();
}
*/

static void *egui_loop_wrapper(void *thisptr){
/*	
	sigset_t usrunblock;
	sigset_t oldset;


	if (sigemptyset(&usrunblock)!=0){
		perror("sigemptyset:");
		exit(1);
	}

	if (sigaddset(&usrunblock, GUI_INTERRUPT )!=0){
		perror("sigaddset:");
		exit(1);
	}

	if (pthread_sigmask(SIG_UNBLOCK, &usrunblock, &oldset)!=0){
		perror("pthread_sigmask:");
		exit(1);
	}
	signal(GUI_INTERRUPT, sigguihandler);

*/	
	((RemoteGui*)thisptr)->gui_loop();
	return NULL;
}


RemoteGui::RemoteGui(GuiCallback *cb, int32_t port): serv_sock(port), sock(NULL){
//	incoming_avail_flag=false;
//	beeper = new Bell(timeoutprovider);
//	my_pid=0;
	set_callback(cb);
//	prompt="NO CALL: ";
	//
	//

/*        
	pthread_t thread;

	int32_t retval = pthread_create(&thread, NULL, egui_loop_wrapper, this);
	if (retval!=0){
		perror("Unable to create pthread");
		exit(1);
	}
*/
        Thread::createThread(egui_loop_wrapper, this);

}

RemoteGui::~RemoteGui(){
//	delete beeper;
//	cerr << "RemoteGui::~RemoteGui: Has shut down curses window"<< endl;
}

void RemoteGui::send_command(string command){
//	cerr << "Trying to send command to gui: "<< command << endl;
	if (sock==NULL){
		cerr << "ERROR: can not send command - GUI not connected"<< endl;
	}else{
		sock->do_write(command+"\n");
//		sock->flush();
	}

}

void RemoteGui::display_remote_ringing(){
//	cerr << "GUI::: remote ringing"<< endl;
//	prompt="REMOTE RINGING: ";
	send_command("remote_ringing");
//	refresh("Ringing at remote side");
}

void RemoteGui::incoming_avail(string from){
	send_command("incoming="+from);
//	prompt="RINGING: ";
//	this->from=from;
//	incoming_avail_flag=true;
//	beeper->start();
//	cerr << "GUI::: incoming available"<< endl;
//	refresh("Incoming call from "+from);
}

void RemoteGui::registred_to_proxy(){
	send_command("registred_to_proxy");
//	refresh("Now registred to proxy.");
}

void RemoteGui::remote_bye(){
//	cerr<<"GUI::: remote_bye"<< endl;	
	send_command("remote_hang_up");
//	prompt = "NO CALL: ";
//	refresh("Remote side hang up");
}

void RemoteGui::remote_accept_invite(){
//	cerr <<"GUI::: remote_accept_invite"<< endl;
	send_command("remote_accept");
//	prompt = "IN CALL: ";
//	refresh("Remote side answered");
}

void RemoteGui::user_not_found(){
	send_command("user_not_found");
}

void RemoteGui::remote_cancelled_invite(){
//	cerr <<"GUI::: remote_cancelled_invite"<< endl;
	send_command("remote_cancelled_invite");
//	incoming_avail_flag=false;
//	prompt ="NO_CALL: ";
//	beeper->stop();
//	refresh("Remote side cancelled");
}

void RemoteGui::print_usage(){
}

void RemoteGui::gui_loop(){
	char inbuf[1024];
	sock=0;

	while (1){
		if (sock==0)
			cerr << "INFO: waiting for GUI..."<< endl;
		if (sock!=0){
			exit(0);
		}
		sock = serv_sock.do_accept();
//		cerr << "INFO: GUI connected"<< endl;


		//	my_pid=getpid();
		bool done =false;
		while(!done){
			//		cout << prompt << flush;
			inbuf[0]=0;
//			cerr << "INFO: GUI waiting for command..." << endl;
			int32_t ret=sock->do_read(inbuf, 1024);
			if (ret>0)
				inbuf[ret]=0;
			if (ret==0)
				done=true;
//			cout << "GUI read "<<ret  <<" bytes <"<< inbuf << ">"<< endl;
			if (ret<0){
				if (errno==EINTR)
					; 
				else{
					perror("while reading from standard input");
					exit(1);
				}
				continue;
			}
			
			if (strncmp(inbuf,"register",8)==0){
				callback->guicb_do_register();
				continue;
			}
			
			if (strncmp(inbuf,"call",4)==0){
				if (strlen(inbuf)>=6){
					string pno(&inbuf[5]);
					pno = trim(pno);
					//				if (incoming_avail_flag){
				//					cout <<"Rejecting incoming call"<< endl;
					//					callback->guicb_deny_incoming();
					//				}
//					cout << "Calling <"<<pno <<">" <<endl;
					//				prompt="CONNECTING: ";
					callback->guicb_do_invite(pno);
				}else
					print_usage();
				continue;
			}
			if (strncmp(inbuf,"accept",6)==0){
				//			if (incoming_avail_flag){
				//				cout <<"Answering call"<< endl;
				//				incoming_avail_flag=false;
				//				beeper->stop();
				//				prompt="IN CALL:";
				callback->guicb_accept_incoming();
				//			}else{
				//				cout << "No incoming call"<< endl;
				//			}
				continue;
			}
	
			if (strncmp(inbuf,"pcmu",4)==0){
				cout << "Changing codec to pcmu"<< endl;
				callback->guicb_set_codec(0);
				continue;
			}
			
			if (strncmp(inbuf,"ilbc",4)==0){
				cout << "Changing codec to ilbc"<< endl;
				callback->guicb_set_codec(97);
				continue;
			}
		
			if (strncmp(inbuf,"reject",6)==0){
				//			if (incoming_avail_flag){
//				cout << "User does not want to accept the call"<< endl;
				//				beeper->stop();
				callback->guicb_deny_incoming();
				//				incoming_avail_flag=false;
				//			 }else{
				//			 	cout << "No incoming call"<< endl;
				//			 }

				continue;
			}
			if (strncmp(inbuf,"cancel",6)==0){
				callback->guicb_cancel_invite();
				continue;
			}
	
			if (strncmp(inbuf,"hang up",7)==0){
//				cout << "DEBUG: Hanging up"<< endl;
//				cerr << "DEBUG: Hanging up"<< endl;


				//			if (strncmp("CONNECTING: ", prompt.c_str(),10)==0 || strncmp("REMOTE RINGING: ",prompt.c_str(),14)==0){
				//					cerr << "DEBUG: In Linegui: doing cancel_invite"<< endl;
				//					callback->guicb_cancel_invite();
				//					prompt="NO CALL :";
				//			}else{
				//				if (strncmp("IN CALL: ", prompt.c_str(),7)==0){
//				cerr << "In linegui: doing guicb_hang_up";
				callback->guicb_hang_up();
				//				}else{
				//					cout << "Can not hang up - not in call"<< endl;
		 		//					cerr << "Can not hang up - not in call"<< endl;
				//				}
				continue;
			}
										//		}else
			if (strncmp(inbuf,"quit",4)==0){
//				cout <<"Quitting"<< endl;
				callback->guicb_quit_program();
				continue;
			}
				
			if (strncmp(inbuf,"help",4)==0){
				print_usage();
				continue;
			}
			cerr << "WARNING: unknown command: "<< inbuf << endl;
			print_usage();
		}
	}
};
