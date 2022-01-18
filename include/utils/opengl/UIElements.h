#include "../../usingOpengl.h"

class RectUI {
	public:
		RectUI(glm::vec2 pos, glm::vec2 size);
		virtual ~RectUI();

		//RectUI & operator=(RectUI const & rhs);
        static void init();


	protected:
		virtual void	_update();
		virtual void	_draw();

        static Shader * _rectShader;
        static GLuint _rectVAO;
        static GLuint _rectVBO;
        static const float _rectVertices[12];
};