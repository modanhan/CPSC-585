#include "Path.h"
#include "Game.h"


using namespace std;
using namespace glm;
using namespace physx;

const float thickness = 14;

const std::vector<glm::vec3> displacements{
	glm::vec3(-thickness,0,thickness),
	glm::vec3(thickness,0,thickness),
	glm::vec3(thickness,0,-thickness),
	glm::vec3(-thickness,0,-thickness)
};

Path::Path(int geometrySize) {
	size = geometrySize;
	offset = 0.0f;
	this->aventador = Game::getFront();
	cooldown = 0.1;
	nextGenTime = Time::time + cooldown;
	for (int i = 0; i < (size + 1) * 6; i++) {
		positions.push_back(vec3(0, 0.01, 0));
		normals.push_back(vec3(0, 1, 0));
		uvs.push_back(vec2(0, 0));
	}
	Graphics::initGeometry(&geometry);
	geometry.elementCount = positions.size();

	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*positions.size(), &positions[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, geometry.textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*uvs.size(), &uvs[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, geometry.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*normals.size(), &normals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Path::generate() {
	for (int i = 0; i < positions.size() - 6; i ++) {
		positions[i] = positions[i + 6];
	}
}

void Path::updateOffset(float &offset) {
	if (Game::didSwitchOccur()) {
		offset = 0;
	}
	else {
		offset += 0.001;
	}
}

void Path::update(mat4 parentTransform) {
	if (Time::time > nextGenTime) {
		generate();
	}

	aventador = Game::getFront();

	//increasing the offset decreases the length
	updateOffset(offset);
	vec3 rightOffset = aventador->wheelPos[0] + vec3(-offset,0,0);
	vec3 leftOffset = aventador->wheelPos[1] + vec3(offset,0,0);

	vec4 frw(rightOffset, 1);
	vec4 flw(leftOffset, 1);
	frw = aventador->transform*frw;
	frw.y = 0.01;
	flw = aventador->transform*flw;
	flw.y = 0.01;
	positions[positions.size() - 6] = positions[positions.size() - 12 + 2];
	positions[positions.size() - 5] = vec3(frw);
	positions[positions.size() - 4] = vec3(flw);
	positions[positions.size() - 3] = positions[positions.size() - 12 + 2];
	positions[positions.size() - 2] = positions[positions.size() - 12 + 1];
	positions[positions.size() - 1] = vec3(frw);

	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*positions.size(), &positions[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	setCenterPoints();
}

void Path::render(mat4 parentTransform) {
	Graphics::Render(&geometry, &Resources::defaultMaterial, parentTransform*transform);
}

float sign(vec2 point1, vec2 point2, vec2 point3) {
	return (point1.x - point3.x) * (point2.y - point3.y) - (point2.x - point3.x) * (point1.y - point3.y);
}

bool Path::pointInPath(float x, float y) {

	vec2 check, p1, p2, p3;
	bool b1, b2, b3;

	check = vec2(x, y);

	for (int i = 0; i < positions.size() - 1; i += 3) {
		p1.x = positions[i].x;
		p1.y = positions[i].z;
		p2.x = positions[i + 1].x;
		p2.y = positions[i + 1].z;
		p3.x = positions[i + 2].x;
		p3.y = positions[i + 2].z;

		b1 = sign(check, p1, p2) < 0.0f;
		b2 = sign(check, p2, p3) < 0.0f;
		b3 = sign(check, p3, p1) < 0.0f;

		if ((b1 == b2) && (b2 == b3)) {
			return true;
		}
	}
	return false;
}

float getcenter(float A, float B, float C) {
	return (A + B + C) / 3;
}

void Path::setCenterPoints() {
	centerPoints.clear();
	PxVec3 point;
	for (int i = 0; i < positions.size() - 1; i += 3) {
		point.x = getcenter(positions[i].x, positions[i + 1].x, positions[i + 2].x);
		point.y = 0.0f;
		point.z = getcenter(positions[i].z, positions[i + 1].z, positions[i + 2].z);
		centerPoints.push_back(point);
	}
}

