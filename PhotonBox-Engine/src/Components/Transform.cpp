#include <algorithm>
#include "Transform.h"
#include "../Core/GameObject.h"
#include "../Components/Camera.h"

Vector3f Transform::forward(){
	Matrix4f r = getRotationMatrix();
	return Vector3f(r(0, 2), r(1, 2), r(2, 2));
}

Vector3f Transform::up(){
	Matrix4f r = getRotationMatrix();
	return Vector3f(r(0, 1), r(1, 1), r(2, 1));
}

Vector3f Transform::right() {
	Matrix4f r = getRotationMatrix();
	return Vector3f(r(0, 0), r(1, 0), r(2, 0));
}

void Transform::removeChild(Transform * child){
	children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

void Transform::renderHandels()
{
	Matrix4f projectionMatrix = Camera::getMainCamera()->getProjectionMatrix();	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((const GLfloat*)&projectionMatrix(0, 0));
	glMatrixMode(GL_MODELVIEW);
	Matrix4f MV = Camera::getMainCamera()->getViewMatrix() * getTransformationMatrix(true, false, true);
	glLoadMatrixf((const GLfloat*)&MV(0, 0));
	
	Vector4f center = Vector4f::ZERO;
	Vector4f x = center + Vector4f::UNIT_X;
	Vector4f z = center + Vector4f::UNIT_Z;
	Vector4f y = center + Vector4f::UNIT_Y;

	glDepthFunc(GL_ALWAYS);

	glUseProgram(0);

	glBegin(GL_LINES);
		glColor3f(0, 0, 1);
		glVertex3fv(&center.x());
		glVertex3fv(&x.x());

		glColor3f(0, 1, 0);
		glVertex3fv(&center.x());
		glVertex3fv(&z.x());

		glColor3f(1, 0, 0);
		glVertex3fv(&center.x());
		glVertex3fv(&y.x());
	glEnd();
	glFinish();

	glDepthFunc(GL_LESS);
}

void Transform::print()
{
	std::cout << gameObject->name << std::endl;
	std::cout << "Position: " << getPosition() << std::endl;
	std::cout << "World Position: " << getPositionWorld() << std::endl;
	std::cout << "Rotation: " << getRotation() << std::endl;
	std::cout << "Scale: " << getScale() << std::endl;
	std::cout << "Transform: " << std::endl << getTransformationMatrix() << std::endl << std::endl;
}

Matrix4f Transform::getRotationMatrix(){
	return Matrix4f::createRotation(_rotation.z(), Vector3f::UNIT_Z) * Matrix4f::createRotation(_rotation.x(), Vector3f::UNIT_X) * Matrix4f::createRotation(_rotation.y(), Vector3f::UNIT_Y);
}

Vector3f Transform::getRotation(){
	return _rotation;
}


Vector3f Transform::getPosition(){
	return _position;
}

Vector3f Transform::getPositionWorld(){
	Matrix4f temp = getTransformationMatrix();
	return Vector3f(temp(3,0), temp(3, 1), temp(3, 2));
}

Vector3f Transform::getScale(){
	return _scale;
}

void Transform::setPosition(Vector3f position){
	if (!position.equals(_position)) {
		_position = position;
		_hasChanged = true;
	}
}

/*
void Transform::setRotation(Matrix4f rot) {
	_rotMat = rot;
	_hasChanged = true;
}
*/

void Transform::setRotation(Vector3f rotation){
	_rotation = rotation;
	_hasChanged = true;
}

void Transform::setScale(Vector3f scale){
	if (!scale.equals(_scale)) {
		_scale = scale;
		_hasChanged = true;
	}
}

void Transform::setParent(Transform *parent) {
	if (_parent != nullptr) {
		_parent->removeChild(this);
	}
	
	_parent = parent;
	
	if(parent != nullptr)
		parent->children.push_back(this);
}

void Transform::setParent(GameObject *_gameObject) {
	setParent(_gameObject->transform);
}

Matrix4f Transform::getTransformationMatrix() {
	return getTransformationMatrix(true, true, true);
}

Matrix4f Transform::getTransformationMatrix(bool rot, bool scale, bool trans){
	if (_parent != nullptr) 
		return (_parent->getTransformationMatrix(rot, scale, trans)) * getLocalTransformationMatrix(rot, scale, trans);
	else
		return getLocalTransformationMatrix(rot, scale, trans);
}

Matrix4f Transform::getLocalTransformationMatrix() {
	if (_hasChanged) {
		_cache = Matrix4f::IDENTITY;

		_cache = getRotationMatrix() * Matrix4f::createScaling(_scale);

		_cache(3, 0) = _position.x();
		_cache(3, 1) = _position.y();
		_cache(3, 2) = _position.z();

		_hasChanged = false;
	}

	return _cache;
}

Matrix4f Transform::getLocalTransformationMatrix(bool rot, bool scale, bool trans) {
	if (rot && scale && trans)
		return getLocalTransformationMatrix();

	_cache = Matrix4f::IDENTITY;
		
	if(rot)
		_cache = _cache * getRotationMatrix();
	
	if(scale)
		_cache = _cache * Matrix4f::createScaling(_scale);
	
	if (trans) {
		_cache(3, 0) = _position.x();
		_cache(3, 1) = _position.y();
		_cache(3, 2) = _position.z();
	}

	_hasChanged = true;

	return _cache;
}
