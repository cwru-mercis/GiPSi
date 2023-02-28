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

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriya Natsupakpong.
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
	system.AccumState(tmp_state, state, f3, h);

	// f4 = f(x4)
	system.DerivState(f4, tmp_state);

	// x = x + h/6 * (f1 + 2*f2 + 2*f3 + f4)
	system.AccumState(state, state, f1, h/6.0);
	system.AccumState(state, state, f2, 2.0*h/6.0);
	system.AccumState(state, state, f3, 2.0*h/6.0);
	system.AccumState(state, state, f4, h/6.0);
}

////////////////////////////////////////////////////////////////
//
//	Semi-Explicit Euler
//
//		The semi explicit Euler:
//		calculate the next step of position by using the next step velocity
//
//		v(t+h) = v(t) + h * a(t, v(t));
//		p(t+h) = p(t) + h * v(t+h, p(t));
//
template <class S>
class SemiEuler : public Integrator<S> {
public:
	SemiEuler(S &system) { 
		system.AllocState(f); 
	}
	void					Integrate(S &system, Real h);
protected:
	State					f;
};

template <class S>
void SemiEuler<S>::Integrate(S &system, Real h)
{
  State &state = system.GetState();
  // calculate f(t, x(t))
  system.DerivState(f, state);
  // x = x + h * f;
  system.AccumStateSemiExplicit(state, state, f, h); 
}

////////////////////////////////////////////////////////////////
//
//	Implicit Integrator
//
//		The Implicit Integrator:
//		the base class for implicit integrator
//
template <class S>
class ImplicitIntegrator : public Integrator<S> {
public:
	ImplicitIntegrator():error(0.01),maxiter(10) {}
	typedef typename S::Jacobian	Jacobian;
	virtual void	Integrate(S &system, Real h) {}
	void			SetError(Real num) { error = num; }
protected:
	virtual void	Solver(S &system, State &state, Real h) {}
	void			CG(S &system, State &x, Jacobian A, State b, int max, Real error); 
	Real			error;
	int				maxiter;
};

template <class S>
void ImplicitIntegrator<S>::CG(S &system, State &x, Jacobian A, State b, int max, Real error) 
{	
	S::State xmin;
	S::State r, u, p, q, rt, vh, uh, ph, qh, qt, Ax, bt;

	system.AllocState(xmin);
	system.AllocState(r);
	system.AllocState(u);
	system.AllocState(p);
	system.AllocState(q);
	system.AllocState(rt);
	system.AllocState(vh);
	system.AllocState(uh);
	system.AllocState(ph);
	system.AllocState(qh);
	system.AllocState(qt);
	system.AllocState(Ax);
	system.AllocState(bt);
	
	int imin=0;
	int max_iter;
	double tolb;
	double rho, rho1, beta, alpha;
	double normr, normr_min, rtvh;
	
	if (max<10) max_iter = 10;
	else max_iter = max;

	// xmin = x;
	system.ScaleState(xmin, x, 1);
	imin = 0;
	
	//tolb = error*b.length();
	tolb = error*system.NormState(b);

	//r = b - A*x;
	system.MultiplyJacobianState(Ax, A, x);
	system.AddState(r, b, Ax, -1);
	
	//normr = r.length();
	normr = system.NormState(r);
	//rt = r;
	system.ScaleState(rt, r, 1);

	normr_min = normr;
	rho = 1;
	
	for(int i=1;i<=max_iter;i++) 
	{
		rho1 = rho;
		//rho = rt*r;
		rho = system.StateDotState(rt, r);
		if(i==1){
			//u = r;
			system.ScaleState(u, r, 1);
			//p = u;
			system.ScaleState(p, u, 1);
		}
		else {
			beta = rho/rho1;
			//u = r + q*beta;
			system.AddState(u, r, q, beta);
			//p = u + (q + p*beta)*beta;
			system.AddState(qt, q, p, beta);
			system.AddState(p, u, qt, beta);
		}
	
		//ph = p;
		system.ScaleState(ph, p, 1);
		//vh = A*ph;
		system.MultiplyJacobianState(vh, A, ph);
		//rtvh = rt*vh;
		rtvh = system.StateDotState(rt, vh);
		alpha = rho/rtvh;
		//q = u - vh*alpha;
		system.AddState(q, u, vh, -alpha);
		//uh = u + q;
		system.AddState(uh, u, q, 1);
		//x = x + uh*alpha; 
		system.AddState(x, x, uh, alpha);
		//normr = (b - A*x).length();		
		system.MultiplyJacobianState(Ax, A, x);
		system.AddState(bt, b, Ax, -1);
		normr = system.NormState(bt);

		if(normr<normr_min){
			normr_min = normr;
			//xmin = x;
			system.ScaleState(xmin, x, 1);
			imin = i;
		}
		//qh = A*uh;
		system.MultiplyJacobianState(qh, A, uh);
		//r = r - qh*alpha;
		system.AddState(r, r, qh, -alpha);
		if(normr<tolb) break;		
	}	
	//x = xmin;		
	system.ScaleState(x, xmin, 1);
}

////////////////////////////////////////////////////////////////
//
//	Implicit Euler
//
//		The Implicit Euler:
//		only use the CG to solve the system equation
//
template <class S>
class ImplicitEuler : public ImplicitIntegrator<S> {
public:
	ImplicitEuler(S &system) { 
		system.AllocState(f);
		system.AllocState(dstate);
		system.AllocState(B);
		system.AllocJacobian(J);
	}	
	void	Integrate(S &system, Real h);	
protected:
	void	Solver(S &system, State &state, Real h);
	Jacobian		J;
	State			f;
  	State			dstate;
	State			B;	
};


template <class S>
void ImplicitEuler<S>::Integrate(S &system, Real h)
{	
	State &state = system.GetState();	
	system.DerivState(f, state);	
	// build A = I-hJ  
	system.IdentityMinushJacobian(J, state, h);	
	// build B = hf
	system.ScaleState(B, f, h);	
	// solve dstate = inv(J) B
	Solver(system, dstate, h);	
	// x = x + dx
	system.AccumState(state, state, dstate, 1.0); 	
}

template <class S>
void ImplicitEuler<S>::Solver(S &system, State &state, Real h)
{
	// Conjugate Gradient
	CG(system, state, J, B, state.size, error);  
}


////////////////////////////////////////////////////////////////
//
//	Implicit Euler NT
//
//		The Implicit Euler with Newton method:
//		use newton method to find the optimal solution
//
/////// need to debug for NT loop//////////////////////////////
template <class S>
class ImplicitEulerNT : public ImplicitIntegrator<S> {
public:
	ImplicitEulerNT(S &system) { 
		system.AllocJacobian(J);  
     	system.AllocState(f);
		system.AllocState(dstate);		
		system.AllocState(Jdstate);	
		system.AllocState(tempstate);		
		system.AllocState(B);			

		system.AllocState(rstate);
		system.AllocState(Axstate);
		system.AllocState(sstate);
	}	
	void		Integrate(S &system, Real h);	
protected:
	void		Solver(S &system, State &state, Real h);
	void		Solver1(S &system, State &state, Real h);
	void		Solver2(S &system, State &state, Real h);
	Jacobian		J;
	State			f;
  	State			dstate;	  	
	State			Jdstate;	  
	State			tempstate;
  	State			B;		

	State			rstate;	  
	State			Axstate;
	State			sstate;
};

template <class S>
void ImplicitEulerNT<S>::Integrate(S &system, Real h)
{
	State &state = system.GetState();
	// set tempstate = state
	system.ScaleState(tempstate, state, 1);
	// solve
	Solver2(system, tempstate, h);	
	// set state = tempstate
	system.AccumState(state, tempstate, tempstate, 0.0); 
}

template <class S>
void ImplicitEulerNT<S>::Solver(S &system, State &state, Real h)
{
	Real lamda;
	int pass = 1;
	bool out = false;
	Real vBlength;
	Real vdXlength;
	unsigned int size = dstate.size;
	Vector<Real> zero_vector(size, 0.0);
	lamda = 1.0;

	// dstate = state - dstate
	system.AddState(dstate, state, state, -1);	
	
	system.DerivState(f, state);
	// build A = I-hJ  
	system.IdentityMinushJacobian(J, state, h);		
	// build B = hf		
	system.ScaleState(B, f, h);		

	// CG with Newton
	while(!out&(pass<maxiter)) {	

		if (pass>1) {
			//B = B – J*dstate
			system.MultiplyJacobianState(Axstate, J, dstate);
			system.AddState(B, B, Axstate, -1);
		}
		vBlength = system.NormState(B);
		if (vBlength<error) out = true;
		lamda *= 0.5;
		if (lamda<0.1) lamda = 0.1;
	    
		// use Conjugate Gradiant to find dstate
		if (!out)
			CG(system, dstate, J, B, size, error); 

		// update X 
		vdXlength = system.NormState(dstate);
		if (vdXlength<error) out = true;		

		//set state = state + lamda * dstate	
		system.AddState(state, state, dstate, lamda);		
		pass++;			
	}
	// End CG with Newton	
}

/* test with another newton implementation 2008/09/30 */
template <class S>
void ImplicitEulerNT<S>::Solver1(S &system, State &state, Real h)
{
	int pass = 0;
	bool out = false;
	Real vBlength;
	Real vdXlength;
	unsigned int size = state.size;
		
	// set dstate to zero
	system.AddState(dstate, state, state, -1);	

	system.DerivState(f, state);

	// build A = I-hJ  
	system.IdentityMinushJacobian(J, state, h);	
	// build B = hf		
	system.ScaleState(B, f, h);	

	// use Conjugate Gradiant to find dstate
	CG(system, dstate, J, B, size, error); 

	system.MultiplyJacobianState(Axstate, J, dstate);
	system.AddState(rstate, Axstate, B, -1.0);

	// Newton method
	while(!out&(pass<maxiter)) {

		if (!out)
			CG(system, sstate, J, rstate, size, error); 		
				
		vdXlength = system.NormState(sstate);
		if (vdXlength<error) out = true;                                                                                                                                                                                                          				
		
		system.AddState(dstate, dstate, sstate, 1.0);
		pass++;			
		
	}	
	// End CG with Newton	
	system.AddState(state, state, dstate, 1.0);
}

/* test with another newton implementation 2008/10/05 */
// got the same result as solver1
template <class S>
void ImplicitEulerNT<S>::Solver2(S &system, State &state, Real h)
{
	int pass = 0;
	bool out = false;
	Real vBlength;
	Real vdXlength;
	unsigned int size = state.size;
		
	// set dstate to zero
	system.AddState(dstate, state, state, -1.0);		

	// Newton method
	while(!out&(pass<maxiter)) {	
		
		// build B = hf - dstate		
		system.DerivState(f, state);
		system.ScaleState(B, f, h);
		system.AddState(B, B, dstate, -1.0);
		// build A = I-hJ  
		system.IdentityMinushJacobian(J, state, h);	
		
		if (!out)
			CG(system, dstate, J, B, size, error); 		
				
		vdXlength = system.NormState(dstate);
		if (vdXlength<error) out = true;                                                                                                                                                                                                          				
		
		system.AddState(state, state, dstate, 1.0);
		pass++;			
	}	
	// End CG with Newton	
}

////////////////////////////////////////////////////////////////
//
//	Implicit Midpoint
//
//		The Implicit Midpoint:
//		only use the CG to solve the system equation
//		(I - hJ/2)(y_n+1/2 - y_n) = h/2 f(t_n, y_n)
//		(I - hJ/2)(dy_n - dy_n-1/2) = h/2 f(t_n+1/2, y_n+1/2) - dy_n-1/2
//		dy_n = y_n+1 - y_n-1/2
//		dy_n-1/2 = y_n+1/2 - y_n
//
template <class S>
class ImplicitMidPoint : public ImplicitIntegrator<S> {
public:
	ImplicitMidPoint(S &system) { 
		system.AllocJacobian(J);
		system.AllocState(f1);
		system.AllocState(f2);
		system.AllocState(dstate1);
		system.AllocState(dstate2);		
		system.AllocState(B);
		system.AllocState(tmp_state);		
	}	
	void	Integrate(S &system, Real h);	
protected:
	void	Solver(S &system, State &state, Real h);
	Jacobian		J;
	State			f1, f2;
  	State			dstate1, dstate2;
	State			B;		
	State			tmp_state;
};


template <class S>
void ImplicitMidPoint<S>::Integrate(S &system, Real h)
{	
	State &state = system.GetState();	

	system.DerivState(f1, state);	
	// build A = I-(h/2)J  
	system.IdentityMinushJacobian(J, state, h/2.0);	
	// build B = (h/2)f1
	system.ScaleState(B, f1, h/2.0);	
	// solve A*dx1 = B, dx1 = x_n+1/2 - x_n
	Solver(system, dstate1, h);	
	// x1 = x + dx1
	system.AddState(tmp_state, state, dstate1, 1.0); 	

	system.DerivState(f2, tmp_state);
	// build A = I-(h/2)J  
	//system.IdentityMinushJacobian(J, tmp_state, h/2.0);	
	// build B = (h/2)f2 - dx1
	system.ScaleState(B, f2, h/2.0);
	system.AddState(B, B, dstate1, -1.0);		
	// solve A*dx2 = B
	Solver(system, dstate2, h);	
	// x = 2*x1 - x + dx2/2
	system.ScaleState(tmp_state, tmp_state, 2.0);
	system.AddState(tmp_state, tmp_state, state, -1.0);	
	system.AccumState(state, tmp_state, dstate2, 0.5); 
}

template <class S>
void ImplicitMidPoint<S>::Solver(S &system, State &state, Real h)
{
	// Conjugate Gradient
	CG(system, state, J, B, state.size, error);  
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
