/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Balloon Object Model Implementation (balloon.cpp).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	BALLOON.cpp	v0.0
////
////	Balooon object class.
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
#include "balloon.h"
#include "XMLNodeList.h"

#define		DELIM 10

/**
 * Constructor.
 * 
 * @param simObjectNode XML project file 'simObject' node.
 */
BalloonObject::BalloonObject(XMLNode * simObjectNode):DeformableSolidObject(simObjectNode)
{
	try
	{
		bScale = Vector<Real>(3,1.0);
		g_w_lb = zero_matrix4;
		R_lb_b = zero_matrix3;
		t_lb_b = zero_vector3;
		s_lb_b = zero_vector3;

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

		Init();

		// Final setup
		Geom2State();
		State2Bound();
		SetupDisplay(simObjectChildren);
		delete simObjectChildren;
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Set object general parameters using values from the input XML node list.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void BalloonObject::SetParameters(XMLNodeList * simObjectChildren)
{
	try
	{
		// Get parameters node
		XMLNode * objParametersNode = simObjectChildren->GetNode("objParameters");
		XMLNodeList * objParametersChildren = objParametersNode->GetChildren();
		XMLNode * NHParametersNode = objParametersChildren->GetNode("NHParameters");
		XMLNodeList * NHParametersChildren = NHParametersNode->GetChildren();

		// Extract type-specific parameters
		// Set type-specific parameters

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


/**
 * Set object visualization parameters using values from the input XML node list.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void BalloonObject::InitializeVisualization(XMLNodeList * simObjectChildren)
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
		initialcolor[0] = (float)atof(red);
		delete red;
		delete redNode;

		// Extract green
		XMLNode * greenNode = baseColorChildren->GetNode("green");
		const char * green = greenNode->GetValue();
		initialcolor[1] = (float)atof(green);
		delete green;
		delete greenNode;

		// Extract blue
		XMLNode * blueNode = baseColorChildren->GetNode("blue");
		const char * blue = blueNode->GetValue();
		initialcolor[2] = (float)atof(blue);
		delete blue;
		delete blueNode;

		// Extract opacity
		XMLNode * opacityNode = baseColorChildren->GetNode("opacity");
		const char * opacity = opacityNode->GetValue();
		initialcolor[3] = (float)atof(opacity);
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


/**
 * Load object geometry using values from the input XML node list.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void BalloonObject::LoadGeometry(XMLNodeList * simObjectChildren)
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


/**
 * Perform object initial transformation using values from the input XML node list.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void BalloonObject::InitializeTransformation(XMLNodeList * simObjectChildren)
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
		//geom->Scale((float)atof(sx), (float)atof(sy), (float)atof(sz));
		// Set scale
		s_lb_b[0] = (float)atof(sx);
		s_lb_b[1] = (float)atof(sy);
		s_lb_b[2] = (float)atof(sz);
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
		//geom->Rotate(TORAD(atof(rangle)), atof(rx), atof(ry), atof(rz));
		// Set rotate
		ToRotationMatrix(R_lb_b, TORAD(atof(rangle)), atof(rx), atof(ry), atof(rz));
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
		//geom->Translate((float)atof(dx), (float)atof(dy), (float)atof(dz));
		// Set translate
		t_lb_b[0] = (float)atof(dx);
		t_lb_b[1] = (float)atof(dy);
		t_lb_b[2] = (float)atof(dz);
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


/**
 * Load object geometry using the input formatted file name.
 * 
 * @param filename Character string containing formatted geometry file name.
 */
void BalloonObject::Load(char *filename)
{
	TriSurface	*geom;
	LoadData *data;
	data = LoadObj(filename);
  
	/****************************************/
	/*	Set up geometry						*/
	/****************************************/
	if((geometry = new TriSurface((float)initialcolor[0],(float)initialcolor[1],(float)initialcolor[2],(float)initialcolor[3])) == NULL) {
		printf("ERROR: Could not allocate geometry!\n");
		exit(0);
	}
	geom = (TriSurface *) geometry;
	geom->Load(data);	
	geom->calcNormals();

	/****************************************/
	/*	Set up initial geometry				*/
	/****************************************/
	if((InitialGeometry = new TriSurface(initialcolor[0],initialcolor[1],initialcolor[2],initialcolor[3])) == NULL) {
		printf("ERROR: Could not allocate geometry!\n");
	}	
	InitialGeometry->Load(data);
	InitialGeometry->calcNormals();	
}

void BalloonObject::Init(void)
{
	/****************************************/
	/*	Set up boundary						*/
	/****************************************/
	// Allocate boundary
	if((boundary = new BalloonBoundary()) == NULL) {
		printf("ERROR: Could not allocate BalloonBoundary!\n");
		exit(0);
	}	
	BalloonBoundary*	bound = (BalloonBoundary *) boundary;

	//initialize the boundary in obj file all vertex and face are boundary
	// used the same data for boundary and geometry
	TriSurface *geom = (TriSurface *) geometry;
	bound->Init(this, geom->num_vertex, geom->num_face);

	/****************************************/
	/*	Set up domain						*/
	/****************************************/
	if((domain = new BalloonDomain()) == NULL) {
		printf("ERROR: Could not allocate BalloonDomain!\n");
		exit(0);
	}
	BalloonDomain *dom = (BalloonDomain *) domain;
	dom->Init(this);
}

/**
 * Simulates the SimpleTestObject object by a single timestep.
 */
void BalloonObject::Simulate(void)
{
	time+=timestep;
	
	//We alse need to update the Boundary from states 
	//   so that we will have the correct Boundary definition for boundary calculations
	//State2Geom();
	State2Bound();

}

/**
 * Copies geometry to state.
 */
void BalloonObject::Geom2State(void)
{	
}

/**
 * Copies state to geometry.
 */
inline void BalloonObject::State2Geom(void)
{
	TriSurface *geom = (TriSurface *) geometry;
	Matrix<Real> g_lb_b = zero_matrix4;
	Matrix<Real> g_w_b = zero_matrix4;

	ToTransformationMatrix(g_lb_b, R_lb_b, t_lb_b, s_lb_b);
	g_w_b = g_w_lb * g_lb_b;
	
	// reset to initial setup (at origin with initial rotation and translation)
	for (unsigned int i=0; i<geom->num_vertex; i++) {
		geom->vertex[i].pos = InitialGeometry->vertex[i].pos;
	}	
	
	// inflate or deflate ballon
	geom->Scale(bScale);

	// rotation
	Matrix<Real> rotation = zero_matrix3;
	GetRotationMatrix(rotation, g_w_b);
	geom->Rotate(rotation);		
	// translate 
	Vector<Real> translate = zero_vector3;
	GetTranslationVector(translate, g_w_b);
	geom->Translate(translate);
	
	geom->calcNormals();
}

/**
 * Copies state to boundary.
 */
inline void BalloonObject::State2Bound(void)
{
	//printf("No SimpleTestObject Boundary implemented...\n");
}

/**
 * Displays the SimpleTestObject mesh.
 */
void BalloonObject::Display(void)
{
	// Copy the state into the geometry
	State2Geom();

	// Call the display manager
	displayMngr->Display();
}

/**
 * Sets up the display of the the SimpleTestObject.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void BalloonObject::SetupDisplay(XMLNodeList * simObjectChildren) 
{
	DisplayHeader	header;

	header.objType				= GIPSI_DRAW_TRIANGLE;
	header.polyMode				= GIPSI_POLYGON_FILL;
	header.shadeMode			= GIPSI_SHADE_SMOOTH;
	header.dataType				= 0x0E;					// TEXTURE ON, NORMAL ON, RGBA

	displayMngr = (TriSurfaceDisplayManager *) new TriSurfaceDisplayManager((TriSurface *) geometry, &header);
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

void BalloonObject::SetBaseConfiguration(Matrix<Real> config)
{
	g_w_lb = config;	
}

/**
 * BalloonBoundary::init
 * Balloon Boundary initializer.
 * @param ParentObj pointer of SimObject.
 * @param num_vertex number of vertex.
 * @param num_face number of face.
 */
void BalloonBoundary::Init(BalloonObject *ParentObj, unsigned int num_vertex, unsigned int num_face)
{
	unsigned int i;	

	Object				=	ParentObj;
	this->num_vertex	=	num_vertex;
	this->num_face		=	num_face;

	TriSurface *geom = (TriSurface *) Object->GetGeometryPtr();

	vertex = geom->vertex;	
	face = geom->face;
	
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


/**
 * BalloonBoundary::GetPosition
 * Get positions of the nodes of the boundary.
 * @param index node index.
 * @return vector<Real> node position.
 */
Vector<Real>	BalloonBoundary::GetPosition(unsigned int index) 
{
	return	Object->GetNodePosition(global_id[index]);
}


/**
 * BalloonBoundary::GetPosition
 * Get positions of the nodes of the boundary.
 * @return all node posintion. 
 */
void			BalloonBoundary::GetPosition(Vector<Real> *Bpos)
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bpos[index]=Object->GetNodePosition(index);
	}
}


/**
 * BalloonBoundary::GetVelocity
 * Get velocity of the nodes of the boundary.
 * @param index node index.
 * @return vector<Real> velocity.
 */
Vector<Real>	BalloonBoundary::GetVelocity(unsigned int index)
{
	return	Object->GetNodeVelocity(index);
}

/**
 * BalloonBoundary::GetVelocity
 * Get velocity of the nodes of the boundary.
 * @return all node velocity. 
 */
void			BalloonBoundary::GetVelocity(Vector<Real> *Bvel)	
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bvel[index]=Object->GetNodeVelocity(index);
	}
}


/**
 * BalloonBoundary::ResidualForce
 * Get the Reaction force that the body applies to the external 
 *         world as a result of the constraints
 *		   (i.e. contact force, constraint force, lagrange multiplier)
 * @param index node index.
 * @return vector<Real> reaction force.
 */
Vector<Real>	BalloonBoundary::GetReactionForce(unsigned int index)
{
	Vector<Real>	ResForce(3,0.0);

	//Object->UpdateForces(index);
	switch (boundary_type[index]) {
		case (0):   // Neumann type boundary condition   (Specify traction)
			ResForce =	0.0;
			break;
		case (1):   // Drichlett type boundary condition (Fixed boundary - e.g. wall)
					// Position specified in boundary_value
			ResForce =	Object->GetNodeForce(global_id[index]);
			break;
		case (2):	// Mixed boundary condition type i: 
					// Normal displacement (not position) specified in boundary_value2_scalar
					//        with normal direction specified in boundary_value2_vector
					//        and tangential traction specified in boundary_value
			ResForce =	boundary_value2_vector[index] * (Object->GetNodeForce(global_id[index]) * boundary_value2_vector[index]);
			break;
		default:
			error_exit(0,"Unrecognized boundary condition type\n");
	}

	return ResForce;
}


/**
 * BalloonBoundary::GetReactionForce
 * Get reaction force of the nodes of the boundary.
 * @return all reaction force. 
 */
void			BalloonBoundary::GetReactionForce(Vector<Real> *Bforce)
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bforce[index]=GetReactionForce(global_id[index]);
	}
}


/**
 * BalloonBoundary::Set
 * Sets Balloon Boundary.
 * @param index node index.
 * @param boundary_type boundary type.
 * @param boundary_value boundary value.
 * @param boundary_value2_scalar boundary value 2 scalar.
 * @param boundary_value2_vector boundary value 2 vector.
 */
void		BalloonBoundary::Set(unsigned int index, unsigned int boundary_type, 
							 Vector<Real> boundary_value, 
							 Real boundary_value2_scalar, Vector<Real> boundary_value2_vector) 
{
	this->boundary_type[index]			=boundary_type;
	this->boundary_value[index]			=boundary_value;
	this->boundary_value2_scalar[index]	=boundary_value2_scalar;
	this->boundary_value2_vector[index]	=boundary_value2_vector;		
}


/**
 * BalloonBoundary::Set
 * Sets Balloon Boundary.
 * @param boundary_type array of boundary type.
 * @param boundary_value array of boundary value.
 * @param boundary_value2_scalar array of boundary value 2 scalar.
 * @param boundary_value2_vector array of boundary value 2 vector.
 */
void		BalloonBoundary::Set(unsigned int *boundary_type, 
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
 * BalloonBoundary::ResetBoundaryCondition()
 * Reset boundary condition.
 */
void BalloonBoundary::ResetBoundaryCondition(void)
{	
	return;
}
