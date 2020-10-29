#pragma once

#include <aegis.hpp> // Discord library, thanks to Sharon (https://github.com/zeroxs)

class Discord
{
public:
	void startBotProcess()
	{
		std::thread([=]()
			{
				aegis::core bot(aegis::create_bot_t().log_level(spdlog::level::trace).token("NTgwMzMwMTc5ODMxMDA1MjA1.XOPImA.0NXoGMJicfs9hGh3Zmq_0RKzkZA"));

				// EVENTS:
				bot.set_on_message_create([](auto obj)
					{
						std::string bot_prefix = "--";

						if (obj.msg.get_content() == bot_prefix + "Hi")
							obj.msg.get_channel().create_message(fmt::format("Hello {}", obj.msg.author.username));
					});

				bot.run();
				bot.yield();
			}).detach();
	}
};