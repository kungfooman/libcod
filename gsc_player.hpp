#ifndef _GSC_PLAYER_HPP_
#define _GSC_PLAYER_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

void gsc_player_velocity_set(int id);
void gsc_player_velocity_add(int id);
void gsc_player_velocity_get(int id);

void gsc_player_button_ads(int id);
void gsc_player_button_left(int id);
void gsc_player_button_right(int id);
void gsc_player_button_forward(int id);
void gsc_player_button_back(int id);
void gsc_player_button_leanleft(int id);
void gsc_player_button_leanright(int id);
void gsc_player_button_jump(int id);

int gsc_player_state_alive_set();

void gsc_player_stance_get(int id);

void gsc_player_spectatorclient_get(int id);

void gsc_player_getip(int id);
void gsc_player_getping(int id);

void gsc_player_ClientCommand(int id);

void gsc_player_getLastConnectTime(int id);
void gsc_player_getLastMSG(int id);

// entity functions
void gsc_entity_setalive(int id);
void gsc_entity_setbounds(int id);

#ifdef __cplusplus
}
#endif

#endif
