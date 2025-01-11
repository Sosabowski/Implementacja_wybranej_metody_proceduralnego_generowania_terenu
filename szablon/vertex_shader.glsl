#version 330 

layout(location = 0) in vec3 aPos; // Lokalizacja wieszcho³ków
layout (location = 2) in vec3 aNormal; //Wektory normalne

uniform mat4 MVP;

out vec3 FragPos;
out vec3 Normal;
//out vec3 height;

void main()
{
    FragPos = aPos;
   // height=aPos.z;
    gl_Position = MVP * vec4(aPos, 1.0);
    Normal=normalize(aNormal);
    //Normal=aNormal;
}