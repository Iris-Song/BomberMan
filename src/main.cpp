#include <iostream>
#include <fstream>
#include <windows.h>

//#include "../include/shader.h"
//#include "../include/gui.h"
#include "../include/bomberman.h"
#include "../include/usingOpengl.h"
#include "../include/utils/FileUtils.h"

#define NOTEST

#ifdef NOTEST

int main()
{
    // 重点在图形学效果，可以不做记录存储与读取

    // GLFW或GLAD初始化，失败返回NULL


    bomberman();

	std::cout << "finished" << std::endl;
    Sleep(1000);

    return 0;
}
#endif // NOTEST

#ifdef TEST

// test Code
#include <stdio.h>
#include <string.h>

extern "C"
{
#include "sdl2/SDL.h"
};
#include "camera.h"
#include "stb_image.h"
#include "gui.h"
#define main main

int main(int argc, char* argv[])
{
	Gui *gui = new Gui();
	gui->init();

	glEnable(GL_DEPTH_TEST);
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

	gui->cam = &camera;

	

	while (true) {
		gui->preDraw();

		gui->postDraw();

		SDL_Delay(10);
	}

	return 0;
}

#endif
 



