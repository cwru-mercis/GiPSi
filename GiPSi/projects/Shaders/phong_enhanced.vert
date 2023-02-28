/*
===============================================================================//
   Phong Shading

   Authors: Svend Johannsen

   Copyright (c) 2005
===============================================================================
*/

// NOTE: The supplied LightPosition should be in view space to simulate a fixed
// position light source in world space.
   
uniform bool HalfWayApprox;

varying vec3 vNormal, vLight, vView, vHalfway;
varying vec4 diffuse, ambient;

/*
===============================================================================
   Phong Shading: Vertex Program
===============================================================================
*/

void main(void)
{
   // Transform vertex position to view space
   
   vec3 pos = vec3( gl_ModelViewMatrix * gl_Vertex );
   
   // Compute normal, light and view vectors in view space
   
   vNormal   = normalize(gl_NormalMatrix * gl_Normal);
   vLight    = normalize(vec3(gl_LightSource[0].position) - pos);
   vView     = normalize(pos);
   
   // Compute the halfway vector if the halfway approximation is used
   
   if(HalfWayApprox)
   {
	vHalfway  = normalize(gl_LightSource[0].halfVector.xyz);
//      vHalfway  = normalize(vLight + vView );
   }


   gl_TexCoord[0] = gl_MultiTexCoord0;
   gl_Position = ftransform();
}