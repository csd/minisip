#include "nrecv.h"


nrecv::nrecv (in_port_t loportp, int dlenp) {
  loport=loportp;
  dlen=dlenp;
  mesg=new char[dlenp];
  initns();
}

nrecv::~nrecv() {
  delete [] mesg;
}

void nrecv::initns () {
  bzero(&losock, sizeof(losock));
  bzero(&desock, sizeof(desock));
  losock.sin_family=AF_INET;
  losock.sin_port=htons(loport);
  losock.sin_addr.s_addr=htonl(INADDR_ANY);
  sockfd=socket(AF_INET, SOCK_DGRAM, 0);
  bind(sockfd, (const struct sockaddr *) &losock, sizeof(losock));
}

int nrecv::recvdata (double art) {
  int n=0;
  socklen_t len;
  len=sizeof(desock);
  while (n<dlen) {
  n=recvfrom(sockfd, mesg, dlen, 0, (struct sockaddr *)&desock, &len);
  }
  decdat(art);
  //printf("the number of bytes received is %i\n", n);
  //printf("mesgptr in recvdata is: %i\n", mesg);
  return(n);
}

void nrecv::decdat (double aart) {
  decod dec(mesg);
  //printf("mesgptr in decdat is: %i\n", mesg);
  if (dlen == 10) {
    dec.decsend();
    sic->set_values(dec.seqnum, dec.st_est, dec.rtt_rr, aart);
    //printf("el mensaje recibido es: %u , %u , %f \n\n", dec.seqnum, dec.st_est, dec.rtt_rr);
  }
  if (dlen == 16) {
    dec.decrx();
    con->set_values(dec.lossr, dec.rtt_rr, dec.delay, dec.st_est, aart);
    //printf ("\nthe message received is: %u , %u , %f, %f \n\n", dec.st_est, dec.delay, dec.rtt_rr, dec.lossr);
  }
}

void nrecv::set_scont(secont *sicp) {
  sic=sicp;
}

void nrecv::set_rcont(fbcont *conp){
  con=conp;
}

int nrecv::get_sockfd () {
  return(sockfd);
}

struct sockaddr_in nrecv::get_desock() {
  return(desock);
}
