/*
======================================================================================================
   Tissue Shader

   Authors: Svend Johannsen

   Copyright (c) 2005
======================================================================================================
*/

// NOTE: The supplied LightPosition should be in view space.

uniform float Displacement;   
uniform vec4  LightPosition;

varying vec3 vNormal, vTangent, vBinormal, vLight, vHalfway;

//attribute vec3 rm_Tangent;

/*
======================================================================================================
   Tissue Shader: Vertex Program
======================================================================================================
*/

void main(void)
{
   vec4 pObjectSpacePos, pViewSpacePos;
   vec3 vView, vVertexTangent;
   
   // Transform vertex position to view space
   
   pObjectSpacePos = gl_Vertex + Displacement * vec4(gl_Normal, 0.0);
   pViewSpacePos   = gl_ModelViewMatrix * pObjectSpacePos;
   
   // HACK: The tangent vector is stored in the vertex color
   
   vVertexTangent = vec3(gl_Color) * 2.0 - 1.0;
   
   // Compute normal, tangent, binormal, light and halfway vectors in view space
   
   vNormal   = normalize(gl_NormalMatrix * gl_Normal);
   vTangent  = normalize(gl_NormalMatrix * vVertexTangent);
   vBinormal = normalize(cross(vNormal, vTangent));
   vLight    = normalize(vec3(LightPosition - pViewSpacePos));
   vView     = normalize(-vec3(pViewSpacePos));
   vHalfway  = normalize(vLight + vView );
   
   gl_TexCoord[0] = gl_MultiTexCoord0;
   gl_Position = gl_ModelViewProjectionMatrix * pObjectSpacePos;
}