#include <windows.h>
#include <chrono>

#include "../../include/scenes/SceneManager.h"
#include "../../include/bomberman.h"

/* import all scenes */
#include "../../include/scenes/SceneMainMenu.h"
#include "../../include/scenes/SceneGame.h"
#include "../../include/scenes/SceneGameOver.h"
#include "../../include/scenes/SceneVictory.h"


/**
 * @brief Construct a new Scene Manager:: Scene Manager object
 */
SceneManager::SceneManager()
	:_isInit(false),
	_gui(nullptr),
	_dtTime(0.0f),
	_scene(SceneNames::GAME),
	_fps(60)
{}


SceneManager::SceneManager(SceneManager const& src) {
	*this = src;
}


SceneManager::~SceneManager() {

}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return SceneManager& A reference to the copied object
 */
SceneManager& SceneManager::operator=(SceneManager const& rhs) {
	(void)rhs;
	logErr("You should never call copy operator for SceneManager");
	return *this;
}

/**
 * @brief get the SceneManager
 *
 * @return SceneManager& the instance of the SceneManager
 */
SceneManager& SceneManager::get() {
	static SceneManager	instance;
	return instance;
}

/**
 * @brief init the SceneManager (load the first scene& load gui)
 *
 * @return false if failure
 */
bool SceneManager::init() {
	return SceneManager::get()._init();
}
/**
 * @brief init the SceneManager (load the first scene& load gui)
 *
 * @return false if failure
 */
bool SceneManager::_init() {
	if (_sceneMap.find(SceneNames::MAIN_MENU) != _sceneMap.end()) {
		std::cout << "SceneManager::init already called";
		return false;
	}

	// GUI����
	_gui = new Gui();
	if (!_gui->init()) {
		return false;
	}

	/* ����MainMenu */
	_scene = SceneNames::MAIN_MENU;
	_sceneMap.insert(std::pair<std::string, SceneMenu*>(SceneNames::MAIN_MENU, new SceneMainMenu(_gui, _dtTime)));
	try {
		if (_sceneMap[_scene]->init() == false) {
			std::cout << "failed to init scene: mainMenu";
			return false;
		}
	}
	catch (std::exception const& e) {
		std::cout << "Error : " << e.what();
		return false;
	}

	try {
		_sceneMap[_scene]->load();  // load first scene
	}
	catch (std::exception const& e) {
		std::cout << "Error : " << e.what();
		return false;
	}

	/* draw */
	Inputs::update();
	_gui->preDraw();
	// draw debug menu scene
	if (_sceneMap[_scene]->draw() == false) {
		return false;
	}
	_gui->postDraw();

	// create and init all scene
	_sceneMap.insert(std::pair<std::string, SceneMenu*>(SceneNames::GAME, new SceneGame(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, SceneMenu*>(SceneNames::GAME_OVER, new SceneGameOver(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, SceneMenu*>(SceneNames::VICTORY, new SceneVictory(_gui, _dtTime)));
	

	for (auto it = _sceneMap.begin(); it != _sceneMap.end(); it++) {
		try {
			if (it->second->init() == false) {
				std::cout << "failed to init scene: " << it->first;
				return false;
			}
		}
		catch (std::exception const& e) {
			std::cout << "Error : " << e.what();
			return false;
		}
	}
	_isInit = true;
	return true;
}

/**
 * @brief this is the main game loop to update & draw everythings
 *
 * @return false if failure
 */
bool SceneManager::run() {
	//��ȡscreen��֡��
	float		maxFrameDuration = 1000.0 / s.j("screen").u("fps");
	return SceneManager::get()._run(maxFrameDuration);
}
/**
 * @brief this is the main game loop to update & draw everythings
 *
 * @return false if failure
 */
bool SceneManager::_run(float maxFrameDuration) {
	std::chrono::milliseconds	lastLoopMs = getMs();

	while (true) {
		/* reset variables */
		_sceneLoadedCurrentFrame = false;
		_dtTime = (getMs().count() - lastLoopMs.count()) / 1000.0;
		lastLoopMs = getMs();
		_fps = 1 / _dtTime;

		if (_sceneMap.find(_scene) == _sceneMap.end()) {
			std::cout << "invalid scene name: " << _scene << "\n";
		}
		else {
			/* update & draw scene */
			if (_update() == false)
				return false;
			if (_draw() == false)
				return false;
		}

		if (_gui->quit) {
			break;
		}

		/* fps control */
		std::chrono::milliseconds loopDuration = getMs() - lastLoopMs;
		float	frameDuration = loopDuration.count();

		if (frameDuration <= maxFrameDuration) {
			Sleep(maxFrameDuration - frameDuration);
		}

	}

	return true;
}

/**
 * @brief Update the current scene����scene
 *
 * @return false if error
 */
bool SceneManager::_update() {
	
	if (_sceneMap[_scene]->update() == false) {
		std::cout<<"Unexpected error when updating scene"<<"\n";
		return false;
	}

	//��ֹ���µ�ʱ���˳�
	if (isSceneChangedInCurFrame())
		_gui->disableExitForThisFrame();

	return true;
}

/**
 * @brief Draw the current scene
 *
 * @return false if error
 */
bool SceneManager::_draw() {
	/* draw */
	_gui->preDraw();

	// draw the scene
	if (_sceneMap[_scene]->draw() == false) {
		std::cout<<"Unexpected error when drawing scene"<<"\n";
		return false;
	}

	_gui->postDraw();

	return true;
}

/**
 * @brief load a scene from his name
 *
 * @param name the scene name
 * @return AScene* a pointer to the scene loaded
 */
SceneMenu* SceneManager::loadScene(std::string const& name) {
	return SceneManager::get()._loadScene(name);
}
/**
 * @brief load a scene from his name
 *
 * @param name the scene name
 * @return AScene* a pointer to the scene loaded
 */
SceneMenu* SceneManager::_loadScene(std::string const& name) {
	if (get()._sceneMap.find(name) == get()._sceneMap.end()) {
		logErr("invalid scene name: " << name << " in loadScene");
		return _sceneMap[_scene];
	}
	if (_scene == name) {
		return _sceneMap[_scene];
	}
	// scene changed, load the new
	_sceneLoadedCurrentFrame = true;
	try {
		_sceneMap[name]->load();  // load new scene (getScene return the name of the last scene)
	}
	catch (std::exception const& e) {
		logErr("Error: " << e.what());
	}
	_scene = name;
	return _sceneMap[_scene];
}

/**
 * @brief get a scene
 *
 * @param name the name of the scene to get
 * @return AScene* a pointer to the scene
 */
SceneMenu* SceneManager::getScene(std::string const& name) {
	return SceneManager::get()._getScene(name);
}
/**
 * @brief get a scene
 *
 * @param name the name of the scene to get
 * @return AScene* a pointer to the scene
 */
SceneMenu* SceneManager::_getScene(std::string const& name) {
	if (get()._sceneMap.find(name) == get()._sceneMap.end()) {
		logErr("invalid scene name: " << name << " in getScene");
		return _sceneMap[_scene];
	}
	return _sceneMap[name];
}

/**
 * @brief get the current scene name
 *
 * @return std::string const& the current scene name
 */
std::string const& SceneManager::getSceneName() {
	return SceneManager::get()._getSceneName();
}
/**
 * @brief get the current scene name
 *
 * @return std::string const& the current scene name
 */
std::string const& SceneManager::_getSceneName() const {
	return _scene;
}


/**
 * @brief Return if the scene has changed in the current frame
 *
 * @return true If the scene changed in the current frame
 */
bool SceneManager::isSceneChangedInCurFrame() {
	return SceneManager::get()._isSceneChangedInCurFrame();
}
/**
 * @brief Return if the scene has changed in the current frame
 *
 * @return true If the scene changed in the current frame
 */
bool SceneManager::_isSceneChangedInCurFrame() const {
	return _sceneLoadedCurrentFrame;
}


/**
 * @brief quit the game
 */
void SceneManager::quit() {
	SceneManager::get()._quit();
}
/**
 * @brief quit the game
 */
void SceneManager::_quit() {
	_gui->quit = true;
}

