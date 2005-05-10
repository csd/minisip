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

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

class LIBMSIP_API SipTimers{
	public:
		static void setT1(int t){A=E=G=T1=t; B=F=H=J=t*64; }
		static void setT2(int t){T2 = t;}
		static void setT4(int t){I=K=T4=t;}
		static void setA(int t){A = t;}
		static void setB(int t){B = t;}
		static void setC(int t){C = t;}
		static void setD(int t){D = t;}
		static void setE(int t){E = t;}
		static void setF(int t){F = t;}
		static void setG(int t){G = t;}
		static void setH(int t){H = t;}
		static void setI(int t){I = t;}
		static void setJ(int t){J = t;}	
		static void setK(int t){K = t;}
		static int getA(){return A;}
		static int getB(){return B;}
		static int getC(){return C;}
		static int getD(){return D;}
		static int getE(){return E;}
		static int getF(){return F;}
		static int getG(){return G;}
		static int getH(){return H;}
		static int getI(){return I;}
		static int getJ(){return J;}
		static int getK(){return K;}
		
	private:
		static int T1;
		static int T2;
		static int T4;
		static int A;
		static int B;
		static int C;
		static int D;
		static int E;
		static int F;
		static int G;
		static int H;
		static int I;
		static int J;
		static int K;

		

};

#endif
