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

#include "ISceneManager.h"
#include "IRenderWindow.h"
#include "IAnimationSet.h"
#include "IImage.h"
#include "IMaterial.h"
#include "IMovieManager.h"

BEGIN_NAMESPACE

class ITexture;

class IRenderEngine
{
public:
	virtual ~IRenderEngine(void) {}

	virtual void Update(void) = 0;
	
	virtual void BeginRenderCall(const Colour &Colour = Colour_BLACK) = 0;
	virtual void EndRenderCall(void) = 0;

	virtual void Draw2DTexture(const ITexture *Texture, const Vector2D &Destination, const bool &UseTransparency = false, const Colour &Colour = Colour_WHITE) = 0;

	virtual bool Initialize(IRenderWindow *Window) = 0;

	virtual ISceneManager *CreateSceneManager(void) = 0;
	virtual void DestroySceneManager(ISceneManager *SceneManager) = 0;

	virtual IImage *CreateImage(const String &Name, const Vector2D &Size) = 0;
	virtual IImage *LoadImageFile(const String &FileName) = 0;
	virtual void DestroyImage(IImage *Img) = 0;

	virtual ITexture *LoadTexture(const String &RelativeFilePath) = 0;
	virtual ITexture *CreateTexture(const String &Name, IImage *Image) = 0;
	virtual ITexture *CreateTexture(const String &Name, const Vector2D &Size) = 0;
	virtual void DestroyTexture(ITexture *Tex) = 0;
	virtual void DestroyAllTextures(void) = 0;
	virtual void ReloadAllTextures(void) = 0;
	
#ifndef LAUNCH_MODE
	virtual IAnimationSet *CreateAnimationSet(const String &Name) = 0;
#endif
	virtual IAnimationSet *LoadAnimationSet(const String &FileName) = 0;
	
#ifndef LAUNCH_MODE
	virtual IMaterial *CreateMaterial(const String &Name) = 0;
#endif
	virtual IMaterial *LoadMaterial(const String &FileName) = 0;

	virtual IRenderWindow *GetRenderWindow(void) = 0;

	virtual IMovieManager *GetMovieManager(void) = 0;

	virtual void SetWindowSize(const Vector2D &Size) = 0;

	virtual void DrawLine(const Vector3D &Start, const Vector3D &End, const Colour &Colour = Colour_WHITE) = 0;
	virtual void DrawPolygon(const Vector3D &Center, const float &Radius, const unsigned int &VertexCount = 20, const Colour &Colour = Colour_WHITE) = 0;
	virtual void DrawPolygon(const List<Vector2D> &Vertices, const Vector3D &Center, const Colour &Colour = Colour_WHITE) = 0;
	virtual void DrawPolygon(const Vector2D &Size, const Vector3D &Center, const Colour &Colour = Colour_WHITE) = 0;
	virtual void DrawAABB(const AABB &AABB, const Colour &Colour = Colour_WHITE) = 0;
	virtual void DrawOBB(const OBB &OBB, const Colour &Colour = Colour_WHITE) = 0;

	virtual void SetDefaultTextureCreationFlags(void) = 0;
	virtual void SetHighQualityTextureCreationFlags(void) = 0;

#ifdef USE_RENDERER_FPS_SYSTEM
	virtual unsigned int GetFPS(void) const = 0;
#endif
};

END_NAMESPACE