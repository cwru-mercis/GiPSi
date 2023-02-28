/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Simple Mass-Spring-Damper Model Implementation (msd.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriya Natsupakpong.
*/

////	MSD.cpp	v0.0
////
////	Simple Mass-Spring-Damper Module
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
#include "msd.h"
#include "XMLNodeList.h"
#include "msd_haptics.h"

#define		DELIM 10

void vprint(Vector<Real> vec) 
{	 
	for(unsigned int i=0;i<vec.size();i++) 
		printf("%10.9f ",vec[i]); 
}

void vprint(Matrix<Real> mat) 
{	 
	for(unsigned int i=0;i<mat.m();i++) 
	{
		vprint(mat.row(i));		
		printf("\n");
	}
}

/** 
 * Spring::setProperties()
 * Set properties of spring.
 * @param lzero rest length of spring.
 * @param kstiff stiffness constant of spring.
 * @param bdamp damper coefficeint of spring.
 */
void Spring::setProperties(Real lzero, Real kstiff, Real bdamp)
{
	l_zero = lzero;
	k_stiff = kstiff;
	b_damp = bdamp;	
}


/**
 * Spring::setMassIndex()
 * Set index of connected massed.
 * @param m1 mass index 1 (node index)
 * @param m2 mass index 2 (node index)
 */
void Spring::setMassIndex(int m1, int m2)
{
	node[0] = m1;
	node[1] = m2;
}


/**
 * Spring::getEnergy()
 * Get spring energy.
 * @return real spring energy.
 */
Real Spring::getEnergy(void)
{
	Real s = dir.length() - l_zero;
	//printf("------------ s = %.9f, E = %.9f\n",s,0.5*k_stiff*s*s);
	return 0.5*k_stiff*s*s;
}


/**
 * MSDObject::MSDObject()
 * Constructor.
 * @param simObjectNode Project file XML 'MSDObject' node.
 * @param g gravity constant.
 */
MSDObject::MSDObject(	XMLNode * simObjectNode,
						Real g)
						:	DeformableSolidObject(simObjectNode),
							int_method(0),
							g(g)
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

		// Load model files
		LoadModelFiles(simObjectChildren);

		// Initialize transformation
		InitializeTransformation(simObjectChildren);

		// Final setup
		
		// Call Init
		Init();

		// Set Intergration Method
		SetIntegrationMethod(int_method);

		//Geom2State();
		//State2Bound();
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
 * MSDObject::SetParameters()
 * Set object parameters.
 * @param simObjectChildren Project file XML 'MSDObject' children node.
 */
void MSDObject::SetParameters(XMLNodeList * simObjectChildren)
{
	try
	{
		// MSD parameters were moved to the MSD file
		return;
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * MSDObject::InitializeVisualization()
 * Initialize visualization parameters.
 * @param simObjectChildren Project file XML 'MSDObject' children node.
 */
void MSDObject::InitializeVisualization(XMLNodeList * simObjectChildren)
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


/**
 * MSDObject::LoadGeometry()
 * Initialize visualization parameters.
 * @param simObjectChildren Project file XML 'MSDObject' children node.
 */
void MSDObject::LoadGeometry(XMLNodeList * simObjectChildren)
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
 * MSDObject::InitializeTransformation()
 * Initialize transformation parameters.
 * @param simObjectChildren Project file XML 'MSDObject' children node.
 */
void MSDObject::InitializeTransformation(XMLNodeList * simObjectChildren)
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
		Scale((float)atof(sx), (float)atof(sy), (float)atof(sz));
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
		Rotate(TORAD(atof(rangle)), atof(rx), atof(ry), atof(rz));
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
		Translate((float)atof(dx), (float)atof(dy), (float)atof(dz));
		printf("translation :%lf, %lf, %lf\n",(float)atof(dx), (float)atof(dy), (float)atof(dz));
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
 * MSDObject::LoadModelFiles()
 * Reads in .msd and .map files.
 * @param simObjectChildren Project file XML 'MSDObject' children node.
 */
void MSDObject::LoadModelFiles(XMLNodeList * simObjectChildren)
{
	try
	{
		// Get path and file name information
		XMLNode * objParametersNode = simObjectChildren->GetNode("objParameters");
		XMLNodeList * objParametersChildren = objParametersNode->GetChildren();
		XMLNode * NHParametersNode = objParametersChildren->GetNode("NHParameters");
		XMLNodeList * NHParametersChildren = NHParametersNode->GetChildren();
		XMLNode * modelParametersNode = NHParametersChildren->GetNode("modelParameters");
		XMLNodeList * modelParametersChildren = modelParametersNode->GetChildren();
		XMLNode * MSDParametersNode = modelParametersChildren->GetNode("MSDParameters");
		XMLNodeList * MSDParametersChildren = MSDParametersNode->GetChildren();
		XMLNode * MSDFileNode = MSDParametersChildren->GetNode("MSDFile");
		XMLNodeList * MSDFileChildren = MSDFileNode->GetChildren();

		XMLNode * pathNode = MSDFileChildren->GetNode("path");
		const char * path = pathNode->GetValue();
		XMLNode * fileNameNode = MSDFileChildren->GetNode("fileName");
		const char * fileName = fileNameNode->GetValue();

		// Intergration Information
		XMLNode * numericMethodNode = NHParametersChildren->GetNode("numericMethod");
		const char * numericMethod = numericMethodNode->GetValue();
		int_method = getIntegrationMethod(numericMethod);

		// Format file name
		char * msdFileName = new char[strlen(path) + strlen(fileName) + 4];
		sprintf_s(msdFileName, strlen(path) + strlen(fileName) + 4, ".\\%s\\%s", path, fileName);
		char * mapFileName = new char[strlen(msdFileName) + 1];
		strcpy(mapFileName, msdFileName);
		mapFileName[strlen(mapFileName) - 3] = 'm';
		mapFileName[strlen(mapFileName) - 2] = 'a';
		mapFileName[strlen(mapFileName) - 1] = 'p';

		// Load the map file
		logger->Message(GetName(), "Loading MAP file...", 1);
		LoadMAP(mapFileName);
		logger->Message(GetName(), "Loading MSD file...", 1);
		LoadMSD(msdFileName);

		delete mapFileName;
		delete msdFileName;
		delete path;
		delete fileName;
		delete numericMethod;

	}
	catch (...)
	{
		throw;
		return;
	}
}


/** 
 * MSDObject::Load()
 * Reads in .obj file
 */
void MSDObject::Load(const char *filename)   
{
	TriSurface	*geom;	
	LoadData *data;

	/****************************************/
	/*	Set up geometry						*/
	/****************************************/
	if((geometry = new TriSurface(initialcolor[0],initialcolor[1],initialcolor[2],initialcolor[3])) == NULL) {
		printf("ERROR: Could not allocate geometry!\n");
		exit(0);
	}
	geom = (TriSurface *) geometry;	
	data=LoadObj(filename);
	geom->Load(data);
	geom->calcNormals();
	delete data;
}


/** 
 * MSDObject::LoadMAP()
 * Reads in .map file.
 */
void MSDObject::LoadMAP(const char *filename)   
{
	FILE			*fp;
	unsigned int	i;
	char			errmsg[1024], 
					buffer[1024],
					file_type[64];
	unsigned int	offset_msd, offset_obj;
	unsigned int	index_msd, index_obj;
  
	fp = fopen(filename, "r");
	if (fp == NULL) {
		sprintf(errmsg, "Cannot open map file %s\n", filename);
		error_exit(-1, errmsg);
	}

	fscanf(fp, "%s%d%d%d%\n", file_type, &(num_mapping), &(offset_msd), &(offset_obj));
	printf("File type : <%s>\tMapping: %d\tMSD Offset: %d\tOBJ Offset: %d\n",file_type, num_mapping, offset_msd, offset_obj);

	if(strcmp("MSDTOOBJ", file_type)) {
		sprintf(errmsg, "Invalid map file type %s\n", filename);
		error_exit(-1, errmsg);
	}

	// Allocate mapping array
	mapping = (unsigned int *) malloc(2*num_mapping*sizeof(unsigned int));		
	if(mapping == NULL) {
		error_exit(-1, "Cannot allocate memory for mapping!\n");
	}
		
	/****************************************/
	/*	Read mapping						*/
	/****************************************/
	i = 0;
	while (i < num_mapping) {
		fgets(buffer, 512,fp);
		sscanf(buffer,"%d%d\n", &index_msd, &index_obj);
		
		index_msd -= offset_msd;
		index_obj -= offset_obj;

		index_msd--;
		index_obj--;

		*(mapping+2*i)= index_msd;
		*(mapping+2*i+1)= index_obj;
		
		i++;
	}
	fclose(fp);
}


/**
 * MSDObject::LoadMSD()
 * Reads in .msd file.
 */
void MSDObject::LoadMSD(const char *filename)   
{
	TriSurface *geom = (TriSurface *) geometry;
	FILE		*fp;
	unsigned int  i;
	char		errmsg[1024], 
				buffer[1024], 
				file_type[64],
				line_type;	
	int			v1, v2;	
	int			mass_index;
	Real		x, y, z;	
	Real		m;
	Real		K0,B0,L0;

	/****************************************/
	/*	Set up MSD Model					*/
	/****************************************/	
	fp = fopen(filename, "r");
	if (fp == NULL) {
		sprintf(errmsg, "Cannot open MSD file %s\n", filename);
		error_exit(-1, errmsg);
	}

	fscanf(fp, "%s%d%d%d%d%d\n", file_type, &(num_mass), &(num_ground), &(num_spring), &(num_vspring), &(num_boundary));
	printf("File type : <%s>\tMass: %d\tV Mass: %d\tSpring: %d\tV Spring:%d\tBoundary: %d\n",file_type, num_mass, num_ground, num_spring, num_vspring, num_boundary);

	if(strcmp("MSD", file_type)) {
		sprintf(errmsg, "Invalid MSD file type %s\n", filename);
		error_exit(-1, errmsg);
	}

	// Allocate force
	force = new Vector<Real>[this->num_mass];
	if(force == NULL) {
		error_exit(-1, "Cannot allocate memory for forces!\n");
	}
	
	// Allocate mass
	mass = new Real[this->num_mass];
	if(mass == NULL) {
		error_exit(-1, "Cannot allocate memory for mass!\n");
	}

	// Allocate massPoint
	massPoint = new Point[this->num_mass];
	if(massPoint == NULL) {
		error_exit(-1, "Cannot allocate memory for massPoint!\n");
	}

	// Allocate springs
	this->spring = new Spring[this->num_spring];
	if(this->spring == NULL) {
		error_exit(-1, "Cannot allocate memory for springs!\n");
	}

	// Allocate ground_pos
	ground_pos = new Vector<Real>[this->num_ground];
	if(ground_pos == NULL) {
		error_exit(-1, "Cannot allocate memory for ground position!\n");
	}

	// Allocate virtual springs
	this->vspring = new Spring[this->num_vspring];
	if(this->vspring == NULL) {
		error_exit(-1, "Cannot allocate memory for virtual springs!\n");
	}

	// Allocate fix boundary
	fix_boundary = (unsigned int *) malloc(num_boundary*sizeof(unsigned int));		
	if(fix_boundary == NULL) {
		error_exit(-1, "Cannot allocate memory for fix boundary!\n");
	}

	/****************************************/
	/*	Read masses							*/
	/****************************************/
	i = 0;
	while (i < num_mass) {
		fgets(buffer, 512,fp);
		sscanf(buffer,"%c%lf%lf%lf%lf\n", &line_type, &m, &x, &y, &z);

		if(line_type == '#' || line_type == DELIM) continue;
		if(line_type != 'm') {
			error_exit(-1, "Wrong number of mass!\n");
		}		
		this->mass[i]		= m;		
		this->massPoint[i].refid = i;		
		this->massPoint[i].pos[0] = x;
		this->massPoint[i].pos[1] = y;
		this->massPoint[i].pos[2] = z;
		i++;
	}

	/****************************************/
	/*	Read ground masses 					*/
	/****************************************/
	i = 0;
	while (i < num_ground) {
		fscanf(fp, "%c%d\n", &line_type, &mass_index);

		if(line_type == '#' || line_type == DELIM) continue;
		if(line_type != 'g') {
			error_exit(-1, "Wrong number of ground mass!\n");
		}					
		this->ground_pos[i]	= massPoint[mass_index-1].pos;		
		i++;
	}
	
	/****************************************/
	/*	Read springs						*/
	/****************************************/
	i = 0;
	while (i < num_spring) {
	    fscanf(fp, "%c%d%d%lf%lf%lf\n", &line_type, &v1, &v2, &K0, &B0, &L0);

		if(line_type == '#' || line_type == DELIM) continue;
		if(line_type != 's') {
			error_exit(-1, "Wrong number of springs!\n");
		}

		v1--;
		v2--;

		spring[i].node[0] = v1;
		spring[i].node[1] = v2;

		//spring[i].dir = geom->vertex[v2].pos - geom->vertex[v1].pos;
		spring[i].dir = massPoint[v1].pos - massPoint[v2].pos;
		spring[i].setProperties(L0,K0,B0);
		spring[i].refid = i;

		i++;
	}

	/****************************************/
	/*	Read virtual springs				*/
	/****************************************/
	i = 0;
	while (i < num_vspring) {
	    fscanf(fp, "%c%d%d%lf%lf%lf\n", &line_type, &v1, &v2, &K0, &B0, &L0);

		if(line_type == '#' || line_type == DELIM) continue;
		if(line_type != 'v') {
			error_exit(-1, "Wrong number of virtual springs!\n");
		}

		v1--;
		v2--;

		vspring[i].node[0] = v1; // mass node
		vspring[i].node[1] = v2; // ground node

		//vspring[i].dir = geom->vertex[v1].pos - ground_pos[v2];
		vspring[i].dir = massPoint[v1].pos - ground_pos[v2];
		vspring[i].setProperties(L0,K0,B0);
		vspring[i].refid = i;

		i++;
	}

	/****************************************/
	/*	Read boundary						*/
	/****************************************/
	i = 0;

	while (i< num_boundary) {
		fscanf(fp, "%c%d\n", &line_type, &v1);

		if(line_type == '#' || line_type == DELIM) continue;
		if(line_type != 'b') {
			error_exit(-1, "Wrong number of boundary!\n");
		}
		
		*(fix_boundary+i) = v1 - 1;
		i++;
	}
	
	fclose(fp);	
}


/** 
 * MSDObject::init()
 * Initilize the MSDObject.
 */
void MSDObject::Init(void)
{
	TriSurface *geom = (TriSurface *) geometry;	
	int				v1, v2,	v3;
	//unsigned int	index_msd;
	//unsigned int	index_obj;
	unsigned int	i;


	/****************************************/
	/*	Set up states						*/
	/****************************************/
	state.size	= num_mass;

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

	for(i=0;i<num_mass;i++) 
	{
		// Initialize state
		//index_msd = *(mapping+2*i);
		//index_obj = *(mapping+2*i+1);

		//this->state.pos[index_msd]	= geom->vertex[index_obj].pos;
		//this->state.vel[index_msd]	= zero_vector3;
		
		//every position of mass is state position
		this->state.pos[i]	= massPoint[i].pos;
		this->state.vel[i]	= zero_vector3;
		//printf("%s -> vertex %d:%lf, %lf, %lf\n",GetName(), i,massPoint[i].pos[0], massPoint[i].pos[1], massPoint[i].pos[2]);
	}
	
	/****************************************/
	/*	Set up boundary						*/
	/****************************************/
	
	// Allocate boundary
	if((boundary = new MSDBoundary()) == NULL) {
		printf("ERROR: Could not allocate boundary!\n");
		exit(0);
	}

	MSDBoundary*	bound = (MSDBoundary *) boundary;
	
	//initialize the boundary in obj file all vertex and face are boundary
	bound->Init(this, geom->num_vertex, geom->num_face);
	
	unsigned int	j = 0;
	unsigned int	bno = 0;
	unsigned int	*g2b = new unsigned int[geom->num_vertex];		// Global to boundary reference table
	//unsigned int	*g2b = new unsigned int[num_mass+num_ground];		// Global to boundary reference table

	// all node are boundary
	for(i=0; i<geom->num_vertex; i++) 
	{
		//
		// Initialize nodes of the boundary
		//		
		bound->vertex[bno] = geom->vertex[i];
		bound->vertex[bno].refid	= bno;
		bound->Set(bno,0,zero_vector3,0.0,zero_vector3);			// Free boundary (zero traction)		
		bound->global_id[bno]		= i;

		g2b[i] = bno;
		bno++;		
	}

	unsigned int fix_boundary_index;
	for(i=0; i<num_boundary; i++) 
	{
		// map all boundary msd into geometry boundary
		int	index_obj;
		fix_boundary_index = *(fix_boundary+i);
		index_obj = getOBJIndex(fix_boundary_index);	
		if(index_obj != -1)
			bound->Set(index_obj, 1, bound->vertex[index_obj].pos, 0.0, zero_vector3);					
	}

	// all faces are boundary
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
	/*	Built MSDModel						*/
	/****************************************/
	msdModel = new MSDModel[num_mass];
	if(msdModel == NULL) {
		error_exit(-1, "Cannot allocate memory for MSDModel!\n");
	}
	msdModelBuilder.BuildModel(msdModel, this, 2);
	//msdModelBuilder.print(msdModel);
}


/**
 * MSDObject::SetIntegrationMethod()
 * Set intergration method.
 * @param method integration method.
 */
void MSDObject::SetIntegrationMethod(int method)
{
	switch(method)
	{
		case 1: //Euler
			integrator = new Euler<MSDObject>(*this);
			break;
		case 2: //MidPoint
			integrator = new ERKMid2<MSDObject>(*this);
			break;
		case 3: // RK4
			integrator = new ERK4<MSDObject>(*this);
			break;
		case 4: //Backward Euler
			integrator = new SemiEuler<MSDObject>(*this);
			break;
		case 5: //Implicit Euler with CG
			integrator = new ImplicitEuler<MSDObject>(*this);
			break;
		case 6: //Implitcit Euler with CG+Newton
			integrator = new ImplicitEulerNT<MSDObject>(*this);
			break;		
		case 7:
			integrator = new ERKHeun3<MSDObject>(*this);
			break;
		case 8: //MidPoint
			integrator = new ImplicitMidPoint<MSDObject>(*this);
			break;
	}
}


/**
 * MSDObject::getIntegrationMethod()
 * Get integration method from spring.
 * @param method the name of integration method.
 * @return int intergration method.
 */
int MSDObject::getIntegrationMethod(const char * method)
{
	int result = 0;
	if (strcmp(method, "Euler") == 0)
		result = 1;
	else if (strcmp(method, "MidPoint") == 0)
		result = 2;
	else if (strcmp(method, "RK4") == 0)
		result = 3;
	else if (strcmp(method, "BEuler") == 0)
		result = 4;
	else if (strcmp(method, "ImEuler") == 0)
		result = 5;
	else if (strcmp(method, "ImEuler2") == 0)
		result = 6;
	else if (strcmp(method, "Heun") == 0)
		result = 7;
	else if (strcmp(method, "ImMidpoint") == 0)
		result = 8;
	return result;
}


/**
 * MSDObject::getEnergy()
 * Get MSD engergy.
 */
Real MSDObject::getEnergy(void)
{		
	double Kinetic = 0.0;
	double Potential = 0.0;
	for(unsigned int i=0;i<num_mass;i++)
		Kinetic += 0.5*mass[i]*state.vel[i].length_sq();
	for(unsigned int i=0;i<num_spring;i++)
		Potential += spring[i].getEnergy();	
	//printf("K = %.9f, P = %.9f, Total = %.9f\n",Kinetic,Potential,Kinetic+Potential);
	return Kinetic+Potential;
}


/**
 * MSDObject::ResetInitialBoundaryCondition()
 * Reset MSD boundary condition to initial values.
 */
inline void MSDObject::ResetInitialBoundaryCondition(void)
{	
	/*
	MSDBoundary*	bound = (MSDBoundary *) boundary;
	static TriSurface *geom = (TriSurface *) geometry;
	int i,j;

	for(j = 0; j<num_boundary;j++)
	{		
		i = *(fix_boundary+j);
		bound->Set(i,1,geom->vertex[bound->global_id[i]].pos,0.0,zero_vector3);	// Fixed boundary (Wall)
	}
	*/	
	MSDBoundary		*bound = (MSDBoundary *) boundary;
	//static 
	TriSurface *geom = (TriSurface *) geometry;
	int	index_msd;
	int	index_obj;

	for(unsigned int i=0;i<num_boundary;i++) 
	{
		index_msd = *(fix_boundary+i);
		index_obj = getOBJIndex(index_msd);
		if( index_obj != -1)
			bound->Set(index_obj, 1, geom->vertex[index_obj].pos, 0.0, zero_vector3);	// Fixed boundary (Wall)		
	}
	
}


/**
 * MSDObject::UpdateForces()
 * Updates the forces.
 * @param state state information.
 */
inline void MSDObject::UpdateForces(State &state)   
{
	unsigned int	i;
	static			Real gg[3] = { 0.0, -g, 0.0 };
	static			const Vector<Real> g_vector(3, gg);
	static			Vector<Real> g_force(3), d_force(3), s_force(3);
	static			Vector<Real> rel_vel(3, 0.0);

	// Clear the forces
	for(i = 0; i < state.size; i++) force[i] = zero_vector3;

	// Update them by adding each component
	for(i = 0; i < state.size; i++) {
		// Add gravity
		g_force = g_vector;
		g_force *= mass[i]; 
		force[i] += g_force;

		// Add global damping
		//d_force = state.vel[i];
		//d_force *= -D;
		//force[i] += d_force;

		// NOTE: Add others here...
	}

	int v1, v2;
	for(i = 0; i < num_spring; i++) {
		// dir = v1-v2
		v1 = spring[i].node[0];
		v2 = spring[i].node[1];
		
		spring[i].dir = state.pos[v1] - state.pos[v2];
		Real L = spring[i].length();

		if(L>0.0){
			// Add spring force
			// f = -k * (1 - L0/L) * dir				
			s_force = -spring[i].k_stiff * (1 - spring[i].l_zero / L) * spring[i].dir;						
			force[v1] += s_force;
			force[v2] -= s_force;

			// Add local damping
			rel_vel = state.vel[v1] - state.vel[v2];
			d_force = -spring[i].b_damp * ((rel_vel * spring[i].dir) / (L*L)) * spring[i].dir;
			force[v1] += d_force;
			force[v2] -= d_force;
		}
	}

	for(i = 0; i < num_vspring; i++) {
		// dir = v1-v2
		v1 = vspring[i].node[0];	// mass node
		v2 = vspring[i].node[1];	// ground node (no force on it)
		
		vspring[i].dir = state.pos[v1] - ground_pos[v2];
		Real L = vspring[i].length();

		// Add spring force
		// f = -k * (1 - L/L0) * dir
		s_force = -vspring[i].k_stiff * vspring[i].dir;
		force[v1] += s_force;		

		// Add local damping
		if(L>0.0) {	// check for divide by zero
			rel_vel = state.vel[v1];
			d_force = -vspring[i].b_damp * ((rel_vel * vspring[i].dir) / (L*L)) * vspring[i].dir;
			force[v1] += d_force;		
		}
	}
}


/**
 * MSDObject::UpdateForces()
 * Updates the forces at boundary index
 */
inline void MSDObject::UpdateForces(unsigned int index)   
{
	MSDBoundary		*bound = (MSDBoundary *) boundary;
	int msd_index = getMSDIndex(index);
	if (msd_index != -1)
		state.pos[msd_index] = bound->GetPosition(index);//bound->boundary_value[index];
	//printf("update force only index %d\n",index);
	//state.pos[bound->global_id[index]] = bound->boundary_value[index];
	UpdateForces(state);
	//for(i = 0; i < state.size; i++) printf("state %d[%lf,%lf,%lf], force [%lf,%lf,%lf]\n",i,state.pos[i][0],state.pos[i][1],state.pos[i][2],force[i][0],force[i][1],force[i][2]);
}


/**
 * MSDObject::AccumState()
 * Computes y(t+h) = y(t) + h * f(..)
 *		where	y(t+h)	= new_state
 *				y(t)	= state
 *				f(..)	= deriv
 * @param state current state.
 * @param deriv deriv state.
 * @param h time step.
 * @return new_state next state.
 */
inline void MSDObject::AccumState(State &new_state, const State &state, const State &deriv, const Real &h)
{
	unsigned int				i;
	static Vector<Real>			temp(3,0.0);
	MSDBoundary		*bound = (MSDBoundary *) boundary;

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
	/*
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
					   //        and tangential traction specified in boundary_value
				new_state.pos[bound->global_id[i]]+=bound->boundary_value2_scalar[i] * bound->boundary_value2_vector[i];
				break;
			default:
				error_exit(0,"Unrecognized boundary condition type\n");
		}
	}
	*/
	
	unsigned int	index_msd;
	unsigned int	index_obj;
	for(i = 0; i < num_mapping; i++) 
	{
		index_msd = *(mapping+2*i);
		index_obj = *(mapping+2*i+1);		
		switch (bound->boundary_type[index_obj]) {
			case (0):   // Neumann type boundary condition   (Specify traction)
				break;
			case (1):   // Drichlett type boundary condition (Fixed boundary - e.g. wall)
					    // Position specified in boundary_value
				new_state.pos[index_msd] = bound->boundary_value[index_obj];
				break;
			case (2):  // Mixed boundary condition type i: 
					   // Normal displacement (not position) specified in boundary_value2_scalar
					   //        with normal direction specified in boundary_value2_vector
					   //        and tangential traction specified in boundary_value
				new_state.pos[index_msd] += bound->boundary_value2_scalar[index_obj] * bound->boundary_value2_vector[index_obj];
				break;
			default:
				error_exit(0,"Unrecognized boundary condition type\n");
		}
	}	
}


/**
 * MSDObject::DerivState()
 * Computes the derivatives of state variables. This is simply the f() evaluation where f() = dy/dt.
 * @param state current state.
 * @return deriv deriv state.
 */
inline void MSDObject::DerivState(State &deriv, State &state)
{
	unsigned int i;
	//static 
	MSDBoundary		*bound = (MSDBoundary *) boundary;

	UpdateForces(state);

	for(i = 0; i < state.size; i++) {
		// dpos/dt = vel
		deriv.pos[i] = state.vel[i];

		// dvel/dt = force/m
		deriv.vel[i] = force[i];
		deriv.vel[i] /= mass[i];			
	}
	/*
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
					   //        and tangential traction specified in boundary_value
				deriv.vel[bound->global_id[i]] += bound->boundary_value[i]*(1.0/mass[bound->global_id[i]]);
				deriv.vel[bound->global_id[i]] -= bound->boundary_value2_vector[i] * (deriv.vel[bound->global_id[i]] * bound->boundary_value2_vector[i]);
				deriv.pos[bound->global_id[i]] -= bound->boundary_value2_vector[i] * (deriv.pos[bound->global_id[i]] * bound->boundary_value2_vector[i]);
				break;
			default:
				error_exit(0,"Unrecognized boundary condition type\n");
		}
	}	
	*/
	unsigned int	index_msd;
	unsigned int	index_obj;
	for(i = 0; i < num_mapping; i++) 
	{
		index_msd = *(mapping+2*i);
		index_obj = *(mapping+2*i+1);		
		switch (bound->boundary_type[index_obj]) {
			case (0):   // Neumann type boundary condition   (Specify traction)
				deriv.vel[index_msd] += bound->boundary_value[index_obj] * (1.0/mass[index_msd]);				
				break;
			case (1):   // Drichlett type boundary condition (Fixed boundary - e.g. wall)
					    // Position specified in boundary_value
				deriv.vel[index_msd] = 0.0;
				deriv.pos[index_msd] = 0.0;
				break;
			case (2):  // Mixed boundary condition type i: 
					   // Normal displacement (not position) specified in boundary_value2_scalar
					   //        with normal direction specified in boundary_value2_vector
					   //        and tangential traction specified in boundary_value
				deriv.vel[index_msd] += bound->boundary_value[index_obj] * (1.0/mass[index_msd]);
				deriv.vel[index_msd] -= bound->boundary_value2_vector[index_obj] * (deriv.vel[index_msd] * bound->boundary_value2_vector[index_obj]);
				deriv.pos[index_msd] -= bound->boundary_value2_vector[index_obj] * (deriv.pos[index_msd] * bound->boundary_value2_vector[index_obj]);
				break;
			default:
				error_exit(0,"Unrecognized boundary condition type\n");
		}
	}	
}


/**
 * MSDObject::AllocState()
 * Allocates the memory for the integrator's local state members.
 * @param s state
 */
inline void MSDObject::AllocState(State &s)
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

/**
 * MSDObject::AllocJacobian()
 * Allocates the memory for the integrator's local Jacobian members.
 * @param J Jacobian
 */
inline void MSDObject::AllocJacobian(Jacobian &J)
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

/**
 * MSDObject::AddState()
 * Add the state1 and h*state2.
 * @param new_state State
 * @param state1 State
 * @param state2 State
 * @param h Real
 */
inline void MSDObject::AddState(State &new_state, const State &state1, const State &state2, const Real h)
{
	(*new_state.VEL) = (*state1.VEL) + h*(*state2.VEL);
	(*new_state.POS) = (*state1.POS) + h*(*state2.POS);
}

/**
 * MSDObject::ScaleState()
 * Scale the state to new_state with scaling value h.
 * @param new_state State
 * @param h Real
 * @param state State
 */
inline void MSDObject::ScaleState(State &new_state, const State &state, const Real h)
{
	(*new_state.VEL) = h*(*state.VEL);
	(*new_state.POS) = h*(*state.POS);  	
}

/**
 * MSDObject::NormState()
 * Calculate the norm of state.
 * @param state State
 * @return real the norm of state
 */
inline Real MSDObject::NormState(const State &state)
{
	return sqrt((*state.POS).length_sq() + (*state.VEL).length_sq());	
}

/**
 * MSDObject::StateDotState()
 * Calculate the dot product of state1 and state2.
 * @param state1 State
 * @param state2 State
 * @return real the dot product of state1 and state2
 */
inline Real MSDObject::StateDotState(const State &state1, const State &state2)
{
	return ((*state1.POS)*(*state2.POS) + (*state1.VEL)*(*state2.VEL));	
}

/**
 * MSDObject::MultiplyJacobianState()
 * Calculate the multiply of Jacobian and state.
 * @param J Jacobian
 * @param state State
 * @return out_state State
 */
inline void MSDObject::MultiplyJacobianState(State &out_state, const Jacobian &J, const State &state)
{
	ASSERT(J.size == state.size);	
	(*out_state.VEL) = (*J.A11)*(*state.VEL) + (*J.A12)*(*state.POS);
	(*out_state.POS) = (J.dA21)*(*state.VEL) + (J.dA22)*(*state.POS);	
}

void MSDObject::PrintJacobian(const Jacobian &J)
{
	int size = J.size;
	printf("A11 = ");
	vprint((*J.A11));	
	printf("A12 = ");
	vprint((*J.A12));	
	printf("dA21 = %f, dA22 = %f\n",J.dA21,J.dA22);
}

void MSDObject::PrintState(const State &state)
{
	int size = state.size;
	printf("vel = ");
	vprint(*state.VEL);	
	printf("pos = ");
	vprint(*state.POS);	
	printf("\n");
}

/**
 * MSDObject::IdentityMinushJacobian()
 * Calculate I-hJ.
 * @param J Jacobian
 * @param state State
 * @param f State
 * @param h Real
 */
inline void MSDObject::IdentityMinushJacobian(Jacobian &J, const State &state, const Real h) 
{  
	Real h_m;			// h/m
	unsigned int m1,m2;	// index of node 1 and 2
	//Real mp,mq;			// mass of p, mass of q
	Vector<Real> p	= zero_vector3;
	Vector<Real> q	= zero_vector3;
	Vector<Real> v	= zero_vector3;
	Vector<Real> w	= zero_vector3;	    

	// clear Jacobian A11 A12 matrix
	*(J.A11) = Matrix<Real>(3*state.size,3*state.size,0.0);
	*(J.A12) = Matrix<Real>(3*state.size,3*state.size,0.0);

/*
 * |         |         |   |     h df |    h df  |
 * |   A11   |   A12   |   | I - - -- |  - - --  |
 * |         |         |   |     m dv |    m dx  |
 * |---------+---------| = |----------|----------|
 * |         |         |   |          |          |
 * |  dA21   |  dA22   |   |   -hI    |     I    |
 * |         |         |   |          |          |
 */
	
	// build matrix A11 and A12 (J)
	for(unsigned int i=0;i<num_spring;i++)
	{
		// mass index start from 0
		m1 = spring[i].node[0];
		m2 = spring[i].node[1];		
		p = state.pos[m1];
		q = state.pos[m2];
		v = state.vel[m1];
		w = state.vel[m2];
		AddInternalSpringEntries(J.A11, J.A12, m1, m2, p, q, v, w, 
 			spring[i].k_stiff, spring[i].l_zero, spring[i].b_damp);		
	}		
	
	// A11 = -h/m Jv, A12 = -h/m Jx	
	for(unsigned int i=0; i<J.A11->m(); i++)
	{
		h_m = -h/mass[i/3];
		for(unsigned int j=0; j<J.A11->n(); j++)	
		{	
			(*(J.A11))[i][j] *= h_m;
			(*(J.A12))[i][j] *= h_m;
		}  
	}	

	// dA21 = -hI, dA22 = I
	J.dA21 = -h;
	J.dA22 = 1.0;

	// A11 = I - h/m Jv
	for(unsigned int i=0; i<J.A11->m(); i++)		
		(*(J.A11))[i][i] += 1.0;		
	// If boundary is set, set matrix row i to zero
	MSDBoundary		*bound = (MSDBoundary *) boundary;
	for(unsigned int i = 0; i < num_mapping; i++) {	
		unsigned int	index_msd;
		unsigned int	index_obj;	
		index_msd = *(mapping+2*i);
		index_obj = *(mapping+2*i+1);	
		if (bound->boundary_type[index_obj]==1) {
			for(unsigned int indexi = 3*index_msd; indexi < 3*index_msd+3; indexi++){
				for(unsigned int indexj = 0; indexj < J.A11->n(); indexj++)
				{
					if(indexi!=indexj){
						(*(J.A11))[indexi][indexj] = 0.0;								
						(*(J.A12))[indexi][indexj] = 0.0;
					}
					else
					{
						(*(J.A11))[indexi][indexj] = 1.0;								
						(*(J.A12))[indexi][indexj] = 1.0;
					}
				}								
			}
		}
	}		
}

/**
 * MSDObject::AccumStateSemiExplicit() for semi-Euler
 * Computes y(t+h) = y(t) + h * f(..)
 *		where	y(t+h)	= new_state
 *				y(t)	= state
 *				f(..)	= deriv
 * @param state current state.
 * @param deriv deriv state.
 * @param h time step.
 * @return new_state next state.
 */
inline void MSDObject::AccumStateSemiExplicit(State &new_state, const State &state, const State &deriv, const Real &h)
{
	unsigned int				i;
	static Vector<Real>			temp(3);

	// NOTE: The below is a good point to measure the overhead caused by binary operators
	for(i = 0; i < state.size; i++) {
//		new_state.vel[i] = state.vel[i] + deriv.vel[i] * h;
		temp = deriv.vel[i];
		temp *= h;
		temp += state.vel[i];
		new_state.vel[i] = temp;
//		new_state.pos[i] = state.pos[i] + new_state.vel[i] * h;
		temp = new_state.vel[i];
		temp *= h;
		temp += state.pos[i];
		new_state.pos[i] = temp;
  }
}

void MSDObject::setInitialCondition(void)
{	
	state.pos[1][1] = 0.0;
	state.vel[1][1] = -5.0;
}

/**
 * MSDObject::Simulate()
 * Simulates the MSD object by a single timestep.
 */
void MSDObject::Simulate(void)
{	
#ifdef _DEBUG
	//if(time==0) setInitialCondition();
	//PrintState(state);
#endif
	//ResetInitialBoundaryCondition();
	integrator->Integrate(*this, timestep);
	time+=timestep;	
	
	//We alse need to update the Boundary from states 
	//   so that we will have the correct Boundary definition for boundary calculations
	State2Bound();
}


/** 
 * MSDObject::Geom2State()
 * Copies geometry to state.
 */
void MSDObject::Geom2State(void)
{	
	static TriSurface *geom = (TriSurface *) geometry;
	unsigned int	index_msd;
	unsigned int	index_obj;

	/*
	for(unsigned int i=0; i < geom->num_vertex; i++) {
		state.pos[i] = geom->vertex[i].pos;
	}
	*/
	for(unsigned int i=0;i<num_mapping;i++) 
	{
		index_msd = *(mapping+2*i);
		index_obj = *(mapping+2*i+1);

		state.pos[index_msd] = geom->vertex[index_obj].pos;
	}	
}


/**
 * MSDObject::State2Geom()
 * Copies state to geometry.
 */
inline void MSDObject::State2Geom(void)
{	
	TriSurface *geom = (TriSurface *) geometry;
	unsigned int	index_msd;
	unsigned int	index_obj;
	/*
	for(unsigned int i=0; i<geom->num_vertex; i++) {
		geom->vertex[i].pos = state.pos[i];		
	}
	*/
	for(unsigned int i=0;i<num_mapping;i++) 
	{
		index_msd = *(mapping+2*i);
		index_obj = *(mapping+2*i+1);

		geom->vertex[index_obj].pos = state.pos[index_msd];					
	}	
	geom->calcNormals();
}


/**
 * MSDObject::State2Bound)
 * Copies state to boundary.
 */
inline void MSDObject::State2Bound(void)
{
	//printf("No MSD Boundary implemented...\n");
	/*
	MSDBoundary		*bound = (MSDBoundary *) boundary;		
	for(unsigned int i=0; i<bound->num_vertex; i++)
		bound->vertex[i].pos = state.pos[bound->global_id[i]];
	bound->calcNormals();
	*/
	MSDBoundary		*bound = (MSDBoundary *) boundary;		
	unsigned int	index_msd;
	unsigned int	index_obj;

	for(unsigned int i=0;i<num_mapping;i++) 
	{
		index_msd = *(mapping+2*i);
		index_obj = *(mapping+2*i+1);

		bound->vertex[index_obj].pos = state.pos[index_msd];	
	}
	bound->calcNormals();	
}


/**
 * MSDObject::SetupDisplay()
 * Sets up the display of the the MSD mesh.
* @param simObjectChildren Project file XML 'MSDObject' children node.
 */
void MSDObject::SetupDisplay(XMLNodeList * simObjectChildren) 
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
 * MSDObject::GetNodePosition()
 * Returns node position.
 * @param index node index.
 * @return vector<Real> node position.
 */
Vector<Real>		MSDObject::GetNodePosition(unsigned int index)
{
	return state.pos[index];
}


/**
 * MSDObject::GetNodeVelocity()
 * Returns node velocity.
 * @param index node index.
 * @return vector<Real> node velocity.
 */
Vector<Real>		MSDObject::GetNodeVelocity(unsigned int index)
{
	return	state.vel[index];
}


/**
 * MSDObject::GetNodeForce()
 * Returns the nodal force.
 * @param index node index.        
 * @return vector<Real> force.
 */
Vector<Real>		MSDObject::GetNodeForce(unsigned int index)
{
	return	force[index];
}


/**
 * MSDObject::Display()
 * Displays the MSD mesh
 */
void MSDObject::Display(void)
{
	// Copy the state into the geometry		
	State2Geom();	

	displayMngr->Display();
}


/**
 *	MSDObject::ReturnHapticModel()
 *	Returns a haptic model built around the current node and current face
 *
 */     
int MSDObject::ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model)
{	
	/**
	 * because Haptic units are in F[N], L[mm], v[mm/s]
	 * but in model units are in F[0.00001N], L[cm], v[cm/s], m[g]
	 */
	static MSDBoundary	*bound = (MSDBoundary *) boundary;	
	
	int msd_index = getMSDIndex(BoundaryNodeIndex);
	if(msd_index < 0) return -1;
	
	// build msd_model around BoundaryNodeIndex
	MSDModel  *msdmodel = &msdModel[msd_index];	
	unsigned int num_mass_internal = msdmodel->getMassSize(1);
	unsigned int num_spring[3];
	unsigned mass_index;
	unsigned spring_index;	
	
	//Vector<Real> oldpos(3, 0.0);
	//Vector<Real> oldvel(3, 0.0);
	
	// get number of spring in each type of spring
	num_spring[0] = msdmodel->getSpringSize(0);
	num_spring[1] = msdmodel->getSpringSize(1);
	num_spring[2] = msdmodel->getSpringSize(2);
	
	// save old position and velocity of BoundaryNodeIndex
	//oldpos = state.pos[msd_index];	
	//oldvel = state.vel[msd_index];	

	// update the position of BoundaryNodeIndex	(Boundary type 1)
	//state.pos[msd_index] = bound->GetPosition(BoundaryNodeIndex); //bound->boundary_value[BoundaryNodeIndex];
			
	// calculate the Low Order Linear Haptic Model
	unsigned int n = num_mass_internal*2*3;
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
	Vector<Real> s_force = zero_vector3;
	Vector<Real> d_force = zero_vector3; 
	Vector<Real> temp_force = zero_vector3;

	Vector<Real> *internal_node_force; 
	Vector<Real> contact_node_force;
	
	// Allocate internal_node_force
	internal_node_force = new Vector<Real>[num_mass_internal];
	if(internal_node_force == NULL) {
		error_exit(-1, "Cannot allocate memory for internal_node_forces!\n");
	}
	
	// Clear the forces
	contact_node_force = zero_vector3;
	for(unsigned int i = 0; i < num_mass_internal; i++) internal_node_force[i] = zero_vector3;

	int v1, v2; // spring node 1 and 2 in full model
	int m1, m2; // spring node 1 and 2 in msdmodel

	//
	// We will first calculate z_dot_0 and f_0
	//

	// loop through the spring types
	for(unsigned int type=0; type<3; type++) 
	{
		// loop through the springs of the given type
		for(unsigned int i = 0; i<num_spring[type]; i++)
		{			
			// dir = v1-v2
			spring_index = msdmodel->getSpringIndex(type, i);
			v1 = spring[spring_index].node[0];
			v2 = spring[spring_index].node[1];
			// collect the force on internal node only (type 1)
			m1 = msdmodel->getMassNum(1, v1);
			m2 = msdmodel->getMassNum(1, v2);
			 
			dir = state.pos[v1] - state.pos[v2];
			Real L = dir.length();			

			if(L>0.0){
				// Add spring force
				// f = -k * (1 - L0/L) * dir
				temp_force = -spring[spring_index].k_stiff * (1 - spring[spring_index].l_zero / L) * dir;
				// Add local damping
				rel_vel = state.vel[v1] - state.vel[v2];
				temp_force += -spring[spring_index].b_damp * ((rel_vel * dir) / (L*L)) * dir;
			}
			if(type == 0)
			{
				if( spring[spring_index].node[0] == msd_index ) 
				{
					contact_node_force += temp_force;
					internal_node_force[m2] -= temp_force;
				}
				else
				{
					internal_node_force[m1] += temp_force;				
					contact_node_force -= temp_force;					
				}
			} 
			
			if(type == 1) 
			{				
				internal_node_force[m1] += temp_force;
				internal_node_force[m2] -= temp_force;			
			}

			if(type == 2)
			{
				if( msdmodel->isInMassIndex(1, v1) ) 				
					internal_node_force[m1] += temp_force;			
				if( msdmodel->isInMassIndex(1, v2) ) 				
					internal_node_force[m2] -= temp_force;			 
			}			
		}
	}

	// for virtual spring ////////////////////////////////////////////
	if(num_ground>0) {		
		// loop through the mass types
		for(unsigned int type=0; type<2; type++) 
		{
			// loop through the mass of the given type
			for(unsigned int i = 0; i<msdModel->getMassSize(type); i++)
			{
				mass_index = msdmodel->getMassIndex(type,i);
				// force from vspring 
				// because index of vspring is the same as node index and ground index
				// dir = v1-v2
				v1 = vspring[mass_index].node[0];	// mass node
				v2 = vspring[mass_index].node[1];	// ground node (no force on it)
				
				dir = state.pos[v1] - ground_pos[v2];
				Real L = dir.length();

				// Add spring force
				// f = -k * (1 - L/L0) * dir
				temp_force = -vspring[mass_index].k_stiff * dir;				
				
				// Add local damping
				if(L>0.0) {	// check for divide by zero
					rel_vel = state.vel[v1];
					temp_force += -vspring[mass_index].b_damp * ((rel_vel * dir) / (L*L)) * dir;				
				}
				if(type==0)
					contact_node_force += temp_force;
				if(type==1)
					internal_node_force[i] += temp_force;
			}
		}
	}
	// end for virtual spring //////////////////////////////////////////

	(*(model->f_0)) = contact_node_force;
	
	// seed z_dot0	
	for(unsigned int i = 0; i < num_mass_internal; i++)
	{	
		Vector<Real> acc = zero_vector3;
		Vector<Real> vel = zero_vector3;
		mass_index = msdmodel->getMassIndex(1, i);
		double mm = mass[mass_index];

		if (mm > 0)
		{
			double mmi = 1/mm;			
			acc = internal_node_force[i] * mmi;
		} 		
	
		// acc
		(*(model->zdot_0))[3*i]   = acc[0];
		(*(model->zdot_0))[3*i+1] = acc[1];
		(*(model->zdot_0))[3*i+2] = acc[2];

		// vel
		 vel = GetNodeVelocity(mass_index);
		(*(model->zdot_0))[n_2+3*i]   = vel[0];
		(*(model->zdot_0))[n_2+3*i+1] = vel[1];
		(*(model->zdot_0))[n_2+3*i+2] = vel[2];
	}
	
	//
	// We will now calculate A, B, C, and D matrices
	//

	// loop through the spring types
	for(unsigned int type=0; type<3; type++)
	{
		// loop through the springs of the given type
		for(unsigned int i = 0; i< num_spring[type]; i++)
		{
			spring_index = msdmodel->getSpringIndex(type, i);
			Spring& s = spring[spring_index];
			Vector<Real> p = GetNodePosition(s.node[0]);
			Vector<Real> q = GetNodePosition(s.node[1]);
			Vector<Real> v = GetNodeVelocity(s.node[0]);
			Vector<Real> w = GetNodeVelocity(s.node[1]);

			if(type==0)  // contact spring
			{
				// check if node[0] is contact node or not
				if(msdmodel->isInMassIndex(type, s.node[0])) 
				{
					// node[0] is contact node then 
					// check if node[1] is boundary node or not
					if( !isFixedBoundary(s.node[1]) ) 					
					{
						// node[1] is not boundary node, then spring is contact spring
						// node[1] is internal node
						int index = msdmodel->getMassNum(type+1, s.node[1]);
						if(index != -1)
							AddContactSpringEntries(model, index, p, q, v, w, s.k_stiff, s.l_zero, s.b_damp);	
					}
					else 
						// node[1] is boundary node, then spring is boundary spring (add only D matrix)
						AddContactSpringEntries(model, p, q, v, w, s.k_stiff, s.l_zero, s.b_damp);						
				}
				else
				{
					// node[1] is contact node then 
					// check if node[0] is boundary node or not
					if( !isFixedBoundary(s.node[0]) )
					{
						// node[0] is not boundary node, then spring is contact spring
						// node[0] is internal node
						int index = msdmodel->getMassNum(type+1, s.node[0]);
						if(index != -1)
							AddContactSpringEntries(model, index, q, p, w, v, s.k_stiff, s.l_zero, s.b_damp);							
					}
					else 
						// node[0] is boundary node, then spring is contact spring (add only D matrix)
						AddContactSpringEntries(model, q, p, w, v, s.k_stiff, s.l_zero, s.b_damp);						
				}
			}
			
			if(type==1) // internal spring
			{
				// check if node[0] and node[1] are not boundary nodes
				if( !(isFixedBoundary(s.node[0]) || isFixedBoundary(s.node[1])) ) 
					AddInternalSpringEntries(model, msdmodel->getMassNum(type, s.node[0]), msdmodel->getMassNum(type, s.node[1]), p, q, v, w, s.k_stiff, s.l_zero, s.b_damp);						
			}

			if(type==2) // boundary spring
			{
				// check if node[0] is boundary node or not
				if(msdmodel->isInMassIndex(type, s.node[0])) 
				{
					int index = msdmodel->getMassNum(type-1, s.node[1]);
					if(index != -1)
						AddBoundarySpringEntries(model, index, q, p, w, v, s.k_stiff, s.l_zero, s.b_damp);						
				}
				else 
				{
					int index = msdmodel->getMassNum(type-1, s.node[0]);
					if(index != -1)						
						AddBoundarySpringEntries(model, index, p, q, v, w, s.k_stiff, s.l_zero, s.b_damp);					
				}
			}			
		}
	}

	// for virtual spring ////////////////////////////////////////////
	if(num_ground>0)
	{
		// loop through the mass types
		for(unsigned int type=0; type<3; type++) 
		{
			// loop through the mass of the given type
			for(unsigned int i = 0; i<msdModel->getMassSize(type); i++)
			{
				mass_index = msdmodel->getMassIndex(type, i);				
				// force from vspring 
				// because index of vspring is the same as node index and ground index
				Spring& s = vspring[mass_index];
				Vector<Real> p = GetNodePosition(s.node[0]);
				Vector<Real> q = ground_pos[s.node[1]];	// ground
				Vector<Real> v = GetNodeVelocity(s.node[0]);
				Vector<Real> w = zero_vector3;			// ground
				if(type==0)			
					AddContactSpringEntries(model, p, q, v, w, s.k_stiff, s.l_zero, s.b_damp);																		
				if(type==2)	{
					int index = msdmodel->getMassNum(type-1, s.node[0]);
					if(index!=-1)
						AddBoundarySpringEntries(model, index, q, p, w, v, s.k_stiff, s.l_zero, s.b_damp);						
				}
			}
		}
	}
	// end for virtual spring //////////////////////////////////////////

	// divide rows of A11, A12 by node masses
	for(unsigned int i = 0; i < model->A11->m(); i++)
	{
		double mm = mass[msdmodel->getMassIndex(1, i/3)];
		if( mm > 0.0)
		{
			double mmi = 1/mm;
			for(unsigned int j = 0; j < model->A11->n(); j++)
			{			
				(*(model->A11))[i][j] = (*(model->A11))[i][j] * mmi;
				(*(model->A12))[i][j] = (*(model->A12))[i][j] * mmi;
			}  
		}
		else
		{
			for(unsigned int j = 0; j < model->A11->n(); j++)
	 		{			
				(*(model->A11))[i][j] = 0.0;
				(*(model->A12))[i][j] = 0.0;				
			}  			
		}

	}

	// divide rows of B1 by node masses
	for(unsigned int i = 0; i < model->B1->m(); i++)
	{
		double mm = mass[msdmodel->getMassIndex(1, i/3)];
		if(mm > 0.0)
		{
			double mmi = 1/mm;
			for(unsigned int j = 0; j < model->B1->n(); j++)
				(*(model->B1))[i][j] = (*(model->B1))[i][j] * mmi;  
		}		
		else
		{
			for(unsigned int j = 0; j < model->B1->n(); j++)
				(*(model->B1))[i][j] = 0.0; 
		}
	}
	
	//printf("\n D\n");		vprint(*(model->D));
	//printf("\n C11\n");	vprint(*(model->C11));
	//printf("\n C12\n");	vprint(*(model->C12));

	delete []internal_node_force;

	Model = (*model);
	delete model;
	
	// set state back to old state
	//state.pos[msd_index] = oldpos;	
	//state.vel[msd_index] = oldvel;	

	return 0;	
}


////////////////////////////////////////////////////// //////////
//
//	MSDObject::ReturnHapticModel()
//
//		Returns a haptic model built around the current face
//     
//		FIXME: This _should_ be based around boundary nodes, 
//             but as currently implemented, boundary nodes are not
//             integrated.  Is this the correct behavior?
//
int MSDObject::ReturnHapticModel(unsigned int FaceNodeIndex1,
	    			   unsigned int FaceNodeIndex2,
					   unsigned int FaceNodeIndex3, GiPSiLowOrderLinearHapticModel &Model)
{	
	return 0;
}

/**
 *	MSDObject::isFixBoundary()
 *	Returns true, if index is in fix_boundary msd index
 *
 */
/*bool MSDObject::isFixedBoundary(unsigned int index)
{
	if(((MSDBoundary*)boundary)->boundary_type[index] == 1)
		return true;
	else
		return false;
}
*/
bool MSDObject::isFixedBoundary(unsigned int index)
{
	bool found = false;	
	for(unsigned int i=0; i<num_boundary; i++)
	{
		if( fix_boundary[i] == index ) {
			found = true;
			break;
		}
	}
	return found;
}

int MSDObject::getMSDIndex(unsigned int OBJIndex)
{
	bool found = false;	
	unsigned int	index_msd;
	unsigned int	index_obj;
	
	for(unsigned int i=0; i<num_mapping; i++)
	{
		index_msd = *(mapping+2*i);
		index_obj = *(mapping+2*i+1);
		if( OBJIndex == index_obj ) {
			found = true;
			break;
		}
	}

	if(!found) index_msd = -1;

	return index_msd;
}

int MSDObject::getOBJIndex(unsigned int MSDIndex)
{
	bool found = false;	
	unsigned int	index_msd;
	unsigned int	index_obj;
	
	for(unsigned int i=0; i<num_mapping; i++)
	{
		index_msd = *(mapping+2*i);
		index_obj = *(mapping+2*i+1);
		if( MSDIndex == index_msd ) {
			found = true;
			break;
		}
	}

	if(!found) index_obj = -1;

	return index_obj;
}

/**
 * Translate the object by the indicated amount.
 * 
 * @param tx Change in x position.
 * @param ty Change in y position.
 * @param tz Change in z position.
 */
void MSDObject::Translate(float tx, float ty, float tz)
{
	unsigned int	i;
	Point			*v;

	for(i = 0; i < num_mass; i++) {
		v = &(massPoint[i]);
		v->pos[0] += tx;
		v->pos[1] += ty;
		v->pos[2] += tz; 
	}

	Vector<Real>	*w;
	for(i = 0; i < num_ground; i++) {
		w = &(ground_pos[i]);
		(*w)[0] += tx;
		(*w)[1] += ty;
		(*w)[2] += tz;
	}
}



/**
 * Translate the object by the indicated amount.
 * 
 * @param p displacement vector
 */
void MSDObject::Translate(Vector<Real> p)
{
	ASSERT( (p.dim()==3) );
	unsigned int	i;
	Point			*v;

	for(i = 0; i < num_mass; i++) {
		v = &(massPoint[i]);
		v->pos += p;
	}

	Vector<Real>	*w;
	for(i = 0; i < num_ground; i++) {
		w = &(ground_pos[i]);
		*w = *w + p;		
	}
}



/**
 * Translate the object by the indicated amount.
 * 
 * @param sx X scaling.
 * @param sy Y scaling.
 * @param sz Z scaling.
 */
void MSDObject::Scale(float sx, float sy, float sz)
{
	unsigned int	i;
	Point			*v;

	for(i = 0; i < num_mass; i++) {
		v = &(massPoint[i]);
		v->pos[0] *= sx;
		v->pos[1] *= sy;
		v->pos[2] *= sz;
	}

	Vector<Real>	*w;
	for(i = 0; i < num_ground; i++) {
		w = &(ground_pos[i]);
		(*w)[0] *= sx;
		(*w)[1] *= sy;
		(*w)[2] *= sz;
	}
}



/**
 * Rotate the object around a given axis.
 * 
 * @param angle Angle to rotate by.
 * @param ax Axis x coord.
 * @param ay Axis y coord.
 * @param az Axis z coord.
 */
void MSDObject::Rotate(Real angle, Real ax, Real ay, Real az)
{
	unsigned int	i;
	Point			*v;
	Matrix<Real>	R(3, 3);
	Real			s	= (Real) cos(angle/2.0);
	Real			sp	= (Real) sin(angle/2.0);
	Real			a	= ax*sp;
	Real			b	= ay*sp;
	Real			c	= az*sp;
	Real			l	= (Real) sqrt(a*a + b*b + c*c + s*s);

	a /= l;
	b /= l;
	c /= l;
	s /= l;

	R[0][0] = 1-2*b*b-2*c*c;
	R[0][1] = 2*a*b-2*s*c;
	R[0][2] = 2*a*c+2*s*b;
	R[1][0] = 2*a*b+2*s*c;
	R[1][1] = 1-2*a*a-2*c*c;
	R[1][2] = 2*b*c-2*s*a;
	R[2][0] = 2*a*c-2*s*b;
	R[2][1] = 2*b*c+2*s*a;
	R[2][2] = 1-2*a*a-2*b*b;

	for(i = 0; i < num_mass; i++) {
		v = &(massPoint[i]);
		v->pos = R * v->pos;
	}

	Vector<Real>	*w;
	for(i = 0; i < num_ground; i++) {
		w = &(ground_pos[i]);
		*w = R * *w;		
	}
}

/**
 * Rotate the object using a given rotation matrix.
 * 
 * @param R rotation matrix to be used in the transformation
 *
 */
void MSDObject::Rotate(Matrix<Real> R)
{
	ASSERT( (R.m()==3) && (R.n()==3) );
	unsigned int	i;
	Point			*v;
	for(i = 0; i < num_mass; i++) {
		v = &(massPoint[i]);
		v->pos = R * v->pos;
	}
	
	Vector<Real>	*w;
	for(i = 0; i < num_ground; i++) {
		w = &(ground_pos[i]);
		*w = R * *w;		
	}
}

/**
 * MSDBoundary::init
 * MSD Boundary initializer.
 * @param ParentObj pointer of SimObject.
 * @param num_vertex number of vertex.
 * @param num_face number of face.
 */
void MSDBoundary::Init(MSDObject *ParentObj, unsigned int num_vertex, unsigned int num_face)
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


/**
 * MSDBoundary::GetPosition
 * Get positions of the nodes of the boundary.
 * @param index node index.
 * @return vector<Real> node position.
 */
Vector<Real>	MSDBoundary::GetPosition(unsigned int index) 
{
	int msd_index = ((MSDObject*)Object)->getMSDIndex(index);
	if (msd_index != -1)
		return	Object->GetNodePosition(msd_index);
	else
		return vertex[index].pos;

	//return	Object->GetNodePosition(global_id[index]);
}


/**
 * MSDBoundary::GetPosition
 * Get positions of the nodes of the boundary.
 * @return all node posintion. 
 */
void			MSDBoundary::GetPosition(Vector<Real> *Bpos)
{
	int msd_index;
	for (unsigned int index=0; index < this->num_vertex; index++) {
		msd_index = ((MSDObject*)Object)->getMSDIndex(index);
		if (msd_index != -1)
			Bpos[index] = Object->GetNodePosition(msd_index);
		else
			Bpos[index] = vertex[index].pos;
	}

	/*
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bpos[index]=Object->GetNodePosition(global_id[index]);
	}
	*/
}


/**
 * MSDBoundary::GetVelocity
 * Get velocity of the nodes of the boundary.
 * @param index node index.
 * @return vector<Real> velocity.
 */
Vector<Real>	MSDBoundary::GetVelocity(unsigned int index)
{
	Vector<Real>	Vel(3,0.0);
	int msd_index = ((MSDObject*)Object)->getMSDIndex(index);
	if (msd_index != -1)
		return	Object->GetNodeVelocity(msd_index);
	else
		return Vel;

	//return	Object->GetNodeVelocity(global_id[index]);
}

/**
 * MSDBoundary::GetVelocity
 * Get velocity of the nodes of the boundary.
 * @return all node velocity. 
 */
void			MSDBoundary::GetVelocity(Vector<Real> *Bvel)	
{
	Vector<Real>	Vel(3,0.0);
	int msd_index;
	for (unsigned int index=0; index < this->num_vertex; index++) {
		msd_index = ((MSDObject*)Object)->getMSDIndex(index);
		if (msd_index != -1)
			Bvel[index] = Object->GetNodeVelocity(msd_index);
		else
			Bvel[index] = Vel;
	}
	
	/*
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bvel[index]=Object->GetNodeVelocity(global_id[index]);
	}
	*/
}


/**
 * MSDBoundary::ResidualForce
 * Get the Reaction force that the body applies to the external 
 *         world as a result of the constraints
 *		   (i.e. contact force, constraint force, lagrange multiplier)
 * @param index node index.
 * @return vector<Real> reaction force.
 */
Vector<Real>	MSDBoundary::GetReactionForce(unsigned int index)
{
	Vector<Real>	ResForce(3,0.0);
	int msd_index = ((MSDObject*)Object)->getMSDIndex(index);
	if( msd_index != -1) 
	{
		switch (boundary_type[index]) {
			case (0):   // Neumann type boundary condition   (Specify traction)
				ResForce =	0.0;
				break;
			case (1):   // Drichlett type boundary condition (Fixed boundary - e.g. wall)
						// Position specified in boundary_value
				ResForce =	Object->GetNodeForce(msd_index);
				break;
			case (2):	// Mixed boundary condition type i: 
						// Normal displacement (not position) specified in boundary_value2_scalar
						//        with normal direction specified in boundary_value2_vector
						//        and tangential traction specified in boundary_value
				ResForce =	boundary_value2_vector[index] * (Object->GetNodeForce(msd_index) * boundary_value2_vector[index]);
				break;
			default:
				error_exit(0,"Unrecognized boundary condition type\n");
		}
	}
	return ResForce;

	/*
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
	*/
}


/**
 * MSDBoundary::GetReactionForce
 * Get reaction force of the nodes of the boundary.
 * @return all reaction force. 
 */
void			MSDBoundary::GetReactionForce(Vector<Real> *Bforce)
{
	
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bforce[index] = GetReactionForce(index);
	}
	
	/*
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bforce[index]=GetReactionForce(global_id[index]);
	}
	*/
}


/**
 * MSDBoundary::Set
 * Sets MSD Boundary.
 * @param index node index.
 * @param boundary_type boundary type.
 * @param boundary_value boundary value.
 * @param boundary_value2_scalar boundary value 2 scalar.
 * @param boundary_value2_vector boundary value 2 vector.
 */
void		MSDBoundary::Set(unsigned int index, unsigned int boundary_type, 
							 Vector<Real> boundary_value, 
							 Real boundary_value2_scalar, Vector<Real> boundary_value2_vector) 
{
	this->boundary_type[index]			=boundary_type;
	this->boundary_value[index]			=boundary_value;
	this->boundary_value2_scalar[index]	=boundary_value2_scalar;
	this->boundary_value2_vector[index]	=boundary_value2_vector;		
}


/**
 * MSDBoundary::Set
 * Sets MSD Boundary.
 * @param boundary_type array of boundary type.
 * @param boundary_value array of boundary value.
 * @param boundary_value2_scalar array of boundary value 2 scalar.
 * @param boundary_value2_vector array of boundary value 2 vector.
 */
void		MSDBoundary::Set(unsigned int *boundary_type, 
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
 * MSDBoundary::ReturnHapticModel()
 * Returns a haptic model.
 * @param BoundaryNodeIndex boundary node index.
 * @param Model GiPSi low order linear Haptic model.
 * @return int result. 
 */
int	MSDBoundary::ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model)
{
	return Object->ReturnHapticModel(BoundaryNodeIndex, Model);
}


int	MSDBoundary::ReturnHapticModel(unsigned int FaceNodeIndex1,
									  unsigned int FaceNodeIndex2,
									  unsigned int FaceNodeIndex3,  GiPSiLowOrderLinearHapticModel &Model)
{
	return Object->ReturnHapticModel(FaceNodeIndex1,FaceNodeIndex2,FaceNodeIndex3, Model);
}

/**
 * MSDBoundary::isTypeOneBoundary()
 * Returns true if index is type 1 boundary
 * @param unsign int index. 
 * @return bool result. 
 */
//bool MSDBoundary::isTypeOneBoundary(unsigned int index)
//{
//	if (boundary_type[index] == 1)
//		return true;
//	else
//		return false;
	/*
	int msd_index = ((MSDObject*)Object)->getMSDIndex(index);
	if (msd_index!=-1){
		printf("msd index = %d, isfix = %d, bound type = %d\n",msd_index,((MSDObject*)Object)->isFixedBoundary(msd_index),boundary_type[msd_index]);
		if (boundary_type[msd_index] == 1)
			return true;
		else
			return false;
	}
	else return false;
	*/
//}

/*
bool MSDBoundary::isFixedBoundary(unsigned int index)
{
	int msd_index = Object->getMSDIndex(index);
	if (msd_index != -1)
		return Object->isFixedBoundary(msd_index);
	else
		return false;
}
*/


/**
 * MSDBoundary::ResetBoundaryCondition()
 * Reset boundary condition.
 */
void MSDBoundary::ResetBoundaryCondition(void)
{	
	MSDBoundary*	bound = this;
	for(unsigned int i=0;i<num_vertex;i++) 
	{
		bound->Set(i,0,zero_vector3,0.0,zero_vector3);
	}
	((MSDObject*)Object)->ResetInitialBoundaryCondition();	
}


/**
 * MSDDomain::init
 * MSD Domain initializer.
 * @param ParentObject SimObject pointer.
 */
void MSDDomain::Init(MSDObject *ParentObj)
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
 * MSDDomain::SetDomStress
 * Sets External Stresses.
 * @param element_index.
 * @param Streess_Tensor.
 */
void MSDDomain::SetDomainStress(unsigned int element_index, Matrix<Real> Stress_Tensor)
{
	DomStress[element_index]=Stress_Tensor;
}


/**
 * MSDDomain::SetDomStress
 * Sets External Stresses.
 * @param Stress_Tensor_Array.
 */
void MSDDomain::SetDomainStress(Matrix<Real> *Stress_Tensor_Array)
{
	for (unsigned int index=0; index < this->num_tet; index++){
		DomStress[index]=Stress_Tensor_Array[index];
	}
}

MassNode::MassNode()
{
	refid = 0;
}

void MassNode::addSpringIndex(unsigned int index)
{	
	spring_index.push_back(index);		
}

unsigned int MassNode::getSpringIndex(unsigned int num)
{
	return spring_index[num];
}

bool MassNode::isInSpringIndex(unsigned int index)
{
	bool found = false;
	for(unsigned int i=0;i<spring_index.size(); i++)
	{
		if(spring_index[i] == index){
			found = true;
			break;
		}
	}
	return found;	
}

MSDModel::MSDModel()
{
}

void MSDModel::addSpringIndex(unsigned int type, unsigned int index)
{
	spring_index[type].push_back(index);	
}

void MSDModel::addMassIndex(unsigned int type, unsigned int index)
{
	mass_index[type].push_back(index);
}

vector<unsigned int> MSDModel::getMassIndex(unsigned int type)
{
	return mass_index[type];
}

vector<unsigned int> MSDModel::getSpringIndex(unsigned int type)
{
	return spring_index[type];
}

unsigned int MSDModel::getMassIndex(unsigned int type, unsigned int num)
{
	return mass_index[type][num];
}

unsigned int MSDModel::getSpringIndex(unsigned int type, unsigned int num)
{
	return spring_index[type][num];
}

int MSDModel::getMassNum(unsigned int type, unsigned int index)
{
	int found = -1;
	for(unsigned int i=0;i<mass_index[type].size(); i++)
	{
		if(mass_index[type][i] == index){
			found = i;
			break;
		}
	}
	return found;
}

int MSDModel::getSpringNum(unsigned int type, unsigned int index)
{
	int found = -1;
	for(unsigned int i=0;i<spring_index[type].size(); i++)
	{
		if(spring_index[type][i] == index){
			found = i;
			break;
		}
	}
	return found;
}

bool MSDModel::isInSpringIndex(unsigned int type, unsigned int index)
{
	bool found = false;
	for(unsigned int i=0;i<spring_index[type].size(); i++)
	{
		if(spring_index[type][i] == index){
			found = true;
			break;
		}
	}
	return found;
}

bool MSDModel::isInMassIndex(unsigned int type, unsigned int index)
{
	bool found = false;
	for(unsigned int i=0;i<mass_index[type].size(); i++)
	{
		if(mass_index[type][i] == index){
			found = true;
			break;
		}
	}
	return found;
}

MSDModelBuilder::MSDModelBuilder()
{	
	depth = 0;
	msd = NULL;
	total_msdModel = 0;
}

MSDModelBuilder::~MSDModelBuilder()
{	
	delete massnode;
}

void MSDModelBuilder::BuildModel(MSDModel *msdModel, MSDObject *msd, unsigned int depth)
{
	//MSDBoundary*	bound = (MSDBoundary *) msd->GetBoundaryPtr();	
	//total_msdModel = bound->num_vertex;
	total_msdModel = msd->getNumMass();

	unsigned int total_mass = msd->getNumMass();
	unsigned int total_spring = msd->getNumSpring();
	
	this->depth = depth;
	this->msd = msd;

	// create the massnode
	massnode = new MassNode[total_mass];
	if(massnode==NULL) {
		error_exit(-1, "Cannot allocate memory for massnode!\n");
	}
	// add mass index information to each massnode
	for(unsigned int i=0; i<total_mass; i++)
	{
		massnode[i].refid = i;				
	}
	// add spring index information to each massnode
	for(unsigned int i=0; i<total_spring; i++)
	{
		unsigned int sIndex = msd->getSpring(i).refid;
		unsigned int node0 = msd->getSpring(i).node[0];
		unsigned int node1 = msd->getSpring(i).node[1];
		if (node0 < total_mass)
			massnode[node0].addSpringIndex(sIndex);
		if (node1 < total_mass)
			massnode[node1].addSpringIndex(sIndex);				
	}

	// offline build msd model for all nodeIndex (i=nodeIndex)
	for(unsigned int i=0; i<total_msdModel; i++)
	{		
		// initilize num_mass and num_spring to zero
		unsigned int nodeIndex = massnode[i].refid;

		// initilzation part // depth = 1 //////////////////////////////////////////////////
		// for nodeIndex is contact node (type 0). 
		// All springs that contact this nodeIndex will be the contact spring (type 0)
		// get number of spring of massnode i
		unsigned int sNum = massnode[nodeIndex].getSpringSize();
		
		msdModel[nodeIndex].addMassIndex(0, nodeIndex);
		// for all spring_index that connected with nodeIndex i
		for(unsigned int sIndex = 0; sIndex<sNum; sIndex++)
		{
			unsigned int spring_index = massnode[nodeIndex].getSpringIndex(sIndex);
			msdModel[nodeIndex].addSpringIndex(0, spring_index);
			Spring spring = msd->getSpring(spring_index);
			if (spring.node[0] == nodeIndex) 
			{
				if ( !msdModel[nodeIndex].isInMassIndex(1, spring.node[1]) )
					msdModel[nodeIndex].addMassIndex(1, spring.node[1]);
			}
			else 
			{
				if ( !msdModel[nodeIndex].isInMassIndex(1, spring.node[0]) )
					msdModel[nodeIndex].addMassIndex(1, spring.node[0]);		
			}
		}	

		// depth looping part ////////////////////////////////////////////////////////////
		for(unsigned int d=1; d<depth; d++) 
		{
			unsigned int num_mass = msdModel[nodeIndex].getMassSize(1);
			vector<unsigned int> mass_index_list = msdModel[nodeIndex].getMassIndex(1);
			for(unsigned int j=0; j<num_mass; j++) 
			{
				unsigned int mass_index = massnode[mass_index_list[j]].refid;
				// get number of spring of massnode j
				sNum = massnode[mass_index].getSpringSize();
				// for all spring_index that connected with nodeIndex i
				for(unsigned int sIndex = 0; sIndex<sNum; sIndex++)
				{
					unsigned int spring_index = massnode[mass_index].getSpringIndex(sIndex);
					Spring spring = msd->getSpring(spring_index);

					if(d<(depth-1)) // nodes will be internal node, spring will be internal spring
					{
						msdModel[nodeIndex].addSpringIndex(1, spring_index);					
						
						if (spring.node[0]==mass_index)
						{
							if ( !msdModel[nodeIndex].isInMassIndex(1, spring.node[1]) )
								msdModel[nodeIndex].addMassIndex(1, spring.node[1]);
						}
						else
						{
							if ( !msdModel[nodeIndex].isInMassIndex(1, spring.node[0]) )
								msdModel[nodeIndex].addMassIndex(1, spring.node[0]);	 	
						}
					}
					else 
					{						
						// spring will be internal spring
						if( msdModel[nodeIndex].isInMassIndex(1, spring.node[0]) &&
							msdModel[nodeIndex].isInMassIndex(1, spring.node[1]) )
						{
							if ( !msdModel[nodeIndex].isInSpringIndex(1, spring_index) )
								msdModel[nodeIndex].addSpringIndex(1, spring_index);
						}

						// spring will be boundary spring
						if( !(msdModel[nodeIndex].isInSpringIndex(0, spring_index) ||
							  msdModel[nodeIndex].isInSpringIndex(1, spring_index)) ) 
						{
							if ( !msdModel[nodeIndex].isInSpringIndex(2, spring_index) )
								msdModel[nodeIndex].addSpringIndex(2, spring_index);							
						
							// nodes will be boundary node, 
							if (spring.node[0]==mass_index) 
							{
								if( !msdModel[nodeIndex].isInMassIndex(2, spring.node[1]) && !msdModel[nodeIndex].isInMassIndex(1, spring.node[1]))
									msdModel[nodeIndex].addMassIndex(2, spring.node[1]);
							}
							else
							{
								if( !msdModel[nodeIndex].isInMassIndex(2, spring.node[0]) && !msdModel[nodeIndex].isInMassIndex(1, spring.node[0]))
									msdModel[nodeIndex].addMassIndex(2, spring.node[0]);
							}
						} 						
					} 
				}
			}
		}
		// no need finalize part because all springs in finalize step are connected with boundary node		
	}
}

/*
void MSDModelBuilder::BuildModel(MSDModel *msdModel, MSDObject *msd, unsigned int depth)
{
	unsigned int total_mass = msd->getNumMass();
	unsigned int total_spring = msd->getNumSpring();
	
	this->depth = depth;
	this->msd = msd;

	// create the massnode
	massnode = new MassNode[total_mass];
	if(massnode==NULL) {
		error_exit(-1, "Cannot allocate memory for massnode!\n");
	}
	// add mass index information to each massnode
	for(unsigned int i=0; i<total_mass; i++)
	{
		massnode[i].refid = i;				
	}
	// add spring index information to each massnode
	for(unsigned int i=0; i<total_spring; i++)
	{
		unsigned int sIndex = msd->getSpring(i).refid;
		unsigned int node0 = msd->getSpring(i).node[0];
		unsigned int node1 = msd->getSpring(i).node[1];
		massnode[node0].addSpringIndex(sIndex);
		massnode[node1].addSpringIndex(sIndex);				
	}

	// build model for all nodeIndex
	// i = nodeIndex
	for(unsigned int i=0; i<total_mass; i++)
	{		
		// initilize num_mass and num_spring to zero
		unsigned int nodeIndex = massnode[i].refid;

		// initilzation part // depth = 1 //////////////////////////////////////////////////
		// get number of spring of massnode i
		unsigned int sNum = massnode[nodeIndex].getNumberSpring();
		// for all spring_index that connected with nodeIndex i
		for(unsigned int sIndex = 0; sIndex<sNum; sIndex++)
		{
			unsigned int spring_index = massnode[nodeIndex].getSpringIndex(sIndex);
			msdModel[nodeIndex].addSpringIndex(spring_index);
			Spring spring = msd->getSpring(spring_index);
			if (spring.node[0]==nodeIndex) 
				msdModel[nodeIndex].addMassIndex(spring.node[1]);
			else
				msdModel[nodeIndex].addMassIndex(spring.node[0]);		
		}	

		// depth looping part ////////////////////////////////////////////////////////////
		for(unsigned int d=1; d<depth; d++) 
		{
			unsigned int num_mass = msdModel[nodeIndex].getNumMass();
			unsigned int *mass_index_list = msdModel[nodeIndex].getMassIndex();
			for(unsigned int j=0; j<num_mass; j++) 
			{
				unsigned int mass_index = massnode[mass_index_list[j]].refid;
				// get number of spring of massnode j
				sNum = massnode[mass_index].getNumberSpring();
				// for all spring_index that connected with nodeIndex i
				for(unsigned int sIndex = 0; sIndex<sNum; sIndex++)
				{
					unsigned int spring_index = massnode[mass_index].getSpringIndex(sIndex);
					msdModel[nodeIndex].addSpringIndex(spring_index);
					Spring spring = msd->getSpring(spring_index);
					if (spring.node[0]==mass_index)
						msdModel[nodeIndex].addMassIndex(spring.node[1]);
					else
						msdModel[nodeIndex].addMassIndex(spring.node[0]);		
				}
			}
		}

		// finalization part /////////////////////////////////////////////////////////////		
		unsigned int num_mass = msdModel[nodeIndex].getNumMass();
		unsigned int *mass_index_list = msdModel[nodeIndex].getMassIndex();
		for(unsigned int j=0; j<num_mass; j++) 
		{
			unsigned int mass_index = massnode[mass_index_list[j]].refid;
			// get number of spring of massnode j
			sNum = massnode[mass_index].getNumberSpring();
			// for all spring_index that connected with nodeIndex i
			for(unsigned int sIndex = 0; sIndex<sNum; sIndex++)
			{
				unsigned int spring_index = massnode[mass_index].getSpringIndex(sIndex);				
				Spring spring = msd->getSpring(spring_index);
				if (spring.node[0]==mass_index) 
				{
					if (msdModel[nodeIndex].isInMassIndex(spring.node[1])) 
						msdModel[nodeIndex].addSpringIndex(spring_index);
				}
				else 
				{
					if (msdModel[nodeIndex].isInMassIndex(spring.node[0])) 
						msdModel[nodeIndex].addSpringIndex(spring_index);						
				}
			}	
		}
	}
}
*/

void MSDModelBuilder::print(MSDModel* msdModel)
{
	for(unsigned int i=0; i<total_msdModel; i++)
	{
		unsigned int mass_index = massnode[i].refid;
		printf("= msdModel %3d ==============================================================\n", mass_index);

		for(unsigned int type=0; type<3; type++)
		{
			printf("--- type : %d ---------------------\n", type);
			unsigned int num_mass = msdModel[mass_index].getMassSize(type);
			unsigned int num_spring = msdModel[mass_index].getSpringSize(type);			
			printf("#mass_index = %d, #spring_index = %d\n", num_mass, num_spring);
			printf("mass_index [");
			for(unsigned int j=0; j<num_mass; j++)
				printf(" %d",msdModel[mass_index].getMassIndex(type, j));
			printf(" ]\n");
			printf("spring_index [");
			for(unsigned int j=0; j<num_spring; j++)
				printf(" %d",msdModel[mass_index].getSpringIndex(type, j));
			printf(" ]\n");
		}
		printf("=============================================================================\n\n");
	}
}
/*
void MSDModelBuilder::print(MSDModel* msdModel)
{
	for(unsigned int i=0; i<total_msdModel; i++)
	{
		unsigned int mass_index = massnode[i].refid;
		unsigned int num_mass = msdModel[mass_index].getMassSize();
		unsigned int num_spring = msdModel[mass_index].getSpringSize();
		printf("============================================================================\n");
		printf("msdModel %d has mass_index = %d, spring_index = %d\n",mass_index, num_mass, num_spring);
		printf("mass_index");
		for(unsigned int j=0; j<num_mass; j++)
			printf(" %d",msdModel[mass_index].getMassIndex(j));
		printf("\n");
		printf("spring_index");
		for(unsigned int j=0; j<num_spring; j++)
			printf(" %d",msdModel[mass_index].getSpringIndex(j));
		printf("\n");
	}
}
*/