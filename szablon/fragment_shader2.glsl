#version 330

out vec4 FragColor;

in vec4 outcol;


void main()
{

	gl_FragColor  = vec4(outcol.x, outcol.y,outcol.z, 0.4);
} 
