#include"EveGui.h"

#include<iostream>
#include<libmnetutil/NetworkFunctions.h>
#include<vector>

using namespace std;

char *sound_devices[]={"/dev/dsp","/dev/dsp1","/dev/dsp2","/dev/dsp3","/dev/dsp4"}; //Warning - these constants are hard coded in "default_menu_items" as well
EveGui *globalEve;
GtkWidget *eve_global_main_window;

void about();

static void start(){
	globalEve->start();
}

static void warn(string msg){
	GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(eve_global_main_window),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_CLOSE,
			msg.c_str());
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

void stop(){
	globalEve->stop();

}

void EveGui::start(){
	if (!started){
		gtk_window_set_title(GTK_WINDOW(eve_global_main_window), "Eve (running)");
		//cerr << "starting eavesdropping"<< endl;
		started = true;
		//cerr << "device = <"<< getInterface()<<">"<< endl;
		try{
			sniffer = new Sniffer(getInterface());
			sipAnalyzer = new SipAnalyzer(this);
			sniffer->addHandler(sipAnalyzer, Sniffer::PROTOCOL_UDP);
			sniffer->addHandler(sipAnalyzer, Sniffer::PROTOCOL_TCP);
			
			audioReceiver = new AudioReceiver(NULL); //XXX DO NOT INITIALIZE SOUND PLAYER TO NULL!!!
			audioAnalyzer= new AudioStreamAnalyzer(this, audioReceiver);
			sniffer->addHandler(audioAnalyzer, Sniffer::PROTOCOL_UDP);
			sniffer->createRunThread();
			audioAnalyzer->startMaintainerThread();

			GtkWidget *mitem = gtk_item_factory_get_widget(item_factory,"<main>/File/Start");
			assert(mitem!=NULL);
			gtk_widget_set_sensitive(mitem,FALSE);
			mitem = gtk_item_factory_get_widget(item_factory,"<main>/File/Stop");
			assert(mitem!=NULL);
			gtk_widget_set_sensitive(mitem,TRUE);
		}catch(SnifferException*){
			warn("The network interface could not be initialized. You must run\n this application as super user (root).");
			
			gtk_window_set_title(GTK_WINDOW(eve_global_main_window), "Eve (stopped)");
			started=false;
		};
	}

}

string EveGui::getInterface(){
	return network_interface;
}

void EveGui::stop(){
	if (started){
		gtk_window_set_title(GTK_WINDOW(eve_global_main_window), "Eve (stopped)");
//		cerr << "stopping eavesdropping"<< endl;
		started=false;
		sniffer->stop();
		delete sniffer;
		sniffer=NULL;
		delete sipAnalyzer;
		sipAnalyzer = NULL;
		delete audioAnalyzer;
		audioAnalyzer=NULL;
		delete audioReceiver;
		audioReceiver=NULL;
		GtkWidget *mitem = gtk_item_factory_get_widget(item_factory,"<main>/File/Start");
		assert(mitem!=NULL);
		gtk_widget_set_sensitive(mitem,TRUE);
		mitem = gtk_item_factory_get_widget(item_factory,"<main>/File/Stop");
		assert(mitem!=NULL);
		gtk_widget_set_sensitive(mitem,FALSE);
	}
}

static void soundcard_cb( gpointer   callback_data,
                            guint      callback_action,
                            GtkWidget *menu_item )
{
   if(GTK_CHECK_MENU_ITEM(menu_item)->active){
   	globalEve->setSoundCard(callback_action);
   }

}

static void interface_cb( gpointer   callback_data,
                            guint      callback_action,
                            GtkWidget *menu_item )
{
   if(GTK_CHECK_MENU_ITEM(menu_item)->active){
	globalEve->setInterface(callback_action);
   }

}

void EveGui::setInterface(int i){
	string netif = all_interfaces[i];
	bool warning = started && (network_interface!=netif);
	network_interface = netif;
	if (warning){
		warn("Stop and start eavesdropping to use this setting.");
	}
}


void EveGui::setSoundCard(int i){
	string scard= sound_devices[i];
	bool warning = started && (sound_card!=scard);
	sound_card = scard;
	if (warning){
		warn("Stop and start eavesdropping to use this setting.");
	}
}



static GtkItemFactoryEntry default_menu_items[] = {
  { "/_File",         NULL,         NULL,           0, "<Branch>" },
  { "/File/_Start",    "<control>S", start,    0, "<StockItem>", GTK_STOCK_MEDIA_PLAY},
  { "/File/Sto_p",    "<control>P", stop,    0, "<StockItem>", GTK_STOCK_MEDIA_STOP},
  { "/File/sep1",     NULL,         NULL,           0, "<Separator>" },
  { "/File/_Quit",    "<CTRL>Q", (void (*)())gtk_main_quit, 0, "<StockItem>", GTK_STOCK_QUIT },
  { "/_Options",      NULL,         NULL,           0, "<Branch>" },
  { "/Options/Interface",  NULL,    NULL, 0, "<Branch>" },
  
  { "/Options/sep1",     NULL,         NULL,           0, "<Separator>" },
  
  { "/Options/Sound card",  NULL,    NULL, 0, "<Branch>" },
  { "/Options/Sound card/dsp",  NULL,    (void (*)())soundcard_cb, 0, "<RadioItem>" },
  { "/Options/Sound card/dsp1",  NULL,    (void (*)())soundcard_cb, 1, "/Options/Sound card/dsp" },
  { "/Options/Sound card/dsp2",  NULL,    (void (*)())soundcard_cb, 2, "/Options/Sound card/dsp" },
  { "/Options/Sound card/dsp3",  NULL,    (void (*)())soundcard_cb, 3, "/Options/Sound card/dsp" },
  { "/Options/Sound card/dsp4",  NULL,    (void (*)())soundcard_cb, 4, "/Options/Sound card/dsp" },
//  { "/Options/Interface/eth1",  NULL,    NULL, 1, "<RadioItem>" },
//  { "/Options/Interface/eth2",  NULL,    NULL, 2, "/Options/Interface/eth1" },
  { "/_Help",         NULL,         NULL,           0, "<LastBranch>" },
  { "/_Help/About",   NULL,         about,           0, "<StockItem>", GTK_STOCK_ABOUT },

};

static gint default_nmenu_items = sizeof (default_menu_items) / sizeof (default_menu_items[0]);


void about(){
	GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(eve_global_main_window), 
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO,
			GTK_BUTTONS_CLOSE,
			"Eavesdropping demonstration application.\n\nTHIS APPLICATION MUST ONLY BE USED\nFOR DEMONSTRATION PURPOSES\n\nBe safe - run minisip --Erik Eliasson"
			);

	g_signal_connect_swapped (dialog, "response",
			G_CALLBACK (gtk_widget_destroy),
			dialog);
	gtk_widget_show(dialog);
}



GtkWidget *EveGui::createSignallingList(){
	GtkWidget *scrolled_window;
	GtkListStore *model;
	GtkCellRenderer *cell;
	GtkTreeViewColumn *column;
	GtkWidget *view;

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
			GTK_POLICY_AUTOMATIC, 
			GTK_POLICY_AUTOMATIC);

	model = gtk_list_store_new (1, G_TYPE_STRING);
	view = gtk_tree_view_new ();

	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), 
			view);
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL (model));
	gtk_widget_show (view);

   

	cell = gtk_cell_renderer_text_new ();

	column = gtk_tree_view_column_new_with_attributes ("Messages",
			cell,
			"text", 0,
			NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW (view),
			GTK_TREE_VIEW_COLUMN (column));


	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_NONE);


	signalView = view;

	return scrolled_window;

}


enum
{
  COL_PLAY = 0,
  COL_NAME,
  NUM_COLS
} ;


static void toggle_handler( GtkCellRendererToggle *cell, gchar *path_str, GtkWidget *view){
        gboolean value;
        GtkTreeIter iter;
        GtkTreePath *path = gtk_tree_path_new_from_string( path_str );
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));

        if( gtk_tree_model_get_iter( model, &iter, path ) ){
                gtk_tree_model_get( model, &iter, COL_PLAY, &value, -1 );
                value = !value;
                gtk_list_store_set( GTK_LIST_STORE( model ), &iter, COL_PLAY, value, -1 );
        }
        gtk_tree_path_free( path );
}


GtkWidget *EveGui::createPlayList(){
	GtkTreeViewColumn   *col;
	GtkCellRenderer     *renderer;
	GtkWidget           *view;
	GtkWidget 	    *scrolled_window;

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
			GTK_POLICY_AUTOMATIC, 
			GTK_POLICY_AUTOMATIC);
	
	view = gtk_tree_view_new ();

	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), 
			view);
	
	renderer = gtk_cell_renderer_toggle_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
			-1,
			"Play",
			renderer,
			"active", COL_PLAY,
			NULL);

	g_signal_connect( renderer, "toggled", G_CALLBACK(toggle_handler), view);

	col = gtk_tree_view_column_new();
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
			-1,
			"Stream",
			renderer,
			"text", COL_NAME,
			NULL);


	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_NONE);


	GtkListStore *model = gtk_list_store_new (NUM_COLS, G_TYPE_BOOLEAN, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(model));

/*
	GtkTreeIter iter;
	gtk_list_store_append (model, &iter);
	gtk_list_store_set (model, &iter,
			COL_PLAY, false,
			COL_NAME, "Erik",
			-1);
*/
	g_object_unref (model);

	gtk_widget_show (view);
	playView = view;
	return scrolled_window;
}



GtkWidget *EveGui::create_menubar_menu( GtkWidget  *window ) {
//	GtkItemFactory *item_factory;
	GtkAccelGroup *accel_group;
	accel_group = gtk_accel_group_new ();
	item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>",
			accel_group);

	all_interfaces = NetworkFunctions::getAllInterfaces();
	setInterface(0);
	int n_if = all_interfaces.size();

	GtkItemFactoryEntry *menu_items = new GtkItemFactoryEntry[default_nmenu_items + n_if];
	memcpy(menu_items, default_menu_items, sizeof(default_menu_items));
	string firstInterfacePath;
	for (int i=0; i<n_if; i++){
		menu_items[default_nmenu_items+i].path = strdup((gchar*)("/Options/Interface/"+all_interfaces[i]).c_str());
		menu_items[default_nmenu_items+i].accelerator = NULL;
		menu_items[default_nmenu_items+i].callback = (void (*)())interface_cb;
		menu_items[default_nmenu_items+i].callback_action = i;
		if (i==0){
			menu_items[default_nmenu_items+i].item_type = "<RadioItem>";
			firstInterfacePath = menu_items[default_nmenu_items+i].path;
		}else{
			menu_items[default_nmenu_items+i].item_type = strdup((gchar*)firstInterfacePath.c_str());
		}
	}


	gtk_item_factory_create_items (item_factory, default_nmenu_items+n_if, menu_items, NULL);
	gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);
	return gtk_item_factory_get_widget (item_factory, "<main>");
}


gboolean timeout_callback(gpointer callback_data){
//	cerr << "Callback running"<< endl;
	void *arg = callback_data;
	EveGui *g = (EveGui*)arg;
	g->timeout();
	return true;
}


EveGui::EveGui(int argc, char **argv):started(false){
	globalEve = this;		//ouch - late night hack
	gtk_init(&argc, &argv);

	eve_global_main_window = window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window, 400,300);
	gtk_window_set_title(GTK_WINDOW(window), "Eve (stopped)");


	g_signal_connect (window, "delete_event", gtk_main_quit, NULL);

	GtkWidget *main_vbox;
	GtkWidget *menubar;
	main_vbox = gtk_vbox_new (FALSE, 1);
	gtk_widget_show(main_vbox);
	
	menubar = create_menubar_menu (window);
	GtkWidget *mitem = gtk_item_factory_get_widget(item_factory,"<main>/File/Stop");
	assert(mitem!=NULL);
	gtk_widget_set_sensitive(mitem,FALSE);
	
	splitPane = gtk_vpaned_new();

	gtk_container_add (GTK_CONTAINER (window), main_vbox);
	gtk_box_pack_start(GTK_BOX (main_vbox), menubar, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX (main_vbox), splitPane, TRUE, TRUE, 0);
	//gtk_container_add (GTK_CONTAINER (main_vbox), splitPane);
	gtk_widget_show(menubar);

	gtk_paned_set_position(GTK_PANED(splitPane),120);
	gtk_widget_show (splitPane);

	GtkWidget *signalViewScroll= createSignallingList();
	gtk_paned_add1 (GTK_PANED (splitPane), signalViewScroll);
	gtk_widget_show (signalViewScroll);

	playScroll= createPlayList();
	gtk_paned_add2 (GTK_PANED (splitPane), playScroll);
	gtk_widget_show(playScroll);
	
	gtk_widget_show(window);

	statusbar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX (main_vbox), statusbar, FALSE, TRUE, 0);
	gtk_widget_show(statusbar);
	
//	guint e = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "EE2");

//	guint m = gtk_statusbar_push(GTK_STATUSBAR(statusbar),e,"this is a statusbar message");

	
	
	g_timeout_add (500, timeout_callback, (gpointer)this);
}


void EveGui::run(){
	gtk_main();
}



bool EveGui::selectedForPlaying(string from, string to){
	return false;
}

void EveGui::addSignallingMessage(string str){
	CommandString cmd("","signal_add",str);
	lock.lock();
	cmdlist.push_front(cmd);
	lock.unlock();

 }

void EveGui::addStream(string from, string to){
	CommandString cmd("","play_add");
	cmd["from"]=from;
	cmd["to"]= to;
	lock.lock();
	cmdlist.push_front(cmd);
	lock.unlock();
}


void EveGui::removeStream(string from, string to){
	CommandString cmd("","play_remove");
	cmd["from"]=from;
	cmd["to"]= to;
	lock.lock();
	cmdlist.push_front(cmd);
	lock.unlock();

}


void EveGui::timeout(){
	CommandString command("","");
	bool done=true;
	lock.lock();
	if (cmdlist.size()>0){
		command = cmdlist.pop_back();
		done=false;
	}
	lock.unlock();

	if (!done){
		string s = command["from"]+"->"+command["to"];
		if( command.getOp() == "play_add" ){
			GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(playView));
			GtkTreeIter iter;
			gtk_list_store_append (GTK_LIST_STORE(model), &iter);
			gtk_list_store_set (GTK_LIST_STORE(model), &iter,
					COL_PLAY, false,
					COL_NAME, s.c_str(),
					-1);
			//g_object_unref (model);
		}
		if( command.getOp() == "play_remove" ){
			string match_str= command["from"]+"->"+command["to"];
			GtkTreeIter iter;
			GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(playView));
			bool valid=gtk_tree_model_get_iter_first (model, &iter);
			while (valid){
				gchar *from_to;
				gtk_tree_model_get(model,&iter, COL_NAME,&from_to,-1);
				if (match_str==from_to){
					gtk_list_store_remove(GTK_LIST_STORE(model),&iter);
				}else{
				}
				g_free (from_to);
				valid = gtk_tree_model_iter_next (model, &iter);
			
			}

			for (int i=0; i< playing.size(); i++){
				if (playing[i]==match_str){
					playing.remove(i);
				}
			}
		}
		if (command.getOp()=="signal_add"){
			GtkTreeIter iter;
			GtkTreeModel *model;

			model = gtk_tree_view_get_model(GTK_TREE_VIEW(signalView));

			gtk_list_store_append (GTK_LIST_STORE (model), &iter);
			gtk_list_store_set (GTK_LIST_STORE (model), 
					&iter,
					0, command.getParam().c_str(),
					-1);

		}
	}

	if (!done){
		timeout();
	}
}


