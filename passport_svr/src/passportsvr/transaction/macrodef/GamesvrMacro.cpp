#include "GamesvrMacro.h"

//��MacroUtil��ֵ��ֵΪ���Ӧ��MacroGameSvr��ֵ
#define MACRODEF(macroval) const uint32_t MacroUtil##macroval=MacroGamesvr##macroval;

#include "tranbase/macrodef/UtilMacro.def"

#undef MACRODEF

#define MACRODEF(macroval) "[MacroGamesvr"#macroval"]",

const char* MacroNameArray[] =
{
#include "GamesvrMacro.def"
#include "tranbase/macrodef/UtilMacro.def"
};
//int a = MacroGamesvrUtilEnd;
//a = MacroUtilUtilEnd;
#undef MACRODEF
