
#include <gainput/gainput.h>
#include <gainput/gestures/GainputTapGesture.h>

#ifdef GAINPUT_ENABLE_TAP_GESTURE
#include "../GainputInputDeltaState.h"
#include "../GainputHelpers.h"

namespace gainput
{

TapGesture::TapGesture(InputManager& manager, DeviceId device, DeviceVariant variant) :
	InputGesture(manager, device),
	firstDownTime_(0)
{
	actionButton_.buttonId = InvalidDeviceButtonId;

	state_ = manager_.GetAllocator().New<InputState>(manager.GetAllocator(), 1);
	GAINPUT_ASSERT(state_);
	previousState_ = manager_.GetAllocator().New<InputState>(manager.GetAllocator(), 1);
	GAINPUT_ASSERT(previousState_);
}

TapGesture::~TapGesture()
{
	manager_.GetAllocator().Delete(state_);
	manager_.GetAllocator().Delete(previousState_);
}

void
TapGesture::Initialize(DeviceId actionButtonDevice, DeviceButtonId actionButton, uint64_t timeSpan)
{
	actionButton_.deviceId = actionButtonDevice;
	actionButton_.buttonId = actionButton;
	timeSpan_ = timeSpan;
}

void
TapGesture::InternalUpdate(InputDeltaState* delta)
{
	if (actionButton_.buttonId == InvalidDeviceButtonId)
	{
		return;
	}

	const InputDevice* actionDevice = manager_.GetDevice(actionButton_.deviceId);
	GAINPUT_ASSERT(actionDevice);

	HandleButton(deviceId_, *state_, *previousState_, delta, TapTriggered, false);

	if (actionDevice->GetBool(actionButton_.buttonId))
	{
		if (firstDownTime_ == 0)
		{
			firstDownTime_ = manager_.GetTime();
		}
	}
	else
	{
		if (firstDownTime_ > 0 && firstDownTime_ + timeSpan_ >= manager_.GetTime())
		{
			HandleButton(deviceId_, *state_, *previousState_, delta, TapTriggered, true);
		}
		firstDownTime_ = 0;
	}
}

}

#endif

