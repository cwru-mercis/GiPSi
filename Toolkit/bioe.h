/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Simple Cardiac Bioelectricity Model Definition (bioe.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	bioe.h v0.0
////
////	Cardiac Bioelectricity Model
////
////////////////////////////////////////////////////////////////

#ifndef _BIOE_H
#define _BIOE_H

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class LoaderUnitTest;

class CardiacBioEObject: public SIMObject {
public:
	typedef struct {
		Real			x[2];
	} State;
	
	// Constructor
	CardiacBioEObject(	XMLNode * simObjectNode,
						Real OscFreq	=1.5, 
						Real SpVel		=1,
						Real DCycle		=0.5);

	// Loaders
	void				Load(char *filename);

	// Integrator interface
	void				AccumState(State &new_state, const State &state, const State &deriv, const Real &h);
	void				DerivState(State &deriv, State &state);
	void				AllocState(State &s);
	void				Simulate(void);

	// Get and Set interfaces for the Boundary and the Domain
	Real				GetExcitation(unsigned int index);
	void				SetMuscleStrain(unsigned int index, Matrix<Real> Strain);	// we will need this for the real thing
	void				SetHeartMuscleGeometry(TetVolume *newgeom);					// will we need this ???


	// Display functions
	void				Display(void);
	void				SetupDisplay(XMLNodeList * simObjectChildren);
	void				State2Geom(void);

	State&				GetState(void)	{	return	state; }

protected:
	void SetParameters(XMLNodeList * simObjectChildren);
	void InitializeVisualization(XMLNodeList * simObjectChildren);
	void LoadGeometry(XMLNodeList * simObjectChildren);
	void InitializeTransformation(XMLNodeList * simObjectChildren);

	State								state;
	Real								TemporalFreq;
	Real								SpatialVel;
	Real								DutyCycle;
	TetVolume *							HeartMuscleGeometry;

	Integrator<CardiacBioEObject>		*integrator;
	// Excitation Calculators
	Real				CalcExcitation(Real xloc);

	friend LoaderUnitTest;
};


// CardiacBioEObject Domain class
class CardiacBioEDomain : public Domain {
public:
	// Initializer
	void			init(CardiacBioEObject *ParentObj);

	// Get and Set interfaces
	Real			GetExcitation(unsigned int element_index);
	void			GetExcitation(Real *Excitation_Array);

	void			SetDomainStrain(unsigned int element_index, Matrix<Real> Strain_Tensor);		// we will need this for the real thing
	void			SetDomainStrain(Matrix<Real> *Strain_Tensor_Array);					// we will need this for the real thing
	void			SetGeometry();														// will we need this ???

protected:
	Matrix<Real>						*DomStrain;

	// Pointer to the parent object
	CardiacBioEObject					*Object; 
};


#endif