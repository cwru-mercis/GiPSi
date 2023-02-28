/*
===============================================================================//
   Simple Shading

   Authors: M. Cenk Cavusoglu

===============================================================================
*/
   
uniform bool HalfWayApprox;
//uniform vec4 c;

varying vec3 vNormal, vLight, vView, vHalfway;
// varying float dist;
varying vec4 diffuse, ambient;

/*
===============================================================================
   Simple Shading: Vertex Program
===============================================================================
*/

void main(void)
{
      vec4 ecPos;
      vec3 aux;
      
      /* first transform the normal into eye space and normalize the result */
      vNormal = normalize(gl_NormalMatrix * gl_Normal);
      
      /* now compute the light's direction */
      // Transform vertex position to view space
      ecPos = gl_ModelViewMatrix * gl_Vertex;
      // Compute light vector
      aux = vec3(gl_LightSource[0].position-ecPos);
      vLight = normalize(aux);
      //dist = length(aux);
      vView = vec3(ecPos);
   
      // Compute the halfway vector if the halfway approximation is used
      if(HalfWayApprox)
      {
         vHalfway = normalize(gl_LightSource[0].halfVector.xyz);
      }
      
      /* Compute the diffuse, ambient and globalAmbient terms */
      //diffuse = c * gl_LightSource[0].diffuse;
      diffuse = gl_Color * gl_LightSource[0].diffuse;
       
      /* The ambient terms have been separated since one of them */
      /* suffers attenuation */
      //ambient = c * gl_LightSource[0].ambient;
      ambient = gl_Color * gl_LightSource[0].ambient;
      //ambientGlobal = gl_LightModel.ambient * gl_Color;
         
       //gl_TexCoord[0] = gl_MultiTexCoord0;
      gl_Position = ftransform();

}