/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is 3D Linear FEM Module Implementation (fem.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	FEM.CPP v0.0
////
////	3D Linear FEM Module
////
////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "algebra.h"
#include "fem.h"
#include "errors.h"
#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "GiPSiException.h"
#include "load_mesh.h"
#include "logger.h"
#include "timing.h"
#include "XMLNodeList.h"


////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::FEM_3LMObject()
//
//		Constructor
//
FEM_3LMObject::FEM_3LMObject(XMLNode * simObjectNode,
							 Real in_g,
							 Real mass)
							 :	DeformableSolidObject(simObjectNode),
								g(in_g),
								defaultMass(mass)
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

		// Load geometry
		LoadGeometry(simObjectChildren);

		// Set object parameters
		SetParameters(simObjectChildren);

		// Initialize visualization
		InitializeVisualization(simObjectChildren);

		// NOTE: The BCs are hardcoded here. Move them into fem file!
		FEMBoundary * bound = (FEMBoundary *)GetBoundaryPtr();
		for(unsigned int i = 0; i < bound->num_vertex; i++) {
			// NOTE: Boundary conditions are hard coded for now
			//if (0) {
			if ((bound->vertex[i].pos[0] < 1.5) && (bound->vertex[i].pos[0] > -1.5)) {
				bound->Set(i,1,bound->vertex[i].pos,0.0,zero_vector3);	// Fixed boundary (Wall)
			}
		}

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
//	FEM_3LMObject::SetParameters()
//
//		Set object parameters
//
void FEM_3LMObject::SetParameters(XMLNodeList * simObjectChildren)
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
		XMLNode * FEMParametersNode = modelParametersChildren->GetNode("FEMParameters");
		XMLNodeList * FEMParametersChildren = FEMParametersNode->GetChildren();

		XMLNode * RhoNode = FEMParametersChildren->GetNode("Rho");
		const char * RhoVal = RhoNode->GetValue();
		delete RhoNode;

		XMLNode * MuNode = FEMParametersChildren->GetNode("Mu");
		const char * MuVal = MuNode->GetValue();
		delete MuNode;

		XMLNode * LambdaNode = FEMParametersChildren->GetNode("Lambda");
		const char * LambdaVal = LambdaNode->GetValue();
		delete LambdaNode;

		XMLNode * NuNode = FEMParametersChildren->GetNode("Nu");
		const char * NuVal = NuNode->GetValue();
		delete NuNode;

		XMLNode * PhiNode = FEMParametersChildren->GetNode("Phi");
		const char * PhiVal = PhiNode->GetValue();
		delete PhiNode;

		// Set type-specific parameters
		SetMaterial((Real)atof(RhoVal), (Real)atof(MuVal), (Real)atof(LambdaVal), (Real)atof(NuVal), (Real)atof(PhiVal));
		delete RhoVal;
		delete MuVal;
		delete LambdaVal;
		delete NuVal;
		delete PhiVal;

		delete FEMParametersChildren;
		delete FEMParametersNode;
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
//	FEM_3LMObject::InitializeVisualization()
//
//		Initialize visualization parameters
//
void FEM_3LMObject::InitializeVisualization(XMLNodeList * simObjectChildren)
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
		initialcolor[0] = atof(red);
		delete red;
		delete redNode;

		// Extract green
		XMLNode * greenNode = baseColorChildren->GetNode("green");
		const char * green = greenNode->GetValue();
		initialcolor[1] = atof(green);
		delete green;
		delete greenNode;

		// Extract blue
		XMLNode * blueNode = baseColorChildren->GetNode("blue");
		const char * blue = blueNode->GetValue();
		initialcolor[2] = atof(blue);
		delete blue;
		delete blueNode;

		// Extract opacity
		XMLNode * opacityNode = baseColorChildren->GetNode("opacity");
		const char * opacity = opacityNode->GetValue();
		initialcolor[3] = atof(opacity);
		delete opacity;
		delete opacityNode;

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
//	FEM_3LMObject::LoadGeometry()
//
//		Initialize visualization parameters
//
void FEM_3LMObject::LoadGeometry(XMLNodeList * simObjectChildren)
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
//	FEM_3LMObject::InitializeTransformation()
//
//		Initialize transformation parameters
//
void FEM_3LMObject::InitializeTransformation(XMLNodeList * simObjectChildren)
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
//	FEM_3LMObject::init()
//
//		Initialize FEM_3LMObject
//
void FEM_3LMObject::init (	unsigned int num_node, unsigned int num_element) 
{
	unsigned int i;

	this->num_node		= num_node;
	this->num_element	= num_element;

	// Allocate geometry
	if((geometry = new TetVolume((float)initialcolor[0],(float)initialcolor[1],(float)initialcolor[2],(float)initialcolor[3])) == NULL) {
		printf("ERROR: Could not allocate geometry!\n");
		exit(0);
	}

	// Allocate and init state
	state.size	= num_node;
	if((state.pos = new Vector<Real>[state.size]) == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	for(i=0; i<state.size; i++) state.pos[i] = zero_vector3;
	
	if((state.vel = new Vector<Real>[state.size]) == NULL) {
		error_exit(-1, "Cannot allocate memory for velocity!\n");
	}
	for(i=0; i<state.size; i++) state.vel[i] = zero_vector3;

	// Allocate and init reference positions
	if((rcpos = new Vector<Real>[num_node]) == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	for(i=0; i<num_node; i++)	rcpos[i] = zero_vector3;

	// Allocate and init forces
	if((force = new Vector<Real>[num_node]) == NULL) {
		error_exit(-1, "Cannot allocate memory for forces!\n");
	}
	for(i=0; i<num_node; i++)	force[i] = zero_vector3;
	
	// Allocate mass
	Vector<Real>	m(num_node, 0.0);
	mass = m;

	// Allocate elements
	if((elements = new Tetrahedra3DFEMElement[num_element]) == NULL) {
		error_exit(-1, "Cannot allocate memory for elements!\n");
	}
}

////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::Load()
//
//		Reads in .fem file
//
void FEM_3LMObject::Load(char *basename)   
{
	unsigned int	i;
	LoadData		*data;
	char			*ext, *name;

	ext=strrchr(basename,'.');
	if (ext==NULL) {
		printf("Unknown tetrahedra geometry file type for FEM_3LMObject.\n");
		exit(1);
	}
	if (!strcmp(ext,".node")) {
		// Load Pyramid/Triangle file format
		name = (char *) malloc(sizeof(char)*(strlen(basename)+1));
		sprintf_s(name,strlen(basename)+1,"%s", basename);
		name[ext-basename]=0x00;		
		
		data = LoadNode(name);

		free(name);
	}
	else if (!strcmp(ext,".neu")) {
		// Load NetGen Neutral file format
        data = LoadNeutral(basename);
	}
	else 
	{
		printf("Unknown tetrahedra geometry file type for FEM_3LMObject.\n");
		exit(1);
	}

	if(data->type != TETRAHEDRA) {
		printf("ERROR: Not a volume object!\n");
		exit(0);
	}

	init(data->num_node, data->num_element);

	// Load geometry
	geometry->Load(data);


	/****************************************/
	/*	Setting up of the boundary			*/
	/****************************************/
	// Allocate boundary
	if((boundary = new FEMBoundary()) == NULL) {
		printf("ERROR: Could not allocate boundary!\n");
		exit(0);
	}
	FEMBoundary*	bound = (FEMBoundary *) boundary;
	
	//let's count the number of boundary faces
	unsigned int num_bface = 0;
	for(i = 0; i < (unsigned int)(data->num_face); i++) {
		if(data->face[i].boundary)	num_bface++;
	}

	//let's count the number of boundary vertices
	unsigned int num_bvertex = 0;
	for(i = 0; i < (unsigned int)(data->num_node); i++) {
		if(data->node[i].boundary)	num_bvertex++;
	}

	//and finally initialize the boundary
	bound->init(this, num_bvertex, num_bface);


	/****************************************/
	/*	Setting up of the domain			*/
	/****************************************/
	// Allocate domain
	if((domain = new FEMDomain()) == NULL) {
		printf("ERROR: Could not allocate boundary!\n");
		exit(0);
	}
	FEMDomain *		dom = (FEMDomain *) domain;

	//and then load the domain
	dom->Load(data);
	dom->init(this);

  
	/****************************************/
	/*	Read vertices						*/
	/****************************************/
	unsigned int	bno = 0;
	unsigned int	*g2b = new unsigned int[data->num_node];		// Global to boundary reference table
	for(i=0; i<(unsigned int)(data->num_node); i++) {
		// Initialize state
		this->state.pos[i] = data->node[i].pos;
		this->state.vel[i] = 0.0;

		// Initialize other node values
		this->rcpos[i]	= data->node[i].pos;	// Reference Config
		this->mass[i]	= 0.0;
		this->force[i]	= 0.0;

		//
		// Initialize nodes of the boundary
		//
		if(data->node[i].boundary) {
			bound->vertex[bno].refid	= bno;
			bound->vertex[bno].pos		= data->node[i].pos;
			bound->global_id[bno]		= i;
			bound->Set(bno,0,zero_vector3,0.0,zero_vector3);			// Free boundary (zero traction)

			g2b[i] = bno;

			bno++;
		}
	}
	printf("%d nodes with %d boundary nodes\n",data->num_node,bno);
  
	/****************************************/
	/*	Read faces							*/
	/****************************************/
	unsigned int	fno = 0;
	for(i=0; i<(unsigned int)(data->num_face); i++) {
		//
		// Initialize faces of the boundary
		//
		if(data->face[i].boundary) {
			bound->face[fno].vertex[0] = &(bound->vertex[g2b[data->face[i].node[0]]]);
			bound->face[fno].vertex[1] = &(bound->vertex[g2b[data->face[i].node[1]]]);
			bound->face[fno].vertex[2] = &(bound->vertex[g2b[data->face[i].node[2]]]);

			*(bound->facetovertexids+3*fno+0) = g2b[data->face[i].node[0]];
			*(bound->facetovertexids+3*fno+1) = g2b[data->face[i].node[1]];
			*(bound->facetovertexids+3*fno+2) = g2b[data->face[i].node[2]];

			bound->face[fno].refid=i;

			fno++;
		}
	}
	printf("%d faces with %d boundary faces\n",data->num_face,fno);

	/****************************************/
	/*	Read tets							*/
	/****************************************/
	unsigned int		nid[4];
	for(i=0; i<(unsigned int)(data->num_element); i++) {
		for(unsigned int j=0; j<4; j++)	
			nid[j] = data->element[i].node[j];

		elements[i].init(nid);

		Real volume = elements[i].computeVolume( state.pos[nid[0]], state.pos[nid[1]], state.pos[nid[2]], state.pos[nid[3]]);
		elements[i].computeBeta( state.pos[nid[0]], state.pos[nid[1]], state.pos[nid[2]], state.pos[nid[3]]);
	}
	printf("%d tetrahedral elements\n",data->num_element);
	
	delete[] g2b;
  

	// NOTE: The method is harcoded for now.
	integrator = new ERKHeun3<FEM_3LMObject>(*this);
	//integrator = new Euler<FEM_3LMObject>(*this);
}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::UpdateForces()
//
//		Updates the forces
//
void FEM_3LMObject::UpdateForces(State &state)   
{
	unsigned int		i;
	static				Real g_v[3] = {0.0, -g, 0.0};		// NOTE: this is ugly
	static	const		Vector<Real> g_Vector(3, g_v), zero_vector3(3, 0.0);
	static				Vector<Real> g_force, d_force, s_force;

	FEMDomain			*dom = (FEMDomain *) domain;

	// Clear the forces
	for(i = 0; i < state.size; i++) force[i] = zero_vector3;

	// Update them by adding each component
	for(i = 0; i < state.size; i++) {
		// Add gravity
		g_force = g_Vector;
		g_force *= mass[i]; 
//		force[i] += g_force;
	}

	for(unsigned int k = 0; k < num_element; k++) {
		Vector<Real>	&p0 = state.pos[elements[k].node_id[0]];
		Vector<Real>	&p1 = state.pos[elements[k].node_id[1]];
		Vector<Real>	&p2 = state.pos[elements[k].node_id[2]];
		Vector<Real>	&p3 = state.pos[elements[k].node_id[3]];

		Vector<Real>	&v0 = state.vel[elements[k].node_id[0]];
		Vector<Real>	&v1 = state.vel[elements[k].node_id[1]];
		Vector<Real>	&v2 = state.vel[elements[k].node_id[2]];
		Vector<Real>	&v3 = state.vel[elements[k].node_id[3]];


		elements[k].computeStrain(p0, p1, p2, p3);
		elements[k].computeStrainVelocity(p0, p1, p2, p3, v0, v1, v2, v3);

		elements[k].computeStress();
		elements[k].stress += dom->DomStress[k];

		elements[k].computeForces(p0, p1, p2, p3);
		
		for(unsigned int j=0; j<4; j++) 
			force[elements[k].node_id[j]] += elements[k].NodeForce[j];
	}
}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::AccumState()
//
//		Computes x(t+h) = x(t) + h * f(..)
//
//		where	x(t+h)	= new_state
//				x(t)	= state
//				f(..)	= deriv
//
inline void FEM_3LMObject::AccumState(State &new_state, const State &state, const State &deriv, const Real &h)
{
	unsigned int i;
	FEMBoundary		*bound = (FEMBoundary *) boundary;

	// NOTE: The below is a good point to measure the overhead caused by binary operators
	for(i = 0; i < state.size; i++) {
		new_state.pos[i] = state.pos[i] + deriv.pos[i] * h;
		new_state.vel[i] = state.vel[i] + deriv.vel[i] * h;
	}
	for(i = 0; i < bound->num_vertex; i++) {
		switch (bound->boundary_type[i]) {
			case (0):   // Neumann type boundary condition   (Specify traction)
				break;
			case (1):   // Drichlett type boundary condition (Fixed boundary - e.g. wall)
					    // Position specified in boundary_value
				new_state.pos[bound->global_id[i]]=bound->boundary_value[i];
				break;
			case (2):  // Mixed boundary condition type i: 
					   // Normal displacement (not position) specified in boundary_value2_scalar
					   //        with normal direction specified in boundary_value2_vector
					   //   and tangential traction specified in boundary_value
				new_state.pos[bound->global_id[i]]+=bound->boundary_value2_scalar[i] * bound->boundary_value2_vector[i];
				break;
			default:
				error_exit(0,"Unrecognized boundary condition type\n");
		}
	}

}




////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::DerivState()
//
//		Computes the derivatives of state variables. This is simply 
//		the f() evaluation where f() = dx/dt.
//
//
inline void FEM_3LMObject::DerivState(State &deriv, State &state)
{
	unsigned int i;
	FEMBoundary		*bound = (FEMBoundary *) boundary;

	UpdateForces(state);

	for(i = 0; i < state.size; i++) {
		// dpos/dt = vel
		deriv.pos[i] = state.vel[i];

		// dvel/dt = force/m
		deriv.vel[i] = force[i];
		deriv.vel[i] /= mass[i];
	}
	for(i = 0; i < bound->num_vertex; i++) {
		switch (bound->boundary_type[i]) {
			case (0):   // Neumann type boundary condition   (Specify traction)
				deriv.vel[bound->global_id[i]] += bound->boundary_value[i]*(1.0/mass[bound->global_id[i]]);
				break;
			case (1):   // Drichlett type boundary condition (Fixed boundary - e.g. wall)
					    // Position specified in boundary_value
				deriv.vel[bound->global_id[i]] = 0.0;
				deriv.pos[bound->global_id[i]] = 0.0;
				break;
			case (2):  // Mixed boundary condition type i: 
					   // Normal displacement (not position) specified in boundary_value2_scalar
					   //        with normal direction specified in boundary_value2_vector
					   //   and tangential traction specified in boundary_value
				deriv.vel[bound->global_id[i]] += bound->boundary_value[i]*(1.0/mass[bound->global_id[i]]);
				deriv.vel[bound->global_id[i]] -= bound->boundary_value2_vector[i] * (deriv.vel[bound->global_id[i]] * bound->boundary_value2_vector[i]);
				deriv.pos[bound->global_id[i]] -= bound->boundary_value2_vector[i] * (deriv.pos[bound->global_id[i]] * bound->boundary_value2_vector[i]);
				break;
			default:
				error_exit(0,"Unrecognized boundary condition type\n");
		}
	}
}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::AllocState()
//
//		Allocates the memory for the integrator's local state 
//		members
//
inline void FEM_3LMObject::AllocState(State &s)
{
	s.pos = new Vector<Real>[state.size];
	if(s.pos == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	for(unsigned int i = 0; i<state.size; i++)	s.pos[i] = zero_vector3;

	s.vel = new Vector<Real>[state.size];
	if(s.vel == NULL) {
		error_exit(-1, "Cannot allocate memory for velocity!\n");
	}
	//for(i = 0; i<state.size; i++)		s.vel[i] = zero_vector3;
	// why was this changed?
	for(unsigned int i = 0; i<state.size; i++)		
		s.vel[i] = zero_vector3;

	s.size = state.size;
}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::Simulate()
//
//		Simulates the FEM_3LM object by a single timestep
//
//
void FEM_3LMObject::Simulate(void)
{
	integrator->Integrate(*this, timestep);
	time+=timestep;

	//We alse need to update the Boundary from states 
	//   so that we will have the correct Boundary definition for boundary calculations
	State2Bound();

}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::Geom2State()
//
//		Copies geometry to state
//
//
void FEM_3LMObject::Geom2State(void)
{
	static TetVolume *geom = (TetVolume *) geometry;

	for(unsigned int i=0; i < geom->num_vertex; i++) {
		state.pos[i] = geom->vertex[i].pos;
	}
}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::State2Geom()
//
//		Copies state to geometry
//
//
inline void FEM_3LMObject::State2Geom(void)
{
	static TetVolume *geom = (TetVolume *) geometry;

	for(unsigned int i=0; i<geom->num_vertex; i++) {
		geom->vertex[i].pos = state.pos[i];
	}
}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::State2Bound()
//
//		Copies state to boundary
//
//
inline void FEM_3LMObject::State2Bound(void)
{
	static FEMBoundary		*bound = (FEMBoundary *) boundary;

	for(unsigned int i=0; i<bound->num_vertex; i++) {
		bound->vertex[i].pos = state.pos[bound->global_id[i]];
	}
	bound->calcNormals();
}

////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::ResetInitialBoundaryCondition()
//
//		Reset boundary condition to initial values.
//
//
inline void FEM_3LMObject::ResetInitialBoundaryCondition(void)
{
}

////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::SetupDisplay()
//
//		Sets up the display of the FEM mesh
//
//
void FEM_3LMObject::SetupDisplay(XMLNodeList * simObjectChildren)
{
	DisplayHeader	header;

	header.objType				= GIPSI_DRAW_TRIANGLE;
	header.polyMode				= GIPSI_POLYGON_FILL;
	header.shadeMode			= GIPSI_SHADE_FLAT;
	header.dataType				= 0x06;

	displayMngr = new TetVolumeDisplayManager((TetVolume *) geometry, &header);
	// Use this if you want to display the boundary surface instead of the tet volume
    //displayMngr = new TriSurfaceDisplayManager((TriSurface *) boundary, header);
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
//	FEM_3LMObject::Display()
//
//		Displays the FEM mesh
//
//
void FEM_3LMObject::Display(void)
{
	// Copy the state into the geometry
	State2Geom();

	displayMngr->Display();
}


////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::GetNodePosition()
//
//		Returns node position
//
//
Vector<Real>		FEM_3LMObject::GetNodePosition(unsigned int index)
{
	return state.pos[index];
}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::GetNodeVelocity()
//
//		Returns node velocity
//
//
Vector<Real>		FEM_3LMObject::GetNodeVelocity(unsigned int index)
{
	return	state.vel[index];
}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::GetNodeForce()
//
//		Returns the nodal force
//         
//
Vector<Real>		FEM_3LMObject::GetNodeForce(unsigned int index)
{
	return	force[index];
}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::SetMaterial
//
//		Sets Material properties
//
void		FEM_3LMObject::SetMaterial(unsigned int element_index, Real Rho, Real Mu, Real Lambda, Real Nu, Real Phi)
{
	
	elements[element_index].density		=	Rho;
	elements[element_index].Mu			=	Mu;
	elements[element_index].Lambda		=	Lambda;
	elements[element_index].Nu			=	Nu;
	elements[element_index].Phi			=	Phi;

	// NOTE:	This function does not update the mass. You need to call updateMass()
	//			after you are done setting materials of elements.
}

void		FEM_3LMObject::SetMaterial(Real *Rho, Real *Mu, Real *Lambda, Real *Nu, Real *Phi)
{
	for (unsigned int index=0; index < num_element; index++){
		elements[index].density		=	Rho[index];
		elements[index].Mu			=	Mu[index];
		elements[index].Lambda		=	Lambda[index];
		elements[index].Nu			=	Nu[index];
		elements[index].Phi			=	Phi[index];
	}

	updateMass();
}


void		FEM_3LMObject::SetMaterial(Real Rho, Real Mu, Real Lambda, Real Nu, Real Phi)
{
	for (unsigned int index=0; index < num_element; index++){
		elements[index].density		=	Rho;
		elements[index].Mu			=	Mu;
		elements[index].Lambda		=	Lambda;
		elements[index].Nu			=	Nu;
		elements[index].Phi			=	Phi;
	}

	updateMass();
}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::updateMass
//
//		Recomputes the element mass values
//
void		FEM_3LMObject::updateMass(void)
{
	mass = 0.0;

	for (unsigned int index=0; index < num_element; index++){
		for(unsigned int j=0; j<4; j++)
			mass[elements[index].node_id[j]] += elements[index].density * elements[index].volume/4.0; 
	}

}



////////////////////////////////////////////////////////////////
//
//	FEMBoundary::init
//
//		FEM Boundary initializer
//
void			FEMBoundary::init(FEM_3LMObject *ParentObj, unsigned int num_vertex, unsigned int num_face)
{
	unsigned int i;

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
	for(i=0; i<num_vertex; i++)	boundary_value[i] = zero_vector3;
	boundary_value2_scalar = new Real[num_vertex];
	if(boundary_value2_scalar == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	boundary_value2_vector = new Vector<Real>[num_vertex];
	if(boundary_value2_vector == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	for(i=0; i<num_vertex; i++)	boundary_value2_vector[i] = zero_vector3;
	
}



////////////////////////////////////////////////////////////////
//
//	FEMBoundary::GetPosition
//
//		Get Positions of the nodes of the boundary
//
Vector<Real>	FEMBoundary::GetPosition(unsigned int index) 
{
	return	Object->GetNodePosition(global_id[index]);
}

void			FEMBoundary::GetPosition(Vector<Real> *Bpos)
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bpos[index]=Object->GetNodePosition(global_id[index]);
	}
}



////////////////////////////////////////////////////////////////
//
//	FEMBoundary::GetVelocity
//
//		Get Velocities of the nodes of the boundary
//

Vector<Real>	FEMBoundary::GetVelocity	(unsigned int index)
{
	return	Object->GetNodeVelocity(global_id[index]);
}

void			FEMBoundary::GetVelocity	(Vector<Real> *Bvel)	
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bvel[index]=Object->GetNodeVelocity(global_id[index]);
	}
}


////////////////////////////////////////////////////////////////
//
//	FEMBoundary::ResidualForce
//
//		Get the Reaction force that the body applies to the external 
//         world as a result of the constraints
//		   (i.e. contact force, constraint force, lagrange multiplier)
//
Vector<Real>	FEMBoundary::GetReactionForce	(unsigned int index)
{
	Vector<Real>	ResForce;

	switch (boundary_type[index]) {
		case (0):   // Neumann type boundary condition   (Specify traction)
			ResForce =	0.0;
			break;
		case (1):   // Drichlett type boundary condition (Fixed boundary - e.g. wall)
					// Position specified in boundary_value
			ResForce =	Object->GetNodeForce(global_id[index]);
			break;
		case (2):  // Mixed boundary condition type i: 
				// Normal displacement (not position) specified in boundary_value2_scalar
				//        with normal direction specified in boundary_value2_vector
				//   and tangential traction specified in boundary_value
			ResForce =	boundary_value2_vector[index] * (Object->GetNodeForce(global_id[index]) * boundary_value2_vector[index]);
			break;
		default:
			error_exit(0,"Unrecognized boundary condition type\n");
	}

	return ResForce;
}

void			FEMBoundary::GetReactionForce	(Vector<Real> *Bforce)
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bforce[index]=GetReactionForce(global_id[index]);
	}
}




////////////////////////////////////////////////////////////////
//
//	FEMBoundary::Set
//
//		Sets FEM Boundary
//
void		FEMBoundary::Set(unsigned int index, unsigned int boundary_type, 
							 Vector<Real> boundary_value, 
							 Real boundary_value2_scalar, Vector<Real> boundary_value2_vector) 
{
	this->boundary_type[index]			=boundary_type;
	this->boundary_value[index]			=boundary_value;
	this->boundary_value2_scalar[index]	=boundary_value2_scalar;
	this->boundary_value2_vector[index]	=boundary_value2_vector;		
}

void		FEMBoundary::Set(unsigned int *boundary_type, 
							 Vector<Real> *boundary_value, 
							 Real *boundary_value2_scalar, Vector<Real> *boundary_value2_vector)
{
	for (unsigned int index=0; index < this->num_vertex; index++){
		this->boundary_type[index]			=boundary_type[index];
		this->boundary_value[index]			=boundary_value[index];
		this->boundary_value2_scalar[index]	=boundary_value2_scalar[index];
		this->boundary_value2_vector[index]	=boundary_value2_vector[index];
	}
}

/**
 * FEMBoundary::ResetBoundaryCondition()
 * Reset boundary condition.
 */
void FEMBoundary::ResetBoundaryCondition(void)
{
}

/**
 * FEMBoundary::ReturnHapticModel()
 * Returns a haptic model.
 * @param BoundaryNodeIndex boundary node index.
 * @param Model GiPSi low order linear Haptic model.
 * @return int result. 
 */
int	FEMBoundary::ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model)
{
	return Object->ReturnHapticModel(BoundaryNodeIndex, Model);
}

////////////////////////////////////////////////////////////////
//
//	FEMDomain::init
//
//		FEM Domain initializer
//
void			FEMDomain::init(FEM_3LMObject *ParentObj)
{
	unsigned int i;

	Object=ParentObj;

	DomStress = new Matrix<Real>[num_tet];
	if(DomStress == NULL) {
		error_exit(-1, "Cannot allocate memory for domain stresses!\n");
	}	
	for(i=0; i<num_tet; i++)	DomStress[i] = zero_Matrix;

}



////////////////////////////////////////////////////////////////
//
//	FEMDomain::SetDomStress
//
//		Sets External Stresses
//
void	FEMDomain::SetDomainStress(unsigned int element_index, Matrix<Real> Stress_Tensor)
{
	DomStress[element_index]=Stress_Tensor;
}



////////////////////////////////////////////////////////////////
//
//	FEMDomain::SetDomStress
//
//		Sets External Stresses
//

void		FEMDomain::SetDomainStress(Matrix<Real> *Stress_Tensor_Array)
{
	for (unsigned int index=0; index < this->num_tet; index++){
		DomStress[index]=Stress_Tensor_Array[index];
	}
}


////////////////////////////////////////////////////////////////
//
//	FEMDomain::SetMaterial
//
//		Sets Material properties
//
void		FEMDomain::SetMaterial(unsigned int element_index, Real Rho, Real Mu, Real Lambda, Real Nu, Real Phi)
{
	((FEM_3LMObject*)Object)->SetMaterial(element_index, Rho, Mu, Lambda, Nu, Phi);
}

void		FEMDomain::SetMaterial(Real *Rho, Real *Mu, Real *Lambda, Real *Nu, Real *Phi)
{
	((FEM_3LMObject*)Object)->SetMaterial(Rho, Mu, Lambda, Nu, Phi);
}


