/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Quasi Static Decouple Spring Model Implementation (qsds.cpp).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	QSDS.cpp	v0.0
////
////	Quasi Static Decouple Spring object class.
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
#include "qsds.h"
#include "XMLNodeList.h"
#include "msd_haptics.h"

#define		DELIM 10

/**
 * Constructor.
 * 
 * @param simObjectNode XML project file 'simObject' node.
 */
QSDSObject::QSDSObject(XMLNode * simObjectNode)
								   : DeformableSolidObject(simObjectNode)
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
		Init();

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
void QSDSObject::SetParameters(XMLNodeList * simObjectChildren)
{
	try
	{
		// Get parameters node
		XMLNode * objParametersNode = simObjectChildren->GetNode("objParameters");
		XMLNodeList * objParametersChildren = objParametersNode->GetChildren();
		XMLNode * NHParametersNode = objParametersChildren->GetNode("NHParameters");
		XMLNodeList * NHParametersChildren = NHParametersNode->GetChildren();

		XMLNode * modelParametersNode = NHParametersChildren->GetNode("modelParameters");
		XMLNodeList * modelParametersChildren = modelParametersNode->GetChildren();
		XMLNode * QSDSParametersNode = modelParametersChildren->GetNode("QSDSParameters");
		XMLNodeList * QSDSParametersChildren = QSDSParametersNode->GetChildren();
		XMLNode * QSDSFileNode = QSDSParametersChildren->GetNode("QSDSFile");
		XMLNodeList * QSDSFileChildren = QSDSFileNode->GetChildren();

		XMLNode * pathNode = QSDSFileChildren->GetNode("path");
		const char * path = pathNode->GetValue();
		XMLNode * fileNameNode = QSDSFileChildren->GetNode("fileName");
		const char * fileName = fileNameNode->GetValue();
		
		// Format file name
		char * qsdsFileName = new char[strlen(path) + strlen(fileName) + 4];
		sprintf_s(qsdsFileName, strlen(path) + strlen(fileName) + 4, ".\\%s\\%s", path, fileName);		

		logger->Message(GetName(), "Loading QSDS file...", 1);
		LoadQSDS(qsdsFileName);

		delete qsdsFileName;
		delete path;
		delete fileName;		
		
		delete fileNameNode;
		delete pathNode;		
		delete QSDSFileChildren;
		delete QSDSFileNode;
		delete QSDSParametersChildren;
		delete QSDSParametersNode;
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

/**
 * Set object visualization parameters using values from the input XML node list.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void QSDSObject::InitializeVisualization(XMLNodeList * simObjectChildren)
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
void QSDSObject::LoadGeometry(XMLNodeList * simObjectChildren)
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
void QSDSObject::InitializeTransformation(XMLNodeList * simObjectChildren)
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
		init_geometry->Scale((float)atof(sx), (float)atof(sy), (float)atof(sz));

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
		init_geometry->Rotate(TORAD(atof(rangle)), atof(rx), atof(ry), atof(rz));	

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
		init_geometry->Translate((float)atof(dx), (float)atof(dy), (float)atof(dz));		

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
void QSDSObject::Load(const char *filename)
{
	TriSurface	*geom;
	LoadData *data;
  
	/****************************************/
	/*	Set up geometry						*/
	/****************************************/
	if((geometry = new TriSurface((float)initialcolor[0],(float)initialcolor[1],(float)initialcolor[2],(float)initialcolor[3])) == NULL) {
		printf("ERROR: Could not allocate geometry!\n");
		exit(0);
	}
	geom = (TriSurface *) geometry;	
	data = LoadObj(filename);
	geom->Load(data);
	geom->calcNormals();

	// make a copy for init_geometry
	if((init_geometry = new TriSurface((float)initialcolor[0],(float)initialcolor[1],(float)initialcolor[2],(float)initialcolor[3])) == NULL) {
		printf("ERROR: Could not allocate geometry!\n");
		exit(0);
	}
	geom = (TriSurface *) init_geometry;	
	data = LoadObj(filename);
	geom->Load(data);
	geom->calcNormals();

	delete data;
}

/**
 * QSDSObject::LoadQSDS()
 * Reads in .qsds file.
 */
void QSDSObject::LoadQSDS(const char *filename)
{	
	FILE			*fp;
	unsigned int	i;
	char			errmsg[1024], 				
					file_type[64],
					line_type;	
	unsigned int	node;	
	unsigned int	num_spring;
	Real			m;
	Real			K0,B0,L0;

	/****************************************/
	/*	Set up QSDS Model					*/
	/****************************************/	
	fp = fopen(filename, "r");
	if (fp == NULL) {
		sprintf(errmsg, "Cannot open QSDS file %s\n", filename);
		error_exit(-1, errmsg);
	}
	
	fscanf(fp, "%s%d\n", file_type, &(num_spring));
	printf("File type : <%s>\tSpring: %d\n", file_type, num_spring);

	if(strcmp("QSDS", file_type)) {
		sprintf(errmsg, "Invalid QSDS file type %s\n", filename);
		error_exit(-1, errmsg);
	}

	/****************************************/
	/*	Read springs						*/
	/****************************************/
	i = 0;
	while (i < num_spring) {
	    fscanf(fp, "%c%d%lf%lf%lf%lf\n", &line_type, &node, &m, &K0, &B0, &L0);

		if(line_type == '#' || line_type == DELIM) continue;
		if(line_type != 's') {
			error_exit(-1, "Wrong number of springs!\n");
		}

		node--;
		spring.node[0] = node;
		spring.node[1] = node;

		mass = m;
		spring.dir = zero_vector3;
		spring.setProperties(L0,K0,B0);
		spring.refid = i;

		i++;
	}
	fclose(fp);	
}

/** 
 * QSDSObject::init()
 * Initilize the QSDSObject.
 */
void QSDSObject::Init(void)
{
	TriSurface *geom = (TriSurface *) geometry;		
	
	/****************************************/
	/*	Set up states						*/
	/****************************************/
	unsigned int i;
	state.size	= geom->num_vertex;
	
	state.pos = new Vector<Real>[state.size];
	if(state.pos == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}	
	state.vel = new Vector<Real>[state.size];
	if(state.vel == NULL) {
		error_exit(-1, "Cannot allocate memory for velocity!\n");
	}
	for(i=0;i<state.size;i++) 
	{
		//every position of mass is state position
		this->state.pos[i]	= geom->vertex[i].pos;	
		this->state.vel[i]	= zero_vector3;
	}
	
	/****************************************/
	/*	Set up boundary						*/
	/****************************************/
	// Allocate boundary
	if((boundary = new QSDSBoundary()) == NULL) {
		printf("ERROR: Could not allocate boundary!\n");
		exit(0);
	}	

	QSDSBoundary*	bound = (QSDSBoundary *) boundary;

	//initialize the boundary in obj file all vertex and face are boundary
	// used the same data for boundary and geometry
	bound->init(this, geom->num_vertex, geom->num_face);		
}

/**
 * Simulates the QSDSObject object by a single timestep.
 */
void QSDSObject::Simulate(void)
{
	//no integration in QSDS object, but need to update boundary value to state
	Bound2State();

	time+=timestep;

	//We alse need to update the Boundary from states 
	//   so that we will have the correct Boundary definition for boundary calculations
	State2Bound();	
}

/** 
 * QSDSObject::Bound2State()
 * Reset state to init_geom.	
 * Copies boundary to state. 
 */
void QSDSObject::Bound2State(void)
{		
	QSDSBoundary  *bound = (QSDSBoundary *) boundary;				
		
	InitGeom2State();

	for (unsigned int i=0; i<bound->num_vertex; i++) {		
		switch (bound->boundary_type[i]) {		
			case (0):   // Neumann type boundary condition   (Specify traction)
				break;
			case (1):   // Drichlett type boundary condition (Fixed boundary - e.g. wall)
						// Position specified in boundary_value
				state.pos[i] = bound->boundary_value[i];
				//bound->vertex[i].pos = bound->boundary_value[i];
				break;
			case (2):	// Mixed boundary condition type i: 
					   // Normal displacement (not position) specified in boundary_value2_scalar
					   //        with normal direction specified in boundary_value2_vector
					   //        and tangential traction specified in boundary_value
				state.pos[i] += bound->boundary_value2_scalar[i] * bound->boundary_value2_vector[i];
				//bound->vertex[i].pos += bound->boundary_value2_scalar[i] * bound->boundary_value2_vector[i];
				break;
			default:
				error_exit(0,"Unrecognized boundary condition type\n");
		}
	}			
}

/** 
 * QSDSObject::InitGeom2State()
 * Copies init_geometry to state.
 */
void QSDSObject::InitGeom2State(void)
{	
	static TriSurface *init_geom = (TriSurface *) init_geometry;	

	for(unsigned int i=0; i < init_geom->num_vertex; i++) {
		state.pos[i] = init_geom->vertex[i].pos;
	}		
}

/**
 * QSDSObject::State2Geom()
 * Copies state to geometry.
 */
void QSDSObject::State2Geom(void)
{	
	State2Bound();
}

/**
 * QSDSObject::State2Bound()
 * Copies state to boundary.
 */
inline void QSDSObject::State2Bound(void)
{	
	QSDSBoundary *bound = (QSDSBoundary *) boundary;

	for(unsigned int i=0; i<bound->num_vertex; i++)
		bound->vertex[i].pos = state.pos[i];
	bound->calcNormals();
}

/**
 * QSDSObject::ResetInitialBoundaryCondition()
 * Reset QSDS boundary condition to initial values.
 */
inline void QSDSObject::ResetInitialBoundaryCondition(void)
{		
	TriSurface *geom = (TriSurface *) geometry;	
	TriSurface *init_geom = (TriSurface *) init_geometry;	
	
	// reset to initial setup (at origin with initial rotation)
	for (unsigned int i=0; i<geom->num_vertex; i++) {
		geom->vertex[i].pos = init_geom->vertex[i].pos;
	}		
	geom->calcNormals();
}

/** 
 * QSDSObject::GetNodePosition()
 * Returns node position.
 * @param index node index.
 * @return vector<Real> node position.
 */
Vector<Real> QSDSObject::GetNodePosition(unsigned int index)
{
	return state.pos[index];
}

/**
 * QSDSObject::GetNodeVelocity()
 * Returns node velocity.
 * @param index node index.
 * @return vector<Real> node velocity.
 */
Vector<Real> QSDSObject::GetNodeVelocity(unsigned int index)
{
	return	state.vel[index];
}

/**
 * QSDSObject::GetNodeForce()
 * Returns the nodal force.
 * @param index node index.        
 * @return vector<Real> force.
 */
Vector<Real> QSDSObject::GetNodeForce(unsigned int index)
{			
	TriSurface *init_geom = (TriSurface *) init_geometry;	
	QSDSBoundary  *bound = (QSDSBoundary *) boundary;
		
	Vector<Real> p = bound->boundary_value[index];		
	//Vector<Real> p = state.pos[index];
	Vector<Real> q = init_geom->vertex[index].pos;
	Vector<Real> dir = p - q;		
	return -spring.k_stiff * dir;
}

/**
 * Displays the QSDSObject mesh.
 */
void QSDSObject::Display(void)
{
	// Copy the state into the geometry	
	//Bound2State();
	State2Geom();	

	displayMngr->Display();
}

/**
 * Sets up the display of the the QSDSObject.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void QSDSObject::SetupDisplay(XMLNodeList * simObjectChildren) 
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

		for (unsigned int i = 0; i < textureNamesChildren->GetLength(); i++)
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
 *	QSDSObject::ReturnHapticModel()
 *	Returns a haptic model built around the current node and current face
 *
 */     
int QSDSObject::ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model)
{
	/**
	 * because Haptic units are in F[N], L[mm], v[mm/s]
	 * but in model units are in F[0.00001N], L[cm], v[cm/s], m[g]
	 */
	QSDSBoundary	*bound = (QSDSBoundary *) boundary;	
	//TriSurface *geom = (TriSurface *) geometry;	
	TriSurface *init_geom = (TriSurface *) init_geometry;	
		
	// update the position of BoundaryNodeIndex	(Boundary type 1)
	bound->vertex[BoundaryNodeIndex].pos = bound->boundary_value[BoundaryNodeIndex];		
			
	// calculate the Low Order Linear Haptic Model
	unsigned int n = 6;
	unsigned int m = 6;
	unsigned int k = 3;
	unsigned int n_2 = n/2;	

	GiPSiLowOrderLinearHapticModel* model = new GiPSiLowOrderLinearHapticModel();		

	model->_n = n;
	model->_m = m;
	model->_k = k;

	// allocate memory
	model->A11 = new Matrix<Real>(n_2, n_2, 0.0);
	model->A12 = new Matrix<Real>(n_2, n_2, 0.0);
	model->B1  = new Matrix<Real>(n_2, m  , 0.0);
	model->C11 = new Matrix<Real>(k  , n_2, 0.0);
	model->C12 = new Matrix<Real>(k  , n_2, 0.0);
	model->D   = new Matrix<Real>(k  , m  , 0.0);
	model->f_0 = new Vector<Real>(k, 0.0);
	model->zdot_0 = new Vector<Real>(n, 0.0);
	model->normal = new Vector<Real>(k, 0.0);

	// seed f_0
	(*(model->f_0))[0] = 0.0;
	(*(model->f_0))[1] = 0.0; 
	(*(model->f_0))[2] = 0.0;
	
	// calculate the force at BoundaryNodeIndex (type 0) and internal node (type 1)
	Vector<Real> dir;
	Vector<Real> rel_vel;
	Vector<Real> temp_force;

	Vector<Real> p = bound->vertex[BoundaryNodeIndex].pos;
	Vector<Real> q = init_geom->vertex[BoundaryNodeIndex].pos;
	dir = p - q;
	Real L = dir.length();

	// Add spring force
	// f = -k * (1 - L/L0) * dir
	temp_force = -spring.k_stiff * dir;				
	
	// Add local damping
	if(L>0.0001) {	// check for divide by zero
		rel_vel = zero_vector3;
		temp_force += -spring.b_damp * ((rel_vel * dir) / (L*L)) * dir;				
	}
	// end for virtual spring //////////////////////////////////////////

	(*(model->f_0)) = temp_force;
		
	AddContactSpringEntries(model, p, q, zero_vector3, zero_vector3, spring.k_stiff, spring.l_zero, spring.b_damp);

	Model = (*model);
	delete model;	
	
	return 0;
}

/**
 * QSDSBoundary::init
 * QSDS Boundary initializer.
 * @param ParentObj pointer of SimObject.
 * @param num_vertex number of vertex.
 * @param num_face number of face.
 */
void QSDSBoundary::init(QSDSObject *ParentObj, unsigned int num_vertex, unsigned int num_face)
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
	for(i=0; i<num_vertex; i++)	boundary_type[i] = 0;

	boundary_value = new Vector<Real>[num_vertex];
	if(boundary_value == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	for(i=0; i<num_vertex; i++)	boundary_value[i] = zero_vector3;

	boundary_value2_scalar = new Real[num_vertex];
	if(boundary_value2_scalar == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	for(i=0; i<num_vertex; i++)	boundary_value2_scalar[i] = 0;

	boundary_value2_vector = new Vector<Real>[num_vertex];
	if(boundary_value2_vector == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	for(i=0; i<num_vertex; i++)	boundary_value2_vector[i] = zero_vector3;
	
}

/**
 * QSDSBoundary::GetPosition
 * Get positions of the nodes of the boundary.
 * @param index node index.
 * @return vector<Real> node position.
 */
Vector<Real> QSDSBoundary::GetPosition(unsigned int index) 
{
	return	Object->GetNodePosition(index);
}

/**
 * QSDSBoundary::GetPosition
 * Get positions of the nodes of the boundary.
 * @return all node posintion. 
 */
void QSDSBoundary::GetPosition(Vector<Real> *Bpos)
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bpos[index]=Object->GetNodePosition(index);
	}
}

/**
 * QSDSBoundary::GetVelocity
 * Get velocity of the nodes of the boundary.
 * @param index node index.
 * @return vector<Real> velocity.
 */
Vector<Real> QSDSBoundary::GetVelocity(unsigned int index)
{
	return	Object->GetNodeVelocity(index);
}

/**
 * QSDSBoundary::GetVelocity
 * Get velocity of the nodes of the boundary.
 * @return all node velocity. 
 */
void QSDSBoundary::GetVelocity(Vector<Real> *Bvel)	
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bvel[index]=Object->GetNodeVelocity(index);
	}
}

/**
 * QSDSBoundary::ResidualForce
 * Get the Reaction force that the body applies to the external 
 *         world as a result of the constraints
 *		   (i.e. contact force, constraint force, lagrange multiplier)
 * @param index node index.
 * @return vector<Real> reaction force.
 */
Vector<Real> QSDSBoundary::GetReactionForce(unsigned int index)
{
	Vector<Real>	ResForce(3,0.0);

	switch (boundary_type[index]) {
		case (0):   // Neumann type boundary condition   (Specify traction)
			ResForce =	0.0;
			break;
		case (1):   // Drichlett type boundary condition (Fixed boundary - e.g. wall)
					// Position specified in boundary_value
			ResForce =	Object->GetNodeForce(index);
			break;
		case (2):	// Mixed boundary condition type i: 
					// Normal displacement (not position) specified in boundary_value2_scalar
					//        with normal direction specified in boundary_value2_vector
					//        and tangential traction specified in boundary_value
			ResForce =	boundary_value2_vector[index] * (Object->GetNodeForce(index) * boundary_value2_vector[index]);
			break;
		default:
			error_exit(0,"Unrecognized boundary condition type\n");
	}

	return ResForce;
}

/**
 * QSDSBoundary::GetReactionForce
 * Get reaction force of the nodes of the boundary.
 * @return all reaction force. 
 */
void QSDSBoundary::GetReactionForce(Vector<Real> *Bforce)
{	
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bforce[index] = GetReactionForce(index);
	}
}

/**
 * QSDSBoundary::Set
 * Sets MSD Boundary.
 * @param index node index.
 * @param boundary_type boundary type.
 * @param boundary_value boundary value.
 * @param boundary_value2_scalar boundary value 2 scalar.
 * @param boundary_value2_vector boundary value 2 vector.
 */
void QSDSBoundary::Set(unsigned int index, unsigned int boundary_type, 
							 Vector<Real> boundary_value, 
							 Real boundary_value2_scalar, Vector<Real> boundary_value2_vector) 
{
	this->boundary_type[index]			=boundary_type;
	this->boundary_value[index]			=boundary_value;
	this->boundary_value2_scalar[index]	=boundary_value2_scalar;
	this->boundary_value2_vector[index]	=boundary_value2_vector;			
}

/**
 * QSDSBoundary::Set
 * Sets MSD Boundary.
 * @param boundary_type array of boundary type.
 * @param boundary_value array of boundary value.
 * @param boundary_value2_scalar array of boundary value 2 scalar.
 * @param boundary_value2_vector array of boundary value 2 vector.
 */
void QSDSBoundary::Set(unsigned int *boundary_type, 
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
 * QSDSBoundary::ReturnHapticModel()
 * Returns a haptic model.
 * @param BoundaryNodeIndex boundary node index.
 * @param Model GiPSi low order linear Haptic model.
 * @return int result. 
 */
int	QSDSBoundary::ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model)
{
	return Object->ReturnHapticModel(BoundaryNodeIndex, Model);
}


int	QSDSBoundary::ReturnHapticModel(unsigned int FaceNodeIndex1,
									  unsigned int FaceNodeIndex2,
									  unsigned int FaceNodeIndex3,  GiPSiLowOrderLinearHapticModel &Model)
{
	return Object->ReturnHapticModel(FaceNodeIndex1,FaceNodeIndex2,FaceNodeIndex3, Model);
}

/**
 * QSDSBoundary::ResetBoundaryCondition()
 * Reset boundary condition.
 */
void QSDSBoundary::ResetBoundaryCondition(void)
{	
	// reset all boundary value to zero
	QSDSBoundary* bound = this;
	for(unsigned int i=0;i<num_vertex;i++) 
	{
		bound->Set(i,0,zero_vector3,0.0,zero_vector3);
	}	
	((QSDSObject*)Object)->ResetInitialBoundaryCondition();	
}

/**
 * QSDSDomain::init
 * QSDS Domain initializer.
 * @param ParentObject SimObject pointer.
 */
void QSDSDomain::init(QSDSObject *ParentObj)
{
	unsigned int i;

	Object=ParentObj;

	DomStress = new Matrix<Real>[num_tet];
	if(DomStress == NULL) {
		error_exit(-1, "Cannot allocate memory for domain stresses!\n");
	}	
	for(i=0; i<num_tet; i++)	DomStress[i] = zero_Matrix;

}

/**
 * QSDSDomain::SetDomStress
 * Sets External Stresses.
 * @param element_index.
 * @param Streess_Tensor.
 */
void QSDSDomain::SetDomainStress(unsigned int element_index, Matrix<Real> Stress_Tensor)
{
	DomStress[element_index] = Stress_Tensor;
}

/**
 * QSDSDomain::SetDomStress
 * Sets External Stresses.
 * @param Stress_Tensor_Array.
 */
void QSDSDomain::SetDomainStress(Matrix<Real> *Stress_Tensor_Array)
{
	for (unsigned int index=0; index < this->num_tet; index++){
		DomStress[index] = Stress_Tensor_Array[index];
	}
}
