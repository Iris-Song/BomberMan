/**************�޸ĸ�ͷ�ļ�·��*******************/
#include "usingOpengl.h"
#include "stb_image.h"

#include <iostream>
#include <vector>

/**************�޸Ĵ˴�shader�ļ�����պ�ͼƬ·��*******************/
#define SHADER_SKYBOX_VS "resource/skybox.vs"
#define SHADER_SKYBOX_FS "resource/skybox.fs"

#define SKYBOX_PATH	"resource/skybox/"

#ifdef JPG_SKY

#define SKYBOX_NAME_LEFT	"nx.jpg"
#define SKYBOX_NAME_RIGHT	"px.jpg"
#define SKYBOX_NAME_TOP		"py.jpg"
#define SKYBOX_NAME_BOTTOM	"ny.jpg"
#define SKYBOX_NAME_FRONT	"pz.jpg"
#define SKYBOX_NAME_BACK	"nz.jpg"

#else

#define SKYBOX_NAME_LEFT	"nx.png"
#define SKYBOX_NAME_RIGHT	"px.png"
#define SKYBOX_NAME_TOP		"py.png"
#define SKYBOX_NAME_BOTTOM	"ny.png"
#define SKYBOX_NAME_FRONT	"pz.png"
#define SKYBOX_NAME_BACK	"nz.png"

#endif

/*****************����ʾ��**************************/
/*
Skybox a;
a.setProjection(your projection matrix);
a.draw(current view matrix, nightprogress);
*/

class Skybox {
public:
	Skybox();
	Skybox(Skybox const& src);
	~Skybox();

	void setProjection(glm::mat4&projection);
	void load(std::vector<std::string>& faces);
	void draw(glm::mat4 view,float nightProgress = 0.0);

	Shader& getShader();
	Shader const& getShader() const;
	uint32_t		getTextureID() const;
private:
	Shader		_shader;  //skybox shader
	uint32_t	_textureID; //texture id
	uint32_t	_vao; 
	uint32_t	_vbo; 

	static const float _vertices[];
};
