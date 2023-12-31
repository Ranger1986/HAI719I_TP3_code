#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;

//TODO create uniform transformations matrices Model View Projection
// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform float scale;
uniform vec3 translate;

void main(){

        // TODO : Output position of the vertex, in clip space : MVP * position
        gl_Position = MVP * vec4(vertices_position_modelspace+translate,scale);

}

