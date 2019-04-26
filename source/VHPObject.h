#ifndef VHPOBJECT_H
#define VHPOBJECT_H

#include "Context.h"

namespace fpmed {
	class VHPObject
	{
	private:
      
		
	public:
		VHPObject();
		~VHPObject();
		
        
        // CreateModel - Creates the visible human model in the scene
        void CreateModel(fpmed::Context c);
	};
}
#endif