/*
===============================================================================//
   Simple Shading

   Authors: M. Cenk Cavusoglu

===============================================================================
*/

uniform bool HalfWayApprox;
uniform float SpecularCoeff;
uniform float SpecularExp;

varying vec3 vNormal, vLight, vView, vHalfway;
varying vec4 diffuse, ambient;


/*
===============================================================================
   Simple Shading: Fragment Program
===============================================================================
*/

void main(void)
{

      vec3 n,halfV,viewV,ldir,reflection;
      vec4 color = vec4(0.0, 0.0, 0.0, diffuse.a );
      float NdotL,NdotHV,VdotL;
      //float att;
      
      /* a fragment shader can't write a varying variable, hence we need
      a new variable to store the normalized interpolated normal */
      n = normalize(vNormal);
      viewV= normalize(vView);
      ldir= normalize(vLight);
      
      /* compute the dot product between normal and normalized lightdir */
      NdotL = max(dot(n,ldir),0.0);
   

      if (NdotL > 0.0) {
         // att = 1.0 / (gl_LightSource[0].constantAttenuation + gl_LightSource[0].linearAttenuation * dist + gl_LightSource[0].quadraticAttenuation * dist * dist);
         color = (diffuse * NdotL + ambient);
        
         if(HalfWayApprox)
         {
         // Approximation to the specular reflection using the halfway vector
         halfV = normalize(vHalfway);
         NdotHV = max(dot(n,halfV),0.0);
         color += SpecularCoeff * gl_LightSource[0].specular * pow(NdotHV,SpecularExp);
         }
         else
         {
         // Conventional specular component
         reflection = normalize(reflect(ldir, n));
         VdotL=max(0.0, dot(viewV, reflection));
         color += SpecularCoeff * gl_LightSource[0].specular * pow(VdotL,SpecularExp);
         }
      }
   
      gl_FragColor = color;


}