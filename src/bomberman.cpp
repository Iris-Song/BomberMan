#include <stdlib.h>
#include <chrono>

#include "../include/bomberman.h"
#include "../include/utils/Logging.h"
#include "../include/utils/FileUtils.h"
#include "../include/utils/opengl/Inputs.h"
#include "../include/scenes/SceneManager.h"

SettingsJson s;

/**
 * @brief 程序有效性检验
 *
 * @return 检验是否成功
 */
int	checkPrgm() {
	

	/* 所需文件 */
	std::vector<std::string> requiredFiles = {
		homeDir + "assets/map/map1.txt"
	};

	for (auto && it : requiredFiles) {
		if (file::isFile(it) == false) {
			std::cout << it << " file doesn't exist";
			return false;
		}
	}
	return true;
}

/**
 * @brief 创建初始设置
 *
 * @param filename 要读取的设置文件的文件名
 * @return 设置是否成功
 */
bool	initSettings(std::string const & filename) {
	// 暂时无要初始化的设置内容
	s.name("settings").description("main settings");

	s.add<SettingsJson>("screen");
		s.j("screen").add<uint64_t>("fps", 60).setMin(30).setMax(120).setDescription("framerate");

	/* font */
	s.add<SettingsJson>("fonts");
		s.j("fonts").add<SettingsJson>("base");
			s.j("fonts").j("base").add<std::string>("file", "bomberman-assets/fonts/snakebold.ttf")
				.setDescription("this is the main font");
			s.j("fonts").j("base").add<uint64_t>("size", 20).setMin(5).setMax(50)
				.setDescription("default size for the text");
		s.j("fonts").add<SettingsJson>("cheatcode");
			s.j("fonts").j("cheatcode").add<std::string>("file", "bomberman-assets/fonts/monaco.ttf")
				.setDescription("this is the font for cheatcode");
			s.j("fonts").j("cheatcode").add<uint64_t>("size", 6).setMin(5).setMax(50)
				.setDescription("default size for the text (cheatcode)");
		s.j("fonts").add<SettingsJson>("text");
			s.j("fonts").j("text").add<std::string>("file", "bomberman-assets/fonts/BalooPaaji2-Regular.ttf")
				.setDescription("this is the main font");
			s.j("fonts").j("text").add<uint64_t>("size", 10).setMin(10).setMax(50)
				.setDescription("default size for the text");

	/* colors */
	s.add<SettingsJson>("colors");
	// buttons
	s.j("colors").add<SettingsJson>("buttons");
		s.j("colors").j("buttons").add<uint64_t>("color", 0x647BCE).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("buttons").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("buttons-disable");
		s.j("colors").j("buttons-disable").add<uint64_t>("color", 0x94A1C7).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("buttons-disable").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("buttons-border");
		s.j("colors").j("buttons-border").add<uint64_t>("color", 0x181818).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("buttons-border").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);

	s.j("colors").add<SettingsJson>("red");
		s.j("colors").j("red").add<uint64_t>("color", 0xCF647A).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("red").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("orange");
		s.j("colors").j("orange").add<uint64_t>("color", 0xCF8466).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("orange").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("blue");
		s.j("colors").j("blue").add<uint64_t>("color", 0x647BCE).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("blue").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("blue-light");
		s.j("colors").j("blue-light").add<uint64_t>("color", 0x94A1C7).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("blue-light").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("white");
		s.j("colors").j("white").add<uint64_t>("color", 0xf8f4ef).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("white").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("black");
		s.j("colors").j("black").add<uint64_t>("color", 0x181818).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("black").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);

	// font color
	s.j("colors").add<SettingsJson>("font");
		s.j("colors").j("font").add<uint64_t>("color", 0x181818).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("font").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	// background color
	s.j("colors").add<SettingsJson>("background");
		s.j("colors").j("background").add<uint64_t>("color", 0x181818).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("background").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("bg-rect");
		s.j("colors").j("bg-rect").add<uint64_t>("color", 0xf8f4ef).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("bg-rect").add<uint64_t>("alpha", 0x55).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("bg-rect-border");
		s.j("colors").j("bg-rect-border").add<uint64_t>("color", 0xCF647A).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("bg-rect-border").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);

	// collider
	s.j("colors").add<SettingsJson>("collider");
		s.j("colors").j("collider").add<uint64_t>("color", 0x647BCE).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("collider").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("boundingBox");
		s.j("colors").j("boundingBox").add<uint64_t>("color", 0x155c2c).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("boundingBox").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);


	/* Folders */
	s.add<std::string>("mapsPath", "bomberman-assets/maps/").setDescription("Folder with all maps");
	s.add<std::string>("imgsUI", "bomberman-assets/imgs/UI/").setDescription("Folder with all UI images");
	s.add<std::string>("loadingImgs", "bomberman-assets/imgs/levels").setDescription("Folder with all loading images");
	s.add<std::string>("loadingSentences", "bomberman-assets/loading/load-sentences.txt")
		.setDescription("File with all loading sentences");

	/* Graphics */
	s.add<SettingsJson>("graphics");
	s.j("graphics").add<bool>("fullscreen", false).setDescription("Display the game on fullscreen or not.");
	s.j("graphics").add<bool>("fitToScreen", false).setDescription("The resolution fit to the screen size");
	s.j("graphics").add<int64_t>("width", 1200).setMin(800).setMax(2560).setDescription("The resolution's width.");
	s.j("graphics").add<int64_t>("height", 800).setMin(600).setMax(1440).setDescription("The resolution's height.");

	/* mouse sensitivity */
	s.add<double>("mouse_sensitivity", 0.1).setMin(0.0).setMax(3.0) \
		.setDescription("Camera mouse sensitivity.");


	/* Debug */
	s.add<SettingsJson>("debug").setDescription("All debug settings");
		s.j("debug").add<bool>("3d-menu", true).setDescription("Use 3D menu");
		s.j("debug").add<SettingsJson>("show").setDescription("All showables settings for command /debug");
			s.j("debug").j("show").add<bool>("baseBoard", true).setDescription("Show the base board");
			s.j("debug").j("show").add<bool>("terrain", true).setDescription("Show the terrain");
			s.j("debug").j("show").add<bool>("entity", true).setDescription("Show the entities (player & enemy)");
			s.j("debug").j("show").add<bool>("flyHeight", false).setDescription("Show the fly height");
			s.j("debug").j("show").add<bool>("movingCollider", false).setDescription("Show the collider of moving entities");
			s.j("debug").j("show").add<bool>("staticCollider", false).setDescription("Show the collider of static entities");
			s.j("debug").j("show").add<bool>("boundingBox", false).setDescription("Show the bounding box of all entities");

	try {
		if (file::isDir(filename)) {
			std::cout<<filename << " is not the settings file, it is a directory";
		}
		else if (s.loadFile(filename) == false) {
			// warning when loading settings
			return false;
		}
	}
	catch(SettingsJson::SettingsException const & e) {
		// file doesn't exist, create it at the end
		std::cout << "the file " << filename << " doesn't exist for now";
		return false;
	}
	return true;
}


/**
 * @brief Get the current time in ms
 *
 * @return std::chrono::milliseconds the ms object
 */
std::chrono::milliseconds getMs() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch());
}



/**
 * @brief 开始游戏，初始化路径
 *
 * @return 成功or失败
 */
int start() {
	
	srand(time(NULL));  // init random

	initSettings(homeDir+SETTINGS_FILE);  // create settings object

	if (!checkPrgm())  // check validity
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

/**
 * @brief 游戏主程序
 *
 * @return int Main exit value
 */
int bomberman() {
	/* 初始化 */
	int ret = start();
	if (ret != EXIT_SUCCESS) {
		return ret;
	}

	/* 初始化 scene manager */
	if (SceneManager::init() == false) {
		return EXIT_FAILURE;
	}

	/* run the game */
	if (SceneManager::run() == false) {
		return EXIT_FAILURE;
	}

	
	std::cout << "sucessfully done\n";
	return ret;
}

