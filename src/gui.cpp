#include "../include/Gui.h"
#include "../include/utils/Logging.h"
#include "../include/utils/opengl/Inputs.h"
#include "../include/scenes/SceneManager.h"

 void framebuffer_size_callback(GLFWwindow* window, int width, int height)
 {
     glViewport(0, 0, width, height);
 }

 void processInput(GLFWwindow *window)
 {
     if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
         glfwSetWindowShouldClose(window, true);
 }


 bool Gui::initOpengl()
{
    /* 设置opengl绘图窗口参数 */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    // openGL双缓冲
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

     /* 绘制SDL窗口 */
     this->_win = SDL_CreateWindow("bomberman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
     if (this->_win == NULL)
     {
         std::cout << "Failed to create SDL window" << std::endl;
         return false;
     }

	// create opengl context
	this->_context = SDL_GL_CreateContext(_win);
	if (this->_context == 0) {
		logErr("while loading OpenGL: " << SDL_GetError());
		return false;
	}
	// init glad
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		logErr("while loading OpenGL: failed to init glad");
		return false;
	}

	// 开启深度测试
	glEnable(GL_DEPTH_TEST);

     return true;
 }

/* ----------------------------------- Gui ----------------------------------*/
Gui::Gui()
: quit(false)
{}

Gui::~Gui() {
}

Gui::Gui(Gui const &src)
: quit(src.quit) {
	*this = src;
}

Gui &Gui::operator=(Gui const &rhs) {
	return *this;
}

/**
 * @brief ��ʼ��GUI
 *
 * @return �Ƿ��ʼ���ɹ�
 */
bool	Gui::init() {
	std::cout<<"create gui\n";

	// ��ʼ��openGL����
    if (false == initOpengl())
        return -1;

	//天空盒

	this->cam = new Camera(glm::vec3(0.0f, 18.0f, 18.0f));
	this->cam->Front = glm::vec3(0.0f, -18.0f, -18.0f);

	this->cam->GetViewMatrix();

	this->_skybox = new Skybox();
	glm::mat4 projection = glm::perspective(glm::radians(this->cam->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	this->_skybox->setProjection(projection);
	
	return true;
}

// -- enableCursor -------------------------------------------------------------
/**
 * @brief enable or disable the cursor visibility
 *
 * @param enable enable boolean
 */
void Gui::enableCursor(bool enable) {
	if (enable) {
		SDL_ShowCursor(SDL_ENABLE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	else {
		SDL_ShowCursor(SDL_DISABLE);
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
}

/**
 * @brief In this frame, the update function cannot exit
 *
 * @param disable True to enable this functionality
 */
void	Gui::disableExitForThisFrame(bool disable) {
	_exitMenuDisabled = disable;
}

// -- draw ---------------------------------------------------------------------
/**
 * @brief call this function to do stuff before drawing scene
 */
void Gui::preDraw() {
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClearColor(0.2, 0.5, 0.95, 1.0);
}

/**
 * @brief call this function to do stuff after drawing scene
 */
void Gui::postDraw() {
	// swap buffer and check errors
	SDL_GL_SwapWindow(_win);

	// 交换缓冲，暂时没用sdl
	// glfwSwapBuffers(window);
    // glfwPollEvents();
}