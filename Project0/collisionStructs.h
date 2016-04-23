#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include "OgreManualObject.h"
#include "btBulletDynamicsCommon.h"
#include "btHeightfieldTerrainShape.h"
#include <cstdlib>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>
#include <vector>
#include <string>
#include <list>

#ifndef __COLLISIONSTRUCTS_h_
#define __COLLISIONSTRUCTS_h_

class MyMotionState : public btMotionState {
public:
	MyMotionState(const btTransform &initialpos, Ogre::SceneNode *node) {
		mVisibleobj = node;
		mPos1 = initialpos;
	}
	virtual ~MyMotionState() {    }
	void setNode(Ogre::SceneNode *node) {
		mVisibleobj = node;
	}
	virtual void getWorldTransform(btTransform &worldTrans) const {
		worldTrans = mPos1;
	}
	virtual void setWorldTransform(const btTransform &worldTrans) {
		if (NULL == mVisibleobj) return; // silently return before we set a node
		btQuaternion rot = worldTrans.getRotation();
		mVisibleobj->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
		btVector3 pos = worldTrans.getOrigin();
		// TODO **** XXX need to fix this up such that it renders properly since this doesnt know the scale of the node
		// also the getCube function returns a cube that isnt centered on Z
		mVisibleobj->setPosition(pos.x(), pos.y() + 5, pos.z() - 5);
	}
protected:
	Ogre::SceneNode *mVisibleobj;
	btTransform mPos1;
};

//collisions structor
struct ogreObject {
	Ogre::Entity* entityObject;
	Ogre::SceneNode* sceneNodeObject;
	MyMotionState* myMotionStateObject;
	btCollisionShape* btCollisionShapeObject;
	btRigidBody* btRigidBodyObject;
	btCollisionObject* btCollisionObjectObject;
	std::string objectType;
	Ogre::Vector3 objectPosition;
	bool objectDelete;
	double timeAlive = 0;

};


// struct to hold contact pairs from contact manifold
struct contactPair {
	const   btCollisionObject * collisionObject1;
	const   btCollisionObject * collisionObject2;
	const   btRigidBody * rigidBodyObject1;
	const   btRigidBody * rigidBodyObject2;
	const Ogre::SceneNode * sceneNodeObject1;
	const Ogre::SceneNode * sceneNodeObject2;
	Ogre::SceneNode ** ptrToSceneNodeObject1;
	Ogre::SceneNode ** ptrToSceneNodeObject2;
	ogreObject * ptrToOgreObject1;
	ogreObject * ptrToOgreObject2;
	ogreObject ** ptrToOgreObject1Ptr;
	ogreObject ** ptrToOgreObject2Ptr;
	std::string typeObject1;
	std::string typeObject2;
	Ogre::Vector3 positionObject1;
	Ogre::Vector3 positionObject2;
	// Added
	const btCollisionShape* collisionShape1;
	const btCollisionShape* collisionShape2;

};

#endif