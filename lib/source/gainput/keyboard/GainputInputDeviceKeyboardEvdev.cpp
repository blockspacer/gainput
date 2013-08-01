
#include <gainput/gainput.h>


#if defined(GAINPUT_PLATFORM_LINUX) && defined(GAINPUT_ENABLE_RAW_INPUT)

#include "GainputInputDeviceKeyboardEvdev.h"
#include "../GainputInputDeltaState.h"
#include "GainputKeyboardKeyNames.h"
#include "../GainputHelpers.h"
#include "../GainputHelpersEvdev.h"


namespace gainput
{

const unsigned KeyboardButtonCount = KeyCount_;

InputDeviceKeyboardImpl::InputDeviceKeyboardImpl(InputManager& manager, DeviceId device) :
	manager_(manager),
	device_(device),
	textInputEnabled_(true),
	dialect_(manager_.GetAllocator()),
	keyNames_(manager_.GetAllocator()),
	fd_(-1),
	state_(0),
	previousState_(0),
	delta_(0)
{
	GetKeyboardKeyNames(keyNames_);

	unsigned matchingDeviceCount = 0;
	for (unsigned i = 0; i < EvdevDeviceCount; ++i)
	{
		fd_ = open(EvdevDeviceIds[i], O_RDONLY|O_NONBLOCK);
		if (fd_ == -1)
		{
			continue;
		}

		EvdevDevice evdev(fd_);

		if (evdev.IsValid())
		{
			if (evdev.GetDeviceType() == InputDevice::DT_KEYBOARD)
			{
				if (matchingDeviceCount == manager_.GetDeviceCountByType(InputDevice::DT_KEYBOARD))
				{
					break;
				}
				++matchingDeviceCount;
			}
		}

		close(fd_);
		fd_ = -1;
	}

	GAINPUT_ASSERT(fd_ != -1);

	dialect_[KEY_ESC] = KeyEscape;
	dialect_[KEY_1] = Key1;
	dialect_[KEY_2] = Key2;
	dialect_[KEY_3] = Key3;
	dialect_[KEY_4] = Key4;
	dialect_[KEY_5] = Key5;
	dialect_[KEY_6] = Key6;
	dialect_[KEY_7] = Key7;
	dialect_[KEY_8] = Key8;
	dialect_[KEY_9] = Key9;
	dialect_[KEY_0] = Key0;
	dialect_[KEY_MINUS] = KeyMinus;
	dialect_[KEY_EQUAL] = KeyEqual;
	dialect_[KEY_BACKSPACE] = KeyBackSpace;
	dialect_[KEY_TAB] = KeyTab;
	dialect_[KEY_Q] = KeyQ;
	dialect_[KEY_W] = KeyW;
	dialect_[KEY_E] = KeyE;
	dialect_[KEY_R] = KeyR;
	dialect_[KEY_T] = KeyT;
	dialect_[KEY_Y] = KeyY;
	dialect_[KEY_U] = KeyU;
	dialect_[KEY_I] = KeyI;
	dialect_[KEY_O] = KeyO;
	dialect_[KEY_P] = KeyP;
	dialect_[KEY_LEFTBRACE] = KeyBraceLeft;
	dialect_[KEY_RIGHTBRACE] = KeyBraceRight;
	dialect_[KEY_ENTER] = KeyReturn;
	dialect_[KEY_LEFTCTRL] = KeyCtrlL;
	dialect_[KEY_A] = KeyA;
	dialect_[KEY_S] = KeyS;
	dialect_[KEY_D] = KeyD;
	dialect_[KEY_F] = KeyF;
	dialect_[KEY_G] = KeyG;
	dialect_[KEY_H] = KeyH;
	dialect_[KEY_J] = KeyJ;
	dialect_[KEY_K] = KeyK;
	dialect_[KEY_L] = KeyL;
	dialect_[KEY_SEMICOLON] = KeySemicolon;
	dialect_[KEY_APOSTROPHE] = KeyApostrophe;
	dialect_[KEY_GRAVE] = KeyGrave;
	dialect_[KEY_LEFTSHIFT] = KeyShiftL;
	dialect_[KEY_BACKSLASH] = KeyBackslash;
	dialect_[KEY_Z] = KeyZ;
	dialect_[KEY_X] = KeyX;
	dialect_[KEY_C] = KeyC;
	dialect_[KEY_V] = KeyV;
	dialect_[KEY_B] = KeyB;
	dialect_[KEY_N] = KeyN;
	dialect_[KEY_M] = KeyM;
	dialect_[KEY_COMMA] = KeyComma;
	dialect_[KEY_DOT] = KeyPeriod;
	dialect_[KEY_SLASH] = KeySlash;
	dialect_[KEY_RIGHTSHIFT] = KeyShiftR;
	dialect_[KEY_KPASTERISK] = KeyKpMultiply;
	dialect_[KEY_LEFTALT] = KeyAltL;
	dialect_[KEY_SPACE] = KeySpace;
	dialect_[KEY_CAPSLOCK] = KeyCapsLock;
	dialect_[KEY_F1] = KeyF1;
	dialect_[KEY_F2] = KeyF2;
	dialect_[KEY_F3] = KeyF3;
	dialect_[KEY_F4] = KeyF4;
	dialect_[KEY_F5] = KeyF5;
	dialect_[KEY_F6] = KeyF6;
	dialect_[KEY_F7] = KeyF7;
	dialect_[KEY_F8] = KeyF8;
	dialect_[KEY_F9] = KeyF9;
	dialect_[KEY_F10] = KeyF10;
	dialect_[KEY_NUMLOCK] = KeyNumLock;
	dialect_[KEY_SCROLLLOCK] = KeyScrollLock;
	dialect_[KEY_KP7] = KeyKpHome;
	dialect_[KEY_KP8] = KeyKpUp;
	dialect_[KEY_KP9] = KeyKpPageUp;
	dialect_[KEY_KPMINUS] = KeyKpSubtract;
	dialect_[KEY_KP4] = KeyKpLeft;
	dialect_[KEY_KP5] = KeyKpBegin;
	dialect_[KEY_KP6] = KeyKpRight;
	dialect_[KEY_KPPLUS] = KeyKpAdd;
	dialect_[KEY_KP1] = KeyKpEnd;
	dialect_[KEY_KP2] = KeyKpDown;
	dialect_[KEY_KP3] = KeyKpPageDown;
	dialect_[KEY_KP0] = KeyKpInsert;
	dialect_[KEY_KPDOT] = KeyKpDelete;

	dialect_[KEY_F11] = KeyF11;
	dialect_[KEY_F12] = KeyF12;
	dialect_[KEY_KPENTER] = KeyKpEnter;
	dialect_[KEY_RIGHTCTRL] = KeyCtrlR;
	dialect_[KEY_KPSLASH] = KeyKpDivide;
	dialect_[KEY_SYSRQ] = KeySysRq;
	dialect_[KEY_RIGHTALT] = KeyAltR;
	dialect_[KEY_HOME] = KeyHome;
	dialect_[KEY_UP] = KeyUp;
	dialect_[KEY_PAGEUP] = KeyPageUp;
	dialect_[KEY_LEFT] = KeyLeft;
	dialect_[KEY_RIGHT] = KeyRight;
	dialect_[KEY_END] = KeyEnd;
	dialect_[KEY_DOWN] = KeyDown;
	dialect_[KEY_PAGEDOWN] = KeyPageDown;
	dialect_[KEY_INSERT] = KeyInsert;
	dialect_[KEY_DELETE] = KeyDelete;
	dialect_[KEY_MUTE] = KeyMute;
	dialect_[KEY_VOLUMEDOWN] = KeyVolumeDown;
	dialect_[KEY_VOLUMEUP] = KeyVolumeUp;
	dialect_[KEY_POWER] = KeyPower;
	dialect_[KEY_PAUSE] = KeyBreak;
	dialect_[KEY_KPCOMMA] = KeyKpDelete;
	dialect_[KEY_LEFTMETA] = KeySuperL;
	dialect_[KEY_RIGHTMETA] = KeySuperR;

	dialect_[KEY_BACK] = KeyBack;
	dialect_[KEY_FORWARD] = KeyForward;
	dialect_[KEY_NEXTSONG] = KeyMediaNext;
	dialect_[KEY_PLAYPAUSE] = KeyMediaPlayPause;
	dialect_[KEY_PREVIOUSSONG] = KeyMediaPrevious;
	dialect_[KEY_STOPCD] = KeyMediaStop;
	dialect_[KEY_CAMERA] = KeyCamera;
	dialect_[KEY_COMPOSE] = KeyEnvelope;
	dialect_[KEY_REWIND] = KeyMediaRewind;
	dialect_[KEY_FASTFORWARD] = KeyMediaFastForward;
}

InputDeviceKeyboardImpl::~InputDeviceKeyboardImpl()
{
	if (fd_ != -1)
	{
		close(fd_);
	}
}

void
InputDeviceKeyboardImpl::Update(InputState& state, InputState& previousState, InputDeltaState* delta)
{
	if (fd_ < 0)
	{
		return;
	}

	struct input_event event;

	for (;;)
	{
		int len = read(fd_, &event, sizeof(struct input_event));
		if (len != sizeof(struct input_event))
		{
			break;
		}

		if (event.type == EV_KEY)
		{
			if (dialect_.count(event.code))
			{
				const DeviceButtonId button = dialect_[event.code];
				HandleButton(device_, state, previousState, delta, button, bool(event.value));
			}
		}
	}
}

size_t
InputDeviceKeyboardImpl::GetKeyName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	HashMap<Key, const char*>::const_iterator it = keyNames_.find(Key(deviceButton));
	if (it == keyNames_.end())
	{
		return 0;
	}
	strncpy(buffer, it->second, bufferLength);
	buffer[bufferLength-1] = 0;
	const size_t nameLen = strlen(it->second);
	return nameLen >= bufferLength ? bufferLength : nameLen+1;
}

DeviceButtonId
InputDeviceKeyboardImpl::GetButtonByName(const char* name) const
{
	for (HashMap<Key, const char*>::const_iterator it = keyNames_.begin();
			it != keyNames_.end();
			++it)
	{
		if (strcmp(name, it->second) == 0)
		{
			return it->first;
		}
	}
	return InvalidDeviceButtonId;
}


}

#include "GainputKeyboardCommon.h"

#endif

