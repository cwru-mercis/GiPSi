/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Source for GiPSi Toolkit Shader Params (ToolkitShaderParams.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	TOOLKITSHADERPARAMS.CPP v0.0
////
////	Source for GiPSi Toolkit Shader Params
////
////////////////////////////////////////////////////////////////


#include "ToolkitShaderParams.h"

/**
 * Constructor.
 * 
 * @param phongParamsNode XML project file 'phongParams' node.
 */
PhongShaderParams::PhongShaderParams(XMLNode * phongParamsNode)
{
	try
	{
		XMLNodeList * phongParamsChildren = phongParamsNode->GetChildren();

		const char * halfWayApproxStr = GetValue(phongParamsChildren, "halfWayApprox");
		if (strcmp(halfWayApproxStr, "true")== 0)
			halfWayApprox = true;
		else if (strcmp(halfWayApproxStr, "false")== 0)
			halfWayApprox = false;
		else
		{
			throw new GiPSiException("PhongShaderParams constructor", "Unrecognized 'halfWayApprox' value found.");
			return;
		}
		delete halfWayApproxStr;

		const char * texUnitBaseStr = GetValue(phongParamsChildren, "texUnitBase");
		texUnitBase = atoi(texUnitBaseStr);
		delete texUnitBaseStr;

		delete phongParamsChildren;
	}
	catch (...)
	{
		throw;
		return;
	}
}



/**
 * Constructor.
 * 
 * @param bumpParamsNode XML project file 'bumpParams' node.
 */
BumpShaderParams::BumpShaderParams(XMLNode * bumpParamsNode)
{
	try
	{
		XMLNodeList * bumpParamsChildren = bumpParamsNode->GetChildren();

		const char * texUnitBaseStr = GetValue(bumpParamsChildren, "texUnitBase");
		texUnitBase = atoi(texUnitBaseStr);
		delete texUnitBaseStr;

		const char * texUnitHeightStr = GetValue(bumpParamsChildren, "texUnitHeight");
		texUnitHeight = atoi(texUnitHeightStr);
		delete texUnitHeightStr;

		delete bumpParamsChildren;
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Constructor.
 * 
 * @param tissueParamsNode XML project file 'tissueParams' node.
 */
TissueShaderParams::TissueShaderParams(XMLNode * tissueParamsNode)
{
	try
	{
		XMLNodeList * tissueParamsChildren = tissueParamsNode->GetChildren();

		const char * texUnitBaseStr = GetValue(tissueParamsChildren, "texUnitBase");
		texUnitBase = atoi(texUnitBaseStr);
		delete texUnitBaseStr;

		const char * texUnitHeightStr = GetValue(tissueParamsChildren, "texUnitHeight");
		texUnitHeight = atoi(texUnitHeightStr);
		delete texUnitHeightStr;

		const char * ambientContributionStr = GetValue(tissueParamsChildren, "ambientContribution");
		ambientContribution = atof(ambientContributionStr);
		delete ambientContributionStr;

		const char * diffuseContributionStr = GetValue(tissueParamsChildren, "diffuseContribution");
		diffuseContribution = atof(diffuseContributionStr);
		delete diffuseContributionStr;

		const char * specularContributionStr = GetValue(tissueParamsChildren, "specularContribution");
		specularContribution = atof(specularContributionStr);
		delete specularContributionStr;

		const char * glossinessStr = GetValue(tissueParamsChildren, "glossiness");
		glossiness = atof(glossinessStr);
		delete glossinessStr;

		const char * stepSizeStr = GetValue(tissueParamsChildren, "stepSize");
		stepSize = atof(stepSizeStr);
		delete stepSizeStr;

		const char * bumpinessStr = GetValue(tissueParamsChildren, "bumpiness");
		bumpiness = atof(bumpinessStr);
		delete bumpinessStr;

		const char * opacityStr = GetValue(tissueParamsChildren, "opacity");
		opacity = atof(opacityStr);
		delete opacityStr;

		const char * displacementStr = GetValue(tissueParamsChildren, "displacement");
		displacement = atof(displacementStr);
		delete displacementStr;

		delete tissueParamsChildren;
	}
	catch (...)
	{
		throw;
		return;
	}
}