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
#include "LauncherCommon.h"
#include "Core.h"
#include "IRenderWindow.h"
#include "IScene.h"

#ifdef FULL_DEBUG_MODE
#include "Utility.h"
#include "FileIO.h"
#endif

USING_NAMESPACE

//#ifndef LAUNCH_MODE
//#error To building Launcher.exe, you must define LAUNCH_MODE preprocessor in Common.h
//#endif

#ifdef FULL_DEBUG_MODE
int main()
#else
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd)
#endif
{
	Core &core = Core::GetReference();

	IRenderWindow *rw = core.Initialize();

#ifdef FULL_DEBUG_MODE
	const String settingFileName("EditorSettings.s");

	if (Utility::FileExists(Core::GetReference().GetInitializePath() + settingFileName))
	{
		ITreeParser *tr = FileIO::GetReference().CreateTreeParser("IE2DSettingFile", Core::GetReference().GetInitializePath() + settingFileName);

		TreeElement *stateElem = tr->GetRoot()->GetChildren("Settings");

		delete tr;
	}
#endif

	IScene *scene = core.CreateScene("", true);
	scene->CreateGameObject("aaaa")->AddComponent("DummyCom");

	core.SetCurrentScene(scene);

	while (!rw->IsClosed())
		core.UpdateOneFrame();

	return core.Shutdown();
}