#include "../../../include/utils/opengl/Inputs.h"
#include "../../../include/utils/Logging.h"
#include "../../../include/bomberman.h"

const std::string	Inputs::input_type_name[] = {
	"up",
	"down",
	"left",
	"right",
	"action",
	"action_2",
	"confirm",
	"cancel",
	"menu",
	"show help",
	"cheatcode",
};
const SDL_Scancode	Inputs::default_keys[] = {
	DEFAULT_UP,
	DEFAULT_DOWN,
	DEFAULT_LEFT,
	DEFAULT_RIGHT,
	DEFAULT_ACTION,
	DEFAULT_ACTION_2,
	DEFAULT_CONFIRM,
	DEFAULT_CANCEL,
	DEFAULT_MENU,
	DEFAULT_HELP,
	DEFAULT_CHEATCODE,
};
const std::string	Inputs::configFile = homeDir+CONTROLS_FILE;

/**
 * @brief Construct a new Inputs:: Inputs object
 */
Inputs::Inputs(): _configuring(false), _quit(false), _scroll_rel(0, 0),
	_left_click(false), _right_click(false),
	_left_click_previous(false), _right_click_previous(false), _isTextInputMode(false)
{
	for (int i = 0; i < Inputs::nb_input; i++) {
		_key_status[i] = false;
		_key_previous_status[i] = false;
	}
	_controls.name("controls").description("controls settings");
	_controls.add<SettingsJson>("keys");
	_controls.j("keys").add<int64_t>("up", DEFAULT_UP) \
		.setMin(4).setMax(286).setDescription("move up.");
	_controls.j("keys").add<int64_t>("down", DEFAULT_DOWN) \
		.setMin(4).setMax(286).setDescription("move down.");
	_controls.j("keys").add<int64_t>("left", DEFAULT_LEFT) \
		.setMin(4).setMax(286).setDescription("move left.");
	_controls.j("keys").add<int64_t>("right", DEFAULT_RIGHT) \
		.setMin(4).setMax(286).setDescription("move right.");
	_controls.j("keys").add<int64_t>("action", DEFAULT_ACTION) \
		.setMin(4).setMax(286).setDescription("action command.");
	_controls.j("keys").add<int64_t>("action_2", DEFAULT_ACTION_2) \
		.setMin(4).setMax(286).setDescription("action command bonus.");
	_controls.j("keys").add<int64_t>("confirm", DEFAULT_CONFIRM) \
		.setMin(4).setMax(286).setDescription("confirm choice.");
	_controls.j("keys").add<int64_t>("cancel", DEFAULT_CANCEL) \
		.setMin(4).setMax(286).setDescription("cancel choice.");
	_controls.j("keys").add<int64_t>("menu", DEFAULT_MENU) \
		.setMin(4).setMax(286).setDescription("go to menu (buttons).");
	_controls.j("keys").add<int64_t>("show help", DEFAULT_HELP) \
		.setMin(4).setMax(286).setDescription("show shortcuts fo buttons");
	_controls.j("keys").add<int64_t>("cheatcode", DEFAULT_CHEATCODE) \
		.setMin(4).setMax(286).setDescription("open cheat code command line");
	try {
		if (!_controls.loadFile(Inputs::configFile)) {
			logWarn("Invalid value in " << Inputs::configFile << ".");
		}
	}
	catch(SettingsJson::SettingsException const & e) {
		logDebug("the file " << Inputs::configFile << " doesn't exist for now.");
	}
	_input_key_map = {
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("up")), InputType::UP },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("down")), InputType::DOWN },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("left")), InputType::LEFT },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("right")), InputType::RIGHT },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("action")), InputType::ACTION },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("action_2")), InputType::ACTION_2 },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("confirm")), InputType::CONFIRM },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("cancel")), InputType::CANCEL },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("menu")), InputType::GOTO_MENU },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("show help")), InputType::SHOW_HELP },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("cheatcode")), InputType::CHEAT_CODE },
	};
	_used_scan = {
		_controls.j("keys").i("up"),
		_controls.j("keys").i("down"),
		_controls.j("keys").i("left"),
		_controls.j("keys").i("right"),
		_controls.j("keys").i("action"),
		_controls.j("keys").i("action_2"),
		_controls.j("keys").i("confirm"),
		_controls.j("keys").i("cancel"),
		_controls.j("keys").i("menu"),
		_controls.j("keys").i("show help"),
		_controls.j("keys").i("cheatcode"),
	};
	_controls.saveToFile(Inputs::configFile);

	_textInputIgnore = {  // list of keys that can be used out of textInput
		SDL_SCANCODE_RETURN,
		SDL_SCANCODE_TAB,
	};
}

/**
 * @brief Destroy the Inputs:: Inputs object
 */
Inputs::~Inputs() {}

/**
	Return a reference to singleton Inputs.

	@return the reference to the singleton.
*/
Inputs				&Inputs::get() {
	static Inputs		instance;
	return (instance);
}

/**
	Return the state of the key corresponding to the action passed.

	@param type The type defining the action to verify.
	@return The state of the key (true == pressed).
*/
bool				Inputs::getKey(InputType::Enum type) {
	return (Inputs::get()._getKey(type));
}
/**
	Return the state of the key corresponding to the action passed.

	@param type The type defining the action to verify.
	@return The state of the key (true == pressed).
*/
bool				Inputs::_getKey(InputType::Enum type) const {
	if (_ignoreInputType(type))
		return false;
	return _key_status[static_cast<int>(type)];
}

/**
	Indicate if the key corresponding to the action passed has just been released.

	@param type The type defining the action to verify.
	@return true if the key has been released.
*/
bool				Inputs::getKeyUp(InputType::Enum type) {
	return (Inputs::get()._getKeyUp(type));
}
/**
	Indicate if the key corresponding to the action passed has just been released.

	@param type The type defining the action to verify.
	@return true if the key has been released.
*/
bool				Inputs::_getKeyUp(InputType::Enum type) const {
	if (_ignoreInputType(type))
		return false;
	int		index = static_cast<int>(type);
	return (!_key_status[index] && _key_previous_status[index]);
}

/**
	Indicate if the key corresponding to the action passed has just been pressed.

	@param type The type defining the action to verify.
	@return true if the key has been pressed.
*/
bool				Inputs::getKeyDown(InputType::Enum type) {
	return (Inputs::get()._getKeyDown(type));
}
/**
	Indicate if the key corresponding to the action passed has just been pressed.

	@param type The type defining the action to verify.
	@return true if the key has been pressed.
*/
bool				Inputs::_getKeyDown(InputType::Enum type) const {
	if (_ignoreInputType(type))
		return false;
	int		index = static_cast<int>(type);
	return (_key_status[index] && !_key_previous_status[index]);
}

/**
 * @brief Get the name of a given key
 *
 * @param type The key
 * @return std::string The name of the key
 */
std::string			Inputs::getKeyName(InputType::Enum type) {
	return (Inputs::get()._getKeyName(type));
}
/**
 * @brief Get the name of a given key
 *
 * @param type The key
 * @return std::string The name of the key
 */
std::string			Inputs::_getKeyName(InputType::Enum type) const {
	for (auto it = _input_key_map.begin(); it != _input_key_map.end(); it++) {
		if (it->second == type) {
			if (it->first == NO_SCANCODE)
				return "no_scancode";
			return SDL_GetScancodeName(it->first);
		}
	}
	return "ERROR";
}

/**
	Configure the action passed to be associated to the next key pressed.
	This function will change the execution of the update function.

	@param type The type defining the action to configure.
*/
void				Inputs::configureKey(InputType::Enum type) {
	Inputs::get()._configureKey(type);
}
/**
	Configure the action passed to be associated to the next key pressed.
	This function will change the execution of the update function.

	@param type The type defining the action to configure.
*/
void				Inputs::_configureKey(InputType::Enum type) {
	_configuring = true;
	_next_action_type = type;
	_used_scan.erase(_controls.j("keys").i(input_type_name[_next_action_type]));
}

/**
	Cancel the wait for a key to configure an action.
	This function will reset the execution of the update function to its normal state.
*/
void				Inputs::cancelConfiguration() {
	Inputs::get()._cancelConfiguration();
}
/**
	Cancel the wait for a key to configure an action.
	This function will reset the execution of the update function to its normal state.
*/
void				Inputs::_cancelConfiguration() {
	if (_configuring) {
		_configuring = false;
		_used_scan.insert(_controls.j("keys").i(input_type_name[_next_action_type]));
	}
}

/**
 * @brief get the state of a key
 *
 * @param scancode the scancode of the key
 * @return true if the key is pressed
 * @return false if the key is not pressed
 */
bool				Inputs::getKeyByScancode(SDL_Scancode scancode) {
	return Inputs::get()._getKeyByScancode(scancode);
}
/**
 * @brief get the state of a key
 *
 * @param scancode the scancode of the key
 * @return true if the key is pressed
 * @return false if the key is not pressed
 */
bool				Inputs::_getKeyByScancode(SDL_Scancode scancode) const {
	if (_ingoreScancode(scancode))
		return false;
	return std::find(_scancodes_pressed.begin(), _scancodes_pressed.end(), scancode) != _scancodes_pressed.end();
}

/**
 * @brief know if a key was released
 *
 * @param scancode the scancode of the key
 * @return true if the key was just released
 * @return false in other cases
 */
bool				Inputs::getKeyByScancodeUp(SDL_Scancode scancode) {
	return Inputs::get()._getKeyByScancodeUp(scancode);
}
/**
 * @brief know if a key was released
 *
 * @param scancode the scancode of the key
 * @return true if the key was just released
 * @return false in other cases
 */
bool				Inputs::_getKeyByScancodeUp(SDL_Scancode scancode) const {
	if (_ingoreScancode(scancode))
		return false;
	auto before = std::find(_scancodes_previous.begin(), _scancodes_previous.end(), scancode) != _scancodes_previous.end();
	auto now = std::find(_scancodes_pressed.begin(), _scancodes_pressed.end(), scancode) != _scancodes_pressed.end();
	return !now && before;
}

/**
 * @brief know if a key was pressed
 *
 * @param scancode the scancode of the key
 * @return true if the key was just pressed
 * @return false in other cases
 */
bool				Inputs::getKeyByScancodeDown(SDL_Scancode scancode) {
	return Inputs::get()._getKeyByScancodeDown(scancode);
}
/**
 * @brief know if a key was pressed
 *
 * @param scancode the scancode of the key
 * @return true if the key was just pressed
 * @return false in other cases
 */
bool				Inputs::_getKeyByScancodeDown(SDL_Scancode scancode) const {
	if (_ingoreScancode(scancode))
		return false;
	auto before = std::find(_scancodes_previous.begin(), _scancodes_previous.end(), scancode) != _scancodes_previous.end();
	auto now = std::find(_scancodes_pressed.begin(), _scancodes_pressed.end(), scancode) != _scancodes_pressed.end();
	return now && !before;
}

/**
 * @brief Get the name of a given scancode
 *
 * @param scancode The scancode
 * @return std::string The name of the scancode
 */
std::string			Inputs::getScancodeName(SDL_Scancode scancode) {
	return (Inputs::get()._getScancodeName(scancode));
}
/**
 * @brief Get the name of a given scancode
 *
 * @param scancode The scancode
 * @return std::string The name of the scancode
 */
std::string			Inputs::_getScancodeName(SDL_Scancode scancode) const {
	if (scancode == NO_SCANCODE)
		return "no_scancode";
	return SDL_GetScancodeName(scancode);
}

/**
	Indicate if the window should be closed or not.

	@return true if the user tried to close the window, false otherwise.
*/
bool				Inputs::shouldQuit() {
	return (Inputs::get()._shouldQuit());
}
/**
	Indicate if the window should be closed or not.

	@return true if the user tried to close the window, false otherwise.
*/
bool				Inputs::_shouldQuit() const {
	return (_quit);
}

/**
	Gives the current coordinates of the mouse.

	@return A reference to a constant vector containing the mouse position.
*/
const glm::ivec2	&Inputs::getMousePos() {
	return (Inputs::get()._getMousePos());
}
/**
	Gives the current coordinates of the mouse.

	@return A reference to a constant vector containing the mouse position.
*/
const glm::ivec2	&Inputs::_getMousePos() const {
	return (_mouse_pos);
}

/**
	Gives the offset of the mouse from the last poll of event.

	@return A reference to a constant vector containing the mouse offset.
*/
const glm::ivec2	&Inputs::getMouseRel() {
	return (Inputs::get()._getMouseRel());
}
/**
	Gives the offset of the mouse from the last poll of event.

	@return A reference to a constant vector containing the mouse offset.
*/
const glm::ivec2	&Inputs::_getMouseRel() const {
	return (_mouse_rel);
}

/**
	Gives the offset of the mouse wheel from the last poll of event.

	@return A reference to a constant vector containing the mouse wheel offset.
*/
const glm::ivec2	&Inputs::getMouseScroll() {
	return (Inputs::get()._getMouseScroll());
}
/**
	Gives the offset of the mouse wheel from the last poll of event.

	@return A reference to a constant vector containing the mouse wheel offset.
*/
const glm::ivec2	&Inputs::_getMouseScroll() const {
	return _scroll_rel;
}

/**
	Gives the right click (true if clicked, else false)

	@return The mouse state
*/
bool				Inputs::getRightClick() {
	return (Inputs::get()._getRightClick());
}
/**
	Gives the right click (true if clicked, else false)

	@return The mouse state
*/
bool				Inputs::_getRightClick() const {
	return (_right_click);
}

/**
	Gives the left click (true if clicked, else false)

	@return The mouse state
*/
bool				Inputs::getLeftClick() {
	return (Inputs::get()._getLeftClick());
}
/**
	Gives the left click (true if clicked, else false)

	@return The mouse state
*/
bool				Inputs::_getLeftClick() const {
	return (_left_click);
}

/**
	Indicate if the right click has just been released.

	@return true if the right click has just been released.
*/
bool				Inputs::getRightClickUp() {
	return (Inputs::get()._getRightClickUp());
}
/**
	Indicate if the right click has just been released.

	@return true if the right click has just been released.
*/
bool				Inputs::_getRightClickUp() const {
	return (!_right_click && _right_click_previous);
}

/**
	Indicate if the left click has just been released.

	@return true if the left click has just been released.
*/
bool				Inputs::getLeftClickUp() {
	return (Inputs::get()._getLeftClickUp());
}
/**
	Indicate if the left click has just been released.

	@return true if the left click has just been released.
*/
bool				Inputs::_getLeftClickUp() const {
	return (!_left_click && _left_click_previous);
}

/**
	Indicate if the right click has just been pressed.

	@return true if the right click has just been pressed.
*/
bool				Inputs::getRightClickDown() {
	return (Inputs::get()._getRightClickDown());
}
/**
	Indicate if the right click has just been pressed.

	@return true if the right click has just been pressed.
*/
bool				Inputs::_getRightClickDown() const {
	return (_right_click && !_right_click_previous);
}

/**
	Indicate if the left click has just been pressed.

	@return true if the left click has just been pressed.
*/
bool				Inputs::getLeftClickDown() {
	return (Inputs::get()._getLeftClickDown());
}
/**
	Indicate if the left click has just been pressed.

	@return true if the left click has just been pressed.
*/
bool				Inputs::_getLeftClickDown() const {
	return (_left_click && !_left_click_previous);
}

/**
	Reset all the key to their default value.
*/
void				Inputs::resetKeys() {
	Inputs::get()._resetKeys();
}
/**
	Reset all the key to their default value.
*/
void				Inputs::_resetKeys() {
	logInfo("reset keys");
	for (auto i = 0; i < InputType::NB_INPUTS; i++) {
		_controls.j("keys").i(input_type_name[i]) = default_keys[i];
	}
	_input_key_map = {
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("up")), InputType::Enum::UP },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("down")), InputType::Enum::DOWN },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("left")), InputType::Enum::LEFT },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("right")), InputType::Enum::RIGHT },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("action")), InputType::Enum::ACTION },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("action_2")), InputType::Enum::ACTION_2 },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("confirm")), InputType::Enum::CONFIRM },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("cancel")), InputType::Enum::CANCEL },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("menu")), InputType::Enum::GOTO_MENU },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("show help")), InputType::Enum::SHOW_HELP },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("cheatcode")), InputType::Enum::CHEAT_CODE },
	};
	_used_scan = {
		_controls.j("keys").i("up"),
		_controls.j("keys").i("down"),
		_controls.j("keys").i("left"),
		_controls.j("keys").i("right"),
		_controls.j("keys").i("action"),
		_controls.j("keys").i("action_2"),
		_controls.j("keys").i("confirm"),
		_controls.j("keys").i("cancel"),
		_controls.j("keys").i("menu"),
		_controls.j("keys").i("show help"),
		_controls.j("keys").i("cheatcode"),
	};
	_controls.saveToFile(Inputs::configFile);
}

/**
	Poll all the SDL events from the last update call.
	This function should be called at the start of each frame in order for this class to have valid values.
*/
void				Inputs::update() {
	Inputs::get()._update();
}
/**
	Poll all the SDL events from the last update call.
	This function should be called at the start of each frame in order for this class to have valid values.
*/
void				Inputs::_update() {
	SDL_Event		event;
	SDL_Scancode	scan;

	_quit = false;
	_mouse_rel.x = 0;
	_mouse_rel.y = 0;
	_scroll_rel = glm::ivec2(0, 0);
	_left_click_previous = _left_click;
	_right_click_previous = _right_click;
	for (int i = 0; i < Inputs::nb_input; i++) {
		_key_previous_status[i] = _key_status[i];
	}
	_scancodes_previous = _scancodes_pressed;
	_currentText = "";
	_lastKeycode = NO_KEYCODE;
	std::vector<SDL_Scancode>::iterator elemScan;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_TEXTINPUT:
			_currentText = std::string(event.text.text);
			break;
		case SDL_KEYDOWN:
			_lastKeycode = event.key.keysym.sym;
			scan = event.key.keysym.scancode;
			elemScan = std::find(_scancodes_pressed.begin(), _scancodes_pressed.end(), scan);
			if (elemScan == _scancodes_pressed.end()) {  // if doesn't exist for now
				_scancodes_pressed.push_back(scan);
			}
			if (!_configuring) {
				try {
					int index = static_cast<int>(_input_key_map.at(scan));
					_key_status[index] = true;
				}
				catch(std::out_of_range const & oor) {
					// unused key
					continue;
				}
			}
			else {
				if (_used_scan.find(scan) == _used_scan.end()) {
					_input_key_map.erase(static_cast<SDL_Scancode>(_controls.j("keys").i(input_type_name[_next_action_type])));
					_controls.j("keys").i(input_type_name[_next_action_type]) = scan;
					_used_scan.insert(scan);
					_input_key_map[scan] = _next_action_type;
					_controls.saveToFile(Inputs::configFile);
					_configuring = false;
					// _key_status[static_cast<int>(_next_action_type)] = true;
					// _key_previous_status[_next_action_type] = true;
					logInfo("Input '" << input_type_name[_next_action_type] << "' set.")
				}
				else {
					logWarn("This key is already used.");
				}
			}
			break;
		case SDL_KEYUP:
			scan = event.key.keysym.scancode;
			elemScan = std::find(_scancodes_pressed.begin(), _scancodes_pressed.end(), scan);
			if (elemScan != _scancodes_pressed.end()) {  // if scancode exist -> remove it
				_scancodes_pressed.erase(elemScan);
			}
			try {
				int index = static_cast<int>(_input_key_map.at(scan));
				_key_status[index] = false;
			}
			catch(std::out_of_range const & oor) {
				// unused key
				continue;
			}
			break;
		case SDL_MOUSEMOTION:
			_mouse_pos.x = event.motion.x;
			_mouse_pos.y = event.motion.y;
			_mouse_rel.x = event.motion.xrel;
			_mouse_rel.y = event.motion.yrel;
			break;
		 case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
						_left_click = true;
                        break;
                    case SDL_BUTTON_RIGHT:
						_right_click = true;
                        break;
                    default:
                        break;
                }
                break;
		case SDL_MOUSEBUTTONUP:
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
						_left_click = false;
                        break;
                    case SDL_BUTTON_RIGHT:
						_right_click = false;
                        break;
                    default:
                        break;
                }
                break;
		case SDL_MOUSEWHEEL:
			_scroll_rel.x = event.wheel.x;
			_scroll_rel.y = event.wheel.y;
			break;
		case SDL_WINDOWEVENT:
			_quit = event.window.type == SDL_WINDOWEVENT_CLOSE;
			break;
		case SDL_QUIT:
			_quit = true;
			break;
		default:
			break;
		}
	}
}

/**
 * @brief Know if an input is currently configuring
 *
 * @return true If an input is configuring
 */
bool									Inputs::isConfiguring() {
	return Inputs::get()._isConfiguring();
}
/**
 * @brief Know if an input is currently configuring
 *
 * @return true If an input is configuring
 */
bool									Inputs::_isConfiguring() {
	return _configuring;
}

/**
 * @brief Enable / disable the TextInput mode
 *
 * @param enable Enable / disable
 */
void				Inputs::setTextInputMode(bool enable) {
	Inputs::get()._setTextInputMode(enable);
}
/**
 * @brief Enable / disable the TextInput mode
 *
 * @param enable Enable / disable
 */
void				Inputs::_setTextInputMode(bool enable) {
	_isTextInputMode = enable;
}

/**
 * @brief Get the last keycode pressed
 *
 * @return SDL_Keycode The keycode
 */
SDL_Keycode				Inputs::getTextInputKeycode() {
	return Inputs::get()._getTextInputKeycode();
}
/**
 * @brief Get the last keycode pressed
 *
 * @return SDL_Keycode The keycode
 */
SDL_Keycode				Inputs::_getTextInputKeycode() const {
	return _lastKeycode;
}

/**
 * @brief The the last key name pressed
 *
 * @return std::string The key name
 */
std::string				Inputs::getTextInputString() {
	return Inputs::get()._getTextInputString();
}
/**
 * @brief The the last key name pressed
 *
 * @return std::string The key name
 */
std::string				Inputs::_getTextInputString() const {
	return _currentText;
}

/**
 * @brief Know if we ignore the state of the key (for example in TextInput mode)
 *
 * @param type The InputType
 * @return true If we ignore the key
 */
bool Inputs::_ignoreInputType(InputType::Enum type) const {
	if (!_isTextInputMode)
		return false;
	for (auto && it : _input_key_map) {
		if (it.second == type)
			return _ingoreScancode(it.first);
	}
	return true;
}
/**
 * @brief Know if we ignore the state of the key (for example in TextInput mode)
 *
 * @param scan The scancode
 * @return true If we ignore the key
 */
bool Inputs::_ingoreScancode(SDL_Scancode scan) const {
	if (!_isTextInputMode)
		return false;
	if (std::find(_textInputIgnore.begin(), _textInputIgnore.end(), scan) != _textInputIgnore.end())
		return false;
	return true;
}
