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
			int winning_score = atoi(msg.substr(8, (msg.find_first_of("-") - 1) - 8).c_str());
			int losing_score = atoi(msg.substr(msg.find_first_of("-") + 2, ((msg.find("Tied!") != msg.npos) ? (msg.find_last_of("-")) - (msg.find_first_of("-") + 2) 
				: (msg.find_last_of("-") - 1) - (msg.find_first_of("-") + 2))).c_str()); // formatting is different when tie game...
			
			if ((winning_score >= 5) && (winning_score - losing_score >= 2)) // minimum 5b2 needed to win (traditionally)
			{
				_cache.game.game_score = std::make_pair(std::to_string(winning_score), std::to_string(losing_score)); // save the scores for the stat table
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

	_cache.game.player_bd_list.push_back(std::make_tuple(std::string(pName), // store their name

		((findPlayerByName(pName) != NULL) ? findPlayerByName(pName)->team : -1), // attempt to get their team by resolving their Player obj

		std::string(String(raw.substr(raw.find_first_of('|', 17) + 2,
			(raw.find_first_of('|', raw.find_first_of('|', 17) + 2) - 1) - (raw.find_first_of('|', 17) + 2)).c_str()).trim().msg), // kills

		std::string(String(raw.substr(raw.find_first_of('|', raw.find_first_of('|', 17) + 2) + 2,
			raw.find_last_of('|') - (raw.find_first_of('|', raw.find_first_of('|', 17) + 2) + 2)).c_str()).trim().msg))); // deaths
}

std::string botInfo::getBDPlayersAndScores(int team)
{
	std::string players_scores;
	if (_cache.game.player_bd_list.size() > 0)
	{
		for (int i = 0; i < _cache.game.player_bd_list.size(); i++)
			if (std::get<1>(_cache.game.player_bd_list[i]) == team)
				players_scores.append((isLinked((String)std::get<0>(_cache.game.player_bd_list[i]).c_str()) ? std::get<0>(_cache.game.player_bd_list[i]) + " \360\237\217\206\n" : std::get<0>(_cache.game.player_bd_list[i]) + "\n") 
					+ std::get<2>(_cache.game.player_bd_list[i]) + " - " + std::get<3>(_cache.game.player_bd_list[i]) + "\n");
	}
	return players_scores;
}
