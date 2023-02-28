/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Main Simulation I/O Module Implementation (simulator_io.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	Simulator_io.CPP v0.0.9
////
////	Main Simulation I/O module 
////
////////////////////////////////////////////////////////////////

// Standard includes
#include "algebra.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "GiPSiAPI.h"
#include "guiman.h"
#include "simulator.h"
#include "msd.h"
#include "fem.h"
#include "bioe.h"
#include "lumpedfluid.h"
#include "simple.h"
#include "connector.h"
#include "timing.h"

#define		DELIM 10
#define		MAX_BUFFER_SIZE	1024

////////////////////////////////////////////////////////////////
//
//	SimulationKernel::LoadProject
//
//		Loads the project file
//
int	SimulationKernel::LoadProject(char *filename)
{
    FILE			*prjfile;
    char			buffer[MAX_BUFFER_SIZE];
    char			objfile[MAX_BUFFER_SIZE];
	char			*oname;
	float			cr, cg, cb, ca;
    float			dx, dy, dz;
	float			sx, sy, sz;
	float			rangle, rx, ry, rz;
	Geometry		*geom;
	Real			color[4];

	unsigned int	i;
	FEMBoundary		*bound; 

	Real	timestep;
	Real	mass;

	oname = new char[MAX_BUFFER_SIZE];
	
    prjfile = fopen(filename, "r");
   
    if (prjfile == NULL) {
		printf( "ERROR: Cannot load project file <%s>!\n",filename);
		exit(0);
    }
   
    while(!feof(prjfile)) {
		if (fgets(buffer, MAX_BUFFER_SIZE, prjfile) == NULL) break;
		switch(buffer[0]) {
		case 'P':   	// Project Line
			this->name = new char[MAX_BUFFER_SIZE];

			sscanf(buffer, "%*c%s\n", this->name);
			break;

	    case 't':   	// Object Line: Simple Test Object
			SimpleTestObject* stobj;

			sscanf(buffer, "%*c%s%s%lf%f%f%f%f%f%f%f%f%f%f%f%f%f%f\n",
				oname, objfile,
				&timestep,
				&cr, &cg, &cb, &ca,
				&dx, &dy, &dz,
				&sx, &sy, &sz,
				&rangle, &rx, &ry, &rz);
	
			color[0]=cr; color[1]=cg; color[2]=cb; color[3]=ca;
			if((stobj = new SimpleTestObject(oname, color, time, timestep)) == NULL) {
				printf("ERROR: Could not allocate object node!\n");
				exit(0);
			}
			printf("\nLoading Simple Test Object %s...\n", stobj->GetName());
		
			// Add the object to the list
			this->object[this->num_object++] = (stobj);

			stobj->Load(objfile);
	    
			geom = stobj->GetGeometryPtr();
	    
			// Reposition the object
			geom->Scale(sx, sy, sz);
			geom->Rotate(TORAD(rangle), rx, ry, rz);
			geom->Translate(dx, dy, dz);
			
			stobj->Geom2State();
			stobj->SetupDisplay();
  	    	    
			break;

		case 's':   	// Object Line: A Mass-Spring-Damper Object
			MSDObject* msd;
			Real K, D;

			sscanf(buffer, "%*c%s%s%lf%lf%lf%lf%f%f%f%f%f%f%f%f%f%f%f%f%f%f\n",
				oname, objfile,
				&timestep,
				&K, &D, &mass,
				&cr, &cg, &cb, &ca,
				&dx, &dy, &dz,
				&sx, &sy, &sz,
				&rangle, &rx, &ry, &rz);
	    
			color[0]=cr; color[1]=cg; color[2]=cb; color[3]=ca;
			if((msd = new MSDObject(oname, color, time, timestep, K, D)) == NULL) {
				printf("ERROR: Could not allocate object node!\n");
				exit(0);
			}
			printf("\nLoading %s...\n", msd->GetName());
		
			// Add the object to the list
			this->object[this->num_object++] = (msd);

			msd->Load(objfile);
	    
			geom = msd->GetGeometryPtr();
	    
			// Reposition the object
			geom->Rotate(TORAD(rangle), rx, ry, rz);
			geom->Translate(dx, dy, dz);
			
			msd->Geom2State();
			msd->SetupDisplay();
  	    	    
			break;

		case 'f':   	// Object Line: A Finite Element Object
			FEM_3LMObject* fem;
			Real Rho, Lambda, Mu, Nu, Phi;

			sscanf(buffer, "%*c%s%s%lf%lf%lf%lf%lf%lf%f%f%f%f%f%f%f%f%f%f%f%f%f%f\n",
				oname, objfile,
				&timestep,
				&Rho, &Mu, &Lambda, &Nu, &Phi,
				&cr, &cg, &cb, &ca,
				&dx, &dy, &dz,
				&sx, &sy, &sz,
				&rangle, &rx, &ry, &rz);
	    
			color[0]=cr; color[1]=cg; color[2]=cb; color[3]=ca;
			if((fem = new FEM_3LMObject(oname, color, time, timestep)) == NULL) {
				printf("ERROR: Could not allocate object node!\n");
				exit(0);
			}
			printf("\nLoading %s...\n", fem->GetName());
		
			// Add the object to the list
			this->object[this->num_object++] = (fem);

			fem->Load(objfile);
			fem->SetMaterial(Rho, Mu, Lambda, Nu, Phi);

			// NOTE: The BCs are hardcoded here. Move them into fem file!
			bound = (FEMBoundary *) fem->GetBoundaryPtr();

			for(i = 0; i < bound->num_vertex; i++) {
				// NOTE: Boundary conditions are hard coded for now
				//if (0) {
				if ((bound->vertex[i].pos[0] < 1.5) && (bound->vertex[i].pos[0] > -1.5)) {
					bound->Set(i,1,bound->vertex[i].pos,0.0,zero_vector3);	// Fixed boundary (Wall)
				}
			}

			geom = fem->GetGeometryPtr();
	    
			// Reposition the object
			geom->Rotate(TORAD(rangle), rx, ry, rz);
			geom->Translate(dx, dy, dz);
			
			fem->Geom2State();
			fem->SetupDisplay();

			break;

		case 'e':   	// Object Line: A Cardiac Bioelectricity Object
			CardiacBioEObject	*bioe;
			Real TempFreq, SpVel, DutyCycle;

			sscanf(buffer, "%*c%s%s%lf%lf%lf%lf%f%f%f%f%f%f%f%f%f%f%f%f%f%f\n",
				oname, objfile,
				&timestep,
				&TempFreq, &SpVel, &DutyCycle,
				&cr, &cg, &cb, &ca,
				&dx, &dy, &dz,
				&sx, &sy, &sz,
				&rangle, &rx, &ry, &rz);
	    
			color[0]=cr; color[1]=cg; color[2]=cb; color[3]=ca;
			if((bioe = new CardiacBioEObject(oname, color, time, timestep, TempFreq, SpVel, DutyCycle)) == NULL) {
				printf("ERROR: Could not allocate object node!\n");
				exit(0);
			}
			printf("\nLoading %s...\n", bioe->GetName());
		
			// Add the object to the list
			this->object[this->num_object++] = (bioe);

			bioe->Load(objfile);

			geom = bioe->GetGeometryPtr();
	    
			// Reposition the object
			geom->Rotate(TORAD(rangle), rx, ry, rz);
			geom->Translate(dx, dy, dz);
			
			//bioe->Geom2State();
			bioe->SetupDisplay();

			break;

		case 'l':   	// Object Line: A Lumped Fluid Object
			LumpedFluidObject* lfo;
			Real	Pi, Po, Pfo, Kc, Bc, Ri, Ro;

			sscanf(buffer, "%*c%s%s%lf%lf%lf%lf%lf%lf%lf%lf%f%f%f%f%f%f%f%f%f%f%f%f%f%f\n",
				oname, objfile,
				&timestep,
				&Pi, &Po, &Pfo, &Kc, &Bc, &Ri, &Ro,
				&cr, &cg, &cb, &ca,
				&dx, &dy, &dz,
				&sx, &sy, &sz,
				&rangle, &rx, &ry, &rz);

			color[0]=cr; color[1]=cg; color[2]=cb; color[3]=ca;
			if((lfo = new LumpedFluidObject(oname, color, time, timestep,Pi,Po,Pfo,Kc,Bc,Ri,Ro)) == NULL) {
				printf("ERROR: Could not allocate object node!\n");
				exit(0);
			}
			printf("\nLoading %s...\n", lfo->GetName());
		
			// Add the object to the list
			this->object[this->num_object++] = (lfo);

			lfo->Load(objfile);

			geom = lfo->GetGeometryPtr();
	    
			// Reposition the object
			geom->Rotate(TORAD(rangle), rx, ry, rz);
			geom->Translate(dx, dy, dz);
			
			lfo->Geom2State();
			lfo->SetupDisplay();

			break;


		case 'c':   	// Connector Line:
			int				obj1, obj2;
			char			type[3];
			char			fname[55];
			Connector		*con;
			Real			param1,param2,param3;

			sscanf(buffer, "%*c%s%d%d%s%lf%lf%lf\n",
				type, &obj1, &obj2, fname, &param1, &param2, &param3);
	    
			if(!strcmp(type, "fe")) 
			{
				FEM_3LMObject		*object1 = (FEM_3LMObject *)(this->object[obj1]);
				CardiacBioEObject	*object2 = (CardiacBioEObject *)(this->object[obj2]);

				con = (FEM3LM_BIOE_Connector *) new FEM3LM_BIOE_Connector(	(FEMDomain *) object1->GetDomainPtr(), 
																			(CardiacBioEDomain *) object2->GetDomainPtr(),
																			param1,param2,param3		); 
				// Add the object to the list
				this->connector[this->num_connector++] = con;
			} 
			else if (!strcmp(type, "fl")) 
			{
				FEM_3LMObject		*object1 = (FEM_3LMObject *)(this->object[obj1]);
				LumpedFluidObject	*object2 = (LumpedFluidObject *)(this->object[obj2]);

				con = (FEM3LM_BIOE_Connector *) new FEM3LM_LUMPEDFLUID_Connector(	(FEMBoundary *) object1->GetBoundaryPtr(), 
																					(LumpedFluidBoundary *) object2->GetBoundaryPtr(),
																					fname		); 
				// Add the object to the list
				this->connector[this->num_connector++] = con;

				FEMBoundary		*test=(FEMBoundary *) object1->GetBoundaryPtr();
			}

			break;

		case '\n':
	    case '#' :
			break;
	    default:
			printf("WARNING: Parser could not understand command <%c>\n",buffer[0]);
	    break;
      }
   }
   
   fclose(prjfile);
 
   return 1;
}



