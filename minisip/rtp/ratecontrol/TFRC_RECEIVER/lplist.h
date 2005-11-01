// TRFC LOSS PACKET LIST


#ifndef LPLIST_H
#define LPLIST_H

class lplist {
public:
	int elems;
	short * lstptr;
	//lplist(int sizep);
	lplist();
	~lplist();
	int set_lsn(short lsnp, int indexp);
	short get_lsn (int indexp);
	int set_values(int sizep);
	int clear_lst (void);
};

#endif


