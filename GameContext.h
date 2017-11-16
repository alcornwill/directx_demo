#pragma once

class GameObjectManager;
class InputManager;
class MeshManager;
class RenderingContext;
class EventManager;

// just neater than passing loads of parameters everywhere
class GameContext
{
public:
	GameContext();
	~GameContext();

	GameObjectManager * gameObjectManager;
	InputManager * inputManager;
	MeshManager * meshManager;
	RenderingContext * renderingContext;
	EventManager * eventManager;
};