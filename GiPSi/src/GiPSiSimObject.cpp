/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Simulation Object API Implementation (GiPSiSimObject.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriya Natsupakpong
*/

////	GiPSiSimObject.cpp v0.1.0
////
////    Implementation for the part of Core GiPSi API
////			for the simulation objects,
////			base class for connectors, and
////			base class for integrators
////	
////
////////////////////////////////////////////////////////////////

#include "GiPSiAPI.h"

////////////////////////////////////////////////////////////////
//
//	SolidBoundary:::GetPosition
//
//		Get Positions of the nodes of the boundary
//
Vector<Real>	SolidBoundary::GetPosition(unsigned int index)
{
	return	Object->GetNodePosition(global_id[index]);
}

void			SolidBoundary::GetPosition(Vector<Real> *Bpos)
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bpos[index]=Object->GetNodePosition(global_id[index]);
	}
}

////////////////////////////////////////////////////////////////
//
//	SolidBoundary::GetVelocity
//
//		Get Velocities of the nodes of the boundary
//
Vector<Real>	SolidBoundary::GetVelocity(unsigned int index)
{
	return	Object->GetNodeVelocity(global_id[index]);
}

void			SolidBoundary::GetVelocity(Vector<Real> *Bvel)
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bvel[index]=Object->GetNodeVelocity(global_id[index]);
	}
}

////////////////////////////////////////////////////////////////
//
//	SolidBoundary::ResidualForce
//
//		Get the Reaction force that the body applies to the external 
//         world as a result of the constraints
//		   (i.e. contact force, constraint force, lagrange multiplier)
//
Vector<Real>	SolidBoundary::GetReactionForce(unsigned int index)
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

void			SolidBoundary::GetReactionForce(Vector<Real> *Bforce)
{
	for (unsigned int index=0; index < this->num_vertex; index++) {
		Bforce[index]=GetReactionForce(global_id[index]);
	}
}

////////////////////////////////////////////////////////////////
//
//	SolidBoundary::Set
//
//		Sets Deformable solid Boundary
//
void			SolidBoundary::Set(unsigned int index, unsigned int boundary_type, 
							Vector<Real> boundary_value, 
							Real boundary_value2_scalar, Vector<Real> boundary_value2_vector)
{
	this->boundary_type[index]			=boundary_type;
	this->boundary_value[index]			=boundary_value;
	this->boundary_value2_scalar[index]	=boundary_value2_scalar;
	this->boundary_value2_vector[index]	=boundary_value2_vector;	
}

void			SolidBoundary::Set(unsigned int *boundary_type, 
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

int				SolidBoundary::ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model)
{
	return 0;
}

void			SolidBoundary::ResetBoundaryCondition(void)
{
	SolidBoundary*	bound = this;
	for(int i=0;i<num_vertex;i++) 
	{
		bound->Set(i,0,zero_vector3,0.0,zero_vector3);
	}
	Object->ResetInitialBoundaryCondition();
}

bool			SolidBoundary::isTypeOneBoundary(unsigned int index)
{
	if (boundary_type[index] == 1)
		return true;
	else
		return false;
}

////////////////////////////////////////////////////////////////
//
//	SolidDomain::GetStress
//
//		Gets Stress
//
Matrix<Real>	SolidDomain::GetStress(unsigned int element_index)
{
	return zero_Matrix;
}

void			SolidDomain::GetStress(Matrix<Real> *StressArray)
{
}

////////////////////////////////////////////////////////////////
//
//	SolidDomain::GetStrain
//
//		Gets Strain
//
Matrix<Real>	SolidDomain::GetStrain(unsigned int element_index)
{
	return zero_Matrix;
}

void			SolidDomain::GetStrain(Matrix<Real> *StrainArray)
{
}

////////////////////////////////////////////////////////////////
//
//	SolidDomain::SetDomStress
//
//		Sets External Stresses
//
void			SolidDomain::SetDomainStress(unsigned int element_index, Matrix<Real> Stress_Tensor)
{
	DomStress[element_index] = Stress_Tensor;
}

void			SolidDomain::SetDomainStress(Matrix<Real> *Stress_Tensor_Array)
{
	for (unsigned int index=0; index < this->num_tet; index++){
		DomStress[index] = Stress_Tensor_Array[index];
	}
}

////////////////////////////////////////////////////////////////
//
//	SolidDomain::SetForce
//
//		Sets External Stresses
//
void			SolidDomain::SetForce(unsigned int node_index, Vector<Real> Force_Vector)
{
}

void			SolidDomain::SetForce(Vector<Real> *Force_Vector)
{
}

////////////////////////////////////////////////////////////////
//
//	SolidDomain::ResetDomainVariables
//
//		Resets the domain variables
//
void			SolidDomain::ResetDomainVariables(void)
{
}

void ToTransformationMatrix(Matrix<Real> &result, const Matrix<Real> &rotation, const Vector<Real> &translation, const Vector<Real> &scale)
{
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			result[i][j] = rotation[i][j];
	for(int i=0; i<3; i++)
		result[i][3] = scale[i]*translation[i];
	result[3][3] = 1.0;
}

void GetRotationMatrix(Matrix<Real> &result, const Matrix<Real> &a)
{
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			result[i][j] = a[i][j];
}

void GetTranslationVector(Vector<Real> &result, const Matrix<Real> &a)
{
	for(int i=0; i<3; i++)
		result[i] = a[i][3];
}

void ToRotationMatrix(Matrix<Real> &R, const Real &angle, const Real &ax, const Real &ay, const Real &az)
{	
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
}

void CollisionRule::initialize(int numBoundary)
{
	numCEBoundary = numBoundary;
	length = numCEBoundary*(numCEBoundary+1)/2;
	if (rules!=NULL) delete rules;
	rules = new int[length];
	for(int i=0;i<length;i++) rules[i] = 0;
}
void CollisionRule::initialize(int numBoundary, int numHBoundary)
{
	numCEBoundary = numBoundary;
	numHIOBoundary = numHBoundary;
	length = numCEBoundary*numHIOBoundary;
	if (rules!=NULL) delete rules;
	rules = new int[length];
	for(int i=0;i<length;i++) rules[i] = 0;
}

void CollisionRule::setRule(const char* rulename, const char* filename, SIMObject *object[], int num_object)
{
	FILE			*fp;
	unsigned int	i, j;
	unsigned int	id1, id2;
	char			errmsg[1024];	
	char			buf1[128], buf2[128];
	char			line_type;
	
	/*--- FOR_ALL ---*/
	if (strcmp(rulename, "FOR_ALL") == 0)
	{
		// set all rules to 1 (collision)
		for(i=0; i<length; i++)	rules[i] = 1; 
	}

	/*--- FOR_NONE ---*/
	else if (strcmp(rulename, "FOR_NONE") == 0)
	{
		// set all rules to 1 (no collision)
		for(i=0; i<length; i++)	rules[i] = 0; 
	}

	/*--- FOR_ALL_EXCEPT_OBJ ---*/
	else if (strcmp(rulename, "FOR_ALL_EXCEPT_OBJ") == 0)
	{
		// set all rules to 1 (collision)
		for(i=0; i<length; i++)	rules[i] = 1;  
		
		fp = fopen(filename, "r");
		if (fp == NULL) {
			sprintf(errmsg, "Cannot open collision rule file %s\n", filename);
			error_exit(-1, errmsg);
		}

		//***********************************//
		//*		Read name of sim object		*//
		//***********************************//
		while (fscanf(fp, "%s\n", buf1) != EOF)
		{
			// find the object id
			for(j=0; j<num_object; j++)
			{
				if (strcmp(buf1, object[j]->GetName()) == 0){
					id1 = object[j]->GetID();
					break;
				}
			}

			for(id2=1; id2<=numCEBoundary; id2++)
			{
				if (isMax(id2, id1)) setCollisionTest(0, id1, id2);
				else setCollisionTest(0, id2, id1);
			}
		}
		fclose(fp);
	}

	/*--- FOR_ALL_EXCEPT_PAIR ---*/
	else if (strcmp(rulename, "FOR_ALL_EXCEPT_PAIR") == 0)
	{
		// set all rules to 1 (collision)
		for(i=0; i<length; i++)	rules[i] = 1;  
		
		fp = fopen(filename, "r");
		if (fp == NULL) {
			sprintf(errmsg, "Cannot open collision rule file %s\n", filename);
			error_exit(-1, errmsg);
		}

		//***********************************//
		//*		Read name of sim object		*//
		//***********************************//
		while (fscanf(fp, "%c%s%s\n", &line_type, buf1, buf2) != EOF)
		{			
			if(line_type == '#') continue;
			if(line_type != 'o') {
				error_exit(-1, "unknown format!\n");
			}
			
			// find the object id
			for(j=0; j<num_object; j++)
			{
				if (strcmp(buf1, object[j]->GetName()) == 0){
					id1 = object[j]->GetID();
					break;
				}
			}
			for(j=0; j<num_object; j++)
			{
				if (strcmp(buf2, object[j]->GetName()) == 0){
					id2 = object[j]->GetID();
					break;
				}
			}

			if (isMax(id2, id1)) setCollisionTest(0, id1, id2);
				else setCollisionTest(0, id2, id1);			
		}
		fclose(fp);

		printf("no implementation face pair in \"FOR_ALL_EXCEPT_PAIR\"\n");
	}

	/*--- FOR_ONLY_OBJ ---*/
	else if (strcmp(rulename, "FOR_ONLY_OBJ") == 0)
	{
		// set all rules to 0 (no collision)
		for(i=0; i<length; i++)	rules[i] = 0;  
		
		fp = fopen(filename, "r");
		if (fp == NULL) {
			sprintf(errmsg, "Cannot open collision rule file %s\n", filename);
			error_exit(-1, errmsg);
		}
		
		// count line
		int num_line = 0;
		while (fscanf(fp, "%s\n", buf1) != EOF)
		{
			num_line++;
		}
		// allocate memory
		int *index  = new int[num_line];
		// rewind the file at start position
		rewind(fp);
		i=0;
		while (fscanf(fp, "%s", buf1) != EOF)
		{
			// find the object id
			for(j=0; j<num_object; j++)
			{
				if (strcmp(buf1, object[j]->GetName()) == 0){
					id1 = object[j]->GetID();
					break;
				}
			}
			index[i++] = id1;			 
		}
		fclose(fp);

		for(i=0; i<num_line; i++) {
			id1 = index[i];
			for(j=i; j<num_line; j++) {
				id2 = index[j];
				if (isMax(id2, id1)) setCollisionTest(1, id1, id2);
				else setCollisionTest(1, id2, id1);
			}
		}
		delete index;
	}

	/*--- FOR_ALL_EXCEPT_PAIR ---*/
	else if (strcmp(rulename, "FOR_ONLY_PAIR") == 0)
	{
		// set all rules to 1 (no collision)
		for(i=0; i<length; i++)	rules[i] = 0;  
		
		fp = fopen(filename, "r");
		if (fp == NULL) {
			sprintf(errmsg, "Cannot open collision rule file %s\n", filename);
			error_exit(-1, errmsg);
		}

		//***********************************//
		//*		Read name of sim object		*//
		//***********************************//
		while (fscanf(fp, "%c%s%s\n", &line_type, buf1, buf2) != EOF)
		{			
			if(line_type == '#') continue;
			if(line_type != 'o') {
				error_exit(-1, "unknown format!\n");
			}
			
			// find the object id
			for(j=0; j<num_object; j++)
			{
				if (strcmp(buf1, object[j]->GetName()) == 0){
					id1 = object[j]->GetID();
					break;
				}
			}
			for(j=0; j<num_object; j++)
			{
				if (strcmp(buf2, object[j]->GetName()) == 0){
					id2 = object[j]->GetID();
					break;
				}
			}

			if (isMax(id2, id1)) setCollisionTest(1, id1, id2);
				else setCollisionTest(1, id2, id1);			
		}
		fclose(fp);
		
		printf("no implementation face pair in \"FOR_ONLY_PAIR\"\n");
	}

#ifdef _DEBUG
	// print the collision rule
	printf("================================================\n");
	printf("Collision Enabled Boundary ID and Collision Rule\n");
	Boundary *bound = NULL;
	for (int j = 0; j < num_object; j++)
	{	
		bound = object[j]->GetBoundaryPtr();
		if (bound!=NULL && object[j]->isCDEnable() && bound->isCollisionEnabledBoundaryType())
			printf("ID: %3d, Name: %s\n",object[j]->GetID(), object[j]->GetName());	
	}	
	if(!type) {
		for(i=0; i<numCEBoundary; i++)
			for(j=i; j<numCEBoundary; j++)
				printf("[%d, %d]: %d\n", i+1, j+1, isCollisionTest(i+1, j+1));
	}
	else {
		for(i=0; i<numHIOBoundary; i++)
			for(j=0; j<numCEBoundary; j++)
				printf("[%d, %d]: %d\n", i+1, j+1, isCollisionTest(i+1, j+1));
	}
#endif
}

void CollisionRule::setCollisionTest(int value, int id1, int id2)
{
	int index;
	if(!type)	// normal collision
		index = id1*numCEBoundary - (id1*(id1+1))/2 + id1 - (numCEBoundary-id2);
	else		// haptic collision
		index = (id1-1)*numCEBoundary + id2;
	rules[index-1] = value;
}

void CollisionRule::setCollisionTest(int value, int id1, int face1, int id2, int face2)
{
	int index;
	if(!type)	// normal collision
		index = id1*numCEBoundary - (id1*(id1+1))/2 + id1 - (numCEBoundary-id2);
	else		// haptic collision
		index = (id1-1)*numCEBoundary + id2;
	rules[index-1] = value;
}

int CollisionRule::isCollisionTest(int id1, int id2)
{
	int index;
	if(!type)	// normal collision
		index = id1*numCEBoundary - (id1*(id1+1))/2 + id1 - (numCEBoundary-id2);
	else		// haptic collision
		index = (id1-1)*numCEBoundary + id2;
	return rules[index-1];
}

int CollisionRule::isCollisionTest(int id1, int face1, int id2, int face2)
{
	int index;
	if(!type)	// normal collision
		index = id1*numCEBoundary - (id1*(id1+1))/2 + id1 - (numCEBoundary-id2);
	else		// haptic collision
		index = (id1-1)*numCEBoundary + id2;
	return rules[index-1];
}