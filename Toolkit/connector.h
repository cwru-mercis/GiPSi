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

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriya Natsupakpong.
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
#include "GiPSiAPI.h"
#include "XMLNode.h"
#include "fem.h"
#include "bioe.h"
#include "lumpedfluid.h"
#include "msd.h"
#include "qsds.h"
#include "probe.h"
#include "catheter.h"
#include "balloon.h"

using namespace GiPSiXMLWrapper;

class LoaderUnitTest;
extern void vprint(Vector<Real> vec);
extern void vprint(Matrix<Real> mat);

/****************************************************************
 *				FEM SOLID - LUMPED FLUID CONN					*  
 ****************************************************************/
class FEM3LM_LUMPEDFLUID_Connector : public Connector {
public:
	//constructor
	FEM3LM_LUMPEDFLUID_Connector(XMLNode * connectorNode, SIMObject * objects[MAX_SIMOBJ], int num_objects);

	void				process(void);

protected:
	Boundary * GetBoundary(const char * objectName, SIMObject * objects[MAX_SIMOBJ], int num_objects);
	void LoadGeometry(XMLNodeList * modelParametersChildren);

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

	friend LoaderUnitTest;
};

/****************************************************************
 *			FEM SOLID - LUMPED ELECTCHEM CONN					*  
 ****************************************************************/
class FEM3LM_BIOE_Connector : public Connector {
public:
	//constructor
	FEM3LM_BIOE_Connector(XMLNode * connectorNode, SIMObject * objects[MAX_SIMOBJ], int num_objects);

	void				process(void);

protected:
	Domain * GetDomain(const char * objectName, SIMObject * objects[MAX_SIMOBJ], int num_objects);
	void LoadGeometry(XMLNodeList * modelParametersChildren);

	FEMDomain			*mech;
	CardiacBioEDomain	*bioe;
	Real				ExcitedStressValueXX, ExcitedStressValueYY, ExcitedStressValueZZ ;

	Matrix<Real>		Excitation_to_Contraction (Real excitation);
	void				Excitation_to_Contraction (Real *Excitation_Array, Matrix<Real> *Stress_Array);

	friend LoaderUnitTest;
};

/****************************************************************
 *				QSDS - MSD CONN									*  
 ****************************************************************/
class QSDS_MSD_Connector : public Connector {
public:
	//constructor
	QSDS_MSD_Connector(XMLNode * connectorNode, SIMObject * objects[MAX_SIMOBJ], int num_objects);

	void				process(void);

protected:
	Boundary * GetBoundary(const char * objectName, SIMObject * objects[MAX_SIMOBJ], int num_objects);
	void LoadGeometry(XMLNodeList * modelParametersChildren);

	QSDSBoundary			*qsds;
	MSDBoundary				*msd;
	
	Vector<Real>			*msdpos;
	Vector<Real>			*residualforce;

	unsigned int		*vcorr;	
	unsigned int		num_vertex;	

	void	Load(char *filename);

	friend LoaderUnitTest;
};

/****************************************************************
 *				Endo - Catheter CONN							*  
 ****************************************************************/
class Endo_Catheter_Connector : public Connector {
public:
	//constructor
	Endo_Catheter_Connector(XMLNode * connectorNode, SIMObject * objects[MAX_SIMOBJ], int num_objects);

	void				process(void);

protected:
	Domain * 			GetDomain(const char * objectName, SIMObject * objects[MAX_SIMOBJ], int num_objects);
	void				LoadParameter(XMLNodeList * modelParametersChildren);

	RigidProbeHIODomain	*endo;
	CatheterHIODomain	*catheter;
	Matrix<Real>		CatheterBase_Endo_Transformation;		// g_e_c

	friend LoaderUnitTest;
};

/****************************************************************
 *				Catheter - Balloon CONN							*  
 ****************************************************************/
class Catheter_Balloon_Connector : public Connector {
public:
	//constructor
	Catheter_Balloon_Connector(XMLNode * connectorNode, SIMObject * objects[MAX_SIMOBJ], int num_objects);

	void				process(void);

protected:
	Domain * 			GetDomain(const char * objectName, SIMObject * objects[MAX_SIMOBJ], int num_objects);	
	void				LoadParameter(XMLNodeList * modelParametersChildren);

	CatheterHIODomain	*catheter;
	BalloonDomain		*balloon;	
	Matrix<Real>		BalloonBase_Catheter_Transformation;	// g_c_b

	friend LoaderUnitTest;
};

#endif