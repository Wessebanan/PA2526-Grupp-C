#pragma once

#include "D3D11Header.h"


// How will this handle large chuncks of data split up in per draw?

/*
	___ INITIALIZIATION AND HANDLING OF RESOURCES ___

	During initialization phase set all resources that are 
	to be bound during the program. No more 'set' is allowed.

	Constant Buffers
	 - Allocate buffers for each slot type
	 - Upload
	 - Choose buffer region with XXSetConstantBuffer1 (Exception)

	Vertex/Index Buffers
	 - Allocate large buffers to contain all necessary mesh data
	 - Choose buffer region with DrawIndexedInstanced/DrawInstanced

	Sampler States
	 - Static during runtime
	 - Initialize all required resources at start 
	 - Usally no more than 6 is often required

	 Textures
	 - Initialized as buffers (can probably generate mips but not become skybox)
	 - Can become rtv, srv and ds
	 - if render target data is necessary for input, copy data to a srv

	... (What about the others?) ...



	--- Allocation of buffers ---

	4 different Buffers at minimum:
		- Dynamic Constants
		- Static Constants
		- Render Targets and Depth Stencil
		- Shader Resources

		
	--- Runtime execution ---

	1. Initialize Device and other independent resources					(internal)
	2. Initialize all static data											(external)
	3. Upload all data to static buffers									(internal)
	4. Set states that will not switch (index buffer / vertex buffers ...)  (internal)
	4. Begin update loop													(external)
		5. Upload necessary data to dynamic buffers							(internal)
		6. Set states and draw												(external)
		9. Present															(external)
	   10. Jump to (5)


	  Input Layout
		- Vertex Position
		- Vertex Normals
		- Vertex UV
		- Per Instance index (0 ... 1023)
		

*/
