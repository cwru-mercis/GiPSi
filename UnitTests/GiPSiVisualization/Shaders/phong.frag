/*
===============================================================================//
   Phong Shading

   Authors: Svend Johannsen

   Copyright (c) 2005
===============================================================================
*/

uniform sampler2D Base;
uniform bool HalfWayApprox;

varying vec3 vNormal, vLight, vView, vHalfway;

const float AmbientContribution  = 0.1;
const float SpecularContribution = 0.3;
const float DiffuseContribution  = 1.0;

vec3 AmbientComponent(void)
{
   return vec3(AmbientContribution);
}

vec3 DiffuseComponent(void)
{
   return vec3(DiffuseContribution * max(0.0, dot(vNormal, vLight)));
}

vec3 SpecularComponent(void)
{
   if(HalfWayApprox)
   {
      // Approximation to the specular reflection using the halfway vector
      
      return vec3(SpecularContribution * pow(max(0.0, dot(vNormal, vHalfway)), 64.0));
   }
   else
   {
      // Conventional specular component
      
      vec3 reflection = normalize(reflect(-vLight, vNormal));
      return vec3(SpecularContribution * pow(max(0.0, dot(vView, reflection)), 16.0));
   }
}

/*
===============================================================================
   Phong Shading: Fragment Program
===============================================================================
*/

void main(void)
{
    // Texture look up
   
   vec4 texel  = texture2D( Base, gl_TexCoord[0].st );
   
   // Phong Illumination Model
   
   vec3 color = (AmbientComponent() + DiffuseComponent()) * texel.rgb +
                SpecularComponent();
  
   // Final color
   
   gl_FragColor = vec4(color, texel.a);
}