#include "Core.h"
#include "../../Resources/Config.h"
#include "../Display.h"
#include "../InputManager.h"
#include "../Profiler.h"
#include "../Time.h"
#include "Lighting.h"
#include "Logic.h"
#include "Physics.h"
#include "PostProcessing.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "UIRenderer.h"


bool Core::_isRunning;

void Core::init(std::map<std::string, Scene*>& sceneMap)
{
	std::cout << "==================================================" << std::endl;
	std::cout << "               INITIALIZING SYSTEMS" << std::endl << std::endl;

	// Load Config
	_config = new Config();
	_config->readConfig();

	// Core Systems
	_display = new Display();
	_inputManager = new InputManager();
	_time = new Time();

	// Subsystems
	_uiRenderer = new UIRenderer();
	_sceneManager = new SceneManager();
	_renderer = new Renderer();
	_logic = new Logic();
	_physics = new Physics();
	_postPocessing = new PostProcessing();
	_lighting = new Lighting();
	_profiler = new Profiler();

	// Initialize OpenGL
	_display->init("PhotonBox Engine", Config::profile.width, Config::profile.height, Config::profile.fullscreen, Config::profile.vsync);

	_renderer->init(Config::profile.supersampling ? 2 : 1);
	_inputManager->init();
	_uiRenderer->init();

	std::cout << std::endl << "                   SYSTEMS READY" << std::endl;
	std::cout << "==================================================" << std::endl << std::endl;

	// Load Scenes
	_sceneManager->addScenes(sceneMap);
	//_sceneManager->addScene("Realistic Rendering", new TestScene());
	//_sceneManager->addScene("Material Test", new PBRScene());
	//_sceneManager->addScene("Demo Scene", new DemoScene());

	_sceneManager->loadSceneImediately("Realistic Rendering");

	// Start Subsystems
	start();
}

void Core::start()
{
	std::cout << "==================================================" << std::endl;
	std::cout << "            LOADING SCENE " << SceneManager::getCurrentName() << std::endl << std::endl;

	_logic->start();
	_renderer->start();
	_lighting->start();
	_postPocessing->start();

	std::cout << std::endl << "                   SCENE READY" << std::endl;
	std::cout << "==================================================" << std::endl << std::endl;
}

void Core::run()
{
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	double lastSecond = 0;
	_isRunning = true;

	std::string statPrint;

	while (_isRunning && _display->isRunning())
	{

		// Measure time
		double currentTime = glfwGetTime();
		_time->setDeltaTime(currentTime - lastTime);
		lastTime = currentTime;
		lastSecond += Time::deltaTime;

		if (lastSecond >= 1.0)
		{
			statPrint = std::to_string(nbFrames) + " FPS  -  " + std::to_string(1000.0f / double(nbFrames)).substr(0, 4) + "ms";
			Profiler::addFps(nbFrames);
			nbFrames = 0;
			lastSecond = 0;
		}

		// Update Logic
		_logic->update();

		// Late update Logic
		_logic->lateUpdate();

		// Update Physics
		_accumulatedTime += Time::deltaTime;
		if (_accumulatedTime > FIXED_TIME_INTERVAL)
		{
			_physics->update();
			_logic->fixedUpdate();
			_accumulatedTime = 0;
		}

		// Update input
		_inputManager->update();


		// Start Rendering
		FrameBuffer::resetDefaultBuffer();

		// Render gBuffers
		_renderer->prePass();

		// Render Scene
		Renderer::render();
		nbFrames++;

		_postPocessing->postProcess();



		// Gizmos
		_renderer->renderGizmos();

		// UI Rendering
		
		if(Config::profile.showFPS)
			_uiRenderer->renderText(statPrint, 10, Display::getHeight() - 20, 0.32f, Vector3f(0, 1, 0));
		if (Config::profile.fpsProfiling)
		{
			_uiRenderer->renderText("min: " + std::to_string(Profiler::getMinFps()) , 10, Display::getHeight() - 35, 0.32f, Vector3f(1, 0, 0));
			_uiRenderer->renderText("max: " + std::to_string(Profiler::getMaxFps()), 10, Display::getHeight() - 50, 0.32f, Vector3f(0, 1, 0));
			_uiRenderer->renderText("avg: " + std::to_string(Profiler::getAvgFps()), 10, Display::getHeight() - 65, 0.32f, Vector3f(0, 0, 1));
		}

		// Stop Rendering
		Display::swapBuffer();
		_renderer->clearDrawCalls();


		_inputManager->pollEvents();

		// End of Frame
		if (_sceneManager->sceneQueued())
		{
			_sceneManager->unloadScene(SceneManager::getCurrentScene());
			reset();
			_sceneManager->loadQueuedScene();
			start();
		}
	}
}

void Core::stop()
{
	_isRunning = false;
}

void Core::reset()
{
	_postPocessing->reset();
	_profiler->reset();
}

void Core::destroy()
{
	_logic->destroy();
	_renderer->destroy();
	_physics->destroy();
	_sceneManager->destroy();
	_display->destroy();

	delete _sceneManager;
	delete _renderer;
	delete _time;
	delete _logic;
	delete _physics;
	delete _postPocessing;
	delete _display;
	delete _lighting;
	delete _config;
}
