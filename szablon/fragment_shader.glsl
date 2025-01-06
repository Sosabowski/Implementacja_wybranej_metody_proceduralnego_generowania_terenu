#version 330


in float height;

uniform int display;
uniform int top;

in vec3 Normal;
in vec3 FragPos;
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float lightStr;

void main()
{	
	// ambient
   float ambientStrength = 0.3;
   vec3 ambient = ambientStrength * lightColor;

	//diffuse 
	vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * lightStr/length(lightPos-FragPos);
	vec3 result = (ambient + diffuse) * objectColor;

	switch (display)
	{
		case 1:
			gl_FragColor  = vec4(0.0, 0.5, 0.0, 1.0);	
			break;
		case 2:
			gl_FragColor=vec4(result, 1.0);
			break;
		case 3:
			if((height/top)>0.4)
				gl_FragColor  = vec4(0.8,(1-0.7*height/top),0,1);
			else
				gl_FragColor  = vec4(2*(height/top),0.72,0,1);	
			break;
	
	}

} 

