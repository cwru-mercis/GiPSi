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

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	CONNECTOR.CPP v0.0
////
////	Connector Class for Linear FEM Module
////
////////////////////////////////////////////////////////////////

#include "connector.h"

// NOTE: Might want to find a better place for this
static	const Vector<Real>	zero_Vector(3, 0.0);
static	const Matrix<Real>	zero_Matrix(3, 3, 0.0);

////////////////////////////////////////////////////////////////
//
//	FEM3LM_BIOE_Connector::FEM3LM_BIOE_Connector
//
//		Constructor
//
FEM3LM_BIOE_Connector::FEM3LM_BIOE_Connector(	FEMDomain			*MechanicalModel,
												CardiacBioEDomain	*ElectricalModel,
												Real				ExStressValueXX, 
												Real				ExStressValueYY, 
												Real				ExStressValueZZ		)
{
	
	bioe	=	ElectricalModel;
	mech	=	MechanicalModel;

	ExcitedStressValueXX = ExStressValueXX;
	ExcitedStressValueYY = ExStressValueYY;
	ExcitedStressValueZZ = ExStressValueZZ;
	
}

////////////////////////////////////////////////////////////////
//
//	FEM3LM_BIOE_Connector::process
//
//		Exchange domain information between models
//
void FEM3LM_BIOE_Connector::process(void)
{

	for (unsigned int i=0; i<bioe->num_tet; i++) {
		mech->SetDomainStress(i,Excitation_to_Contraction(bioe->GetExcitation(i)));
	}

}

////////////////////////////////////////////////////////////////
//
//	FEM3LM_BIOE_Connector::Excitation_to_Contraction
//
//		Excitation to Contraction Coupling Model
//
Matrix<Real>	FEM3LM_BIOE_Connector::Excitation_to_Contraction (Real excitation) {

	Matrix<Real>	Stress=zero_Matrix;

	if (excitation>0.0) { 
		Stress[0][0]+=ExcitedStressValueXX;
		Stress[1][1]+=ExcitedStressValueYY;
		Stress[2][2]+=ExcitedStressValueZZ;
	}
	
	return  Stress;

}

void			FEM3LM_BIOE_Connector::Excitation_to_Contraction (Real *Excitation_Array, Matrix<Real> *Stress_Array) {

	for (unsigned int index=0; index < bioe->num_tet; index++){
		Stress_Array[index]=Excitation_to_Contraction(Excitation_Array[index]);
	}

}


////////////////////////////////////////////////////////////////
//
//	FEM3LM_LUMPEDFLUID_Connector::FEM3LM_BIOE_Connector
//
//		Constructor
//
FEM3LM_LUMPEDFLUID_Connector::FEM3LM_LUMPEDFLUID_Connector(		FEMBoundary				*MechanicalModel,
																LumpedFluidBoundary		*LFluidModel,
																char *filename		)
{
	unsigned int	i;

	lfluid	=	LFluidModel;
	mech	=	MechanicalModel;
	Load(filename);
	
	mechpos	=	new	Vector<Real>[num_vertex];
	forceaccum=	new Vector<Real>[mech->num_vertex];

	for	(i=0; i<num_vertex; i++)	mechpos[i]=mech->GetPosition(*(vcorr+2*i+0));
	mechtime=	mech->Object->GetTime();
	for (i=0; i<mech->num_vertex; i++)	forceaccum[i]=zero_Vector; 

	for (i=0; i<num_vertex; i++)	lfluid->Set(*(vcorr+2*i+1),0, zero_Vector);
}

////////////////////////////////////////////////////////////////
//
//	FEM3LM_LUMPEDFLUID_Connector::process
//
//		Exchange domain information between models
//
void FEM3LM_LUMPEDFLUID_Connector::process(void)
{
	unsigned int	i,j,femidx,fluididx,femnodes[3],fluidnodes[3];
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


////////////////////////////////////////////////////////////////
//
//	FEM3LM_LUMPEDFLUID_Connector::Load
//
//		Loader for correspondence file
//
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


