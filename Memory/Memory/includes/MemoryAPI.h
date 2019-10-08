#pragma once

#include <unordered_map>
#include "MemoryGlobals.h"

#include "Heap.h"



namespace memory
{
	/*
		Static functions for usability.
		These funtions fetches the memory manager instance and calls
		on its methods.
	*/
	static inline bool Initialize(uint size);
	static inline void End();
	static inline Heap* CreateHeap(uint size);

	static inline void* Allocate(size_t size);

	template <typename T>
	static inline void Free(T* pObject);

	/*
		MemoryManager is a singleton class that handles the creation and
		destruction of all primary memory domains.

		The idea is that MemoryManager uses malloc for a large portion of
		memory at the beginning of the application, where all the future
		allocations happens inside this pre-allocated memory.
	*/
	class MemoryManager
	{
	public:
		// Delete default copy constructor and copy operator.
		DENY_COPY(MemoryManager)

		/*
			Returns an instance to the Memory Manager. Use this instance
			to create custom memory domains.
		*/
		static MemoryManager& Instance();

		/*
			Allocates main memory for the MemoryManager. This method has to
			be called before using the manager to create allocators or allocate
			memory.
		*/
		bool Initialize(uint size);

		/*
			Frees all allocated memory.
		*/
		static void End();

		/*
			Allocate directly on the main heap.
		*/
		void* Allocate(uint size);

		/*
			Free directly on main heap.
		*/
		template <typename T>
		void Free(T* pObject);

		/*
			!BETA METHOD, LATER IMPLEMENTATIONS WILL SPECIFY ALLOCATOR TYPE!
			Creates a heap for a new memory domain (graphics, ECS etc.) that the
			user can use.
		*/
		Heap* CreateHeap(uint size);

		/*
			Getters (TODO)
		*/

		uint GetMainHeapSize();
		uint GetMainAllocatorSize();
		uint GetTotalAllocatedMemorySize();

	private:

		MemoryManager();
		~MemoryManager();

		static MemoryManager* mInstance;

		uint mMemorySize;
		void *mpMemoryStart;
		Heap mMainHeap;


		inline bool IsInitialized()
		{
			return mpMemoryStart;
		}

		void EndInternal()
		{
			mMainHeap.Terminate();
		}

		#define IF_INITIALIZED_RETURN(ret) if (mpMemoryStart) {return ret;}
		#define IF_NOT_INITIALIZED_RETURN(ret) if (!mpMemoryStart) {return ret;}
		#define IF_NOT_INITIALIZED_RETURN_VOID if (!mpMemoryStart) {return;}
	};

	/*
		Templated methods for memory manager, must be in header as types are not
		known at compile time.
	*/

	template <typename T>
	void MemoryManager::Free(T* pObject)
	{
		if (!mpMemoryStart) { return; }
		pObject->~T();
		mMainHeap.Free(pObject);
	}




	/*
		Static function declarations
	*/

	static inline bool Initialize(uint size)
	{
		return MemoryManager::Instance().Initialize(size);
	}

	static inline void End()
	{
		MemoryManager::Instance().End();
	}

	static inline Heap* CreateHeap(uint size)
	{
		return MemoryManager::Instance().CreateHeap(size);
	}

	static inline void* Allocate(size_t size)
	{
		return MemoryManager::Instance().Allocate((uint)size);
	}

	template <typename T>
	static inline void Free(T* pObject)
	{
		MemoryManager::Instance().Free(pObject);
	}
}