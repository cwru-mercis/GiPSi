/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Source for GiPSi Loader Unit Test (LoaderUnitTest.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	LOADERUNITTEST.CPP v0.0
////
////	Source for GiPSi Loader Unit Test
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/


#include <stdio.h>

#include "bioe.h"
#include "collision.h"
#include "CollisionDARParameters.h"
#include "CollisionResponseParameters.h"
#include "connector.h"
#include "fem.h"
#include "GiPSiCamera.h"
#include "GiPSiException.h"
#include "GiPSiLight.h"
#include "LoaderUnitTest.h"
#include "logger.h"
#include "lumpedfluid.h"
#include "probe.h"
#include "msd.h"
#include "simple.h"
#include "ToolkitCollisionDARLoader.h"
#include "ToolkitCollisionDARParams.h"
#include "ToolkitConnectorLoader.h"
#include "ToolkitObjectLoader.h"
#include "ToolkitProjectLoader.h"
#include "ToolkitShaderParamLoader.h"
#include "ToolkitShaderParams.h"
#include "ToolkitShaderLoader.h"
#include "ToolkitShaders.h"
#include "XMLDocument.h"
#include "XMLDocumentBuilder.h"
#include "XMLNode.h"
#include "XMLNodeList.h"

using namespace GiPSiXMLWrapper;

/*
===============================================================================
	LoaderUnitTest class
===============================================================================
*/

LoaderUnitTest::LoaderUnitTest()
{
	myFailedCount = 0;
}

void LoaderUnitTest::Run()
{
	if (!logger)
		logger = new Logger();

	XMLDocumentBuilder builder;

	SimpleTestObject * sto = NULL;
	FEM_3LMObject * fem = NULL;
	CardiacBioEObject * cbe = NULL;
	LumpedFluidObject * lf = NULL;
	MSDObject * msd = NULL;
	RigidProbeHIO * probe = NULL;

	SimulationKernel * sk = NULL;
	VisualizationEngine * ve = NULL;

	// Test all shader parameters
	try
	{
		// Phong shader params
		printf("\nTesting Phong shader params\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\PhongShaderParams.xml");

		printf("Testing initialization:\t\t\t");
		XMLNode * rootNode = doc->GetRootNode();
		XMLNodeList * rootChildren = rootNode->GetChildren();
		XMLNode * paramsNode = rootChildren->GetNode("params");
		PhongShaderParams * params = new PhongShaderParams(paramsNode);
		TEST_VERIFY(params != NULL);
		
		printf("Testing values:\t\t\t\t");
		TEST_VERIFY(params->GetShaderName() == ShaderName_Phong &&
					params->halfWayApprox == true &&
					params->texUnitBase == 5);

		delete params;
		delete paramsNode;
		delete rootChildren;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	try
	{
		// Bump shader params
		printf("\nTesting Bump shader params\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\BumpShaderParams.xml");

		printf("Testing initialization:\t\t\t");
		XMLNode * rootNode = doc->GetRootNode();
		XMLNodeList * rootChildren = rootNode->GetChildren();
		XMLNode * paramsNode = rootChildren->GetNode("params");
		BumpShaderParams * params = new BumpShaderParams(paramsNode);
		TEST_VERIFY(params != NULL);
		
		printf("Testing values:\t\t\t\t");
		TEST_VERIFY(params->GetShaderName() == ShaderName_Bump &&
					params->texUnitBase == 1 &&
					params->texUnitHeight == 2);

		delete params;
		delete paramsNode;
		delete rootChildren;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	try
	{
		// Tissue shader params
		printf("\nTesting Tissue shader params\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\TissueShaderParams.xml");

		printf("Testing initialization:\t\t\t");
		XMLNode * rootNode = doc->GetRootNode();
		XMLNodeList * rootChildren = rootNode->GetChildren();
		XMLNode * paramsNode = rootChildren->GetNode("params");
		TissueShaderParams * params = new TissueShaderParams(paramsNode);
		TEST_VERIFY(params != NULL);
		
		printf("Testing values:\t\t\t\t");
		TEST_VERIFY(params->GetShaderName() == ShaderName_Tissue &&
					params->texUnitBase == 1 &&
					params->texUnitHeight == 2 &&
					params->ambientContribution == 3.0 &&
					params->diffuseContribution == 4.0 &&
					params->specularContribution == 5.0 &&
					params->glossiness == 6.0 &&
					params->stepSize == 7.0 &&
					params->bumpiness == 8.0 &&
					params->opacity == 9.0 &&
					params->displacement == 10.0);

		delete params;
		delete paramsNode;
		delete rootChildren;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	// Test Shader param loader
	printf("\n");
	ToolkitShaderParamLoader shaderParamLoader;
	try
	{
		// Test Phong params load
		printf("Testing Phong Params Load:\t\t");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\PhongShaderParams.xml");

		XMLNode * rootNode = doc->GetRootNode();
		ShaderParams * params = shaderParamLoader.LoadShaderParams(rootNode);
		TEST_VERIFY(params &&
					params->GetShaderName() == ShaderName_Phong);

		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		throw;
		return;
	}

	try
	{
		// Test Bump params load
		printf("Testing Bump Params Load:\t\t");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\BumpShaderParams.xml");

		XMLNode * rootNode = doc->GetRootNode();
		ShaderParams * params = shaderParamLoader.LoadShaderParams(rootNode);
		TEST_VERIFY(params &&
					params->GetShaderName() == ShaderName_Bump);

		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		throw;
		return;
	}

	try
	{
		// Test Tissue params load
		printf("Testing Tissue Params Load:\t\t");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\TissueShaderParams.xml");

		XMLNode * rootNode = doc->GetRootNode();
		ShaderParams * params = shaderParamLoader.LoadShaderParams(rootNode);
		TEST_VERIFY(params &&
					params->GetShaderName() == ShaderName_Tissue);
	}
	catch (...)
	{
		throw;
		return;
	}

	// Test all shaders
	/*
	As it turns out, we cannot test loading shaders outside of some glut context.  I don't know
	what context that is, but one of the visualization engine's sets it up.
	try
	{
		// Phong shader
		printf("\nTesting Phong shader\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\PhongShader.xml");

		printf("Testing initialization:\t\t");
		XMLNode * rootNode = doc->GetRootNode();
		XMLNodeList * rootChildren = rootNode->GetChildren();
		XMLNode * phongNode = rootChildren->GetNode(unsigned int(0));
		PhongShader * shader = new PhongShader(phongNode);
		TEST_VERIFY(shader != NULL);
		
		printf("Testing values:\t\t\t\t");
		TEST_VERIFY(shader->GetShaderName() == ShaderName_Phong &&
					shader->GetPasses() == 1);

		delete rootNode;
		delete rootChildren;
		delete phongNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	try
	{
		// Bump shader
		printf("\nTesting Bump shader\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\BumpShader.xml");

		printf("Testing initialization:\t\t");
		XMLNode * rootNode = doc->GetRootNode();
		XMLNodeList * rootChildren = rootNode->GetChildren();
		XMLNode * bumpNode = rootChildren->GetNode(unsigned int(0));
		BumpShader * shader = new BumpShader(bumpNode);
		TEST_VERIFY(shader != NULL);
		
		printf("Testing values:\t\t\t\t");
		TEST_VERIFY(shader->GetShaderName() == ShaderName_Bump &&
					shader->GetPasses() == 1);

		delete rootNode;
		delete rootChildren;
		delete bumpNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}
	
	try
	{
		// Tissue shader
		printf("\nTesting Tissue shader\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\TissueShader.xml");

		printf("Testing initialization:\t\t");
		XMLNode * rootNode = doc->GetRootNode();
		XMLNodeList * rootChildren = rootNode->GetChildren();
		XMLNode * tissueNode = rootChildren->GetNode(unsigned int(0));
		TissueShader * shader = new TissueShader(tissueNode);
		TEST_VERIFY(shader != NULL);
		
		printf("Testing values:\t\t\t\t");
		TEST_VERIFY(shader->GetShaderName() == ShaderName_Tissue &&
					shader->GetPasses() == 1);

		delete rootNode;
		delete rootChildren;
		delete tissueNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	// Test Shader loader
	ToolkitShaderLoader shaderLoader;
	try
	{
		// Test Phong load
		printf("Testing Phong Load:\t\t");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\PhongShader.xml");

		XMLNode * rootNode = doc->GetRootNode();
		Shader * shader = shaderLoader.LoadShader(rootNode);
		TEST_VERIFY(shader &&
					shader->GetShaderName() == ShaderName_Phong);
	}
	catch (...)
	{
		throw;
		return;
	}

	try
	{
		// Test Bump load
		printf("Testing Bump Load:\t\t");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\BumpShader.xml");

		XMLNode * rootNode = doc->GetRootNode();
		Shader * shader = shaderLoader.LoadShader(rootNode);
		TEST_VERIFY(shader &&
					shader->GetShaderName() == ShaderName_Bump);
	}
	catch (...)
	{
		throw;
		return;
	}

	try
	{
		// Test Tissue load
		printf("Testing Tissue Load:\t\t");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\TissueShader.xml");

		XMLNode * rootNode = doc->GetRootNode();
		Shader * shader = shaderLoader.LoadShader(rootNode);
		TEST_VERIFY(shader &&
					shader->GetShaderName() == ShaderName_Tissue);
	}
	catch (...)
	{
		throw;
		return;
	}
	*/

	// Test all simulation objects
	try
	{
		// Simple test object
		printf("\nTesting Simple Test object\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\SIMPLE.xml");

		printf("Testing object initialization:\n");
		XMLNode * rootNode = doc->GetRootNode();
		sto = new SimpleTestObject(rootNode);
		printf("\t\t\t\t");
		TEST_VERIFY(sto != NULL);

		printf("Testing general parameters:\t");
		TEST_VERIFY(strcmp(sto->name, "DUMBELL") == 0 &&
					sto->timestep == 0.001 &&
					sto->time == 0);

		printf("Testing color:\t\t\t");
		TEST_VERIFY(sto->initialcolor[0] == 1 &&
					sto->initialcolor[1] == 2 &&
					sto->initialcolor[2] == 3 &&
					sto->initialcolor[3] == 4);

		printf("Testing geometry:\t\t");
		TEST_VERIFY(sto->geometry != NULL &&
					sto->boundary == NULL &&
					sto->domain == NULL);

		printf("Testing texture:\t\t");
		TEST_VERIFY(sto->displayMngr->displayBuffer.nTextures == 1 &&
					strcmp(sto->displayMngr->displayBuffer.texture[0], "SmallTGA") == 0);

		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	try
	{
		// FEM object
		printf("\nTesting Finite Element object\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\FEM.xml");

		printf("Testing object initialization:\n");
		if (fem)
		{
			delete fem;
			fem = NULL;
		}
		XMLNode * rootNode = doc->GetRootNode();
		fem = new FEM_3LMObject(rootNode);
		printf("\t\t\t\t");
		TEST_VERIFY(fem != NULL);

		printf("Testing general parameters:\t");
		TEST_VERIFY(strcmp(fem->name, "MUSCLE") == 0 &&
					fem->timestep == 0.001);

		printf("Testing FEM parameters:\t\t");
		TEST_VERIFY(fem->elements[0].density	== 0.1 &&
					fem->elements[0].Mu			== 0.2 &&
					fem->elements[0].Lambda		== 0.3 &&
					fem->elements[0].Nu			== 1 &&
					fem->elements[0].Phi		== 2);

		printf("Testing color:\t\t\t");
		TEST_VERIFY(fem->initialcolor[0] == 0.1 &&
					fem->initialcolor[1] == 0.2 &&
					fem->initialcolor[2] == 0.3 &&
					fem->initialcolor[3] == 0.4);

		printf("Testing geometry:\t\t");
		TEST_VERIFY(fem->geometry != NULL &&
					fem->boundary != NULL &&
					fem->domain != NULL);

		printf("Testing texture:\t\t");
		TEST_VERIFY(fem->displayMngr->displayBuffer.nTextures == 1 &&
					strcmp(fem->displayMngr->displayBuffer.texture[0], "SmallTGA") == 0);

		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	try
	{
		// MSD object
		printf("\nTesting Mass Spring Damper object\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\MSD.xml");

		printf("Testing object initialization:\n");
		XMLNode * rootNode = doc->GetRootNode();
		msd = new MSDObject(rootNode);
		printf("\t\t\t\t");
		TEST_VERIFY(msd != NULL);

		printf("Testing general parameters:\t");
		TEST_VERIFY(strcmp(msd->name, "SHEET") == 0 &&
					msd->timestep == 0.001);

		printf("Testing MSD parameters:\t\t");
		TEST_VERIFY(true);

		printf("Testing color:\t\t\t");
		TEST_VERIFY(msd->initialcolor[0] == 0.1 &&
					msd->initialcolor[1] == 0.2 &&
					msd->initialcolor[2] == 0.3 &&
					msd->initialcolor[3] == 0.4);

		printf("Testing geometry:\t\t");
		TEST_VERIFY(msd->geometry != NULL &&
					msd->boundary != NULL &&
					msd->domain == NULL);

		printf("Testing texture:\t\t");
		TEST_VERIFY(msd->displayMngr->displayBuffer.nTextures == 1 &&
					strcmp(msd->displayMngr->displayBuffer.texture[0], "SmallTGA") == 0);

		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	try
	{
		// CBE object
		printf("\nTesting Cardiac Bioelectricity object\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\CBE.xml");

		printf("Testing object initialization:\n");
		if (cbe)
		{
			delete cbe;
			cbe = NULL;
		}
		XMLNode * rootNode = doc->GetRootNode();
		cbe = new CardiacBioEObject(rootNode);
		printf("\t\t\t\t");
		TEST_VERIFY(cbe != NULL);


		printf("Testing general parameters:\t");
		TEST_VERIFY(strcmp(cbe->name, "BIOELE") == 0 &&
					cbe->timestep == 0.001);

		printf("Testing CBE parameters:\t\t");
		TEST_VERIFY(cbe->TemporalFreq == 1 &&
					cbe->SpatialVel == 2 &&
					cbe->DutyCycle == 5);

		printf("Testing color:\t\t\t");
		Real temp0a = cbe->HeartMuscleGeometry->color[0];
		Real temp0b = (float)atof("0.1");
		TEST_VERIFY(cbe->HeartMuscleGeometry->color[0] == (float)atof("0.1") &&
					cbe->HeartMuscleGeometry->color[1] == (float)atof("0.2") &&
					cbe->HeartMuscleGeometry->color[2] == (float)atof("0.3") &&
					cbe->HeartMuscleGeometry->color[3] == (float)atof("0.4"));

		printf("Testing geometry:\t\t");
		TEST_VERIFY(cbe->geometry != NULL &&
					cbe->boundary == NULL &&
					cbe->domain != NULL);

		printf("Testing texture:\t\t");
		TEST_VERIFY(cbe->displayMngr->displayBuffer.nTextures == 1 &&
					strcmp(cbe->displayMngr->displayBuffer.texture[0], "SmallTGA") == 0);

		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	try
	{
		// LF object
		printf("\nTesting Lumped Fluid object\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\LF.xml");

		printf("Testing object initialization:\t");
		if (lf)
		{
			delete lf;
			lf = NULL;
		}
		XMLNode * rootNode = doc->GetRootNode();
		lf = new LumpedFluidObject(rootNode);
		TEST_VERIFY(lf != NULL);

		printf("Testing general parameters:\t");
		TEST_VERIFY(strcmp(lf->name, "BLOOD") == 0 &&
					lf->timestep == 0.001);

		printf("Testing LF parameters:\t\t");
		TEST_VERIFY(lf->Pi == 1 &&
					lf->Po == 2 &&
					lf->Pfo == 3 &&
					lf->K == 4 &&
					lf->B == 0.5 &&
					lf->Ri == 0.6 &&
					lf->Ro == 0.7);

		printf("Testing color:\t\t\t");
		TEST_VERIFY(lf->ChamberGeometry->color[0] == (float)atof("0.1") &&
					lf->ChamberGeometry->color[1] == (float)atof("0.2") &&
					lf->ChamberGeometry->color[2] == (float)atof("0.3") &&
					lf->ChamberGeometry->color[3] == (float)atof("0.4"));

		printf("Testing geometry:\t\t");
		TEST_VERIFY(lf->geometry != NULL &&
					lf->boundary != NULL &&
					lf->domain == NULL);

		printf("Testing texture:\t\t");
		TEST_VERIFY(lf->displayMngr->displayBuffer.nTextures == 1 &&
					strcmp(lf->displayMngr->displayBuffer.texture[0], "SmallTGA") == 0);

		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	try
	{
		// RigidProbeHIO
		printf("\nTesting RigidProbeHIO object\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\PHIO.xml");

		printf("Testing object initialization:\t");
		if (probe)
		{
			delete probe;
			probe = NULL;
		}
		XMLNode * rootNode = doc->GetRootNode();
		probe = new RigidProbeHIO(rootNode);
		TEST_VERIFY(probe != NULL);

		printf("Testing general parameters:\t");
		TEST_VERIFY(strcmp(probe->name, "PHANTOM") == 0 &&
					probe->timestep == 0.001);

		printf("Testing PHIO parameters:\t");
		TEST_VERIFY(probe->Identifier == 1);

		printf("Testing color:\t\t\t");
		TEST_VERIFY(probe->initialcolor[0] == (float)atof("0.1") &&
					probe->initialcolor[1] == (float)atof("0.2") &&
					probe->initialcolor[2] == (float)atof("0.3") &&
					probe->initialcolor[3] == (float)atof("0.4"));

		printf("Testing geometry:\t\t");
		TEST_VERIFY(probe->geometry != NULL &&
					probe->boundary != NULL &&
					probe->domain == NULL);

		printf("Testing texture:\t\t");
		TEST_VERIFY(probe->displayMngr->displayBuffer.nTextures == 1 &&
					strcmp(probe->displayMngr->displayBuffer.texture[0], "SmallTGA") == 0);

		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	// Test object loader
	ToolkitObjectLoader oloader(new ToolkitShaderParamLoader());
	printf("\nTesting object loader\n");
	try
	{
		printf("Testing STO Load:\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\SIMPLE.xml");
		XMLNode * rootNode = doc->GetRootNode();
		SimpleTestObject * obj = dynamic_cast<SimpleTestObject*>(oloader.LoadObject(rootNode));
		printf("\t\t\t\t");
		TEST_VERIFY(obj != NULL);
		delete obj;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}
	try
	{
		printf("Testing LF Load:\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\LF.xml");
		XMLNode * rootNode = doc->GetRootNode();
		LumpedFluidObject * obj = dynamic_cast<LumpedFluidObject*>(oloader.LoadObject(rootNode));
		printf("\t\t\t\t");
		TEST_VERIFY(obj != NULL);
		delete obj;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}
	try
	{
		printf("Testing MSD Load:\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\MSD.xml");
		XMLNode * rootNode = doc->GetRootNode();
		MSDObject * obj = dynamic_cast<MSDObject*>(oloader.LoadObject(rootNode));
		printf("\t\t\t\t");
		TEST_VERIFY(obj != NULL);
		delete obj;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}
	try
	{
		printf("Testing FEM Load:\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\FEM.xml");
		XMLNode * rootNode = doc->GetRootNode();
		FEM_3LMObject * obj = dynamic_cast<FEM_3LMObject*>(oloader.LoadObject(rootNode));
		printf("\t\t\t\t");
		TEST_VERIFY(obj != NULL);
		delete obj;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		printf("\t\t\t\t");
		TEST_VERIFY(false);
	}
	try
	{
		printf("Testing BIOE Load:\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\CBE.xml");
		XMLNode * rootNode = doc->GetRootNode();
		CardiacBioEObject * obj = dynamic_cast<CardiacBioEObject*>(oloader.LoadObject(rootNode));
		printf("\t\t\t\t");
		TEST_VERIFY(obj != NULL);
		delete obj;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		printf("\t\t\t\t");
		TEST_VERIFY(false);
	}
	try
	{
		printf("Testing PHIO Load:\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\PHIO.xml");
		XMLNode * rootNode = doc->GetRootNode();
		RigidProbeHIO * obj = dynamic_cast<RigidProbeHIO*>(oloader.LoadObject(rootNode));
		printf("\t\t\t\t");
		TEST_VERIFY(obj != NULL);
		delete obj;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		printf("\t\t\t\t");
		TEST_VERIFY(false);
	}


	// Test all connectors
	try
	{
		// FEM/LF connector
		printf("\nTesting FEM/LF connector\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\FEM_LF.xml");

		SIMObject * object[2];
		object[0] = fem;
		object[1] = lf;

		printf("Testing initialization:\n");
		XMLNode * rootNode = doc->GetRootNode();
		FEM3LM_LUMPEDFLUID_Connector * femlf = new FEM3LM_LUMPEDFLUID_Connector(rootNode, object, 2);
		printf("\t\t\t\t");
		TEST_VERIFY(femlf != NULL);

		printf("Testing boundaries:\t\t");
		TEST_VERIFY(femlf->mech != NULL &&
					femlf->lfluid != NULL);

		printf("Testing geometry:\t\t");
		TEST_VERIFY(femlf->num_face > 0 &&
					femlf->num_vertex > 0 &&
					femlf->fcorr != NULL &&
					femlf->vcorr != NULL);
		delete femlf;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	try
	{
		// FEM/CBE connector
		printf("\nTesting FEM/CBE connector\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\FEM_CBE.xml");

		SIMObject * object[2];
		object[0] = fem;
		object[1] = cbe;

		printf("Testing initialization:\t\t");
		XMLNode * rootNode = doc->GetRootNode();
		FEM3LM_BIOE_Connector * femcbe = new FEM3LM_BIOE_Connector(rootNode, object, 2);
		TEST_VERIFY(femcbe != NULL);

		printf("Testing boundaries:\t\t");
		TEST_VERIFY(femcbe->mech != NULL &&
					femcbe->bioe != NULL);

		printf("Testing geometry:\t\t");
		TEST_VERIFY(femcbe->ExcitedStressValueXX == 1 &&
					femcbe->ExcitedStressValueYY == 2 &&
					femcbe->ExcitedStressValueZZ == 3);
		delete femcbe;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	// Test connector loader
	ToolkitConnectorLoader cloader;
	printf("\nTesting connector loader\n");
	try
	{
		printf("Testing FEM/LF Load:\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\FEM_LF.xml");

		SIMObject * object[2];
		object[0] = fem;
		object[1] = lf;

		XMLNode * rootNode = doc->GetRootNode();
		FEM3LM_LUMPEDFLUID_Connector * conn = dynamic_cast<FEM3LM_LUMPEDFLUID_Connector*>(cloader.LoadConnector(rootNode, object, 2));
		printf("\t\t\t\t");
		TEST_VERIFY(conn != NULL);
		delete conn;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		printf("\t\t\t\t");
		TEST_VERIFY(false);
	}
	try
	{
		printf("Testing FEM/CBE Load:\t\t");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\FEM_CBE.xml");

		SIMObject * object[2];
		object[0] = fem;
		object[1] = cbe;

		XMLNode * rootNode = doc->GetRootNode();
		FEM3LM_BIOE_Connector * conn = dynamic_cast<FEM3LM_BIOE_Connector*>(cloader.LoadConnector(rootNode, object, 2));
		TEST_VERIFY(conn != NULL);
		delete conn;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}
/* sxn66
	// Test collision detection and response method parameters
	try
	{
		// Tri-Tri collision detection method
		printf("\nTesting Tri-Tri Collision Detection\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\TriTri.xml");

		printf("Testing initialization:\t\t");
		XMLNode * rootNode = doc->GetRootNode();
		TriTriCDParameters * tritri = new TriTriCDParameters(rootNode);
		TEST_VERIFY(tritri != NULL);

		printf("Testing parameters:\t\t");
		TEST_VERIFY(strcmp(tritri->GetMethodName(), "TriTri") == 0 &&
					tritri->colThreshold == 0.5);
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	// Test collision detection and response method loaders
	ToolkitCollisionDARLoader cdarloader;
	printf("\nTesting CDAR Loader\n");
	try
	{
		printf("Testing Tri-Tri Load:\t\t");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\TriTriLoad.xml");
		XMLNode * rootNode = doc->GetRootNode();
		TriTriCDParameters * cd = dynamic_cast<TriTriCDParameters*>(cdarloader.LoadCollisionDetectionParameters(rootNode));
		TEST_VERIFY(cd != NULL);
		delete cd;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{		
		TEST_VERIFY(false);
	}
*/
/*
	try
	{
		printf("Testing xxxxxxx Load:\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\xxxxxxx.xml");
		XMLNode * rootNode = doc->GetRootNode();
		xxxxxxx * cr = dynamic_cast<xxxxxxx*>(cdarloader.LoadCollisionResponseParameters(rootNode));
		if (!cr)
			TEST_VERIFY(false);
		else
			TEST_VERIFY(strcmp(cr->GetMethodName(), "xxxxxxx") == 0);
		delete cr;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}
*/
/* sxn66
	// Test collision detection and response object
	try
	{
		printf("\nTesting Collision Detection and Response parameters\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\CDAR.xml");

		printf("Testing initialization:\t\t");
		XMLNode * rootNode = doc->GetRootNode();
		CollisionDARParameters * cdar = new CollisionDARParameters(rootNode, new ToolkitCollisionDARLoader());
		TEST_VERIFY(cdar != NULL);

		printf("Testing methods:\t\t");
		TEST_VERIFY(cdar->collisionDetectionParameters != NULL &&
					strcmp(cdar->collisionDetectionParameters->GetMethodName(), "TriTri") == 0 &&
					cdar->collisionResponseParameters != NULL &&
					strcmp(cdar->collisionResponseParameters->GetMethodName(), "None") == 0 &&
					cdar->Enabled == false);
		delete cdar;
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}
*/
	Camera * camera = NULL;
	// Test camera object
	try
	{
		printf("\nTesting Camera object\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\Camera.xml");

		printf("Testing initialization:\t\t");
		XMLNode * rootNode = doc->GetRootNode();
		camera = new Camera(rootNode);
		TEST_VERIFY(camera != NULL);

		printf("Testing parameters:\t\t");
		TEST_VERIFY(strcmp(camera->name, "camera1") == 0 &&
					camera->pos[0] == 3 &&
					camera->pos[1] == 2 &&
					camera->pos[2] == 1 &&
					camera->lookAt[0] == 1 &&
					camera->lookAt[1] == 2 &&
					camera->lookAt[2] == 3 &&
					camera->up[0] == 4 &&
					camera->up[1] == 5 &&
					camera->up[2] == 6 &&
					camera->fov == 90);
		delete rootNode;
		delete doc;
	}
	catch (...)
	{
		TEST_VERIFY(false);
	}

	// Test light object
	try
	{
		printf("\nTesting Light object\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\Light2.xml");

		printf("Testing initialization:\t\t");
		XMLNode * rootNode = doc->GetRootNode();
		Light * light = new Light(rootNode, &camera, 1);
		TEST_VERIFY(light != NULL);

		printf("Testing parameters:\t\t");
		TEST_VERIFY(light->pos[0] == 1 &&
					light->pos[1] == 2 &&
					light->pos[2] == 3 &&
					light->dir[0] == 4 &&
					light->dir[1] == 5 &&
					light->dir[2] == 6 &&
					light->color[0] == 0.1 &&
					light->color[1] == 0.2 &&
					light->color[2] == 0.3 &&
					light->color[3] == 0.4 &&
					light->attached == NULL &&
					camera->nLights == 1 &&
					camera->light[0] == light);

		delete light;
		delete rootNode;
		delete doc;
		camera->nLights = 0;
		delete camera->light;
		camera->light = NULL;

		printf("Testing attachement:\t\t");
		doc = builder.Build(".\\XMLFiles\\Light.xml");
		rootNode = doc->GetRootNode();
		light = new Light(rootNode, &camera, 1);
		TEST_VERIFY(light != NULL &&
					light->attached == camera &&
					camera->nLights == 1 &&
					camera->light[0] == light);

	}
	catch (...)
	{
		TEST_VERIFY(false);
	}
	delete camera;
	camera = NULL;

	// Test simulation kernel
	try
	{
		printf("\nTesting Sim. Kernel Load:\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\SimulationKernel.xml");
		if (!doc)
		{
			printf("\t\t\t\t");
			TEST_VERIFY(false);
		}
		else
		{
			XMLNode * rootNode = doc->GetRootNode();
			sk = new SimulationKernel(rootNode, new ToolkitObjectLoader(new ToolkitShaderParamLoader()), new ToolkitConnectorLoader(), new ToolkitCollisionDARLoader());
			printf("\t\t\t\t");
			TEST_VERIFY(sk != NULL &&
						sk->num_object == 3 &&
						strcmp(sk->object[0]->GetName(), "BLOOD") == 0 &
						strcmp(sk->object[1]->GetName(), "BIOELE") == 0 &
						strcmp(sk->object[2]->GetName(), "MUSCLE") == 0 &
						sk->num_connector == 2 &&
						sk->computationalHook == false &&
						sk->networkHook == false &&
						sk->simTime == 0.01);/* &&
						strcmp(sk->collisionDAR->collisionDetectionParameters->GetMethodName(), "Tri-Tri") == 0 &&
						strcmp(sk->collisionDAR->collisionResponseParameters->GetMethodName(), "None") == 0);*/
			delete rootNode;
			delete doc;
		}
	}
	catch (...)
	{
		printf("\t\t\t\t");
		TEST_VERIFY(false);
	}

	// Test visualization engine
	try
	{
		printf("\nTesting Vis. Engine Load:\n");
		XMLDocument * doc = builder.Build(".\\XMLFiles\\VisualizationEngine.xml");
		if (!doc)
		{
			printf("\t\t\t\t");
			TEST_VERIFY(false);
		}
		else
		{
			char * argv[] = {"", ".\\XMLFiles\\VisualizationEngine.xml"};
			XMLNode * rootNode = doc->GetRootNode();
			ve = new GlutVisualizationEngine(2, argv, sk->GetDisplayBufferHead(), rootNode, 200, 200, 800, 600);
			printf("\t\t\t\t");
			TEST_VERIFY(ve != NULL &&
						ve->nBuffers == 6 &&
						ve->nTextures == 3 &&
						strcmp(ve->allTextures[0]->GetObjectName(), "SmallTGA") == 0 &&
						strcmp(ve->allTextures[1]->GetObjectName(), "MediumTGA") == 0 &&
						strcmp(ve->allTextures[2]->GetObjectName(), "LargeTGA") == 0 &&
						ve->nShaders == int(Shader::Compatibility()) * 3 &&
							ve->allShaders[0]->GetShaderName() == ShaderName_Phong &&
							ve->allShaders[0]->GetPasses() == 1 &&
							ve->allShaders[1]->GetShaderName() == ShaderName_Bump &&
							ve->allShaders[1]->GetPasses() == 1 &&
							ve->allShaders[2]->GetShaderName() == ShaderName_Tissue &&
							ve->allShaders[2]->GetPasses() == 1 &&
						ve->nScenes == 1 &&
						ve->allScenes[0]->nCamera == 1 &&
						ve->allScenes[0]->nLight == 1 &&
						ve->allScenes[0]->nBuffer == 3 &&
						ve->allScenes[0]->nTexture == ve->nTextures &&
						ve->allScenes[0]->nShader == int(Shader::Compatibility()) * 3 &&
							ve->allScenes[0]->shader[0]->GetShaderName() == ShaderName_Phong &&
							ve->allScenes[0]->shader[1]->GetShaderName() == ShaderName_Bump &&
							ve->allScenes[0]->shader[2]->GetShaderName() == ShaderName_Tissue);
			delete rootNode;
			delete doc;
		}
	}
	catch (...)
	{
		printf("\t\t\t\t");
		TEST_VERIFY(false);
	}

	// Perform extra shader tests
	try
	{
		printf("\nPerforming ShaderParam packing tests:\n");
		printf("Test PhongShaderParams:\t\t");
		sk->object[0]->Display();
		sk->object[0]->displayMngr->displayBuffer.ConditionalDequeue();
		DisplayArray * readArray = sk->object[0]->displayMngr->displayBuffer.GetReadArray();
		TriSurface * geometry1 = dynamic_cast<TriSurface*>(sk->object[0]->geometry);
		TEST_VERIFY(	geometry1 &&
						readArray &&
						readArray->dispArray[0] == (float)geometry1->vertex[0].color[0] &&
						readArray->dispArray[1] == (float)geometry1->vertex[0].color[1] &&
						readArray->dispArray[2] == (float)geometry1->vertex[0].color[2] &&
						readArray->dispArray[3] == (float)geometry1->vertex[0].color[3] &&
						readArray->dispArray[4] == (float)geometry1->vertex[0].n[0] &&
						readArray->dispArray[5] == (float)geometry1->vertex[0].n[1] &&
						readArray->dispArray[6] == (float)geometry1->vertex[0].n[2] &&
						readArray->dispArray[7] == (float)geometry1->vertex[0].pos[0] &&
						readArray->dispArray[8] == (float)geometry1->vertex[0].pos[1] &&
						readArray->dispArray[9] == (float)geometry1->vertex[0].pos[2]);

		printf("Test BumpShaderParams:\t\t");
		sk->object[1]->Display();
		sk->object[1]->displayMngr->displayBuffer.ConditionalDequeue();
		readArray = sk->object[1]->displayMngr->displayBuffer.GetReadArray();
		TetVolume * geometry2 = dynamic_cast<TetVolume*>(sk->object[1]->geometry);
		TEST_VERIFY(	geometry2 &&
						readArray &&
						readArray->dispArray[0] == (float)geometry2->vertex[0].texcoord[0] &&
						readArray->dispArray[1] == (float)geometry2->vertex[0].texcoord[1] &&
						readArray->dispArray[2] == (float)geometry2->vertex[0].color[0] &&
						readArray->dispArray[3] == (float)geometry2->vertex[0].color[1] &&
						readArray->dispArray[4] == (float)geometry2->vertex[0].color[2] &&
						readArray->dispArray[5] == (float)geometry2->vertex[0].color[3] &&
						readArray->dispArray[6] == (float)geometry2->vertex[0].n[0] &&
						readArray->dispArray[7] == (float)geometry2->vertex[0].n[1] &&
						readArray->dispArray[8] == (float)geometry2->vertex[0].n[2] &&
						readArray->dispArray[9] == (float)geometry2->vertex[0].pos[0] &&
						readArray->dispArray[10] == (float)geometry2->vertex[0].pos[1] &&
						readArray->dispArray[11] == (float)geometry2->vertex[0].pos[2] &&
						readArray->dispArray[12] == (float)geometry2->vertex[0].tangent[0] &&
						readArray->dispArray[13] == (float)geometry2->vertex[0].tangent[1] &&
						readArray->dispArray[14] == (float)geometry2->vertex[0].tangent[2]);

		printf("Test TissueShaderParams:\t");
		sk->object[2]->Display();
		sk->object[2]->displayMngr->displayBuffer.ConditionalDequeue();
		readArray = sk->object[2]->displayMngr->displayBuffer.GetReadArray();
		geometry2 = dynamic_cast<TetVolume*>(sk->object[2]->geometry);
		TEST_VERIFY(	geometry2 &&
						readArray &&
						readArray->dispArray[0] == (float)geometry2->vertex[0].texcoord[0] &&
						readArray->dispArray[1] == (float)geometry2->vertex[0].texcoord[1] &&
						readArray->dispArray[2] == (float)geometry2->vertex[0].color[0] &&
						readArray->dispArray[3] == (float)geometry2->vertex[0].color[1] &&
						readArray->dispArray[4] == (float)geometry2->vertex[0].color[2] &&
						readArray->dispArray[5] == (float)geometry2->vertex[0].color[3] &&
						readArray->dispArray[6] == (float)geometry2->vertex[0].n[0] &&
						readArray->dispArray[7] == (float)geometry2->vertex[0].n[1] &&
						readArray->dispArray[8] == (float)geometry2->vertex[0].n[2] &&
						readArray->dispArray[9] == (float)geometry2->vertex[0].pos[0] &&
						readArray->dispArray[10] == (float)geometry2->vertex[0].pos[1] &&
						readArray->dispArray[11] == (float)geometry2->vertex[0].pos[2] &&
						readArray->dispArray[12] == (float)geometry2->vertex[0].tangent[0] &&
						readArray->dispArray[13] == (float)geometry2->vertex[0].tangent[1] &&
						readArray->dispArray[14] == (float)geometry2->vertex[0].tangent[2]);
	}
	catch (...)
	{
		printf("\t\t\t\t");
		TEST_VERIFY(false);
	}


	// Test full project
	try
	{
		printf("\nTesting Full Project Load:\n");
		if (ve)
		{
			delete ve;
			ve = NULL;
		}
		if (sk)
		{
			delete sk;
			sk = NULL;
		}

		char * argv[] = {"", ".\\XMLFiles\\Project.xml"};
		ToolkitProjectLoader ploader;
		HapticsManager * hm;
		ploader.LoadProject(1, argv, &sk, &ve, &hm);
		if (!sk || !ve)
		{
			printf("\t\t\t\t");
			TEST_VERIFY(false);
		}
		else
		{
			printf("\t\t\t\t");
			TEST_VERIFY(sk &&
						sk->num_object == 3 &&
						sk->num_connector == 2 &&
						sk->computationalHook == false &&
						sk->networkHook == false &&
						sk->simTime == (Real)atof("0.01") &&
						sk->num_texture == 3 &&
						strcmp(sk->texture[0]->GetObjectName(), "SmallTGA") == 0 &&
						strcmp(sk->texture[1]->GetObjectName(), "MediumTGA") == 0 &&
						strcmp(sk->texture[2]->GetObjectName(), "LargeTGA") == 0 &&/*
						strcmp(sk->collisionDAR->collisionDetectionParameters->GetMethodName(), "TriTri") == 0 &&
						strcmp(sk->collisionDAR->collisionResponseParameters->GetMethodName(), "None") == 0 &&*/
						ve &&
						ve->nBuffers == 6 &&
						ve->nTextures == 3 &&
						strcmp(ve->allTextures[0]->GetObjectName(), "SmallTGA") == 0 &&
						strcmp(ve->allTextures[1]->GetObjectName(), "MediumTGA") == 0 &&
						strcmp(ve->allTextures[2]->GetObjectName(), "LargeTGA") == 0 &&
						ve->nShaders == int(Shader::Compatibility()) &&
						ve->allShaders[0]->GetShaderName() == ShaderName_Phong &&
						ve->nScenes == 1 &&
						ve->allScenes[0]->nCamera == 1 &&
						ve->allScenes[0]->nLight == 1 &&
						ve->allScenes[0]->nBuffer == 3 &&
						ve->allScenes[0]->nTexture == ve->nTextures &&
						ve->allScenes[0]->nShader == int(Shader::Compatibility()) &&
						ve->allScenes[0]->shader[0]->GetShaderName() == ShaderName_Phong);
		}
	}
	catch (...)
	{
		printf("\t\t\t\t");
		TEST_VERIFY(false);
	}

	// cleanup
	if (fem)
	{
		delete fem;
		fem = NULL;
	}
	if (cbe)
	{
		delete cbe;
		cbe = NULL;
	}
	if (lf)
	{
		delete lf;
		lf = NULL;
	}
	if (msd)
	{
		delete msd;
		msd = NULL;
	}
	if (probe)
	{
		delete probe;
		probe = NULL;
	}
	if (sk)
	{
		delete sk;
		sk = NULL;
	}
	if (sto)
	{
		delete sto;
		sto = NULL;
	}
	if (ve)
	{
		delete ve;
		ve = NULL;
	}

	if (logger)
	{
		delete logger;
		logger = NULL;
	}
}

void LoaderUnitTest::TEST_VERIFY(bool test)
{
	if (test)
	{
		printf("Passed\n");
	}
	else
	{
		myFailedCount++;
		printf("Failed\n");
	}
}
