#pragma once

class CInput
{
public:
	CInput();
	~CInput(void);

	BOOL Initialize(HWND wnd);
	BOOL IsKeyDown(BYTE keyid);
};
