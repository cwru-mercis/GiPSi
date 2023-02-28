/*
======================================================================================================
   Tissue Shader

   Authors: Svend Johannsen

   Copyright (c) 2005
======================================================================================================
*/

uniform sampler2D Base;
uniform sampler2D Height;

uniform float AmbientContribution;
uniform float DiffuseContribution;
uniform float SpecularContribution;
uniform float Glossiness;
uniform float StepSize;
uniform float Bumpiness;
uniform float Opacity;


varying vec3 vNormal, vTangent, vBinormal, vLight, vHalfway;

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
   
   return vec3(SpecularContribution * pow(max(0.0, dot(normal, vHalfway)), Glossiness));
}

/*
======================================================================================================
   Tissue Shader: Fragment Program
======================================================================================================
*/

void main(void)
{
   // Matrix to transform from tangent space into light space
   
   mat3 rotation = mat3(normalize(vTangent), normalize(vBinormal), normalize(vNormal));
   
   // Texture look up
   
   vec4 texel    = texture2D(Base,   gl_TexCoord[0].st);
   vec4 height   = texture2D(Height, gl_TexCoord[0].st);
   vec4 heightS  = texture2D(Height, gl_TexCoord[0].st + vec2(StepSize,  0.0));
   vec4 heightT  = texture2D(Height, gl_TexCoord[0].st + vec2(0.0,       StepSize));
   
   // Calculate the change in height in both texture directions
   
   float pertS   = height.r - heightS.r;
   float pertT   = height.r - heightT.r;
   
   vec3 vTangentSpaceNormal = normalize(vec3(Bumpiness*pertS, Bumpiness*pertT, 1.0));
   
   // Normal to view space
   
   vec3 vViewSpaceNormal = rotation * vTangentSpaceNormal;
   
   // Phong Illumination Model
   
   vec3 color = (AmbientComponent() + DiffuseComponent(vViewSpaceNormal)) * texel.rgb
              + SpecularComponent(vViewSpaceNormal);
  
   // Final color
   
   gl_FragColor = vec4(color, Opacity*texel.a);
}