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

#include"LineGui.h"
#include<unistd.h>
#include<curses.h>
#include<pthread.h>
#include<signal.h>
#include<errno.h>
#include"../../util/trim.h"
#include"../../util/TimeoutProvider.h"
#include<iostream>

// How?
// A prompt is displayed to the user
// If a message is available, nothing is prompted, only a bell is sounded.
// The user has the commands:
//   help		displays available commands
//   answer		answers current incoming call
//   reject/deny	do not accpt the call
//   call <nr>		calls a remote UA

using namespace std;

#define GUI_INTERRUPT SIGUSR2

void sigguihandler(int32_t i){
//	cerr << "Gui signal handler: invoked"<< endl;
}


void LineGui::wake_up(){
	if (my_pid>0){
//		cout << "INFO: Gui sending SIGUSR2 to self"<< endl;
		kill(my_pid, GUI_INTERRUPT);
	}else{
		std::cerr << "WARNING Gui interrupt ignored"<< std::endl;
	}
}


void LineGui::refresh(){
	refresh("");
}

void LineGui::refresh(std::string message){
	std::cout << "\r"<<message<<"\n"<< prompt<<std::flush;
	wake_up();
}

static void *egui_loop_wrapper(void *thisptr){
	
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
	
	((LineGui*)thisptr)->gui_loop();
	return NULL;
}


LineGui::LineGui(GuiCallback *cb, TimeoutProvider *timeoutprovider){
	incoming_avail_flag=false;
	beeper = new Bell(timeoutprovider);
	my_pid=0;
	set_callback(cb);
	prompt="NO CALL: ";

	pthread_t thread;

	int32_t retval = pthread_create(&thread, NULL, egui_loop_wrapper, this);
	if (retval!=0){
		perror("Unable to create pthread");
		exit(1);
	}


}

LineGui::~LineGui(){
	delete beeper;
//	cerr << "LineGui::~LineGui: Has shut down curses window"<< endl;
}

void LineGui::display_remote_ringing(){
//	cerr << "GUI::: remote ringing"<< endl;
	prompt="REMOTE RINGING: ";
	refresh("Ringing at remote side");
}

void LineGui::incoming_avail(std::string from){
	prompt="RINGING: ";
	this->from=from;
	incoming_avail_flag=true;
	beeper->start();
//	cerr << "GUI::: incoming available"<< endl;
	refresh("Incoming call from "+from);
}

void LineGui::registred_to_proxy(){
	refresh("Now registred to proxy.");
}

void LineGui::user_not_found(){

}

void LineGui::remote_bye(){
//	cerr<<"GUI::: remote_bye"<< endl;	
	prompt = "NO CALL: ";
	refresh("Remote side hang up");
}

void LineGui::remote_accept_invite(){
//	cerr <<"GUI::: remote_accept_invite"<< endl;
	prompt = "IN CALL: ";
	refresh("Remote side answered");
}

void LineGui::remote_cancelled_invite(){
//	cerr <<"GUI::: remote_cancelled_invite"<< endl;
	incoming_avail_flag=false;
	prompt ="NO_CALL: ";
	beeper->stop();
	refresh("Remote side cancelled");
}

void print_usage(){
	std::cout << "Commands:\n\thelp			displays this screen\n\tregister		Registers to the proxy to enable incoming calls\n\tcall <no>		Calls the user <no>\n\treject or deny		reject incoming call\n\taccept			answers call\n\thang up			ends the call\n\tquit			quits the program\n\tpcmu			force PCMu CODEC\n\tilbc			Force iLBC CODEC"<< std::endl;
}

void LineGui::gui_loop(){
	char inbuf[1024];

	my_pid=getpid();
	while(1){
		std::cout << prompt << std::flush;
		inbuf[0]=0;
		int ret=read(STDIN_FILENO, inbuf, 1024);
//		cout << "read <"<< inbuf << ">"<< endl;
		if (ret<0){
			if (errno==EINTR)
				; 
			else{
				perror("while reading from standard input");
				exit(1);
			}
		}else
		if (strncmp(inbuf,"register",8)==0){
			callback->guicb_do_register();
		}else
		if (strncmp(inbuf,"call",4)==0){
			if (strlen(inbuf)>=6){
				std::string pno(&inbuf[5]);
				pno = trim(pno);
				if (incoming_avail_flag){
					std::cout <<"Rejecting incoming call"<< std::endl;
					callback->guicb_deny_incoming();
				}
				std::cout << "Calling <"<<pno <<">" <<std::endl;
				prompt="CONNECTING: ";
				callback->guicb_do_invite(pno);
			}else
				print_usage();
		}else
		if (strncmp(inbuf,"accept",6)==0){
			if (incoming_avail_flag){
				std::cout <<"Answering call"<< endl;
				incoming_avail_flag=false;
				beeper->stop();
				prompt="IN CALL:";
				callback->guicb_accept_incoming();
			}else{
				cout << "No incoming call"<< endl;
			}
		}else
		if (strncmp(inbuf,"pcmu",4)==0){
			std::cout << "Changing codec to pcmu"<< endl;
			callback->guicb_set_codec(0);
		}else
		if (strncmp(inbuf,"ilbc",4)==0){
			std::cout << "Changing codec to ilbc"<< endl;
			callback->guicb_set_codec(97);
		}else
		if (strncmp(inbuf,"reject",6)==0){
			if (incoming_avail_flag){
				std::cout << "User does not want to accept the call"<< endl;
				beeper->stop();
				callback->guicb_deny_incoming();
				incoming_avail_flag=false;
			 }else{
				 std::cout << "No incoming call"<< endl;
			 }
			 	 
		}else
		if (strncmp(inbuf,"hang up",7)==0){
			std::cout << "DEBUG: Hanging up"<< endl;
			std::cerr << "DEBUG: Hanging up"<< endl;
			

			if (strncmp("CONNECTING: ", prompt.c_str(),10)==0 || strncmp("REMOTE RINGING: ",prompt.c_str(),14)==0){
				std::cerr << "DEBUG: In Linegui: doing cancel_invite"<< endl;
					callback->guicb_cancel_invite();
					prompt="NO CALL :";
			}else{
				if (strncmp("IN CALL: ", prompt.c_str(),7)==0){
					std::cerr << "In linegui: doing guicb_hang_up";
					callback->guicb_hang_up();
				}else{
					std::cout << "Can not hang up - not in call"<< endl;
					std::cerr << "Can not hang up - not in call"<< endl;
				}
			}
		}else
		if (strncmp(inbuf,"quit",4)==0){
			cout <<"Quitting"<< endl;
			callback->guicb_quit_program();
		}else
		if (strncmp(inbuf,"help",4)==0){
			print_usage();
		}else{
			print_usage();
		}
	}

};
