#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 finalColor;  

uniform vec3 ambientLight; 
uniform int display;
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float lightStr;
uniform vec3 viewPos;

void main()
{
	//ambient
    float ambientStrength = 0.3;
	vec3 ambient = ambientStrength * lightColor;
    //finalColor = vec4(ambient, 1.0);

	//diffuse
	//vec3 norm = normalize(Normal);
	vec3 norm = Normal;
    vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * lightStr/length(lightPos-FragPos);


	//Specular
	float specularStrength = 0.2;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColor;

	//Result
	vec3 result = (ambient + diffuse + specular) * objectColor;

    switch (display)
	{
		case 1: //Siatka
			finalColor  = vec4(objectColor, 1.0);	
			break;
		case 2: //Finalnie bêdzie tutaj oœwietlenie tylko muszê jeszcze ogarn¹æ jak je dodaæ
			finalColor=vec4(result, 1.0);
			break;
	
	}


}