#pragma once
#include "Plugin.h"
namespace Sound
{
	class Voice
	{
	public:
		Voice();
		~Voice();
		// Starts from the first plugin and recursively
		// process the data
		void Fill(Samples Start, Samples Count, float* Data);
		// Repurpose this voice to another task
		void New(Plugin::Plugin* pEntryPlugin);
		bool IsActive() const;
	private:
		void Delete();
		Plugin::Plugin* mpEntryPlugin;
		bool mActive;
	};
}