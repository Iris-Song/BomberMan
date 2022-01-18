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
 * @brief ����rhs
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
 * @brief ���º��� ÿһ֡(frame)����һ��
 *
 * @return �Ƿ���³ɹ�
 */
bool	SceneMenu::update() {
	
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
	scGame.updateForMenu();
	return true;
}

/**
 * @brief ��ͼ���� ÿһ֡(frame)����һ��
 *
 * @return �Ƿ���³ɹ�
 */
bool	SceneMenu::draw() {	
	return  true;
}

void SceneMenu::load() {
	_gui->enableCursor(true);
}

