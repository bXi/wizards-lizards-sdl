#pragma once

#include "menustructure.h"

#include "configuration/configuration.h"
#include "settings/settingshandler.h"
#include "audio/audiohandler.h"
#include "input/inputhandler.h"
#include <state/basestate.h>

class MenuState : public BaseState {
private:
	int menuChoice = 0;
	int subMenuChoice = 0;

	menuobject mo;
	menumanager mm;

	std::string sLastAction;
	std::vector<InputDevice*> menucontrollers;

public:

	void load() override;
	void unload() override;
	void draw() override;


	std::string getVolumeBar(float volume);


};
