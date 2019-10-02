#pragma once
#include "Plugin.h"
namespace Sound
{
	class Voice
	{
	public:
		Voice();
		~Voice();
		void Fill(Samples Start, Samples Count, float* Data);
		void New(Plugin::Plugin* pEntryPlugin);
		bool IsActive() const;
	private:
		void Delete();
		Plugin::Plugin* mpEntryPlugin;
		bool mActive;
	};
}