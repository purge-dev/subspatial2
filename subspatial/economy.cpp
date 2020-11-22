#include "spawn.h"

/* economy.cpp ~
*  A metagame/social experiment of mine that enables users to invest in players according to various performance metrics.
*  Investing is done in Discord only by Elite tier players.
*  Bank's starting balance is X scrap. Players must !bank to open their accounts and earn scrap paid by bank based on player events.
* 
* Player Performance Profile (PPP) (!pp/ppp):
* PPP = (session_kills + (session_spree/(anch_ship) ? 5 : 2) + session_highest_bounty/100*(anch_ship) ? 5 : 2))/deaths ^ (total kills/deaths)/10
* 
* Player sessions are saved to disk after player specs/leaves arena. Autosaves every 5min.
*/ 

int botInfo::Economy::getPlayerScrap(Player* p)
{
	for (int i = 0; i < cache.accounts.size(); i++)
		if (strcmp(cache.accounts[i].second.c_str(), merv->getKnownAccount(p->name).c_str()) == 0)
			return cache.accounts[i].first;

	return -1;
}


