#version 330 core
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 atangent;

out vec3 tangent;
out vec3 lightDir;
out vec3 viewDir;


void main(){
    gl_Position = projection * view * vec4(aPos, 1.0);
    tangent = atangent;
    lightDir = lightPos - aPos;
    viewDir = cameraPos - aPos;
    
}
