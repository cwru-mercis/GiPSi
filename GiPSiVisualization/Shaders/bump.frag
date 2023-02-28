/*
===============================================================================//
   Bump Mapping

   Authors: Svend Johannsen

   Copyright (c) 2005
===============================================================================
*/

uniform sampler2D Base;
uniform sampler2D Height;

varying vec3 vNormal, vTangent, vBinormal, vLight, vHalfway;

const float AmbientContribution  = 0.5;
const float DiffuseContribution  = 0.5;
const float SpecularContribution = 0.3;

vec3 AmbientComponent(void)
{
   return vec3(AmbientContribution);
}

vec3 DiffuseComponent(vec3 normal)
{
   return vec3(DiffuseContribution * max(0.0, dot(normal, vLight)));
}

vec3 SpecularComponent(vec3 normal)
{
   // Approximation to the specular reflection using the halfway vector
   
   return vec3(SpecularContribution * pow(max(0.0, dot(normal, vHalfway)), 16.0));
}

/*
===============================================================================
   Bump Mapping: Fragment Program
===============================================================================
*/

void main(void)
{
   // Matrix to transform from tangent space into light space
   
   mat3 rotation = mat3(normalize(vTangent) , normalize(vBinormal), normalize(vNormal));
   
   // Texture look up
   
   vec4 texel    = texture2D(Base, gl_TexCoord[0].st);
   vec4 height   = texture2D(Height, gl_TexCoord[0].st);
   vec4 heightS  = texture2D(Height, gl_TexCoord[0].st + vec2(0.01, 0.0));
   vec4 heightT  = texture2D(Height, gl_TexCoord[0].st + vec2(0.0, 0.01));
   
   // Calculate the change in height in both directions
   
   float pertS   = heightS.r - height.r;
   float pertT   = heightT.r - height.r;
   
   float emphasis = 1.0;
   vec3  vTangentSpaceNormal = normalize(vec3(emphasis*pertS, emphasis*pertT, 1.0));
   
   // Normal to view space
   
   vec3 vViewSpaceNormal = vTangentSpaceNormal * rotation;
   
   // Phong Illumination Model
   
   vec3 color = (AmbientComponent() + DiffuseComponent(vViewSpaceNormal)) * texel.rgb
              + SpecularComponent(vViewSpaceNormal);
  
   // Final color
   
   gl_FragColor = vec4(color, 1.0);
}