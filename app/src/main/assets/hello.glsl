#type vertex
#version 330 core

layout (location = 0) in vec3 a_pos;

out vec4 out_vertex_color;

void main()
{
    gl_Position = vec4(a_pos, 1.0);
    out_vertex_color = vec4(0.5, 0.0, 0.0, 1.0);
}

#type pixel
#version 330 core
out vec4 out_pixel_color;

in vec4 out_vertex_color;

void main()
{
    out_pixel_color = out_vertex_color;
}