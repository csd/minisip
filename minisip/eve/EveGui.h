#ifndef _EVE_GTKGUI_H
#define _EVE_GTKGUI_H

#include<gtk/gtk.h>

#include"AudioStreamAnalyzer.h"
#include"SipAnalyzer.h"

#include<string>
#include<vector>
#include<libmutil/Mutex.h>
#include<list>
#include<libmutil/CommandString.h>
#include<libmutil/minilist.h>

#include"eve.h"

using namespace std;

class EveGui{
	public:
		EveGui(int main, char **argv);

		void run();

		bool selectedForPlaying(string from, string to);
		void addStream(string from, string to);
		void removeStream(string from, string to);

		void addSignallingMessage(string);

		void setInterface(int i);
		void setSoundCard(int i);

		string getInterface();

		void timeout();

		void start();
		void stop();

		//void gotCommand();
	private: 
		Mutex lock;

		minilist<CommandString> cmdlist;
		minilist<string> playing;

		Sniffer *sniffer;
		SipAnalyzer *sipAnalyzer;
		AudioStreamAnalyzer *audioAnalyzer;
		AudioReceiver *audioReceiver;
	
		
		GtkWidget *create_menubar_menu( GtkWidget  *window );

		
		string network_interface;
		string sound_card;

		vector<string> all_interfaces;

		
		GtkWidget *createSignallingList();
		GtkWidget *createPlayList();
		
		GtkWidget *window;
		GtkWidget *layoutBox;

		bool started;
	
		/// Vertical split of the signal and play areas
		GtkWidget *splitPane;

		///
		GtkWidget *signalView;
		
		
		///
		GtkWidget *playScroll;
		GtkWidget *playView;


		///
		GtkWidget *statusbar;
		
		GtkItemFactory *item_factory;
};

#endif
