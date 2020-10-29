#pragma once

#include <aegis.hpp> // Discord library, thanks to Sharon (https://github.com/zeroxs)

std::string getINIString(int type);

class Discord
{
public:
	void startBotProcess()
	{
		std::thread([=]()
			{
				aegis::core bot(aegis::create_bot_t().log_level(spdlog::level::trace).token(getINIString(1)));

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