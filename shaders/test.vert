#version 460 core

layout (location = 0) in vec2 in_Position;
layout (location = 1) in vec3 in_Color;

layout (location = 0) out vec3 out_Color;


void main() {
	vec2 positions[3] = vec2[](
		vec2(0.0, -0.5),
		vec2(-0.5, 0.5),
		vec2(0.5, 0.5)
	);

	vec3 colors[3] = vec3[](
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 0.0, 1.0)
	);

	gl_Position = vec4(in_Position, 0.0, 1.0);
	out_Color = in_Color;
	//gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	//out_Color = colors[gl_VertexIndex];
}
