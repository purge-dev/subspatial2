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
			std::thread([=]() // fire a separate thread to avoid crash
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
		}
	);
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
							_cache.loadCache();
							_cache.loadLists(0);
							_cache.loadLists(1);
							_cache.loadLists(2);
							_cache.loadLists(3);
							bot.create_message(obj.msg.get_channel_id(), ":recycle: Bot settings **refreshed**!");
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

								bot.create_dm_message(obj.msg.get_author_id(), ":closed_lock_with_key: Please login to the zone and PM **" + (std::string)me->name + "** with **!link** ||" + code
									+ "|| in arena **" + _cache.game.arena + "** to finish linking your Continuum account to Discord!");
							}
							else
							{
								std::string elite_acc = getKnownAccount(obj.msg.author.id.gets());

								bot.create_dm_message(obj.msg.get_author_id(), ":octagonal_sign: Your Discord account is already linked to the Continuum alias **" + elite_acc
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
							if (isLinked(obj.msg.author.id.gets()))
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
					else if (obj.msg.get_content() == _cache.discord.bot_prefix + "help") // TODO: finish menu
					{
						if (onCooldown("help", obj.msg.author.id.gets())) 
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
										field().name("\360\237\226\245 User Commands").value(_cache.discord.bot_prefix + "link (binds your Discord handle to Continuum) \n" +
										_cache.discord.bot_prefix + "unlink (unbinds your Continuum/Discord handles)")
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
					if (obj.msg.get_content() == _cache.discord.bot_prefix + "find " + cmdGetParam(obj.msg.get_content())) // outputs a pretty embed for ?find
					{
						if (onCooldown("find", obj.msg.author.id.gets())) // command on cooldown
							bot.create_message(obj.msg.get_channel_id(), ":warning: **Slow down!** There is a 10 second cooldown until you can use this command again.");
						else
						{
							std::thread([=]()
								{
									if (_cache.discord.find.size() == 0)
									{
										merv->sendPublic("?find " + (String)cmdGetParam(obj.msg.get_content()).c_str());
										std::this_thread::sleep_for(std::chrono::seconds(3));
										std::string name, status, type;
										int32_t color;
										using aegis::create_message_t;
										using aegis::gateway::objects::embed;
										using aegis::gateway::objects::field;
										using aegis::gateway::objects::thumbnail;
										using aegis::gateway::objects::footer;

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

										_cache.discord.bot->find_channel(obj.channel)->create_message(create_message_t()
											.embed(
												embed()
												.color(color).thumbnail(thumbnail("https://cdn.discordapp.com/avatars/580330179831005205/49035f8777ff7dc50c44bf69e99b30bb.png"))
												.title("\360\237\232\200 Continuum User Lookup").url("https://store.steampowered.com/app/352700")
												.description(name)
												.fields
												({
													field().name(type).value(status)
													})
												.footer(footer("The above lookup was done by " + std::string(me->name) + " in " + _cache.game.zone))
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

		});

	bot.run();
	_cache.discord.bot = &bot; // live life on the edge
	bot.yield();
}