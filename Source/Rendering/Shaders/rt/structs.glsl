#ifndef _STRUCTS_GLSL
#define _STRUCTS_GLSL

struct Primitive
{
	mat4 world;
	vec3 center;
	int id;
	vec3 extents;
	int matID;
};

struct TriangleVert{
	vec3 pos;
	int _pad;
};

struct TriangleIndex{
	int v0;
	int v1;
	int v2;
	int id;
};

struct Mesh{
	int startIndex;
	int endIndex;
	int startVert;
	int endVert;
};

struct Material{
	vec3 diffuse;
	float reflective;

	float roughness;
	float transparency;
	float refractiveIndex;
	int textureID;
};

struct Light{
	vec3 pos;
	float intensity;
	vec3 color;
	int id;
};

struct flool{
	float t;
	bool b;
};

struct boxReturn{
	float t;
	vec3 tdd;
	float _pad;
	vec3 t1;
};

struct finalmaterial{
	vec3 color;
	float reflection;

	float roughness;
	float transparency;
	float refraction;
	float _pad;
};

struct sectID{
	uint type;
	int id;
	int cId;
};


#endif