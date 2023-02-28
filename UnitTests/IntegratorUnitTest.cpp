/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Source for GiPSi Integrator Unit Test (IntegratorUnitTest.cpp).

The Initial Developer of the Original Code is Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	INTEGRATORUNITTEST.CPP v0.0
////
////	Source for GiPSi Integrator Unit Test
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include <stdio.h>
#include <string.h>
#include "logger.h"
#include "IntegratorUnitTest.h"

/*
===============================================================================
	IntegrationTestCase class
===============================================================================
*/

IntegrationTestCase::IntegrationTestCase():integrator(NULL)
{
	time = 0.0;
}

IntegrationTestCase::~IntegrationTestCase()
{
	if (integrator) {
		delete integrator;
		integrator = NULL;
	}
}

void IntegrationTestCase::Init(int method, Real h, Real start, Real end)
{
	unsigned int i;

	state.size	= 1;

	state.POS = new Vector<Real>(state.size * 3, 0.0);
	if(state.POS == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	state.pos = new Vector<Real>[state.size];
	for(i = 0; i < state.POS->dim(); i +=3)
		state.pos[i/3].remap(3, &((*(state.POS))[i]));

	state.VEL = new Vector<Real>(state.size * 3, 0.0);
	if(state.VEL == NULL) {
		error_exit(-1, "Cannot allocate memory for velocity!\n");
	}
	state.vel = new Vector<Real>[state.size];
	for(i = 0; i < state.VEL->dim(); i +=3)
		state.vel[i/3].remap(3, &((*(state.VEL))[i]));

	for(i=0;i<state.size;i++) 
	{		
		this->state.pos[i]	= zero_vector3;
		this->state.vel[i]	= zero_vector3;		
	}

	SetIntegrationMethod(method);
	timestep = h;
	starttime = start;
	endtime = end;
	time = 0.0;
}

void IntegrationTestCase::SetIntegrationMethod(int method)
{
	if (integrator) {
		delete integrator;
		integrator = NULL;
	}
	switch(method)
	{
		case 1: //Euler
			integrator = new Euler<IntegrationTestCase>(*this);
			break;
		case 2: //MidPoint
			integrator = new ERKMid2<IntegrationTestCase>(*this);
			break;
		case 3: // RK4
			integrator = new ERK4<IntegrationTestCase>(*this);
			break;
		case 4: //Backward Euler
			//integrator = new SemiEuler<IntegrationTestCase>(*this);
			break;
		case 5: //Implicit Euler with CG
			integrator = new ImplicitEuler<IntegrationTestCase>(*this);
			break;
		case 6: //Implitcit Euler with CG+Newton
			integrator = new ImplicitEulerNT<IntegrationTestCase>(*this);
			break;		
		case 7:
			integrator = new ERKHeun3<IntegrationTestCase>(*this);
			break;
		case 8: //MidPoint
			integrator = new ImplicitMidPoint<IntegrationTestCase>(*this);
			break;
	}
}

void IntegrationTestCase::setInitialCondition(void)
{	
	state.pos[0][1] = -1;
	state.vel[0][1] = -5;
}

Vector<Real> IntegrationTestCase::func(Vector<Real> pos, Vector<Real> vel)
{
	Real m = 0.1;
	Real k = 100.0;
	Real l_zero = -1.0;
	Real d = 1.0;
	Real g = -10.0;
	Vector<Real> out = zero_vector3;

	out[1] = (k/m)*(l_zero - pos[1]) - (d/m)*vel[1] + g;
	return out;
}

void IntegrationTestCase::Run(void)
{
	int size = (endtime - starttime)/timestep;
	for(unsigned int i=0;i<=size;i++)
		Simulate();
}

void IntegrationTestCase::AllocState(State &s)
{
	static unsigned int i;

	s.POS = new Vector<Real>(state.POS->dim(), 0.0);
	if(s.POS == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	s.pos = new Vector<Real>[state.size];
	for(i = 0; i < s.POS->dim(); i +=3)
		s.pos[i/3].remap(3, &((*s.POS)[i]));

	s.VEL = new Vector<Real>(state.VEL->dim(), 0.0);
	if(s.VEL == NULL) {
		error_exit(-1, "Cannot allocate memory for velocity!\n");
	}
	s.vel = new Vector<Real>[state.size];
	for(i = 0; i < s.VEL->dim(); i +=3)
		s.vel[i/3].remap(3, &((*s.VEL)[i]));

	s.size = state.size;
}

void IntegrationTestCase::DerivState(State &deriv, State &state)
{
	for(unsigned int i = 0; i < state.size; i++) {
		// dpos/dt = vel
		deriv.pos[i] = state.vel[i];
		// dvel/dt = func
		deriv.vel[i] = func(state.pos[i], state.vel[i]);		
	}
}

void IntegrationTestCase::AccumState(State &new_state, const State &state, const State &deriv, const Real &h)
{
	static Vector<Real>			temp(3,0.0);
	for(unsigned int i = 0; i < state.size; i++) {
//		new_state.pos[i] = state.pos[i] + deriv.pos[i] * h;
		temp = deriv.pos[i];
		temp *= h;
		temp += state.pos[i];
		new_state.pos[i] = temp;
		
//		new_state.vel[i] = state.vel[i] + deriv.vel[i] * h;
		temp = deriv.vel[i];
		temp *= h;
		temp += state.vel[i];
		new_state.vel[i] = temp;
	}
}

void IntegrationTestCase::Simulate(void)
{	
	if(time==0) setInitialCondition();
	PrintState(state, time);
	integrator->Integrate(*this, timestep);
	time+=timestep;	
}

void IntegrationTestCase::PrintState(State &state, Real t)
{
	int size = state.size;
	printf("time = %f ", t);
	printf("vel = ");
	vprint(*state.VEL);	
	printf("pos = ");
	vprint(*state.POS);	
	printf("\n");
}

/**
 * IntegrationTestCase::IdentityMinushJacobian()
 * Calculate I-hJ.
 * @param J Jacobian
 * @param state State
 * @param f State
 * @param h Real
 */
inline void IntegrationTestCase::IdentityMinushJacobian(Jacobian &J, const State &state, const Real h) 
{  
	// clear Jacobian A11 A12 matrix
	*(J.A11) = Matrix<Real>(3*state.size,3*state.size,0.0);
	*(J.A12) = Matrix<Real>(3*state.size,3*state.size,0.0);	

	Real m = 0.1;
	Real k = 100.0;
	Real l_zero = -1.0;
	Real d = 1.0;
	Real g = -10.0;
	Vector<Real> out = zero_vector3;

	(*(J.A11))[1][1] = 1+h*(d/m);		
	(*(J.A12))[1][1] = h*(k/m);
		
	J.dA21 = -h;
	J.dA22 = 1.0;	
}

/**
 * IntegrationTestCase::AllocJacobian()
 * Allocates the memory for the integrator's local Jacobian members.
 * @param J Jacobian
 */
inline void IntegrationTestCase::AllocJacobian(Jacobian &J)
{   
	J.A11 = new Matrix<Real>(3*state.size,3*state.size,0.0);
	if(J.A11 == NULL) {
		error_exit(-1, "Cannot allocate memory for jacobian A11!\n");
	}	
	
	J.A12 = new Matrix<Real>(3*state.size,3*state.size,0.0);
	if(J.A12 == NULL) {
		error_exit(-1, "Cannot allocate memory for jacobian A12!\n");
	}	

	J.size = state.size;
	J.dA21 = 0;
	J.dA22 = 0;	
}

inline void IntegrationTestCase::InverseJacobian(Matrix<Real> &invJ, const Jacobian &J)
{
	int size = J.size;
	Matrix<Real> MJ(6*size, 6*size, 0.0);
	for(int i=0;i<3*size;i++) {
		for(int j=0;j<3*size;j++) {
			MJ[i][j] = (*J.A11)[i][j];
			MJ[3*size+i][j] = (*J.A12)[i][j];			
		}
		MJ[3*size+i][i] = J.dA21;
		MJ[3*size+i][3*size+i] = J.dA22;
	}
	invM(invJ, MJ);
}
inline void IntegrationTestCase::MultiplyMatrixState(State &out_state, const Matrix<Real> &M, const State &state)
{
	int size = state.size;
	ASSERT(M.size() == 3*size);	
	Vector<Real> tmp(6*size, 0.0);
	Vector<Real> result(6*size, 0.0);
	for(int i=0;i<size;i++) {		
		tmp[i] = (*state.VEL)[i];
		tmp[size+i] = (*state.POS)[i];
	}

	result = M*tmp;

	for(int i=0;i<size;i++) {		
		(*out_state.VEL)[i] = result[i];
		(*out_state.POS)[i] = result[size+i];
	}	
}

/**
 * IntegrationTestCase::AddState()
 * Add the state1 and h*state2.
 * @param new_state State
 * @param state1 State
 * @param state2 State
 * @param h Real
 */
inline void IntegrationTestCase::AddState(State &new_state, const State &state1, const State &state2, const Real h)
{
	(*new_state.VEL) = (*state1.VEL) + h*(*state2.VEL);
	(*new_state.POS) = (*state1.POS) + h*(*state2.POS);
}

/**
 * IntegrationTestCase::ScaleState()
 * Scale the state to new_state with scaling value h.
 * @param new_state State
 * @param h Real
 * @param state State
 */
inline void IntegrationTestCase::ScaleState(State &new_state, const State &state, const Real h)
{
	(*new_state.VEL) = h*(*state.VEL);
	(*new_state.POS) = h*(*state.POS);  	
}

/**
 * IntegrationTestCase::NormState()
 * Calculate the norm of state.
 * @param state State
 * @return real the norm of state
 */
inline Real IntegrationTestCase::NormState(const State &state)
{
	return sqrt((*state.POS).length_sq() + (*state.VEL).length_sq());	
}

/**
 * IntegrationTestCase::StateDotState()
 * Calculate the dot product of state1 and state2.
 * @param state1 State
 * @param state2 State
 * @return real the dot product of state1 and state2
 */
inline Real IntegrationTestCase::StateDotState(const State &state1, const State &state2)
{
	return ((*state1.POS)*(*state2.POS) + (*state1.VEL)*(*state2.VEL));	
}

/**
 * IntegrationTestCase::MultiplyJacobianState()
 * Calculate the multiply of Jacobian and state.
 * @param J Jacobian
 * @param state State
 * @return out_state State
 */
inline void IntegrationTestCase::MultiplyJacobianState(State &out_state, const Jacobian &J, const State &state)
{
	ASSERT(J.size == state.size);	
	(*out_state.VEL) = (*J.A11)*(*state.VEL) + (*J.A12)*(*state.POS);
	(*out_state.POS) = (J.dA21)*(*state.VEL) + (J.dA22)*(*state.POS);	
}

void IntegrationTestCase::PrintJacobian(const Jacobian &J)
{
	int size = J.size;
	printf("A11 = ");
	vprint((*J.A11));	
	printf("A12 = ");
	vprint((*J.A12));	
	printf("dA21 = %f, dA22 = %f\n",J.dA21,J.dA22);
}

/*
===============================================================================
	IntegratorUnitTest class
===============================================================================
*/

IntegratorUnitTest::IntegratorUnitTest()
{
	myFailedCount = 0;
}

void IntegratorUnitTest::Run()
{
	if (!logger)
		logger = new Logger();

	IntegrationTestCase test;
/*
	// Test 1 - Integrator ImEuler h=0.01
	printf("1 - Testing Integrator ImEuler h=0.1\n");	
	test.Init(5, 0.1, 0.0, 1.0);
	test.Run();

	// Test 2 - Integrator ImEuler h=0.01
	printf("2 - Testing Integrator ImEuler h=0.02\n");	
	test.Init(5, 0.02, 0.0, 1.0);
	test.Run();

	// Test 3 - Integrator ImEuler h=0.01
	printf("3 - Testing Integrator Euler h=0.01\n");	
	test.Init(5, 0.01, 0.0, 1.0);
	test.Run();

	// Test 4 - Integrator ImEuler h=0.005
	printf("4 - Testing Integrator ImEuler h=0.005\n");	
	test.Init(5, 0.005, 0.0, 1.0);
	test.Run();

	// Test 5 - Integrator ImEuler h=0.001
	printf("5 - Testing Integrator ImEuler h=0.001\n");	
	test.Init(5, 0.001, 0.0, 1.0);
	test.Run();
*/	

/*
	// Test 1 - Integrator Euler h=0.02
	printf("1 - Testing Integrator Euler h=0.02\n");	
	test.Init(1, 0.02, 0.0, 1.0);
	test.Run();

	// Test 2 - Integrator ImEuler h=0.02
	printf("2 - Testing Integrator ImEuler h=0.02\n");	
	test.Init(5, 0.02, 0.0, 1.0);
	test.Run();

	// Test 3 - Integrator Euler h=0.01
	printf("3 - Testing Integrator Euler h=0.01\n");	
	test.Init(1, 0.01, 0.0, 1.0);
	test.Run();

	// Test 4 - Integrator ImEuler h=0.01
	printf("4 - Testing Integrator ImEuler h=0.01\n");	
	test.Init(5, 0.01, 0.0, 1.0);
	test.Run();

	// Test 5 - Integrator Euler h=0.00125
	printf("5 - Testing Integrator Euler h=0.00125\n");	
	test.Init(1, 0.00125, 0.0, 1.0);
	test.Run();
*/
/*
	// Test 1 - Integrator Im Midpoint h=0.01
	printf("1 - Testing Integrator ImMidpoint h=0.01\n");	
	test.Init(8, 0.01, 0.0, 1.0);
	test.Run();
*/
	// Test 1 - Integrator ImEuletNT h=0.01
	printf("1 - Testing Integrator ImEulerNT h=0.01\n");	
	test.Init(6, 0.01, 0.0, 1.0);
	test.Run();

	if (logger)
	{
		delete logger;
		logger = NULL;
	}
}

void IntegratorUnitTest::TEST_VERIFY(bool test)
{
	if (test)
	{
		printf("Passed\n");
	}
	else
	{
		myFailedCount++;
		printf("Failed\n");
	}
}