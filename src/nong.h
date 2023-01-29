
#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <filesystem>
#include <string>

#include <gd.h>
#include <cocos2d.h>

#include <MinHook.h>

#include "extensions2.h"

using namespace cocos2d;

namespace PlayLayer {
	inline bool(__thiscall* init)(gd::PlayLayer* self, gd::GJGameLevel* GJGameLevel);
	bool __fastcall initHook(gd::PlayLayer* self, int edx, gd::GJGameLevel* GJGameLevel);
}
