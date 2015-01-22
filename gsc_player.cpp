#include "gsc_player.hpp"

#if COMPILE_PLAYER == 1

#if COD_VERSION == COD2_1_0
	int playerStates = 0x086F1480; // search 'winner'
	int sizeOfPlayer = 0x28A4;
#elif COD_VERSION == COD2_1_2
	int playerStates = 0x08705480; // as in game initialisation "------- Game Initializati"
	int sizeOfPlayer = 0x28A4;
	// memset(&playerStates_8705480, 0, 0xA2900u);  
	// then we need a bit math: 0xA2900 / 64 = 0x28A4
#elif COD_VERSION == COD2_1_3
	// 8716558 pointed on that!! and that i found in setorigin() with ida decompiler
	// looked it up and it points to game_initialization_8109096()
	int playerStates = 0x087a2500;
	int sizeOfPlayer = 0x28A4;
#elif COD_VERSION == COD4_1_7
	/*
		memset((void *)0x841F260, 0, 0x9D000u);
		v836f6c4 = 138539616;
		v836f8a4 = *(_DWORD *)(v84bc268 + 12);
		memset((void *)0x84BC3A0, 0, 0xC6100u); // bottom = playerstates
	*/
	int playerStates = 0x084BC3A0;
	int sizeOfPlayer = 0x3184;
#elif COD_VERSION == COD4_1_7_L
	int playerStates = 0x084BD120;
	int sizeOfPlayer = 0x3184;
#else
	#warning int playerStates int sizeOfPlayer
	int playerStates = NULL;
	int sizeOfPlayer = NULL;
#endif

#if COD_VERSION == COD2_1_0
	int gentities = 0x08665480;
	int gentities_size = 560;
#elif COD_VERSION == COD2_1_2
	int gentities = 0x08679380;
	int gentities_size = 560;
#elif COD_VERSION == COD2_1_3
	int gentities = 0x08716400;
	int gentities_size = 560;
#elif COD_VERSION == COD4_1_7
	int gentities = 0x0841F260;
	int gentities_size = 628;
#elif COD_VERSION == COD4_1_7_L
	int gentities = 0x0841FFE0;
	int gentities_size = 628;
#else
	#warning int gentities int gentities_size
	int gentities = NULL;
	int gentities_size = NULL;
#endif

#define PLAYERSTATE(playerid) (playerStates + playerid * sizeOfPlayer)
#if COD_VERSION == COD2_1_0 || COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	#define PLAYERSTATE_VELOCITY(playerid) (PLAYERSTATE(playerid) + 0x20)
#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
	#define PLAYERSTATE_VELOCITY(playerid) (PLAYERSTATE(playerid) + 40)
#else
	#warning NO PLAYERSTATE_VELOCITY!
	#define PLAYERSTATE_VELOCITY(playerid) 0
#endif

#if COD_VERSION == COD2_1_0
	int playerinfo_base = 0x0841FB0C;
	int playerinfo_size = 0x78F14;
#elif COD_VERSION == COD2_1_2
	int playerinfo_base = 0x0842200C;
	int playerinfo_size = 0x79064;
#elif COD_VERSION == COD2_1_3
	int playerinfo_base = 0x0842308C;
	int playerinfo_size = 0xB1064;
#elif COD_VERSION == COD4_1_7
	int playerinfo_base = 0x090B420C;
	int playerinfo_size = 0x2958F;
#elif COD_VERSION == COD4_1_7_L
	int playerinfo_base = 0x090B4F8C;
	int playerinfo_size = 0x2958F;
#else
	#warning PLAYERBASE() got no working addresses
	int playerinfo_base = 0x0;
	int playerinfo_size = 0x0;
#endif

#if COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
	#define PLAYERBASE(playerid) (playerinfo_base + playerid * playerinfo_size)
#else
	#define PLAYERBASE(playerid) (*(int*)(playerinfo_base) + playerid * playerinfo_size)
#endif

void gsc_player_velocity_set(int id) {
	float velocity[3];

	if ( ! stackGetParams("v", &velocity)) {
		printf("scriptengine> wrongs args for gsc_player_velocity_add(vector velocity);\n");
		stackPushUndefined();
		return;
	}

	float *player_0_velocity_x = (float *)(PLAYERSTATE_VELOCITY(id) + 0);
	float *player_0_velocity_y = (float *)(PLAYERSTATE_VELOCITY(id) + 4);
	float *player_0_velocity_z = (float *)(PLAYERSTATE_VELOCITY(id) + 8);
	
	*player_0_velocity_x = velocity[0];
	*player_0_velocity_y = velocity[1];
	*player_0_velocity_z = velocity[2];
	
	stackReturnInt(1);
}

extern int rename_blocked[64];

void gsc_player_allow_rename(int id) {
	int allow;

	if ( ! stackGetParams("i", &allow)) {
		printf("scriptengine> wrongs args for gsc_player_allow_rename(bool);\n");
		stackPushUndefined();
		return;
	}
	if(allow)
		rename_blocked[id] = 0;
	else
		rename_blocked[id] = 1;
	stackReturnInt(1);
}

void gsc_player_clientuserinfochanged(int id)
{
	int val = *allow_clientuserchange; // store old value

	if(rename_blocked[id])
		*allow_clientuserchange = 1;
	else
		*allow_clientuserchange = val;

	int result = changeClientUserinfo(id);
	*allow_clientuserchange = val; // restore old value

	stackPushInt(result);
}

void gsc_player_velocity_add(int id) {
	float velocity[3];

	if ( ! stackGetParams("v", &velocity)) {
		printf("scriptengine> wrongs args for gsc_player_velocity_add(vector velocity);\n");
		stackPushUndefined();
		return;
	}

	float *player_0_velocity_x = (float *)(PLAYERSTATE_VELOCITY(id) + 0);
	float *player_0_velocity_y = (float *)(PLAYERSTATE_VELOCITY(id) + 4);
	float *player_0_velocity_z = (float *)(PLAYERSTATE_VELOCITY(id) + 8);

	*player_0_velocity_x += velocity[0];
	*player_0_velocity_y += velocity[1];
	*player_0_velocity_z += velocity[2];

	stackReturnInt(1);
}

void gsc_get_userinfo(int id)
{
	char* key;
	char* val;
	if(!stackGetParamString(0, &key))
	{
		printf("First param needs to be a string for get_userinfo\n");
		stackPushUndefined();
		return;
	}
	
	int entity = PLAYERBASE(id);
	val = Info_ValueForKey((char*)entity+12, key);
	
	if(strlen(val))
		stackPushString(val);
	else
		stackPushUndefined();
}

void gsc_set_userinfo(int id)
{
	char* key;
	char* value;
	if(!stackGetParamString(0, &key))
	{
		printf("First param needs to be a string for get_userinfo\n");
		stackPushUndefined();
		return;
	}
	if(!stackGetParamString(1, &value))
	{
		printf("second param needs to be a string for get_userinfo\n");
		stackPushUndefined();
		return;
	}
	
	int entity = PLAYERBASE(id);
	Info_SetValueForKey((char*)entity + 12, key, value);
	stackPushUndefined();
}

void gsc_player_velocity_get(int id) {
	//int currentPlayer = playerStates + id * sizeOfPlayer;
	float *vectorVelocity = (float *)PLAYERSTATE_VELOCITY(id); // (currentPlayer + 0x20);
	stackReturnVector(vectorVelocity);	
}

// aimButtonPressed (toggleads or +speed/-speed)
void gsc_player_button_ads(int id) {
	int currentPlayer = playerStates + id * sizeOfPlayer;
	unsigned char *aim_address = (unsigned char *)(currentPlayer + 0x26CD);
	int aimButtonPressed = *aim_address & 0xF0; // just the first 4 bits tell the state
	stackReturnInt(aimButtonPressed);
}

void gsc_player_button_left(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26FD);
	#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA7);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	int leftButtonPressed = (*aim_address & 0x81)==0x81;
	stackReturnInt(leftButtonPressed);
}

void gsc_player_button_right(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26FD);
	#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA7);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif

	int rightButtonPressed = (*aim_address & 0x7F)==0x7F;
	stackReturnInt(rightButtonPressed);
}

void gsc_player_button_forward(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26FC);
	#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA6);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif

	int forwardButtonPressed = (*aim_address & 0x7F)==0x7F;
	stackReturnInt(forwardButtonPressed);
}

void gsc_player_button_back(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26FC);
	#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA6);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	
	int backButtonPressed = (*aim_address & 0x81)==0x81;
	stackReturnInt(backButtonPressed);
}

void gsc_player_button_leanleft(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26E8);
	#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FB4);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif

	int leanleftButtonPressed = (*aim_address & 0x40)==0x40;
	stackReturnInt(leanleftButtonPressed);
}

void gsc_player_button_leanright(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26E8);
	#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FB4);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	
	int leanrightButtonPressed = (*aim_address & 0x80)==0x80;
	stackReturnInt(leanrightButtonPressed);
}

void gsc_player_button_jump(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26E9);
	#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FB5);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	
	int jumpButtonPressed = (*aim_address & 0x04)==0x04;
	stackReturnInt(jumpButtonPressed);
}

/*
CoD2 = 26E8 == leanleft:40 leanright:80
CoD4 = 26E9 == jump:04
CoD2 = 26FC == forward:7f backward:81
CoD2 = 26FD == left:81 right:7f

CoD4 = 2FA6 == forward:7f backward:81
CoD4 = 2FA7 == left:81 right:7f
CoD4 = 2FB4 == leanleft:40 leanright:80
CoD4 = 2FB5 == jump:04
*/

void gsc_player_stance_get(int id) {
	int entity = gentities + id * gentities_size;
	unsigned char *stance_address = (unsigned char *)(entity + 8);
	int code = *stance_address & 0x0F; // just the last 4 bits tell the state

	char *stance = "";
	switch (code) {
		case  0: stance = "stand"; break; // also in spec
		case  2: stance = "stand"; break;
		case  4: stance = "duck"; break;
		case  6: stance = "duck"; break;
		case  8: stance = "lie"; break;
		case 10: stance = "lie"; break;
		default: printf("unknown stance for player id=%d, code=%d\n", id, code);
	}

	stackPushString(stance);
}

void gsc_player_spectatorclient_get(int id) {
	int spectatorClient = *(unsigned char *)(PLAYERSTATE(id) + 0xCC);
	
	//printf("spectator client: %x=%d\n", entity, spectatorClient);
	
	// ups, its ALWAYS returning a real id
	// when i have 2 bots, then i got id 2, when i spec "myself" it will return 2, also when i play myself
	//if ( ! spectatorClient)
	//	return stackPushUndefined();
	
	stackPushEntity(gentities + spectatorClient * gentities_size);
}

void gsc_player_getip(int id) {
	#if COD_VERSION == COD2_1_0
		int info_ip_offset = 0x6E5C8;
	#elif COD_VERSION == COD2_1_2
		int info_ip_offset = 0x6E6D8;	
	#elif COD_VERSION == COD2_1_3
		int info_ip_offset = 0x6E6D8;	
	#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		int info_ip_offset = 0x9;	
	#else
		#warning gsc_player_getip() got no working addresses
		int info_ip_offset = 0x0;
	#endif
	
	char tmp[64];
	unsigned int ip_a, ip_b, ip_c, ip_d;
	
	#if COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
	char iphex[9];
	snprintf(iphex, 9, "%08x", ((int *)PLAYERBASE(id))[info_ip_offset]);
	sscanf(iphex, "%2x%2x%2x%2x", &ip_d, &ip_c, &ip_b, &ip_a);
	#else	
	int info_player = PLAYERBASE(id);
	ip_a = *(unsigned char *)(info_player + info_ip_offset + 0);
	ip_b = *(unsigned char *)(info_player + info_ip_offset + 1); // dafuq, its +1 but in IDA its +4 step :S
	ip_c = *(unsigned char *)(info_player + info_ip_offset + 2);
	ip_d = *(unsigned char *)(info_player + info_ip_offset + 3);
	//int port = *(unsigned char *)(info_player + info_ip_offset + 16);
	//snprintf(tmp, 64, "%d.%d.%d.%d:%d", ip_a, ip_b, ip_c, ip_d, port);
	#endif
	snprintf(tmp, 64, "%d.%d.%d.%d", ip_a, ip_b, ip_c, ip_d);
	
	stackPushString(tmp);
}

void gsc_player_getping(int id) {
	#if COD_VERSION == COD2_1_0
		int info_ping_offset = 0x6E5A4;
	#elif COD_VERSION == COD2_1_2
		int info_ping_offset = 0x6E6B4;
	#elif COD_VERSION == COD2_1_3
		int info_ping_offset = 0x6E6B4;
	#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		int info_ping_offset = 0x804EC;
	#else
		#warning gsc_player_getport() got no working addresses
		int info_ping_offset = 0x0;
	#endif
	
	int ping = *(unsigned int *)(PLAYERBASE(id) + info_ping_offset);
	stackPushInt(ping);
}

void gsc_player_ClientCommand(int id) {
	stackPushInt(ClientCommand(id));
}

int getSVSTime()
{
	#if COD_VERSION == COD2_1_0
		int info_start = *(int *)0x0841FB04;
	#elif COD_VERSION == COD2_1_2
		int info_start = *(int *)0x08422004;
	#elif COD_VERSION == COD2_1_3
		int info_start = *(int *)0x08423084;
	#else
		#warning getSVSTime() got no working addresses
		int info_start = *(int *)0x0;
	#endif
	return info_start;
}

void gsc_player_getLastConnectTime(int id) {
	#if COD_VERSION == COD2_1_0
		int info_connecttime_offset = 0x20D14;
	#elif COD_VERSION == COD2_1_2
		int info_connecttime_offset = 0x20E24;
	#elif COD_VERSION == COD2_1_3
		int info_connecttime_offset = 0x20E24;
	#else
		#warning gsc_player_getLastConnectTime() got no working addresses
		int info_connecttime_offset = 0x0;
	#endif

	int lastconnect = getSVSTime() - *(unsigned int *)(PLAYERBASE(id) + info_connecttime_offset);
	stackPushInt(lastconnect);
}

int getLastPacketTime(int id) {
	#if COD_VERSION == COD2_1_0
		int info_lastmsg_offset = 0x20D10;
	#elif COD_VERSION == COD2_1_2
		int info_lastmsg_offset = 0x20E20;
	#elif COD_VERSION == COD2_1_3
		int info_lastmsg_offset = 0x20E20;
	#else
		#warning getLastPacketTime() got no working addresses
		int info_lastmsg_offset = 0x0;
	#endif
	
	return (PLAYERBASE(id) + info_lastmsg_offset);
}

void gsc_player_getLastMSG(int id) {
	int lastmsg = getSVSTime() - *(unsigned int *)getLastPacketTime(id);
	stackPushInt(lastmsg);
}

void gsc_player_getclientstate(int id) {
	int info_player = PLAYERBASE(id);
	stackPushInt(*(int*)info_player);
}

int getAddressType(int id) {
	#if COD_VERSION == COD2_1_0
		int info_addresstype_offset = 0x6E5C4;
	#elif COD_VERSION == COD2_1_2
		int info_addresstype_offset = 0x6E6D4;
	#elif COD_VERSION == COD2_1_3
		int info_addresstype_offset = 0x6E6D4;
	#else
		#warning gsc_player_addresstype() got no working addresses
		int info_addresstype_offset = 0x0;
	#endif

	int addrtype = *(unsigned int *)(PLAYERBASE(id) + info_addresstype_offset);
}

void gsc_player_addresstype(int id) {
	stackPushInt(getAddressType(id));
}

void gsc_player_renamebot(int id) {
	char * key;

	if ( ! stackGetParams("s", &key)) {
		printf("scriptengine> ERROR: gsc_player_renamebot(): param \"key\"[1] has to be an string!\n");
		stackPushUndefined();
		return;
	}
	
	int info_player = PLAYERBASE(id);
	Info_SetValueForKey((char *)(info_player + 12), "name", key);
	char * name = (char *)(info_player + 134216);
	memcpy(&name[0], key, 32);
	name[31] = '\0';
	printf("name = %s\n", name);

	stackPushInt(1);
}

void gsc_player_outofbandprint(int id) {
	char* cmd; // print\ninsert test message here!!!\n

	if ( ! stackGetParams("s", &cmd)) {
		printf("scriptengine> ERROR: gsc_player_outofbandprint(): param \"cmd\"[1] has to be an string!\n");
		stackPushUndefined();
		return;
	}
	
	#if COD_VERSION == COD2_1_0
		int remoteaddress_offset = 452036;
	#else
		int remoteaddress_offset = 452308;
	#endif

	int info_player = PLAYERBASE(id);
	netadr_t * from = (netadr_t*)(info_player + remoteaddress_offset);
	NET_OutOfBandPrint(0, *from, cmd); // 0 = SERVER, 1 = CLIENT

	stackReturnInt(1);
}

void gsc_player_connectionlesspacket(int id) {
	char* cmd; // rcon pass status

	if ( ! stackGetParams("s", &cmd)) {
		printf("scriptengine> ERROR: gsc_player_connectionlesspacket(): param \"cmd\"[1] has to be an string!\n");
		stackPushUndefined();
		return;
	}

	char message[1024];
	message[0] = -1;
	message[1] = -1;
	message[2] = -1;
	message[3] = -1;
	message[4] = 0;
	strcat(message, cmd);
	msg_t msg;
	msg.data = message;
	msg.maxsize = 131072;
	msg.cursize = strlen(msg.data)+1;
	msg.readcount = 0;
	msg.overflowed = false;
	msg.bit = 0;
	
	#if COD_VERSION == COD2_1_0
		int remoteaddress_offset = 452036;
	#else
		int remoteaddress_offset = 452308;
	#endif

	int info_player = PLAYERBASE(id);
	netadr_t * from = (netadr_t*)(info_player + remoteaddress_offset);
	SV_ConnectionlessPacket(*from, &msg);

	stackReturnInt(1);
}

void gsc_player_resetNextReliableTime(int id)
{
	#if COD_VERSION == COD2_1_0
		int offset = 134412;
	#else
		int offset = 134684;
	#endif

	*(int *)(PLAYERBASE(id) + offset) = 0;
	stackPushInt(0);
}


// entity functions (could be in own file, but atm not many pure entity functions)

void gsc_entity_setalive(int id) { // as in isAlive?
	int isAlive;

	if ( ! stackGetParams("i", &isAlive)) {
		printf("scriptengine> ERROR: gsc_player_setalive(): param \"isAlive\"[1] has to be an integer!\n");
		stackPushUndefined();
		return;
	}
	
	*(char *)(gentities + gentities_size*id + 353) = isAlive;	
	stackReturnInt(1);
}

void gsc_entity_setbounds(int id) {
	float width, height;

	if ( ! stackGetParams("ff", &width, &height)) {
		printf("scriptengine> ERROR: please specify width and height to gsc_entity_setbounds()\n");
		stackPushUndefined();
		return;
	}

	*(float*)(gentities + gentities_size*id + 280) = height;
	*(float*)(gentities + gentities_size*id + 276) = width;
	*(float*)(gentities + gentities_size*id + 272) = width;
	*(float*)(gentities + gentities_size*id + 264) = -width;
	*(float*)(gentities + gentities_size*id + 260) = -width;
	
	printf("id=%d height=%f width=%f\n", id, height, width);
	stackReturnInt(1);
}

void gsc_free_slot()
{
	int id = 0;
	if(!stackGetParamInt(0, &id))
	{
		printf("Param 0 needs to be an int for free_slot\n");
		stackPushUndefined();
		return;
	}
	int entity = PLAYERBASE(id);
	*(int*)entity = 0; //CS_FREE
	stackPushUndefined();
}

void gsc_kick_slot()
{
	int id;
	char* msg;
	char* reason;

	if ( ! stackGetParams("is", &id, &msg)) {
		printf("scriptengine> ERROR: gsc_kick_slot(): param \"id\"[1] has to be an int!\n");
		printf("scriptengine> ERROR: gsc_kick_slot(): param \"msg\"[2] has to be an string!\n");
		stackPushUndefined();
		return;
	}
	
	if(getAddressType(id) == NA_LOOPBACK)
	{
		stackReturnInt(0);
		return; // host
	}
	
	#if COD_VERSION == COD2_1_0
		int guid_offset = 0x765F4;
	#elif COD_VERSION == COD2_1_2
		int guid_offset = 0x76704;
	#elif COD_VERSION == COD2_1_3
		int guid_offset = 0xAE704;
	#else
		#warning gsc_kick_slot() got no working addresses for guid_offset
		int guid_offset = 0x0;
	#endif
	
	int entity = PLAYERBASE(id);
	char* name = Info_ValueForKey((char*)entity+12, "name"); // read before drop client resets the userinfo
	int guid = *(int*)(entity + guid_offset);
	SV_DropClient(entity, msg);
	int * lastPacketTime = (int*)getLastPacketTime(id);
	*lastPacketTime = getSVSTime(); // in case there is a funny zombie (copied from Q3)
	
	if(!stackGetParamString(2, &reason))
	{
		#if COD_VERSION >= COD4_1_7
			Com_Printf(0, "%s (guid %i) was kicked\n", name, guid);
		#else
			Com_Printf("%s (guid %i) was kicked\n", name, guid);
		#endif
		stackReturnInt(1);
		return;
	}
	
	#if COD_VERSION >= COD4_1_7
		Com_Printf(0, "%s (guid %i) was kicked for %s\n", name, guid, reason);
	#else
		Com_Printf("%s (guid %i) was kicked for %s\n", name, guid, reason);
	#endif
	stackReturnInt(1);
}

#endif
