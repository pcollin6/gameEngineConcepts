#include "btBulletDynamicsCommon.h"
#include "btHeightfieldTerrainShape.h"
#include "../Tutorial3.h"
#include "OgreManualObject.h"
#include <stdlib.h>

#ifndef _Target_h_
#define _Target_h_

class target {
public:
	bool hit;
	std::list<Ogre::SceneNode> targetCubes;
	target();

private:
	void buildTarget();

};


#endif // #ifndef _Target_h_