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

#include "Common.h"
#include "Singleton.h"
#include "ListenerHolder.h"
#include <map>
#include <Windows.h>

BEGIN_NAMESPACE

class IScene;
class IInputManager;
class IRenderWindow;
class IRenderEngine;
class IPhysicsEngine;
class ISoundSystem;
class IVirtualMachine;
class IGUISystem;


class DLL_DECLARATION Core
{
	DECLARE_SINGLETON(Core)

	BEGIN_DECLARE_LISTENER_HOLDER
		virtual void OnBeforeSceneRemoved(IScene *Scene) = 0;
		virtual void OnSceneAdded(IScene *Scene) = 0;
		
		virtual void OnSetCurrentScene(IScene *Scene) = 0;
		
		virtual void OnBeforeUpdate(void) = 0;
		virtual void OnAfterUpdate(void) = 0;
		
		virtual void OnBeforeRender(void) = 0;
		virtual void OnAfterRender(void) = 0;
	END_DECLARE_LISTENER_HOLDER

public:
	Core(void);
	~Core(void);

	int Shutdown(void);
	
	IRenderWindow *Initialize(const bool &AutoCreateRenderWindow = true);
	IRenderWindow *Initialize(const HWND &WindowHandle);
	
private:
	IRenderWindow *CreateRenderWindow(const String &Name, const String &Text);
	IRenderWindow *CreateRenderWindow(const String &Name, const HWND &WindowHandle);

public:
	void UpdateOneFrame(void);

	IScene *CreateScene(const String &FileName, const bool &InternalUse = false, const bool &LoadSync = false);
	void DestroyScene(IScene *Scene);
	void DestroyScene(const String &FileName);
	IScene *GetScene(const String &FileName);
	
	void SetCurrentScene(const String &FileName);
	void SetCurrentScene(IScene *Scene);
	IScene *GetCurrentScene(void)
	{
		return m_CurrentScene;
	}

	unsigned int GetFPS(void) const;

	void InitializeDataPath(const String &Path = "");

private:
	void InitializeCore(IRenderWindow *Window);

public:
	const String &GetEngineName(void) const
	{
		return m_EngineName;
	}

	const String &GetInitializePath(void) const
	{
		return m_InitializePath;
	}

	const String &GetDataPath(void) const
	{
		return m_DataPath;
	}

	const String &GetAnimationPath(void) const
	{
		return m_AnimationPath;
	}

	const String &GetFontPath(void) const
	{
		return m_FontPath;
	}

	const String &GetMaterialPath(void) const
	{
		return m_MaterialPath;
	}

	const String &GetMoviePath(void) const
	{
		return m_MoviePath;
	}

	const String &GetScenePath(void) const
	{
		return m_ScenePath;
	}

	const String &GetScriptPath(void) const
	{
		return m_ScriptPath;
	}

	const String &GetSoundPath(void) const
	{
		return m_SoundPath;
	}

	const String &GetTexturePath(void) const
	{
		return m_TexturePath;
	}

	const String &GetAndroidPath(void) const
	{
		return m_AndroidPath;
	}

	void SetInputManager(IInputManager *InputManager)
	{
		m_InputManager = InputManager;
	}

	IInputManager *GetInputManager(void) const
	{
		return m_InputManager;
	}

	void SetRenderer(IRenderEngine *Renderer)
	{
		m_Renderer = Renderer;
	}

	IRenderEngine *GetRenderer(void) const
	{
		return m_Renderer;
	}

	void SetPhysics(IPhysicsEngine *Physics)
	{
		m_Physics = Physics;
	}

	IPhysicsEngine *GetPhysics(void) const
	{
		return m_Physics;
	}

	void SetSoundSystem(ISoundSystem *SoundSystem)
	{
		m_SoundSystem = SoundSystem;
	}

	ISoundSystem* GetSoundSystem(void)
	{
		return m_SoundSystem;
	}

	void SetGUISystem(IGUISystem *GUISystem)
	{
		m_GUISystem = GUISystem;
	}

	IGUISystem* GetGUISystem(void)
	{
		return m_GUISystem;
	}

	const bool &GetIsInitialized(void) const
	{
		return m_IsInitialized;
	}

	IRenderWindow *GetRenderWindow(void) const
	{
		return m_RenderWindows.begin()->second;
	}

private:
	typedef std::map<String, IRenderWindow*> RenderWindowsMap;
	//typedef std::map<String, IScene*> ScenesMap;
	typedef Vector<IScene*> ScenesList;

	String m_EngineName;
	String m_InitializePath;
	String m_DataPath;
	String m_AnimationPath;
	String m_FontPath;
	String m_MaterialPath;
	String m_MoviePath;
	String m_ScenePath;
	String m_ScriptPath;
	String m_SoundPath;
	String m_TexturePath;
	String m_AndroidPath;
	
#ifndef USE_RENDERER_FPS_SYSTEM
	unsigned int m_TempRealTime;
	unsigned int m_LastFPSChangeTime;
	unsigned int m_FramesCount;
	unsigned int m_FPS;
#endif

	IInputManager *m_InputManager;
	IRenderEngine *m_Renderer;
	IPhysicsEngine *m_Physics;
	ISoundSystem *m_SoundSystem;
	IGUISystem *m_GUISystem;

	bool m_IsInitialized;

	RenderWindowsMap m_RenderWindows;
	ScenesList m_Scenes;
	IScene *m_CurrentScene;
};

END_NAMESPACE