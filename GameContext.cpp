#include "GameContext.h"

#include "EventManager.h"
#include "InputManager.h"
#include "GameObjectManager.h"
#include "MeshManager.h"
#include "RenderingContext.h"
#include "GameInfo.h"

GameContext::GameContext()
{
	// register self as game context
	GameInfo::gameContext = this;

	eventManager = new EventManager();
	inputManager = new InputManager();
	gameObjectManager = new GameObjectManager();
	meshManager = new MeshManager();
	renderingContext = new RenderingContext();
}

GameContext::~GameContext()
{
	delete gameObjectManager;
	delete meshManager;
	delete renderingContext;
	delete inputManager;
	delete eventManager;
}