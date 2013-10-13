#include "gsc_player.hpp"

#if COMPILE_PLAYER == 1

#if COD2_VERSION == COD2_VERSION_1_2
	int playerStates = 0x08705480; // as in game initialisation "------- Game Initializati"
	int sizeOfPlayer = 0x28A4;
	// memset(&playerStates_8705480, 0, 0xA2900u);  
	// then we need a bit math: 0xA2900 / 64 = 0x28A4
#elif COD2_VERSION == COD2_VERSION_1_3
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
#else
	#warning int playerStates int sizeOfPlayer
	int playerStates = NULL;
	int sizeOfPlayer = NULL;
#endif

#if COD2_VERSION == COD2_VERSION_1_2
	int gentities = 0x08679380;
	int gentities_size = 560;
#elif COD2_VERSION == COD2_VERSION_1_3
	int gentities = 0x08716400;
	int gentities_size = 560;
#else
	#warning int gentities int gentities_size
	int gentities = NULL;
	int gentities_size = NULL;
#endif

#define PLAYERSTATE(playerid) (playerStates + playerid * sizeOfPlayer)
#if COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
	#define PLAYERSTATE_VELOCITY(playerid) (PLAYERSTATE(playerid) + 0x20)
#elif COD_VERSION == COD4_1_7
	#define PLAYERSTATE_VELOCITY(playerid) (PLAYERSTATE(playerid) + 40)
#else
	#warning NO PLAYERSTATE_VELOCITY!
	#define PLAYERSTATE_VELOCITY(playerid) 0
#endif

int gsc_player_velocity_set()
{
	int playerid;
	float velocity_x, velocity_y, velocity_z;
	
	if (stackGetNumberOfParams() < 5) // function, playerid, x, y, z
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamFloat(2, &velocity_x))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"velocity_x\"[2] has to be an float!\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamFloat(3, &velocity_y))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"velocity_y\"[3] has to be an float!\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamFloat(4, &velocity_z))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"velocity_z\"[4] has to be an float!\n");
		return stackReturnInt(0);
	}
	//int currentPlayer = playerStates + playerid * sizeOfPlayer;

	float *player_0_velocity_x = (float *)(PLAYERSTATE_VELOCITY(playerid) + 0);
	float *player_0_velocity_y = (float *)(PLAYERSTATE_VELOCITY(playerid) + 4);
	float *player_0_velocity_z = (float *)(PLAYERSTATE_VELOCITY(playerid) + 8);
	
	*player_0_velocity_x = velocity_x;
	*player_0_velocity_y = velocity_y;
	*player_0_velocity_z = velocity_z;
	
	// todo: let it fail if no player...
	return stackReturnInt(1);
}


int gsc_player_velocity_add()
{
	int playerid;
	float velocity_x, velocity_y, velocity_z;
	
	if (stackGetNumberOfParams() < 5) // function, playerid, x, y, z
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamFloat(2, &velocity_x))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"velocity_x\"[2] has to be an float!\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamFloat(3, &velocity_y))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"velocity_y\"[3] has to be an float!\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamFloat(4, &velocity_z))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"velocity_z\"[4] has to be an float!\n");
		return stackReturnInt(0);
	}

	float *player_0_velocity_x = (float *)(PLAYERSTATE_VELOCITY(playerid) + 0);
	float *player_0_velocity_y = (float *)(PLAYERSTATE_VELOCITY(playerid) + 4);
	float *player_0_velocity_z = (float *)(PLAYERSTATE_VELOCITY(playerid) + 8);

	//int currentPlayer = playerStates + playerid * sizeOfPlayer;
	//float *player_0_velocity_x = (float *)(currentPlayer + 40);
	//float *player_0_velocity_y = (float *)(currentPlayer + 44);
	//float *player_0_velocity_z = (float *)(currentPlayer + 48);
	//printf("z:%.2f\n", *player_0_velocity_z);
	
	*player_0_velocity_x += velocity_x;
	*player_0_velocity_y += velocity_y;
	*player_0_velocity_z += velocity_z;
	
	// todo: let it fail if no player...
	return stackReturnInt(1);
}

int gsc_player_velocity_get()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}
	//int currentPlayer = playerStates + playerid * sizeOfPlayer;

	float *vectorVelocity = (float *)PLAYERSTATE_VELOCITY(playerid); // (currentPlayer + 0x20);
	return stackReturnVector(vectorVelocity);
	
	// todo: return undefined if no player
	// return stackReturnInt(1);
}

// aimButtonPressed (toggleads or +speed/-speed)
int gsc_player_button_ads()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}
	int currentPlayer = playerStates + playerid * sizeOfPlayer;

	unsigned char *aim_address = (unsigned char *)(currentPlayer + 0x26CD);
	int aimButtonPressed = *aim_address & 0xF0; // just the first 4 bits tell the state
	return stackReturnInt(aimButtonPressed);
	// todo: return undefined if no player
	// return stackReturnInt(1);
}

int gsc_player_button_left()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}

	#if COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x26FD);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x2FA7);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	int leftButtonPressed = (*aim_address & 0x81)==0x81;
	return stackReturnInt(leftButtonPressed);
}
int gsc_player_button_right()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}

	#if COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x26FD);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x2FA7);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif

	int rightButtonPressed = (*aim_address & 0x7F)==0x7F;
	return stackReturnInt(rightButtonPressed);
}
int gsc_player_button_forward()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}
	
	#if COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x26FC);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x2FA6);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif

	int forwardButtonPressed = (*aim_address & 0x7F)==0x7F;
	return stackReturnInt(forwardButtonPressed);
}
int gsc_player_button_back()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}
	
	#if COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x26FC);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x2FA6);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	
	int backButtonPressed = (*aim_address & 0x81)==0x81;
	return stackReturnInt(backButtonPressed);
}
int gsc_player_button_leanleft()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}

	#if COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x26E8);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x2FB4);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	
	
	int leanleftButtonPressed = (*aim_address & 0x40)==0x40;
	return stackReturnInt(leanleftButtonPressed);
}
int gsc_player_button_leanright()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}
	
	#if COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x26E8);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x2FB4);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	
	int leanrightButtonPressed = (*aim_address & 0x80)==0x80;
	return stackReturnInt(leanrightButtonPressed);
}
int gsc_player_button_jump()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}
	
	#if COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x26E9);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(playerid) + 0x2FB5);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	
	int jumpButtonPressed = (*aim_address & 0x04)==0x04;
	return stackReturnInt(jumpButtonPressed);
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

int gsc_player_state_alive_set() // as in isAlive?
{
	int playerid;
	int isAlive;
	if (stackGetNumberOfParams() < 3) // function, playerid, isAlive
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(2, &isAlive))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"isAlive\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}
	
	
	*(char *)(gentities + gentities_size*playerid + 353) = isAlive;

	
	return stackReturnInt(0);
}

int gsc_player_stance_get()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackReturnInt(0);
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackReturnInt(0);
	}
	int entity = gentities + playerid * gentities_size;

	unsigned char *stance_address = (unsigned char *)(entity + 8);
	int stance = *stance_address & 0x0F; // just the last 4 bits tell the state
	return stackReturnInt(stance);
	// todo: return undefined if no player
	// return stackReturnInt(1);
}

int gsc_player_spectatorclient_get()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 5 arguments to closer()\n");
		return stackPushUndefined();
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackPushUndefined();
	}
	int entity = playerStates + playerid * sizeOfPlayer;

	int spectatorClient = *(unsigned char *)(entity + 0xCC);
	
	//printf("spectator client: %x=%d\n", entity, spectatorClient);
	
	// ups, its ALWAYS returning a real id
	// when i have 2 bots, then i got id 2, when i spec "myself" it will return 2, also when i play myself
	//if ( ! spectatorClient)
	//	return stackPushUndefined();
	
	return stackPushEntity(gentities + spectatorClient * gentities_size);
}




int gsc_player_getip()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 2 arguments to gsc_player_getip()\n");
		return stackPushUndefined();
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackPushUndefined();
	}
	
	#if COD2_VERSION == COD2_VERSION_1_3
		int info_base = *(int *)0x0842308C;
		int info_size = 0xB1064;
		int info_ip_offset = 0x6E6D8;
		int info_port_offset = 0x6E6B4;
	#elif COD2_VERSION == COD2_VERSION_1_2
		int info_base = *(int *)0x0842200C;
		int info_size = 0x79064;
		int info_ip_offset = 0x6E6D8;
		int info_port_offset = 0x6E6B4;
	#else
		#warning gsc_player_getip() got no working addresses
		int info_base = *(int *)0x0;
		int info_size = 0x0;
		int info_ip_offset = 0x0;
		int info_port_offset = 0x0;
	#endif
	
	int info_player = info_base + playerid * info_size;

	int ip_a = *(unsigned char *)(info_player + info_ip_offset + 0);
	int ip_b = *(unsigned char *)(info_player + info_ip_offset + 1); // dafuq, its +1 but in IDA its +4 step :S
	int ip_c = *(unsigned char *)(info_player + info_ip_offset + 2);
	int ip_d = *(unsigned char *)(info_player + info_ip_offset + 3);
	//int port = *(unsigned char *)(info_player + info_ip_offset + 16);

	char tmp[64];
	snprintf(tmp, 64, "%d.%d.%d.%d", ip_a, ip_b, ip_c, ip_d);
	//snprintf(tmp, 64, "%d.%d.%d.%d:%d", ip_a, ip_b, ip_c, ip_d, port);
	
	return stackPushString(tmp);
}


int gsc_player_getping()
{
	int playerid;
	if (stackGetNumberOfParams() < 2) // function, playerid
	{
		printf_hide("scriptengine> ERROR: please specify atleast 2 arguments to gsc_player_getip()\n");
		return stackPushUndefined();
	}
	if (!stackGetParamInt(1, &playerid))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"playerid\"[1] has to be an integer!\n");
		return stackPushUndefined();
	}
	
	#if COD2_VERSION == COD2_VERSION_1_3
		int info_base = *(int *)0x0842308C;
		int info_size = 0xB1064;
		int info_ip_offset = 0x6E6D8;
		int info_port_offset = 0x6E6B4;
	#elif COD2_VERSION == COD2_VERSION_1_2
		int info_base = *(int *)0x0842200C;
		int info_size = 0x79064;
		int info_ip_offset = 0x6E6D8;
		int info_port_offset = 0x6E6B4;
	#else
		#warning gsc_player_getip() got no working addresses
		int info_base = *(int *)0x0;
		int info_size = 0x0;
		int info_ip_offset = 0x0;
		int info_port_offset = 0x0;
	#endif
	
	int info_player = info_base + playerid * info_size;
	int ping = *(unsigned int *)(info_player + info_port_offset);
	return stackPushInt(ping);
}
#endif