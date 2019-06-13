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

#include "IBaseObject.h"
#include "Vector2D.h"
#include "AABB.h"
#include "OBB.h"
#include "Colour.h"
#include "IAnimationSet.h"
#include "IMaterial.h"

BEGIN_NAMESPACE

//class IGameObject;
class ITexture;

//<Description>
//This is sample description
class IRenderOperation
{
public:
	virtual ~IRenderOperation(void) {}

	//virtual IGameObject *GetHolder(void) const = 0;

	virtual void SetColour(const Colour &Colour) = 0;
	virtual const Colour &GetColour(void) = 0;
	
	virtual void SetMaterial(const String &MaterialName) = 0;
	virtual void SetMaterial(IMaterial *Material) = 0;
	virtual IMaterial *GetMaterial(void) const = 0;
	
	virtual void SetAnimationSet(const String &FileName) = 0;
	virtual void SetAnimationSet(IAnimationSet *Set) = 0;
	virtual IAnimationSet *GetAnimationSet(void) = 0;
	virtual void SetCurrentAnimation(const String &Name) = 0;
	virtual IAnimation *GetCurrentAnimation(void) = 0;
	
	virtual void SetSize(const float &Width, const float &Height) = 0;
	virtual void SetSize(const Vector2D &Size) = 0;
	virtual const Vector2D &GetSize(void) const = 0;
	
	virtual void SetTexCoords(const Vector2D &TopLeftUV, const Vector2D &BottomRightUV) = 0;
	virtual const Vector2D &GetTopLeftUV(void) = 0;
	virtual const Vector2D &GetBottomRightUV(void) = 0;

	virtual void SetQueueID(const int &QueueID) = 0;
	virtual const int &GetQueueID(void) const = 0;

	virtual void ManualUpdate(Vector3D &WorldPosition, AABB &WorldAABB, OBB &WorldOBB) = 0;
};

END_NAMESPACE