#pragma once

#include "MyDirectDevice.h"

class MyDirectInput8 : public IDirectInput8
{
public:
	MyDirectInput8(IDirectInput8* di) : m_di(di)
	{
	}

	/*** IUnknown methods ***/
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObj)
	{
		ATLTRACE(TEXT("MyDirectInput8::QueryInterface"));
		return m_di->QueryInterface(riid, ppvObj);
	}

    ULONG STDMETHODCALLTYPE AddRef()
	{
		ATLTRACE(TEXT("MyDirectInput8::AddRef"));
		return m_di->AddRef();
	}

    ULONG STDMETHODCALLTYPE Release()
	{
		ATLTRACE(TEXT("MyDirectInput8::Release"));
		ULONG count = m_di->Release();
		if(0 == count)
			delete this;

		return count;
	}

    /*** IDirectInput8 methods ***/
    STDMETHOD(CreateDevice)(REFGUID rguid, IDirectInputDevice8** device, LPUNKNOWN unknown)
	{
		ATLTRACE(TEXT("MyDirectInput8::CreateDevice %x %x %x"), rguid.Data1, rguid.Data2, rguid.Data3);
		HRESULT hr = m_di->CreateDevice(rguid, device, unknown);
		if(SUCCEEDED(hr)/* && rguid == GUID_SysKeyboard*/)
		{
			// Return our own keyboard device that checks for injected keypresses
			*device = new MyDirectDevice(*device);

			::CreateDevice(*device);
		}

		return hr;
	}

    STDMETHOD(EnumDevices)(DWORD devType,LPDIENUMDEVICESCALLBACK callback, LPVOID ref, DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectInput8::EnumDevices %x"), devType);
		return m_di->EnumDevices(devType, callback, ref, flags);
	}

    STDMETHOD(GetDeviceStatus)(REFGUID rguid)
	{
		ATLTRACE(TEXT("MyDirectInput8::GetDeviceStatus"));
		return m_di->GetDeviceStatus(rguid);
	}

    STDMETHOD(RunControlPanel)(HWND owner, DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectInput8::RunControlPanel"));
		return m_di->RunControlPanel(owner, flags);
	}

    STDMETHOD(Initialize)(HINSTANCE instance, DWORD version)
	{
		ATLTRACE(TEXT("MyDirectInput8::Initialize"));
		return m_di->Initialize(instance, version);
	}

    STDMETHOD(FindDevice)(REFGUID rguid, LPCTSTR name, LPGUID guidInstance)
	{
		ATLTRACE(TEXT("MyDirectInput8::FindDevice"));
		return m_di->FindDevice(rguid, name, guidInstance);
	}

    STDMETHOD(EnumDevicesBySemantics)(LPCTSTR username, LPDIACTIONFORMAT action,
		LPDIENUMDEVICESBYSEMANTICSCB callback, LPVOID ref, DWORD flags)
	{
		ATLTRACE(TEXT("MyDirectInput8::EnumDevicesBySemantics"));
		return m_di->EnumDevicesBySemantics(username, action, callback, ref, flags);
	}

    STDMETHOD(ConfigureDevices)(LPDICONFIGUREDEVICESCALLBACK callback, LPDICONFIGUREDEVICESPARAMS params,
		DWORD flags, LPVOID ref)
	{
		ATLTRACE(TEXT("MyDirectInput8::ConfigureDevices"));
		return m_di->ConfigureDevices(callback, params, flags, ref);
	}

private:
	IDirectInput8* m_di;
};