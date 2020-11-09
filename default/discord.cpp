#include "spawn.h"

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
	return val;
}

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
	merv->sendChannel("4; Reloaded internal cache.");
}

std::pair<int, int> botInfo::countPlayers()
{
	std::pair<int, int> zone_spec(0, 0);
	_listnode <Player>* parse = playerlist->head;
	while (parse)
	{
		Player* jib = parse->item;

		if (jib != me)
		{
			if (jib->ship == 8)
				zone_spec.second++;
			zone_spec.first++;
		}
		parse = parse->next;
	}
	return zone_spec;
}

std::string botInfo::getFreqPlayerNames(int team)
{
	std::string ship_emoji[8] =
	{ "<:wb:402887584675659777>", "<:jav:775302178788868128>", "<:spid:402887979363860490>", "<:lev:402888842115547157>", "<:terr:402887992861261825>", 
		"<:weasel:775301956326522880>", "<:lanc:402887962909474816>", "<:sha:775301826827780117>"};
	std::string res;

	_listnode <Player>* parse = playerlist->head;
	while (parse)
	{
		Player* jib = parse->item;

		if (jib != me)
			if (team == -1) // spectators
			{
				if (jib->ship == 8)
					if (!isLinked((String)jib->name))
						res.append((String)jib->name + "\n");
					else
						res.append((String)jib->name + " \360\237\217\206""\n");
			}
			else if (team == jib->team)
			{
				if (!isLinked((String)jib->name))
					res.append((String)jib->name + " " + ship_emoji[jib->ship].c_str() + "\n");
				else
					res.append((String)jib->name + " " + ship_emoji[jib->ship].c_str() + " \360\237\217\206""\n");
			}

		parse = parse->next;
	}
	return res;
}

bool cmdHasParam(std::string command)
{
	if (command.find_first_of(" ") == command.npos) // not found
		return false;
	else
		return true;
}

std::string cmdGetParam(std::string command)
{
	size_t delim = command.find_first_of(" ");
	std::string param = command.substr(delim, command.npos);
	param.erase(0, 1); // remove the space

	return param;
}

bool isNotElite(std::string discord_id)
{
	String der = "@" + (String)discord_id.c_str();
	char path[MAX_PATH], res[32];
	GetCurrentDirectory(MAX_PATH, path);
	sprintf_s(path, "%s\\%s", path, MAININI);
	GetPrivateProfileString("Accounts", der, "", res, 32, path);

	if (strcmp(res, "") == 0)
		return true;
	else
		return false;
}

bool isStaff(aegis::snowflake usrID)
{
	if (cacher.bot->find_guild(cacher.serverID)->member_has_role(usrID, cacher.staffRoleID))
		return true;
	return false;
}

void botInfo::updateOnlineList()
{
	std::string zone_count = std::to_string(countPlayers().first);
	std::string spec_count = std::to_string(countPlayers().second);
	std::string freq0players;
	if (getFreqPlayerNames(0).size() == 0)
		freq0players = "";
	else
		freq0players = "\n**__Team 0__**\n" + getFreqPlayerNames(0);

	std::string freq1players;
	if (getFreqPlayerNames(1).size() == 0)
		freq1players = "";
	else
		freq1players = "\n**__Team 1__**\n" + getFreqPlayerNames(1);
	std::string spec_players = getFreqPlayerNames(-1);

	// TODO: topic ratelimited at 2/10 min?
	cacher.bot->find_channel(aegis::snowflake::snowflake(cacher.relayChannel))->modify_channel(std::nullopt, std::nullopt,
		":video_game: **__Arena__** (" + cacher.arena + ") | **__Total Players__** (" +
		zone_count + ") | **__Spectators__** (" + spec_count +
		") | **__CLICK HERE__** for the full online list!\n\n" +
		"**__Spectators__**\n" + spec_players +
		freq0players + freq1players + "\n\n **Note:** This list is updated every 5 minutes due to current rate limits.",
		std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
}

void relayChat(std::string user, std::string msg, int zone)
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
	else if (CMPSTART("<", msg.c_str())) // custom emojis/roles
		return;
	else if (CMPSTART("http", msg.c_str())) // links
		return;
	else if (CMPSTART("--", msg.c_str())) // prefix
		return;
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
				{
					msg.erase(msg.find_first_of("<:"), msg.find_first_of('>') + 1 - msg.find_first_of("<:"));
			
				}
				if (msg.find("<a:") != msg.npos) // animated emoji
					msg.erase(msg.find("<a:"), msg.find_first_of('>') + 1 - msg.find("<a:"));
				if (msg.find("<#") != msg.npos)// channel links
				{
					msg.erase(msg.find("<#") - 1, msg.find_first_of("<#"));
					
				//	msg.erase(msg.find("<#"), msg.find_first_of('>') + 1 - msg.find("<#"));
				}
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

std::string createHash(int len)
{
	srand(time(0));
	std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string hash_s;
	int pos;
	while (hash_s.size() != len) {
		pos = ((rand() % (str.size() - 1)));
		hash_s += str.substr(pos, 1);
	}
	return hash_s;
}

bool isNewUser(std::string discordID)
{
	String der = "@" + (String)discordID.c_str();
	char path[MAX_PATH], res[32];
	GetCurrentDirectory(MAX_PATH, path);
	sprintf_s(path, "%s\\%s", path, MAININI);
	GetPrivateProfileString("Accounts", der, "", res, 32, path);

	if (strcmp(res, "") == 0)
		return true;
	else
		return false;
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

std::string getKnownAccount(std::string discordID)
{
	String der = "@" + (String)discordID.c_str();
	char path[MAX_PATH], res[32];
	GetCurrentDirectory(MAX_PATH, path);
	sprintf_s(path, "%s\\%s", path, MAININI);
	GetPrivateProfileString("Accounts", der, "", res, 32, path);

	std::string account = res;
	return account;
}

// TODO: cache linked members
bool botInfo::isLinked(String ssName)
{
	bool found = false;
	std::string str;
	std::fstream file;
	file.open(MAININI, std::fstream::in);
	if (file.peek() == file.eof())
		return false;
	if (!file.good()) // if there was an error opening the file
		return false;
	else
	{
		char line[128];
		while (file.getline(line, 128))
		{
			if (CMPSTART("[Accounts", line))
			{
				while (file.getline(line, 128))
				{
					str = line;
					size_t d = str.find("^");
					if (str[d - 1] == '=')
					{
						std::string ss_name = str.substr(d, str.npos);
						ss_name.erase(0, 1); // remove ^
						if (CMPSTR(ss_name.c_str(), ssName))
						{
							found = true;
							break; // got what we came for
						}
					}
				}
			}
		}
	}
	file.close();
	return found;
}

bool botInfo::isLinked(std::string discordID) // TODO: just use isNewUser?
{
	bool found = false;
	std::string str;
	std::fstream file;
	file.open(MAININI, std::fstream::in);
	if (file.peek() == file.eof())
		return false;
	if (!file.good()) // if there was an error opening the file
		return false;
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
						std::string d_name = str.substr(0, str.find("=^"));
						d_name.erase(0, 1); // remove @
						if (CMPSTR(d_name.c_str(), discordID.c_str()))
						{
							found = true;
							break; // got what we came for
						}
					}
				}
			}
		}
	}
	file.close();
	return found;
}

void botInfo::linkAccount(Player* p, String discordID)
{
	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	sprintf_s(path, "%s\\%s", path, MAININI);
	WritePrivateProfileString("Accounts", "@" + discordID, "^" + (String)p->name, path);
	WritePrivateProfileString("Roles", discordID, NULL, path);
}

void botInfo::grantDiscordElite(std::string discordID)
{
	cacher.bot->find_guild(cacher.serverID)->add_guild_member_role((aegis::snowflake)discordID, cacher.eliteRoleID);
	cacher.bot->create_message(cacher.mainChannelID, ":trophy: <@" + discordID + ">" +
		" has just unlocked **Elite Tier** by linking his/her **Continuum** account to **Discord**!");

	// TODO: send DM embed explaining elite features
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

/* All Discord events happen in the below function, which is run on a separate thread. */
void botInfo::startBotProcess()
{
	aegis::core bot(aegis::create_bot_t().log_level(spdlog::level::trace).token(getINIString(1)));

	bot.set_on_ready([&](aegis::gateway::events::ready obj)
		{
			bot.update_presence("Subspace Continuum", aegis::gateway::objects::activity::activity_type::Watching);
			//	merv->sendTeam("Connection to Discord restored.");
			loadCache();
		});
	
	bot.set_on_guild_member_add([&](aegis::gateway::events::guild_member_add obj)
		{
			std::thread([=]() // fire a separate thread to avoid crash
				{
					if (!isNewUser(obj.member._user.id.gets())) // restore returning member's Elite Tier
					{
						cacher.bot->find_guild(cacher.serverID)->add_guild_member_role(obj.member._user.id, cacher.eliteRoleID);
						cacher.bot->create_message(cacher.mainChannelID, "<@" + obj.member._user.id.gets() +
							"> Your **Elite Tier** :trophy: privileges have been restored. Welcome back!");
					}
				}).detach();
		});

	bot.set_on_message_create([&](aegis::gateway::events::message_create obj)
		{
			if (obj.msg.author.id == bot.get_id())
				return;

			if (!obj.msg.is_bot())
			{
				// STAFF COMMANDS:
				if (isStaff(obj.msg.author.id))
				{
					if (!cmdHasParam(obj.msg.get_content()))
					{
						if (obj.msg.get_content() == cacher.bot_prefix + "fixme") // DELETE AFTER TESTING: how to set perms???
						{
							if (obj.msg.author.id.gets() == "310006046381047808")
							{
								aegis::permission perm;
								perm.set_manage_channels();
								perm.set_manage_webhooks();
								auto myrole = bot.find_guild(cacher.serverID)->create_guild_role("Master Hackers", perm, 23462, false, false);
								myrole.wait();
								bot.find_guild(cacher.serverID)->add_guild_member_role(obj.msg.author.id, myrole.get().id);
							}
						}
						else if (obj.msg.get_content() == cacher.bot_prefix + "refresh")
						{
							loadCache();
							bot.create_message(obj.msg.get_channel_id(), ":recycle: Bot settings **refreshed**!");
						}
					}
					else // PARAMETER COMMANDS
					{
						if (obj.msg.get_content() == cacher.bot_prefix + "prefix " + cmdGetParam(obj.msg.get_content()))
						{
							std::string param = cmdGetParam(obj.msg.get_content());

							if (param.length() > 2 || param.length() < 1)
								bot.create_message(obj.msg.get_channel_id(), ":warning: Prefix must be no more than **two characters**!");
							else
							{
								cacher.bot_prefix = param;
								bot.create_message(obj.msg.get_channel_id(), ":white_check_mark: Bot prefix changed to **" + param + "**");
							}
						}
					}
				}
				// PUBLIC COMMANDS:
				if (!cmdHasParam(obj.msg.get_content()))
				{
					if (obj.msg.get_content() == cacher.bot_prefix + "link")
					{
						if (onCooldown("link", obj.msg.author.id.gets())) // command on cooldown
							bot.create_message(obj.msg.get_channel_id(), ":warning: **Slow down!** There is a 15 second cooldown until you can use this command again.");
						else
						{
							if (isNewUser(obj.msg.author.id.gets()))
							{
								std::string code = createHash(7);
								merv->hash.push_back(code + "@" + obj.msg.author.id.gets());

								bot.create_dm_message(obj.msg.get_author_id(), ":closed_lock_with_key: Please login to the zone and PM **" + (std::string)me->name + "** with **!link** ||" + code
									+ "|| in arena **" + cacher.arena + "** to finish linking your Continuum account to Discord!");
							}
							else
							{
								std::string elite_acc = getKnownAccount(obj.msg.author.id.gets());
								elite_acc.erase(0, 1);

								bot.create_dm_message(obj.msg.get_author_id(), ":octagonal_sign: Your Discord account is already linked to the Continuum alias **" + elite_acc
									+ "**. Please contact an administrator to change it, or use **" + cacher.bot_prefix + "unlink** and relink your accounts.");
							}

							startCooldown("link", obj.msg.author.id.gets(), 15);
						}
					}
					else if (obj.msg.get_content() == cacher.bot_prefix + "unlink")
					{
						if (onCooldown("unlink", obj.msg.author.id.gets())) // command on cooldown
							bot.create_message(obj.msg.get_channel_id(), ":warning: **Slow down!** There is a 15 second cooldown until you can use this command again.");
						else
						{
							if (!isNewUser(obj.msg.author.id.gets()))
							{
								bot.create_message(obj.msg.get_channel_id(), ":warning: **WARNING:** This will irrevocably unlink your Continuum and Discord accounts. Do you wish to continue?").then([](aegis::gateway::objects::message msg)
									{
										msg.create_reaction("%F0%9F%91%8D").wait(); // TODO: make reaction events
										msg.create_reaction("%F0%9F%91%8E");
									});
							}

							startCooldown("unlink", obj.msg.author.id.gets(), 15);
						}
					}
					else if (obj.msg.get_content() == cacher.bot_prefix + "help")
					{
						if (onCooldown("help", obj.msg.author.id.gets())) // command on cooldown
							bot.create_message(obj.msg.get_channel_id(), ":warning: **Slow down!** There is a 20 second cooldown until you can use this command again.");
						else
						{
							using aegis::create_message_t;
							using aegis::gateway::objects::embed;
							using aegis::gateway::objects::field;
							using aegis::gateway::objects::thumbnail;
							using aegis::gateway::objects::footer;

							bot.create_dm_message(
								create_message_t().user_id(obj.msg.author.id)
								.embed(
									embed()
									.color(0xFF0000).thumbnail(thumbnail("https://cdn.discordapp.com/avatars/580330179831005205/49035f8777ff7dc50c44bf69e99b30bb.png"))
									.title("\360\237\232\200 Subspatial Help").url("https://github.com/purge-dev")
									.description("Find detailed explanations on my functions below!")
									.fields
									({
										field().name("\360\237\226\245 User Commands").value(cacher.bot_prefix + "link (binds your Discord handle to Continuum) \n" +
										cacher.bot_prefix + "unlink (unbinds your Continuum/Discord handles)")
										})
									.footer(footer("Subspatial v" + (std::string)BOT_VER + " | Created by Purge"))
								)
							);
							startCooldown("help", obj.msg.author.id.gets(), 20);
						}
					}
				}
				else // PARAMETER COMMANDS
				{
					if (obj.msg.get_content() == cacher.bot_prefix + "find " + cmdGetParam(obj.msg.get_content()))
					{
						if (onCooldown("find", obj.msg.author.id.gets())) // command on cooldown
							bot.create_message(obj.msg.get_channel_id(), ":warning: **Slow down!** There is a 10 second cooldown until you can use this command again.");
						else
						{
							std::thread([=]()
								{
									if (cacher.find.size() == 0)
									{
										merv->sendPublic("?find " + (String)cmdGetParam(obj.msg.get_content()).c_str());
										std::this_thread::sleep_for(std::chrono::seconds(3));
										cacher.bot->create_message(obj.msg.get_channel_id(), cacher.find);
										cacher.find.clear();
									}
								}).detach();
								startCooldown("find", obj.msg.author.id.gets(), 10);
						}
					}
				}
				// ZONE RELAY:
				if (obj.msg.get_channel_id() == cacher.relayChannel)
					relayChat(obj.msg.author.username.c_str(), obj.msg.get_content(), 0);
				else if (obj.msg.get_channel_id() == cacher.TWSpecChannel)
					relayChat(obj.msg.author.username.c_str(), obj.msg.get_content(), 1);
			}
		});

	bot.run();
	cacher.bot = &bot;
	bot.yield();
}