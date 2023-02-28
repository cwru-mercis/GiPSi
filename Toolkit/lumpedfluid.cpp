/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Lumped Fluid Implementation (lumpedfluid.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	lumpedfluid.CPP v0.0
////
////	Lumped Fluid Model
////
////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "algebra.h"
#include "errors.h"
#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "GiPSiException.h"
#include "logger.h"
#include "lumpedfluid.h"
#include "timing.h"
#include "XMLNodeList.h"



LumpedFluidObject::LumpedFluidObject(XMLNode * simObjectNode,
									 Real _Pi,
									 Real _Po,
									 Real _Pfo,
									 Real _K,
									 Real _B,
									 Real _Ri,
									 Real _Ro)
									 :	SIMObject(simObjectNode),
									 Pi(_Pi),Po(_Po),Pfo(_Pfo),K(_K),B(_B),Ri(_Ri),Ro(_Ro)
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
		Geom2State();
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
//	LumpedFluidObject::SetParameters()
//
//		Set object parameters
//
void LumpedFluidObject::SetParameters(XMLNodeList * simObjectChildren)
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
		XMLNode * LFParametersNode = modelParametersChildren->GetNode("LFParameters");
		XMLNodeList * LFParametersChildren = LFParametersNode->GetChildren();

		XMLNode * PiNode = LFParametersChildren->GetNode("Pi");
		const char * PiVal = PiNode->GetValue();
		delete PiNode;
		XMLNode * PoNode = LFParametersChildren->GetNode("Po");
		const char * PoVal = PoNode->GetValue();
		delete PoNode;
		XMLNode * PfoNode = LFParametersChildren->GetNode("Pfo");
		const char * PfoVal = PfoNode->GetValue();
		delete PfoNode;
		XMLNode * KcNode = LFParametersChildren->GetNode("Kc");
		const char * KcVal = KcNode->GetValue();
		delete KcNode;
		XMLNode * BcNode = LFParametersChildren->GetNode("Bc");
		const char * BcVal = BcNode->GetValue();
		delete BcNode;
		XMLNode * RiNode = LFParametersChildren->GetNode("Ri");
		const char * RiVal = RiNode->GetValue();
		delete RiNode;
		XMLNode * RoNode = LFParametersChildren->GetNode("Ro");
		const char * RoVal = RoNode->GetValue();
		delete RoNode;

		// Set type-specific parameters
		Pi = (Real)atof(PiVal);
		Po = (Real)atof(PoVal);
		Pfo = (Real)atof(PfoVal);
		K = (Real)atof(KcVal);
		B = (Real)atof(BcVal);
		Ri = (Real)atof(RiVal);
		Ro = (Real)atof(RoVal);

		delete PiVal;
		delete PoVal;
		delete PfoVal;
		delete KcVal;
		delete BcVal;
		delete RiVal;
		delete RoVal;
		delete LFParametersChildren;
		delete LFParametersNode;
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
//	LumpedFluidObject::InitializeVisualization()
//
//		Initialize visualization parameters
//
void LumpedFluidObject::InitializeVisualization(XMLNodeList * simObjectChildren)
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

		delete baseColorChildren;
		delete baseColorNode;
		delete visualizationChildren;
		delete visualizationNode;

		// Initialize ChamberGeometry using color information
		ChamberGeometry = new TriSurface((float)atof(red), (float)atof(green), (float)atof(blue), (float)atof(opacity));
		delete opacity;
		delete blue;
		delete green;
		delete red;
	}
	catch (...)
	{
		throw;
		return;
	}
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::LoadGeometry()
//
//		Initialize visualization parameters
//
void LumpedFluidObject::LoadGeometry(XMLNodeList * simObjectChildren)
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
//	LumpedFluidObject::InitializeTransformation()
//
//		Initialize transformation parameters
//
void LumpedFluidObject::InitializeTransformation(XMLNodeList * simObjectChildren)
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
//	LumpedFluidObject::init()
//
//		Initialize the LumpedFluidObject
//
void LumpedFluidObject::init(unsigned int num_node)
{
	unsigned int i;

	this->num_node		= num_node;

	// Allocate and init state
	state.size	= num_node;
	if((state.pos = new Vector<Real>[state.size]) == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	for(i=0; i<state.size; i++) state.pos[i] = zero_vector3;
	
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::Load()
//
//		Reads in .fem file
//
void LumpedFluidObject::Load(char *basename)   
{
	unsigned int	i;

	/************************************************************/
	/*	Load the geometric model for the heart muscle			*/
	/************************************************************/
	ChamberGeometry->Load(basename);
	
	/************************************************************/
	/*	Initialize the LumpedFluidObject						*/
	/************************************************************/
	init(ChamberGeometry->num_vertex);

	/****************************************/
	/*	Set geometry        				*/
	/****************************************/
	geometry=ChamberGeometry;

	/****************************************/
	/*	Setting up of the boundary			*/
	/****************************************/
	// Allocate boundary
	if((boundary = new LumpedFluidBoundary()) == NULL) {
		printf("ERROR: Could not allocate boundary!\n");
		exit(0);
	}
	LumpedFluidBoundary*	bound = (LumpedFluidBoundary *) boundary;
	
	// For performance we are using a reference to vertex and face lists, not a separate variable
	// NOTE:
	//   This makes the vertex list and face list of the geometry accessible
	//   through the LumpedFluidBoundary, while it was protected in SimObject
	bound->init(this,ChamberGeometry->num_vertex,ChamberGeometry->num_face,
					ChamberGeometry->vertex,ChamberGeometry->face);

	//  Fill in the rest of the boundary
	//
	for (i=0; i<bound->num_vertex; i++)
	{
		bound->global_id[i]=i;
		bound->Set(i,0,zero_vector3);
	}
	for (i=0; i<bound->num_face; i++)
	{
		*(bound->facetovertexids+3*i+0)=bound->face[i].vertex[0]->refid;
		*(bound->facetovertexids+3*i+1)=bound->face[i].vertex[1]->refid;
		*(bound->facetovertexids+3*i+2)=bound->face[i].vertex[2]->refid;
	}


	/****************************************/
	/*	Setting up of the domain			*/
	/****************************************/
	// No need for a domain


	/****************************************/
	/*	Initialize the states				*/
	/****************************************/
  
	// first find the center of the chamber
	Vector<Real>	tempv(3,0.0);
	for (i=0; i<ChamberGeometry->num_vertex; i++)
		state.pos[i]=ChamberGeometry->vertex[i].pos;
	for (i=0; i<state.size; i++)
		tempv+=state.pos[i];
	tempv=tempv*(1.0/state.size);
	state.Vf=0;
	for (i=0;i<ChamberGeometry->num_face;i++) 
		state.Vf-=TetrahedraVolume(ChamberGeometry->face[i].vertex[0]->pos,
								ChamberGeometry->face[i].vertex[1]->pos,
								ChamberGeometry->face[i].vertex[2]->pos,
								tempv);
	Vm=state.Vf;
	VfminusVm_last=0.0;
	Pf=Pfo;

	// MENTAL NOTE: move this to somewhere logical asap!
	// NOTE: The method is harcoded for now.
	integrator = new ERKHeun3<LumpedFluidObject>(*this);
	//integrator = new Euler<LumpedFluidObject>(*this);
}



////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::AccumState()
//
//		Computes x(t+h) = x(t) + h * f(..)
//
//		where	x(t+h)	= new_state
//				x(t)	= state
//				f(..)	= deriv
//
inline void LumpedFluidObject::AccumState(State &new_state, const State &state, const State &deriv, const Real &h)
{
	new_state.Vf = state.Vf + deriv.Vf * h;
	for(unsigned int i = 0; i <state.size; i++) {
		new_state.pos[i] = state.pos[i] + deriv.pos[i] * h;
	}
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::DerivState()
//
//		Computes the derivatives of state variables. This is simply 
//		the f() evaluation where f() = dx/dt.
//
//
inline void LumpedFluidObject::DerivState(State &deriv, State &state)
{
	unsigned int			i;
	LumpedFluidBoundary		*bound=(LumpedFluidBoundary *) this->boundary;
	Vector<Real>			tempv(3,0.0);
	Real					Vm,qi,qo,Pf;

	// first find the center of the chamber
	for (i=0; i<state.size; i++)
		tempv+=state.pos[i];
	tempv=tempv*(1.0/state.size);
	// now calculate chamber volume Vm, 
	Vm=0;
	for (i=0;i<bound->num_face;i++) 
		Vm-=TetrahedraVolume(state.pos[bound->face[i].vertex[0]->refid],
								state.pos[bound->face[i].vertex[1]->refid],
								state.pos[bound->face[i].vertex[2]->refid],
								tempv);
	
	for (i=0; i<state.size; i++) 
	{
		if (bound->boundary_type[i]==0) 
			deriv.pos[i]=bound->boundary_value[i];
		else 
			error_exit(-1,"Functionality not implemented or unknown boundary type...");
	}

	Pf=K*(state.Vf-Vm)+Pfo;
	qi=(Pi-Pf)/Ri;  qi=(qi>0)?qi:0;
	qo=(Pf-Po)/Ro;  qo=(qo>0)?qo:0;
	deriv.Vf=qi-qo;

	//printf("Pf=%f Vf=%f Vm=%f qi=%f qo=%f\n",Pf,state.Vf,Vm,qi,qo);

}



////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::AllocState()
//
//		Allocates the memory for the integrator's local state 
//		members
//
inline void LumpedFluidObject::AllocState(State &s)
{
	s.pos = new Vector<Real>[state.size];
	if(s.pos == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	for(unsigned int i = 0; i<state.size; i++)	s.pos[i] = zero_vector3;

	s.size = state.size;

	// no need to do anything for Vf since it doesn't require any memory allocation
}



////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::Simulate()
//
//		Integrates the FEM_3LM object by a single timestep
//
//
void LumpedFluidObject::Simulate(void)
{
	integrator->Integrate(*this, timestep);
	time+=timestep;

	//We alse need to update the Boundary from states 
	//   so that we will have the correct Boundary definition for boundary calculations
	State2Bound();

	// now calculate chamber volume Vm, 
	unsigned int i;
	LumpedFluidBoundary		*bound=(LumpedFluidBoundary *) this->boundary;
	// first find the center of the chamber
	Vector<Real>	tempv(3,0.0);
	for (i=0; i<state.size; i++)
		tempv+=state.pos[i];
	tempv=tempv*(1.0/state.size);
	// then calculate volume
	Vm=0;
	for (i=0;i<bound->num_face;i++) 
		Vm-=TetrahedraVolume(state.pos[bound->face[i].vertex[0]->refid],
								state.pos[bound->face[i].vertex[1]->refid],
								state.pos[bound->face[i].vertex[2]->refid],
								tempv);
	// and chamber pressure
	Pf=K*(state.Vf-Vm)+Pfo+B*(state.Vf-Vm-VfminusVm_last)/timestep;
	VfminusVm_last=state.Vf-Vm;

	//printf("Pf=%f Pfo=%f Vf=%f Vm=%f \n",Pf,Pfo,state.Vf,Vm);
}




////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::Display()
//
//		Displays the mesh
//
//
void LumpedFluidObject::Display(void)
{
	// Copy the state into the geometry
	State2Geom();

	displayMngr->Display();

}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::Geom2State()
//
//		Copies geometry to state
//
//
void LumpedFluidObject::Geom2State(void)
{
	static TriSurface *geom = (TriSurface *) geometry;

	for(unsigned int i=0; i < geom->num_vertex; i++) {
		state.pos[i] = geom->vertex[i].pos;
	}
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::State2Geom()
//
//		Copies state to geometry
//
//
inline void LumpedFluidObject::State2Geom(void)
{
	unsigned int		i;
	// Define an intermediate variable so that we can access the geometry without explicit type casting every time
	static TriSurface *geom = (TriSurface *) geometry;  

	// We don't need to do anything to vertex poitions with current implementation 
	//    as geometry is shared with boundary, which is updated at every time step
	// Let's just change color with Pressure

	Real	colormult=(Pf-Pi*.99)/(1.01*Po-Pi*.99);
	for (i=0; i< geom->num_vertex; i++) 
		geom->vertex[i].color[0]=geom->vertex[i].color[2]=colormult;
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::State2Bound()
//
//		Copies state to boundary
//
//
inline void LumpedFluidObject::State2Bound(void)
{
	static LumpedFluidBoundary		*bound = (LumpedFluidBoundary *) boundary;

	for(unsigned int i=0; i<bound->num_vertex; i++) {
		bound->vertex[i].pos = state.pos[bound->global_id[i]];
	}
	bound->calcNormals();
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::SetupDisplay()
//
//		Sets up the display of the LF mesh
//
//
void LumpedFluidObject::SetupDisplay(XMLNodeList * simObjectChildren)
{
	DisplayHeader	header;

	header.objType				= GIPSI_DRAW_TRIANGLE;
	header.polyMode				= GIPSI_POLYGON_FILL;
	header.shadeMode			= GIPSI_SHADE_FLAT;
	header.dataType				= 0x06;

	displayMngr = new TriSurfaceDisplayManager((TriSurface *) geometry, &header);
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
//	LumpedFluidObject::GetPressure()
//
//		Returns pressure level for a node and the whole boundary
//
//
Real	LumpedFluidObject::GetPressure(unsigned int index)
{
	return Pf;
}

void	LumpedFluidObject::GetPressure(Real *BPres)
{
	unsigned int i;
	for (i=0; i < boundary->num_vertex; i++)
	{
		*(BPres+i)= Pf;
	}
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidBoundary::init
//
//		LumpedFluidBoundary initializer
//
void	LumpedFluidBoundary::init(LumpedFluidObject *ParentObj, unsigned int num_vertex, unsigned int num_face)
{
	Object				=	ParentObj;
	this->num_vertex	=	num_vertex;
	this->num_face		=	num_face;

	vertex = new Vertex[num_vertex];
	if(vertex == NULL) {
		error_exit(-1, "Cannot allocate memory for vertices!\n");
	}
	face = new Triangle[num_face];
	if(face == NULL) {
		error_exit(-1, "Cannot allocate memory for faces!\n");
	}
	global_id = new unsigned int[num_vertex];
	if(global_id == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	facetovertexids = (unsigned int *) malloc(3*num_face*sizeof(unsigned int));
	if(facetovertexids == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	boundary_type = new unsigned int[num_vertex];
	if(boundary_type == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	boundary_value = new Vector<Real>[num_vertex];
	if(boundary_value == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
}

////////////////////////////////////////////////////////////////
//
//	LumpedFluidBoundary::init
//
//		LumpedFluidBoundary initializer
//			this version uses already existing vertex, face, and glocal idlist rather than allocating new
//
void	LumpedFluidBoundary::init(LumpedFluidObject *ParentObj, unsigned int num_vertex, unsigned int num_face,
						 Vertex *vertexlist, Triangle *facelist)
{
	Object				=	ParentObj;
	this->num_vertex	=	num_vertex;
	this->num_face		=	num_face;

	vertex				=	vertexlist;
	face				=	facelist;

	global_id = new unsigned int[num_vertex];
	if(global_id == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	facetovertexids = (unsigned int *) malloc(3*num_face*sizeof(unsigned int));
	if(facetovertexids == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	boundary_type = new unsigned int[num_vertex];
	if(boundary_type == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	boundary_value = new Vector<Real>[num_vertex];
	if(boundary_value == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidBoundary::GetPressure()
//
//		Returns pressure for a node and the whole boundary
//
//
Real	LumpedFluidBoundary::GetPressure(unsigned int index)
{
	return Object->GetPressure(index);
}

void	LumpedFluidBoundary::GetPressure(Real *BPres)
{
	Object->GetPressure(BPres);
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidBoundary::Set
//
//		Sets boundary conditions for one node and whole boundary
//
void	LumpedFluidBoundary::Set(int index, unsigned int boundary_type, Vector<Real> boundary_value)
{
	this->boundary_type[index]			=boundary_type;
	this->boundary_value[index]			=boundary_value;
}

void	LumpedFluidBoundary::Set(unsigned int *boundary_type, Vector<Real> *boundary_value)
{
	for (unsigned int index=0; index < this->num_vertex; index++){
		this->boundary_type[index]			=boundary_type[index];
		this->boundary_value[index]			=boundary_value[index];
	}
}

