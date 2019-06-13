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

#include "ISerializable.h"
#include "IBaseObject.h"
#include "AABB.h"
#include "OBB.h"
#include "Physics\IBody.h"
#include "Renderer\IRenderOperation.h"

BEGIN_NAMESPACE

class ITransform;
class IScene;
class Component;

class IGameObject : public ISerializable
{
public:
	typedef Vector<IGameObject*> GameObjectsList;

public:
	virtual ~IGameObject(void) {}

	virtual void Update(void) = 0;

	virtual void Render(void) = 0;

	virtual Component *AddComponent(const String &Type) = 0;

	virtual IScene *GetHolder(void) = 0;

	virtual void SetTag(const unsigned int &Value) = 0;
	virtual const unsigned int &GetTag(void) const = 0;

	virtual void SetName(const String &Name) = 0;
	virtual const String &GetName(void) const = 0;

	virtual void SetVisible(const bool &Visible) = 0;
	virtual const bool &GetVisible(void) const = 0;

	virtual ITransform *GetTransform(void) = 0;

	virtual IRenderOperation *GetRenderOperation(void) const = 0;

	virtual IBody *CreateBody(const IBody::BodyType &BodyType = IBody::BT_STATIC) = 0;
	virtual void AddBody(IBody *Body) = 0;
	virtual void RemoveBody(void) = 0;
	virtual void DestroyBody(void) = 0;
	virtual IBody *GetBody(void) = 0;
	
	virtual void AddGameObject(IGameObject *GameObject) = 0;
	virtual void RemoveGameObject(IGameObject *GameObject) = 0;
	virtual void Destroy(void) = 0;
	virtual void DestroyGameObject(IGameObject *GameObject) = 0;
	virtual IGameObject *CreateGameObject(const String &Name) = 0;
	
	virtual IGameObject *GetGameObject(const String &Name) = 0;
	virtual IGameObject *GetGameObject(const Vector2D &Position) = 0;
	virtual IGameObject *GetGameObject(const unsigned int &Tag) = 0;
	virtual GameObjectsList GetGameObjects(const String &Name) = 0;
	virtual GameObjectsList GetGameObjects(const String &Name, const bool &SearchInChildren) = 0;
	virtual GameObjectsList GetGameObjects(const Vector2D &Position) = 0;
	virtual GameObjectsList GetGameObjects(const unsigned int &Tag) = 0;

	virtual IGameObject *Clone(const String &Name, IGameObject *NewParent) = 0;
};

END_NAMESPACE