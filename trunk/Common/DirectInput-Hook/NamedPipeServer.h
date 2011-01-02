#pragma once

/*
Class: NamedPipeServer
	A simple data server that uses a named pipe for data communication. Communication is
	uni-directional, with the client writing and the server reading.

	The class establishes a pipe with the given name on construction, and removes it automatically
	on destruction (or manually via <Destroy>).

	Messages take the simple form of strings.
*/
class NamedPipeServer
{
public:
	NamedPipeServer(const TCHAR* pipeName);
	~NamedPipeServer();
	
	void Destroy();
	bool IsCreated();
	
	bool IsDataAvailable();	
	bool Read(std::string& message);
	
private:
	HANDLE m_pipe;
	char* m_readBuffer;
	int m_bufferSize;
};