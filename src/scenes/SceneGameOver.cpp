#include "../../include/scenes/SceneGameOver.h"
#include "../../include/scenes/SceneGame.h"


SceneGameOver::SceneGameOver(Gui * gui, float const &dtTime)
: SceneMenu(gui, dtTime),
  _lastSceneName(SceneNames::MAIN_MENU)
{
}


SceneGameOver::SceneGameOver(SceneGameOver const & src)
: SceneMenu(src)
{
	*this = src;
}


SceneGameOver::~SceneGameOver() {}


SceneGameOver & SceneGameOver::operator=(SceneGameOver const & rhs) {
	return *this;
}


bool			SceneGameOver::init() {

	//UI

	return true;
}


bool	SceneGameOver::update() {
	SceneMenu::update();
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
	
	scGame.updateForMenu();

	/* Cancan : trick,这里写死了贴图中按钮的位置 */
	/*
	RESTART
	左上：80 330
	左下：80 430
	右上：320 330
	右下：320 430

	END
	左上：80 466
	左下：80 569
	右上：320 466
	右下：320 569
	*/
	const int left_border = 80, right_border = 320;
	const int start_up = 330, start_down = 430;
	const int end_up = 466, end_down = 569;

	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_MOUSEBUTTONDOWN && SDL_BUTTON_LEFT == e.button.button) {
			int px = e.button.x;
			int py = e.button.y;
			//printf("x, y %d %d ...............\n", px, py);

			
			if (px >= left_border && px <= right_border) {
				if (py >= start_up && py <= start_down)
					_states.restart = true;
				else if (py >= end_up && py <= end_down)
					_states.exit = true;
			}
			

		}
		else if (e.type == SDL_QUIT) {
			_states.exit = true;
		}
	}

	if (_states.restart) {
		
		_states.restart = false;
		// reload the current level
		if (!scGame.loadLevel(scGame.level)) {
			return false;
		}
		SceneManager::loadScene(_lastSceneName);
	}
	else if (_states.menu) {
		
		_states.menu = false;
		SceneManager::loadScene(SceneNames::MAIN_MENU);
	}
	else if (_states.exit) {
		
		_states.exit = false;
		this->_gui->quit = true;
	}
	return true;
}

void SceneGameOver::load() {
	SceneMenu::load();
	if (SceneManager::getSceneName() != SceneNames::EXIT) {
		_lastSceneName = SceneManager::getSceneName();
	}
}


bool SceneGameOver::draw() {
	bool ret = true;

	/* 3d background */
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
	ret = scGame.drawGameOver();  // draw the game if possible
	
	ret = SceneMenu::draw();
	return ret & true;
}
