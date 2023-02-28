/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Test Object Model Implementation (collisiontest.cpp).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	CollisionTest.cpp	v0.0
////
////	Collision test object class.
////
////////////////////////////////////////////////////////////////


#include <stdlib.h>
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
#include "collisiontest.h"
#include "XMLNodeList.h"

#define		DELIM 10


////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::CollisionTestObject()
//
//		Constructor
//
CollisionTestObject::CollisionTestObject(	XMLNode * simObjectNode)				
						:	SolidObject(simObjectNode),g(0.0)
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

		VF_flag = true; 

		XMLNodeList * simObjectChildren = simObjectNode->GetChildren();
		
		// Initialize visualization
		InitializeVisualization(simObjectChildren);

		// Load geometry
		LoadGeometry(simObjectChildren);

		// Initialize transformation
		InitializeTransformation(simObjectChildren);
		
		// Load model files
		LoadModelFiles(simObjectChildren);

		// Final setup
		Geom2State();
		State2Bound();
		SetupDisplay(simObjectChildren);

		mass = 1;	
		if (strcmp(GetName(), "FIX") == 0)
			boundaryfix = true;
		else
			boundaryfix = false;

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
//	CollisionTestObject::InitializeVisualization()
//
//		Initialize visualization parameters
//
void CollisionTestObject::InitializeVisualization(XMLNodeList * simObjectChildren)
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

		// Extract green
		XMLNode * greenNode = baseColorChildren->GetNode("green");
		const char * green = greenNode->GetValue();

		// Extract blue
		XMLNode * blueNode = baseColorChildren->GetNode("blue");
		const char * blue = blueNode->GetValue();

		// Extract opacity
		XMLNode * opacityNode = baseColorChildren->GetNode("opacity");
		const char * opacity = opacityNode->GetValue();

		// Initialize HeartMuscleGeometry using color information
		initialcolor[0] = (float)atof(red);
		initialcolor[1] = (float)atof(green);
		initialcolor[2] = (float)atof(blue);
		initialcolor[3] = (float)atof(opacity);

		delete red;
		delete green;
		delete blue;
		delete opacity;
	}
	catch (...)
	{
		throw;
		return;
	}
}

////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::LoadGeometry()
//
//		Initialize visualization parameters
//
void CollisionTestObject::LoadGeometry(XMLNodeList * simObjectChildren)
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
//	CollisionTestObject::InitializeTransformation()
//
//		Initialize transformation parameters
//
void CollisionTestObject::InitializeTransformation(XMLNodeList * simObjectChildren)
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
//	CollisionTestObject::LoadModelFiles()
//
void CollisionTestObject::LoadModelFiles(XMLNodeList * simObjectChildren)
{
	try
	{
		// Get path and file name information
		XMLNode * objParametersNode = simObjectChildren->GetNode("objParameters");
		XMLNodeList * objParametersChildren = objParametersNode->GetChildren();
		XMLNode * NHParametersNode = objParametersChildren->GetNode("NHParameters");
		XMLNodeList * NHParametersChildren = NHParametersNode->GetChildren();
		
		// Intergration Information
		XMLNode * numericMethodNode = NHParametersChildren->GetNode("numericMethod");
		const char * numericMethod = numericMethodNode->GetValue();

		delete numericMethod;

		// Call Init
		Init();
	}
	catch (...)
	{
		throw;
		return;
	}
}

////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::Load()
//
//		Reads in .obj file
//
void CollisionTestObject::Load(char *filename)   
{
	TriSurface	*geom;
	VectorField *vf_geom;
  
	/****************************************/
	/*	Set up geometry						*/
	/****************************************/
	if((geometry = new TriSurface(initialcolor[0],initialcolor[1],initialcolor[2],initialcolor[3])) == NULL) {
		printf("ERROR: Could not allocate geometry!\n");
		exit(0);
	}
	geom = (TriSurface *) geometry;

	geom->Load(filename);
	// should we replace this with the following?
	// LoadData *data;
	// data=LoadObj(filename);
	// geom->Load(data);

	geom->calcNormals();

	num_vertex = geom->num_vertex;
	
	if(VF_flag==1)
	{		
		if((VF_geometry = new VectorField(initialcolor[0],initialcolor[1],initialcolor[2],initialcolor[3])) == NULL) {
			printf("ERROR: Could not allocate geometry!\n");
			exit(0);
		}
		vf_geom = (VectorField *) VF_geometry;

		vf_geom->Load(filename);		
	}
}

////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::Init()
//
//	initilize the collision test object
//
void CollisionTestObject::Init(void)
{
	TriSurface *geom = (TriSurface *) geometry;	
	VectorField *vf_geom = (VectorField *) VF_geometry;
	int				v1, v2,	v3;
	unsigned int	i;


	// Allocate force
	force = new Vector<Real>[num_vertex];
	if(force == NULL) {
		error_exit(-1, "Cannot allocate memory for forces!\n");
	}

	/****************************************/
	/*	Set up states						*/
	/****************************************/
	state.size	= num_vertex;

	state.pos = new Vector<Real>[num_vertex];
	if(state.pos == NULL) {
		error_exit(-1, "Cannot allocate memory for state.pos!\n");
	}

	state.vel = new Vector<Real>[num_vertex];
	if(state.vel == NULL) {
		error_exit(-1, "Cannot allocate memory for state.vel!\n");
	}

	for(i=0;i<num_vertex;i++) 
	{
		// Initialize state
		this->state.pos[i]	= geom->vertex[i].pos;
		this->state.vel[i]	= zero_vector3;
	}

	/****************************************/
	/*	Set up boundary						*/
	/****************************************/
	
	// Allocate boundary
	if((boundary = new CollisionTestBoundary()) == NULL) {
		printf("ERROR: Could not allocate boundary!\n");
		exit(0);
	}

	CollisionTestBoundary*	bound = (CollisionTestBoundary *) boundary;
	
	//initialize the boundary in obj file all vertex and face are boundary
	bound->init(this, geom->num_vertex, geom->num_face);
	
	unsigned int	j = 0;
	unsigned int	bno = 0;
	unsigned int	*g2b = new unsigned int[num_vertex];		// Global to boundary reference table
	
	for(i=0;i<geom->num_vertex;i++) 
	{
		//
		// Initialize nodes of the boundary
		//
		bound->vertex[bno].refid	= bno;
		bound->vertex[bno].pos		= geom->vertex[i].pos;
		bound->vertex[bno].color	= geom->vertex[i].color;
		bound->global_id[bno]		= i;
		bound->Set(bno,0,zero_vector3,0.0,zero_vector3);			// Free boundary (zero traction)
		g2b[i] = bno;
		bno++;

		vf_geom->vertex[i].pos = geom->vertex[i].pos;
		vf_geom->vertex[i].color = geom->vertex[i].color;
	}

	unsigned int	fno = 0;
	for(i=0;i<geom->num_face;i++) 
	{
		//
		// Initialize faces of the boundary
		//
		v1 = geom->face[i].vertex[0]->refid;
		v2 = geom->face[i].vertex[1]->refid;
		v3 = geom->face[i].vertex[2]->refid;
		bound->face[fno].vertex[0] = &(bound->vertex[g2b[v1]]);
		bound->face[fno].vertex[1] = &(bound->vertex[g2b[v2]]);
		bound->face[fno].vertex[2] = &(bound->vertex[g2b[v3]]);
		*(bound->facetovertexids+3*fno+0) = g2b[v1];
		*(bound->facetovertexids+3*fno+1) = g2b[v2];
		*(bound->facetovertexids+3*fno+2) = g2b[v3];
		bound->face[fno].refid=i;
		fno++;
	}

	delete[] g2b;

	/****************************************/
	/*	Set up integrator					*/
	/****************************************/
	// MENTAL NOTE: move this to somewhere logical asap!
	integrator = new ERKHeun3<CollisionTestObject>(*this);
}

////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::UpdateForces()
//
//		Updates the forces
//
inline void CollisionTestObject::UpdateForces(State &state)   
{
	static			Real gg[3] = { 0.0, -g, 0.0 };
	static			const Vector<Real> g_vector(3, gg);
	static			Vector<Real> g_force(3);

	// Clear the forces
	for(int i = 0; i < state.size; i++) force[i] = zero_vector3;
	
	// Update them by adding each component
	for(int i = 0; i < state.size; i++) {
		// Add gravity
		g_force = g_vector;
		g_force *= mass; 
		force[i] += g_force;

		// Add global damping
		//d_force = state.vel[i];
		//d_force *= -D;
		//force[i] += d_force;

		// NOTE: Add others here...
	}
}

////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::AccumState()
//
//		Computes y(t+h) = y(t) + h * f(..)
//
//		where	y(t+h)	= new_state
//				y(t)	= state
//				f(..)	= deriv
//
inline void CollisionTestObject::AccumState(State &new_state, const State &state, const State &deriv, const Real &h)
{
	unsigned int				i;
	static Vector<Real>			temp(3);
	CollisionTestBoundary		*bound = (CollisionTestBoundary *) boundary;

	// NOTE: The below is a good point to measure the overhead caused by binary operators
	for(i = 0; i < state.size; i++) {
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
//	CollisionTestObject::DerivState()
//
//		Computes the derivatives of state variables. This is simply 
//		the f() evaluation where f() = dy/dt.
//
//
inline void CollisionTestObject::DerivState(State &deriv, State &state)
{
	unsigned int i;
	CollisionTestBoundary		*bound = (CollisionTestBoundary *) boundary;

	UpdateForces(state);

	for(i = 0; i < state.size; i++) {
		// dpos/dt = vel
		deriv.pos[i] = state.vel[i];

		// dvel/dt = force/m
		deriv.vel[i] = force[i];
		deriv.vel[i] /= mass;		
	}

	for(i = 0; i < bound->num_vertex; i++) {
		switch (bound->boundary_type[i]) {
			case (0):   // Neumann type boundary condition   (Specify traction)
				deriv.vel[bound->global_id[i]] += bound->boundary_value[i]/mass;///mass[bound->global_id[i]]);
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
				deriv.vel[bound->global_id[i]] += bound->boundary_value[i]/mass;///mass[bound->global_id[i]]);
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
//	CollisionTestObject::AllocState()
//
//		Allocates the memory for the integrator's local state 
//		members
//
inline void CollisionTestObject::AllocState(State &s)
{
	static unsigned int i;

	s.pos = new Vector<Real>[state.size];
	if(s.pos == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	s.vel = new Vector<Real>[state.size];
	if(s.vel == NULL) {
		error_exit(-1, "Cannot allocate memory for velocity!\n");
	}
	s.size = state.size;
}



////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::Simulate()
//
//		Simulates the CollisionTestObject object by a single timestep
//
//
void CollisionTestObject::Simulate(void)
{
	integrator->Integrate(*this, timestep);
	time+=timestep;

	//We alse need to update the Boundary from states 
	//   so that we will have the correct Boundary definition for boundary calculations
	State2Bound();

}


////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::Geom2State()
//
//		Copies geometry to state
//
//
void CollisionTestObject::Geom2State(void)
{
	TriSurface *geom = (TriSurface *) geometry;

	for(unsigned int i=0; i < geom->num_vertex; i++) {
		state.pos[i] = geom->vertex[i].pos;
	}
	//State2Bound();
}


////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::State2Geom()
//
//		Copies state to geometry
//
//
inline void CollisionTestObject::State2Geom(void)
{
	TriSurface *geom = (TriSurface *) geometry;
	VectorField *vf_geom = (VectorField *) VF_geometry;

	for(unsigned int i=0; i<geom->num_vertex; i++) {
		geom->vertex[i].pos = state.pos[i];		
		vf_geom->field_vector[i].pos = (state.pos[i] - vf_geom->vertex[i].pos);			
		vf_geom->vertex[i].pos = state.pos[i];				
	}
}

////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::State2Bound()
//
//		Copies state to boundary
//
//
inline void CollisionTestObject::State2Bound(void)
{
	CollisionTestBoundary		*bound = (CollisionTestBoundary *) boundary;
	//VectorField *vf_geom = (VectorField *) VF_geometry;
	
	for(unsigned int i=0; i<bound->num_vertex; i++) {
		bound->vertex[i].pos = state.pos[bound->global_id[i]];
		//vf_geom->vertex[i].pos = bound->vertex[i].pos;		
		//vf_geom->field_vector[i].pos[0] = this->GetNodeForce(i)[0];		
		//vf_geom->field_vector[i].pos[1] = this->GetNodeForce(i)[1];	
		//vf_geom->field_vector[i].pos[2] = this->GetNodeForce(i)[2];	
	}
	bound->calcNormals();
}

inline void CollisionTestObject::Bound2State(void)
{
	CollisionTestBoundary		*bound = (CollisionTestBoundary *) boundary;
		
	for(unsigned int i=0; i<bound->num_vertex; i++)
		state.pos[bound->global_id[i]] = bound->vertex[i].pos;			
	
}
  
////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::GetNodePosition()
//
//		Returns node position
//
//
Vector<Real>		CollisionTestObject::GetNodePosition(unsigned int index)
{
	return state.pos[index];
}



////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::GetNodeVelocity()
//
//		Returns node velocity
//
//
Vector<Real>		CollisionTestObject::GetNodeVelocity(unsigned int index)
{
	return	state.vel[index];
}



////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::GetNodeForce()
//
//		Returns the nodal force
//         
//
Vector<Real>		CollisionTestObject::GetNodeForce(unsigned int index)
{
	return	force[index];
}

////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::GetInitColor()
//
//	get the initilize color of object
//
Vector<Real>		CollisionTestObject::GetInitColor(void)
{
	Vector<Real> color(4,initialcolor);
	return color;
}


////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::Display()
//
//		Displays the CollisionTestObject mesh
//
//
void CollisionTestObject::Display(void)
{
	//TriSurfaceDisplayManager	*tmp = (TriSurfaceDisplayManager	*) displayMngr;

	// Update state after collision response 
	Bound2State();

	// Copy the state into the geometry
	State2Geom();

	// Copy color to the geometry
	//((CollisionTestBoundary*)boundary)->Bound2Geom();	
	displayMngr->Display();
	//pointCloudDisplayMngr->Display();
	VFdisplayMngr->Display();
	
}

////////////////////////////////////////////////////////////////
//
//	CollisionTestObject::SetupDisplay()
//
//		Sets up the display of the the CollisionTestObject
//
//
void CollisionTestObject::SetupDisplay(XMLNodeList * simObjectChildren) 
{
	DisplayHeader	header;

	header.objType				= GIPSI_DRAW_TRIANGLE;
	header.polyMode				= GIPSI_POLYGON_FILL;
	header.shadeMode			= GIPSI_SHADE_SMOOTH;
#ifdef _DEBUG_COLLISION
	header.dataType				= 0x06;
#else	
	header.dataType				= 0x0E;
#endif	

	displayMngr = (TriSurfaceDisplayManager *) new TriSurfaceDisplayManager((TriSurface *) geometry, &header);
	//displayMngr->SetObjectName(GetName());

	//DisplayHeader	header1;
	//header.objType				= GIPSI_DRAW_POINT;
	//header.shadeMode				= GIPSI_SHADE_SMOOTH;
	//header.dataType				= 0x01;					// RGBA
	//header.pointSize				= 2.0;
	//displayMngr = (PointCloudDisplayManager *) new PointCloudDisplayManager((PointCloud *) geometry, header);

	header.objType				= GIPSI_DRAW_LINE;
	header.polyMode				= GIPSI_POLYGON_OUTLINE;
	header.shadeMode			= GIPSI_SHADE_FLAT;
	header.dataType				= 0x01;					// RGBA
	header.lineSize				= 2.0;	
	VFdisplayMngr = (VectorFieldDisplayManager *) new VectorFieldDisplayManager((VectorField *) VF_geometry, &header);		
	VFdisplayMngr->SetObjectName(GetName());

	displayMngr->GetDisplayBuffer()->SetNext(VFdisplayMngr->GetDisplayBuffer());

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

/**
 * CollisionTestObject::ResetInitialBoundaryCondition()
 * Reset CollisionTestObject boundary condition to initial values.
 */
void CollisionTestObject::ResetInitialBoundaryCondition()
{
	CollisionTestBoundary *bound = (CollisionTestBoundary*) boundary;		
	if(boundaryfix)
	{
		for(int i=0;i<num_vertex;i++) 
			bound->Set(i,1,GetNodePosition(i),0.0,zero_vector3);		
	}
}

////////////////////////////////////////////////////////////////
//
//	CollisionTestBoundary::init
//
//		CollisionTest Boundary initializer
//
void			CollisionTestBoundary::init(CollisionTestObject *ParentObj, unsigned int num_vertex, unsigned int num_face)
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
//	CollisionTestBoundary::GetPosition
//
//		Get Positions of the nodes of the boundary
//
Vector<Real>	CollisionTestBoundary::GetPosition(unsigned int index) 
{
	return	Object->GetNodePosition(global_id[index]);
}

void			CollisionTestBoundary::GetPosition(Vector<Real> *Bpos)
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bpos[index]=Object->GetNodePosition(global_id[index]);
	}
}



////////////////////////////////////////////////////////////////
//
//	CollisionTestBoundary::GetVelocity
//
//		Get Velocities of the nodes of the boundary
//

Vector<Real>	CollisionTestBoundary::GetVelocity	(unsigned int index)
{
	return	Object->GetNodeVelocity(global_id[index]);
}

void			CollisionTestBoundary::GetVelocity	(Vector<Real> *Bvel)	
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bvel[index]=Object->GetNodeVelocity(global_id[index]);
	}
}


////////////////////////////////////////////////////////////////
//
//	CollisionTestBoundary::ResidualForce
//
//		Get the Reaction force that the body applies to the external 
//         world as a result of the constraints
//		   (i.e. contact force, constraint force, lagrange multiplier)
//
Vector<Real>	CollisionTestBoundary::GetReactionForce	(unsigned int index)
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

void			CollisionTestBoundary::GetReactionForce	(Vector<Real> *Bforce)
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bforce[index]=GetReactionForce(global_id[index]);
	}
}




////////////////////////////////////////////////////////////////
//
//	CollisionTestBoundary::Set
//
//		Sets CollisionTest Boundary
//
void		CollisionTestBoundary::Set(unsigned int index, unsigned int boundary_type, 
							 Vector<Real> boundary_value, 
							 Real boundary_value2_scalar, Vector<Real> boundary_value2_vector) 
{	
	this->boundary_type[index]			=boundary_type;
	this->boundary_value[index]			=boundary_value;
	this->boundary_value2_scalar[index]	=boundary_value2_scalar;
	this->boundary_value2_vector[index]	=boundary_value2_vector;		
}

void		CollisionTestBoundary::Set(unsigned int *boundary_type, 
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


////////////////////////////////////////////////////////////////
//
//	CollisionTestBoundary::ResetBoundState()
//
//		reset boundary state
//
//
void CollisionTestBoundary::ResetBoundState(void)
{
	//TriSurface *bound = this;
	CollisionTestBoundary*	bound = this;
	Vector<Real> color = ((CollisionTestObject*)Object)->GetInitColor();

	for(unsigned int i=0; i < bound->num_vertex; i++) {
		bound->vertex[i].color = color;		
	}
}


////////////////////////////////////////////////////////////////
//
//	CollisionTestBoundary::Bound2Geom()
//
//		Copies boundary state to geometry
//
//
void CollisionTestBoundary::Bound2Geom(void)
{
	TriSurface *geom = (TriSurface *) Object->GetGeometryPtr();
	//TriSurface *bound = this;
	CollisionTestBoundary*	bound = this;
	//VectorField *vf_geom = (VectorField *) Object->VF_geometry;

	for(unsigned int i=0; i < geom->num_vertex; i++) {
		geom->vertex[i].color = bound->vertex[i].color;	
		geom->vertex[i].pos = bound->vertex[i].pos;
		//vf_geom->vertex[i].pos = GetPosition(i);		
		//vf_geom->field_vector[i].pos[0] = GetReactionForce(i)[0];
		//vf_geom->field_vector[i].pos[1] = GetReactionForce(i)[1];
		//vf_geom->field_vector[i].pos[2] = GetReactionForce(i)[2];
	}
}

void CollisionTestBoundary::ResetBoundaryCondition(void)
{
	CollisionTestBoundary*	bound = this;	
	for(int i=0;i<num_vertex;i++) 
	{
		bound->Set(i,0,zero_vector3,0.0,zero_vector3);
	}
	Object->ResetInitialBoundaryCondition();
	//printf("reset boundary in collisiontestboundary in %d\n",num_vertex);
}

bool CollisionTestBoundary::isTypeOneBoundary(unsigned int index)
{
	if (boundary_type[index] == 1)
		return true;
	else
		return false;
}