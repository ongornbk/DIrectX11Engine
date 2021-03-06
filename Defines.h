#pragma once
#define DEBUGG_DEV_MODE
#define DIRECTSOUND_VERSION  0x0800
#define GAME_NAME_VERSION "ONGORNBK"
#pragma region
#define SETTINGS Settings::get()->
#define CAMERA   Camera::GetCurrentCamera()->
#define GLOBAL   Global::GetInstance()->
#define GRAPHICS Engine::GetEngine()->GetGraphics()->
#define ENGINE Engine::GetEngine()->
#define FRAMEWORK Engine::GetEngine()->GetFrameWork()->
#define GETSHADER  (TextureShader*)ResourceManager::GetInstance()->GetShaderByName(
#define END       );
#define CLOSE     )
#pragma endregion