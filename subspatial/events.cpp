#include "spawn.h"
/* events.cpp ~
* 
*  All Discord callbacks/events are handled within the startBotProcess function, which is run on a separate thread.
*  Any reference made to the Discord bot object should be carefully handled in memory with Cacher{}
*/
std::string createHash(int len) 
{
	std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string hash_s;
	while (hash_s.size() != len) 
		hash_s += str.substr(merv->randomizer(str.length()), 1);

	return hash_s;
}

void botInfo::startBotProcess()
{
	aegis::core bot(aegis::create_bot_t().log_level(spdlog::level::trace).token(getINIString(1)));

	bot.set_on_ready([&](aegis::gateway::events::ready obj)
		{
			bot.update_presence("Subspace Continuum", aegis::gateway::objects::activity::activity_type::Watching);
		//	merv->sendTeam("[Connection to Discord restored]");
			std::thread([=]() // run in separate thread to avoid startup slowdowns in slow regions 
				{
					_cache.loadCache();
					_cache.loadLists(0);
					_cache.loadLists(1);
					_cache.loadLists(2);
					_cache.loadLists(3);
					merv->sendChannel("4; Reloaded internal cache and data files.");
				}).detach();
		});

	bot.set_on_guild_member_add([&](aegis::gateway::events::guild_member_add obj)
		{
			std::thread([=]() 
				{
					if (isLinked(obj.member._user.id.gets())) // restore returning member's Elite Tier
					{
						_cache.discord.bot->find_guild(_cache.discord.flakes.serverID)->add_guild_member_role(obj.member._user.id, _cache.discord.flakes.eliteRoleID);
						_cache.discord.bot->create_message(_cache.discord.flakes.mainChannelID, "<@" + obj.member._user.id.gets() +
							"> Your **Elite Tier** :trophy: privileges have been restored. Welcome back!");
					}
				}).detach();
		});

	bot.set_on_message_create_dm([&](aegis::gateway::events::message_create obj)
		{
			if (obj.msg.author.id == bot.get_id())
				return;

			// CROSS PMs:
			if (!obj.msg.is_bot())
			{
				std::string recipient;
				if (priv_msg.size() > 0)
				{
					for (int i = 0; i < priv_msg.size(); i++)
					{
						if (std::get<0>(priv_msg[i]) == obj.msg.get_channel_id() && std::get<1>(priv_msg[i]) == obj.msg.author.id) // this is an open PM
							recipient = std::get<2>(priv_msg[i]); // we should relay msg content to this guy
					}
					DM2Game(recipient.c_str(), "[" + (String)obj.msg.author.username.c_str() + "] " + obj.msg.get_content().c_str());
				}
			}
			/* TODO
			if (_cache.discord.await_settings_change.size() > 0)
			{
				for (int i = 0; i < _cache.discord.await_settings_change.size(); i++)
				{
					if (_cache.discord.await_settings_change[i].first->author)
				}
			} */
		});

	bot.set_on_message_reaction_add([&](aegis::gateway::events::message_reaction_add obj)
	{
			if (obj.user_id == bot.get_id()) return;

			checkUnlinker(obj);
			checkSettingsCommand(obj);
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
						if (obj.msg.get_content() == _cache.discord.bot_prefix + "refresh")
						{
							_cache.loadCache(); _cache.loadLists(0); _cache.loadLists(1); _cache.loadLists(2); _cache.loadLists(3);
							bot.create_message(obj.msg.get_channel_id(), ":recycle: Bot settings **refreshed**!");
						}
						else if (obj.msg.get_content() == _cache.discord.bot_prefix + "settings")
						{
							if (onCooldown("settings", "global"))
								bot.create_message(obj.msg.get_channel_id(), ":warning: **Slow down!** Someone recently used this command. Please wait 10 seconds.");
							else
							{
								// DM embed with options to change channel IDs/staff ID/etc.
								DEF_FOOTER;
								_cache.discord.bot->create_dm_message(
									aegis::create_message_t().user_id(obj.msg.author.id)
									.embed(
										aegis::gateway::objects::embed()
										.color(0xFFFF00).thumbnail(aegis::gateway::objects::thumbnail("https://cdn.discordapp.com/avatars/580330179831005205/49035f8777ff7dc50c44bf69e99b30bb.png"))
										.title("\342\232\231 Settings Menu").url("https://github.com/purge-dev")
										.description("Please select a setting to change from the below options.")
										.fields
										({
											aegis::gateway::objects::field().name("1").value("Relay Category ID"),
											aegis::gateway::objects::field().name("2").value("Main Chat Channel ID"),
											aegis::gateway::objects::field().name("3").value("Relay Channel ID"),
											aegis::gateway::objects::field().name("4").value("Music Channel ID"),
											aegis::gateway::objects::field().name("5").value("Elite Role ID"),
											aegis::gateway::objects::field().name("6").value("Staff Role ID"),
											aegis::gateway::objects::field().name("7").value("Relay Webhook"),
											aegis::gateway::objects::field().name("8").value("Spam Webhook"),
											aegis::gateway::objects::field().name("9").value("Server ID")
											})
										.footer(foot)
									)
								).then([](aegis::gateway::objects::message settings_msg)
									{
										_cache.discord.settings_menu.push_back(&settings_msg); // queue the msg to check for reactions
										settings_msg.create_reaction("1%EF%B8%8F%E2%83%A3").wait(); // reactions must be URL encoded
										std::this_thread::sleep_for(std::chrono::seconds(1));
										settings_msg.create_reaction("2%EF%B8%8F%E2%83%A3");
										std::this_thread::sleep_for(std::chrono::seconds(1));
										settings_msg.create_reaction("3%EF%B8%8F%E2%83%A3");
										std::this_thread::sleep_for(std::chrono::seconds(1));
										settings_msg.create_reaction("4%EF%B8%8F%E2%83%A3");
										std::this_thread::sleep_for(std::chrono::seconds(1));
										settings_msg.create_reaction("5%EF%B8%8F%E2%83%A3");
										std::this_thread::sleep_for(std::chrono::seconds(1));
										settings_msg.create_reaction("6%EF%B8%8F%E2%83%A3");
										std::this_thread::sleep_for(std::chrono::seconds(1));
										settings_msg.create_reaction("7%EF%B8%8F%E2%83%A3");
										std::this_thread::sleep_for(std::chrono::seconds(1));
										settings_msg.create_reaction("8%EF%B8%8F%E2%83%A3");
										std::this_thread::sleep_for(std::chrono::seconds(1));
										settings_msg.create_reaction("9%EF%B8%8F%E2%83%A3");
										settings_msg.create_reaction("%E2%9D%8C");		
									});
								startCooldown("settings", "global", 10);
							}
						}
					}
					else // PARAMETER COMMANDS
					{
						if (obj.msg.get_content() == _cache.discord.bot_prefix + "prefix " + cmdGetParam(obj.msg.get_content()))
						{
							std::string param = cmdGetParam(obj.msg.get_content());

							if (param.length() > 2 || param.length() < 1)
								bot.create_message(obj.msg.get_channel_id(), ":warning: Prefix must be no more than **two characters**!");
							else
							{
								_cache.discord.bot_prefix = param;
								bot.create_message(obj.msg.get_channel_id(), ":white_check_mark: Bot prefix changed to **" + param + "**");
							}
						}
					}
				}
				// PUBLIC COMMANDS:
				if (!cmdHasParam(obj.msg.get_content()))
				{
					if (obj.msg.get_content() == _cache.discord.bot_prefix + "link")
					{
						if (onCooldown("link", obj.msg.author.id.gets())) // command on cooldown
							bot.create_message(obj.msg.get_channel_id(), ":warning: **Slow down!** There is a 15 second cooldown until you can use this command again.");
						else
						{
							if (!isLinked(obj.msg.author.id.gets()))
							{
								std::string code = createHash(7);
								merv->hash.push_back(code + "@" + obj.msg.author.id.gets());

								bot.create_dm_message(obj.msg.get_author_id(), ":closed_lock_with_key: Please login to **" + _cache.game.zone.substr(0, _cache.game.zone.find_last_of("."))
									+ " (" + _cache.game.arena + ")** and PM **" + (std::string)me->name + "** with **!link** ||" + code
									+ "|| to finish linking your Continuum account to Discord!");
							}
							else
							{
								std::string elite_acc = getKnownAccount(obj.msg.author.id.gets());

								bot.create_dm_message(obj.msg.get_author_id(), ":no_entry: Your Discord account is already linked to the Continuum alias **" + elite_acc
									+ "**. Please contact an administrator to change it, or use **" + _cache.discord.bot_prefix + "unlink** and relink your accounts.");
							}

							startCooldown("link", obj.msg.author.id.gets(), 15);
						}
					}
					else if (obj.msg.get_content() == _cache.discord.bot_prefix + "unlink")
					{
						if (onCooldown("unlink", obj.msg.author.id.gets()))
							bot.create_message(obj.msg.get_channel_id(), ":warning: **Slow down!** There is a 15 second cooldown until you can use this command again.");
						else
						{
							std::thread([=]()
								{
									if (isLinked(obj.msg.author.id.gets()))
									{
										DEF_FOOTER;
										_cache.discord.bot->create_dm_message(
											aegis::create_message_t().user_id(obj.msg.author.id)
											.embed(
												aegis::gateway::objects::embed()
												.color(0xFFFF00).thumbnail(aegis::gateway::objects::thumbnail("https://cdn.discordapp.com/avatars/580330179831005205/49035f8777ff7dc50c44bf69e99b30bb.png"))
												.title("Continuum/Discord Account Unlinker").url("https://github.com/purge-dev")
												.description("\342\232\240 You are about to unlink your Discord/Continuum accounts.")
												.fields
												({
													aegis::gateway::objects::field().name("CONTINUE?").value("Select \342\234\205 to complete the unlinking process.\n\n**OR**\n\nSelect \342\233\224 to abort this process.")
													})
												.footer(foot)
											)
										).then([](aegis::gateway::objects::message unlink_msg)
											{
												_cache.discord.unlinker.push_back(&unlink_msg); // add message to unlinker list and wait for reaction reply
												unlink_msg.create_reaction("%E2%9C%85").wait(); // reactions must be URL encoded
												std::this_thread::sleep_for(std::chrono::seconds(1));
												unlink_msg.create_reaction("%E2%9B%94");
											});
									}
								}).detach();

								startCooldown("unlink", obj.msg.author.id.gets(), 15);
						}
					}
					else if (obj.msg.get_content() == _cache.discord.bot_prefix + "help")
					{
						if (onCooldown("help", obj.msg.author.id.gets()))
							bot.create_message(obj.msg.get_channel_id(), ":warning: **Slow down!** There is a 20 second cooldown until you can use this command again.");
						else
						{
							DEF_FOOTER;
							bot.create_dm_message(
								aegis::create_message_t().user_id(obj.msg.author.id)
								.embed(
									aegis::gateway::objects::embed()
									.color(0x800080).thumbnail(aegis::gateway::objects::thumbnail("https://cdn.discordapp.com/avatars/580330179831005205/49035f8777ff7dc50c44bf69e99b30bb.png"))
									.title("\360\237\232\200 Subspatial Help").url("https://github.com/purge-dev")
									//	.description("Find detailed explanations on my functions below!\n")
									.fields
									({
										aegis::gateway::objects::field().name("\360\237\226\245 User Commands").value("**" + _cache.discord.bot_prefix + "link** (binds your Discord handle to Continuum) \n" +
										"**" + _cache.discord.bot_prefix + "stats** (displays some cool statistics)\n" +
										"**" + _cache.discord.bot_prefix + "find** <PlayerName> (runs a ?find search in SSC)\n" +
										"**" + _cache.discord.bot_prefix + "unlink** (unbinds your Continuum/Discord handles) \360\237\217\206 \n\n" +
										"\360\237\217\206 **Elite Tier Perks** \n" +
											"**- Cross-platform DMs**\n" +
										"**- Continuum to Discord user mentions**\n" +
										"**- Exclusive access to evolving economy**"),
										})
									.footer(foot)
								)
							);
							startCooldown("help", obj.msg.author.id.gets(), 20);
						}
					}
					else if (obj.msg.get_content() == _cache.discord.bot_prefix + "stats")
					{
						if (onCooldown("stats", "global"))
							bot.create_message(obj.msg.get_channel_id(), ":warning: **Slow down!** Someone recently used this command. Please wait 5 minutes.");
						else
						{
							std::thread([=]()
								{		
									DEF_FOOTER;
									merv->sendPublic("?uptime"); merv->sendPublic("?usage");
									std::this_thread::sleep_for(std::chrono::seconds(2));

									_cache.discord.bot->find_channel(obj.channel)->create_message(
										aegis::create_message_t()
										.embed(
											aegis::gateway::objects::embed()
											.color(0x800080).thumbnail(aegis::gateway::objects::thumbnail("https://media.tenor.com/images/c0dc2d892c76ef0802f57cfdd5ac8415/tenor.gif"))
											.title("\360\237\232\200 Subspatial Statistics").url("https://github.com/purge-dev")
											.description("Check out what's ticking under my hood!")
											.fields
											({
												aegis::gateway::objects::field().name("\360\237\216\256 " + _cache.game.zone.substr(0, _cache.game.zone.find_last_of("."))).value("**Current Arena:** " + _cache.game.arena +
													"\n**Total Playing:** " + std::to_string(countPlayers(1).first + countPlayers(1).second) + "\n**Total Spectators:** " + std::to_string(countPlayers(0).second)).is_inline(true),

												aegis::gateway::objects::field().name("\360\237\217\206 Elite Tier").value("**Newest Member:** " + _cache.discord.elite.accounts.front().second
													+ "\n**Total Users:** " + std::to_string(_cache.discord.elite.accounts.size())).is_inline(true),

												aegis::gateway::objects::field().name("\342\232\231 Server Stats").value("**Zone Uptime:** " + ((_cache.statistics.zone_uptime != "") ? _cache.statistics.zone_uptime : "``ERROR``") +
													"\n**Zone Bot:** " + ((_cache.statistics.zone_uptime != "") ? "``\342\234\205 Connected for " + _cache.statistics.bot_uptime + " (h:m:s)``" : "``\342\230\240 Disconnected``") + 
												"\n**Discord:** `` \342\234\205 Connected for " + _cache.discord.bot->uptime_str() + "``")						
												})
											.footer(foot)
										)
									);
									_cache.statistics.zone_uptime.clear();
									_cache.statistics.bot_uptime.clear();
									startCooldown("stats", "global", 300); // create a global cooldown, not per-player
								}).detach();
						}
					}
				}
				else // PARAMETER COMMANDS
				{
					if (obj.msg.get_content() == _cache.discord.bot_prefix + "find " + cmdGetParam(obj.msg.get_content())) // outputs a pretty embed for ?find
					{
						if (onCooldown("find", obj.msg.author.id.gets())) // command on cooldown
							bot.create_message(obj.msg.get_channel_id(), ":warning: **Slow down!** There is a 10 second cooldown until you can use this command again.");
						else
						{
							std::thread([=]()
								{
									if (_cache.discord.find.empty())
									{
										merv->sendPublic("?find " + (String)cmdGetParam(obj.msg.get_content()).c_str());
										std::this_thread::sleep_for(std::chrono::seconds(3));
										std::string name, status, type;
										int32_t color;

										if (CMPSTART("Not online", _cache.discord.find.c_str()))
										{
											color = 0xFF0000;
											name = "**" + cmdGetParam(obj.msg.get_content()) + "**";
											type = "\342\233\224 OFFLINE";
											status = "**Last Seen:** " + _cache.discord.find.substr(_cache.discord.find.find(" last seen ") + 11, _cache.discord.find.npos -
												_cache.discord.find.find(" last seen "));
										}
										else if (CMPSTART("Unknown", _cache.discord.find.c_str()))
										{
											color = 0xFFFF00;
											name = "**" + cmdGetParam(obj.msg.get_content()) + "**";
											type = "\342\232\240 ERROR";
											status = "Not a known user on the **SSC network**.";
										}
										else if (_cache.discord.find == "")
										{
											color = 0xFFFF00;
											name = "**" + cmdGetParam(obj.msg.get_content()) + "**";
											type = "\342\232\240 Connection ERROR";
											status = "**Unable to fetch data (SSC link down)**";
										}
										else
										{
											color = 0x008000;
											name = "**" + _cache.discord.find.substr(0, _cache.discord.find.find(" is in")) + "**";
											if (_cache.discord.find.find(" is in arena") != _cache.discord.find.npos)
											{
												type = "\342\234\205 ONLINE";
												status = "Currently in arena **" + _cache.discord.find.substr(_cache.discord.find.find(" is in arena ") + 13, _cache.discord.find.npos -
													_cache.discord.find.find(" is in arena ")) + "**";
											}
											else
											{
												type = "\342\234\205 ONLINE";
												status = "Currently in **" + _cache.discord.find.substr(_cache.discord.find.find(" is in ") + 7, _cache.discord.find.npos -
													_cache.discord.find.find(" is in ")) + "**";
											}
										}

										_cache.discord.bot->find_channel(obj.channel)->create_message(aegis::create_message_t()
											.embed(
												aegis::gateway::objects::embed()
												.color(color).thumbnail(aegis::gateway::objects::thumbnail("https://cdn.discordapp.com/avatars/580330179831005205/49035f8777ff7dc50c44bf69e99b30bb.png"))
												.title("\360\237\224\215 Continuum User Lookup").url("https://store.steampowered.com/app/352700")
												.description(name)
												.fields
												({
													aegis::gateway::objects::field().name(type).value(status)
													})
												.footer(aegis::gateway::objects::footer("The above lookup was performed by " + std::string(me->name) + " in " + _cache.game.zone))
											)
										);
										_cache.discord.find.clear();
									}
									startCooldown("find", obj.msg.author.id.gets(), 10);
								}).detach();
						}
					}
				}
				// ZONE RELAY:
				if (obj.msg.get_channel_id() == _cache.discord.flakes.relayChannel)
					relayChat(obj.msg.author.username, obj.msg.get_content(), 0);
				else if (obj.msg.get_channel_id() == _cache.discord.flakes.TWSpecChannel)
					relayChat(obj.msg.author.username, obj.msg.get_content(), 1);
			}
			else
				// MUSIC TICKER:
				if (obj.msg.get_channel_id() == _cache.discord.flakes.MusicChannelID)
					parseMusic(obj); 

		});

	bot.run();
	_cache.discord.bot = &bot; // live life on the edge in the name of Subspace
	bot.yield();
}