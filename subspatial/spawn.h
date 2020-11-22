/*
	MERVBot Spawn-callback by Catid@pacbell.net
*/

#ifndef SPAWN_H
#define SPAWN_H

#define STRING_CAST_CHAR

#define MAININI "discord\\config\\discord.ini"
#define LISTINI "discord\\data\\lists.dat"
#define ELITEINI "discord\\data\\elite.dat"
#define ECONINI "discord\\data\\economy.dat"

#define BOT_VER "2.0.3" // always keep 5 chars

#include "..\mervbot\dllcore.h"

#include "..\mervbot\clientprot.h"

#include <aegis.hpp>
#include <thread>
#include <curl/curl.h>

struct PlayerTag
{
	Player *p;
	int index;
	int data;
};
/////// DISCORD ///////
struct EliteCache
{
	std::vector <std::string> muted,                           // stores muted Continuum player names
		ignored;                                               // stores ignored names for online list
	std::vector <std::pair<aegis::snowflake, std::string>>	accounts; // stores elite tier players' usr flakes + Continuum names 
};

struct FlakesCache
{
	aegis::snowflake relayChannel, serverID, TWSpecChannel, eliteRoleID, mainChannelID, staffRoleID, updateChannel;
};

struct Discord
{
	aegis::core* bot;
	std::string token, relayWebhook, TWSpecWebhook, DevaMainDiscordWebhook, TWSpecMainDiscordWebhook, spamHook, bot_prefix, find;

	struct EliteCache elite;
	struct FlakesCache flakes;
	void updateBot(std::string version);
};

struct GameCache
{
	std::string arena, zone;
};

struct PlayerSession
{
	Player* usr;
	int kills, deaths, best_spree, best_bounty;
	bool anch;
};

struct EconomyCache
{
	struct PlayerSession session;
	std::vector <std::pair<int, std::string>> accounts; // stored as <scrap, DiscordID>
	float savings_rate, performance_rate;
};

// Mega dump of stored values read on boot
struct Cachedump 
{
	struct Discord discord;
	struct GameCache game;
	struct EconomyCache economy;

	void loadCache();
	void loadLists(int type);
};

/////// DISCORD ///////
#define MAX_OBJECTS 20

class botInfo
{
	bool CONNECTION_DENIED;

	_linkedlist <PlayerTag> taglist;
	_linkedlist <PlayerSession> sessionlist;

	int get_tag(Player *p, int index);
	void set_tag(Player *p, int index, int data);
	void killTags(Player *p);
	void killTags();

	CALL_HANDLE handle;
	CALL_COMMAND callback;
	CALL_PLIST playerlist;
	CALL_FLIST flaglist;
	CALL_MAP map;
	CALL_BLIST bricklist;
	char *arena;
	arenaSettings *settings;
	Player *me;
	bool biller_online;

	objectInfo object_array[MAX_OBJECTS];
	int num_objects;
	Player *object_dest;

	int countdown[6];

	// Put bot data here

public:
	
	class Economy 
	{
		
	public:
		struct EconomyCache cache;

		int getPlayerScrap(Player* p);
		float computePPP(Player* p);
		int getPerformanceStat(int type);
	};

	botInfo(CALL_HANDLE given)
	{
		handle = given;
		callback = 0;
		playerlist = 0;
		flaglist = 0;
		map = 0;
		countdown[0] = 8; // Discord connection
		countdown[1] = 30; // online list updater
		countdown[2] = 300; // priv_msg handle cleanup
		countdown[3] = 0;
		countdown[4] = 0;
		CONNECTION_DENIED = false;
		me = 0;
		biller_online = true;
		num_objects = 0;
		object_dest = NULL;

		// Put initial values here
	}

	/////// DISCORD ///////
	int randomizer(int range); 

	std::string getFreqPlayerNames(int team);
	std::string getINIString(int type);
	std::string getKnownAccount(std::string discordID);
	std::string getKnownAccount(char* pname);
	std::string cmdGetParam(std::string command);
	std::string getElitePairStrings();
	std::string createUserMentionString(std::string msg);
	std::string getMutedPlayers();

	std::vector <std::tuple<aegis::snowflake, aegis::snowflake, std::string>> priv_msg; // channel id, discord id, recipient string
	std::vector <std::tuple<std::string, std::string, bool>> cooldown; // cooldown stored as userID, command string, timer
	std::vector <std::string> hash;
	std::pair<int, int> countPlayers(int teams = 0); // returns total in zone, spec by default; freq 0/1 if > 0

	bool isLinked(String ssName);
	bool isLinked(std::string discordID);
	bool isStaff(aegis::snowflake usrID);
	bool DM2Discord(std::string out);
	bool onCooldown(std::string cmd, std::string usrID);
	bool CMPnSTART(const char* constant, const char* control);
	bool isIgnored(String pname);
	bool isMuted(String pname);
	bool cmdHasParam(std::string command);

	void clearDMhandles();
	void DM2Game(String recipient, String msg);
	void startCooldown(std::string cmd, std::string usrID, int timer);
	void grantDiscordElite(std::string discordID);
	void linkAccount(Player* p, String discordID);
	void unlinkAccount(std::string discordID);
	void updateOnlineList();
	void curlChatter(String name, String msg, int ship, std::string channelhook);
	void startBotProcess();
	void relayChat(std::string user, std::string msg, int zone);

	aegis::user* getUser(std::string usrname);
	aegis::user* getUser(char* usrname);
	Player* findPlayerByName(char* name);

	/////// END-DISCORD ///////

	void clear_objects();
	void object_target(Player *p);
	void toggle_objects();
	void queue_enable(int id);
	void queue_disable(int id);

	void gotEvent(BotEvent &event);

	void tell(BotEvent event);

	bool validate(CALL_HANDLE given) { return given == handle; }

	void sendPrivate(Player *player, char *msg);
	void sendPrivate(Player *player, BYTE snd, char *msg);

	void sendTeam(char *msg);
	void sendTeam(BYTE snd, char *msg);

	void sendTeamPrivate(Uint16 team, char *msg);
	void sendTeamPrivate(Uint16 team, BYTE snd, char *msg);

	void sendPublic(char *msg);
	void sendPublic(BYTE snd, char *msg);

	void sendPublicMacro(char *msg);
	void sendPublicMacro(BYTE snd, char *msg);

	void sendChannel(char *msg);			// #;Message
	void sendRemotePrivate(char *msg);		// :Name:Messsage
	void sendRemotePrivate(char *name, char *msg);

	void gotHelp(Player *p, Command *c);
	void gotCommand(Player *p, Command *c);
	void gotRemoteHelp(char *p, Command *c, Operator_Level l);
	void gotRemote(char *p, Command *c, Operator_Level l);
};


// The botlist contains every bot to ever be spawned of this type,
// some entries may no longer exist.

extern _linkedlist <botInfo> botlist;
/////// DISCORD ///////
extern botInfo *merv;
extern Cachedump _cache;
/////// END-DISCORD ///////

botInfo *findBot(CALL_HANDLE handle);

#endif	// SPAWN_H
