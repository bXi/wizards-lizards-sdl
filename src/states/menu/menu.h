#pragma once

#include "menustructure.h"

#include "configuration/configuration.h"
#include "settings/settingshandler.h"

#include "luminoveau.h"

#include <state/basestate.h>

class MenuState : public BaseState {
private:
	menuobject mo;
	menumanager mm;

	std::string sLastAction;
	std::vector<InputDevice*> menucontrollers;

    Font titleFont;
    Font menuFont;
public:

	void load() override;
	void unload() override;
	void draw() override;


	std::string getVolumeBar(float volume);


};
