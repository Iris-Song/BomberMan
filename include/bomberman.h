#pragma once


#include <cstdlib>
#include<chrono>
#include "utils/SettingsJson.hpp"
#include "utils/Logging.h"
#include "utils/useGlm.h"

/*---------------------------------路径定义-------------------------------*/

#define CONFIG_DIR				"configs/"
#define SETTINGS_FILE			CONFIG_DIR"settings.json"
#define CONTROLS_FILE			CONFIG_DIR"controls.json"
#define MAP_DIR				    "assets/map/"

#define SCR_WIDTH 1066
#define SCR_HEIGHT 600

static const std::string	homeDir = "";
extern SettingsJson s;//通用设置

/*---------------------------------函数-------------------------------*/
int						    checkPrgm();
bool						initSettings(std::string const & filename);
std::chrono::milliseconds	getMs();
int                         bomberman();
int                         start();


