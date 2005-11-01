#include "RecvBwMngr.h"

RecvBwMngr::RecvBwMngr(/*char *remoteipp*/) : fbt(&ph), recrat(&lpi, &ph), fbi(&ph, &pc, &recrat), rxs(&fbi) {
//  remoteip=remoteipp; 
  pc.set_packhist(&ph);
  pc.set_rxrate(&recrat);
  firstpack=1;
}

RecvBwMngr::~RecvBwMngr() {
}

int RecvBwMngr::set_tfrc_vals(short seqnumb,unsigned long senderts, float rttv){
  
   rxclock clk;
   double arrtif;
   arrtif=clk.get_timems();
  

  //Basic Variables
  float minx=2466;
  int esp,au3;
  unsigned long sendtsf;

  //Error check - make sure that the IP address is set.
  assert(remoteip.size()>0);
  
  //// Network variables
  nsend s2s((char*)remoteip.c_str(), 27001);
  
  sic.set_values(seqnumb, senderts, rttv, arrtif);

  if (firstpack==1) {
    sic.rtt=0; //first packet with invalid rtt information
    recrat.set_ival(sic.seqnum,sic.arrtime);
    pc.set_prevals(sic.seqnum, sic.arrtime);
    lpi.set_snexp((sic.seqnum)+1);
    fbt.set_to((sic.arrtime));
    ph.set_pack(sic);
    pc.set_p(0);
    recrat.calc_recvrate();
    rxs.sendfbi();
    s2s.senddata(rxs.fbdata, 16);
    fbt.resetto();
    firstpack=0;
    return(0);
  }
 	

  lpi.set_snarr(sic.seqnum);
  esp=lpi.isexp();
  
  if (esp == 1) { //if the arrived packet is the expected
    ph.set_pack(sic);
    pc.set_p(0);
    if (fbt.isittime()==1){ 
      recrat.calc_recvrate();
      rxs.sendfbi();
      s2s.senddata(rxs.fbdata, 16);
      fbt.resetto();
    }
  }
  else {
    if (esp == 2) { //if the arrived packet is not the expected
      ph.set_pack(sic);
      pc.set_llptr(lpi.get_llptr());
      if (lpi.isfirstl()==1) { //if it is the first lost packet
	pc.inli();
	recrat.calc_recvrate();
	rxs.sendfbi();
	s2s.senddata(rxs.fbdata, 16);
	fbt.resetto();
      }
      else { //if the lost packet is other than the first lost packet
	pc.isp();
	if (pc.check_p()==1) {
	  fbt.tonow();
	}
	if (fbt.isittime()==1){
	  recrat.calc_recvrate();
	  rxs.sendfbi();
	  s2s.senddata(rxs.fbdata, 16);
	  fbt.resetto();
	}
      }
      lpi.get_llptr()->clear_lst(); //clears the lost packet list
    }
  } 
  return (0);
}

void RecvBwMngr::rtpReceived(MRef<RtpPacket*> rtp, string fromIp, int fromPort){
	int seqno = rtp->getHeader().getSeqNo();
	uint32_t rtt_ms = rtp->getHeader().getRttEstimate();
	int sts = rtp->getHeader().getSendingTimestamp();
	
	remoteip = fromIp;
	
	set_tfrc_vals(seqno, sts, float(rtt_ms));
	
}




