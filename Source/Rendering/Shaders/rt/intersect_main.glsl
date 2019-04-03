#ifndef _INTERSECT_MAIN_GLSL
#define _INTERSECT_MAIN_GLSL

#include "intersect_plane.glsl"
#include "intersect_box.glsl"
#include "intersect_sphere.glsl"
#include "intersect_mesh.glsl"
#include "intersect_cylinder.glsl"
#include "intersect_disk.glsl"

const uint TYPE_SPHERE = 0x00000001u;
const uint TYPE_BOX = 0x00000002u;
const uint TYPE_PLANE = 0x00000004u;
const uint TYPE_MESH = 0x00000008u;
const uint TYPE_CYLINDER = 0x00000010u;
const uint TYPE_DISK = 0x00000020u;

sectID intersect(in vec3 rayO, in vec3 rayD, inout float resT, inout vec3 norm)
{
	sectID id = sectID(0, -1, -1);

	for (int i = 0; i < primitives.length(); ++i) {
		if (primitives[i].id > 0) {
			mat4 invWorld = inverse(primitives[i].world);
			vec3 rdd = (invWorld*vec4(rayD, 0.0)).xyz / primitives[i].extents;
			vec3 roo = (invWorld*vec4(rayO, 1.0)).xyz / primitives[i].extents;

			flool tMesh = boundsIntersect(roo, rdd, primitives[i]);
			if (tMesh.b && (tMesh.t > EPSILON) && (tMesh.t < resT)) {
				for (int j = meshes[primitives[i].id].startIndex; j < meshes[primitives[i].id].endIndex; j++) {
					if (TRIINTERSECT) {
						flool tTri = triIntersect(roo, rdd, indices[j]);
						if (tTri.b) {
							if ((tTri.t > EPSILON) && (tTri.t < resT)) {
								id = sectID(TYPE_MESH, i, j);
								resT = tTri.t;
							}
						}
					}
					else {
						QuadIndex quad;
						quad.v[0] = indices[j].v0; quad.v[1] = indices[j].v1;
						quad.v[2] = indices[j].v2; quad.v[3] = indices[j].v2;
						flool tQuad = quadIntersect(roo, rdd, quad, norm);
						if (tQuad.b) {
							if ((tQuad.t > EPSILON) && (tQuad.t < resT)) {
								id = sectID(TYPE_MESH, i, j);
								resT = tQuad.t;
							}
						}
					}
				}
			}
		}//id > 0

		else if (primitives[i].id == -1) { //SPHERE INTERSECT
			float tSphere = sphereIntersect(rayO, rayD, primitives[i]);
			if ((tSphere > EPSILON) && (tSphere < resT))
			{
				id = sectID(TYPE_SPHERE, i, -1);
				resT = tSphere;
			}
		}
		else if (primitives[i].id == -2) { //BOX INTERSECT
			vec4 tBox = boxIntersect(rayO, rayD, primitives[i]);
			if (tBox.x > 0) {
				if ((tBox.x > EPSILON) && (tBox.x < resT)) {
					id = sectID(TYPE_BOX, i, -1);
					resT = tBox.x;
					norm = tBox.yzw;
				}
			}
		}
		else if (primitives[i].id == -3) { //CYLINDER INTERSECT
			vec4 tCylinder = cylinderIntersect(rayO, rayD, primitives[i]);
			if ((tCylinder.x > EPSILON) && (tCylinder.x < resT)) {
				id = sectID(TYPE_CYLINDER, i, -1);
				resT = tCylinder.x;
				norm = tCylinder.yzw;
			}
		}
		else if (primitives[i].id == -4) { //PLANE INTERSECT
			float tplane = planeIntersect(rayO, rayD, primitives[i]);
			if ((tplane > EPSILON) && (tplane < resT))
			{
				id = sectID(TYPE_PLANE, i, -1);
				resT = tplane;
			}
		}
		else if (primitives[i].id == -5) { //DISK INTERSECT
			float tDisk = diskIntersect(rayO, rayD, primitives[i]);
			if ((tDisk > EPSILON) && (tDisk < resT)) {
				id = sectID(TYPE_DISK, i, -1);
				resT = tDisk;
			}
		}
	}

	return id;
}

//rayO = pos, rayD = light, primitiveId = duh, t = t = length(lights[0].pos - pos);
float calcShadow(in vec3 rayO, in vec3 rayD, in sectID primitiveId, inout float t)
{
	for (int i = 0; i < primitives.length(); ++i) {
		if (primitives[i].id > 0) {/////-----MESH-----|||||
			mat4 invWorld = inverse(primitives[i].world);
			vec3 rdd = (invWorld*vec4(rayD, 0.0)).xyz / primitives[i].extents;
			vec3 roo = (invWorld*vec4(rayO, 1.0)).xyz / primitives[i].extents;

			flool tMesh = boundsIntersect(roo, rdd, primitives[i]);
			if (tMesh.b && (tMesh.t > EPSILON) && (tMesh.t < t)) {
				for (int j = meshes[primitives[i].id].startIndex; j < meshes[primitives[i].id].endIndex; j++) {
					if (TRIINTERSECT) {
						flool tTri = triIntersect(roo, rdd, indices[j]);
						if (tTri.b) {
							if ((tTri.t > EPSILON) && (tTri.t < t)) {
								t = tTri.t;
								return SHADOW;
							}
						}
					}
					else
					{
						QuadIndex quad;
						quad.v[0] = indices[j].v0; quad.v[1] = indices[j].v1;
						quad.v[2] = indices[j].v2; quad.v[3] = indices[j].v2;
						vec3 normal = vec3(0, 1, 0);
						flool tQuad = quadIntersect(roo, rdd, quad, normal);
						if (tQuad.b) {
							if ((tQuad.t > EPSILON) && (tQuad.t < t)) {
								t = tQuad.t;
								return SHADOW;
							}
						}
					}
				}
			}
		}
		else if (primitives[i].id == -1) { /////-----SPHERE-----|||||
			float tSphere = sphereIntersect(rayO, rayD, primitives[i]);
			if ((tSphere > EPSILON) && (tSphere < t)) {
				t = tSphere;
				return SHADOW;
			}
		}
		else if (primitives[i].id == -2) { /////-----BOX-----|||||
			float tBox = boxIntersect(rayO, rayD, primitives[i]).x;
			if ((tBox > EPSILON) && (tBox < t))
			{
				t = tBox;
				return SHADOW;
			}
		}
		else if (primitives[i].id == -3) { /////-----CYLINDER-----|||||
			float tcylinder = cylinderIntersect(rayO, rayD, primitives[i]).x;
			if ((tcylinder > EPSILON) && (tcylinder < t))
			{
				t = tcylinder;
				return SHADOW;
			}
		}
		/*
		else if(primitives[i].id == -4){ /////-----PLANE-----|||||
			float tPlane = planeIntersect(rayO, rayD, primitives[i]);
			if((tPlane > EPSILON) && (tPlane < t)){
				t = tPlane;
				return SHADOW;
			}
		}
		else if(primitives[i].id == -5){ /////-----DISK-----|||||
			float tDisk = diskIntersect(rayO,rayD, primitives[i]);
			if((tDisk > EPSILON) && (tDisk < t)){
				t = tDisk;
				return SHADOW;
			}
		}*/
	}
	return 1.0;
}


#endif
