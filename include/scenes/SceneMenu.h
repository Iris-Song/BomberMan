#pragma once

#include <vector>
#include "../utils/useGlm.h"

#include "SceneManager.h"
#include "drawScene.h"

/**
 * @brief Scene object to re-implement in all scenes for menu
 */
class SceneMenu {
	public:
		// Constructors
		SceneMenu(Gui * gui, float const &dtTime);
		virtual ~SceneMenu();
		SceneMenu(SceneMenu const &src);

		// Operators
		SceneMenu &operator=(SceneMenu const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneMenu& myClass);

		// Scene methods
		virtual bool		init() = 0;
		virtual bool		update();
		virtual bool		draw();
		virtual void		load();
		
	protected:

		Gui* _gui;
		float const& _dtTime;
		
};
