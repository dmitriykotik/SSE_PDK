#pragma once

#define SSEOVERLAY_INTERFACE001	"SSeOverlay001"

typedef void(__cdecl *_ConsoleCommand)(const char* pszArgs[], uint32 nArgs, void* pContext);

class ISSeOverlay
{
public:
	virtual void ConsoleAddCommand(const char* command, const char* description, const char* usage_example, _ConsoleCommand pCmd, void* pContext) = 0;
	virtual void ConsoleRemoveCommand(const char* command, _ConsoleCommand pCmd) = 0;
	virtual void ConsolePrint(const char* text, uint32 color = 0xffc3c3c3) = 0;
	virtual void ConsoleClear() = 0;
};
