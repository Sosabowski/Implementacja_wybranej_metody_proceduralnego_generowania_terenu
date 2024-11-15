#version 330

out vec4 FragColor;
in vec2 texcoord0;
in vec4 outcol;
uniform sampler2D tex0;



void main()
{
	vec4 blend_text;
	blend_text = texture( tex0, texcoord0 );
	blend_text[3]=0.5;
	gl_FragColor = blend_text;

} 

