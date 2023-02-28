/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Connector Class Implementations (connector.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriy Natsupakpong.
*/

////	CONNECTOR.CPP v0.0
////
////	Connector Class for Linear FEM Module
////
////////////////////////////////////////////////////////////////

#include "connector.h"
#include "GiPSiException.h"
#include "XMLNodeList.h"

/**
 * Constructor.
 * 
 * @param connectorNode XML project file 'connector' node.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
FEM3LM_BIOE_Connector::FEM3LM_BIOE_Connector(XMLNode * connectorNode,
											 SIMObject * objects[MAX_SIMOBJ],
											 int num_objects)
											 :	mech(NULL),
												bioe(NULL)
{
	try
	{
		XMLNodeList * connectorChildren = connectorNode->GetChildren();
		// Set connector name
		XMLNode * nameNode = connectorChildren->GetNode("name");
		char * name = nameNode->GetValue();
		SetName(name);

		// Extract object1 name
		XMLNode * object1NameNode = connectorChildren->GetNode("object1Name");
		const char * object1Name = object1NameNode->GetValue();
		// Set domain1
		mech = dynamic_cast<FEMDomain *>(GetDomain(object1Name, objects, num_objects));
		if (!mech)
		{
			char error[256]("");
			sprintf_s(error, 256, "Object reference by object1Name \"%s\" is of an incorrect type.", object1Name);
			throw new GiPSiException("FEM3LM_BIOE_Connector constructor", error);
			return;
		}

		// Extract object2 name
		XMLNode * object2NameNode = connectorChildren->GetNode("object2Name");
		const char * object2Name = object2NameNode->GetValue();
		// Set domain2
		bioe = dynamic_cast<CardiacBioEDomain *>(GetDomain(object2Name, objects, num_objects));
		if (!bioe)
		{
			char error[256]("");
			sprintf_s(error, 256, "Object reference by object2Name \"%s\" is of an incorrect type.", object1Name);
			throw new GiPSiException("FEM3LM_BIOE_Connector constructor", error);
			return;
		}

		// Extract model parameters
		XMLNode * modelParametersNode = connectorChildren->GetNode("modelParameters");
		XMLNodeList * modelParametersChildren = modelParametersNode->GetChildren();
		LoadGeometry(modelParametersChildren);
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Get the domain of the specified object, if it exists.
 * 
 * @param objectName Character string containing the object name.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
Domain * FEM3LM_BIOE_Connector::GetDomain(const char * objectName, SIMObject * objects[64], int num_objects)
{
	for (int i = 0; i < num_objects; i++)
	{
		if (strcmp(objects[i]->GetName(), objectName) == 0)
			return objects[i]->GetDomainPtr();
	}
	char error[256]("");
	sprintf_s(error, 256, "Object with name \"%s\" does not exist.", objectName);
	throw new GiPSiException("FEM3LM_LUMPEDFLUID_Connector GetDomain", error);
	return NULL;
}


/**
 * Load model parameters for FEM3LM_BIOE_Connector.
 * 
 * @param objectName Character string containing the object name.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
void FEM3LM_BIOE_Connector::LoadGeometry(XMLNodeList * modelParametersChildren)
{
	try
	{
		XMLNode * FEM_CBEParametersNode = modelParametersChildren->GetNode("FEM_CBEParameters");
		XMLNodeList * FEM_CBEParametersChildren = FEM_CBEParametersNode->GetChildren();
		XMLNode * ExStressValueXXNode = FEM_CBEParametersChildren->GetNode("ExStressValueXX");
		XMLNode * ExStressValueYYNode = FEM_CBEParametersChildren->GetNode("ExStressValueYY");
		XMLNode * ExStressValueZZNode = FEM_CBEParametersChildren->GetNode("ExStressValueZZ");

		// Set model parameters
		ExcitedStressValueXX = (Real)atof(ExStressValueXXNode->GetValue());
		ExcitedStressValueYY = (Real)atof(ExStressValueYYNode->GetValue());
		ExcitedStressValueZZ = (Real)atof(ExStressValueZZNode->GetValue());
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Exchange domain information between models.
 */
void FEM3LM_BIOE_Connector::process(void)
{

	for (unsigned int i=0; i<bioe->num_tet; i++) {
		mech->SetDomainStress(i,Excitation_to_Contraction(bioe->GetExcitation(i)));
	}

}


/**
 * Excitation to Contraction Coupling Model.
 * 
 * @param excitation Placeholder text.
 */
Matrix<Real>	FEM3LM_BIOE_Connector::Excitation_to_Contraction (Real excitation) {

	Matrix<Real>	Stress=zero_Matrix;

	if (excitation>0.0) { 
		Stress[0][0]+=ExcitedStressValueXX;
		Stress[1][1]+=ExcitedStressValueYY;
		Stress[2][2]+=ExcitedStressValueZZ;
	}
	
	return  Stress;

}

/**
 * Excitation to Contraction Coupling Model.
 * 
 * @param Excitation_Array Placeholder text.
 * @param Stress_Array Placeholder text.
 */
void			FEM3LM_BIOE_Connector::Excitation_to_Contraction (Real *Excitation_Array, Matrix<Real> *Stress_Array) {

	for (unsigned int index=0; index < bioe->num_tet; index++){
		Stress_Array[index]=Excitation_to_Contraction(Excitation_Array[index]);
	}

}


/**
 * Constructor.
 * 
 * @param connectorNode XML project file 'connector' node.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
FEM3LM_LUMPEDFLUID_Connector::FEM3LM_LUMPEDFLUID_Connector(XMLNode * connectorNode,
														   SIMObject * objects[MAX_SIMOBJ],
														   int num_objects)
														   :	mech(NULL),
																lfluid(NULL)
{
	try
	{
		XMLNodeList * connectorChildren = connectorNode->GetChildren();
		// Set connector name
		XMLNode * nameNode = connectorChildren->GetNode("name");
		char * name = nameNode->GetValue();
		SetName(name);

		// Extract object1 name
		XMLNode * object1NameNode = connectorChildren->GetNode("object1Name");
		const char * object1Name = object1NameNode->GetValue();
		// Set boundary1
		mech = dynamic_cast<FEMBoundary *>(GetBoundary(object1Name, objects, num_objects));
		if (!mech)
		{
			char error[256]("");
			sprintf_s(error, 256, "Object reference by object1Name \"%s\" is of an incorrect type.", object1Name);
			throw new GiPSiException("FEM3LM_LUMPEDFLUID_Connector constructor", error);
			return;
		}

		// Extract object2 name
		XMLNode * object2NameNode = connectorChildren->GetNode("object2Name");
		const char * object2Name = object2NameNode->GetValue();
		// Set boundary2
		lfluid = dynamic_cast<LumpedFluidBoundary *>(GetBoundary(object2Name, objects, num_objects));
		if (!lfluid)
		{
			char error[256]("");
			sprintf_s(error, 256, "Object reference by object2Name \"%s\" is of an incorrect type.", object1Name);
			throw new GiPSiException("FEM3LM_LUMPEDFLUID_Connector constructor", error);
			return;
		}

		// Extract file name and path
		XMLNode * modelParametersNode = connectorChildren->GetNode("modelParameters");
		XMLNodeList * modelParametersChildren = modelParametersNode->GetChildren();
		LoadGeometry(modelParametersChildren);
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Get the boundary of the specified object, if it exists.
 * 
 * @param objectName Character string containing the object name.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
Boundary * FEM3LM_LUMPEDFLUID_Connector::GetBoundary(const char * objectName, SIMObject * objects[MAX_SIMOBJ], int num_objects)
{
	for (int i = 0; i < num_objects; i++)
	{
		if (strcmp(objects[i]->GetName(), objectName) == 0)
			return objects[i]->GetBoundaryPtr();
	}
	char error[256]("");
	sprintf_s(error, 256, "Object with name \"%s\" does not exist.", objectName);
	throw new GiPSiException("FEM3LM_LUMPEDFLUID_Connector GetBoundary", error);
	return NULL;
}


/**
 * Load model parameters for FEM3LM_LUMPEDFLUID_Connector.
 * 
 * @param objectName Character string containing the object name.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
void FEM3LM_LUMPEDFLUID_Connector::LoadGeometry(XMLNodeList * modelParametersChildren)
{
	try
	{
		XMLNode * FEM_LFParametersNode = modelParametersChildren->GetNode("FEM_LFParameters");
		XMLNodeList * FEM_LFParametersChildren = FEM_LFParametersNode->GetChildren();
		XMLNode * corrFileNode = FEM_LFParametersChildren->GetNode("corrFile");
		XMLNodeList * corrFileChildren = corrFileNode->GetChildren();
		XMLNode * pathNode = corrFileChildren->GetNode("path");
		XMLNode * fileNameNode = corrFileChildren->GetNode("fileName");
		char fullFilePath[256]("");
		sprintf_s(fullFilePath, 256, "%s\\%s", pathNode->GetValue(), fileNameNode->GetValue());
		// Load file
		Load(fullFilePath);

		// Other setup stuff
		mechpos	=	new	Vector<Real>[num_vertex];
		forceaccum=	new Vector<Real>[mech->num_vertex];

		for	(unsigned int i=0; i<num_vertex; i++)
			mechpos[i]=mech->GetPosition(*(vcorr+2*i+0));
		mechtime = mech->Object->GetTime();
		for (unsigned int i=0; i<mech->num_vertex; i++)
			forceaccum[i]=zero_Vector; 

		for (unsigned int i=0; i<num_vertex; i++)
			lfluid->Set(*(vcorr+2*i+1),0, zero_Vector);
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Exchange boundary information between models.
 */
void FEM3LM_LUMPEDFLUID_Connector::process(void)
{
	unsigned int	i,j,femidx,fluididx,femnodes[3]/*,fluidnodes[3]*/;
	Real			sum=0;
	Vector<Real>	v(3,0.0),oldpos(3,0.0);
	Real			oldtime;
	Real			pressure;
	Real			aread3;

	oldtime=mechtime;
	mechtime=mech->Object->GetTime();

	for (i=0; i<num_vertex; i++)
	{
		femidx		=	*(vcorr+2*i+0);
		fluididx	=	*(vcorr+2*i+1);

		// This coupling does not quite work as the two boundaries start to disassociate
		//  when we use a multistep integrator for the mechanical model.
		//  This is because when we use a multistep integrator for the mechanical model,
		//  the nodal velocity varies within the time step, whereas the fluid model uses
		//  a constant velocity value (the value at the beginning of the time step).
		//lfluid->Set(fluididx,0,mech->GetVelocity(femidx));

		// This is the alternative approach
		//	note that the position of fluid boundary follows the solid boundary one step behind
		//  using this method.
		if (mechtime>oldtime)
		{
			oldpos=mechpos[i];
			mechpos[i]=mech->GetPosition(femidx);
			lfluid->Set(fluididx,0,(mechpos[i]-oldpos)/(mechtime-oldtime));
		}
	
		// To test if the two boundaries disassociate or not calculate the distance between them
		//v=(lfluid->vertex[fluididx].pos-mech->vertex[femidx].pos);
		//v=(lfluid->vertex[fluididx].pos-oldpos);
		//sum+=v.length();

		// initially assign zero forces on all the fem nodes
		forceaccum[femidx]=zero_Vector;
	}
	//printf("Distance between the two boundaries=\t%f\n",sum);

	//  As pressure is constant for the lumped fluid model we don't need to get pressures for all faces
	//lfluid->GetPressure(PressureArray);
	//  Evaluation at a single node will do.
	pressure=lfluid->GetPressure((unsigned int) 0);

	for (i=0; i<num_face; i++)
	{
		femidx		=	*(fcorr+2*i+0);
		fluididx	=	*(fcorr+2*i+1);

		aread3=TriangleArea(mech->face[femidx].vertex[0]->pos,
							mech->face[femidx].vertex[1]->pos,
							mech->face[femidx].vertex[2]->pos	);
		aread3=fabs(aread3/3.0);

		//  Find the indices of the nodes which belong to this face
		femnodes[0]		=   *(mech->facetovertexids+3*femidx+0);
		femnodes[1]		=   *(mech->facetovertexids+3*femidx+1);
		femnodes[2]		=   *(mech->facetovertexids+3*femidx+2);
		//  These will not be needed as pressure is constant for the lumped fluid model
		/*
		fluidnodes[0]	=   *(lfluif->facetovertexids+3*fluididx+0);
		fluidnodes[1]	=   *(lfluif->facetovertexids+3*fluididx+1);
		fluidnodes[2]	=   *(lfluif->facetovertexids+3*fluididx+2);
		*/
		// apply the forces to the nodes that belong to this face
		for (j=0; j<3; j++) forceaccum[femnodes[j]]+= -pressure*aread3*mech->vertex[femnodes[j]].n;
	}
	
	// apply the boundary conditions
	for (i=0; i<num_vertex; i++)	
	{
		femidx		=	*(vcorr+2*i+0);
		fluididx	=	*(vcorr+2*i+1);
		mech->Set(femidx,0,forceaccum[femidx],0.0,zero_Vector);
	}
}


/**
 * Loader for correspondence file.
 * 
 * @param filename Character string containing file name.
 */
void FEM3LM_LUMPEDFLUID_Connector::Load(char *filename)
{
	FILE			*fp;
	unsigned int	i;
	char			errmsg[1024], 
					file_type[64],
					line_type;
	int				offset1,offset2;
	int				node_offset;
	int				obj_offset;
	unsigned int	v1, v2;
  
	num_vertex	= 0;
	num_face	= 0;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		sprintf(errmsg, "Cannot open corr file %s\n", filename);
		error_exit(-1, errmsg);
	}

	//***************************************//
	//*		Read CORR header information	*//
	//***************************************//
	fscanf(fp, "%s%d%d%d%d\n", file_type, &(num_vertex), &(num_face), &offset1, &offset2);
	printf("File type : <%s>\tVertex Corr: %d\tFace Corr: %d\tOffset1: %d\tOffset2: %d\n",
							file_type, num_vertex, num_face, offset1,offset2);

	if(!strcmp("NODETOOBJ", file_type)) {
		node_offset=offset1;
		obj_offset=offset2;

		/****************************************/
		/*	Allocate continuous memory			*/
		/****************************************/
		vcorr = (unsigned int *) malloc(2*num_vertex*sizeof(int));
		if(vcorr == NULL) {
			error_exit(-1, "Cannot allocate memory for vertices!\n");
		}
		fcorr = (unsigned int *) malloc(2*num_face*sizeof(int));
		if(fcorr == NULL) {
			error_exit(-1, "Cannot allocate memory for face!\n");
		}

		/****************************************/
		/*	Read vertices						*/
		/****************************************/
		i = 0;
		while (i < num_vertex) {
			fscanf(fp, "%c%d%d\n", &line_type, &v1, &v2);

			if(line_type == '#') continue;
			if(line_type != 'n') {
				error_exit(-1, "Wrong number of nodes!\n");
			}

			v1 -= node_offset;
			v2 -= obj_offset;

			*(vcorr+2*i)= v1;
			*(vcorr+2*i+1)= v2;

			i++;
		}

		//  We will need to read the .node and .obj files to make sense out of the face numbers because:
		//    the face numbers in the .corr file refer to the face indices in the .node and .obj file
		//    while the indices in the boundary class refer to indices generated 
		unsigned int mechfmax=0;  
		for (i=0; i<mech->num_face; i++) 
			mechfmax=((mechfmax>=mech->face[i].refid)?(mechfmax):(mech->face[i].refid));
		unsigned int lfluidfmax=0;  
		for (i=0; i<lfluid->num_face; i++) 
			lfluidfmax=((lfluidfmax>=lfluid->face[i].refid)?(lfluidfmax):(lfluid->face[i].refid));
		unsigned int	*mechflist		=	new unsigned int[mechfmax+1];
		unsigned int	*lfluidflist	=	new unsigned int[lfluidfmax+1];

		for (i=0; i<mech->num_face; i++)	mechflist[mech->face[i].refid]=i;
		for (i=0; i<lfluid->num_face; i++)	lfluidflist[lfluid->face[i].refid]=i;

  		/****************************************/
		/*	Read faces							*/
		/****************************************/
		i = 0;
		while (i < num_face) {
			fscanf(fp, "%c%d%d\n", &line_type, &v1, &v2);

			if(line_type == '#') continue;
			if(line_type != 'f') {
				error_exit(-1, "Wrong number of faces!\n");
			}

			v1 -= node_offset;	if (v1>mechfmax)	printf("Face numbers don't match between .node and .corr files.\n");
			v2 -= obj_offset;	if (v2>lfluidfmax)	printf("Face numbers don't match between .obj and .corr files.\n");

			*(fcorr+2*i)	= mechflist[v1];	
			*(fcorr+2*i+1)	= lfluidflist[v2];	

			i++;
		}
	}
	else if(!strcmp("OBJTOOBJ", file_type)) {

		/****************************************/
		/*	Allocate continuous memory			*/
		/****************************************/
		vcorr = (unsigned int *) malloc(2*num_vertex*sizeof(int));
		if(vcorr == NULL) {
			error_exit(-1, "Cannot allocate memory for vertices!\n");
		}
		fcorr = (unsigned int *) malloc(2*num_face*sizeof(int));
		if(fcorr == NULL) {
			error_exit(-1, "Cannot allocate memory for face!\n");
		}
	  
		/****************************************/
		/*	Read vertices						*/
		/****************************************/
		i = 0;
		while (i < num_vertex) {
			fscanf(fp, "%c%d%d\n", &line_type, &v1, &v2);

			if(line_type == '#') continue;
			if(line_type != 'n') {
				error_exit(-1, "Wrong number of nodes!\n");
			}

			v1 -= offset1;
			v2 -= offset2;

			*(vcorr+2*i)= v1;
			*(vcorr+2*i+1)= v2;

			i++;
		}

  		/****************************************/
		/*	Read faces							*/
		/****************************************/
		i = 0;
		while (i < num_face) {
			fscanf(fp, "%c%d%d\n", &line_type, &v1, &v2);

			if(line_type == '#') continue;
			if(line_type != 'f') {
				error_exit(-1, "Wrong number of faces!\n");
			}

			v1 -= offset1;	
			v2 -= offset2;	

			*(fcorr+2*i)	= v1;	
			*(fcorr+2*i+1)	= v2;	

			i++;
		}
	}
	else {
		sprintf(errmsg, "Invalid file type %s\n", filename);
		error_exit(-1, errmsg);
	}


	fclose(fp);

}


/**
 * Constructor.
 * 
 * @param connectorNode XML project file 'connector' node.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
QSDS_MSD_Connector::QSDS_MSD_Connector(XMLNode * connectorNode,
											SIMObject * objects[MAX_SIMOBJ],
											int num_objects)
											:	qsds(NULL),
												msd(NULL)
{
	try
	{
		XMLNodeList * connectorChildren = connectorNode->GetChildren();
		// Set connector name
		XMLNode * nameNode = connectorChildren->GetNode("name");
		char * name = nameNode->GetValue();
		SetName(name);

		// Extract object1 name
		XMLNode * object1NameNode = connectorChildren->GetNode("object1Name");
		const char * object1Name = object1NameNode->GetValue();
		// Set boundary1
		qsds = dynamic_cast<QSDSBoundary *>(GetBoundary(object1Name, objects, num_objects));
		if (!qsds)
		{
			char error[256]("");
			sprintf_s(error, 256, "Object reference by object1Name \"%s\" is of an incorrect type.", object1Name);
			throw new GiPSiException("QSDS_MSD_Connector constructor", error);
			return;
		}

		// Extract object2 name
		XMLNode * object2NameNode = connectorChildren->GetNode("object2Name");
		const char * object2Name = object2NameNode->GetValue();
		// Set boundary2
		msd = dynamic_cast<MSDBoundary *>(GetBoundary(object2Name, objects, num_objects));
		if (!msd)
		{
			char error[256]("");
			sprintf_s(error, 256, "Object reference by object2Name \"%s\" is of an incorrect type.", object1Name);
			throw new GiPSiException("QSDS_MSD_Connector constructor", error);
			return;
		}

		// Extract file name and path
		XMLNode * modelParametersNode = connectorChildren->GetNode("modelParameters");
		XMLNodeList * modelParametersChildren = modelParametersNode->GetChildren();
		LoadGeometry(modelParametersChildren);
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Get the boundary of the specified object, if it exists.
 * 
 * @param objectName Character string containing the object name.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
Boundary * QSDS_MSD_Connector::GetBoundary(const char * objectName, SIMObject * objects[MAX_SIMOBJ], int num_objects)
{
	for (int i = 0; i < num_objects; i++)
	{
		if (strcmp(objects[i]->GetName(), objectName) == 0)
			return objects[i]->GetBoundaryPtr();
	}
	char error[256]("");
	sprintf_s(error, 256, "Object with name \"%s\" does not exist.", objectName);
	throw new GiPSiException("QSDS_MSD_Connector GetBoundary", error);
	return NULL;
}


/**
 * Load model parameters for QSDS_MSD_Connector.
 * 
 * @param modelParametersChildren XMLNodelist. 
 */
void QSDS_MSD_Connector::LoadGeometry(XMLNodeList * modelParametersChildren)
{
	try
	{
		XMLNode * FEM_LFParametersNode = modelParametersChildren->GetNode("QSDS_MSDParameters");
		XMLNodeList * FEM_LFParametersChildren = FEM_LFParametersNode->GetChildren();
		XMLNode * corrFileNode = FEM_LFParametersChildren->GetNode("corrFile");
		XMLNodeList * corrFileChildren = corrFileNode->GetChildren();
		XMLNode * pathNode = corrFileChildren->GetNode("path");
		XMLNode * fileNameNode = corrFileChildren->GetNode("fileName");
		char fullFilePath[256]("");
		sprintf_s(fullFilePath, 256, "%s\\%s", pathNode->GetValue(), fileNameNode->GetValue());
		// Load file
		Load(fullFilePath);

		// Other setup stuff
		msdpos = new Vector<Real>[num_vertex];
		residualforce = new Vector<Real>[num_vertex];

		for	(unsigned int i=0; i<num_vertex; i++)
			msdpos[i] = msd->GetPosition(*(vcorr+2*i+1));
		
		for (unsigned int i=0; i<num_vertex; i++)
			residualforce[i] = zero_vector3; 

		for (unsigned int i=0; i<num_vertex; i++)
			qsds->Set(*(vcorr+2*i+1),0, zero_vector3, 0, zero_vector3);
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Exchange boundary information between models.
 * the order of this connect is MSD-Connector-QSDS
 */
void QSDS_MSD_Connector::process(void)
{
	unsigned int	i,j;
	unsigned int	qsdsidx,msdidx;			
	
	for (i=0; i<num_vertex; i++)
	{
		qsdsidx	= *(vcorr+2*i+0);
		msdidx = *(vcorr+2*i+1);		
		
		// check if qsds node move		
		if (qsds->GetBoundaryType(qsdsidx)==1)
		{
			// From QSDS to MSD	
			msdpos[i] = qsds->GetPosition(qsdsidx);				
			msd->Set(msdidx, 1, msdpos[i], 0.0, zero_vector3);
			// From MSD to QSDS
			// no need force feed back to qsds,
			// if it set the force back to qsds, the qsds display of the connector node will not move
			//residualforce[i] = msd->GetReactionForce(msdidx);				
			//qsds->Set(qsdsidx, 0, residualforce[i], 0.0, zero_vector3);
		}
		else //if (msd->GetBoundaryType(msdidx)==1)
		{
			// From MSD to QSDS
			msdpos[i] = msd->GetPosition(msdidx);
			qsds->Set(qsdsidx, 1, msdpos[i], 0.0, zero_vector3);
			// From QSDS to MSD	
			residualforce[i] = qsds->GetReactionForce(qsdsidx);				
			msd->Set(msdidx, 0, residualforce[i], 0.0, zero_vector3);
		}
	}		
}


/**
 * Loader for correspondence file.
 * 
 * @param filename Character string containing file name.
 */
void QSDS_MSD_Connector::Load(char *filename)
{
	FILE			*fp;
	unsigned int	i;
	char			errmsg[1024], 
					file_type[64],
					line_type;
	int				offset1,offset2;
	unsigned int	v1, v2;
  
	num_vertex	= 0;	

	fp = fopen(filename, "r");
	if (fp == NULL) {
		sprintf(errmsg, "Cannot open corr file %s\n", filename);
		error_exit(-1, errmsg);
	}

	//***************************************//
	//*		Read CORR header information	*//
	//***************************************//
	fscanf(fp, "%s%d%d%d%d\n", file_type, &(num_vertex), &offset1, &offset2);
	printf("File type : <%s>\tVertex Corr: %d\tOffset1: %d\tOffset2: %d\n",
							file_type, num_vertex, offset1,offset2);

	if(!strcmp("OBJTOOBJ", file_type)) {

		/****************************************/
		/*	Allocate continuous memory			*/
		/****************************************/
		vcorr = (unsigned int *) malloc(2*num_vertex*sizeof(int));
		if(vcorr == NULL) {
			error_exit(-1, "Cannot allocate memory for vertices!\n");
		}
	  
		/****************************************/
		/*	Read vertices						*/
		/****************************************/
		i = 0;
		while (i < num_vertex) {
			fscanf(fp, "%c%d%d\n", &line_type, &v1, &v2);

			if(line_type == '#') continue;
			if(line_type != 'n') {
				error_exit(-1, "Wrong number of nodes!\n");
			}

			v1 -= offset1;
			v2 -= offset2;

			*(vcorr+2*i)= v1;
			*(vcorr+2*i+1)= v2;

			i++;
		}  		
	}
	else {
		sprintf(errmsg, "Invalid file type %s\n", filename);
		error_exit(-1, errmsg);
	}

	fclose(fp);
}

/**
 * Constructor.
 * 
 * @param connectorNode XML project file 'connector' node.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
Endo_Catheter_Connector::Endo_Catheter_Connector(XMLNode * connectorNode,
											SIMObject * objects[MAX_SIMOBJ],
											int num_objects)
											:	endo(NULL),
												catheter(NULL)
{
	try
	{
		XMLNodeList * connectorChildren = connectorNode->GetChildren();
		// Set connector name
		XMLNode * nameNode = connectorChildren->GetNode("name");
		char * name = nameNode->GetValue();
		SetName(name);

		// Extract object1 name
		XMLNode * object1NameNode = connectorChildren->GetNode("object1Name");
		const char * object1Name = object1NameNode->GetValue();
		// Set domain1
		endo = dynamic_cast<RigidProbeHIODomain *>(GetDomain(object1Name, objects, num_objects));
		if (!endo)
		{
			char error[256]("");
			sprintf_s(error, 256, "Object reference by object1Name \"%s\" is of an incorrect type.", object1Name);
			throw new GiPSiException("Endo_Catheter_Connector constructor", error);
			return;
		}

		// Extract object2 name
		XMLNode * object2NameNode = connectorChildren->GetNode("object2Name");
		const char * object2Name = object2NameNode->GetValue();
		// Set boundary2
		catheter = dynamic_cast<CatheterHIODomain *>(GetDomain(object2Name, objects, num_objects));
		if (!catheter)
		{
			char error[256]("");
			sprintf_s(error, 256, "Object reference by object2Name \"%s\" is of an incorrect type.", object1Name);
			throw new GiPSiException("Endo_Catheter_Connector constructor", error);
			return;
		}

		CatheterBase_Endo_Transformation = zero_matrix4;	
		// Extract model parameter
		XMLNode * modelParametersNode = connectorChildren->GetNode("modelParameters");
		XMLNodeList * modelParametersChildren = modelParametersNode->GetChildren();
		LoadParameter(modelParametersChildren);
	}
	catch (...)
	{
		throw;
		return;
	}
}

/**
 * Get the domain of the specified object, if it exists.
 * 
 * @param objectName Character string containing the object name.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
Domain * Endo_Catheter_Connector::GetDomain(const char * objectName, SIMObject * objects[64], int num_objects)
{
	for (int i = 0; i < num_objects; i++)
	{
		if (strcmp(objects[i]->GetName(), objectName) == 0)
			return objects[i]->GetDomainPtr();
	}
	char error[256]("");
	sprintf_s(error, 256, "Object with name \"%s\" does not exist.", objectName);
	throw new GiPSiException("Endo_Catheter_Connector GetDomain", error);
	return NULL;
}

/**
 * Load model parameters for Endo_Catheter_Connector.
 * 
 * @param modelParametersChildren XMLNodelist. 
 */
void Endo_Catheter_Connector::LoadParameter(XMLNodeList * modelParametersChildren)
{
	try
	{
		Matrix<Real> rotation = zero_matrix3;
		Vector<Real> translation = zero_vector3;
		Vector<Real> scale(3,1.0);

		XMLNode * transformationNode = modelParametersChildren->GetNode("transformation");
		XMLNodeList * transformationChildren = transformationNode->GetChildren();

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

		// Set orientation component
		ToRotationMatrix(rotation, TORAD(atof(rangle)), atof(rx), atof(ry), atof(rz));

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
		
		// Set translation component
		
		translation[0] = (float)atof(dx);
		translation[1] = (float)atof(dy);
		translation[2] = (float)atof(dz);

		ToTransformationMatrix(CatheterBase_Endo_Transformation, rotation, translation, scale);
		
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
 * Get the endoscope configuration and set the catheter base configuration. 
 */
void Endo_Catheter_Connector::process(void)
{
	Matrix<Real>	gwe = endo->GetConfiguration();
	catheter->SetBaseConfiguration(gwe * CatheterBase_Endo_Transformation);
}

/**
 * Constructor.
 * 
 * @param connectorNode XML project file 'connector' node.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
Catheter_Balloon_Connector::Catheter_Balloon_Connector(XMLNode * connectorNode,
											SIMObject * objects[MAX_SIMOBJ],
											int num_objects)
											:	catheter(NULL),
												balloon(NULL)
{
	try
	{
		XMLNodeList * connectorChildren = connectorNode->GetChildren();
		// Set connector name
		XMLNode * nameNode = connectorChildren->GetNode("name");
		char * name = nameNode->GetValue();
		SetName(name);

		// Extract object1 name
		XMLNode * object1NameNode = connectorChildren->GetNode("object1Name");
		const char * object1Name = object1NameNode->GetValue();
		// Set domain1
		catheter = dynamic_cast<CatheterHIODomain *>(GetDomain(object1Name, objects, num_objects));
		if (!catheter)
		{
			char error[256]("");
			sprintf_s(error, 256, "Object reference by object1Name \"%s\" is of an incorrect type.", object1Name);
			throw new GiPSiException("Catheter_Balloon_Connector constructor", error);
			return;
		}

		// Extract object2 name
		XMLNode * object2NameNode = connectorChildren->GetNode("object2Name");
		const char * object2Name = object2NameNode->GetValue();
		// Set boundary2
		balloon = dynamic_cast<BalloonDomain *>(GetDomain(object2Name, objects, num_objects));
		if (!balloon)
		{
			char error[256]("");
			sprintf_s(error, 256, "Object reference by object2Name \"%s\" is of an incorrect type.", object1Name);
			throw new GiPSiException("Catheter_Balloon_Connector constructor", error);
			return;
		}

		BalloonBase_Catheter_Transformation = zero_matrix4;	
		// Extract file name and path
		XMLNode * modelParametersNode = connectorChildren->GetNode("modelParameters");
		XMLNodeList * modelParametersChildren = modelParametersNode->GetChildren();
		LoadParameter(modelParametersChildren);

	}
	catch (...)
	{
		throw;
		return;
	}
}

/**
 * Get the domain of the specified object, if it exists.
 * 
 * @param objectName Character string containing the object name.
 * @param objects Simulation object array.
 * @param num_objects Size of objects array.
 */
Domain * Catheter_Balloon_Connector::GetDomain(const char * objectName, SIMObject * objects[64], int num_objects)
{
	for (int i = 0; i < num_objects; i++)
	{
		if (strcmp(objects[i]->GetName(), objectName) == 0)
			return objects[i]->GetDomainPtr();
	}
	char error[256]("");
	sprintf_s(error, 256, "Object with name \"%s\" does not exist.", objectName);
	throw new GiPSiException("Catheter_Balloon_Connector GetDomain", error);
	return NULL;
}

/**
 * Load model parameters for Catheter_Balloon_Connector.
 * 
 * @param modelParametersChildren XMLNodelist. 
 */
void Catheter_Balloon_Connector::LoadParameter(XMLNodeList * modelParametersChildren)
{
	try
	{
		Matrix<Real> rotation = zero_matrix3;
		Vector<Real> translation = zero_vector3;
		Vector<Real> scale(3,1.0);

		XMLNode * transformationNode = modelParametersChildren->GetNode("transformation");
		XMLNodeList * transformationChildren = transformationNode->GetChildren();

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

		// Set orientation component
		ToRotationMatrix(rotation, TORAD(atof(rangle)), atof(rx), atof(ry), atof(rz));

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
		
		// Set translation component
		
		translation[0] = (float)atof(dx);
		translation[1] = (float)atof(dy);
		translation[2] = (float)atof(dz);

		ToTransformationMatrix(BalloonBase_Catheter_Transformation, rotation, translation, scale);
		
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
 * Get the catheter configuration and set the balloon base configuration. 
 */
void Catheter_Balloon_Connector::process(void)
{
	Matrix<Real>	gwt = catheter->GetConfiguration();
	balloon->SetBaseConfiguration(gwt * BalloonBase_Catheter_Transformation);
}