/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Numerical Integrators Definitions (Part 
of GiPSi Computational Toolset) (GiPSiIntegrators.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	GiPSiIntegrators.h v 1.0
////
////    Part of GiPSi Computational Toolset
////
////	1)	Defines Basic Explicit Integrators
////
////////////////////////////////////////////////////////////////

#ifndef _GiPSiINTEGRATORS_H
#define _GiPSiINTEGRATORS_H

//#include "GiPSiAPI.h"


////////////////////////////////////////////////////////////////
//
//	Euler
//
//		The simple (quite unstable) explicit Euler:
// 
//		x(t+h) = x(t) + h * f(t, x(t));
//
template <class S>
class Euler : public Integrator<S> {
public:

	Euler(S &system) { 
		system.AllocState(f); 
	}

  void						Integrate(S &system, Real h);

protected:
	
	State						f;
  
};


template <class S>
void Euler<S>::Integrate(S &system, Real h)
{
  State &state = system.GetState();
  
  // calculate f(t, x(t))
  system.DerivState(f, state);

  // x = x + h * f;
  system.AccumState(state, state, f, h); 
  
}




////////////////////////////////////////////////////////////////
//
//	Mid Point
//
//		Explicit Mid Point method:
// 
//		f1 = f(t, x(t));
//		x2 = x(t) + h/2 * f1;
//		f2 = f(t + h/2, x(t+h/2));
//		x(t+h) = x(t) + h * f2;
//
template <class S>
class ERKMid2 : public Integrator<S> {
public:

	ERKMid2(S &system) { 
		system.AllocState(f1);
		system.AllocState(f2);
		system.AllocState(tmp_state);
	}

	void					Integrate(S &system, Real h);

protected:

	State						f1, f2;
	State						tmp_state;

};

template <class S>
inline void ERKMid2<S>::Integrate(S &system, Real h)
{
	State &state = system.GetState();
  
	// f1 = f(x)
	system.DerivState(f1, state);

	// x2 = x + h/2 * f1
	system.AccumState(tmp_state, state, f1, h/2.0);

	// f2 = f(x2)
	system.DerivState(f2, tmp_state);

	// x = x + h * f2
	system.AccumState(state, state, f2, h);
}




////////////////////////////////////////////////////////////////
//
//	Heun3   
//
//		Explicit Heun method	(stage = 3, order = 3)
//   
//		f1 = f(t      , x(t));
//		x2 = x(t) + h*(1/3 * f1);
//		f2 = f(t+h/3  , x2);
//		x3 = x(t) + h*(2/3 * f2);
//		f3 = f(t+2*h/3, x3);
//		x(t+h) = x(t) + h/4 * (f1 + 3*f3);
//
template <class S>
class ERKHeun3 : public Integrator<S> {
public:

	ERKHeun3(S &system) { 
		system.AllocState(f1);
		system.AllocState(f2);
		system.AllocState(f3);
		system.AllocState(tmp_state);
	}

	void				Integrate(S &system, Real h);

protected:

	State					f1, f2, f3;
	State					tmp_state;

};


template <class S>
inline	void ERKHeun3<S>::Integrate(S &system, Real h)
{
	State &state = system.GetState();

	// f1 = f(x)
	system.DerivState(f1, state);

	// x2 = x + h*1/3 * d1
	system.AccumState(tmp_state, state, f1, h*1.0/3.0);

	// f2 = f(x2)
	system.DerivState(f2, tmp_state);

	// x3 = x + h*2/3 * d2
	system.AccumState(tmp_state, state, f2, h*2.0/3.0);

	// f3 = f(x3)
	system.DerivState(f3, tmp_state);

	// x = x + h/4 * (f1 + 3*f3)
	system.AccumState(state, state, f1,	h/4.0);
	system.AccumState(state, state, f3,	3.0*h/4.0);
}



////////////////////////////////////////////////////////////////
//
//	Runga-Kutta4   
//
//		Explicit Runga Kutta (order = 4)
//   
//		f1 = f(t, x(t));
//		x2 = x(t) + h/2 * f1;
//		f2 = f(t+h/2, x2);
//		x3 = x(t) + h/2 * f2;
//		f3 = f(t+h/2, x3);
//		x4 = x(t) + h * f3;
//		f4 = f(t+h, x4);
//		x(t+h) = x(t) + h/6 * (f1 + 2*f2 + 2*f3 + f4);
//
template <class S>
class ERK4 : public Integrator<S> {
public:

  ERK4(S &system) { 
		system.AllocState(f1);
		system.AllocState(f2);
		system.AllocState(f3);
		system.AllocState(f4);
		system.AllocState(tmp_state);
	}
  
  void					Integrate(S &system, Real h);

protected:
	
  State						f1, f2, f3, f4;
  State						tmp_state;

};


template <class S>
void ERK4<S>::Integrate(S &system, Real h)
{
	State &state = system.GetState();

	// f1 = f(x)
	system.DerivState(f1, state);

	// x2 = x + h/2 * f1
	system.AccumState(tmp_state, state, f1, h/2.0);

	// f2 = f(x2)
	system.DerivState(f2, tmp_state);

	// x3 = x + h/2 * f2
	system.AccumState(tmp_state, state, f2, h/2.0);

	// f3 = f(x3)
	system.DerivState(f3, tmp_state);

	// x4 = x + h * f3
	system.AccumState(tmp_state, state, f3 h);

	// f4 = f(x4)
	system.DerivState(f4, tmp_state);

	// x = x + h/6 * (f1 + 2*f2 + 2*f3 + f4)
	system.AccumState(state, state, f1, h/6.0);
	system.AccumState(state, state, f2, 2.0*h/6.0);
	system.AccumState(state, state, f3, 2.0*h/6.0);
	system.AccumState(state, state, f4, h/6.0);
}



// NOTE: Everything below is incomplete!!

template <class S>
class ERK3 : public Integrator<S> {
public:

	void				Integrate(S &system, Real h);

protected:
	State					deriv, deriv2, deriv3;
	State					tmp_state;
};


template <class S>
class AB2 : public Integrator<S> {
public:

  AB2() : iter(0), first(0), last(1), lastSize(0) { }

  void Integrate(S &system, Real h);

protected:

  State deriv[2], tderiv2, tderiv3;
  State tmp_state;
  int iter, first, last, lastSize;

};


template <class S>
class AB3 : public Integrator<S> {
public:

  AB3() : iter(0), first(0), lastSize(0) { }
  
  void Integrate(S &system, Real h);

protected:
	
  State deriv[3], tderiv2, tderiv3, tderiv4;
  State tmp_state;
  int iter, first, lastSize;

};


template <class S>
class AB4 : public Integrator<S> {
public:
  
  AB4() : iter(0), first(0), lastSize(0) { }

  void Integrate(S &system, Real h);

protected:

  State deriv[4], tderiv2, tderiv3, tderiv4;
  State tmp_state;
  int iter, first, lastSize;

};


template <class S>
class ABAM3 : public Integrator<S> {
public:

  ABAM3() : iter(0), first(0), lastSize(0) { }
  
  void Integrate(S &system, Real h);

protected:

  State deriv[3], tderiv2, tderiv3, tderiv4, fderiv;
  State tmp_state;
  int iter, first, lastSize;

};


template <class S>
class ABAM3_PEC2E : public Integrator<S> {
public:

  ABAM3_PEC2E() : iter(0), first(0), lastSize(0) { }
  void Integrate(S &system, Real h);

protected:

  State deriv[3], tderiv2, tderiv3, tderiv4, fderiv;
  State tmp_state, tmp_state2;
  int iter, first, lastSize;

};


template <class S>
class ABAM4 : public Integrator<S> {
public:

  ABAM4() : iter(0), first(0), lastSize(0) { }  
  void Integrate(S &system, Real h);

protected:
	
  State deriv[4], tderiv2, tderiv3, tderiv4, fderiv;
  State tmp_state;
  int iter, first, lastSize;

};

template <class S>
class RKN : public Integrator<S> {
public:
  
  void Integrate(S &system, Real h);

protected:

  State k1, k2, k3, k4, tmp;
  State tmp_state;

};


#endif // INTEGRATOR_H



/* 
   Runge-Kutta 2

   Midpoint
   
   x(t+h) = x(t) + h * f( t + h/2, x(t) + h/2 * f( t, x(t) )

   or

   F1 = f(t, x(t));
   F2 = f(t + h/2, x(t) + h/2 * F1);
   x(t+h) = x(t) + h * F2;

   or

   F1 = f(t, x(t));
     x2 = x(t) + h/2 * F1;
   F2 = f(t + h/2, x2);
   x(t+h) = x(t) + h * F2;



   Modified Euler Method

   x(t+h) = x(t) + h/2 * [ f( t, x(t) ) + f( t+h, w(t) + h * f( t, x(t) ) ) ]

   or

   F1 = f(t, x(t));
   F2 = f(t+h, x(t) + h * F1);
   x(t+h) = x(t) + h/2 * (F1 + F2);

   or
   
   F1 = f(t, x(t));
     x2 = x(t) + h * F1;
   F2 = f(t+h, x2);
   x(t+h) = x(t) + h/2 * (F1 + F2);
   

   Heun's Method

   x(t+h) = x(t) + h/4 * [ f( t, x(t) ) + 3 * f( t+h*2/3, x(t) + h*2/3 * f( t, x(t) ) ) ];

   or

   F1 = f(t, x(t));
   F2 = f(t+h*2/3, x(t) + h*2/3 * F1);
   x(t+h) = x(t) + h/4 * (F1 + 3 * F2);

   or

   F1 = f(t, x(t));
     x2 = x(t) + h*2/3 * F1;
   F2 = f(t+h*2/3, x2);
   x(t+h) = x(t) + h/4 * (F1 + 3 * F2);
*/

/* Runge-Kutta 3

   Heun3   (stage = 3, order = 3)
   
   F1 = f(t      , x(t));
   F2 = f(t+h/3  , x(t) + h*(1/3 * F1));
   F3 = f(t+2*h/3, x(t) + h*(2/3 * F2));
   x(t+h) = x(t) + h/4 * (F1 + 3*F3);
   
   or

   F1 = f(t      , x(t));
     x2 = x(t) + h*(1/3 * F1);
   F2 = f(t+h/3  , x2);
     x3 = x(t) + h*(2/3 * F2);
   F3 = f(t+2*h/3, x3);
   x(t+h) = x(t) + h/4 * (F1 + 3*F3);
   


   Kutta3   (stage = 3, order = 3)
  
   F1 = f(t    , x(t));
   F2 = f(t+h/2, x(t) + h * (1/2 * F));
   F3 = f(t+h  , x(t) + h * (-F1 + 2 *F2));
   x(t+h) = x(t) + h/6 * (F1 + 4*F2 + F3);

   or

   F1 = f(t    , x(t));
     x2 = x(t) + h * (1/2 * F);
   F2 = f(t+h/2, x2);
     x3 = x(t) + h * (-F1 + 2 *F2)
   F3 = f(t+h  , x3);
   x(t+h) = x(t) + h/6 * (F1 + 4*F2 + F3);

*/

/*
  Runge-Kutta 4
  
  F1 = f(t    , x(t));
  F2 = f(t+h/2, x(t) + h/2 * F1);
  F3 = f(t+h/2, x(t) + h/2 * F2);
  F4 = f(t+h  , x(t) + h   * F3);
  x(t+h) = x(t) + h/6 * (F1 + F2 + F3 + F4);

  or

  F1 = f(t, x(t));
    x2 = x(t) + h/2 * F1;
  F2 = f(t+h/2, x2);
    x3 = x(t) + h/2 * F2;
  F3 = f(t+h/2, x3);
    x4 = x(t) + h   * F3;
  F4 = f(t+h  , x4);
  x(t+h) = x(t) + h/6 * (F1 + 2*F2 + 2*F3 + F4);
 */
