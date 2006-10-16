/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/



#ifndef _SIPTIMERS_H
#define _SIPTIMERS_H

#include<libmsip/libmsip_config.h>

#include<libmutil/MemObject.h>

class LIBMSIP_API SipTimers : public MObject{
	public:
		SipTimers();

		std::string getMemObjectType() const {return "SipTimers";}
		
		void setT1(int t){A=E=G=T1=t; B=F=H=J=t*64; }
		void setT2(int t){T2 = t;}
		void setT4(int t){I=K=T4=t;}
		void setA(int t){A = t;}
		void setB(int t){B = t;}
		void setC(int t){C = t;}
		void setD(int t){D = t;}
		void setE(int t){E = t;}
		void setF(int t){F = t;}
		void setG(int t){G = t;}
		void setH(int t){H = t;}
		void setI(int t){I = t;}
		void setJ(int t){J = t;}	
		void setK(int t){K = t;}
		int getT1(){return T1;}
		int getT2(){return T2;}
		int getT4(){return T4;}
		int getA(){return A;}
		int getB(){return B;}
		int getC(){return C;}
		int getD(){return D;}
		int getE(){return E;}
		int getF(){return F;}
		int getG(){return G;}
		int getH(){return H;}
		int getI(){return I;}
		int getJ(){return J;}
		int getK(){return K;}
		
	private:
		int T1;
		int T2;
		int T4;
		int A;
		int B;
		int C;
		int D;
		int E;
		int F;
		int G;
		int H;
		int I;
		int J;
		int K;

};

#endif
