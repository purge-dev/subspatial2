#include "spawn.h"

#include "..\mervbot\algorithms.h"

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
			std::thread([=]()
				{
					loadCache();
					loadLists(0);
					loadLists(1);
					loadLists(2);
				}).detach();

				sendPrivate(p, "Reloaded new settings from " + (String)MAININI + " and " + (String)LISTINI);
		}
		else if (c->check("dmute"))
		{
			if (!*c->final)
				sendPrivate(p, "Discord name not given.");
			else
			{
				if (getUser(c->final) == nullptr)
					sendPrivate(p, "That user is not in Discord. Please use the USERNAME (not nickname) of the user, and check your spelling.");
				else
				{
					aegis::user* usr = getUser(c->final);
					using aegis::edit_channel_permissions_t;

					auto reply = cacher.bot->find_channel(cacher.relayChannel)->edit_channel_permissions(edit_channel_permissions_t()
						.overwrite_id(usr->get_id()).allow(aegis::permissions::VIEW_CHANNEL).deny(aegis::permissions::SEND_MESSAGES).type("1"));
					sendPrivate(p, (String)c->final + " has been muted from the Discord relay channel. Use !dunmute to unmute him/her.");
				}

			}
		}
		else if (c->check("dunmute"))
		{
			if (!*c->final)
				sendPrivate(p, "Discord name not given.");
			else
			{
				if (getUser(c->final) == nullptr)
					sendPrivate(p, "That user is not in Discord. Please use the USERNAME (not nickname) of the user, and check your spelling.");
				else
				{
					aegis::user* usr = getUser(c->final);
					using aegis::edit_channel_permissions_t;
					auto reply = cacher.bot->find_channel(cacher.relayChannel)->delete_channel_permission(usr->get_id());

					if (reply.get().reply_code == 400)
						sendPrivate(p, "That user is not currently muted. Please check your spelling!");
					else
						sendPrivate(p, (String)c->final + " has been unmuted from the Discord relay channel.");
				}
			}
		}
		else if (c->check("mute"))
		{
			if (!*c->final)
				sendPrivate(p, "Player name not given.");
			else
			{
				if (findPlayerByName(c->final) == NULL)
					sendPrivate(p, "That player is not in this arena.");
				else
				{
					Player* mut = findPlayerByName(c->final);
					if (isMuted((String)mut->name))
						sendPrivate(p, "That player is already muted. Use !unmute to unmute him/her.");
					else
					{
						char path[MAX_PATH];
						GetCurrentDirectory(MAX_PATH, path);
						sprintf_s(path, "%s\\%s", path, LISTINI);
						WritePrivateProfileString("Muted", "^" + (String)mut->name, "^$mut", path);
						sendPrivate(p, (String)mut->name + " has been muted and will no longer have his/her messages relayed to Discord.");
						loadLists(0);
					}
				}
			}
		}
		else if (c->check("unmute"))
		{
			if (!*c->final)
				sendPrivate(p, "Player name not given.");
			else
			{
				if (!isMuted((String)c->final))
					sendPrivate(p, "That player is not muted. Please check your spelling (full name required)!");
				else
				{
					char path[MAX_PATH];
					GetCurrentDirectory(MAX_PATH, path);
					sprintf_s(path, "%s\\%s", path, LISTINI);
					WritePrivateProfileString("Muted", "^" + (String)c->final, NULL, path);
					sendPrivate(p, (String)c->final + " has been unmuted.");
					loadLists(0);
				}
			}
		}
		else if (c->check("listmute"))
		{
			sendPrivate(p, "MUTED Players from Continuum:");
			sendPrivate(p, "----------------------------------------------");
			sendPrivate(p, (String)getMutedPlayers().c_str());
			sendPrivate(p, "----------------------------------------------");
			sendPrivate(p, "Use !unmute to unmute a player (full name required). Check Discord's channel permissions to view those muted on Discord.");
		}
		else if (c->check("listelite"))
		{

			sendPrivate(p, "Registered ELITE accounts (DiscordID:ContinuumName)");
			sendPrivate(p, "---------------------------------------------------");
			sendPrivate(p, (String)getElitePairStrings().c_str());
			sendPrivate(p, "---------------------------------------------------");
		}
		else if (c->check("acc"))
		{
		String pname = p->name;
		sendPrivate(p, (String)getKnownAccount(std::string("310006046381047808")).c_str());
		sendPrivate(p, (String)getKnownAccount(p->name).c_str());
		}
	}
	case OP_Limited:
		{	// Limited-level commands
		}
	case OP_Player:
		{	// Player-level commands
		if (c->check("about"))
		{
			sendPrivate(p, "------ [Subspatial v" + (String)BOT_VER + "] -------------------------------------------");
			sendPrivate(p, "| FEATURES: Discord-Continuum cross-platform chat                    |");
			sendPrivate(p, "|           Post-game statistics beautification                      |");
			sendPrivate(p, "|           Epic RPG Discord Metagame                                |");
			sendPrivate(p, "|           Premium User perks via Elite Tier account binding        |");
			sendPrivate(p, "| For more information: https://github.com/purge-dev                 |");
			sendPrivate(p, "----------------------------------------------------- [by Purge] ----");
		}
		else if (c->check("spam"))
		{
			if (isLinked((String)p->name))
			{
				if (countdown[3] > 0)
					sendPrivate(p, "Someone recently used !spam. You must wait " + (String)((countdown[3] / 60) % 60) + " minutes.");
				else
				{
					String msg = "<@&509789987060056074> Calling all pilots to ?go " + (String)arena + " for a game! Enter now to play.";
					std::thread([=]() { curlChatter((String)p->name, msg, p->ship, cacher.spamHook); }).detach();
					
					sendChannel(";" + (String)p->name + " is calling all available pilots to ?go " + (String)arena + " for a game! Enter now to play.");
					sendChannel(";2;" + (String)p->name + " is calling all available pilots to ?go " + (String)arena + " for a game! Enter now to play.");
					sendPrivate(p, "Advertised to Discord and local chat(s).");
					countdown[3] = 900;
				}
			}
			else
			{
				sendPrivate(p, "This command is only available to Elite accounts. Please login to Discord and PM @Subspatial with --link to unlock the Elite Tier!");
				sendPrivate(p, "[NOTE] You may use !extra if you do not wish to link your Discord/Continuum accounts.");
			}
		}
		else if (c->check("extra"))
		{
			if (countdown[4] > 0)
				sendPrivate(p, "Someone recently used !extra. You must wait " + (String)((countdown[5] / 60) % 60) + " minutes.");
			else
			{
				String msg = "More pilots are needed in ?go " + (String)arena + "! Current matchup: " + (String)countPlayers(1).first + " vs " + (String)countPlayers(1).second;
				std::thread([=]() { curlChatter("More Players Needed!", msg, p->ship, cacher.spamHook); }).detach();
				
				sendChannel(";" + msg);
				sendChannel(";2;" + msg);
				sendPrivate(p, "Advertised to Discord and local chat(s).");
				countdown[4] = 300;
			}
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
			else if (c->check("unlink"))
			{
			if (!*c->final)
			{
				sendPrivate(p, "WARNING: You are about to unlink your Discord/Continuum handles! Doing so will forfeit all Elite tier privileges. To confirm, type !unlink <DiscordID>.");
				sendPrivate(p, "TIP: You can find your DiscordID by right clicking your name in Discord and selecting Copy ID with developer mode enabled.");
			}
			else
			{
				if (strcmp(getKnownAccount(p->name).c_str(), c->final) == 0) // valid ID given
					std::thread([=]() { unlinkAccount(getKnownAccount(p->name)); }).detach();
				sendPrivate(p, "Your Discord/Continuum accounts have been unlinked, and your Elite tier status has been revoked.");
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
