/*
===============================================================================//
   Bump Mapping

   Authors: Svend Johannsen

   Copyright (c) 2005
===============================================================================
*/

// NOTE: The supplied LightPosition should be in view space.
   
uniform vec4 LightPosition;

varying vec3 vNormal, vTangent, vBinormal, vLight, vHalfway;

//attribute vec3 rm_Tangent;

/*
===============================================================================
   Bump Mapping: Vertex Program
===============================================================================
*/

void main(void)
{
   vec3 pPos, vView, vVertexTangent;
   
   // Transform vertex position to view space
   
   pPos = vec3( gl_ModelViewMatrix * gl_Vertex );
   
   // HACK: The tangent vector is stored in the vertex color
   
   //vVertexTangent = rm_Tangent;
   //vVertexTangent = vec3(1.0);
   vVertexTangent = vec3(gl_Color) * 2.0 - 1.0;
   
   // Compute normal, tangent, binormal, light and halfway vectors in view space
   
   vNormal   = normalize(gl_NormalMatrix * gl_Normal);
   vTangent  = normalize(gl_NormalMatrix * vVertexTangent);
   vBinormal = normalize(cross(vNormal, vTangent));
   vLight    = normalize(vec3(LightPosition) - pPos);
   vView     = normalize(-pPos);
   vHalfway  = normalize(vLight + vView );
   
   gl_TexCoord[0] = gl_MultiTexCoord0;
   gl_Position = ftransform();
}