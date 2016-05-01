#include "Tutorial3.h"
 
TutorialApplication::TutorialApplication()
  : mTerrainGroup(0),
    mTerrainGlobals(0),
    mInfoLabel(0)
{
}
 
TutorialApplication::~TutorialApplication()
{
}
 
void TutorialApplication::CreateSphere(const btVector3 &Position, btScalar Mass, const btVector3 &scale, char * name, float velocity){
	// empty ogre vectors for the sphere size and position
	Ogre::Vector3 size = Ogre::Vector3::ZERO;
	Ogre::Vector3 pos = Ogre::Vector3::ZERO;

	ptrToOgreObject = new ogreObject;
	// Convert the bullet physics vector to the ogre vector
	pos.x = Position.getX();
	pos.y = Position.getY();
	pos.z = Position.getZ();
	ptrToOgreObject->entityObject = mSceneMgr->createEntity(name, "sphere.mesh");
	ptrToOgreObject->entityObject->setCastShadows(true);

	ptrToOgreObject->sceneNodeObject = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	ptrToOgreObject->sceneNodeObject->attachObject(ptrToOgreObject->entityObject);
	ptrToOgreObject->sceneNodeObject->scale(Ogre::Vector3(scale.getX(), scale.getY(), scale.getZ()));

	Ogre::AxisAlignedBox boundingB = ptrToOgreObject->entityObject->getBoundingBox();

	boundingB.scale(Ogre::Vector3(scale.getX(), scale.getY(), scale.getZ()));
	size = boundingB.getSize()*0.95f;
	btTransform Transform;
	Transform.setIdentity();
	Transform.setOrigin(Position);
	ptrToOgreObject->myMotionStateObject = new MyMotionState(Transform, ptrToOgreObject->sceneNodeObject);

	btScalar HalfExtents(size.x);
	ptrToOgreObject->btCollisionShapeObject = new btSphereShape(HalfExtents);
	btVector3 LocalInertia;
	ptrToOgreObject->btCollisionShapeObject->calculateLocalInertia(Mass, LocalInertia);
	ptrToOgreObject->btRigidBodyObject = new btRigidBody(Mass, ptrToOgreObject->myMotionStateObject, ptrToOgreObject->btCollisionShapeObject, LocalInertia);

	ptrToOgreObject->btRigidBodyObject->setUserPointer((void *)(ptrToOgreObject->sceneNodeObject));

	ptrToOgreObject->btCollisionObjectObject = ptrToOgreObject->btRigidBodyObject;
	ptrToOgreObject->objectDelete = false;
	ptrToOgreObject->objectType = name;
	ptrToOgreObject->timeAlive = 0;
	ptrToOgreObjects.push_back(ptrToOgreObject);

	dynamicsWorld->addRigidBody(ptrToOgreObject->btRigidBodyObject);
	collisionShapes.push_back(ptrToOgreObject->btCollisionShapeObject);

	//gets cameras direction
	Ogre::Vector3 CamDirection = mCamera->getDerivedDirection();

	//Vector conversion
	btVector3 FireVelocity = btVector3(CamDirection.x, CamDirection.y, CamDirection.z);

	//Now as we discussed above, we want our vector to have a certain speed. We first normalize it, and then multiply it by Speed
	FireVelocity.normalize();
	FireVelocity *= (velocity * 100);

	//Now we finally propel our box
	ptrToOgreObject->btRigidBodyObject->setLinearVelocity(FireVelocity * 1);
}

void TutorialApplication::CreateCube(const btVector3 &Position, btScalar Mass, const btVector3 &scale, char * name, bool vis){
	// empty ogre vectors for the cubes size and position
	Ogre::Vector3 size = Ogre::Vector3::ZERO;
	Ogre::Vector3 pos = Ogre::Vector3::ZERO;

	ptrToOgreObject = new ogreObject;
	// Convert the bullet physics vector to the ogre vector
	pos.x = Position.getX();
	pos.y = Position.getY();
	pos.z = Position.getZ();
	ptrToOgreObject->entityObject = mSceneMgr->createEntity(name, "cube.mesh");
	ptrToOgreObject->entityObject->setCastShadows(true);
	try{
		ptrToOgreObject->sceneNodeObject = mSceneMgr->getSceneNode(name);
	}
	catch (Ogre::Exception& e){
		ptrToOgreObject->sceneNodeObject = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	}
	ptrToOgreObject->sceneNodeObject->attachObject(ptrToOgreObject->entityObject);
	ptrToOgreObject->sceneNodeObject->scale(Ogre::Vector3(scale.getX(), scale.getY(), scale.getZ()));
	//boxNode->setScale(Vector3(0.1,0.1,0.1));
	Ogre::AxisAlignedBox boundingB = ptrToOgreObject->entityObject->getBoundingBox();
	//Ogre::AxisAlignedBox boundingB = boxNode->_getWorldAABB();
	boundingB.scale(Ogre::Vector3(scale.getX(), scale.getY(), scale.getZ()));
	size = boundingB.getSize()*0.95f;
	btTransform Transform;
	Transform.setIdentity();
	Transform.setOrigin(Position);
	ptrToOgreObject->myMotionStateObject = new MyMotionState(Transform, ptrToOgreObject->sceneNodeObject);
	//Give the rigid body half the size
	// of our cube and tell it to create a BoxShape (cube)
	btVector3 HalfExtents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
	ptrToOgreObject->btCollisionShapeObject = new btBoxShape(HalfExtents);
	btVector3 LocalInertia;
	ptrToOgreObject->btCollisionShapeObject->calculateLocalInertia(Mass, LocalInertia);
	ptrToOgreObject->btRigidBodyObject = new btRigidBody(Mass, ptrToOgreObject->myMotionStateObject, ptrToOgreObject->btCollisionShapeObject, LocalInertia);

	// Store a pointer to the Ogre Node so we can update it later
	ptrToOgreObject->btRigidBodyObject->setUserPointer((void *)(ptrToOgreObject->sceneNodeObject));

	ptrToOgreObject->btCollisionObjectObject = ptrToOgreObject->btRigidBodyObject;
	ptrToOgreObject->objectDelete = false;
	ptrToOgreObject->objectType = name;
	ptrToOgreObjects.push_back(ptrToOgreObject);

	// Add it to the physics world
	dynamicsWorld->addRigidBody(ptrToOgreObject->btRigidBodyObject);
	collisionShapes.push_back(ptrToOgreObject->btCollisionShapeObject);
}

void TutorialApplication::createBulletSim(void) {
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new   btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -100, 0));
	{
		///create a few basic rigid bodies
		// start with ground plane, 1500, 1500
		Ogre::Terrain * pTerrain = mTerrainGroup->getTerrain(0, 0);
		float* terrainHeightData = pTerrain->getHeightData();
		Ogre::Vector3 terrainPosition = pTerrain->getPosition();
		float * pDataConvert = new float[pTerrain->getSize() *pTerrain->getSize()];
		for (int i = 0; i<pTerrain->getSize(); i++)
			memcpy(
			pDataConvert + pTerrain->getSize() * i, // source
			terrainHeightData + pTerrain->getSize() * (pTerrain->getSize() - i - 1), // target
			sizeof(float)*(pTerrain->getSize()) // size
			);

		float metersBetweenVertices = pTerrain->getWorldSize() / (pTerrain->getSize() - 1); //edit: fixed 0 -> 1 on 2010-08-13
		btVector3 localScaling(metersBetweenVertices, 1, metersBetweenVertices);

		btHeightfieldTerrainShape* groundShape = new btHeightfieldTerrainShape(
			pTerrain->getSize(),
			pTerrain->getSize(),
			pDataConvert,
			1/*ignore*/,
			pTerrain->getMinHeight(),
			pTerrain->getMaxHeight(),
			1,
			PHY_FLOAT,
			true);

		groundShape->setUseDiamondSubdivision(true);
		groundShape->setLocalScaling(localScaling);

		btRigidBody * mGroundBody = new btRigidBody(0, new btDefaultMotionState(), groundShape);

		mGroundBody->getWorldTransform().setOrigin(
			btVector3(
			terrainPosition.x,
			terrainPosition.y + (pTerrain->getMaxHeight() - pTerrain->getMinHeight()) / 2,
			terrainPosition.z));

		mGroundBody->getWorldTransform().setRotation(
			btQuaternion(
			Ogre::Quaternion::IDENTITY.x,
			Ogre::Quaternion::IDENTITY.y,
			Ogre::Quaternion::IDENTITY.z,
			Ogre::Quaternion::IDENTITY.w));

		dynamicsWorld->addRigidBody(mGroundBody);
		collisionShapes.push_back(groundShape);



	}
	for (int i = 0; i < 5; i++){
		char *cubeNum = (char*)malloc(3);
		itoa(numOfCubes, cubeNum, 10);
		char *cubeName = (char*)malloc(strlen(cubeNum) + strlen("Cube") + 1);
		strcpy(cubeName, "Cube");
		strcat(cubeName, cubeNum);
		std::srand(std::time(NULL));
		int locationIndex = std::rand() % (targetLocations.size());
		btVector3 location = targetLocations[locationIndex];
		targetLocations.erase(targetLocations.begin() + locationIndex);
		CreateCube(location, 1.0f, sizeRandomize(), cubeName, randomize());
		itemsLeftOver++;
		numOfCubes++;
	}
  }

btVector3 TutorialApplication::sizeRandomize() {
	std::random_device seeder;
	std::mt19937 engine(seeder());
	std::uniform_int_distribution<int> dist(0, 4);
	int randNum = dist(engine);
	return sizes[randNum];
}

bool TutorialApplication::randomize() {
	std::random_device seeder;
	std::mt19937 engine(seeder());
	std::uniform_int_distribution<int> dist(0, 10);
	int randNum = dist(engine);
	if (randNum >= 5)
		return true;
	else
		return false;

}

void TutorialApplication::resetTargets() {
	for (int i = 0; i < ptrToOgreObjects.size(); i++){
		removeObject(ptrToOgreObjects[i]);
		i--;
	}
	numOfCubes = 0;
	numOfSpheres = 0;
	collisionShapes.clear();
	delete dynamicsWorld;
	targetLocations.clear();
	addLocations();
	itemsLeftOver = 0;
	timeInt = 0;
	points = 0;
	createBulletSim();
	char* targetsLeft = (char*)malloc(3 + strlen(" items left") + 1);
	itoa(itemsLeftOver, targetsLeft, 10);
	strcat(targetsLeft, " items left");
	itemsLeft->setText(CEGUI::String(targetsLeft));
}

void TutorialApplication::removeObject(ogreObject *object) {
	for (int i = 0; i < ptrToOgreObjects.size(); i++){
		if (ptrToOgreObjects[i] == object){
			ptrToOgreObjects.erase(ptrToOgreObjects.begin() + i);
		}
	}
	object->entityObject->detachFromParent();
	mSceneMgr->destroyEntity(object->entityObject);
	object->entityObject = NULL;
	mSceneMgr->destroySceneNode(object->sceneNodeObject);
	object->sceneNodeObject = NULL;
	object->objectDelete = true;

	if (object->btRigidBodyObject && object->btRigidBodyObject->getMotionState())
		delete object->btRigidBodyObject->getMotionState();
	object->myMotionStateObject = NULL;
	dynamicsWorld->removeCollisionObject(object->btCollisionObjectObject);
	delete object->btCollisionObjectObject;
	object->btCollisionObjectObject = NULL;
	object->btRigidBodyObject = NULL;
	delete object->btCollisionShapeObject;
	object->btCollisionShapeObject = NULL;

	// now need to clean up the memory and set the pointer to NULL for that memory
	delete object;
	object = NULL;
}

Ogre::ManualObject* TutorialApplication::createCubeMesh(Ogre::String name, Ogre::String matName) {

	Ogre::ManualObject* cube = new Ogre::ManualObject(name);

	cube->begin(matName);

	cube->position(0.5, -0.5, 1.0); cube->normal(0.408248, -0.816497, 0.408248); cube->textureCoord(1, 0);
	cube->position(-0.5, -0.5, 0.0); cube->normal(-0.408248, -0.816497, -0.408248); cube->textureCoord(0, 1);
	cube->position(0.5, -0.5, 0.0); cube->normal(0.666667, -0.333333, -0.666667); cube->textureCoord(1, 1);
	cube->position(-0.5, -0.5, 1.0); cube->normal(-0.666667, -0.333333, 0.666667); cube->textureCoord(0, 0);
	cube->position(0.5, 0.5, 1.0); cube->normal(0.666667, 0.333333, 0.666667); cube->textureCoord(1, 0);
	cube->position(-0.5, -0.5, 1.0); cube->normal(-0.666667, -0.333333, 0.666667); cube->textureCoord(0, 1);
	cube->position(0.5, -0.5, 1.0); cube->normal(0.408248, -0.816497, 0.408248); cube->textureCoord(1, 1);
	cube->position(-0.5, 0.5, 1.0); cube->normal(-0.408248, 0.816497, 0.408248); cube->textureCoord(0, 0);
	cube->position(-0.5, 0.5, 0.0); cube->normal(-0.666667, 0.333333, -0.666667); cube->textureCoord(0, 1);
	cube->position(-0.5, -0.5, 0.0); cube->normal(-0.408248, -0.816497, -0.408248); cube->textureCoord(1, 1);
	cube->position(-0.5, -0.5, 1.0); cube->normal(-0.666667, -0.333333, 0.666667); cube->textureCoord(1, 0);
	cube->position(0.5, -0.5, 0.0); cube->normal(0.666667, -0.333333, -0.666667); cube->textureCoord(0, 1);
	cube->position(0.5, 0.5, 0.0); cube->normal(0.408248, 0.816497, -0.408248); cube->textureCoord(1, 1);
	cube->position(0.5, -0.5, 1.0); cube->normal(0.408248, -0.816497, 0.408248); cube->textureCoord(0, 0);
	cube->position(0.5, -0.5, 0.0); cube->normal(0.666667, -0.333333, -0.666667); cube->textureCoord(1, 0);
	cube->position(-0.5, -0.5, 0.0); cube->normal(-0.408248, -0.816497, -0.408248); cube->textureCoord(0, 0);
	cube->position(-0.5, 0.5, 1.0); cube->normal(-0.408248, 0.816497, 0.408248); cube->textureCoord(1, 0);
	cube->position(0.5, 0.5, 0.0); cube->normal(0.408248, 0.816497, -0.408248); cube->textureCoord(0, 1);
	cube->position(-0.5, 0.5, 0.0); cube->normal(-0.666667, 0.333333, -0.666667); cube->textureCoord(1, 1);
	cube->position(0.5, 0.5, 1.0); cube->normal(0.666667, 0.333333, 0.666667); cube->textureCoord(0, 0);

	cube->triangle(0, 1, 2);      cube->triangle(3, 1, 0);
	cube->triangle(4, 5, 6);      cube->triangle(4, 7, 5);
	cube->triangle(8, 9, 10);      cube->triangle(10, 7, 8);
	cube->triangle(4, 11, 12);   cube->triangle(4, 13, 11);
	cube->triangle(14, 8, 12);   cube->triangle(14, 15, 8);
	cube->triangle(16, 17, 18);   cube->triangle(16, 19, 17);
	cube->end();

	return cube;
}

void TutorialApplication::addSizes() {
	sizes.push_back(btVector3(0.1, 0.1, 0.1));
	sizes.push_back(btVector3(0.5, 0.5, 0.5));
	sizes.push_back(btVector3(0.1, 0.5, 0.5));
	sizes.push_back(btVector3(0.5, 0.1, 0.5));
	sizes.push_back(btVector3(0.2, 0.3, 0.2));
}

void TutorialApplication::addLocations(){
	targetLocations.push_back(btVector3(1500, 30, 1800)); // right of camera
	targetLocations.push_back(btVector3(1150, 90, 1650)); // behind camera
	targetLocations.push_back(btVector3(1500, 60, 1200)); // left of camera
	targetLocations.push_back(btVector3(2000, 30, 1880));
	targetLocations.push_back(btVector3(2200, 30, 1700));
	targetLocations.push_back(btVector3(2100, 30, 1500));
	targetLocations.push_back(btVector3(2400, 30, 1550));
	targetLocations.push_back(btVector3(3500, 200, 1975));
	targetLocations.push_back(btVector3(2263, 30, 2000));
}

void TutorialApplication::createScene()
{
  addLocations();
  addSizes();

  mCamera->setPosition(Ogre::Vector3(1500, 30, 1650));
  mCamera->lookAt(Ogre::Vector3(1963, 30, 1685));
  mCamera->setNearClipDistance(.1);
 
  bool infiniteClip =
    mRoot->getRenderSystem()->getCapabilities()->hasCapability(
      Ogre::RSC_INFINITE_FAR_PLANE);
 
  if (infiniteClip)
    mCamera->setFarClipDistance(0);
  else
    mCamera->setFarClipDistance(50000);
 
  mSceneMgr->setAmbientLight(Ogre::ColourValue(.2, .2, .2));
 
  Ogre::Vector3 lightDir(.55, -.3, .75);
  lightDir.normalise();
 
  Ogre::Light* light = mSceneMgr->createLight("TestLight");
  light->setType(Ogre::Light::LT_DIRECTIONAL);
  light->setDirection(lightDir);
  light->setDiffuseColour(Ogre::ColourValue::White);
  light->setSpecularColour(Ogre::ColourValue(.4, .4, .4));
 
  // Fog
 Ogre::ColourValue fadeColour(.9, .9, .9);
  mWindow->getViewport(0)->setBackgroundColour(fadeColour);
 
//mSceneMgr->setFog(Ogre::FOG_EXP2, fadeColour, 0.002);
 
  // Terrain
  mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
 
  mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(
    mSceneMgr,
    Ogre::Terrain::ALIGN_X_Z,
    513, 12000.0);
  mTerrainGroup->setFilenameConvention(Ogre::String("terrain"), Ogre::String("dat"));
  mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
 
  configureTerrainDefaults(light);
 
  for (long x = 0; x <= 0; ++x)
    for (long y = 0; y <= 0; ++y)
      defineTerrain(x, y);
 
  mTerrainGroup->loadAllTerrains(true);
 
  if (mTerrainsImported)
  {
    Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
 
    while (ti.hasMoreElements())
    {
      Ogre::Terrain* t = ti.getNext()->instance;
      initBlendMaps(t);
    }
  }
 
  mTerrainGroup->freeTemporaryResources();
 
  // Sky Techniques
  // mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 300, false);
  mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);


  mCamera2->setPosition(Ogre::Vector3(1963, 800, 1660));
  mCamera2->pitch(Ogre::Radian(4.71F));
  mCamera2->rotate(Ogre::Vector3::NEGATIVE_UNIT_Y, Ogre::Degree(90.0f));
  createBulletSim();


  mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");

  CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
  CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

  CEGUI::Window *Rebuild = wmgr.createWindow("TaharezLook/Button", "Button");
  Rebuild->setText("Y To Restart");
  Rebuild->setSize(CEGUI::USize(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
  Rebuild->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0.10, 0)));

  //Creates the textbox for the program
  itemsLeft = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/Editbox"); 
  char* targetsLeft = (char*)malloc(3 + strlen(" items left") + 1);
  itoa(itemsLeftOver, targetsLeft, 10);
  strcat(targetsLeft, " items left");
  itemsLeft->setText(CEGUI::String(targetsLeft));
  itemsLeft->setSize(CEGUI::USize(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));

  //Creates the textbox for the program
  time = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/Editbox");
  time->setText("");
  time->setSize(CEGUI::USize(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
  time->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0.05, 0)));

  pointsWindow = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/Editbox");
  pointsWindow->setText("0 Points");
  pointsWindow->setSize(CEGUI::USize(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
  pointsWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0, 0)));

  sheet->addChild(Rebuild);
  sheet->addChild(itemsLeft);
  sheet->addChild(time);
  sheet->addChild(pointsWindow);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);
}
 
void TutorialApplication::createFrameListener()
{
  BaseApplication::createFrameListener();
}
 
void TutorialApplication::destroyScene()
{
  OGRE_DELETE mTerrainGroup;
  OGRE_DELETE mTerrainGlobals;
}

bool TutorialApplication::frameStarted(const Ogre::FrameEvent &evt)
{

	for (int i = 0; i < ptrToOgreObjects.size(); i++){
		if (ptrToOgreObjects[i]->objectType.substr(0, 10) == "Projectile"){
			ptrToOgreObjects[i]->timeAlive += evt.timeSinceLastFrame;
			if (ptrToOgreObjects[i]->timeAlive >= 5){
				removeObject(ptrToOgreObjects[i]);
			}
		}
	}

	dynamicsWorld->stepSimulation(evt.timeSinceLastFrame);
	return true;
}

void TutorialApplication::handleCollisions(std::vector<contactPair> pairs){

	for (int i = 0; i < pairs.size(); i++) {
		if (!((pairs[i].ptrToOgreObject1->objectType.substr(0, 10) == "Projectile" && pairs[i].ptrToOgreObject2->objectType.substr(0, 4) == "Cube") || (pairs[i].ptrToOgreObject2->objectType.substr(0, 10) == "Projectile" && pairs[i].ptrToOgreObject1->objectType.substr(0, 4) == "Cube"))) {
			pairs.erase(pairs.begin() + i);
			i--;
		}
		else{
			points += 500;
			removeObject(pairs[i].ptrToOgreObject1);
			removeObject(pairs[i].ptrToOgreObject2);
			char* targetsLeft = (char*)malloc(3 + strlen(" items left") + 1);
			itemsLeftOver--;
			itoa(itemsLeftOver, targetsLeft, 10);
			strcat(targetsLeft, " items left");
			itemsLeft->setText(CEGUI::String(targetsLeft));
			pairs.erase(pairs.begin() + i);
			i--;
		}
	}

	contactPairs.clear();
}

// Called in getCollisionPairs function
// sets the ptr to ogreObject to the ogreObject based on btCollisionObject
ogreObject* TutorialApplication::getOgreObject(const btCollisionObject * obj) {
	for (int i = 0; i < ptrToOgreObjects.size(); ++i) {
		if (ptrToOgreObjects[i]->btCollisionObjectObject == obj)
			return ptrToOgreObjects[i];

	}
	ogreObject* ret = new ogreObject;
	return ret;

}

void TutorialApplication::getContactPairs(std::vector<contactPair> &contactPairs) {

	//dynamicsworld->stepsimulation called in frameStarted function
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++) {

		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++) {
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f) {
				// really, all we need to keep is the 1st time an object contacts because we want to deal with contact as soon as it happens
				// so subsequent contacts can be ignored

				// temporary to hold extracted 1st contact pair information
				contactPair pushToVec;

				// get the pointers to the pair of contact objects
				pushToVec.collisionObject1 = contactManifold->getBody0();
				pushToVec.collisionObject2 = contactManifold->getBody1();

				// up cast those pointers to get the pair of rigidbodypointers
				pushToVec.rigidBodyObject1 = ((btRigidBody*)contactManifold->getBody0());
				pushToVec.rigidBodyObject2 = ((btRigidBody*)contactManifold->getBody1());

				pushToVec.ptrToOgreObject1 = getOgreObject(contactManifold->getBody0());
				pushToVec.ptrToOgreObject2 = getOgreObject(contactManifold->getBody1());

				pushToVec.ptrToOgreObject1Ptr = &(pushToVec.ptrToOgreObject1);
				pushToVec.ptrToOgreObject2Ptr = &(pushToVec.ptrToOgreObject2);

				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				// not using the normal
				const btVector3& normalOnB = pt.m_normalWorldOnB;

				// get the contact position out of bullet and put it in an ogre vector
				pushToVec.positionObject1.x = ptA.getX();
				pushToVec.positionObject1.y = ptA.getY();
				pushToVec.positionObject1.z = ptA.getZ();
				pushToVec.positionObject2.x = ptB.getX();
				pushToVec.positionObject2.y = ptB.getY();
				pushToVec.positionObject2.z = ptB.getZ();

				contactPairs.push_back(pushToVec);

			}
		}
	}
}
 
bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
  bool ret = BaseApplication::frameRenderingQueued(fe);
  if (itemsLeftOver > 0){
	  timeInt += fe.timeSinceLastFrame;
	  std::ostringstream ss;
	  ss << timeInt;
	  std::string str = ss.str();
	  time->setText(CEGUI::String(str.c_str()));
  }

  // this should hold collision information, is this in the right place?
  getContactPairs(contactPairs);

  handleCollisions(contactPairs);

  processUnbufferedInput(fe);

  char *pointsVal = (char*)malloc(10 + strlen(" Points"));
  itoa(points, pointsVal, 10);
  strcat(pointsVal, " Points");
  pointsWindow->setText(CEGUI::String(pointsVal));

  return ret;
}

void TutorialApplication::processUnbufferedInput(const Ogre::FrameEvent& fe){
	if (!mKeyboard->isKeyDown(OIS::KC_Y) && isDown) {
		isDown = false;
	}
	if (mKeyboard->isKeyDown(OIS::KC_Y) && !isDown) {
		resetTargets();
		isDown = true;
	}
	if (mKeyboard->isKeyDown(OIS::KC_SPACE)){
		fire = true;
		power += (fe.timeSinceLastFrame*4);
	}
	else if (!mKeyboard->isKeyDown(OIS::KC_SPACE) && fire){
		fire = false; 
		points -= 50;
		char *projNum = (char*)malloc(3);
		itoa(numOfSpheres, projNum, 10);
		char *projName = (char*)malloc(strlen(projNum) + strlen("Projectile") + 1);
		strcpy(projName, "Projectile");
		strcat(projName, projNum);
		CreateSphere(btVector3(mCamera->getPosition().x, mCamera->getPosition().y, mCamera->getPosition().z), 1.0f, btVector3(0.05, 0.05, 0.05), projName, power);
		power = 0;
		numOfSpheres++;
	}
}
 
void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
  img.load("terrain.png",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  if (flipX)
    img.flipAroundY();
  if (flipY)
    img.flipAroundX();
 
}
 
void TutorialApplication::defineTerrain(long x, long y)
{
  Ogre::String filename = mTerrainGroup->generateFilename(x, y);
 
  bool exists =
    Ogre::ResourceGroupManager::getSingleton().resourceExists(
      mTerrainGroup->getResourceGroup(),
      filename);
 
  if (exists)
    mTerrainGroup->defineTerrain(x, y);
  else
  {
    Ogre::Image img;
    getTerrainImage(x % 2 != 0, y % 2 != 0, img);
    mTerrainGroup->defineTerrain(x, y, &img);
 
    mTerrainsImported = true;
  }
}
 
void TutorialApplication::initBlendMaps(Ogre::Terrain* terrain)
{
  Ogre::Real minHeight0 = 70;
  Ogre::Real fadeDist0 = 40;
  Ogre::Real minHeight1 = 70;
  Ogre::Real fadeDist1 = 15;
 
  Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
  Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
 
  float* pBlend0 = blendMap0->getBlendPointer();
  float* pBlend1 = blendMap1->getBlendPointer();
 
  for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
  {
    for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
    {
      Ogre::Real tx, ty;
 
      blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
      Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
      Ogre::Real val = (height - minHeight0) / fadeDist0;
      val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
      *pBlend0++ = val;
 
      val = (height - minHeight1) / fadeDist1;
      val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
      *pBlend1++ = val;
    }
  }
 
  blendMap0->dirty();
  blendMap1->dirty();
  blendMap0->update();
  blendMap1->update();
 
}
 
void TutorialApplication::configureTerrainDefaults(Ogre::Light* light)
{
  mTerrainGlobals->setMaxPixelError(8);
  mTerrainGlobals->setCompositeMapDistance(3000);
 
  mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
  mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
  mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
 
  Ogre::Terrain::ImportData& importData = mTerrainGroup->getDefaultImportSettings();
  importData.terrainSize = 513;
  importData.worldSize = 12000.0;
  importData.inputScale = 600;
  importData.minBatchSize = 33;
  importData.maxBatchSize = 65;
 
  importData.layerList.resize(3);
  importData.layerList[0].worldSize = 100;
  importData.layerList[0].textureNames.push_back(
    "dirt_grayrocky_diffusespecular.dds");
  importData.layerList[0].textureNames.push_back(
    "dirt_grayrocky_normalheight.dds");
  importData.layerList[1].worldSize = 30;
  importData.layerList[1].textureNames.push_back(
    "grass_green-01_diffusespecular.dds");
  importData.layerList[1].textureNames.push_back(
    "grass_green-01_normalheight.dds");
  importData.layerList[2].worldSize = 200;
  importData.layerList[2].textureNames.push_back(
    "growth_weirdfungus-03_diffusespecular.dds");
  importData.layerList[2].textureNames.push_back(
    "growth_weirdfungus-03_normalheight.dds");
 
}
 
 
#if Ogre_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
  int main(int argc, char *argv[])
#endif
  {
    // Create application object
    TutorialApplication app;
 
    try {
      app.go();
    } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
      std::cerr << "An exception has occured: " <<
	e.getFullDescription().c_str() << std::endl;
#endif
    }
 
    return 0;
  }
 
#ifdef __cplusplus
}
#endif