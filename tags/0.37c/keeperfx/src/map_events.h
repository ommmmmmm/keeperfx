/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file map_events.h
 *     Header file for map_events.c.
 * @par Purpose:
 *     Map events support functions.
 * @par Comment:
 *     Just a header file - #defines, typedefs, function prototypes etc.
 * @author   Tomasz Lis
 * @date     11 Mar 2010 - 12 May 2010
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#ifndef DK_MAP_EVENTS_H
#define DK_MAP_EVENTS_H

#include "globals.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
#define EVENT_BUTTONS_COUNT    12
#define EVENT_KIND_COUNT       27
#define EVENTS_COUNT          100
/******************************************************************************/
#ifdef __cplusplus
#pragma pack(1)
#endif
/******************************************************************************/
extern struct EventTypeInfo event_button_info[28];
/******************************************************************************/
DLLIMPORT struct EventTypeInfo _DK_event_button_info[27];
/******************************************************************************/
#ifdef __cplusplus
#pragma pack()
#endif
/******************************************************************************/
long event_create_event_or_update_nearby_existing_event(MapCoord map_x, MapCoord map_y, unsigned char a3, unsigned char dngn_id, long msg_id);
void event_initialise_all(void);
long event_move_player_towards_event(struct PlayerInfo *player, long var);
struct Event *event_create_event(long map_x, long map_y, unsigned char a3, unsigned char dngn_id, long msg_id);
struct Event *event_allocate_free_event_structure(void);
void event_initialise_event(struct Event *event, long map_x, long map_y, unsigned char evkind, unsigned char dngn_id, long msg_id);
void event_add_to_event_list(struct Event *event, struct Dungeon *dungeon);
void event_delete_event(long plridx, long num);
void go_on_then_activate_the_event_box(long plridx, long evidx);

/******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif