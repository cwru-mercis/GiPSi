varying vec3 vLightDir;
varying vec3 vHalfAngle;

attribute vec3 vTangent;

uniform vec3 LightPosition;

void main()
{
	vec3 pos = vec3( gl_ModelViewMatrix * gl_Vertex );
	vec4 eye_position = ( gl_ModelViewMatrix * gl_Vertex );

	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * vTangent);
	vec3 b = cross(n, t);

	vLightDir = normalize(vec3(LightPosition - pos));

	vec3 v;
	v.x = dot(vLightDir, t);
	v.y = dot(vLightDir, b);
	v.z = dot(vLightDir, n);

	vLightDir = normalize(v);

	v  = eye_position.xyz - gl_Vertex.xyz;
	vec3 temp_view_vector;
	temp_view_vector.x = dot( v, t );
	temp_view_vector.y = dot( v, b );
	temp_view_vector.z = dot( v, n );

	vHalfAngle = normalize(vLightDir - temp_view_vector);

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}