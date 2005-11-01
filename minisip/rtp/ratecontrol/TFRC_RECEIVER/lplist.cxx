// TFRC LOSS PACKET LIST


#include <iostream>
#include <cstdlib>
#include "lplist.h"
using namespace std;

int lplist::set_values (int sizep) {
	lstptr=new short[sizep];
	elems=sizep;
}

lplist::lplist () {
}

lplist::~lplist () {
	delete[] lstptr;
}

int lplist::set_lsn (short lsnp, int indexp) {
	//*(lstptr+indexp)=lsnp;
	lstptr[indexp]=lsnp;
	return (0);
}

short lplist::get_lsn (int indexp) {
	return (lstptr[indexp]);
}

int lplist::clear_lst (void) {
	delete[] lstptr;
}
