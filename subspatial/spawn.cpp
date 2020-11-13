#include "spawn.h"

#include "..\algorithms.h"

#include "..\dllcore.cpp"
#include "..\datatypes.cpp"
#include "..\algorithms.cpp"
#include "..\player.cpp"
#include "..\commtypes.cpp"

#define UNASSIGNED 0xffff
Cacher cacher{};
CacheLists list{};

//////// Bot list ////////
botInfo *merv;
_linkedlist <botInfo> botlist;

botInfo *findBot(CALL_HANDLE handle)
{
	_listnode <botInfo> *parse = botlist.head;

	while (parse)
	{
		botInfo *item = parse->item;

		if (item->validate(handle))
		{
			merv = item; // now we can use botInfo globally
			return item;
		}

		parse = parse->next;
	}

	return 0;
}

//////// DISCORD ////////
std::string botInfo::getINIString(int type)
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
	else if (type == 2) // relay channelID
	{
		GetPrivateProfileString("Channels", "Relay", "", key, 128, path);
		val = key;
	}
	else if (type == 3) // relay webhook 
	{
		GetPrivateProfileString("Webhooks", "Relay", "", key, 128, path);
		val = key;
	}
	else if (type == 4) // serverID
	{
		GetPrivateProfileString("Client", "ServerID", "", key, 128, path);
		val = key;
	}
	else if (type == 5) // TWSpec webhook
	{
		GetPrivateProfileString("Webhooks", "TWSpec", "", key, 128, path);
		val = key;
	}
	else if (type == 6) // TWSpec relay channel 
	{
		GetPrivateProfileString("Channels", "TWSpec", "", key, 128, path);
		val = key;
	}
	else if (type == 7)
	{
		GetPrivateProfileString("Client", "EliteRoleID", "", key, 128, path);
		val = key;
	}
	else if (type == 8)
	{
		GetPrivateProfileString("Client", "MainChannelID", "", key, 128, path);
		val = key;
	}
	else if (type == 9)
	{
		GetPrivateProfileString("Client", "StaffRoleID", "", key, 128, path);
		val = key;
	}
	else if (type == 10)
	{
		GetPrivateProfileString("Webhooks", "SpamAd", "", key, 128, path);
		val = key;
	}
	return val;
}

bool botInfo::onCooldown(std::string cmd, std::string usrID)
{
	if (cooldown.size() == 0) return false;
	bool exists = false;

	for (int i = 0; i < cooldown.size(); i++)
		if (std::get<0>(cooldown[i]) == usrID && std::get<1>(cooldown[i]) == cmd)
			if (std::get<2>(cooldown[i]))
				return true;

	return exists;
}

void botInfo::startCooldown(std::string cmd, std::string usrID, int timer)
{
	std::thread([=]()
		{
			bool exists = false;

			for (int i = 0; i < cooldown.size(); i++)
				if (std::get<0>(cooldown[i]) == usrID && std::get<1>(cooldown[i]) == cmd) // local entry already exists
				{
					std::get<2>(cooldown[i]) = true;
					exists = true;
				}

			if (!exists)
				cooldown.push_back(std::make_tuple(usrID, cmd, true));

			std::this_thread::sleep_for(std::chrono::seconds(timer));

			for (int j = 0; j < cooldown.size(); j++)
				if (std::get<0>(cooldown[j]) == usrID && std::get<1>(cooldown[j]) == cmd)
				{
					cooldown.pop_back(); // remove entry from memory
				//	std::get<2>(cooldown[j]) = false;
					return;
				}
		}).detach();
}

bool botInfo::CMPnSTART(const char* constant, const char* control) // ripped from Catid =)
{
	char c;

	while (c = *control)
	{
		if (tolower(*constant) != tolower(c)) // but is now case-insensitive
			return false;

		++control;
		++constant;
	}

	return true;
}

std::pair<int, int> botInfo::countPlayers(int teams)
{
	std::pair<int, int> zone0_spec1(0, 0);
	_listnode <Player>* parse = playerlist->head;
	while (parse)
	{
		Player* jib = parse->item;

		if (jib != me)
		{
			if (teams == 0)
			{
				if (jib->ship == 8)
					zone0_spec1.second++;
				zone0_spec1.first++;
			}
			else
			{
				if (jib->team == 0)
					zone0_spec1.first++;
				else if (jib->team == 1)
					zone0_spec1.second++;
			}
		}
		parse = parse->next;
	}
	return zone0_spec1;
}

bool botInfo::cmdHasParam(std::string command)
{
	if (command.find_first_of(" ") == command.npos) // not found
		return false;
	else
		return true;
}

std::string botInfo::cmdGetParam(std::string command)
{
	size_t delim = command.find_first_of(" ");
	std::string param = command.substr(delim, command.npos);
	param.erase(0, 1); // remove the space

	return param;
}

bool botInfo::isMuted(String pname)
{
	for (int i = 0; i < list.muted.size(); i++)
		if (CMPSTR(list.muted[i].c_str(), pname))
			return true;
	return false;
}

bool botInfo::isIgnored(String pname)
{
	for (int i = 0; i < list.ignored.size(); i++)
		if (CMPSTR(list.ignored[i].c_str(), pname))
			return true;
	return false;
}

std::string botInfo::getFreqPlayerNames(int team) // TODO: create a player badge system
{ 
	std::string ship_emoji[9] =
	{ "<:wb:402887584675659777> ", "<:jav:775302178788868128> ", "<:spid:402887979363860490> ", "<:lev:402888842115547157> ", "<:terr:402887992861261825> ",
		"<:weasel:775301956326522880> ", "<:lanc:402887962909474816> ", "<:sha:775301826827780117> ", "<:spec:775658736114335755> " };
	std::string res;

	_listnode <Player>* parse = playerlist->head;
	while (parse)
	{
		Player* jib = parse->item;

		if (!isIgnored((String)jib->name))
		{
			if (team == -1) // spectators
			{
				if (jib->ship == 8)
				{
					if (isLinked((String)jib->name))
					{
						if (isStaff((aegis::snowflake)getKnownAccount(jib->name))) // Linked discord staff
						{
							if (CMPSTR((String)jib->name, "tm_master"))
								res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\221\221 \360\237\217\206 \n");
							else if (CMPSTR((String)jib->name, "Purge"))
								res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\233\240 \360\237\221\224 \360\237\217\206 \n");
							else
								res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\221\224 \360\237\217\206 \n");
						}
						else
							res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\217\206 \n");
					}
					else
						if (CMPSTR((String)jib->name, (String)me->name))
							res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\244\226 \n");
						else if (CMPSTR((String)jib->name, "tm_master"))
							res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\221\221 \n");
						else
							res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + "\n");
				}
			}
			else if (team == jib->team)
			{
				if (isLinked((String)jib->name))
				{
					if (isStaff((aegis::snowflake)getKnownAccount(jib->name))) // Linked discord staff
					{
						if (CMPSTR((String)jib->name, "tm_master"))
							res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\221\221 \360\237\217\206 \n");
						else if (CMPSTR((String)jib->name, "Purge"))
							res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\233\240 \360\237\221\224 \360\237\217\206 \n");
						else
							if (CMPSTR((String)jib->name, "Purge"))
								res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\233\240 \360\237\217\206 \n");
							else
								res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\221\224 \360\237\217\206 \n");
					}
					else
						res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\217\206 \n");
				}
				else
					if (CMPSTR((String)jib->name, (String)me->name))
						res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\244\226 \n");
					else if (CMPSTR((String)jib->name, "tm_master"))
						res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + " \360\237\221\221 \n");
					else
						res.append(ship_emoji[jib->ship].c_str() + (String)jib->name + "\n");
			}	
		}
		parse = parse->next;
	}
	return res;
}

Player* botInfo::findPlayerByName(char* name) // case insensitive + partial names accepted
{
	Player* found = NULL;
	_listnode <Player>* parse = playerlist->head;
	while (parse)
	{
		Player* p = parse->item;

		if (CMPnSTART((String)p->name, (String)name)) // found
			found = p;

		parse = parse->next;
	}
	return found;
}

std::string botInfo::getMutedPlayers()
{
	std::string res;
	for (int i = 0; i < list.muted.size(); i++)
		res.append(list.muted[i] + ", ");
	return res;
}

//////// DLL "import" ////////

void botInfo::tell(BotEvent event)
{
	if (callback && handle)
	{
		event.handle = handle;
		callback(event);
	}
}

void botInfo::gotEvent(BotEvent &event)
{
	if (CONNECTION_DENIED) return;

	switch (event.code)
	{
//////// Periodic ////////
	case EVENT_Tick:
		{
			for (int i = 0; i < 6; ++i)
				--countdown[i];

			// Connect to Discord
			if (countdown[0] == 0)
				std::thread([=]()
					{startBotProcess(); }).detach();
			// Update online list
			if (countdown[1] == 0)
			{
				std::thread([=]()
					{
						updateOnlineList();

					}).detach();
					countdown[1] = 301; // channel updates limited to 2/10 minutes
			}
			// Clear PM handles
			if (countdown[2] == 0)
			{
				std::thread([=]()
					{
						clearDMhandles();
					}).detach();
					countdown[2] = 300;
			}
			// Message clean-up
			/*
			if (countdown[1] == 0)
			{
				std::thread([=]() 
					{
						if (cacher.msg_trash_list.size() > 0)
						{
							aegis::channel* relayCh = cacher.bot->find_channel(aegis::snowflake::snowflake(cacher.relayChannel));
							for (int i = 0; i < cacher.msg_trash_list.size(); i++)
								relayCh->delete_message(cacher.msg_trash_list[i].get_id());
							cacher.msg_trash_list.clear();
						}
						else
							cacher.bot->create_message((aegis::snowflake)cacher.relayChannel, "zero");

					}).detach();
					countdown[1] = 5;
			}
			*/
		}
		break;
//////// Arena ////////
	case EVENT_ArenaEnter:
		{
			arena = (char*)event.p[0];
			me = (Player*)event.p[1];	// if(me) {/*we are in the arena*/}
			bool biller_online = *(bool*)&event.p[2];
			cacher.arena = arena;
		}
		break;
	case EVENT_ArenaSettings:
		{
			settings = (arenaSettings*)event.p[0];
		}
		break;
	case EVENT_ArenaLeave:
		{
			me = 0;
			killTags();
		}
		break;
	case EVENT_ArenaListEntry:
		{
			char *name = (char*)event.p[0];
			bool current = *(bool*)&event.p[1];
			int population = *(int*)&event.p[2];
		}
		break;
	case EVENT_ArenaListEnd:
		{
			char *name = (char*)event.p[0];
			bool current = *(bool*)&event.p[1];
			int population = *(int*)&event.p[2];
		}
		break;
//////// Flag ////////
	case EVENT_FlagGrab:
		{
			Player *p = (Player*)event.p[0];
			Flag *f = (Flag*)event.p[1];
		}
		break;
	case EVENT_FlagDrop:
		{
			Player *p = (Player*)event.p[0];
		}
		break;
	case EVENT_FlagMove:
		{
			Flag *f = (Flag*)event.p[0];
		}
		break;
	case EVENT_FlagVictory:
		{
			int team = *(int*)&event.p[0];
			int reward = *(int*)&event.p[1];
		}
		break;
	case EVENT_FlagGameReset:
		{
		}
		break;
	case EVENT_FlagReward:
		{
			int team = *(int*)&event.p[0];
			int reward = *(int*)&event.p[1];
		}
		break;
//////// Timed ////////
	case EVENT_TimedGameOver:
		{
			Player *p[5];
			for (int i = 0; i < 5; i++)
				p[i] = (Player*)event.p[i];
		}
		break;
//////// Soccer ////////
	case EVENT_SoccerGoal:
		{
			int team = *(int*)&event.p[0];
			int reward = *(int*)&event.p[1];
		}
		break;
	case EVENT_BallMove:
		{
			PBall *ball = (PBall*)event.p[0];
		}
		break;
//////// File ////////
	case EVENT_File:
		{
			char *name = (char*)event.p[0];
		}
		break;
//////// Player ////////
	case EVENT_PlayerEntering:
		{
			Player *p = (Player*)event.p[0];

			String message = ":inbox_tray: **" + (String)p->name + "** has entered the arena **[" + (String)arena + "]**";

			std::thread([=]()
				{
				//	cacher.bot->create_message((aegis::snowflake)cacher.relayChannel, message.msg);
					curlChatter("Player Connected", message, 9, cacher.relayWebhook);
				}).detach();
		}
		break;
	case EVENT_PlayerMove:
		{
			Player *p = (Player*)event.p[0];
		}
		break;
	case EVENT_PlayerWeapon:
		{
			Player *p = (Player*)event.p[0];
			weaponInfo wi;
			wi.n = *(Uint16*)&event.p[1];
		}
		break;
	case EVENT_WatchDamage:
		{
			Player *p = (Player*)event.p[0];	// player
			Player *k = (Player*)event.p[1];	// attacker
			weaponInfo wi;
			wi.n = *(Uint16*)&event.p[2];
			Uint16 energy = *(Uint16*)&event.p[3];
			Uint16 damage = *(Uint16*)&event.p[4];
		}
		break;
	case EVENT_PlayerDeath:
		{
			Player *p = (Player*)event.p[0],
				   *k = (Player*)event.p[1];
			Uint16 bounty = (Uint16)(Uint32)event.p[2];
			Uint16 flags = (Uint16)event.p[3];
		}
		break;
	case EVENT_BannerChanged:
		{
			Player *p = (Player*)event.p[0];
		}
		break;
	case EVENT_PlayerScore:
		{
			Player *p = (Player*)event.p[0];
		}
		break;
	case EVENT_PlayerPrize:
		{
			Player *p = (Player*)event.p[0];
			int prize = *(int *)&event.p[1];
		}
		break;
	case EVENT_PlayerShip:
		{
			Player *p = (Player*)event.p[0];
			Uint16 oldship = (Uint16)(Uint32)event.p[1];
			Uint16 oldteam = (Uint16)(Uint32)event.p[2];
		}
		break;
	case EVENT_PlayerSpec:
		{
			Player *p = (Player*)event.p[0];
			Uint16 oldteam = (Uint16)(Uint32)event.p[1];
			Uint16 oldship = (Uint16)(Uint32)event.p[2];
		}
		break;
	case EVENT_PlayerTeam:
		{
			Player *p = (Player*)event.p[0];
			Uint16 oldteam = (Uint16)(Uint32)event.p[1];
			Uint16 oldship = (Uint16)(Uint32)event.p[2];
		}
		break;
	case EVENT_PlayerLeaving:
		{
			Player *p = (Player*)event.p[0];
			killTags(p);

			String message = ":outbox_tray: **" + (String)p->name + "** has left the arena **[" + (String)arena + "]**";

			std::thread([=]()
				{
				//	cacher.bot->create_message((aegis::snowflake)cacher.relayChannel, message.msg);
					curlChatter("Player Disconnected", message, 10, cacher.relayWebhook);
				}).detach();
		}
		break;
//////// Selfish ////////
	case EVENT_SelfShipReset:
		{
		}
		break;
	case EVENT_SelfPrize:
		{
			Uint16 prize = (Uint16)(Uint32)event.p[0];
			Uint16 count = (Uint16)(Uint32)event.p[1];
		}
		break;
	case EVENT_SelfUFO:
		{
		}
		break;
	case EVENT_PositionHook:
		{
		}
		break;
//////// Bricks ////////
	case EVENT_BrickDropped:
		{
			int x1 = *(int*)&event.p[0];
			int y1 = *(int*)&event.p[1];
			int x2 = *(int*)&event.p[2];
			int y2 = *(int*)&event.p[3];
			int team = *(int*)&event.p[4];
		}
		break;
//////// LVZ ////////
	case EVENT_ObjectToggled:
		{
			objectInfo obj;
			obj.n = (Uint16)(Uint32)event.p[0];
		}
		break;
//////// Turret ////////
	case EVENT_CreateTurret:
		{
			Player *turreter = (Player*)event.p[0];
			Player *turretee = (Player*)event.p[1];
		}
		break;
	case EVENT_DeleteTurret:
		{
			Player *turreter = (Player*)event.p[0];
			Player *turretee = (Player*)event.p[1];
		}
		break;
//////// Chat ////////
	case EVENT_Chat:
		{
			int type  = *(int*)&event.p[0];
			int sound = *(int*)&event.p[1];
			Player *p = (Player*)event.p[2];
			char *msg = (char*)event.p[3];

			switch (type)
			{
			case MSG_Arena:
				{
				std::string response = msg;
				if (response.find("Not online,") != response.npos)
					cacher.find = msg;
				else if (response.find(" is in SSC") != response.npos || (response.find(" is in arena") != response.npos))
					cacher.find = msg;
				else if (response.find("Unknown user") != response.npos)
					cacher.find = msg;
				}
				break;
			case MSG_PublicMacro:		if (!p) break;
				{
				}
				break;
			case MSG_Public:			if (!p) break;
			{
				if ((msg[0] == '.') || (msg[0] == '!'))
					break;

				if (p != me)
				{
					if (!isMuted((String)p->name)) // do not relay muted players
					{
						std::string name = p->name;
						if (p->team > 99) // priv freqs
							if (p->team == me->team)
								name = name + " [Spec]";
							else
								name = name + " [Private Freq]";
						else
							name = name + " [Freq " + std::to_string(p->team) + "]";
						String message = msg;

						if (message.firstInstanceOf('"') > -1) // quotes mess up curl, we should parse them out
							for (int i = 0; i < message.len; i++)
								if (message.firstInstanceOf('"') > -1) // replace all quotes
									message.replace('"', '*');

						std::thread([=]()
							{
								std::string final_name = name; // needs to be copied to new string since new thread
								if (isLinked((String)p->name))
								{
									final_name.append(" \360\237\217\206");
									curlChatter(final_name.c_str(), message, p->ship, cacher.relayWebhook);
								}
								else
									curlChatter(final_name.c_str(), message, p->ship, cacher.relayWebhook);
							}).detach();
					}
				}
			}
				break;
			case MSG_Team:				if (!p) break;
				{
				if ((msg[0] == '.') || (msg[0] == '!'))
					break;

				if (p != me)
				{
					if (!isMuted((String)p->name)) // do not relay muted players
					{
						std::string name = p->name;
						name = name + " [Spec Chat]";
						String message = msg;

						if (message.firstInstanceOf('"') > -1) // quotes mess up curl, we should parse them out
							for (int i = 0; i < message.len; i++)
								if (message.firstInstanceOf('"') > -1) // replace all quotes
									message.replace('"', '*');

						std::thread([=]()
							{
								std::string final_name = name; // needs to be copied to new string since new thread
								if (isLinked((String)p->name))
								{
									final_name.append(" \360\237\217\206");
									curlChatter(final_name.c_str(), message, p->ship, cacher.relayWebhook);
								}
								else
									curlChatter(final_name.c_str(), message, p->ship, cacher.relayWebhook);
							}).detach();
					}
				}
				}
				break;
			case MSG_TeamPrivate:		if (!p) break;
				{
				}
				break;
			case MSG_Private:			if (!p) break;
				{
				if (msg[0] == '@') // CROSS PMs
				{
					if (!isMuted((String)p->name)) // do not relay muted players
					{
						String name = p->name;
						String message = msg;

						if (message.firstInstanceOf('"') > -1) // quotes mess up curl, we should parse them out
							for (int i = 0; i < message.len; i++)
								if (message.firstInstanceOf('"') > -1) // replace all quotes
									message.replace('"', '*');

						if (isLinked(name))
						{
							std::thread([=]()
								{
									std::string out = message.msg;
									out.append(" $~$@" + name);

									if (!DM2Discord(out))
										sendPrivate(p, "That user was not found on Discord. Please check your spelling!");
								}).detach();
						}
						else
							sendPrivate(p, "PMing users on Discord via Continuum requires an Elite account. Please use !help link for more info.");
					}
				}
				}
				break;
			case MSG_PlayerWarning:		if (!p) break;
				{
				}
				break;
			case MSG_RemotePrivate:
				{
				}
				break;
			case MSG_ServerError:
				{
				}
				break;
			case MSG_Channel:
				{
				std::string message = msg;
				std::string twbot = message.substr(2, 7);
				std::string egbot = message.substr(11, 3);
				if (strcmp(twbot.c_str(), "TW-Chat") == 0)
				{
					std::string output = message.erase(0, 10);
					std::string pname = output.substr(0, output.find_last_of('>'));
					std::string chatter = output.substr(output.find_last_of('>') + 1, output.npos);
					std::string trimmed_name = pname.substr(pname.find_last_of(' ') + 1, pname.npos - pname.find_last_of(' ') + 1);

					if (strcmp("#EG", egbot.c_str()) != 0) // ignore EG->TW relays
					{
						if ((chatter.find('"') != chatter.npos) || (chatter.find('@') != chatter.npos)) // quotes/ mess up curl, we should parse them out
						{
							for (int i = 0; i < chatter.length(); i++)
							{
								if (chatter.find('"') != chatter.npos) // replace all quotes
									chatter.replace(chatter.find('"'), 1, "*");
								if (chatter.find('@') != chatter.npos) // prevent tags
									chatter.replace(chatter.find('@'), 1, "!");
							}
						}

						std::thread([=]()
							{
								if (isLinked((String)trimmed_name.c_str()))
									curlChatter((String)pname.c_str() + " \360\237\217\206", (String)chatter.c_str(), 13, cacher.TWSpecWebhook);
								else
									curlChatter((String)pname.c_str(), (String)chatter.c_str(), 13, cacher.TWSpecWebhook);
							}).detach();
					}
				}
				}
				break;
			};
		}
		break;
	case EVENT_LocalCommand:
		{
			Player *p = (Player*)event.p[0];
			Command *c = (Command*)event.p[1];

			gotCommand(p, c);
		}
		break;
	case EVENT_LocalHelp:
		{
			Player *p = (Player*)event.p[0];
			Command *c = (Command*)event.p[1];

			gotHelp(p, c);
		}
		break;
	case EVENT_RemoteCommand:
		{
			char *p = (char*)event.p[0];
			Command *c = (Command*)event.p[1];
			Operator_Level o = *(Operator_Level*)&event.p[2];

			gotRemote(p, c, o);
		}
		break;
	case EVENT_RemoteHelp:
		{
			char *p = (char*)event.p[0];
			Command *c = (Command*)event.p[1];
			Operator_Level o = *(Operator_Level*)&event.p[2];

			gotRemoteHelp(p, c, o);
		}
		break;
//////// Containment ////////
	case EVENT_Init:
		{
			int major	= HIWORD(*(int*)&event.p[0]);
			int minor	= LOWORD(*(int*)&event.p[0]);
			callback	= (CALL_COMMAND)event.p[1];
			playerlist	= (CALL_PLIST)event.p[2];
			flaglist	= (CALL_FLIST)event.p[3];
			map			= (CALL_MAP)event.p[4];
			bricklist	= (CALL_BLIST)event.p[5];

			if (major > CORE_MAJOR_VERSION)
			{
				tell(makeEcho("DLL plugin cannot connect.  This plugin is out of date."));

				CONNECTION_DENIED = true;

				return;
			}
			else if ((major < CORE_MAJOR_VERSION) || (minor < CORE_MINOR_VERSION))
			{
				tell(makeEcho("DLL plugin cannot connect.  This plugin requires the latest version of MERVBot."));

				CONNECTION_DENIED = true;

				return;
			}
			else
				tell(makeEcho("DLL plugin connected."));

		}
		break;
	case EVENT_Term:
		{
			tell(makeEcho("DLL plugin disconnected."));
			cacher.bot->shutdown();
		}
		break;
	};
}


//////// DLL export ////////

_declspec(dllexport) void __stdcall talk(BotEvent &event)
{
	botInfo *bot;

	bot = findBot(event.handle);

	if (!bot)
	{
		bot = new botInfo(event.handle);
		botlist.append(bot);
	}

	if (bot) bot->gotEvent(event);
}


//////// Tag emulation ////////

int botInfo::get_tag(Player *p, int index)
{
	_listnode <PlayerTag> *parse = taglist.head;
	PlayerTag *tag;

	while (parse)
	{
		tag = parse->item;

		if (tag->p == p)
		if (tag->index == index)
			return tag->data;

		parse = parse->next;
	}

	return 0;
}

void botInfo::set_tag(Player *p, int index, int data)
{
	_listnode <PlayerTag> *parse = taglist.head;
	PlayerTag *tag;

	while (parse)
	{
		tag = parse->item;

		if (tag->p == p)
		if (tag->index == index)
		{
			tag->data = data;
			return;
		}

		parse = parse->next;
	}

	tag = new PlayerTag;
	tag->p = p;
	tag->index = index;
	tag->data = data;
	taglist.append(tag);
}

void botInfo::killTags(Player *p)
{
	_listnode <PlayerTag> *parse = taglist.head;
	PlayerTag *tag;

	while (parse)
	{
		tag = parse->item;
		parse = parse->next;

		if (tag->p != p) continue;

		taglist.kill(tag);
	}
}

void botInfo::killTags()
{
	taglist.clear();
}


//////// LVZ Object Toggling ////////

void botInfo::clear_objects()
{
	num_objects = 0;
}

void botInfo::object_target(Player *p)
{
	object_dest = p;
}

void botInfo::toggle_objects()
{
	Player *p = object_dest;

	if (!p)	tell(makeToggleObjects(UNASSIGNED, (Uint16 *)object_array, num_objects));
	else	tell(makeToggleObjects(p->ident, (Uint16 *)object_array, num_objects));

	num_objects = 0;
}

void botInfo::queue_enable(int id)
{
	if (num_objects == MAX_OBJECTS)
		toggle_objects();

	object_array[num_objects].id = id;
	object_array[num_objects].disabled = false;
	++num_objects;
}

void botInfo::queue_disable(int id)
{
	if (num_objects == MAX_OBJECTS)
		toggle_objects();

	object_array[num_objects].id = id;
	object_array[num_objects].disabled = true;
	++num_objects;
}


//////// Chatter ////////

void botInfo::sendPrivate(Player *player, BYTE snd, char *msg)
{
	tell(makeSay(MSG_Private, snd, player->ident, msg));
}

void botInfo::sendPrivate(Player *player, char *msg)
{
	tell(makeSay(MSG_Private, 0, player->ident, msg));
}

void botInfo::sendTeam(char *msg)
{
	tell(makeSay(MSG_Team, 0, 0, msg));
}

void botInfo::sendTeam(BYTE snd, char *msg)
{
	tell(makeSay(MSG_Team, snd, 0, msg));
}

void botInfo::sendTeamPrivate(Uint16 team, char *msg)
{
	tell(makeSay(MSG_TeamPrivate, 0, team, msg));
}

void botInfo::sendTeamPrivate(Uint16 team, BYTE snd, char *msg)
{
	tell(makeSay(MSG_TeamPrivate, snd, team, msg));
}

void botInfo::sendPublic(char *msg)
{
	tell(makeSay(MSG_Public, 0, 0, msg));
}

void botInfo::sendPublic(BYTE snd, char *msg)
{
	tell(makeSay(MSG_Public, snd, 0, msg));
}

void botInfo::sendPublicMacro(char *msg)
{
	tell(makeSay(MSG_PublicMacro, 0, 0, msg));
}

void botInfo::sendPublicMacro(BYTE snd, char *msg)
{
	tell(makeSay(MSG_PublicMacro, snd, 0, msg));
}

void botInfo::sendChannel(char *msg)
{
	tell(makeSay(MSG_Channel, 0, 0, msg));
}

void botInfo::sendRemotePrivate(char *msg)
{
	tell(makeSay(MSG_RemotePrivate, 0, 0, msg));
}

void botInfo::sendRemotePrivate(char *name, char *msg)
{
	String s;
	s += ":";
	s += name;
	s += ":";
	s += msg;

	sendRemotePrivate(s);
}
