#include "stdafx.h"
#include "NamedPipeServer.h"

NamedPipeServer::NamedPipeServer(const TCHAR* pipeName) : m_readBuffer(new char[256]), m_bufferSize(256)
{
	m_pipe = CreateNamedPipe(pipeName, PIPE_ACCESS_INBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1, 0,
		256, 200, 0);
}

NamedPipeServer::~NamedPipeServer()
{
	Destroy();
}

bool NamedPipeServer::IsCreated()
{
	return m_pipe != INVALID_HANDLE_VALUE;
}

void NamedPipeServer::Destroy()
{
	if(m_readBuffer)
	{
		delete[] m_readBuffer;
		m_readBuffer = 0;
		m_bufferSize = 0;
	}

	if(m_pipe != INVALID_HANDLE_VALUE)
	{
		DisconnectNamedPipe(m_pipe);
		CloseHandle(m_pipe);
		m_pipe = INVALID_HANDLE_VALUE;
	}
}

bool NamedPipeServer::IsDataAvailable()
{
	DWORD numBytesInMessage;
	return PeekNamedPipe(m_pipe, 0, 0, 0, 0, &numBytesInMessage) && numBytesInMessage;
}

bool NamedPipeServer::Read(std::string& message)
{
	DWORD numBytesInMessage;
	if(PeekNamedPipe(m_pipe, 0, 0, 0, 0, &numBytesInMessage))
	{
		// The extra character is used for a NULL terminator
		if(m_bufferSize < numBytesInMessage + 1)
		{
			delete[] m_readBuffer;
			m_readBuffer = new char[numBytesInMessage + 1];
			m_bufferSize = numBytesInMessage + 1;
		}

		DWORD numBytesRead;
		if(ReadFile(m_pipe, m_readBuffer, numBytesInMessage, &numBytesRead, 0))
		{
			m_readBuffer[numBytesInMessage + 1] = 0;
			message = m_readBuffer;
			return true;
		}
	}

	return false;
}