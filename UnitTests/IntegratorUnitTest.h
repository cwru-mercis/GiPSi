/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for GiPSi Integrator Unit Test (IntegratorUnitTest.h).

The Initial Developer of the Original Code is Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	INTEGRATORUNITTEST.H v0.0
////
////	Header for GiPSi Integrator Unit Test
////
////////////////////////////////////////////////////////////////

#ifndef _INTEGRATOR_UNIT_TEST_H_
#define _INTEGRATOR_UNIT_TEST_H_

#include "GiPSiAPI.h"
#include "GiPSiIntegrators.h"

extern void vprint(Vector<Real> vec);
extern void vprint(Matrix<Real> mat);

class IntegrationTestCase {
public:
	IntegrationTestCase();
	~IntegrationTestCase();

	/**< State parameter. the state information */
	typedef struct {		
		Vector<Real>	*POS;		/**< Global position vector */
		Vector<Real>	*pos;		/**< position */
		Vector<Real>	*VEL;		/**< Global velocity vector */
		Vector<Real>	*vel;		/**< velocity */			
		unsigned int	size;		/**< state size */
	} State;

	/**< Jacobian parameter for implicit method */
	typedef struct 
	{
		Matrix<Real>	*A11;		/**< Matrix A11  : 3*size x 3*size */
		Matrix<Real>	*A12;		/**< Matrix A12  : 3*size x 3*size */
		Real			dA21;		/**< Real dA21 : value of diagonal matrix A21 */
		Real			dA22;		/**< Real dA22 : value of diagonal matrix A22 */
		unsigned int	size;		/**< Jabobian size (=state size) */
	} Jacobian;

	void				Init(int method, Real h, Real start, Real end);
	void				SetIntegrationMethod(int method);
	void				setInitialCondition(void);	
	Vector<Real> 		func(Vector<Real> x, Vector<Real> v);
	void				Run(void);

	// Integrator interface
	void				AllocState(State &s);	
	State&				GetState(void)	{ return state; }
	void				DerivState(State &deriv, State &state);
	void				AccumState(State &new_state, const State &state, const State &deriv, const Real &h);	
	void				Simulate(void);
	void				PrintState(State &state, Real t);

	void				AllocJacobian(Jacobian &J);
	void				InverseJacobian(Matrix<Real> &invJ, const Jacobian &J);
	void				MultiplyMatrixState(State &out_state, const Matrix<Real> &M, const State &state);
	void				AddState(State &new_state, const State &state1, const State &state2, const Real h);
	void				ScaleState(State &new_state, const State &state, const Real h);
	Real				NormState(const State &state);
	Real				StateDotState(const State &state1, const State &state2);
	void				MultiplyJacobianState(State &out_state, const Jacobian &J, const State &state);
	void				IdentityMinushJacobian(Jacobian &J, const State &state, const Real h); 
	void				PrintJacobian(const Jacobian &J);	

protected:
	State								state;	
	Integrator<IntegrationTestCase>		*integrator;	/**< intergrator pointer */
	Real								time;				// Local time of the object
	Real								timestep;			// The local timestep	
	Real								starttime;
	Real								endtime;
};

class IntegratorUnitTest
{
public:
	IntegratorUnitTest();

	void Run();
	int GetFailedCount() { return myFailedCount; }
	void TEST_VERIFY(bool test);

private:
	int myFailedCount;
};

#endif
