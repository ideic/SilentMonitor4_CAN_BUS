#pragma once
class IRunable
{
public:
	virtual void Run() = 0;
	virtual void Stop() = 0;
	virtual bool IsRunning() = 0;
};

