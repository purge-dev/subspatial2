#include "spawn.h"
/* discord.cpp ~
* 
*  Functions here deal with the Discord interface and Elite tier account handling.
*  All other custom functions are in spawn.cpp.
*/

void botInfo::loadCache()
{
	cacher.bot_prefix = "--";
	cacher.token = getINIString(1);
	cacher.relayChannel = (aegis::snowflake)getINIString(2);
	cacher.relayWebhook = getINIString(3);
	cacher.serverID = (aegis::snowflake)getINIString(4);
	cacher.TWSpecWebhook = getINIString(5);
	cacher.TWSpecChannel = (aegis::snowflake)getINIString(6);
	cacher.eliteRoleID = (aegis::snowflake)getINIString(7);
	cacher.mainChannelID = (aegis::snowflake)getINIString(8);
	cacher.staffRoleID = (aegis::snowflake)getINIString(9);
	cacher.spamHook = getINIString(10);
}

void botInfo::loadLists(int type)
{
	if (type == 0) // muted list
	{
		list.muted.clear();

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
							list.muted.push_back(found);
						}
					}
				}
			}
		}
		file.close();
	}
	else if (type == 1) // topic ignore list
	{
		list.ignored.clear();

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
							list.ignored.push_back(found);
						}
					}
				}
			}
		}
	}
	else if (type == 2) // elite players
	{
		list.elite.clear();

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
							list.elite.push_back(std::make_pair((aegis::snowflake)usrID, pname));
						}
					}
				}
			}
		}
		file.close();
	}
}

std::string botInfo::getElitePairStrings()
{
	std::string res;
	for (int i = 0; i < list.elite.size(); i++)
		res.append("(" + list.elite[i].first.gets() + ":" + list.elite[i].second + "), ");
	return res;
}

bool botInfo::isStaff(aegis::snowflake usrID)
{
	if (cacher.bot->find_guild(cacher.serverID)->member_has_role(usrID, cacher.staffRoleID))
		return true;
	else
		return false;
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
	cacher.bot->find_channel(aegis::snowflake::snowflake(cacher.relayChannel))->modify_channel(std::nullopt, std::nullopt,
		":video_game: **__Arena__** (" + cacher.arena + ") | **__Total Players__** (" +
		zone_count + ") | **__Spectators__** (" + spec_count +
		") | **__CLICK HERE__** for the full online list!\n\n" +
		"**__Spectators__**\n" + spec_players +
		freq0players + freq1players + "\n\n **Note:** This list is updated every 5 minutes due to current rate limits.",
		std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
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
	else if (CMPSTART(cacher.bot_prefix.c_str(), msg.c_str())) // prefix
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
				if (msg.find_first_of('ð') != msg.npos)
					msg.erase(msg.find_first_of('ð'), 2);
				if (msg.find_first_of("<:") != msg.npos) // normal emoji TODO: remove emoji at beginning of msg but preserve rest of msg
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
	for (int i = 0; i < list.elite.size(); i++)
		if (list.elite[i].first.gets() == discordID)
			res = list.elite[i].second;

	return res;
}

std::string botInfo::getKnownAccount(char* pname)
{
	std::string res;
	for (int i = 0; i < list.elite.size(); i++)
		if (strcmp(list.elite[i].second.c_str(), pname) == 0)
			res = list.elite[i].first.gets();

	return res;
}

bool botInfo::isLinked(String ssName)
{
	for (int i = 0; i < list.elite.size(); i++)
		if (CMPSTR(list.elite[i].second.c_str(), ssName))
			return true;
	return false;
}

bool botInfo::isLinked(std::string discordID) 
{
	for (int i = 0; i < list.elite.size(); i++)
		if (CMPSTR(list.elite[i].first.gets().c_str(), discordID.c_str()))
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
	loadLists(2); // update the cache
}

void botInfo::unlinkAccount(std::string discordID)
{
	std::string accID = "@" + discordID;
	char path[MAX_PATH], res[32];
	GetCurrentDirectory(MAX_PATH, path);
	sprintf_s(path, "%s\\%s", path, ELITEINI);

	WritePrivateProfileString("Accounts", accID.c_str(), NULL, path);
	cacher.bot->find_guild(cacher.serverID)->remove_guild_member_role((aegis::snowflake)discordID, cacher.eliteRoleID);
	loadLists(2);
}

void botInfo::grantDiscordElite(std::string discordID)
{
	cacher.bot->find_guild(cacher.serverID)->add_guild_member_role((aegis::snowflake)discordID, cacher.eliteRoleID);
	cacher.bot->create_message(cacher.mainChannelID, ":trophy: <@" + discordID + ">" +
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
		aegis::future<aegis::gateway::objects::message> msg_fut = cacher.bot->create_dm_message(usr->get_id(), "```\nPrivate Message from: " + sender + "\n```\n" + raw_message);
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
	auto member_map = cacher.bot->find_guild(cacher.serverID)->get_members();

	for (auto x : member_map)
		if (CMPnSTART(x.second->get_username().c_str(), usrname.c_str()))
			found = x.second;

	return found;
}

aegis::user* botInfo::getUser(char* usrname)
{
	aegis::user* found = nullptr;
	auto member_map = cacher.bot->find_guild(cacher.serverID)->get_members();

	for (auto x : member_map)
		if (CMPnSTART(x.second->get_username().c_str(), usrname))
			found = x.second;

	return found;
}

void botInfo::clearDMhandles()
{ // TODO: reset based on lack of activity
	if (priv_msg.size() > 0)
		for (int i = 0; i < priv_msg.size(); i++)
		{
			cacher.bot->create_dm_message(std::get<1>(priv_msg[i]), ":mailbox_closed: Your private message handle has been **reset**. Use **"
				+ cacher.bot_prefix + "pm + <continuum_alias>** to start a new PM.");
			std::this_thread::sleep_for(std::chrono::seconds(10));
		}

	priv_msg.clear();
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

