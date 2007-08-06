#ifndef __BUTTON_SOUND_CONTROL_H
#define __BUTTON_SOUND_CONTROL_H

#include "Button_System.h"

typedef enum ButtonSoundScheme
{
	BUTTON_SOUND_SCHEME_NONE,
	BUTTON_SOUND_SCHEME_GENERIC,
	BUTTON_SOUND_SCHEME_VERYSMALLSWITCH1,
	BUTTON_SOUND_SCHEME_VERYSMALLSWITCH2,
	BUTTON_SOUND_SCHEME_SMALLSWITCH1,
	BUTTON_SOUND_SCHEME_SMALLSWITCH2,
	BUTTON_SOUND_SCHEME_SMALLSWITCH3,
	BUTTON_SOUND_SCHEME_BIGSWITCH3,
	BUTTON_SOUND_SCHEME_COMPUTERBEEP2,
	BUTTON_SOUND_SCHEME_COMPUTERSWITCH1,
} ButtonSoundScheme;

void SpecifyButtonSoundScheme(INT32 iButtonID, ButtonSoundScheme SoundScheme);

void PlayButtonSound(const GUI_BUTTON* btn, INT32 iSoundType);

#endif
