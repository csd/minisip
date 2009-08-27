
#include"Animate.h"
#include<libmutil/mtime.h>
#include<libmutil/merror.h>
#include<libmutil/massert.h>
#include<iostream>
#include<math.h>

using namespace std;

Animate::Animate(int _duration, float _startval, float _endval, const int _type){
	massert(_duration>0);
	start_time=-1;
	end_time=-1;

	duration=_duration;
	
	startval=_startval;
	endval=_endval;
	type=_type;
	continuous=false;
	delta=0.0F;

}

Animate::Animate(float fixedval){
	startval=endval=fixedval;
	start_time=-1;
	end_time=-1;

	duration=-1;
	
	type=ANIMATE_CONSTANT;
	continuous=true;
	delta=0;

}

Animate::Animate(float _startval, float _increase_per_s){
	start_time=-1;
	end_time=-1;

	duration=-1;
	
	startval=_startval;
	endval=-1;
	type=ANIMATE_LINEAR;
	continuous=true;
	delta=_increase_per_s;

}



void Animate::start(){
	start_time=timeMs();
	if (!continuous)
		end_time=start_time+duration;
}

#define PI 3.1415926535
float Animate::getVal(){
	long long now = timeMs();
	int ms = now-start_time;

//	cerr <<"Animate::getVal: ms="<<ms<<endl;
//	cerr << "Continuous="<<continuous<<endl;
	
	float p, sinscale;
	switch(type){
		case ANIMATE_CONSTANT:
			return endval;
		case ANIMATE_LINEAR:
			if (continuous){
				return startval + (float)ms/1000.0F * delta;
			}else{
				if (ms>=duration)
					return endval;
				p = (float)ms/(float)duration;
				return startval + p*(endval-startval);
			}
			break;
		case ANIMATE_STARTSTOP:
			{
				if (ms>=duration)
					return endval;
				p = (float)ms/(float)duration;
				p = p*PI;
				sinscale = (sin(p-PI/2.0F)+1.0F)/2.0F; // in 0..1 when p=0..pi range
				return startval + sinscale*(endval-startval);
			}
		case ANIMATE_EXPONENTIAL:
			{
				if (ms>=duration)
					return endval;
				p = (float)ms/(float)duration;
				return startval + p*p*(endval-startval);
			}

		default:
			merror("Unimplemented animation type");
			break;

	};
	merror("Animate: but in Animate::getVal - could not calculate animation");
	return startval;

}

bool Animate::ended(){
	if (continuous)
		return false;

	long long now = timeMs();
	if (now>=end_time){
		return true;
	}else{
		return false;
	}
}

long long Animate::timeMs(){
	return mtime();
}


