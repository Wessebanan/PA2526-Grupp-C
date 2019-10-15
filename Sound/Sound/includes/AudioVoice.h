#pragma once
#include "AudioPlugin.h"
namespace Audio
{
	class Voice
	{
	public:
		Voice();
		~Voice();
		// Starts from the first plugin and recursively
		// process the data
		void Fill(Samples start, Samples count, float* pData);
		// Repurpose this voice to another task
		void New(Plugin::Plugin* pEntryPlugin);
		bool IsActive() const;
	private:
		void Delete();
		Plugin::Plugin* mpEntryPlugin;
		bool mActive;
	};
}