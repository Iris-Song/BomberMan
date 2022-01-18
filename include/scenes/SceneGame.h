#ifndef SCENEGAME_HPP_
#define SCENEGAME_HPP_

#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include "../utils/useGlm.h"

#include "SceneMenu.h"
#include "drawScene.h"
#include "drawModelsTool.h"

#define NO_LEVEL -1  // value is no level loaded

class Player;
struct DrawMessage;

namespace GameState {
	/**
	 * @brief All possible game states (INTRO, PLAY, PAUSE, GAME_OVER, ...)
	 */
	enum Enum {
		INTRO,
		PLAY,
		PAUSE,
		GAME_OVER,
		WIN,
	};
}  // namespace GameState

/**
 * @brief This is the game Scene. In this scene, you can play to the game and load levels
 */
class SceneGame : public SceneMenu {
private:
	SceneGame();

	// Methods
	bool	_loadLevel(int32_t levelId);
	bool	_unloadLevel();

protected:

	DrawSceneTool _overTool;
	DrawSceneTool _victoryTool;
	DrawModelsTool modelsTool;

public:
	// Members
	Player						*player;  /**< The player */
	DrawMessage					*drawMessage;
	
	int32_t						level;  /**< The current level ID (-1 for no level) */
	GameState::Enum				state;  /**< Actual game state (PLAY, PAUSE, GAME_OVER, ...) */
	
	// Constructors
	SceneGame(Gui * gui, float const &dtTime);
	virtual ~SceneGame();
	SceneGame(SceneGame const &src);

	// Operators
	SceneGame &operator=(SceneGame const &rhs);

	// SceneGame methods
	virtual bool	init();
	virtual bool	update();
	virtual void	load();
	virtual bool	draw();
	bool			updateForMenu();
	bool			drawGame();
	bool			drawForMenu();
	bool			drawVictory();
	bool			drawGameOver();
	bool			drawEndGame();
	bool			loadLevel(int32_t levelId);
};

#endif  // SCENEGAME_HPP_
