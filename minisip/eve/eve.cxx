/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include"EveGui.h"

int  main(int argc, char **argv){
	EveGui eve_gui(argc,argv);
	eve_gui.run();
	return 0;
}

