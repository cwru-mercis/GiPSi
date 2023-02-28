/*
===============================================================================//
   Phong Shading

   Authors: Svend Johannsen

   Copyright (c) 2005
===============================================================================
*/

uniform sampler2D Base;
uniform bool HalfWayApprox;
uniform float AmbientContribution;
uniform float DiffuseContribution;
uniform float SpecularContribution;
uniform float SpecularExponent;

varying vec3 vNormal, vLight, vView, vHalfway;
vec3 n, ldir, viewV;
float ndotl;

vec3 AmbientComponent(void)
{
   return vec3(AmbientContribution * gl_LightSource[0].ambient.rgb);
}

vec3 DiffuseComponent(void)
{
   return vec3(DiffuseContribution * ndotl * gl_LightSource[0].diffuse.rgb);
}

vec3 SpecularComponent(void)
{
   vec3 halfV, reflection;
   float NdotHV, VdotL;

   if(HalfWayApprox)
   {
      // Approximation to the specular reflection using the halfway vector
      halfV = normalize(vHalfway);
      NdotHV = max(dot(n,halfV),0.0);
      return SpecularContribution * gl_LightSource[0].specular.rgb * pow(NdotHV,SpecularExponent);
   }
   else
   {
      // Conventional specular component
      reflection = normalize(reflect(ldir, n));
      VdotL=max(0.0, dot(viewV, reflection));
      return SpecularContribution * gl_LightSource[0].specular.rgb * pow(VdotL,SpecularExponent);
   }
}

/*
===============================================================================
   Phong Shading: Fragment Program
===============================================================================
*/

void main(void)
{
   n	= normalize(vNormal);
   ldir	= normalize(vLight);
   viewV = normalize(vView);
   ndotl = max(dot(n, ldir), 0.0);
   vec3 color = vec3(0.0, 0.0, 0.0);

    // Texture look up
   
   vec4 texel  = texture2D( Base, gl_TexCoord[0].st );
   
   // Phong Illumination Model
   if (ndotl > 0.0)
   {
      color = (AmbientComponent() + DiffuseComponent()) * texel.rgb +
                SpecularComponent();
   }
  
   // Final color
   
   gl_FragColor = vec4(color, texel.a);
}