// TFRC LOSS EVENT RATE DETERMINATION

#include <iostream>
#include <cstdlib>
#include "pcalc.h"
using namespace std;


pcalc::pcalc () {
	prevp=0;
}

pcalc::~pcalc () {
}

int pcalc::interpol (short snbef, double tibef, short snaft, double tiaft, short snl) {
	ltime=tibef+(((tiaft-tibef)/(snaft-snbef))*(snl-snbef));
	return (0);
	}


int pcalc::isp (void) {
	//int retval=2;
	for (int i=0; i<(llptr->elems); i++) {
		int snl;
		short snbef, snaft;
		double tibef, tiaft;
		float rtt;
		snl=(llptr->get_lsn(i));
		snbef=(phptr->get_pack(1)).seqnum;
		tibef=(phptr->get_pack(1)).arrtime;
		snaft=(phptr->get_pack(0)).seqnum;
		tiaft=(phptr->get_pack(0)).arrtime;
		rtt=((phptr->get_pack(0)).rtt); //remember that the round trip time is in milliseconds
		interpol(snbef, tibef, snaft, tiaft, snl);
		//cout << "Interpoled lost time for " << snl << " is " << ltime << "\n";
		//cout << "the rtt is: " << rtt << "\n";
		//cout << "It has passed " << ltime-prevpti << " milliseconds after the last loss event \n";
		if ((prevpti+rtt) < ltime) {
			calc_lint(snl);
			cout << "New loss event begins on sequence number: " << snl << "\n";
			rxra->calc_recvrate();
			//return (1);
			//retval=1
		}
		else {
			cout << "the lost packet " << snl << " belongs to the same loss event\n";
			//return(0);
		}
	}
	//return retval
}

int pcalc::calc_lint (short snlp) {
	lint=snlp-prevpsn;
	//cout << "The lost interval is: " << lint << "\n";
	prevpsn=snlp;
	prevpti=ltime;
	lintl.set_intv(lint);
	calc_lrate();
	//return (0);
}

int pcalc::calc_avglint (void) {
	int n=7, i=0;
	float weight[]={1, 1, 1, 1, 0.8, 0.6, 0.4, 0.2};
	float i0=0, i1=0, wt=0, wt1=0;
	for (i; i<=n; i++) {
		if (lintl.lilst[i] != 0) {
		wt=wt+weight[i];
		}
		if (lintl.lilst[i+1] != 0) {
		wt1=wt1+weight[i];
		}
		i0=i0+(lintl.lilst[i]*weight[i]);
		i1=i1+(lintl.lilst[i+1]*weight[i]);
	}
	//ii=max(i0,i1);
	//avgint=ii/wt;
	if (i1 == 0 ) {
	avgint=i0/wt;
	}
	else {
	avgint=max(i0/wt, i1/wt1);
	}
	//cout << "the average loss interval is: " << avgint << "\n";
	return (0);
}

int pcalc::calc_lrate(void) {
	calc_avglint();
	prevp=p;
	p=1/avgint;
	//cout << "the lost event rate is: " << p << "\n";
	return (0);
}

int pcalc::check_p (void) {
	cout << "the previous lost event rate is: " << prevp << "\n";
	cout << "the lost event rate is: " << p << "\n";
	if (p > prevp) {
		return (1);
	}
	else {
		return (0);
	}
}

int pcalc::set_llptr (lplist* llptrp) {
	llptr=llptrp;
	return (0);
}

int pcalc::set_p (float pp){
	p=pp;
	return (0);
}

int pcalc::set_packhist (packhist* phptrp) {
	phptr=phptrp;
	return (0);
}

float pcalc::calcx (float tp, float xrecp) {
	float xt;
	float r;
	r=((phptr->get_pack(0)).rtt)/1000.0;
	xt=(s/(r*(sqrt(2*tp/3)+(12*tp*sqrt(3*tp/8)*(1+32*pow(tp,2))))))-xrecp;
	return(xt);
	}

int pcalc::inli (void) {
	float p0=0.001, p1=0.999, err=0.0001, max=100, rate0, rate1, ratem, pm;
	int inlint, c=1;
	float xrec;
	rxra->calc_recvrate();
	//xrec=(rxra->recvrate)/1000.0;
	xrec=(rxra->recvrate);
	rate0=calcx(p0, xrec);
	rate1=calcx(p1, xrec);
	if ((rate0*rate1)>0) {
		cout << "X received has not a matching loss event rate setting p to default value\n";
		p1=0.20;
	}
	else {
	while ((c<max) && (fabs((p1-p0)/p1)> err)) {
//		cout << "Iteracion número: " << c << "\n";
//		cout << "la diferencia y : " << p1 << "\n";
		pm=p0+(p1-p0)/2;
		ratem=calcx(pm, xrec);
		if (rate0*ratem<0)
			p1=pm;
		else
		p0=pm;
		c++;
	}
	}
	//cout << "The round trip time for the initialization of p is " << (phptr->get_pack(0)).rtt << "\n";
	//cout << "el valor de loss event rate obtenido para: " << xrec << " es " << p0 << "\n";
	p=p0;
	inlint=(int) (1/p0); //verify that the casting works properly
	//cout << "el valor de loss interval inicial para: " << xrec << " es " << inlint << "\n";
	prevpsn=(llptr->get_lsn(0)); 
	prevpti=(phptr->get_pack(0)).arrtime; // modify for accuracy
	lintl.set_intv(inlint);
	return (0);
	
}


int pcalc::set_rxrate(rxrate* rxrap) {
	rxra=rxrap;
	return (0);
}

int pcalc::set_prevals(short prevpsnp, double prevptip) {
	prevpsn=prevpsnp;
	prevpti=prevptip;
	return (0);
}
