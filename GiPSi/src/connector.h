/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Connector Class Definitions (connector.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	connector.h v0.1.0
////
////	1)	Defines connectors used in simulations
////
////
////////////////////////////////////////////////////////////////

#ifndef _CONNECTOR_H
#define _CONNECTOR_H

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "errors.h"
#include "fem.h"
#include "bioe.h"
#include "lumpedfluid.h"



/****************************************************************
 *				FEM SOLID - LUMPED FLUID CONN					*  
 ****************************************************************/
class FEM3LM_LUMPEDFLUID_Connector : public Connector {
public:
	//constructor
	FEM3LM_LUMPEDFLUID_Connector(	FEMBoundary				*MechanicalModel,
									LumpedFluidBoundary		*LFluidModel,
									char *filename								);

	void				process(void);

protected:
	FEMBoundary				*mech;
	LumpedFluidBoundary		*lfluid;
	
	Vector<Real>			*mechpos;
	Vector<Real>			*forceaccum;

	Real					mechtime;

	unsigned int		*vcorr;
	unsigned int		*fcorr;
	unsigned int		num_vertex;
	unsigned int		num_face;

	void	Load(char *filename);
};

/****************************************************************
 *			FEM SOLID - LUMPED ELECTCHEM CONN					*  
 ****************************************************************/
class FEM3LM_BIOE_Connector : public Connector {
public:
	//constructor
	FEM3LM_BIOE_Connector(	FEMDomain				*MechanicalModel,
							CardiacBioEDomain		*ElectricalModel,
							Real					ExStressValueXX, 
							Real					ExStressValueYY, 
							Real					ExStressValueZZ  );

	void				process(void);

protected:
	FEMDomain			*mech;
	CardiacBioEDomain	*bioe;
	Real				ExcitedStressValueXX, ExcitedStressValueYY, ExcitedStressValueZZ ;

	Matrix<Real>		Excitation_to_Contraction (Real excitation);
	void				Excitation_to_Contraction (Real *Excitation_Array, Matrix<Real> *Stress_Array);

};


#endif