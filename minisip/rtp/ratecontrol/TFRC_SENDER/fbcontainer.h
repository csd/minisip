//TFRC CONTAINER FOR FEEDBACK INFORMATION

#ifndef FBCONTAINER_H
#define FBCONTAINER_H


class fbcont {
public:
		float p, xrec;
		unsigned long rxdelay, echots;
		double arrtime;
		//fbcont(float pp, float xrecp, unsigned long rxdelayp, unsigned long echotsp);
		fbcont();
		~fbcont();
		//int set_arrtime (unsigned long arrtimep);
		int set_values(float pp, float xrecp, unsigned long rxdelayp, unsigned long echotsp, double arrtimep);
};


#endif
