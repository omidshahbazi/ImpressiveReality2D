///////////////////////////////////////////////////////////////////////////////////
///
///  Impressive Engine 2D
///
/// Copyright (c) 2012-2013 Impressive Reality team
///
/// The license is
///
/// Permission is denied, to any person or company
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// Project leader : O.Shahbazi <sh_omid_m@yahoo.com>
///////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IGameObject.h"

BEGIN_NAMESPACE
	
class IRenderOperation;
class ICamera;
class IPhysicsScene;
class IGUIScene;
struct Line3D;

//<Description>
//This is sample description
class IScene : public ISerializable
{
public:
	enum SceneState
	{
		SS_STOPPED = 0,
		SS_PLAYING
	};

	BEGIN_DECLARE_LISTENER_HOLDER
		virtual void OnReloadAll(IGameObject *RootGameObject) = 0;
		virtual void OnGameObjectAdded(IGameObject *GameObject) = 0;
		virtual void OnBeforeGameObjectRemoved(IGameObject *GameObject) = 0;
		virtual void OnAfterGameObjectRemoved(void) = 0;
		virtual void OnBeforeGameModified(IGameObject *GameObject) = 0;
		virtual void OnAfterGameModified(IGameObject *GameObject) = 0;
	END_DECLARE_LISTENER_HOLDER

public:
	virtual ~IScene(void) {}

	virtual void Clear(void) = 0;

	virtual bool Save(const String &FileName) = 0;
	virtual void SetInitializeState(void) = 0;
	virtual void ResetToInitializeState(void) = 0;

	virtual void SetState(const SceneState &State) = 0;
	virtual const SceneState &GetState(void) = 0;

	virtual const String &GetName(void) const = 0;

	virtual IGameObject *CreateGameObject(const String &Name) = 0;

	virtual void Update(void) = 0;

	virtual void Render(void) = 0;

	//virtual ISceneManager *GetSceneManager(void) const = 0;

	//virtual IGameObject *GetRootGameObject(void) const = 0;

	virtual Line3D GetRayFromScreenCoordinates(const Vector2D &MousePosition) = 0;

	virtual Vector3D GetMousePositionInScene(const Vector2D &MousePosition, const float &Length) = 0;

	virtual ICamera *GetCamera(void) = 0;
	
	virtual void SetClearColour(const Colour &Colour) = 0;
	virtual const Colour &GetClearColour(void) const = 0;
	
	virtual IPhysicsScene *GetPhysicsScene(void) = 0;

	virtual IGUIScene *GetGUIScene(void) = 0;

	virtual const bool GetLoaded(void) const = 0;
};

END_NAMESPACE