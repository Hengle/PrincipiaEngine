#pragma once
#include "includes.h"


#ifndef maxVal
#define maxVal(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef minVal
#define minVal(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define CCAST reinterpret_cast<char*>


int UID = 0;
int newUniqueID() {
	UID++;
	int time = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
	return time + UID;
};
auto cmp = [](std::pair<std::string, int> const & a, std::pair<std::string, int> const & b)
{
	return a.second != b.second ? a.second < b.second : a.first < b.first;
};

//Found from: https://stackoverflow.com/questions/29184311/how-to-rotate-a-skinned-models-bones-in-c-using-assimp
//inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
//{
//	glm::mat4 to;
//
//	to[0][0] = (glm::float32)from->a1; to[0][1] = (glm::float32)from->b1;  to[0][2] = (glm::float32)from->c1; to[0][3] = (glm::float32)from->d1;
//	to[1][0] = (glm::float32)from->a2; to[1][1] = (glm::float32)from->b2;  to[1][2] = (glm::float32)from->c2; to[1][3] = (glm::float32)from->d2;
//	to[2][0] = (glm::float32)from->a3; to[2][1] = (glm::float32)from->b3;  to[2][2] = (glm::float32)from->c3; to[2][3] = (glm::float32)from->d3;
//	to[3][0] = (glm::float32)from->a4; to[3][1] = (glm::float32)from->b4;  to[3][2] = (glm::float32)from->c4; to[3][3] = (glm::float32)from->d4;
//
//	return to;
//}

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2
#define W_AXIS 3

glm::mat4 axisChange(const glm::mat4& from) {
	glm::mat4 to;

	to[0][X_AXIS] = from[0][X_AXIS];
	for (int i = 0; i < 4; ++i) {
		to[i][X_AXIS] = from[i][X_AXIS];
		to[i][Y_AXIS] = from[i][Y_AXIS];
		to[i][Z_AXIS] = from[i][Z_AXIS];
		to[i][W_AXIS] = from[i][W_AXIS];
	}

	return to;
}

glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from)
{
	glm::mat4 to;

	to[0][0] = (glm::float32)from.a1;	to[1][0] = (glm::float32)from.b1;	to[2][0] = (glm::float32)from.c1;	to[3][0] = (glm::float32)from.d1;
	to[0][1] = (glm::float32)from.a2;	to[1][1] = (glm::float32)from.b2;	to[2][1] = (glm::float32)from.c2;	to[3][1] = (glm::float32)from.d2;
	to[0][2] = (glm::float32)from.a3;	to[1][2] = (glm::float32)from.b3;	to[2][2] = (glm::float32)from.c3;	to[3][2] = (glm::float32)from.d3;
	to[0][3] = (glm::float32)from.a4;	to[1][3] = (glm::float32)from.b4;	to[2][3] = (glm::float32)from.c4;	to[3][3] = (glm::float32)from.d4;

	return to;
	//return axisChange(to);
}

glm::vec4 aiQuatToGLM(const aiQuaternion& from) {
	glm::vec4 to;
	to.x = from.x;
	to.y = from.y;
	to.z = from.z;
	to.w = from.w;
	return to;
}


enum class ShapeType {
	MESH,
	SPHERE,
	CYLINDER,
	BOX
};

ShapeType ShapeCheck(const std::string &name) {
	if ((name[0] == 'n') && (name[2] == '_')) {
		switch (name[1])
		{
		case 's':
			return ShapeType::SPHERE;
			break;
		//case 'b':
		//	return ShapeType::BOX;
		//	break;
		//case 'c':
		//	return ShapeType::CYLINDER;
		//	break;
		default:
			return ShapeType::MESH;
			break;
		}
	}
	else return ShapeType::MESH;
}

Shape ShapeCreate (const Mesh &m, const ShapeType &type){
	Shape ret;
	ret.center = m.center;
	ret.extents = m.extent;
	ret.name = m.name;
	ret.originalName = m.originalName;
	switch (type) {
		case ShapeType::SPHERE: {
			ret.type = -1;
			//if (m.extent.x != m.extent.y)
			//	ret.extents.x = m.extent.y;
			break;
		}
		case ShapeType::BOX: {
			ret.type = -2;
			break;
		}
		case ShapeType::CYLINDER: {
			ret.type = -3;
			break;
		}
	}
	return ret;
}

//so what you actually want to do is...
//find the..... average center and the max total extents 
//so you need 3 things. 1. totalMax (center + max) 2. avgCenter (centers avg'd together) 3. modelExtents = (totalMax - avgCenter)
//model center = avg center, model extents = extents;
bool ModelScaler(PrincipiaModel& m) {
	glm::mat4 world = glm::mat4(1);
	float maxE = -FLT_MAX;
	float minE = FLT_MAX;
	int jind;
	//glm::vec3 avgCenter;
	//so what you need is so that if the max is X then you get hte min of that, if Y then etc....
	for (size_t i = 0; i < m.meshes.size(); ++i) {
		for (int j = 0; j < 3; ++j) {
			float curr = m.meshes[i].extent[j] + m.meshes[i].center[j];
			if (curr > maxE) {
				jind = j;
				maxE = curr;
			}
		}
	}
	//now find the min
	for (size_t i = 0; i < m.meshes.size(); ++i) {
		minE = minVal(m.meshes[i].center[jind] - m.meshes[i].extent[jind], minE);
	}

	float ratE = (maxE - minE) * 0.5f;
	//compare it to size
	float ratio = 1 / ratE;

	//Scale it
	world = glm::scale(world, glm::vec3(ratio));

	//Scale the bounds
	for (int i = 0; i < m.meshes.size(); ++i) {
		m.meshes[i].center = glm::vec3(world * glm::vec4(m.meshes[i].center, 1.f));
		m.meshes[i].extent = glm::vec3(world * glm::vec4(m.meshes[i].extent, 1.f));
		for (int j = 0; j < m.meshes[i].vertices.size(); j++) {
			m.meshes[i].vertices[j].position = glm::vec3(world * glm::vec4(m.meshes[i].vertices[j].position, 1.f)) - m.meshes[i].center;
		}
		//avgCenter += m.meshes[i].center;
	}

	//Scale the shapes
	for (int i = 0; i < m.shapes.size(); ++i) {
		m.shapes[i].center  = glm::vec3(world * glm::vec4(m.shapes[i].center, 1.f));
		m.shapes[i].extents = glm::vec3(world * glm::vec4(m.shapes[i].extents, 1.f));
		//avgCenter += m.meshes[i].center;
	}

	//Transform the verts to fit the size

	//find the biggest of extents for the entire model as a whole.... should be 1? actually it shouldn't be that because the if you scaled two things...
	//find the avg center as well
	m.extents = glm::vec3(1.f);
	int totalSize = m.meshes.size() + m.shapes.size();
	//m.center = avgCenter / float(totalSize);
	
	return true;
}

//Jointify does a few things
/*
1. it converts all vertexes into bone space
2. it create a bone bounding box
*/
bool GetJointExtents(PrincipiaModel& m,  PrincipiaSkeleton& s) {
	for (auto &joint : s.joints) {
		glm::vec3 maxVert = -glm::vec3(FLT_MAX);
		glm::vec3 minVert = glm::vec3(FLT_MAX);

		for (auto jo : joint.jointObjs) {
			if (jo.objID > -1) {// aka if its not a shape
				//Get the mesh's center
				const glm::vec3& mCenter = m.meshes[jo.objID].center;
				
				//Get the face;
				const Face& mFace = m.meshes[jo.objID].faces[jo.faceID];

				//Get the Vertices
				for (int i = 0; i < 4; ++i) {
					const Vertex& v = m.meshes[jo.objID].vertices[mFace.v[i]];
					for (int c = 0; c < 3; c++) {
						maxVert[c] = maxVal(maxVert[c], v.position[c] + mCenter[c]);
						minVert[c] = minVal(minVert[c], v.position[c] - mCenter[c]);
					}
				}
			}
			else { //aka its a sphere or some other shape
				//Get the shape
				const Shape& mShape = m.shapes[jo.faceID];
				for (int c = 0; c < 3; c++) {
					maxVert[c] = maxVal(maxVert[c], mShape.center[c] + mShape.extents[c]);
					minVert[c] = minVal(minVert[c], mShape.center[c] - mShape.extents[c]);
				}
			}
		}

		joint.center = (minVert + maxVert) * 0.5f;
		joint.extents = (minVert - maxVert) * 0.5f;
	}

	return true;
}

bool ConvertJointVerts(PrincipiaModel& m,  PrincipiaSkeleton& s) {
	for (auto &joint : s.joints) {
	//for(std::vector<Joint>::iterator joint = s.joints.begin(); joint != s.joints.end(); ++joint){
		//create a map that keeps track of the times an vert has been visited
		//first = mesh id, second = face.index, result = if true index
		std::map<std::pair<int, int>, int> cjvMap;
		std::vector<Vertex> newVerts;	//the new vertex list
		std::vector<Face> newFaces;		//The new face list
		std::vector<int> meshIds;
		std::vector<Shape> newShapes;
		int newIndex = 0;				//This is iterated when a new vertex is found

		for (auto jo : joint.jointObjs) {
			if (jo.objID > -1) {// aka if its not a shape
				//Get the Face and convert the vertices to joint space
				const Face& mFace = m.meshes[jo.objID].faces[jo.faceID];
				Face newFace;
				for (int i = 0; i < 4; ++i) {
					//Make a key of hte mesh id and the face index
					std::pair<int, int> key = std::make_pair(jo.objID, mFace.v[i]);
					//check if its in the map, if not then insert it
					if (cjvMap.find(key) == cjvMap.end()) {
						cjvMap.insert({ key, newIndex });

						//convert the vertex so that it's relative to the joint
						Vertex v = m.meshes[jo.objID].vertices[mFace.v[i]];
						v.position -= joint.center;

						//push it to the new vertex list and let the joint object know its index
						newVerts.push_back(v);
						newFace.v[i] = newIndex;
						newIndex++;
					}
					else { //It's already in the map so just push that index in
						newFace.v[i] = cjvMap.find(key)->second;
					}
				}
				newFaces.push_back(newFace);
				meshIds.push_back(jo.objID);
			}
			else { //aka its a sphere or some other shape
				//Get the shape and convert to joint space
				Shape mShape = m.shapes[jo.faceID];
				mShape.center -= joint.center;
				newShapes.push_back(mShape);
			}
		}
		joint.verts = newVerts;
		joint.faces = newFaces;
		joint.meshIds = meshIds;
		joint.shapes = newShapes;
	}
	return true;
}

bool boneVerify(const aiScene* scene) {
	for (size_t c = 0; c < scene->mNumMeshes; c++) {
		if (scene->mMeshes[c]->HasBones())
			return true;
	}
	return false;
}
/*
Sort as you place something in
one way is to just use a map then flatten it out using an in order traversal
*/