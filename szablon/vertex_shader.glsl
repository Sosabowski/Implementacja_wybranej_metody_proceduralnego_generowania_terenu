#version 330 

layout (location = 0) in vec3 bPos;

uniform mat4 MVP;

out vec3 fragColor; // Przekazanie koloru do fragment shadera

void main()
{
    fragColor = vec3(0.0, 1.0, 0.0);
    gl_Position = MVP * vec4(bPos, 1.0);
}