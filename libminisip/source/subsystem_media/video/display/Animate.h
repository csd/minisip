#ifndef MANIMATE
#define MANIMATE

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

//Constant value (no animation)
#define ANIMATE_CONSTANT 1

//Fixed delta (constant speed/change)
#define ANIMATE_LINEAR 2


// y=x^2
//  |         .
//  |        .
//  |      .
//  |    .
//  |. 
//  +-----------
#define ANIMATE_EXPONENTIAL 3


// Accelerating at start of interval, and slowing to halt at end
// Based on the sinus curve.
//   |           . .
//   |        .
//   |      . 
//   |     .
//   |    . 
//   |. .
//   +----------------
#define ANIMATE_STARTSTOP 4

#define ANIMATE_SINUS 5


class Animate : public MObject{
	public:
		Animate(float fixedVal);
		Animate(int duration_ms, float startval, float endval, const int type);
		Animate(float startval, float increase_per_s);
	
		/**
 		 * Sets starting time to now if no argument is given, or sets it
 		 */
		void start(long long time=-1);

		long long getStartTime();

		float getVal();

		bool ended();

		int getDuration(){return duration;}

		int getType(){return type;}

	private:
		long long timeMs();

		float startval;
		int type;

		bool continuous;

		//for continuous animations
		float delta;

		//for non-continuous animations		
		int duration;
		long long start_time;
		long long end_time;
		float endval;

};

#endif
