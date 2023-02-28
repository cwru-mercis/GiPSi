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

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
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

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "msd.h"
#include "errors.h"

#define		DELIM 10

////////////////////////////////////////////////////////////////
//
//	MSDObject::Load()
//
//		Reads in .obj file
//
void MSDObject::Load(char *filename)   
{
	FILE		*fp;
	unsigned int  i;
	char		errmsg[1024], 
				buffer[1024], 
				file_type[64],
				line_type;
	int			vertex_offset;
	int			v1, v2,	v3;
	Real		x, y, z;
	Real		minx, miny, minz, maxx, maxy, maxz;
	TriSurface	*geom;
  
	minx = miny = minz = FLT_MAX;
	maxx = maxy = maxz = -FLT_MAX;
  
	fp = fopen(filename, "r");
	if (fp == NULL) {
		sprintf(errmsg, "Cannot open obj file %s\n", filename);
		error_exit(-1, errmsg);
	}

	if((geometry = new TriSurface(initialcolor[0],initialcolor[1],initialcolor[2],initialcolor[3])) == NULL) {  
		printf("ERROR: Could not allocate geometry!\n");
		exit(0);
	}

	geom = (TriSurface *) geometry;

	geom->num_vertex	= 0;
	geom->num_face		= 0;


	/****************************************/
	/*	Set up boundary						*/
	/****************************************/
	//   MSD Model does not have a Boundary Implemented

	/****************************************/
	/*	Read MSD header information			*/
	/****************************************/
	fscanf(fp, "%s%d%d%d%d\n", file_type, &(geom->num_vertex), &(geom->num_face), &(num_spring), &vertex_offset);
	printf("File type : <%s>\tVertex: %d\tFace: %d\tSpring: %d\tOffset: %d\n",file_type, geom->num_vertex, geom->num_face, num_spring, vertex_offset);

	if(strcmp("MSD", file_type)) {
		sprintf(errmsg, "Invalid file type %s\n", filename);
		error_exit(-1, errmsg);
	}

	state.size					= geom->num_vertex;

	/********************************************/
	/*	Allocate continuous memory				*/
	/********************************************/

	// Allocate State
	state.POS = new Vector<Real>(geom->num_vertex * 3);
	if(state.POS == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	state.pos = new Vector<Real>[geom->num_vertex];
	for(i = 0; i < state.POS->dim(); i +=3)
		state.pos[i/3].remap(3, &((*(state.POS))[i]));

	state.VEL = new Vector<Real>(geom->num_vertex * 3);
	if(state.VEL == NULL) {
		error_exit(-1, "Cannot allocate memory for velocity!\n");
	}
	state.vel = new Vector<Real>[geom->num_vertex];
	for(i = 0; i < state.VEL->dim(); i +=3)
		state.vel[i/3].remap(3, &((*(state.VEL))[i]));

	// Allocate force and mass
	force = new Vector<Real>[geom->num_vertex];
	if(force == NULL) {
		error_exit(-1, "Cannot allocate memory for forces!\n");
	}
	mass = new Real[geom->num_vertex];
	if(mass == NULL) {
		error_exit(-1, "Cannot allocate memory for mass!\n");
	}

	// Allocate springs
	this->spring = new Spring[this->num_spring];
	if(this->spring == NULL) {
		error_exit(-1, "Cannot allocate memory for faces!\n");
	}

	// Allocate geom
	geom->vertex = new Vertex[geom->num_vertex];
	if(geom->vertex == NULL) {
		error_exit(-1, "Cannot allocate memory for vertices!\n");
	}
	geom->face = new Triangle[geom->num_face];
	if(geom->face == NULL) {
		error_exit(-1, "Cannot allocate memory for faces!\n");
	}

	/****************************************/
	/*	Read vertices						*/
	/****************************************/
	i = 0;
	while (i < geom->num_vertex) {
		fgets(buffer, 512,fp);
		sscanf(buffer, "%c%lf%lf%lf\n", &line_type, &x, &y, &z);

		if(line_type == '#' || line_type == DELIM) continue;
		if(line_type != 'v') {
			error_exit(-1, "Wrong number of vertices!\n");
		}

		geom->vertex[i].pos[0] = x; 
		geom->vertex[i].pos[1] = y; 
		geom->vertex[i].pos[2] = z;

		if(geom->vertex[i].pos[0] < minx) minx = geom->vertex[i].pos[0];
		if(geom->vertex[i].pos[1] < miny) miny = geom->vertex[i].pos[1];
		if(geom->vertex[i].pos[2] < minz) minz = geom->vertex[i].pos[2];
		if(geom->vertex[i].pos[0] > maxx) maxx = geom->vertex[i].pos[0];
		if(geom->vertex[i].pos[1] > maxy) maxy = geom->vertex[i].pos[1];
		if(geom->vertex[i].pos[2] > maxz) maxz = geom->vertex[i].pos[2];
		
		geom->vertex[i].refid = i;

		geom->vertex[i].color[0] = geom->color[0];
		geom->vertex[i].color[1] = geom->color[1];
		geom->vertex[i].color[2] = geom->color[2];
		geom->vertex[i].color[3] = geom->color[3];

		// Initialize state
		this->state.pos[i]	= geom->vertex[i].pos;
		this->state.vel[i]	= 0.0;
		this->mass[i]		= this->defaultMass;

		i++;
	}
  


	/****************************************/
	/*	Read faces							*/
	/****************************************/
	i = 0;
	while (i < geom->num_face) {
	    fscanf(fp, "%c%d%d%d\n", &line_type, &v1, &v2, &v3);

		if(line_type == '#' || line_type == DELIM) continue;
		if(line_type != 'f') {
			error_exit(-1, "Wrong number of faces!\n");
		}

		v1 -= vertex_offset;
		v2 -= vertex_offset;
		v3 -= vertex_offset;

		geom->face[i].vertex[0] = &(geom->vertex[v1]);
		geom->face[i].vertex[1] = &(geom->vertex[v2]);
		geom->face[i].vertex[2] = &(geom->vertex[v3]);
		geom->vertex[v1].valence++;
		geom->vertex[v2].valence++;
		geom->vertex[v3].valence++;
	
		i++;
	}
 	geom->calcNormals();
	
	/****************************************/
	/*	Read springs						*/
	/****************************************/
	i = 0;
	while (i < num_spring) {
	    fscanf(fp, "%c%d%d\n", &line_type, &v1, &v2);

		if(line_type == '#' || line_type == DELIM) continue;
		if(line_type != 's') {
			error_exit(-1, "Wrong number of springs!\n");
		}

		v1 -= vertex_offset;
		v2 -= vertex_offset;

		spring[i].node[0] = v1;
		spring[i].node[1] = v2;

		spring[i].dir	= state.pos[v2] - state.pos[v1];
		spring[i].L0	= spring[i].length();
		spring[i].k		= K / spring[i].L0;

		i++;
	}

	
	/****************************************/
	/*	Set B-Box							*/
	/****************************************/
  
	fclose(fp);

	// MENTAL NOTE: move this to somewhere logical asap!
	integrator = new ERKHeun3<MSDObject>(*this);

	// NOTE: This is hardwired fixing. Get rid of this asap!
	mass[0] = 0.0;
	mass[3] = 0.0;
}




////////////////////////////////////////////////////////////////
//
//	MSDObject::UpdateForces()
//
//		Updates the forces
//
inline void MSDObject::UpdateForces(State &state)   
{
	unsigned int	i;
	static			Real gg[3] = { 0.0, -g, 0.0 };
	static			const Vector<Real> g_vector(3, gg), zero_vector(3, 0.0);
	static			Vector<Real> g_force(3), d_force(3), s_force(3);
	static			Vector<Real> rel_vel(3, 0.0);

	// Clear the forces
	for(i = 0; i < state.size; i++) force[i] = zero_vector;

	// Update them by adding each component
	for(i = 0; i < state.size; i++) {
		// Add gravity
		g_force = g_vector;
		g_force *= mass[i]; 
		force[i] += g_force;

		// Add global damping
		d_force = state.vel[i];
		d_force *= -D;
		force[i] += d_force;

		// NOTE: Add others here...
	}

	int v1, v2;
	for(i = 0; i < num_spring; i++) {
		// dir = v1-v2
		v1 = spring[i].node[0];
		v2 = spring[i].node[1];

		spring[i].dir = state.pos[v1] - state.pos[v2];
		Real L = spring[i].length();

		// f = -k * (1 - L/L0) * dir
		s_force = -spring[i].k * (1 - spring[i].L0 / L) * spring[i].dir;
		force[v1] += s_force;
		force[v2] -= s_force;

		// Add local damping
		rel_vel = state.vel[v1] - state.vel[v2];
		d_force = -D * ((rel_vel * spring[i].dir) / (L*L)) * spring[i].dir;
		force[v1] += d_force;
		force[v2] -= d_force;
	}

}




////////////////////////////////////////////////////////////////
//
//	MSDObject::AccumState()
//
//		Computes y(t+h) = y(t) + h * f(..)
//
//		where	y(t+h)	= new_state
//				y(t)	= state
//				f(..)	= deriv
//
inline void MSDObject::AccumState(State &new_state, const State &state, const State &deriv, const Real &h)
{
	unsigned int				i;
	static Vector<Real>			temp(3);

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

}




////////////////////////////////////////////////////////////////
//
//	MSDObject::DerivState()
//
//		Computes the derivatives of state variables. This is simply 
//		the f() evaluation where f() = dy/dt.
//
//
inline void MSDObject::DerivState(State &deriv, State &state)
{
	unsigned int i;

	UpdateForces(state);

	for(i = 0; i < state.size; i++) {
		// dpos/dt = vel
		deriv.pos[i] = state.vel[i];

		if(mass[i] == 0.0)	// This is fixed vertex
			deriv.vel[i] = 0.0;
		else {				// dvel/dt = force/m
			deriv.vel[i] = force[i];
			deriv.vel[i] /= mass[i];
		}
	}
}



////////////////////////////////////////////////////////////////
//
//	MSDObject::AllocState()
//
//		Allocates the memory for the integrator's local state 
//		members
//
inline void MSDObject::AllocState(State &s)
{
	static unsigned int i;

	s.POS = new Vector<Real>(state.POS->dim());
	if(s.POS == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	s.pos = new Vector<Real>[state.size];
	for(i = 0; i < s.POS->dim(); i +=3)
		s.pos[i/3].remap(3, &((*s.POS)[i]));

	s.VEL = new Vector<Real>(state.VEL->dim());
	if(s.VEL == NULL) {
		error_exit(-1, "Cannot allocate memory for velocity!\n");
	}
	s.vel = new Vector<Real>[state.size];
	for(i = 0; i < s.VEL->dim(); i +=3)
		s.vel[i/3].remap(3, &((*s.VEL)[i]));

	s.size = state.size;
}



////////////////////////////////////////////////////////////////
//
//	MSDObject::Simulate()
//
//		Simulates the MSD object by a single timestep
//
//
void MSDObject::Simulate(void)
{
	integrator->Integrate(*this, timestep);
	time+=timestep;

	//We alse need to update the Boundary from states 
	//   so that we will have the correct Boundary definition for boundary calculations
	State2Bound();

}



////////////////////////////////////////////////////////////////
//
//	MSDObject::Geom2State()
//
//		Copies geometry to state
//
//
void MSDObject::Geom2State(void)
{
	static TriSurface *geom = (TriSurface *) geometry;

	for(unsigned int i=0; i < geom->num_vertex; i++) {
		state.pos[i] = geom->vertex[i].pos;
	}
}



////////////////////////////////////////////////////////////////
//
//	MSDObject::State2Geom()
//
//		Copies state to geometry
//
//
inline void MSDObject::State2Geom(void)
{
	static TriSurface *geom = (TriSurface *) geometry;

	for(unsigned int i=0; i<geom->num_vertex; i++) {
		geom->vertex[i].pos = state.pos[i];
	}
}

////////////////////////////////////////////////////////////////
//
//	MSDObject::State2Geom()
//
//		Copies state to boundary
//
//
inline void MSDObject::State2Bound(void)
{
	printf("No MSD Boundary implemented...\n");
}


////////////////////////////////////////////////////////////////
//
//	MSDObject::Display()
//
//		Displays the MSD mesh
//
//
void MSDObject::Display(void)
{
	TriSurfaceDisplayManager	*tmp = (TriSurfaceDisplayManager	*) displayMngr;

	// Copy the state into the geometry
	State2Geom();

	displayMngr->Display();
}
