#pragma once
#include <string>

// --------- DebugInfo general usage ----------
// Add parameters to be tracked by DebugInfo using the macro
// DINFO_ADD_PARAM(name, type, value) under the approproite 
// namespace (feel free to add any missing namespace within
// the DInfo namespace below).

// Once setup, use the parameters like this example:
//*	int Fps = DInfo::Graphics::Fps;
//*	DInfo::Graphics::Fps = 60;
//* std::cout << DInfo::Graphics::Fps.ToString();

// In case you only want variables to be set while
// debugging, use DINFO_SET(name, value).
// This will effectivly remove the code line when in
// release configuration.

// --------- MACROS ----------
// DINFO_ADD_PARAM(name, type, value)
//*	name: Name of the parameter
//* type: What type it is (e.g. int, std::string)
//* value: Default initial value

// DINFO_SET(name, value)
//* name: Name of the parameter to set
//* value: Value to set parameter to
// The set won't happen unless in debug configuration
#pragma region DINFO MACROS
	#define DINFO_ADD_PARAM(name, type, value) \
		_structs::structInfo<type> name(value)
	#ifdef _DEBUG
		#define DINFO_SET(name, value) \
			name = value
		//#define DINFO_GET(name) \
		//	name
	#else
		#define DINFO_SET(name, value) 
		//#define DINFO_GET(name)
	#endif
#pragma endregion

namespace DInfo
{
	// ------ Example usage, feel free to add! -------

	//namespace Graphics
	//{
	//	//DINFO_ADD_PARAM(Text, std::string, "");
	//	//DINFO_ADD_PARAM(Fps, int, 0);
	//}
	//namespace Sound
	//{
	//	
	//}


	namespace _structs
	{
		template <typename T>
		struct structInfo
		{
			T value;
			structInfo(T other)
			{
				value = other;
			}
			std::string ToString()
			{
				return std::to_string(value);
			}
			operator T() const
			{
				return value;
			}
			T operator=(const T other)
			{
				value = other;
				return value;
			}
		};

		template<>
		struct structInfo<std::string>
		{
			std::string value;
			structInfo(std::string other)
			{
				value = other;
			}
			std::string ToString()
			{
				return value;
			}
			operator std::string() const
			{
				return value;
			}
			std::string operator=(const std::string other)
			{
				value = other;
				return value;
			}
		};
	}
}