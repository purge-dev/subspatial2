#include "spawn.h"
/* discord.cpp ~
* 
*  Functions here deal with the Discord interface and Elite tier account handling.
*  All other custom functions are in spawn.cpp.
*/

void Cachedump::loadCache()
{
	_cache.discord.bot_prefix = "--";
	_cache.discord.token = merv->getINIString(1);
	_cache.discord.flakes.relayChannel = (aegis::snowflake)merv->getINIString(2);
	_cache.discord.relayWebhook = merv->getINIString(3);
	_cache.discord.flakes.serverID = (aegis::snowflake)merv->getINIString(4);
	_cache.discord.TWSpecWebhook = merv->getINIString(5);
	_cache.discord.flakes.TWSpecChannel = (aegis::snowflake)merv->getINIString(6);
	_cache.discord.flakes.eliteRoleID = (aegis::snowflake)merv->getINIString(7);
	_cache.discord.flakes.mainChannelID = (aegis::snowflake)merv->getINIString(8);
	_cache.discord.flakes.staffRoleID = (aegis::snowflake)merv->getINIString(9);
	_cache.discord.spamHook = merv->getINIString(10);
	_cache.discord.TWSpecMainDiscordWebhook = merv->getINIString(11);
	_cache.discord.DevaMainDiscordWebhook = merv->getINIString(12);
	_cache.discord.flakes.MusicChannelID = (aegis::snowflake)merv->getINIString(13);
	_cache.discord.flakes.relayCategory = (aegis::snowflake)merv->getINIString(14);

	// Snapshot cache for postgame stats embed
	std::fstream file;

	file.open("discord\\data\\snaps.dat", std::ios::in);
	if (file.peek() == file.eof())
		return;

	if (!file.good()) 
		return;
	else
	{
		char line[1024];
		while (file.getline(line, 1024))
			_cache.game.snapshots.push_back(line); // feed every url into vector
	}
	file.close();
}

void Cachedump::loadLists(int type)
{
	if (type == 0) // muted list
	{
		_cache.discord.elite.muted.clear();

		std::string str;
		std::fstream file;
		file.open(LISTINI, std::fstream::in);
		if (file.peek() == file.eof())
			return;
		if (!file.good())
			return;
		else
		{
			char line[128];
			while (file.getline(line, 128))
			{
				if (CMPSTART("[Muted", line))
				{
					while (file.getline(line, 128))
					{
						if (line[0] == '^')
						{
							str = line;
							std::string found = str.substr(0, str.find("=^$mut"));
							found.erase(0, 1); // remove @
							_cache.discord.elite.muted.push_back(found);
						}
					}
				}
			}
		}
		file.close();
	}
	else if (type == 1) // topic ignore list
	{
		_cache.discord.elite.ignored.clear();

		std::string str;
		std::fstream file;
		file.open(LISTINI, std::fstream::in);
		if (file.peek() == file.eof())
			return;
		if (!file.good())
			return;
		else
		{
			char line[128];
			while (file.getline(line, 128))
			{
				if (CMPSTART("[TopicIgnore", line))
				{
					while (file.getline(line, 128))
					{
						if (line[0] == '@')
						{
							str = line;
							std::string found = str.substr(0, str.find("=^$ign"));
							found.erase(0, 1); // remove @
							_cache.discord.elite.ignored.push_back(found);
						}
					}
				}
			}
		}
	}
	else if (type == 2) // elite players
	{
		_cache.discord.elite.accounts.clear();

		std::string str;
		std::fstream file;
		file.open(ELITEINI, std::fstream::in);
		if (file.peek() == file.eof())
			return;
		if (!file.good())
			return;
		else
		{
			char line[128];
			while (file.getline(line, 128))
			{
				if (CMPSTART("[Accounts", line))
				{
					while (file.getline(line, 128))
					{
						if (line[0] == '@')
						{
							str = line;
							std::string usrID = str.substr(0, str.find("=^"));
							std::string pname = str.substr(str.find("=^"), str.npos - (str.find("=^") + 2)).c_str();
							usrID.erase(0, 1); // remove @
							pname.erase(0, 2); // remove =^
							_cache.discord.elite.accounts.push_back(std::make_pair((aegis::snowflake)usrID, pname));
						}
					}
				}
			}
		}
		file.close();
	}
	else if (type == 3) // economy scrap
	{
		_cache.economy.accounts.clear();

		std::string str;
		std::fstream file;
		file.open(ECONINI, std::fstream::in);
		if (file.peek() == file.eof())
			return;
		if (!file.good())
			return;
		else
		{
			char line[128];
			while (file.getline(line, 128))
			{
				if (CMPSTART("[Accounts", line))
				{
					while (file.getline(line, 128))
					{
						if (line[0] == '@')
						{
							str = line;
							std::string usrID = str.substr(0, str.find("=^"));
							std::string scrap = str.substr(str.find("=^"), str.npos - (str.find("=^") + 2)).c_str();
							usrID.erase(0, 1); // remove @
							scrap.erase(0, 2); // remove =^
							_cache.economy.accounts.push_back(std::make_pair(std::stoi(scrap), usrID));
						}
					}
				}
			}
		}
		file.close();
	}
}

void botInfo::parseMusic(aegis::gateway::events::message_create obj)
{
	if (obj.msg.author.username == "Rythm 2")
	{
		auto em = obj.msg.embeds;
		for (int i = 0; i < em.size(); i++)
		{
			if (em[i].title().front() == 'N')
			{
				size_t delim = em[i].title().find_last_of(" ");
				title[0] = em[i].title().erase(delim, em[i].title().length() - delim);

				if (strcmp(title[0].c_str(), "Now Playing "))
				{
					requester[0] = em[i].description().substr(em[i].description().find("Requested by:") + 14, (em[i].description().find("Up Next:") - 5) - (em[i].description().find("Requested by:") + 14));
					next[0] = em[i].description().substr(em[i].description().find("Up Next:") + 9, em[i].description().npos - em[i].description().find("Up Next:"));
					em[i].description().erase(0, 1);

					if (em[i].description().find("[") != em[i].description().npos)
						em[i].description().erase(em[i].description().find("["), 1);

					size_t parse = (em[i].description().find_first_of("]"));
					songname[0] = em[i].description().erase(parse, em[i].description().length() - delim);

					merv->sendTeam("[Discord Music 1] Now Playing: " + (String)songname[0].c_str() + " (requested by" + (String)requester[0].c_str() + ")");
					merv->sendTeam("Up Next: " + (String)next[0].c_str());
				}
			}
		}
	}
	if (obj.msg.author.username == "Rythm")
	{
		auto em = obj.msg.embeds;
		for (int i = 0; i < em.size(); i++)
		{
			if (em[i].title().front() == 'N')
			{
				size_t delim = em[i].title().find_last_of(" ");
				title[1] = em[i].title().erase(delim, em[i].title().length() - delim);

				if (strcmp(title[1].c_str(), "Now Playing "))
				{
					requester[1] = em[i].description().substr(em[i].description().find("Requested by:") + 14, (em[i].description().find("Up Next:") - 5) - (em[i].description().find("Requested by:") + 14));
					next[1] = em[i].description().substr(em[i].description().find("Up Next:") + 9, em[i].description().npos - em[i].description().find("Up Next:"));
					em[i].description().erase(0, 1);

					if (em[i].description().find("[") != em[i].description().npos)
						em[i].description().erase(em[i].description().find("["), 1);

					size_t parse = (em[i].description().find_first_of("]"));
					songname[1] = em[i].description().erase(parse, em[i].description().length() - delim);

					merv->sendTeam("[Discord Music 2] Now Playing: " + (String)songname[1].c_str() + " (requested by" + (String)requester[1].c_str() + ")");
					merv->sendTeam("Up Next: " + (String)next[1].c_str());
				}
			}
		}
	}
}

bool botInfo::isStaff(aegis::snowflake usrID)
{
	if ((_cache.discord.bot->find_guild(_cache.discord.flakes.serverID)->member_has_role(usrID, _cache.discord.flakes.staffRoleID)) ||
		(_cache.discord.bot->find_guild(_cache.discord.flakes.serverID)->member_has_role(usrID, (aegis::snowflake)"774914545827053588"))) // REMOVE BEFORE RELEASE
		return true;
	else
		return false;
}

std::string botInfo::createUserMentionString(std::string msg)
{
	std::vector <std::string> usrIDs;
	std::string testmsg = msg;
	
	if (testmsg.find_first_of('@') != testmsg.npos) // parse entire msg to extract each user
	{
		for (int i = 0; i < testmsg.length(); i++) 
		{
			if (testmsg.find("@") != testmsg.npos)
			{
				size_t loc = testmsg.find("@");
				std::string raw_usrname = testmsg.substr(loc + 1, ((testmsg.find_first_of(' ', loc) != testmsg.npos) ? 
					testmsg.find_first_of(' ', loc) : testmsg.npos) - loc - 1); // parse out the username, accounting for mentions at end of string
			
				if (getUser(raw_usrname) != nullptr)
				{
					testmsg.erase(loc, testmsg.find_first_of(' ', loc) - loc); testmsg.insert(loc, "$!~"); // only modify the parts with actual users
					aegis::user* usr = getUser(raw_usrname);
					usrIDs.push_back(usr->get_id().gets()); // save the usrID for each iteration
				}
			}
		}
	}
	std::string newmsg = testmsg;
	
	for (int j = 0; j < usrIDs.size(); j++) // create the newly parsed msg with support for multiple user mentions
	{
		if (newmsg.find("$!~") != newmsg.npos)
		{
			size_t newloc = newmsg.find("$!~");
			newmsg.erase(newloc, 3);
			newmsg.insert(newloc, "<@" + usrIDs[j] + ">");
		}
	}
	
	return newmsg;
}

void botInfo::updateOnlineList()
{
	std::string zone_count = std::to_string(countPlayers().first);
	std::string spec_count = std::to_string(countPlayers().second);
	std::string freq0players = getFreqPlayerNames(0);

	if (freq0players.size() == 0)
		freq0players = "";
	else
		freq0players = "\n**__Team 0__**\n" + freq0players;

	std::string freq1players = getFreqPlayerNames(1);
	if (freq1players.size() == 0)
		freq1players = "";
	else
		freq1players = "\n**__Team 1__**\n" + freq1players;
	std::string spec_players = getFreqPlayerNames(-1);

	// topic ratelimited at 2/10 min -> adjust cooldown[1]
	// TODO: crashes if string > 1024 chars?
	std::string online_list_str = ":video_game: **__Arena__** (" + _cache.game.arena + ") | **__Total Players__** (" +
		zone_count + ") | **__Spectators__** (" + spec_count +
		") | **__CLICK HERE__** for the full online list!\n\n" +
		"**__Spectators__**\n" + spec_players + freq0players + freq1players;

	if (online_list_str.length() < 1024)
		_cache.discord.bot->find_channel(aegis::snowflake::snowflake(_cache.discord.flakes.relayChannel))->modify_channel(std::nullopt, std::nullopt,
			online_list_str, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt);

	else
	{
		online_list_str = ":video_game: **__Arena__** (" + _cache.game.arena + ") | **__Total Players__** (" +
			zone_count + ") | **__Spectators__** (" + spec_count +
			") | **__CLICK HERE__** for the full online list!\n\n" +
			freq0players + freq1players;

		_cache.discord.bot->find_channel(aegis::snowflake::snowflake(_cache.discord.flakes.relayChannel))->modify_channel(std::nullopt, std::nullopt,
			online_list_str, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
	}

	_cache.discord.bot->find_channel(_cache.discord.flakes.relayCategory)->modify_channel("\360\237\222\254 Zone Chat (" + zone_count + " Online)", std::nullopt, std::nullopt,
		std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
}

void botInfo::checkUnlinker(aegis::gateway::events::message_reaction_add obj) 
{
	for (int i = 0; i < _cache.discord.unlinker.size(); i++)
	{
		if (_cache.discord.unlinker[i]->get_id() == obj.message_id) // reacted to msg awaiting reply
		{
			DEF_FOOTER;
			if (obj.emoji.name == u8"\u2705") // yes
			{
				std::string unlinked_acc = getKnownAccount(obj.user_id.gets());
				unlinkAccount(obj.user_id.gets());
				_cache.discord.bot->find_channel(obj.channel_id)->edit_message(aegis::edit_message_t().message_id(obj.message_id)
					.embed(
						aegis::gateway::objects::embed()
						.color(0x00FF00).thumbnail(aegis::gateway::objects::thumbnail("https://cdn.discordapp.com/avatars/580330179831005205/49035f8777ff7dc50c44bf69e99b30bb.png"))
						.title("Continuum/Discord Account Unlinker").url("https://github.com/purge-dev")
							.description("\342\234\205 Your Discord/Continuum accounts have been successfully unlinked.")
						.fields
						({
							aegis::gateway::objects::field().name("Continuum Account Unlinked:").value(unlinked_acc + "\n\n All \360\237\217\206 **Elite Tier** privileges have been forfeited.")
							})
						.footer(foot)
					)
				);
			}
			else if (obj.emoji.name == u8"\u26D4") // no
				_cache.discord.bot->find_channel(obj.channel_id)->edit_message(aegis::edit_message_t().message_id(obj.message_id)
					.embed(
						aegis::gateway::objects::embed()
						.color(0xFF0000).thumbnail(aegis::gateway::objects::thumbnail("https://cdn.discordapp.com/avatars/580330179831005205/49035f8777ff7dc50c44bf69e99b30bb.png"))
						.title("Continuum/Discord Account Unlinker").url("https://github.com/purge-dev")
						.description("\342\233\224 The unlinking process has been aborted.")
						.fields
						({
							aegis::gateway::objects::field().name("Currently Linked Account:").value(getKnownAccount(obj.user_id.gets()) + "\n\n \360\237\217\206 **Elite Tier** privileges remain active.")
							})
						.footer(foot)
					)
				);

			_cache.discord.unlinker.erase(_cache.discord.unlinker.begin() + i); // erase this message handle from vector
		}
	}
}

void botInfo::checkSettingsCommand(aegis::gateway::events::message_reaction_add obj)
{
	for (int i = 0; i < _cache.discord.settings_menu.size(); i++)
	{
		if (_cache.discord.settings_menu[i]->get_id() == obj.message_id)
		{
			DEF_FOOTER;
			if (obj.emoji.name == u8"\u0031\uFE0F\u20E3" || obj.emoji.name == u8"\u0032\uFE0F\u20E3" || obj.emoji.name == u8"\u0033\uFE0F\u20E3" || 
				obj.emoji.name == u8"\u0034\uFE0F\u20E3" || obj.emoji.name == u8"\u0035\uFE0F\u20E3" || obj.emoji.name == u8"\u0036\uFE0F\u20E3" ||
				obj.emoji.name == u8"\u0037\uFE0F\u20E3" || obj.emoji.name == u8"\u0038\uFE0F\u20E3" || obj.emoji.name == u8"\u0039\uFE0F\u20E3") // 1, 2, 3, etc.

				_cache.discord.bot->find_channel(obj.channel_id)->edit_message(aegis::edit_message_t().message_id(obj.message_id)
					.embed(
						aegis::gateway::objects::embed()
						.color(0x00FF00).thumbnail(aegis::gateway::objects::thumbnail("https://cdn.discordapp.com/avatars/580330179831005205/49035f8777ff7dc50c44bf69e99b30bb.png"))
						.title("Attempting to Change System Settings").url("https://github.com/purge-dev")
						.description("Please enter the new value or press \342\235\214 to cancel.")
						.footer(foot)));

			_cache.discord.await_settings_change.push_back(std::make_pair(_cache.discord.settings_menu[i], obj.emoji.name)); // add this msg and emoji name to check for value
			_cache.discord.settings_menu.erase(_cache.discord.settings_menu.begin() + i);
		}
	}
}


void botInfo::relayChat(std::string user, std::string msg, int zone)
{
	if (CMPSTART("//", msg.c_str())) // spec chat
	{
		String content = msg.c_str();
		content.replace('/', ' ');
		content.replace('/', ' ');
		String set = "[Discord] " + (String)user.c_str() + ">" + content;

		merv->sendTeam(set.msg);
	}
	else if (CMPSTART("t@", msg.c_str())) // skynet commands
		return;
	else if (CMPSTART("!", msg.c_str())) // skynet commands
		return;
	else if (CMPSTART("``", msg.c_str())) // code tags
		return;
	else if (CMPSTART(_cache.discord.bot_prefix.c_str(), msg.c_str())) // prefix
		return;
	else if (CMPSTART("http", msg.c_str())) // gifs
	{
		if (msg.find("gif") != msg.npos)
			return;
	}
	else
	{
		if ((msg.find_first_of('ð') != msg.npos) || (msg.find("<:") != msg.npos) || (msg.find("<#") != msg.npos) ||
			(msg.find("<a:") != msg.npos) || (msg.find("<@") != msg.npos)) // emoji/roles/channels
		{
			for (int i = 0; i < msg.length(); i++) 
			{
				if (msg.find_first_of('ð') != msg.npos) // forgot what this was
					msg.erase(msg.find_first_of('ð'), 2);
				if (msg.find_first_of("<:") != msg.npos) // normal emoji
					msg.erase(msg.find_first_of("<:"), msg.find_first_of('>') + 1 - msg.find_first_of("<:"));
				if (msg.find("<a:") != msg.npos) // animated emoji
					msg.erase(msg.find("<a:"), msg.find_first_of('>') + 1 - msg.find("<a:"));
				if (msg.find("<#") != msg.npos)// channel links
					msg.erase(msg.find("<#") - 1, msg.find_first_of("<#"));
				if (msg.find("<@") != msg.npos) // role/user mentions
					msg.erase(msg.find("<@") - 1, msg.find_first_of("<#"));
			}
		}

		if (zone == 0)
			merv->sendPublic("?fuschiaaa " + (String)user.c_str() + "> " + (String)msg.c_str());

		else if (zone == 1) // TWspec
			merv->sendChannel("5;[Discord] " + (String)user.c_str() + "> " + (String)msg.c_str());
	}
}

std::string botInfo::getKnownAccount(std::string discordID)
{
	std::string res;
	for (int i = 0; i < _cache.discord.elite.accounts.size(); i++)
		if (_cache.discord.elite.accounts[i].first.gets() == discordID)
			res = _cache.discord.elite.accounts[i].second;

	return res;
}

std::string botInfo::getKnownAccount(char* pname)
{
	std::string res;
	for (int i = 0; i < _cache.discord.elite.accounts.size(); i++)
		if (strcmp(_cache.discord.elite.accounts[i].second.c_str(), pname) == 0)
			res = _cache.discord.elite.accounts[i].first.gets();

	return res;
}

bool botInfo::isLinked(String ssName)
{
	for (int i = 0; i < _cache.discord.elite.accounts.size(); i++)
		if (CMPSTR(_cache.discord.elite.accounts[i].second.c_str(), ssName))
			return true;
	return false;
}

bool botInfo::isLinked(std::string discordID) 
{
	for (int i = 0; i < _cache.discord.elite.accounts.size(); i++)
		if (CMPSTR(_cache.discord.elite.accounts[i].first.gets().c_str(), discordID.c_str()))
			return true;
	return false;
}

void botInfo::linkAccount(Player* p, String discordID)
{
	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	sprintf_s(path, "%s\\%s", path, ELITEINI);
	WritePrivateProfileString("Accounts", "@" + discordID, "^" + (String)p->name, path);
	WritePrivateProfileString("Roles", discordID, NULL, path);
	_cache.loadLists(2); // update the cache
}

void botInfo::unlinkAccount(std::string discordID)
{
	std::string accID = "@" + discordID;
	char path[MAX_PATH], res[32];
	GetCurrentDirectory(MAX_PATH, path);
	sprintf_s(path, "%s\\%s", path, ELITEINI);

	WritePrivateProfileString("Accounts", accID.c_str(), NULL, path);
	_cache.discord.bot->find_guild(_cache.discord.flakes.serverID)->remove_guild_member_role((aegis::snowflake)discordID, _cache.discord.flakes.eliteRoleID);
	_cache.loadLists(2);
}

void botInfo::grantDiscordElite(std::string discordID)
{
	_cache.discord.bot->find_guild(_cache.discord.flakes.serverID)->add_guild_member_role((aegis::snowflake)discordID, _cache.discord.flakes.eliteRoleID);
	_cache.discord.bot->create_message(_cache.discord.flakes.mainChannelID, ":trophy: <@" + discordID + ">" +
		" has just unlocked **Elite Tier** by linking his/her **Continuum** account to **Discord**!");

	// TODO: send DM embed explaining elite features
}

void botInfo::DM2Game(String recipient, String msg)
{
	bool exists = false;
	_listnode <Player>* parse = playerlist->head;
	while (parse)
	{
		Player* p = parse->item;
		if (CMPSTR((String)p->name, recipient)) // in-zone PMs
		{
			sendPrivate(p, msg.msg);
			exists = true;
		}
		parse = parse->next;
	}

	if (!exists) // cross-zone PMs
		sendRemotePrivate(recipient.msg, msg.msg);
}

bool botInfo::DM2Discord(std::string out)
{
	std::string name;
	std::string output = out;
	
	output.erase(0, 1);
	size_t delim = output.find_first_of(' ');
	name = output.substr(0, delim);

	if (getUser(name) != nullptr)
	{
		std::string raw_message = output.substr(delim, output.find("$~$@") - delim);
		std::string sender = output.substr(output.find("$~$@") + 4, output.npos - output.find("$~$@"));

		aegis::user* usr = getUser(name);
		aegis::future<aegis::gateway::objects::message> msg_fut = _cache.discord.bot->create_dm_message(usr->get_id(), "```\nPrivate Message from: " + sender + "\n```\n" + raw_message);
		/* might get memory intensive if we keep adding entries to vector unchecked, so finish this part if PMs are popular
		for (int i = 0; i < priv_msg.size(); i++)
		{
			if (std::get<0>(priv_msg[i]) == msg_fut.get().get_channel_id() && std::get<1>(priv_msg[i]) == msg_fut.get().author.id) // a DM b/w bot + discord usr exists
			{
				if (std::get<2>(priv_msg[i]) == sender) // already in dialogue with same player
				{
					openPM = true;
					return true;
				}
			}
		}
		if (!openPM) */
		priv_msg.push_back(std::make_tuple(msg_fut.get().get_channel_id(), usr->get_id(), sender)); // make a new entry with the new sender

		return true;
	}
	else return false;
}

aegis::user* botInfo::getUser(std::string usrname)
{
	aegis::user* found = nullptr;
	auto member_map = _cache.discord.bot->find_guild(_cache.discord.flakes.serverID)->get_members();

	for (auto x : member_map)
		if (CMPnSTART(x.second->get_username().c_str(), usrname.c_str()))
			found = x.second;

	return found;
}

aegis::user* botInfo::getUser(char* usrname)
{
	aegis::user* found = nullptr;
	auto member_map = _cache.discord.bot->find_guild(_cache.discord.flakes.serverID)->get_members();

	for (auto x : member_map)
		if (CMPnSTART(x.second->get_username().c_str(), usrname))
			found = x.second;

	return found;
}

void botInfo::clearDMhandles()
{ // TODO: reset based on lack of activity
	if (priv_msg.size() > 0)
	{
		priv_msg.clear();
		for (int i = 0; i < priv_msg.size(); i++)
		{
			_cache.discord.bot->create_dm_message(std::get<1>(priv_msg[i]), ":mailbox_closed: Your private message handle has been **reset**. Use **"
				+ _cache.discord.bot_prefix + "pm + <continuum_alias>** to start a new PM.");
			std::this_thread::sleep_for(std::chrono::seconds(10));
		}
	}
}

void botInfo::curlChatter(String name, String msg, int ship, std::string channelhook)
{
	String avatar;
	if (ship == 0) // wb
		avatar = "https://cdn.discordapp.com/app-assets/611578814073405465/750459651283222548.png";
	else if (ship == 1)
		avatar = "https://cdn.discordapp.com/app-assets/611578814073405465/750459651392405523.png";
	else if (ship == 2)
		avatar = "https://cdn.discordapp.com/app-assets/611578814073405465/750459651820355605.png";
	else if (ship == 3)
		avatar = "https://cdn.discordapp.com/app-assets/611578814073405465/750459652009099287.png";
	else if (ship == 4)
		avatar = "https://cdn.discordapp.com/app-assets/611578814073405465/750459651560308806.png";
	else if (ship == 5)
		avatar = "https://cdn.discordapp.com/app-assets/611578814073405465/750459651534880899.png";
	else if (ship == 6)
		avatar = "https://cdn.discordapp.com/app-assets/611578814073405465/750459650918580325.png";
	else if (ship == 7)
		avatar = "https://cdn.discordapp.com/app-assets/611578814073405465/750459651019243692.png";
	else if (ship == 8)
		avatar = "https://cdn.discordapp.com/app-assets/611578814073405465/775276910996881429.png";
	else if (ship == 9) // player enter
		avatar = "https://i.ibb.co/3SfRrS9/012-connection-connect-512.png";
	else if (ship == 10)
		avatar = "https://i.ibb.co/pLvLfy0/013-disconnect-connection-512.png";
	else if (ship == 11) // zone bot reconnected
		avatar = "https://i.ibb.co/TqzXnjg/156257735015260342.png";
	else if (ship == 12)
		avatar = "https://i.ibb.co/kqCHY1z/156257743647197917.png";
	else if (ship == 13) // tw logo
		avatar = "https://cdn.discordapp.com/app-assets/611578814073405465/774895705494913074.png";

	CURL* curl;
	CURLcode res;
	struct curl_slist* headerlist = NULL;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (curl)
	{
		headerlist = curl_slist_append(headerlist, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_URL, channelhook);

		String output;
		output += "{\"username\":";
		output += "\"";
		output += name;
		output += "\"";
		output += ",\"";
		output += "avatar_url\"";
		output += ":\"";
		output += avatar;
		output += "\"";
		output += ",\"";
		output += "content\"";
		output += ":\"";
		output += msg;
		output += "\"";
		output += "}";

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output.msg);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
}

