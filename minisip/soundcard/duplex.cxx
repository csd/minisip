/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

/*
 * Playing and recording at the same time. Input from mic is sent to speakers.
*/

//#define HAVE_LIBASOUND

#include"SoundIO.h"
#include"OssSoundDevice.h"
#include"AlsaSoundDevice.h"
#include<unistd.h>
#include<math.h>
#include"SoundRecorderCallback.h"

#define SAMPLES_READ 160

#define SD_LIMIT_ON 15
#define SD_LIMIT_OFF 7

#define MAX_NOICE_TABLE 300


SoundIO *scp;
int dropper=0;

float roundf(float x);

int iabs(int i){
	if (i<0)
		return -i;
	return i;
}

int energy(short *buf, int n){
	long long ret=0;
	for (int i=0; i< n; i++)
		ret+=iabs(buf[i]);
	return ret/n;
}

class dummy: public SoundIOPLCInterface{
	public:
		virtual short *get_plc_sound(int &ret_size){
			cerr << "X"<< endl;
			for (int i=0; i<SAMPLES_READ; i++)
				sound[i]=0;
			ret_size=SAMPLES_READ;
			return &sound[0];       

		}
	private:
		short sound[160];

};


class Sampler : public SoundRecorderCallback{
	public:
		Sampler():index(0){
			for (int i=0; i<MAX_NOICE_TABLE; i++)
				noice_table[i]=0;
		}

		void srcb_handleSound(void *buf){
			short *data = (short *)buf;
                        cerr <<"."<< flush;
//			cerr.setf(ios::hex, ios::basefield);
//			for (int i=0; i< 160; i++){
//				if (i==100)
//					cerr << data[i]<< " ";
//
//			}
//			cerr.setf(ios::dec, ios::basefield);
//			cerr<< endl;
			//			if ( !(dropper++ % 50==1)){
			//				scp->push_sound(data);
			//			}
			int e = energy(data, SAMPLES_READ);
			if (e < MAX_NOICE_TABLE)
				noice_table[e]++;
			if (e>SD_LIMIT_ON && silence){
				silence = false;
				cerr<<"silence=off,i="<<index << endl;
			}
			if (e<= SD_LIMIT_OFF && !silence){
				silence = true;
				cerr <<"silence=on, i="<<index << endl;
			}
//			cerr << "Energy: "<< e << endl;
			if (silence)
				for (int ii=0; ii<160; ii++)
					data[ii]=0;
//			cerr << "before pushSound" << flush;
			scp->pushSound(34, data, SAMPLES_READ, index++, false);
//			cerr << "after pushSound" << flush;
			if (index %1000==0){
				for (int i=0; i<MAX_NOICE_TABLE; i++){
                                        cerr << i<<": ";
					for (int j=0; j<noice_table[i];j++)
						cerr << '*';
					cerr << endl;
//					cout<<i<< ":\t" << noice_table[i]<< endl;
				}
			}

		}

		virtual u_int32_t srcb_getSSRC(){return 33;}
		int index;
		int noice_table[MAX_NOICE_TABLE];
		bool silence;
		
};



int main(int argc, char **argv){
	Sampler sampler;
	if (argc!=2){
		cerr << "Usage: duplex <device>" << endl;
		exit(1);
	}

	SoundIO sc(new OssSoundDevice(argv[1]), 1, 8000);
	dummy d;
	scp = &sc;


	cerr << "Starting player"<< endl;
	sc.start_sound_player();
	sleep( 1 );
	sc.registerSource(34,NULL);
	cerr << "Starting recorder"<< endl;
	sc.start_recorder();
	
	cerr << "Registring receiver"<< endl;
        sc.register_recorder_receiver(&sampler, SAMPLES_READ, false);
	sleep(1);
	sc.startRecord();


	while (1)
		sleep(1);
	return 0;
}


