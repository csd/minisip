
#include<libminisip/Minisip.h>
#include"gui/textui/MinisipTextUI.h"


int main( int , char ** ){
	
//	ContactDb *contactDb = new ContactDb();
//	ContactEntry::setDb(contactDb);
	
	MRef<MinisipTextUI*> gui = new MinisipTextUI();

//	cerr << "Setting contact db"<< endl;
//	gui->setContactDb(contactDb);
		
	
        Minisip minisip( *gui );
        minisip.startSip();

	gui->run();
        //minisip.runGui();
        //minisip.exit();
	return 0;
}

