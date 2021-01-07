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
//			break;
		case OP_SysOp:
			{	// SysOp-level commands
//				sendPrivate(p, "Ext: ");
			}
//			break;
		case OP_SuperModerator:
			{	// SuperModerator-level commands
//				sendPrivate(p, "Ext: ");
			}
//			break;
		case OP_Moderator:
			{	// Moderator-level commands
			sendPrivate(p, "----------------------------------------------------");
			sendPrivate(p, "|           [Subspatial | Staff Commands]          |");
			sendPrivate(p, "----------------------------------------------------");
			sendPrivate(p, "| Config:     !read          (refreshes settings)  |");
			sendPrivate(p, "| Moderation: !mute/unmute   (Continuum players)   |");
			sendPrivate(p, "|             !dmute/dunmute (Discord users)       |");
			sendPrivate(p, "|             !listelite/listmute                  |");
			}
//			break;
		case OP_Limited:
			{	// Limited-level commands
			}
		case OP_Player:
			{	// Player-level commands
			sendPrivate(p, "------ [Subspatial v" + (String)BOT_VER + "] -------------------------------------------");
			sendPrivate(p, "| Account:   !link/unlink    (binds your Discord/Continuum handles)  |");
			sendPrivate(p, "|            !elite     [*]  (shows available Elite tier perks)      |");
			sendPrivate(p, "| Game:      !spam      [*]  (pings Discord to search for players)   |");
			sendPrivate(p, "|            !extra          (calls for more players)                |");
			sendPrivate(p, "| Music:     !np             (check what's playing in Discord)       |");
			sendPrivate(p, "| [*] Requires an Elite account. Type !link for more information.    |");
			sendPrivate(p, "----------------------------------------------------- [by Purge] ----");
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
			if (c->checkParam("read"))
			{
				sendPrivate(p, "[SYNTAX: !read] Refreshes all bot config and data files relevant to Subspatial. Only useful in troubleshooting.");
			}
			else if (c->checkParam("dmute") || c->checkParam("dunmute"))
			{
				sendPrivate(p, "[SYNTAX: !dmute/dunmute <DiscordName>] Mutes a Discord user from chatting in Continuum by USERNAME (partial names allowed).");
			}
			else if (c->checkParam("mute") || c->checkParam("unmute") || c->checkParam("listmute"))
			{
				sendPrivate(p, "[SYNTAX: !mute/unmute <PlayerName>] Mutes a Continuum player's chat relays to Discord (partial names allowed).");
				sendPrivate(p, "[SYNTAX: !listmute]                 Lists all muted players in Continuum.");
			}
			else if (c->checkParam("listelite"))
			{
				sendPrivate(p, "[SYNTAX: !listelite] Lists all registered Elite users according to (DiscordID:ContinuumName).");
			}
		}
	case OP_Limited:
		{	// Limited-level commands
		}
	case OP_Player:
		{	// Player-level commands
		if (c->checkParam("link") || c->checkParam("elite"))
		{
			sendPrivate(p, "----------------------------------------------------------");
			sendPrivate(p, "|                      Elite Tier                        |");
			sendPrivate(p, "----------------------------------------------------------");
			sendPrivate(p, "| Linking your accounts will unlock Elite Tier, a free   |");
			sendPrivate(p, "| premium pass for additional game modes & functionality!|");
			sendPrivate(p, "----------------------------------------------------------");
			sendPrivate(p, "| Step 1: Login to Discord and type --link in main chat  |");
			sendPrivate(p, "| Step 2: Open your DMs and copy the code from Subspatial|");
			sendPrivate(p, "| Step 3: Login to Continuum and PM me with !link <code> |");
			sendPrivate(p, "----------------------------------------------------------");
			sendPrivate(p, "| Type !elite to see all available perks. Have fun!      |");
			sendPrivate(p, "----------------------------------------------------------");
		}
		else if (c->checkParam("unlink"))
		{
			sendPrivate(p, "[SYNTAX: !unlink <DiscordID>] Unlinks your Discord/Continuum handles. This action is irreversible, and all Elite tier perks will be lost. To unlock Elite tier again, you will have to !link again.");
		}
		else if (c->checkParam("np"))
		{
			sendPrivate(p, "[SYNTAX: !np] Displays currently playing songs, as well as the next song, in the Music channels on Discord.");
		}
		else if (c->checkParam("spam") || c->checkParam("extra"))
		{
			sendPrivate(p, "[SYNTAX: !spam] Pings a notifiable role on Discord calling for players to join your game. Requires Elite tier.");
			sendPrivate(p, "[SYNTAX: !extra] Calls for additional players to enter the game on both Discord and chat channels.");
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
					_cache.loadCache();
					_cache.loadLists(0);
					_cache.loadLists(1);
					_cache.loadLists(2);
				}).detach();

				sendPrivate(p, "Reloaded new settings from " + (String)MAININI + " and data files.");
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

					auto reply = _cache.discord.bot->find_channel(_cache.discord.flakes.relayChannel)->edit_channel_permissions(edit_channel_permissions_t()
						.overwrite_id(usr->get_id()).allow(aegis::permissions::VIEW_CHANNEL).deny(aegis::permissions::SEND_MESSAGES).type("1"));

					if (reply.get().reply_code == 400)
						sendPrivate(p, "Unknown error when attempting to mute: " + (String)usr->get_username().c_str() + ". Please check bot permissions!");
					else
						sendPrivate(p, (String)usr->get_username().c_str() + " has been muted from the Discord relay channel. Use !dunmute to unmute him/her.");
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
					auto reply = _cache.discord.bot->find_channel(_cache.discord.flakes.relayChannel)->delete_channel_permission(usr->get_id());

					if (reply.get().reply_code == 400)
						sendPrivate(p, (String)usr->get_username().c_str() + " is not currently muted. Please check your spelling!");
					else
						sendPrivate(p, (String)usr->get_username().c_str() + " has been unmuted from the Discord relay channel.");
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
						_cache.loadLists(0);
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
					_cache.loadLists(0);
				}
			}
		}
		else if (c->check("listmute"))
		{
			sendPrivate(p, "MUTED Players from Continuum:");
			sendPrivate(p, "----------------------------------------------");
			for (int i = 0; i < _cache.discord.elite.muted.size(); i++)
				sendPrivate(p, (String)_cache.discord.elite.muted[i].c_str());
			sendPrivate(p, "----------------------------------------------");
			sendPrivate(p, "Use !unmute to unmute a player (full name required). Check Discord's channel permissions to view those muted on Discord.");
		}
		else if (c->check("listelite"))
		{

			sendPrivate(p, "Registered Elite accounts (DiscordID:ContinuumName):");
			sendPrivate(p, "---------------------------------------------------");
			for (int i = 0; i < _cache.discord.elite.accounts.size(); i++)
				sendPrivate(p, "(" + (String)_cache.discord.elite.accounts[i].first.gets().c_str() + ":" + (String)_cache.discord.elite.accounts[i].second.c_str() + ")");
			sendPrivate(p, "---------------------------------------------------");
		}
		if (c->check("dinvite"))
		{
			if (!*c->final)
				sendPrivate(p, "You must specify the player to invite to Discord.");
			else
			{
				Player* invitee = findPlayerByName(c->final);
				if (invitee == NULL)
					sendPrivate(p, "That player was not found in the arena! Please check your spelling.");
				else
				{
					sendPrivate(p, "Generating Discord invite link for " + (String)invitee->name + "...");
					std::thread([=]() 
						{ 
							auto invite = _cache.discord.bot->find_channel(_cache.discord.flakes.mainChannelID)->create_channel_invite(86400, 1, false, false);
							auto raw_code = invite.get().content;
							std::string inv_code = raw_code.substr(raw_code.find("code") + 8, (raw_code.find("guild") - 4) - (raw_code.find("code") + 8));
							sendPrivate(invitee, "You have been invited to the official Discord server of " + (String)_cache.game.zone.c_str() + 
								" Click here to join: https://discord.gg/" + (String)inv_code.c_str());		
						}).detach();
				}
			}
		}
		else if (c->check("troll"))
		{

		if (!*c->final)
			sendPrivate(p, "Player name not given.");
		else
		{
			if (findPlayerByName(c->final) == NULL)
				sendPrivate(p, "That player is not in this arena.");
			else
			{
				Player* troll = findPlayerByName(c->final);
				sendPrivate(troll, "*spec");
				
			}
		}
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
			sendPrivate(p, "|           Automated Player Performance Profiles                    |");
			sendPrivate(p, "|           Dynamic & Evolving Economy                               |");
			sendPrivate(p, "|           Premium User perks via Elite Tier account binding        |");
			sendPrivate(p, "| For more information: https://github.com/purge-dev                 |");
			sendPrivate(p, "----------------------------------------------------- [by Purge] ----");
		}
		else if (c->check("np"))
		{
			if (isLinked((String)p->name))
			{
				if (songname[0].empty() && songname[1].empty())
					sendPrivate(p, "There is nothing playing right now, or I was unable to catch the current song!");
				if (!songname[0].empty())
				{
					sendPrivate(p, "[Discord Music 1] Now Playing: " + (String)songname[0].c_str() + " (requested by" + (String)requester[0].c_str() + ")");
					sendPrivate(p, "Up Next: " + (String)next[0].c_str());
				}
				if (!songname[1].empty())
				{
					sendPrivate(p, "[Discord Music 2] Now Playing: " + (String)songname[1].c_str() + " (requested by" + (String)requester[1].c_str() + ")");
					sendPrivate(p, "Up Next: " + (String)next[1].c_str());
				}
			}
			else
				sendPrivate(p, "This command is only available to Elite accounts. Type !link for more info.");
		}
		else if (c->check("spam"))
		{
			if (isLinked((String)p->name))
			{
				if (countdown[3] > 0)
					sendPrivate(p, "Someone recently used !spam. You must wait " + (String)((countdown[3] / 60) % 60) + " minutes.");
				else
				{
					String msg = "<@&509789987060056074> Calling all pilots to **?go " + (String)arena + "** for a game! Enter now to play.";
					std::thread([=]() { curlChatter((String)p->name, msg, p->ship, _cache.discord.spamHook); }).detach();
					
					sendChannel(";" + (String)p->name + " is calling all available pilots to ?go " + (String)arena + " for a game! Enter now to play.");
					sendChannel(";2;" + (String)p->name + " is calling all available pilots to ?go " + (String)arena + " for a game! Enter now to play.");
					sendPrivate(p, "Advertised to Discord and local chat(s).");
					countdown[3] = 900;
				}
			}
			else
			{
				sendPrivate(p, "This command is only available to Elite accounts. Type !link for more info.");
				sendPrivate(p, "[NOTE] You may use !extra if you do not wish to link your Discord/Continuum accounts.");
			}
		}
		else if (c->check("extra"))
		{
			if (countdown[4] > 0)
				sendPrivate(p, "Someone recently used !extra. You must wait " + (String)((countdown[4] / 60) % 60) + " minutes.");
			else
			{
				String msg = "More pilots are needed in **?go " + (String)arena + "**! Current matchup: " + (String)countPlayers(1).first + " vs " + (String)countPlayers(1).second;
				std::thread([=]() { curlChatter("More Players Needed!", msg, p->ship, _cache.discord.spamHook); }).detach();
				
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
								std::thread([=]() { grantDiscordElite(dName); }).detach();
								hash.erase(hash.begin() + i); // remove this code from global vector 
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
				{
					std::thread([=]() { unlinkAccount(getKnownAccount(p->name)); }).detach();
					sendPrivate(p, "Your Discord/Continuum accounts have been unlinked, and your Elite tier status has been revoked.");
				}
				else
					sendPrivate(p, "That DiscordID is not linked to this account. Please check your ID again!");
			}
		}
		else if (c->check("elite"))
		{
		sendPrivate(p, "-----------------------------------------------------");
		sendPrivate(p, "| Elite Tier Perks:                                 |");
		sendPrivate(p, "-----------------------------------------------------");
		sendPrivate(p, "| Features:                                         |");
		sendPrivate(p, "|         Mentions (include @Username in any chat)  |");
		sendPrivate(p, "|         Cross platform PM (PM me @DiscordName msg)|");
		sendPrivate(p, "|         Subspace-wide chat via Discord            |");
		sendPrivate(p, "|         Custom Role/Color (use --role in Discord) |");
		sendPrivate(p, "| Commands:                                         |");
		sendPrivate(p, "|         !np   (check what's playing in #music)    |");
		sendPrivate(p, "|         !skip (requests DJ to skip current song)  |");
		sendPrivate(p, "|         !spam (role pings Discord users to play)  |");
		sendPrivate(p, "-----------------------------------------------------");
		}

		//////// ECONOMY ////////

		else if (c->check("openaccount"))
		{
			if (isLinked((String)p->name))
			{
				Economy eco;
				if (eco.getPlayerScrap(p) == -1) // new users
				{
					sendPrivate(p, "[Bank of Subspatial] Thank you for your interest in opening a scrap (SCR) account! We are currently offering the following rates:");
					
				}
				else;
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
