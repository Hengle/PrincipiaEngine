#ifndef _LAYOUTS_GLSL
#define _LAYOUTS_GLSL

layout(binding = 1) uniform UBO
{
	vec3 pos;
	float aspectRatio;
	vec3 lookat;
	float fov;
} ubo;


layout(std140, binding = 2) buffer TriangleVerts
{
	TriangleVert verts[];
};

layout(std140, binding = 3) buffer TriangleIndices {
	TriangleIndex indices[];
};

layout(std140, binding = 4) buffer Meshs {
	Mesh meshes[];
};

layout(std140, binding = 5) buffer Primitives
{
	Primitive primitives[];
};

layout(std140, binding = 6) buffer Materials {
	Material materials[];
};

layout(std140, binding = 7) buffer Lights {
	Light lights[];
};


#endif