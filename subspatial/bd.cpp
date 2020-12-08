#include "spawn.h"
/* bd.cpp ~
*
*  SSCJ Devastation baseduel related code for game results, stat tracking, etc.
*  Any other zones wishing to use the below functions should edit the strings accordingly.
*/

bool botInfo::isBDVictory(std::string msg)
{
	if (_cache.game.arena == "bd" && _cache.game.zone == "SSCJ Devastation.")
	{
		if (CMPSTART("[Score", msg.c_str())) // parse out valid score
		{
			std::string winning_score = msg.substr(8, (msg.find_first_of("-") - 1) - 8);
			std::string losing_score = msg.substr(msg.find_first_of("-") + 2, (msg.find_last_of("-") - 1) - (msg.find_first_of("-") + 2));

			if ((atoi(winning_score.c_str()) >= 5) && (atoi(winning_score.c_str()) - atoi(losing_score.c_str()) >= 2)) // minimum 5b2 needed to win (traditionally)
			{
				_cache.game.game_score = std::make_pair(winning_score, losing_score); // save the scores for the stat table
				return true;
			}
		}
	}
	return false;
}

std::pair <std::string, std::string> botInfo::parseFreqBDStats(std::string raw)
{
	return std::make_pair(std::string(String(raw.substr(raw.find_first_of('|', 1) + 2,
		(raw.find_first_of('|', raw.find_first_of('|', 1) + 2) - 1) - (raw.find_first_of('|', 1) + 2)).c_str()).trim().msg),

		std::string(String(raw.substr(raw.find_first_of('|', raw.find_first_of('|', 1) + 2) + 2,
			raw.find_last_of('|') - (raw.find_first_of('|', raw.find_first_of('|', 1) + 2) + 2)).c_str()).trim().msg)); // returns freq's kills, deaths
}

void botInfo::parsePlayerBDStats(std::string raw)
{
	String pName = String(raw.substr(3, raw.find_first_of('|', 3) - 3).c_str()).trim().msg;

	_cache.game.player_bd_list.push_back(std::make_tuple(std::string(pName),

		((findPlayerByName(pName) != NULL) ? findPlayerByName(pName)->team : -1), // attempt to get their team by resolving their Player obj

		std::string(String(raw.substr(raw.find_first_of('|', 17) + 2,
			(raw.find_first_of('|', raw.find_first_of('|', 17) + 2) - 1) - (raw.find_first_of('|', 17) + 2)).c_str()).trim().msg),

		std::string(String(raw.substr(raw.find_first_of('|', raw.find_first_of('|', 17) + 2) + 2,
			raw.find_last_of('|') - (raw.find_first_of('|', raw.find_first_of('|', 17) + 2) + 2)).c_str()).trim().msg))); 
}

std::string botInfo::getBDPlayersAndScores(int team)
{
	std::string players_scores;
	if (_cache.game.player_bd_list.size() > 0)
	{
		for (int i = 0; i < _cache.game.player_bd_list.size(); i++)
			if (std::get<1>(_cache.game.player_bd_list[i]) == team)
				players_scores.append(std::get<0>(_cache.game.player_bd_list[i]) + "\n" + std::get<2>(_cache.game.player_bd_list[i]) + " - " + std::get<3>(_cache.game.player_bd_list[i]) + "\n");
	}
	return players_scores;
}
