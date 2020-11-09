#include "spawn.h"

#include "..\algorithms.h"

void botInfo::gotHelp(Player *p, Command *c)
{
	// List commands

	if (!*c->final)
	{
		switch (p->access)
		{
		case OP_Duke:
		case OP_Baron:
		case OP_King:
		case OP_Emperor:
		case OP_RockStar:
		case OP_Q:
		case OP_God:
		case OP_Owner:
			{	// Owner-level commands
//				sendPrivate(p, "Ext: ");
			}
			break;
		case OP_SysOp:
			{	// SysOp-level commands
//				sendPrivate(p, "Ext: ");
			}
			break;
		case OP_SuperModerator:
			{	// SuperModerator-level commands
//				sendPrivate(p, "Ext: ");
			}
			break;
		case OP_Moderator:
			{	// Moderator-level commands
//				sendPrivate(p, "Ext: ");
			}
			break;
		case OP_Limited:
			{	// Limited-level commands
			}
		case OP_Player:
			{	// Player-level commands
				sendPrivate(p, "!about (query me about my function)");
			}
		}

		return;
	}

	// Specific command help

	switch (p->access)
	{
	case OP_Duke:
	case OP_Baron:
	case OP_King:
	case OP_Emperor:
	case OP_RockStar:
	case OP_Q:
	case OP_God:
	case OP_Owner:
		{	// Owner-level commands
			if (c->checkParam("owner") || c->checkParam("all"))
			{
//				sendPrivate(p, "Ext: ");
			}
		}
	case OP_SysOp:
		{	// SysOp-level commands
			if (c->checkParam("sop") || c->checkParam("all"))
			{
//				sendPrivate(p, "Ext: ");
			}
		}
	case OP_SuperModerator:
		{	// SuperModerator-level commands
			if (c->checkParam("smod") || c->checkParam("all"))
			{
//				sendPrivate(p, "Ext: ");
			}
		}
	case OP_Moderator:
		{	// Moderator-level commands
			if (c->checkParam("mod") || c->checkParam("all"))
			{
//				sendPrivate(p, "Ext: ");
			}
		}
	case OP_Limited:
		{	// Limited-level commands
		}
	case OP_Player:
		{	// Player-level commands
			if (c->checkParam("about"))
			{
				sendPrivate(p, "!about (query me about my function)");
			}
		}
	}
}

void botInfo::gotCommand(Player *p, Command *c)
{
	if (!p) return;
	if (!c) return;

	switch (p->access)
	{
	case OP_Duke:
	case OP_Baron:
	case OP_King:
	case OP_Emperor:
	case OP_RockStar:
	case OP_Q:
	case OP_God:
	case OP_Owner:
		{	// Owner-level commands
		}
	case OP_SysOp:
		{	// SysOp-level commands
		}
	case OP_SuperModerator:
		{	// SuperModerator-level commands
		}
	case OP_Moderator:
		{	// Moderator-level commands
		if (c->check("read"))
		{
			loadCache();
			sendPrivate(p, "Reloaded new settings from " + (String)MAININI);
		}
		}
	case OP_Limited:
		{	// Limited-level commands
		}
	case OP_Player:
		{	// Player-level commands
			if (c->check("about"))
			{
				sendPrivate(p, "Subspatial v" + (String)BOT_VER);
			}
			else if (c->check("link"))
			{
				if (!*c->final)
				{
					sendPrivate(p, "----------------------------------------------------------");
					sendPrivate(p, "| LINKING DISCORD AND CONTINUUM ACCOUNTS - ELITE TIER    |");
					sendPrivate(p, "----------------------------------------------------------");
					sendPrivate(p, "| Linking your accounts will unlock Elite Tier, a free   |");
					sendPrivate(p, "| premium pass for additional game modes & functionality!|");
					sendPrivate(p, "----------------------------------------------------------");
					sendPrivate(p, "| Step 1: Login to Discord and type --link in main chat  |");
					sendPrivate(p, "| Step 2: Open your DMs and copy the code the bot sent   |");
					sendPrivate(p, "| Step 3: Login to Continuum and PM " + (String)me->name);
					sendPrivate(p, "|         with !link <code>                              |");
					sendPrivate(p, "----------------------------------------------------------");
					sendPrivate(p, "| Type !elite to see all available perks. Have fun!      |");
					sendPrivate(p, "----------------------------------------------------------");
				}
				else
				{
					String ar = c->final;
					String check = ar;
					if (ar.len < 7)
					{
						sendPrivate(p, "You have entered an incorrect code. Please try again or use --link to refresh the code on Discord.");
						break;
					}
					else if (ar.len > 7)
						check = ar.trim(); // remove all spaces

					bool valid = false;

					if (hash.size() > 0) // are there any outstanding codes to claim?
					{
						for (int i = 0; i < hash.size(); i++) // recv'd string format: grEgxvW@usrid
						{
							size_t delim = hash[i].find_first_of("@");
							std::string hash_test = hash[i].substr(0, delim);

							if (strcmp(hash_test.c_str(), check) == 0) // valid code given
							{
								if (!isLinked((String)p->name))
								{
									std::string dName = hash[i].substr(delim, hash[i].npos - delim); // dName = usrid
									dName.erase(0, 1);   // remove the @

									sendPrivate(p, SND_Ahhhh, "Congratulations! You have unlocked the Elite Tier by linking your Continuum account to Discord. Type !elite to see available perks.");
									linkAccount(p, (String)dName.c_str()); // write to file
									std::thread([=](){ grantDiscordElite(dName); }).detach();
									hash[i].clear(); // remove this code from global vector 
									valid = true;
									break; // no need to keep cycling
								}
								else
									sendPrivate(p, "This Continuum account is already registered on Discord! Type --unlink in Discord if you wish to link another account.");
							}
						}
					}
					if (!valid)
						sendPrivate(p, "You have entered an incorrect code. Please try again or use --link to refresh the code on Discord.");
				}
			}
		}
	}
}

void botInfo::gotRemoteHelp(char *p, Command *c, Operator_Level l)
{
	// List commands

	if (!*c->final)
	{
		switch (l)
		{
		case OP_Duke:
		case OP_Baron:
		case OP_King:
		case OP_Emperor:
		case OP_RockStar:
		case OP_Q:
		case OP_God:
		case OP_Owner:
			{	// Owner-level commands
			}
		case OP_SysOp:
			{	// SysOp-level commands
			}
		case OP_SuperModerator:
			{	// SuperModerator-level commands
			}
		case OP_Moderator:
			{	// Moderator-level commands
			}
		case OP_Limited:
			{	// Limited-level commands
			}
		case OP_Player:
			{	// Player-level commands
			}
		}

		return;
	}

	// Specific command help

	switch (l)
	{
	case OP_Duke:
	case OP_Baron:
	case OP_King:
	case OP_Emperor:
	case OP_RockStar:
	case OP_Q:
	case OP_God:
	case OP_Owner:
		{	// Owner-level commands
		}
	case OP_SysOp:
		{	// SysOp-level commands
		}
	case OP_SuperModerator:
		{	// SuperModerator-level commands
		}
	case OP_Moderator:
		{	// Moderator-level commands
		}
	case OP_Limited:
		{	// Limited-level commands
		}
	case OP_Player:
		{	// Player-level commands
		}
	}
}

void botInfo::gotRemote(char *p, Command *c, Operator_Level l)
{
	if (!c) return;

	switch (l)
	{
	case OP_Duke:
	case OP_Baron:
	case OP_King:
	case OP_Emperor:
	case OP_RockStar:
	case OP_Q:
	case OP_God:
	case OP_Owner:
		{	// Owner-level commands
		}
	case OP_SysOp:
		{	// SysOp-level commands
		}
	case OP_SuperModerator:
		{	// SuperModerator-level commands
		}
	case OP_Moderator:
		{	// Moderator-level commands
		}
	case OP_Limited:
		{	// Limited-level commands
		}
	case OP_Player:
		{	// Player-level commands
		}
	}
}
