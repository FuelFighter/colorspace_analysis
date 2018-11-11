#version 300 es
precision mediump float;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
uniform mat3 projection;
smooth out vec4 Color;
void main()
{
    gl_Position = vec4(clamp(projection*position, vec3(-1,-1,-1), vec3(1,1,1)) ,1.0);
    float tmpscalar = .4-(gl_Position.z)*.5;
    float scalar = clamp(tmpscalar, 0.0, 1.0);
    Color = vec4(scalar*color,1);
    //Color = vec4(color,1);
}
