#include "spawn.h"

std::string getINIString(int type)
{
	std::string val;
	char path[MAX_PATH], key[128];
	GetCurrentDirectory(MAX_PATH, path);
	sprintf_s(path, "%s\\%s", path, MAININI);

	if (type == 1) // token
	{
		GetPrivateProfileString("Client", "Token", "", key, 128, path);
		val = key;
	}

	return val;
}