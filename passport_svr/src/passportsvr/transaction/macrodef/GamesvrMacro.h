/*******************************************************************************
@ ��Ȩ����(C) Tencent 2012
********************************************************************************/
#ifndef __GAMESVR_MACRODEF_MACRO_H__
#define __GAMESVR_MACRODEF_MACRO_H__

#include "tranbase/macrodef/UtilMacro.h"
#include "tranbase/Util.h"

//����MacroGameSvr�ĺ�
#define MACRODEF(macroval) MacroGamesvr##macroval,

enum GameSvrMacro
{
//��GameSvr��RedUtil��Util�ĺ궼����͸�ֵ
#include "GamesvrMacro.def"
#include "tranbase/macrodef/UtilMacro.def"
};

#undef MACRODEF
//ʹ�õ�ʱ��ͨ��MACROGAMESVR��������
#define MACROGAMESVR(macroval) MacroGamesvr##macroval


#endif // end of __GAMESVR_MACRODEF_MACRO_H__
