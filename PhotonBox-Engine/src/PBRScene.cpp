#include "PBRScene.h"
#include "Resources/BasicShader.h"
#include "Resources/ForwardShader.h"
#include "Resources/LitShader.h"
#include "PrinterScript.cpp"
#include "Components/MeshRenderer.h"
#include "Components/PointRenderer.h"
#include "Components/AxisRenderer.h"
#include "CameraController.h"
#include "Components/AmbientLight.h"

void PBRScene::Load() {

	// RESOURCES
	plane = OBJLoader::loadObj("./res/plane_big.obj");
	sphere = OBJLoader::loadObj("./res/sphere.obj");
	forwardShader_ = new ForwardShader();
	tex = new Texture("./res/grid.png", true);
	material2 = new Material(forwardShader_, tex);



	// CAMERA
	GameObject* cam = instanciate("Camera");
	cam->addComponent<Camera>();
	cam->getComponent<Transform>()->setPosition(Vector3f(0, 1, -10));
	cam->getComponent<Transform>()->setRotation(Vector3f(0, 0, 0));
	cam->addComponent<CameraController>();



	// LIGHTS
	GameObject* ambient = instanciate("Ambient");
	ambient->addComponent<AmbientLight>();
	ambient->getComponent<AmbientLight>()->color = Vector3f(0.3f, 0.3f, 0.3f);
	ambient->getComponent<AmbientLight>()->intensity = 1.0f;

	GameObject* sun = instanciate("Sun");
	sun->addComponent<DirectionalLight>();
	sun->getComponent<DirectionalLight>()->color = Vector3f(0.93f, 0.92f, 0.94f);
	sun->getComponent<DirectionalLight>()->direction = Vector3f(-1, 1, -1);
	sun->getComponent<DirectionalLight>()->intensity = 0.5f;
	//sun->setEnable(false);


	ForwardShader* forwardShaderMaster = new ForwardShader();

	// OBJECTS
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {

			ForwardShader* forwardShader = new ForwardShader();
			forwardShader->directionalLightShader->shininess = i + j * 4;
			forwardShader->pointLightShader->shininess = i + j * 4;
			material = new Material(forwardShader);
			GameObject* probe = instanciate("Sphere" + i + j);
			probe->getComponent<Transform>()->setPosition(Vector3f(i, 1, j));
			probe->getComponent<Transform>()->setScale(Vector3f(0.3f, 0.3f, 0.3f));
			probe->addComponent<MeshRenderer>();
			probe->getComponent<MeshRenderer>()->setMesh(sphere);
			probe->getComponent<MeshRenderer>()->setMaterial(material);
		}
	}

	GameObject* quad = instanciate("Quad");
	quad->getComponent<Transform>()->setPosition(Vector3f(0, -0.2f, 0));
	quad->getComponent<Transform>()->setScale(Vector3f(1, 1, 1));
	quad->addComponent<MeshRenderer>();
	quad->getComponent<MeshRenderer>()->setMesh(plane);
	quad->getComponent<MeshRenderer>()->setMaterial(material2);
}

void PBRScene::OnUnload() {
	delete plane;
	delete sphere;
	delete forwardShader_;
	delete tex;
	delete material2;
}
