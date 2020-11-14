#include "spawn.h"
/* events.cpp ~
* 
*  All Discord callbacks/events are handled within the startBotProcess function, which is run on a separate thread.
*  Any reference made to the Discord bot object should be carefully handled in memory with Cacher{}
*/
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

void botInfo::startBotProcess()
{
	aegis::core bot(aegis::create_bot_t().log_level(spdlog::level::trace).token(getINIString(1)));

	bot.set_on_ready([&](aegis::gateway::events::ready obj)
		{
			bot.update_presence("Subspace Continuum", aegis::gateway::objects::activity::activity_type::Watching);
			//	merv->sendTeam("Connection to Discord restored.");
			std::thread([=]() // run in separate thread to avoid startup slowdowns in slow regions 
				{
					loadCache();
					std::this_thread::sleep_for(std::chrono::seconds(2));
					loadLists(0);
					std::this_thread::sleep_for(std::chrono::seconds(2));
					loadLists(1);
					std::this_thread::sleep_for(std::chrono::seconds(2));
					loadLists(2);
					merv->sendChannel("4; Reloaded internal cache and data files.");
				}).detach();
		});

	bot.set_on_guild_member_add([&](aegis::gateway::events::guild_member_add obj)
		{
			std::thread([=]() // fire a separate thread to avoid crash
				{
					if (isLinked(obj.member._user.id.gets())) // restore returning member's Elite Tier
					{
						cacher.bot->find_guild(cacher.serverID)->add_guild_member_role(obj.member._user.id, cacher.eliteRoleID);
						cacher.bot->create_message(cacher.mainChannelID, "<@" + obj.member._user.id.gets() +
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
							if (!isLinked(obj.msg.author.id.gets()))
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
					else if (obj.msg.get_content() == cacher.bot_prefix + "help") // TODO: finish menu
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

							if (cacher.find.size() == 0)
							{
								merv->sendPublic("?find " + (String)cmdGetParam(obj.msg.get_content()).c_str());
								std::this_thread::sleep_for(std::chrono::seconds(3));
								std::string name, status, type;
								using aegis::create_message_t;
								using aegis::gateway::objects::embed;
								using aegis::gateway::objects::field;
								using aegis::gateway::objects::thumbnail;
								using aegis::gateway::objects::footer;

								
								if (CMPSTART(cacher.find.c_str(), "Not online"))
								{
									name = cmdGetParam(obj.msg.get_content());
									type = "Offline";
									status = "Last Seen " + cacher.find.substr(cacher.find.find(" last seen "), cacher.find.npos -
										cacher.find.find(" last seen "));
								}
								else
								{
									name = cacher.find.substr(0, cacher.find.find(" is in"));
									if (cacher.find.find(" is in arena") != cacher.find.npos)
									{
										type = "Arena";
										status = cacher.find.substr(cacher.find.find(" is in arena "), cacher.find.npos -
											cacher.find.find(" is in arena "));
									}
									else
									{
										type = "Zone";
										status = cacher.find.substr(cacher.find.find(" is in "), cacher.find.npos -
											cacher.find.find(" is in "));
									}
									//	cacher.bot->create_message(obj.msg.get_channel_id(), cacher.find);
								}

								cacher.bot->find_channel(obj.channel)->create_message(create_message_t()
									.embed(
										embed()
										.color(0xFF0000).thumbnail(thumbnail("https://cdn.discordapp.com/avatars/580330179831005205/49035f8777ff7dc50c44bf69e99b30bb.png"))
										.title("\360\237\232\200 Continuum User Lookup").url("https://store.steampowered.com/app/352700")
										.description(type)
										.fields
										({
											field().name(type).value(type)
											})
										.footer(footer("The following lookup was done by " + std::string(me->name) + " on the SSC network."))
									)
								);

								bot.create_message(obj.channel, name + ":" + type + ":" + status);
								cacher.find.clear();
							}

							startCooldown("find", obj.msg.author.id.gets(), 10);
						}
					}
				}
				// ZONE RELAY:
				if (obj.msg.get_channel_id() == cacher.relayChannel)
					relayChat(obj.msg.author.username, obj.msg.get_content(), 0);
				else if (obj.msg.get_channel_id() == cacher.TWSpecChannel)
					relayChat(obj.msg.author.username, obj.msg.get_content(), 1);
			}
		});

	bot.run();
	cacher.bot = &bot;
	bot.yield();
}