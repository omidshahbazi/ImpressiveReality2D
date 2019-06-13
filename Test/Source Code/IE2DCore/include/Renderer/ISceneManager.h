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

#include "Vector2D.h"
#include "Line3D.h"
#include "ICamera.h"
#include "IRenderOperation.h"

BEGIN_NAMESPACE

class IGameObject;

class ISceneManager
{
public:
	virtual ~ISceneManager(void) {}

	virtual void Clear(void) = 0;
	
	virtual void Render(void) = 0;

	virtual IRenderOperation *CreateRenderOpertaion(IGameObject *Holder) = 0;
	virtual void DestroyRenderOpertaion(IRenderOperation *RenderOperation) = 0;

	virtual ICamera *GetCamera(void) const = 0;

	virtual Line3D GetRayFromScreenCoordinates(const Vector2D &MousePosition) = 0;

	virtual Vector3D GetMousePositionInScene(const Vector2D &MousePosition, const float &Length) = 0;

};

END_NAMESPACE