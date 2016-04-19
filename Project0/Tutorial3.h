#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include "OgreManualObject.h"
#include "btBulletDynamicsCommon.h"
#include "btHeightfieldTerrainShape.h"
#include "BaseApplication.h"
#include <cstdlib>
#include <random>
 
class TutorialApplication : public BaseApplication
{
public:
  TutorialApplication();
  virtual ~TutorialApplication();
 
protected:
  virtual void createScene();
  virtual void createFrameListener();
  virtual void destroyScene();
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);
  bool frameStarted(const Ogre::FrameEvent &evt);
  void CreateCube(const btVector3 &Position, btScalar Mass, const btVector3 &scale, char * name, bool vis);
  void CreateSphere(const btVector3 &Position, btScalar Mass, const btVector3 &scale, char * name, float velocity);
  Ogre::ManualObject *createCubeMesh(Ogre::String name, Ogre::String matName);
 
private:
  void resetTargets();
  bool randomize();
  void defineTerrain(long x, long y);
  void processUnbufferedInput(const Ogre::FrameEvent& fe);
  bool fire = false;
  void initBlendMaps(Ogre::Terrain* terrain);
  void configureTerrainDefaults(Ogre::Light* light);
  void createBulletSim(void);
  bool mTerrainsImported;
  Ogre::TerrainGroup* mTerrainGroup;
  Ogre::TerrainGlobalOptions* mTerrainGlobals;
 
  bool isDown = false;
  int numOfSpheres = 0;
  float power = 0;
  OgreBites::Label* mInfoLabel;
  btDefaultCollisionConfiguration* collisionConfiguration;
  btCollisionDispatcher* dispatcher;
  btBroadphaseInterface* overlappingPairCache;
  btSequentialImpulseConstraintSolver* solver;
  btDiscreteDynamicsWorld* dynamicsWorld;
  btCollisionShape* groundShape;
  btAlignedObjectArray<btCollisionShape*> collisionShapes;
 
};
 

