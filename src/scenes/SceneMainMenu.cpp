#include "../../include/scenes/SceneMainMenu.h"
#include "../../include/scenes/SceneGame.h"

#include "drawScene.h"

SceneMainMenu::SceneMainMenu(Gui * gui, float const &dtTime)
: SceneMenu(gui, dtTime)
{}


SceneMainMenu::SceneMainMenu(SceneMainMenu const & src)
: SceneMenu(src)
{
	*this = src;
}

SceneMainMenu::~SceneMainMenu() {}


SceneMainMenu & SceneMainMenu::operator=(SceneMainMenu const & rhs) {
	return *this;
}


/**
 * @brief ��ʼ��menu
 *
 * @return ��ʼ���Ƿ�ɹ�
 */
bool			SceneMainMenu::init() {
	//UI
	this->_tool.init("resource/scenes/home.jpg");

	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneMainMenu::load() {
	SceneMenu::load();
}

/**
 * @brief ���º��� ÿһ֡(frame)����һ��
 *
 * @return �Ƿ���³ɹ�
 */
bool	SceneMainMenu::update() {
	SceneMenu::update();

	/* Cancan : trick,这里写死了贴图中按钮的位置 */
	/*
	START
	左上：224 460
	左下：224 505
	右上：441 460
	右下：441 505

	END 
	左上：628 460
	左下：628 505
	右上：848 460
	右下：848 505
	*/
	const int up_border = 460, down_border = 505;
	const int start_left = 224, start_right = 441;
	const int end_left = 628, end_right = 848;
	
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_MOUSEBUTTONDOWN && SDL_BUTTON_LEFT == e.button.button) {
			int px = e.button.x;
			int py = e.button.y;
			//printf("x, y %d %d ...............\n", px, py);

			if (py >= up_border && py <= down_border) {
				if (px >= start_left && px <= start_right)
					_states.newGame = true;
				else if (px >= end_left && px <= end_right)
					_states.exit = true;
			}

		}
		else if (e.type == SDL_QUIT) {
			_states.exit = true;
		}
	}

	if (true) {
		if (_states.newGame) {
			_states.newGame = false;
			SceneManager::loadScene(SceneNames::GAME);

			/* Cancan : simplified */
			SceneGame& scGame = *reinterpret_cast<SceneGame*>(SceneManager::getScene(SceneNames::GAME));
			if (!scGame.loadLevel(1)) {
				return false;
			}
		}
		else if (_states.exit) {
			_states.exit = false;
			this->_gui->quit = true;
		}
	}

	return true;
}


bool SceneMainMenu::draw()
{
	this->_tool.draw();
	return true;
}