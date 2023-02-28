/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Simple Cardiac Bioelectricity Model Implementation (bioe.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	bioe.CPP v0.0
////
////	Cardiac Bioelectricity Model
////
////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "algebra.h"
#include "bioe.h"
#include "errors.h"
#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "GiPSiException.h"
#include "logger.h"
#include "timing.h"
#include "XMLNodeList.h"


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::CardiacBioEObject()
//
//		Constructor
//
CardiacBioEObject::CardiacBioEObject(	XMLNode * simObjectNode,
					Real OscFreq,
					Real SpVel,
					Real DCycle)
					:	SIMObject(simObjectNode),
						TemporalFreq(OscFreq),SpatialVel(SpVel), DutyCycle(DCycle)
{
	try
	{
		// Extract initialization information
		if (simObjectNode == NULL)
		{
			char location[256]("");
			sprintf_s(location, 256, "%s constructor", GetName());
			throw new GiPSiException(location, "Empty simObject node recieved.");
			return;
		}
		XMLNodeList * simObjectChildren = simObjectNode->GetChildren();

		// Set object parameters
		SetParameters(simObjectChildren);

		// Initialize visualization
		InitializeVisualization(simObjectChildren);

		// Load geometry
		LoadGeometry(simObjectChildren);

		// Initialize transformation
		InitializeTransformation(simObjectChildren);

		// Final setup
//		Geom2State();
		SetupDisplay(simObjectChildren);
		delete simObjectChildren;
	}
	catch (...)
	{
		throw;
		return;
	}
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::SetParameters()
//
//		Set object parameters
//
void CardiacBioEObject::SetParameters(XMLNodeList * simObjectChildren)
{
	try
	{
		// Get parameters node
		XMLNode * objParametersNode = simObjectChildren->GetNode("objParameters");
		XMLNodeList * objParametersChildren = objParametersNode->GetChildren();
		XMLNode * NHParametersNode = objParametersChildren->GetNode("NHParameters");
		XMLNodeList * NHParametersChildren = NHParametersNode->GetChildren();

		// Extract type-specific parameters
		XMLNode * modelParametersNode = NHParametersChildren->GetNode("modelParameters");
		XMLNodeList * modelParametersChildren = modelParametersNode->GetChildren();
		XMLNode * CBEParametersNode = modelParametersChildren->GetNode("CBEParameters");
		XMLNodeList * CBEParametersChildren = CBEParametersNode->GetChildren();

		XMLNode * TempFreqNode = CBEParametersChildren->GetNode("TempFreq");
		const char * TempFreqVal = TempFreqNode->GetValue();
		delete TempFreqNode;
		XMLNode * SpVelNode = CBEParametersChildren->GetNode("SpVel");
		const char * SpVelVal = SpVelNode->GetValue();
		delete SpVelNode;
		XMLNode * DutyCycleNode = CBEParametersChildren->GetNode("DutyCycle");
		const char * DutyCycleVal = DutyCycleNode->GetValue();
		delete DutyCycleNode;

		// Set type-specific parameters
		TemporalFreq = atof(TempFreqVal);
		SpatialVel = atof(SpVelVal);
		DutyCycle = atof(DutyCycleVal);

		delete TempFreqVal;
		delete SpVelVal;
		delete DutyCycleVal;
		delete CBEParametersChildren;
		delete CBEParametersNode;
		delete modelParametersChildren;
		delete modelParametersNode;
		delete NHParametersChildren;
		delete NHParametersNode;
		delete objParametersChildren;
		delete objParametersNode;
	}
	catch (...)
	{
		throw;
		return;
	}
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::InitializeVisualization()
//
//		Initialize visualization parameters
//
void CardiacBioEObject::InitializeVisualization(XMLNodeList * simObjectChildren)
{
	// This loader contains a lot of checking.  It makes it safer,
	// but substantially increases the length of the method.  Is
	// it worth adding these checks to the other init methods?
	try
	{
		// Extract baseColor
		XMLNode * visualizationNode = simObjectChildren->GetNode("visualization");
		XMLNodeList * visualizationChildren = visualizationNode->GetChildren();

		// Extract baseColor
		XMLNode * baseColorNode = visualizationChildren->GetNode("baseColor");
		XMLNodeList * baseColorChildren = baseColorNode->GetChildren();

		// Extract red
		XMLNode * redNode = baseColorChildren->GetNode("red");
		const char * red = redNode->GetValue();
		delete redNode;

		// Extract green
		XMLNode * greenNode = baseColorChildren->GetNode("green");
		const char * green = greenNode->GetValue();
		delete greenNode;

		// Extract blue
		XMLNode * blueNode = baseColorChildren->GetNode("blue");
		const char * blue = blueNode->GetValue();
		delete blueNode;

		// Extract opacity
		XMLNode * opacityNode = baseColorChildren->GetNode("opacity");
		const char * opacity = opacityNode->GetValue();
		delete opacityNode;

		// Initialize HeartMuscleGeometry using color information
		HeartMuscleGeometry = new TetVolume((float)atof(red), (float)atof(green), (float)atof(blue), (float)atof(opacity));
		delete red;
		delete green;
		delete blue;
		delete opacity;

		delete baseColorChildren;
		delete baseColorNode;
		delete visualizationChildren;
		delete visualizationNode;
	}
	catch (...)
	{
		throw;
		return;
	}
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::LoadGeometry()
//
//		Initialize visualization parameters
//
void CardiacBioEObject::LoadGeometry(XMLNodeList * simObjectChildren)
{
	try
	{
		// Get path and file name information
		XMLNode * geometriesNode = simObjectChildren->GetNode("geometries");
		XMLNodeList * geometriesChildren = geometriesNode->GetChildren();
		XMLNode * geometryNode = geometriesChildren->GetNode("geometry");
		XMLNodeList * geometryChildren = geometryNode->GetChildren();
		XMLNode * geometryFileNode = geometryChildren->GetNode("geometryFile");
		XMLNodeList * geometryFileChildren = geometryFileNode->GetChildren();

		XMLNode * pathNode = geometryFileChildren->GetNode("path");
		const char * path = pathNode->GetValue();
		XMLNode * fileNameNode = geometryFileChildren->GetNode("fileName");
		const char * fileName = fileNameNode->GetValue();
		delete fileNameNode;
		delete pathNode;
		delete geometryFileChildren;
		delete geometryFileNode;
		delete geometryChildren;
		delete geometryNode;
		delete geometriesChildren;
		delete geometriesNode;

		// Format file name
		char * formattedFileName = new char[strlen(path) + strlen(fileName) + 4];
		sprintf_s(formattedFileName, strlen(path) + strlen(fileName) + 4, ".\\%s\\%s", path, fileName);

		// Load the object file
		logger->Message(GetName(), "Loading object file...", 1);
		Load(formattedFileName);
		delete formattedFileName;
		delete path;
		delete fileName;
	}
	catch (...)
	{
		throw;
		return;
	}
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::InitializeTransformation()
//
//		Initialize transformation parameters
//
void CardiacBioEObject::InitializeTransformation(XMLNodeList * simObjectChildren)
{
	try
	{
		Geometry * geom = GetGeometryPtr();
		XMLNode * transformationNode = simObjectChildren->GetNode("transformation");
		XMLNodeList * transformationChildren = transformationNode->GetChildren();

		// Extract scaling information
		XMLNode * scalingNode = transformationChildren->GetNode("scaling");
		XMLNodeList * scalingChildren = scalingNode->GetChildren();

		// Extract scaling components
		XMLNode * sxNode = scalingChildren->GetNode("x");
		const char * sx = sxNode->GetValue();
		XMLNode * syNode = scalingChildren->GetNode("y");
		const char * sy = syNode->GetValue();
		XMLNode * szNode = scalingChildren->GetNode("z");
		const char * sz = szNode->GetValue();
		// Scale the object
		geom->Scale((float)atof(sx), (float)atof(sy), (float)atof(sz));
		delete sx;
		delete sy;
		delete sz;
		delete szNode;
		delete syNode;
		delete sxNode;
		delete scalingChildren;
		delete scalingNode;

		// Extract rotation information
		XMLNode * rotationNode = transformationChildren->GetNode("rotation");
		XMLNodeList * rotationChildren = rotationNode->GetChildren();
		XMLNode * axisRotationNode = rotationChildren->GetNode("axisRotation");
		XMLNodeList * axisRotationChildren = axisRotationNode->GetChildren();
		XMLNode * axisNode = axisRotationChildren->GetNode("axis");
		XMLNodeList * axisChildren = axisNode->GetChildren();

		// Extract rotation components
		XMLNode * rxNode = axisChildren->GetNode("x");
		const char * rx = rxNode->GetValue();
		XMLNode * ryNode = axisChildren->GetNode("y");
		const char * ry = ryNode->GetValue();
		XMLNode * rzNode = axisChildren->GetNode("z");
		const char * rz = rzNode->GetValue();
		XMLNode * rangleNode = axisRotationChildren->GetNode("angle");
		const char * rangle = rangleNode->GetValue();
		// Rotate the object
		geom->Rotate(TORAD(atof(rangle)), atof(rx), atof(ry), atof(rz));
		delete rangle;
		delete rx;
		delete ry;
		delete rz;
		delete rangleNode;
		delete rzNode;
		delete ryNode;
		delete rxNode;
		delete axisChildren;
		delete axisNode;
		delete axisRotationChildren;
		delete axisRotationNode;
		delete rotationChildren;
		delete rotationNode;

		// Extract translation information
		XMLNode * translationNode = transformationChildren->GetNode("translation");
		XMLNodeList * translationChildren = translationNode->GetChildren();

		// Extract translation components
		XMLNode * dxNode = translationChildren->GetNode("x");
		const char * dx = dxNode->GetValue();
		XMLNode * dyNode = translationChildren->GetNode("y");
		const char * dy = dyNode->GetValue();
		XMLNode * dzNode = translationChildren->GetNode("z");
		const char * dz = dzNode->GetValue();
		// Translate the object
		geom->Translate((float)atof(dx), (float)atof(dy), (float)atof(dz));
		delete dx;
		delete dy;
		delete dz;
		delete dzNode;
		delete dyNode;
		delete dxNode;
		delete translationChildren;
		delete translationNode;

		delete transformationChildren;
		delete transformationNode;
	}
	catch (...)
	{
		throw;
		return;
	}
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::Load()
//
//		Reads in .fem file
//
void CardiacBioEObject::Load(char *basename)   
{
	// Load the geometric model for the heart muscle
	HeartMuscleGeometry->Load(basename);
	geometry=HeartMuscleGeometry;

	/****************************************/
	/*	Setting up of the boundary			*/
	/****************************************/
	// No need for a boundary


	/****************************************/
	/*	Setting up of the domain			*/
	/****************************************/

	//first allocate the domain
	if((domain = new CardiacBioEDomain()) == NULL) {
		printf("ERROR: Could not allocate boundary!\n");
		exit(0);
	}
	CardiacBioEDomain *		dom = (CardiacBioEDomain *) domain;

	//and then load the domain
	dom->Load(basename);
	dom->init(this);

	/****************************************/
	/*	Initialize the states				*/
	/****************************************/
  
	state.x[0]=1.0;
	state.x[1]=0.0;
	
	// NOTE: The method is harcoded for now.
	integrator = new ERKHeun3<CardiacBioEObject>(*this);
}



////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::AccumState()
//
//		Computes x(t+h) = x(t) + h * f(..)
//
//		where	x(t+h)	= new_state
//				x(t)	= state
//				f(..)	= deriv
//
inline void CardiacBioEObject::AccumState(State &new_state, const State &state, const State &deriv, const Real &h)
{
	for(int i = 0; i <2; i++) {
		new_state.x[i] = state.x[i] + deriv.x[i] * h;
	}
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::DerivState()
//
//		Computes the derivatives of state variables. This is simply 
//		the f() evaluation where f() = dx/dt.
//
//
inline void CardiacBioEObject::DerivState(State &deriv, State &state)
{
	deriv.x[0]=state.x[1];
	deriv.x[1]=-(2*M_PI*TemporalFreq)*(2*M_PI*TemporalFreq)*state.x[0];
}



////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::AllocState()
//
//		Allocates the memory for the integrator's local state 
//		members
//
inline void CardiacBioEObject::AllocState(State &s)
{
}



////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::Simulate()
//
//		Integrates the FEM_3LM object by a single timestep
//
//
void CardiacBioEObject::Simulate(void)
{
	integrator->Integrate(*this, timestep);
	time+=timestep;
}




////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::Display()
//
//		Displays the FEM mesh
//
//
void CardiacBioEObject::Display(void)
{
	// Copy the state into the geometry
	State2Geom();

	displayMngr->Display();

}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::State2Geom()
//
//		Copies state to geometry
//
//
inline void CardiacBioEObject::State2Geom(void)
{
	static TetVolume *geom = (TetVolume *) geometry;

	for(unsigned int i=0; i<geom->num_vertex; i++) {
		geom->vertex[i].color[1] = CalcExcitation(geom->vertex[i].pos[0]);
	}
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::SetupDisplay()
//
//		Sets up the display of the BIOE mesh
//
//
void CardiacBioEObject::SetupDisplay(XMLNodeList * simObjectChildren)
 {
	DisplayHeader	header;

	header.objType				= GIPSI_DRAW_TRIANGLE;
	header.polyMode				= GIPSI_POLYGON_OUTLINE;
	header.shadeMode			= GIPSI_SHADE_FLAT;
	header.dataType				= 0x06;

	displayMngr = new TetVolumeDisplayManager((TetVolume *) geometry, &header);
	displayMngr->SetObjectName(GetName());

	// Load texture names for each DisplayManager
	XMLNode * geometriesNode = simObjectChildren->GetNode("geometries");
	XMLNodeList * geometriesChildren = geometriesNode->GetChildren();
	XMLNode * geometryNode = geometriesChildren->GetNode("geometry");
	XMLNodeList * geometryChildren = geometryNode->GetChildren();
	XMLNode * textureNamesNode = geometryChildren->GetNode("textureNames");
	if (textureNamesNode->GetNumChildren() > 0)
	{
		XMLNodeList * textureNamesChildren = textureNamesNode->GetChildren();

		for (int i = 0; i < textureNamesChildren->GetLength(); i++)
		{
			XMLNode * textureNameNode = textureNamesChildren->GetNode(i);
			char * textureName = textureNameNode->GetValue();
			displayMngr->AddTexture(textureName);
			delete textureName;
			delete textureNameNode;
		}
		delete textureNamesChildren;
	}
	delete textureNamesNode;
	delete geometryChildren;
	delete geometryNode;
	delete geometriesChildren;
	delete geometriesNode;
}

////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::GetExcitation()
//
//		Returns excitation level for the element
//
//
Real	CardiacBioEObject::GetExcitation(unsigned int index)
{
	Real xloc=0.0;

	for (int i=0; i<4; i++) 
		xloc += 0.25*HeartMuscleGeometry->tet[index].vertex[i]->pos[0];

	return CalcExcitation(xloc);
}

////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::CalcExcitation()
//
//		Returns excitation level for a location
//
//

Real	CardiacBioEObject::CalcExcitation(Real xloc)
{
	Real refcoord= 0.0;

	// The following is a simple finite state machine bioelectric model
	//   used in the two chamber heart simulation
	// Timing of the switching is coupled to the second order oscillator
	//   from the state of the model
	if (state.x[0]>0.1) {
		return (xloc>2.5)?1:0;
	}
	else if (state.x[0]<-0.1) {
		return (xloc<-2.5)?1:0;
	}
	else {
		return 0.0;
	}
	// The following is a simple wave propagation bioelectric model
	//   used in the single chamber heart model
	//Real temp=cos(2*M_PI*TemporalFreq*(time-(xloc-refcoord)/SpatialVel)+M_PI)-cos(M_PI*DutyCycle);
	//return (xloc>0)? ((temp>0)?1:0):(0.0) ;
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::SetMuscleStrain
//
//		Sets Muscle strain to use in excitation contraction coupling
//
void	CardiacBioEObject::SetMuscleStrain(unsigned int index, Matrix<Real> Strain)	// we will need this for the real thing
{
	error_exit(-1,"Functionality not implemented yet!\n");
}

////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::SetMuscleGeometry
//
//		Updates the muscle geometry
//
void	CardiacBioEObject::SetHeartMuscleGeometry(TetVolume *newgeom)				// will we need this ???
{
	error_exit(-1,"Functionality not implemented yet!\n");
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEDomain::init
//
//		CardiacBioEObject Domain initializer
//
void			CardiacBioEDomain::init(CardiacBioEObject *ParentObj)
{
	unsigned int i;

	Object=ParentObj;

	DomStrain = new Matrix<Real>[num_tet];
	
	if(DomStrain == NULL) {
		error_exit(-1, "Cannot allocate memory for domain stresses!\n");
	}	
	
	for(i=0; i<num_tet; i++)	DomStrain[i] = zero_Matrix;

}

////////////////////////////////////////////////////////////////
//
//	CardiacBioEDomain::SetDomStrain
//
//		Sets Domain Strain
//
void	CardiacBioEDomain::SetDomainStrain(unsigned int element_index, Matrix<Real> Strain_Tensor)
{
	DomStrain[element_index]=Strain_Tensor;
}

////////////////////////////////////////////////////////////////
//
//	CardiacBioEDomain::SetDomStrain
//
//		Sets Domain Strain
//

void		CardiacBioEDomain::SetDomainStrain(Matrix<Real> *Strain_Tensor_Array)
{
	for (unsigned int index=0; index < this->num_tet; index++){
		DomStrain[index]=Strain_Tensor_Array[index];
	}
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEDomain::SetGeometry
//
//		Sets Domain Geometry
//

void		CardiacBioEDomain::SetGeometry()
{
	error_exit(-1,"Functionality not implemented yet!\n");
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEDomain::Get
//
//		Sets External Excitation
//
Real		CardiacBioEDomain::GetExcitation(unsigned int element_index)
{
	
	return  Object->GetExcitation(element_index);
}


void		CardiacBioEDomain::GetExcitation(Real *Excitation_Array)
{
	for (unsigned int index=0; index < this->num_tet; index++){
		Excitation_Array[index]=Object->GetExcitation(index);
	}
}

