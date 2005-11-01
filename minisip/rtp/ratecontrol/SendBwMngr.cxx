#include "SendBwMngr.h"

SendBwMngr::SendBwMngr () {
  ut.genrandn(ranum);
  ut.gentimebase(basetime);

}

SendBwMngr::~SendBwMngr () {
}


void SendBwMngr::set_rtp_header_v() {
  stsgen stsa(ranum,basetime);
  psts=stsa.get_sts();
}




void SendBwMngr::runcli() {

	//Basic vairabales with inital values
	int se, sspace=10, szise=1460;
	uint16_t seqno=0;
	double artt;
		
	
	//TFRC objects
	timep ti(ranum, basetime);
	fbcont con;
	rttcalc rttc(ranum, basetime);
	
	ratecalc rcal;
	stimer seout(szise,&con, &rcal, &rttc);
	
	//network objects
	nrecv rci(27001, 16);
	rci.set_rcont(&con);
	gather go(10);
	char * pspack;
	fd_set fds;
	timeval to;
	to.tv_sec=0;
	to.tv_usec=sspace*1000;
   	rttc.set_iniseqn((unsigned long)seqno);
	rttv=rttc.rtt_next;
	

	for (; ;) {
	  FD_ZERO(&fds);
	  FD_SET(rci.get_sockfd(),&fds);
	  se=select((rci.get_sockfd())+1, &fds, NULL, NULL, &to);
	  artt=ti.gentimems();
	  seqno++;
	  
	  
	  if (se==1){ //a feedback packet was received
	    rci.recvdata(artt);  //copy info from socket buffer to tfrc feedback container
	    rttc.set_iptrval(&con); //updates the values for the rtt calculation
	    rcal.set_values(&con, rttc.get_rttnext((unsigned long)seqno));//set inf calc rate
	    x=rcal.get_xnext(); //calc the fair rate
	    seout.resetto(); //reset the "no feedback timeout"
	  }
	  
	  if (se==0 & seout.istime()==1 ){ //feedback timeout over
	    x=rcal.get_coxnext(); //calculate the fair rate
	    seout.resetto(); //reset the feedback timeout
	  }
	  
	  rcal.Ceffx(); //calculates the efficient rate for voip
	  ex=rcal.effx;
	  //codec.set_target_bw(ex);
	  
	}
}

