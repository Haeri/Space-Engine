#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

class Scene;

#include <unordered_map>
#include <typeindex>
#include <iostream>
#include "../Components/Transform.h"
#include "Component.h"

class GameObject
{
public:
	Scene * parentScene;
	Transform* transform;
	std::string name;

	GameObject(Scene &_scene);
	GameObject(Scene &_scene, std::string _name);

	template <class T>
	T* addComponent()
	{
		if (_componentMap.find(typeid(T)) != _componentMap.end())
		{
			std::cerr << name << " GameObject already contains Component " << std::endl;
			return nullptr;
		}
		T* c = new T();
		c->setGameObject(this);
		c->setTransform(transform);
		_componentMap.insert(std::pair<std::type_index, Component*>(typeid(T), c));
		return c;
	}

	template<class T>
	T* getComponent()
	{
		if (_componentMap.find(typeid(T)) == _componentMap.end())
		{
			std::cerr << "Component was not found!" << std::endl;
			return nullptr;
		}
		return (T*)_componentMap[typeid(T)];
	}

	template <class T>
	void removeComponent()
	{
		T* c = getComponent<T>();
		_componentMap.erase(typeid(T));
		delete c;
	}

	void removeComponent(std::type_index ti)
	{
		Component *c = _componentMap[ti];
		c->destroy();
		_componentMap.erase(ti);
		delete c;
	}

	void destroyComponents();
	void destroy();
	int getId();
	void setEnable(bool enable);
	bool getEnable() { return _isEnabled; }
	void setStatic(bool _static);
	bool getStatic() { return _isStatic; }
	void printComponents();
private:
	static int _idCnt;
	bool _isEnabled;
	bool _isStatic;
	int _id;
	std::unordered_map<std::type_index, Component*> _componentMap;
};
#endif /* defined(GAME_OBJECT_H) */