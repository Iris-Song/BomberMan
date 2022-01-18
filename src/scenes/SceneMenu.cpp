#include "../../include/scenes/SceneMenu.h"
#include "../../include/scenes/SceneGame.h"


SceneMenu::SceneMenu(Gui * gui, float const &dtTime)
 :_gui(gui),
  _dtTime(dtTime)
  {}


SceneMenu::~SceneMenu() {
}

SceneMenu::SceneMenu(SceneMenu const &src)
: _gui(src._gui),
  _dtTime(src._dtTime) {
	*this = src;
}

/* -------------------------------- Operators ----------------------------------*/

/**
 * @brief 拷贝rhs
 */
SceneMenu &SceneMenu::operator=(SceneMenu const &rhs) {
	return *this;
}


std::ostream &	operator<<(std::ostream & os, const SceneMenu& myClass) {
	(void)myClass;
	os << "<ASceneMenu object>";
	return os;
}

/* -------------------------------- Methods----------------------------------*/
/**
 * @brief 更新函数 每一帧(frame)更新一次
 *
 * @return 是否更新成功
 */
bool	SceneMenu::update() {
	
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
	scGame.updateForMenu();
	return true;
}

/**
 * @brief 绘图函数 每一帧(frame)更新一次
 *
 * @return 是否更新成功
 */
bool	SceneMenu::draw() {	
	return  true;
}

void SceneMenu::load() {
	_gui->enableCursor(true);
}

