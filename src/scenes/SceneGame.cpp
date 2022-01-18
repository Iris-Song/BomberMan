#include <stdlib.h>
#include <time.h>

#include "../../include/Player.h"
#include "../../include/scenes/SceneGame.h"
#include "../../include/bomberman.h"
#include "../../include/utils/FileUtils.h"


// -- Constructors -------------------------------------------------------------

/**
 * @brief Construct a new Scene Game:: Scene Game object
 *
 * @param gui A pointer on the gui object
 * @param dtTime A reference to the delta time
 */
SceneGame::SceneGame(Gui * gui, float const &dtTime)
: SceneMenu(gui, dtTime)
{
	player = nullptr;
	level = NO_LEVEL;
	state = GameState::PLAY;
	
}

/**
 * @brief Destroy the Scene Game:: Scene Game object
 */
SceneGame::~SceneGame() {
	_unloadLevel();  // delete all elements of current level if needed

	if (player != nullptr) {
		delete player;
	}
}

/**
 * @brief Construct a new Scene Game:: Scene Game object
 *
 * @param src The object to do the copy
 */
SceneGame::SceneGame(SceneGame const &src)
: SceneMenu(src) {
	*this = src;
}



/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return SceneGame& A reference to the copied object
 */
SceneGame &SceneGame::operator=(SceneGame const &rhs) {
	if ( this != &rhs ) {
		player = rhs.player;
		level = rhs.level;
		state = rhs.state;
	}
	return *this;
}



/**
 * @brief called when the scene is loaded
 */
void SceneGame::load() {
	if (state == GameState::WIN
	|| state == GameState::GAME_OVER) {
		state = GameState::PLAY;
	}
}

/**
 * @brief draw is called each frame to draw the Game Scene.
 *
 * @return true
 * @return false
 */
bool	SceneGame::draw() {
	if (level == NO_LEVEL)
		return drawForMenu();
	return drawGame();
}

/**
 * @brief Draw function if we are in a menu (no level loaded)
 *
 * @return false If failed
 */
bool	SceneGame::drawForMenu() {
	// draw skybox
	this->_gui->_skybox->draw(this->_gui->cam->GetViewMatrix());

	return true;
}

/**
 * @brief Draw function if we are in a level
 *
 * @return false If failed
 */
bool	SceneGame::drawGame() {

	// draw player
	if (this->drawMessage == NULL) {
		this->drawMessage = new DrawMessage();
		if (!this->drawMessage)
		{
			cout << "apply error!" << endl;
		}
	}
	this->player->GetDrawMessage(this->drawMessage);
	this->drawMessage->display();

	// draw skybox
	this->_gui->_skybox->draw(this->_gui->cam->GetViewMatrix());

	// draw models
	this->modelsTool.Render(this->_gui->cam, this->drawMessage);
	
	return true;
}

/**
 * @brief Draw function if we are in victory menu
 *
 * @return false If failed
 */
bool	SceneGame::drawVictory() {
	
	this->_victoryTool.draw();

	return true;
}

/**
 * @brief Draw function if we are in game over menu
 *
 * @return false If failed
 */
bool	SceneGame::drawGameOver() {
	
	this->_overTool.draw();

	return true;
}

/**
 * @brief Draw function if we are in endGame menu
 *
 * @return false If failed
 */
bool	SceneGame::drawEndGame() {
	
	this->_gui->_skybox->draw(this->_gui->cam->GetViewMatrix());

	return true;
}



/**
 * init game method.
 */
bool			SceneGame::init() {
	
	this->_overTool.init("resource/scenes/lose.jpg");
	this->_victoryTool.init("resource/scenes/win.jpg");
	this->modelsTool.init(this->_gui->cam);

	return true;
}


/**
 * @brief load a level by ID
 *
 * @param levelId the level ID
 * @return true if the level loading is a success
 * @return false if the level loading failed
 */
bool SceneGame::loadLevel(int32_t levelId) {
	if (_unloadLevel() == false) {
		level = NO_LEVEL;
		return false;
	}
	std::cout<<"load level " << levelId<<"\n";
	bool result = _loadLevel(levelId);

	state = GameState::INTRO;
	return result;
}


/**
 * @brief Unload data of level.
 *
 * @return true if succeed
 * @return false
 */
bool	SceneGame::_unloadLevel() {
	if (level == NO_LEVEL)
		return true;
	std::cout<<"Unload level " << level;
	
	// Delete old player
	delete player;
	player = nullptr;

	
	level = NO_LEVEL;
	return true;
}

/**
 * @brief Load Level method. Throw a SceneException if the level is incomplete.
 *
 * @param levelId
 * @return true
 * @return false
 */
bool	SceneGame::_loadLevel(int32_t levelId) {
	if (levelId == NO_LEVEL)
		return true;
	if (levelId > 10) {//�������ؿ���
		std::cout<<"unable to load level " << levelId << ": doesn't exist"<<"\n";
		return false;
	}
	string filepath = homeDir+ MAP_DIR+"//"+"map"+ to_string(levelId)+".txt";
	level = levelId;  // save new level ID
	player = new Player(filepath);

	return true;
}




/**
 * @brief Update function to be called in menu
 *
 * @return false If failed
 */
bool	SceneGame::updateForMenu() {
	return true;
}
void gotoxy1(int x, int y) {
	COORD pos = { (SHORT)x,(SHORT)y };
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOut, pos);
}
/**
 * @brief update is called each frame.
 * @brief update is called each frame.
 *
 * @return true
 * @return false
 */
bool	SceneGame::update() {
	
	//�߼�update
	if (player->Update()) {
		if (player->IsWin())
			state = GameState::WIN;
		else
			state = GameState::GAME_OVER;
	}
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	GetConsoleCursorInfo(hOut, &info);
	info.bVisible = false;
	SetConsoleCursorInfo(hOut, &info);
	gotoxy1(0, 0);
	player->Display();
	
	if (Inputs::getKeyUp(InputType::CANCEL)) {
		_gui->disableExitForThisFrame();
	}

	 if (state == GameState::INTRO) {
		if ( Inputs::getKeyUp(InputType::CONFIRM) || Inputs::getKeyUp(InputType::ACTION)
			|| Inputs::getKeyUp(InputType::CANCEL)) {
			state = GameState::PLAY;
		}
		return true;
	}
	else if (state == GameState::WIN) {
		delete player;
		player = nullptr;
		SceneManager::loadScene(SceneNames::VICTORY);
		return true;
	}
	else if (state == GameState::GAME_OVER) {
		delete player;
		player = nullptr;
		SceneManager::loadScene(SceneNames::GAME_OVER);
		return true;
	}
	return true;
}

