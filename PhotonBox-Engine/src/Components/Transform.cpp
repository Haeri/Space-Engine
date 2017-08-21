#include <algorithm>
#include "Transform.h"
#include "../Core/GameObject.h"

Vector3f Transform::forward(){
	Matrix4f r = getRotationMatrix();
	return Vector3f(r(0, 2), r(1, 2), r(2, 2));
}

Vector3f Transform::up(){
	Matrix4f r = getRotationMatrix();
	return Vector3f(r(0, 1), r(1, 1), r(2, 1));
}

void Transform::removeChild(Transform * child){
	children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

void Transform::print()
{
	std::cout << gameObject->name << std::endl;
	std::cout << "Position: " << getPosition() << std::endl;
	std::cout << "Rotation: " << getRotation() << std::endl;
	std::cout << "Scale: " << getScale() << std::endl;
	std::cout << "Transform: " << std::endl << getTransformationMatrix() << std::endl << std::endl;
}

Matrix4f Transform::getRotationMatrix(){
	return Matrix4f::createRotation(_rotation.z(), Vector3f::UNIT_Z) *  Matrix4f::createRotation(_rotation.y(), Vector3f::UNIT_Y) * Matrix4f::createRotation(_rotation.x(), Vector3f::UNIT_X);
}

Vector3f Transform::getRotation()
{
	return _rotation;
}


Vector3f Transform::getPosition(){
	return _position;
}

Vector3f Transform::getPositionWorld()
{
	Matrix4f temp = getTransformationMatrix();
	return Vector3f(temp(0,3), temp(1, 3), temp(2, 3));
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

Matrix4f Transform::getTransformationMatrix(){
	if (_parent != nullptr) 
		return (_parent->getTransformationMatrix()) * getLocalTransformationMatrix();
	else
		return getLocalTransformationMatrix();
}

Matrix4f Transform::getLocalTransformationMatrix(){
	if (_hasChanged) {
		_cache = (getRotationMatrix() * Matrix4f::createScaling(_scale));
		
		_cache(0,3) = _position.x();
		_cache(1,3) = _position.y();
		_cache(2,3) = _position.z();
		_hasChanged = false;
	}

	return _cache;
}