varying vec3 vLightDir;
varying vec3 vHalfAngle;

uniform sampler2D base;
uniform sampler2D height;

void main()
{
	vec4 BumpNorm = texture2D(height, gl_TexCoord[0].xy);
	vec4 DecolCol = texture2D(base, gl_TexCoord[0].st);

	float dDot = max(dot(BumpNorm.rgb, vLightDir), 0.0);
	float sDot = max(dot(BumpNorm.rgb, vHalfAngle), 0.0);

	float kd = 0.9;
	float ks = BumpNorm.a;
	float sexp = (BumpNorm.a * 20.0);

	vec3 litColor = (kd * DecolCol.rgb * dDot) + (ks * DecolCol.rgb * pow(sDot, sexp));

	gl_FragColor = vec4(litColor, DecolCol.a);
}