
#include<config.h>
#include<libmsip/SipTimers.h>

#define _T1 500
#define _T4 5000


const int SipTimers::T1 = _T1;
const int SipTimers::T2 = 4000;
const int SipTimers::T4 = _T4;
const int SipTimers::timerA = _T1; //=T1
const int SipTimers::timerB = 64 * _T1;
const int SipTimers::timerC = 4*60*1000; //4min (should be >3min)
const int SipTimers::timerD = 60000; // 60s (should be >32s)
const int SipTimers::timerE = _T1;
const int SipTimers::timerF = 64 * _T1;
const int SipTimers::timerG = _T1;
const int SipTimers::timerH = 64*_T1;
const int SipTimers::timerI = _T4;
const int SipTimers::timerJ = 64*_T1;
const int SipTimers::timerK = _T4;

