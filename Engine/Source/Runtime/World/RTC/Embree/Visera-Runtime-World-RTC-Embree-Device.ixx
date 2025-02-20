module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.World.RTC.Embree:Device;

import Visera.Core.Signal;

export namespace VE
{
	
	class FEmbreeDevice
	{
	public:
		auto GetHandle() const -> const RTCDevice { return Handle; }
		operator RTCDevice() const { return Handle; }

	private:
		RTCDevice Handle = nullptr;

	public:
		FEmbreeDevice();
		~FEmbreeDevice();

	private:
		void static
		EmbreeMessengerCallback(void* _UserPtr, RTCError _Error, const char* _Message)
		{ throw SRuntimeError(Text("Embree Error {}: {}", VE::ErrorCode(_Error), _Message)); }
	};

	FEmbreeDevice::
	FEmbreeDevice()
	{
		Handle = rtcNewDevice(NULL);
		if (!Handle) throw SRuntimeError("Failed to create the Embree Device!");

		rtcSetDeviceErrorFunction(Handle, EmbreeMessengerCallback, NULL);
	}

	FEmbreeDevice::
	~FEmbreeDevice()
	{

	}

} // namespace VE