// TFRC LOSS INTERVAL LIST

#ifndef LINTLST_H
#define LINTLST_H


class lintlst {
public:
	short lilst[9];
	lintlst();
	~lintlst();
	int set_intv (int intvp);
	int get_intv (int indexp);
	int shift (void);
	int initialize (void);
};

#endif
