#pragma once

#include <atltrace.h>

void RealityKeyDown(LPVOID devobj, DWORD size, LPVOID data);
void SimulateKeyDown(LPVOID devobj, DWORD size, LPVOID data);
void CreateDevice(LPVOID devobj, DWORD devid);

/*
Class: MyDirectDevice
	This class implements the interface IDirectInputDevice8 and represents the core of the hooking procedure,
	together with <NamedPipeServer>.

	On creation, this class creates a <NamedPipeServer> with a pipe named "DInput-Hook". The pipe accepts
	messages in the form of strings. Each message (string) is a space-delimited list of scan-codes.
	The keys with the corresponding scan-codes will appear pressed to the hooked application the next time
	it retrieves the device state (in immediate mode) via <GetDeviceState>.

	If a malformed message is received, no action is taken - there's no error reporting or anything.
	You might want to implement a more sophisticated communication protocol to suit your needs.

	The class only works in immediate mode. I didn't implement anything for buffered input because you need
	to define the 'semantics' first - i.e. how would you like your injected scancodes interact with genuine
	user-input ones. Totally override? Come before? Come after?

	The bulk of functionality, thus, lies in <GetDeviceState>. Check it out.

Sidenote:
	Typing out all these functions and their parameters literally WRECKED my fingers. My right-hand is in
	pain. This is about the only thing I hate about hooking!
*/
class MyDirectDevice : public IDirectInputDevice8
{
public:
	MyDirectDevice(IDirectInputDevice8* device) : m_device(device)
	{
	}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj)
	{
		ATLTRACE(TEXT("MyDirectDevice::QueryInterface"));
		return m_device->QueryInterface(riid, ppvObj);
	}

	STDMETHOD_(ULONG,AddRef)()
	{
		ATLTRACE(TEXT("MyDirectDevice::AddRef"));
		return m_device->AddRef();
	}

	STDMETHOD_(ULONG,Release)()
	{
		ATLTRACE(TEXT("MyDirectDevice::Release"));
		ULONG count = m_device->Release();
		if(0 == count)
			delete this;

		return count;
	}

	/*** IDirectInputDevice8 methods ***/
	STDMETHOD(GetCapabilities)(LPDIDEVCAPS devCaps)
	{
		ATLTRACE(TEXT("MyDirectDevice::GetCapabilities"));
		return m_device->GetCapabilities(devCaps);
	}

	STDMETHOD(EnumObjects)(LPDIENUMDEVICEOBJECTSCALLBACK callback, LPVOID ref, DWORD flags)	
	{
		ATLTRACE(TEXT("MyDirectDevice::EnumObjects"));
		return m_device->EnumObjects(callback, ref, flags);
	}

	STDMETHOD(GetProperty)(REFGUID rguid, LPDIPROPHEADER ph)
	{
		ATLTRACE(TEXT("MyDirectDevice::GetProperty"));
		return m_device->GetProperty(rguid, ph);
	}

	STDMETHOD(SetProperty)(REFGUID rguid, LPCDIPROPHEADER ph)
	{
		ATLTRACE(TEXT("MyDirectDevice::SetProperty"));
		return m_device->SetProperty(rguid, ph);

		/*
		You can detect immediate/buffered modes as such:
		
		HRESULT hr = m_device->SetProperty(rguid, ph);

		if(SUCCEEDED(hr) && rguid == DIPROP_BUFFERSIZE)
		{
			DWORD data = *reinterpret_cast<const DWORD*>(ph + 1);
			m_is_immediate = (data == 0);
		}

		return hr;
		*/
	}

	STDMETHOD(Acquire)()
	{
		HRESULT hr = m_device->Acquire();

		return hr;
	}

	STDMETHOD(Unacquire)()
	{
		HRESULT hr = m_device->Unacquire();
		ATLTRACE(TEXT("MyDirectDevice::Unacquire %d"), hr);
		return hr;
	}

	STDMETHOD(GetDeviceState)(DWORD size, LPVOID data)
	{
		HRESULT hr = m_device->GetDeviceState(size, data);
		if (SUCCEEDED(hr))
		{
			RealityKeyDown(this, size, data);
			SimulateKeyDown(this, size, data);
		}
		return hr;
	}

	STDMETHOD(GetDeviceData)(DWORD size, LPDIDEVICEOBJECTDATA data, LPDWORD numElements, DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectDevice::GetDeviceData"));
		return m_device->GetDeviceData(size, data, numElements, flags);
	}

	STDMETHOD(SetDataFormat)(LPCDIDATAFORMAT df)
	{
		ATLTRACE(TEXT("MyDirectDevice::SetDataFormat"));
		return m_device->SetDataFormat(df);
	}

	STDMETHOD(SetEventNotification)(HANDLE event)
	{
		ATLTRACE(TEXT("MyDirectDevice::SetEventNotification"));
		return m_device->SetEventNotification(event);
	}

	STDMETHOD(SetCooperativeLevel)(HWND window, DWORD level)
	{
		ATLTRACE(TEXT("MyDirectDevice::SetCooperativeLevel"));
		return m_device->SetCooperativeLevel(window, level);
	}

	STDMETHOD(GetObjectInfo)(LPDIDEVICEOBJECTINSTANCE object, DWORD objId, DWORD objHow)
	{
		ATLTRACE(TEXT("MyDirectDevice::GetObjectInfo"));
		return m_device->GetObjectInfo(object, objId, objHow);
	}

	STDMETHOD(GetDeviceInfo)(LPDIDEVICEINSTANCE di)
	{
		ATLTRACE(TEXT("MyDirectDevice::GetDeviceInfo"));
		return m_device->GetDeviceInfo(di);
	}

	STDMETHOD(RunControlPanel)(HWND owner, DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectDevice::RunControlPanel"));
		return m_device->RunControlPanel(owner, flags);
	}

	STDMETHOD(Initialize)(HINSTANCE instance, DWORD version, REFGUID rguid)
	{
		ATLTRACE(TEXT("MyDirectDevice::Initialize"));
		return m_device->Initialize(instance, version, rguid);
	}

	STDMETHOD(CreateEffect)(REFGUID rguid, LPCDIEFFECT effect_params, LPDIRECTINPUTEFFECT* effect, LPUNKNOWN unknown)
	{
		ATLTRACE(TEXT("MyDirectDevice::CreateEffect"));
		return m_device->CreateEffect(rguid, effect_params, effect, unknown);
	}

    STDMETHOD(EnumEffects)(LPDIENUMEFFECTSCALLBACK callback, LPVOID ref, DWORD type)
	{
		ATLTRACE(TEXT("MyDirectDevice::EnumEffects"));
		return m_device->EnumEffects(callback, ref, type);
	}

    STDMETHOD(GetEffectInfo)(LPDIEFFECTINFO effect_info, REFGUID rguid)
	{
		ATLTRACE(TEXT("MyDirectDevice::GetEffectInfo"));
		return m_device->GetEffectInfo(effect_info, rguid);
	}

    STDMETHOD(GetForceFeedbackState)(LPDWORD state)
	{
		ATLTRACE(TEXT("MyDirectDevice::GetForceFeedbackState"));
		return m_device->GetForceFeedbackState(state);
	}

    STDMETHOD(SendForceFeedbackCommand)(DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectDevice::SendForceFeedbackCommand"));
		return m_device->SendForceFeedbackCommand(flags);
	}

    STDMETHOD(EnumCreatedEffectObjects)(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK callback, LPVOID ref, DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectDevice::EnumCreatedEffectObjects"));
		return m_device->EnumCreatedEffectObjects(callback, ref, flags);
	}

    STDMETHOD(Escape)(LPDIEFFESCAPE escape)
	{
		ATLTRACE(TEXT("MyDirectDevice::Escape"));
		return m_device->Escape(escape);
	}

    STDMETHOD(Poll)()
	{
		return m_device->Poll();
	}

	STDMETHOD(SendDeviceData)(DWORD size, LPCDIDEVICEOBJECTDATA data, LPDWORD num_elements, DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectDevice::SendDeviceData"));
		return m_device->SendDeviceData(size, data, num_elements, flags);
	}

	STDMETHOD(EnumEffectsInFile)(LPCTSTR file_name, LPDIENUMEFFECTSINFILECALLBACK callback, LPVOID ref, DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectDevice::EnumEffectsInFile"));
		return m_device->EnumEffectsInFile(file_name, callback, ref, flags);
	}

    STDMETHOD(WriteEffectToFile)(LPCTSTR file_name, DWORD num_entries, LPDIFILEEFFECT effects, DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectDevice::WriteEffectToFile"));
		return m_device->WriteEffectToFile(file_name, num_entries, effects, flags);
	}

    STDMETHOD(BuildActionMap)(LPDIACTIONFORMAT format, LPCTSTR username, DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectDevice::BuildActionMap"));
		return m_device->BuildActionMap(format, username, flags);
	}

    STDMETHOD(SetActionMap)(LPDIACTIONFORMAT format, LPCTSTR username, DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectDevice::SetActionMap"));
		return m_device->SetActionMap(format, username, flags);
	}

    STDMETHOD(GetImageInfo)(LPDIDEVICEIMAGEINFOHEADER image_header)
	{
		ATLTRACE(TEXT("MyDirectDevice::GetImageInfo"));
		return m_device->GetImageInfo(image_header);
	}

private:
	IDirectInputDevice8* m_device;
};