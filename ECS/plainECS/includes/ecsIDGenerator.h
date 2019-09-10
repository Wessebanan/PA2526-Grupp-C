#pragma once

template <typename T>
class IDGenerator
{
public:
	IDGenerator(T startIDValue)
	{
		currentIDValue = startIDValue;
	}

	~IDGenerator()
	{
		//
	}

	T generateID()
	{
		return currentIDValue++;
	}
private:
	T currentIDValue;
};