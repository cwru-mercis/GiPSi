/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Visualization Engine implementation
(GiPSiVisualizationEngine.cpp).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

/*
===============================================================================
	Headers
===============================================================================
*/

#include <GL/glut.h>

#include "GiPSiVisualizationEngine.h"

/*
===============================================================================
	VisualizationEngine
===============================================================================
*/

VisualizationEngine::VisualizationEngine(SimulationKernel* simKernel)
{
	// At this point GLut has not yet been initialized therefore all we can do
	// is to pass the reference to the simulation kernel.
	
	this->simKernel		= simKernel;

	this->nScenes		= 0;
	this->allScenes		= NULL;
	this->nTextures		= 0;
	this->allTextures	= NULL;
	this->nShaders		= 0;
	this->allShaders	= NULL;
}

DisplayArray ** VisualizationEngine::ExtractMeshes(void)
{
	DisplayArray **objects;
	objects = new DisplayArray*[this->simKernel->num_object];

	for (int i=0; i<this->simKernel->num_object; i++)
	{
		objects[i] = this->simKernel->object[i]->displayMngr->GetDisplay();	
	}

	return objects;
}

void VisualizationEngine::Init(void)
{
	// Setup OpenGL

	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK,	GL_LINE);

	// *******************************************
	// *******************************************
	//  APPLICATION SPECIFIC PART BEGINS HERE
	// *******************************************
	// *******************************************

	// Load Textures

	this->nTextures			= 13;
	this->allTextures		= new Texture*[this->nTextures];
	
	this->allTextures[0]	= new Texture2D(TextureName_TestBlood,						"../GiPSiVisualization/Textures/test_blood.tga");
	this->allTextures[1]	= new Texture2D(TextureName_VentricleSystemBase,			"../GiPSiVisualization/Textures/ventricle_system_veins_base.tga");
	this->allTextures[2]	= new Texture2D(TextureName_VentricleSystemHeightMap,		"../GiPSiVisualization/Textures/ventricle_system_veins_heightmap.tga");
	this->allTextures[3]	= new Texture2D(TextureName_ChoroidPlexusBase,				"../GiPSiVisualization/Textures/choroid_plexus_base.tga");
	this->allTextures[4]	= new Texture2D(TextureName_ChoroidPlexusHeightMap,			"../GiPSiVisualization/Textures/choroid_plexus_heightmap.tga");
	this->allTextures[5]	= new Texture2D(TextureName_AnteriorSeptalVeinBase,			"../GiPSiVisualization/Textures/anterior_septal_vein_base.tga");
	this->allTextures[6]	= new Texture2D(TextureName_AnteriorSeptalVeinHeightMap,	"../GiPSiVisualization/Textures/anterior_septal_vein_heightmap.tga");
	this->allTextures[7]	= new Texture2D(TextureName_BasilarArteryBase,				"../GiPSiVisualization/Textures/basilar_artery_base.tga");
	this->allTextures[8]	= new Texture2D(TextureName_BasilarArteryHeightMap,			"../GiPSiVisualization/Textures/basilar_artery_heightmap.tga");
	this->allTextures[9]	= new Texture2D(TextureName_MammillarryBodiesBase,			"../GiPSiVisualization/Textures/mammillarry_bodies_base.tga");
	this->allTextures[10]	= new Texture2D(TextureName_MammillarryBodiesHeightMap,		"../GiPSiVisualization/Textures/mammillarry_bodies_heightmap.tga");
	this->allTextures[11]	= new Texture2D(TextureName_VentricleFloorBase,				"../GiPSiVisualization/Textures/ventricle_floor_base.tga");
	this->allTextures[12]	= new Texture2D(TextureName_VentricleFloorHeightMap,		"../GiPSiVisualization/Textures/ventricle_floor_heightmap.tga");
	
	// Load Shaders

	if (Shader::Compatibility())
	{
		this->nShaders		= 3;
		this->allShaders	= new Shader*[this->nShaders];

		this->allShaders[0] = new PhongShader (	"../GiPSiVisualization/Shaders/phong",	1, ShaderName_Phong);
		this->allShaders[1] = new BumpShader  (	"../GiPSiVisualization/Shaders/bump",	1, ShaderName_Bump);
		this->allShaders[2] = new TissueShader(	"../GiPSiVisualization/Shaders/tissue",	1, ShaderName_Tissue);
	}
	else
	{
		fprintf(stderr, "Driver does not support OpenGL Shading Language\n");
	}

	// Create Scenes

	this->nScenes		= 1;
	this->allScenes		= new Scene*[this->nScenes];

	this->allScenes[0]	= new Scene(this->simKernel->num_object, this->ExtractMeshes(), this->nTextures, this->allTextures, this->nShaders, this->allShaders);

	// *******************************************
	// *******************************************
	//  APPLICATION SPECIFIC PART BEGINS HERE
	// *******************************************
	// *******************************************

}
