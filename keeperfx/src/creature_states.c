/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file creature_states.c
 *     Creature states structure and function definitions.
 * @par Purpose:
 *     Defines elements of states[] array, containing valid creature states.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     23 Sep 2009 - 22 Feb 2010
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include "creature_states.h"
#include "globals.h"

#include "bflib_math.h"
#include "thing_list.h"
#include "creature_control.h"
#include "creature_instances.h"
#include "creature_graphics.h"
#include "config_creature.h"
#include "config_rules.h"
#include "config_terrain.h"
#include "config_crtrstates.h"
#include "thing_stats.h"
#include "thing_physics.h"
#include "thing_objects.h"
#include "thing_effects.h"
#include "thing_navigate.h"
#include "room_data.h"
#include "room_jobs.h"
#include "room_workshop.h"
#include "room_library.h"
#include "tasks_list.h"
#include "map_events.h"
#include "map_blocks.h"
#include "power_hand.h"
#include "gui_topmsg.h"
#include "gui_soundmsgs.h"
#include "engine_arrays.h"
#include "player_utils.h"
#include "lvl_script.h"
#include "thing_traps.h"
#include "sounds.h"
#include "game_legacy.h"

#include "creature_states_gardn.h"
#include "creature_states_hero.h"
#include "creature_states_lair.h"
#include "creature_states_mood.h"
#include "creature_states_prisn.h"
#include "creature_states_rsrch.h"
#include "creature_states_scavn.h"
#include "creature_states_spdig.h"
#include "creature_states_tortr.h"
#include "creature_states_train.h"
#include "creature_states_wrshp.h"
#include "creature_states_combt.h"
#include "creature_states_guard.h"
#include "creature_states_pray.h"
#include "creature_states_tresr.h"
#include "creature_states_barck.h"
#include "creature_states_combt.h"

#include "keeperfx.hpp"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
#define CREATURE_GUI_STATES_COUNT 3
/* Please note that functions returning 'short' are not ment to return true/false only! */
/******************************************************************************/
DLLIMPORT short _DK_already_at_call_to_arms(struct Thing *creatng);
DLLIMPORT short _DK_arrive_at_alarm(struct Thing *creatng);
DLLIMPORT short _DK_arrive_at_call_to_arms(struct Thing *creatng);
DLLIMPORT short _DK_cleanup_hold_audience(struct Thing *creatng);
DLLIMPORT short _DK_creature_being_dropped(struct Thing *creatng);
DLLIMPORT short _DK_creature_cannot_find_anything_to_do(struct Thing *creatng);
DLLIMPORT short _DK_creature_change_from_chicken(struct Thing *creatng);
DLLIMPORT short _DK_creature_change_to_chicken(struct Thing *creatng);
DLLIMPORT short _DK_creature_doing_nothing(struct Thing *creatng);
DLLIMPORT short _DK_creature_dormant(struct Thing *creatng);
DLLIMPORT short _DK_creature_escaping_death(struct Thing *creatng);
DLLIMPORT short _DK_creature_evacuate_room(struct Thing *creatng);
DLLIMPORT short _DK_creature_explore_dungeon(struct Thing *creatng);
DLLIMPORT short _DK_creature_fired(struct Thing *creatng);
DLLIMPORT short _DK_creature_follow_leader(struct Thing *creatng);
DLLIMPORT short _DK_creature_in_hold_audience(struct Thing *creatng);
DLLIMPORT short _DK_creature_kill_creatures(struct Thing *creatng);
DLLIMPORT short _DK_creature_leaves(struct Thing *creatng);
DLLIMPORT short _DK_creature_leaves_or_dies(struct Thing *creatng);
DLLIMPORT short _DK_creature_leaving_dungeon(struct Thing *creatng);
DLLIMPORT short _DK_creature_persuade(struct Thing *creatng);
DLLIMPORT short _DK_creature_present_to_dungeon_heart(struct Thing *creatng);
DLLIMPORT short _DK_creature_pretend_chicken_move(struct Thing *creatng);
DLLIMPORT short _DK_creature_pretend_chicken_setup_move(struct Thing *creatng);
DLLIMPORT short _DK_creature_search_for_gold_to_steal_in_room(struct Thing *creatng);
DLLIMPORT short _DK_creature_set_work_room_based_on_position(struct Thing *creatng);
DLLIMPORT short _DK_creature_slap_cowers(struct Thing *creatng);
DLLIMPORT short _DK_creature_steal_gold(struct Thing *creatng);
DLLIMPORT short _DK_creature_take_salary(struct Thing *creatng);
DLLIMPORT short _DK_creature_unconscious(struct Thing *creatng);
DLLIMPORT short _DK_creature_vandalise_rooms(struct Thing *creatng);
DLLIMPORT short _DK_creature_wait_at_treasure_room_door(struct Thing *creatng);
DLLIMPORT short _DK_creature_wants_a_home(struct Thing *creatng);
DLLIMPORT short _DK_creature_wants_salary(struct Thing *creatng);
DLLIMPORT short _DK_move_backwards_to_position(struct Thing *creatng);
DLLIMPORT long _DK_move_check_attack_any_door(struct Thing *creatng);
DLLIMPORT long _DK_move_check_can_damage_wall(struct Thing *creatng);
DLLIMPORT long _DK_move_check_kill_creatures(struct Thing *creatng);
DLLIMPORT long _DK_move_check_near_dungeon_heart(struct Thing *creatng);
DLLIMPORT long _DK_move_check_on_head_for_room(struct Thing *creatng);
DLLIMPORT long _DK_move_check_persuade(struct Thing *creatng);
DLLIMPORT long _DK_move_check_wait_at_door_for_wage(struct Thing *creatng);
DLLIMPORT short _DK_move_to_position(struct Thing *creatng);
DLLIMPORT char _DK_new_slab_tunneller_check_for_breaches(struct Thing *creatng);
DLLIMPORT short _DK_patrol_here(struct Thing *creatng);
DLLIMPORT short _DK_patrolling(struct Thing *creatng);
DLLIMPORT short _DK_person_sulk_at_lair(struct Thing *creatng);
DLLIMPORT short _DK_person_sulk_head_for_lair(struct Thing *creatng);
DLLIMPORT short _DK_person_sulking(struct Thing *creatng);
DLLIMPORT short _DK_seek_the_enemy(struct Thing *creatng);
DLLIMPORT short _DK_state_cleanup_dragging_body(struct Thing *creatng);
DLLIMPORT short _DK_state_cleanup_dragging_object(struct Thing *creatng);
DLLIMPORT short _DK_state_cleanup_in_room(struct Thing *creatng);
DLLIMPORT short _DK_state_cleanup_unable_to_fight(struct Thing *creatng);
DLLIMPORT short _DK_state_cleanup_unconscious(struct Thing *creatng);
DLLIMPORT long _DK_setup_random_head_for_room(struct Thing *creatng, struct Room *room, unsigned char a3);
DLLIMPORT void _DK_anger_set_creature_anger(struct Thing *creatng, long a1, long a2);
DLLIMPORT void _DK_create_effect_around_thing(struct Thing *creatng, long eff_kind);
DLLIMPORT void _DK_remove_health_from_thing_and_display_health(struct Thing *creatng, long delta);
DLLIMPORT long _DK_slab_by_players_land(unsigned char plyr_idx, unsigned char slb_x, unsigned char slb_y);
DLLIMPORT struct Room *_DK_find_nearest_room_for_thing_excluding_two_types(struct Thing *creatng, char owner, char a3, char a4, unsigned char a5);
DLLIMPORT unsigned char _DK_initialise_thing_state(struct Thing *creatng, long a2);
DLLIMPORT long _DK_cleanup_current_thing_state(struct Thing *creatng);
DLLIMPORT unsigned char _DK_find_random_valid_position_for_thing_in_room_avoiding_object(struct Thing *creatng, struct Room *room, struct Coord3d *pos);
DLLIMPORT long _DK_setup_head_for_empty_treasure_space(struct Thing *creatng, struct Room *room);
DLLIMPORT short _DK_creature_choose_random_destination_on_valid_adjacent_slab(struct Thing *creatng);
DLLIMPORT long _DK_person_get_somewhere_adjacent_in_room(struct Thing *creatng, struct Room *room, struct Coord3d *pos);
DLLIMPORT unsigned char _DK_external_set_thing_state(struct Thing *creatng, long state);
DLLIMPORT void _DK_process_person_moods_and_needs(struct Thing *creatng);
DLLIMPORT long _DK_get_best_position_outside_room(struct Thing *creatng, struct Coord3d *pos, struct Room *room);
DLLIMPORT struct Room * _DK_find_nearest_room_for_thing(struct Thing *thing, char a2, char a3, unsigned char a4);
/******************************************************************************/
short already_at_call_to_arms(struct Thing *creatng);
short arrive_at_alarm(struct Thing *creatng);
short arrive_at_call_to_arms(struct Thing *creatng);
short cleanup_hold_audience(struct Thing *creatng);
short creature_being_dropped(struct Thing *creatng);
short creature_cannot_find_anything_to_do(struct Thing *creatng);
short creature_change_from_chicken(struct Thing *creatng);
short creature_change_to_chicken(struct Thing *creatng);
short creature_doing_nothing(struct Thing *creatng);
short creature_dormant(struct Thing *creatng);
short creature_escaping_death(struct Thing *creatng);
short creature_evacuate_room(struct Thing *creatng);
short creature_explore_dungeon(struct Thing *creatng);
short creature_fired(struct Thing *creatng);
short creature_follow_leader(struct Thing *creatng);
short creature_in_hold_audience(struct Thing *creatng);
short creature_kill_creatures(struct Thing *creatng);
short creature_leaves(struct Thing *creatng);
short creature_leaves_or_dies(struct Thing *creatng);
short creature_leaving_dungeon(struct Thing *creatng);
short creature_persuade(struct Thing *creatng);
short creature_present_to_dungeon_heart(struct Thing *creatng);
short creature_pretend_chicken_move(struct Thing *creatng);
short creature_pretend_chicken_setup_move(struct Thing *creatng);
short creature_search_for_gold_to_steal_in_room(struct Thing *creatng);
short creature_set_work_room_based_on_position(struct Thing *creatng);
short creature_slap_cowers(struct Thing *creatng);
short creature_steal_gold(struct Thing *creatng);
short creature_take_salary(struct Thing *creatng);
short creature_unconscious(struct Thing *creatng);
short creature_vandalise_rooms(struct Thing *creatng);
short creature_wait_at_treasure_room_door(struct Thing *creatng);
short creature_wants_a_home(struct Thing *creatng);
short creature_wants_salary(struct Thing *creatng);
short move_backwards_to_position(struct Thing *creatng);
CrCheckRet move_check_attack_any_door(struct Thing *creatng);
CrCheckRet move_check_can_damage_wall(struct Thing *creatng);
CrCheckRet move_check_kill_creatures(struct Thing *creatng);
CrCheckRet move_check_near_dungeon_heart(struct Thing *creatng);
CrCheckRet move_check_on_head_for_room(struct Thing *creatng);
CrCheckRet move_check_persuade(struct Thing *creatng);
CrCheckRet move_check_wait_at_door_for_wage(struct Thing *creatng);
short move_to_position(struct Thing *creatng);
char new_slab_tunneller_check_for_breaches(struct Thing *creatng);
short patrol_here(struct Thing *creatng);
short patrolling(struct Thing *creatng);
short person_sulk_at_lair(struct Thing *creatng);
short person_sulk_head_for_lair(struct Thing *creatng);
short person_sulking(struct Thing *creatng);
short seek_the_enemy(struct Thing *creatng);
short state_cleanup_dragging_body(struct Thing *creatng);
short state_cleanup_dragging_object(struct Thing *creatng);
short state_cleanup_in_room(struct Thing *creatng);
short state_cleanup_unable_to_fight(struct Thing *creatng);
short state_cleanup_unconscious(struct Thing *creatng);
short creature_search_for_spell_to_steal_in_room(struct Thing *creatng);
short creature_pick_up_spell_to_steal(struct Thing *creatng);

/******************************************************************************/
#ifdef __cplusplus
}
#endif
/******************************************************************************/
struct StateInfo states[] = {
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {imp_doing_nothing, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {imp_arrives_at_dig_or_mine, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {imp_arrives_at_dig_or_mine, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {imp_digs_mines, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {imp_digs_mines, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 1, 0, 0, 0, 0, 1},
  {imp_drops_gold, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {imp_last_did_job, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
  {imp_arrives_at_improve_dungeon, NULL, NULL,  NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {imp_improves_dungeon, NULL, NULL, NULL, // [10]
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {creature_picks_up_trap_object, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {creature_arms_trap, state_cleanup_dragging_object, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {creature_picks_up_trap_for_workshop, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {move_to_position, NULL, NULL, NULL,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 1, 0, 0, 0, 0, 1},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0,  0, 1, 0, 0, 0, 0, 1},
  {creature_drops_crate_in_workshop, state_cleanup_dragging_object, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,  0, 0, 1},
  {creature_doing_nothing, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1},
  {creature_to_garden, NULL, NULL, NULL,
    0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 0, 0, 1, 0, 59, 1, 0,  1},
  {creature_arrived_at_garden, state_cleanup_in_room, NULL, move_check_on_head_for_room,
    0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 0, 0, 2, 0, 59, 1, 0,  1},
  {creature_wants_a_home, NULL, NULL, NULL, // [20]
    0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 58, 1, 0,  1},
  {creature_choose_room_for_lair_site, NULL, NULL, NULL,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0,  1, 0, 58, 1, 0, 1},
  {creature_at_new_lair, NULL, NULL, NULL,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 58, 1, 0,  1},
  {person_sulk_head_for_lair, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0,   55, 1, 0, 1},
  {person_sulk_at_lair, NULL, NULL, NULL,
    0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 55, 1, 0,  1},
  {creature_going_home_to_sleep, NULL, NULL, NULL,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 0, 0, 1, 0,  54, 1, 0, 1},
  {creature_sleep, cleanup_sleep, NULL, NULL,
    0, 1, 0, 0, 0,  0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 0, 0, 2, 0, 54, 1, 0, 1},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,  0, 1, 0, 0, 0, 0, 1},
  {tunnelling, NULL, new_slab_tunneller_check_for_breaches, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0,  0, 0, 0, 1},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 0, 0, 0, 0, 1},
  {at_research_room, NULL, NULL, move_check_on_head_for_room, // [30]
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 56, 1, 0,   1},
  {researching, state_cleanup_in_room, NULL, process_research_function,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 56, 1, 0,   1},
  {at_training_room, NULL, NULL, move_check_on_head_for_room,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 57, 1, 0,   1},
  {training, state_cleanup_in_room, NULL, NULL,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 57, 1, 0,   1},
  {good_doing_nothing, NULL, NULL, NULL,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
  {good_returns_to_start, NULL, NULL, NULL,
    0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,  1},
  {good_back_at_start, NULL, NULL, NULL,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
  {good_drops_gold, NULL, NULL, NULL,
    0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
  {NULL, NULL, NULL, NULL,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,  1, 1, 0, 0, 0, 0, 0},
  {arrive_at_call_to_arms, NULL, NULL, NULL,
    1,  0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 1, 0, 62,   1, 0, 0},
  {creature_arrived_at_prison, state_cleanup_unable_to_fight, NULL, move_check_on_head_for_room, // [40]
    1, 0,   1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 1, 0, 0, 0, 66, 1, 0,  0},
  {creature_in_prison, cleanup_prison, NULL, process_prison_function,
    1, 0, 1,   1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 1, 0, 0, 0, 66, 1, 0, 0},
  {at_torture_room, state_cleanup_unable_to_fight, NULL, move_check_on_head_for_room,
    1, 0,   1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 1, 0, 0, 0, 65, 1, 0,  0},
  {torturing, cleanup_torturing, NULL, process_torture_function,
    1, 0, 1,   1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 1, 0, 0, 0, 65, 1, 0, 0},
  {at_workshop_room, NULL, NULL, move_check_on_head_for_room,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 67, 1, 0,   1},
  {manufacturing, state_cleanup_in_room, NULL, NULL,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 67, 1, 0,   1},
  {at_scavenger_room, NULL, NULL, move_check_on_head_for_room,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 69, 1, 0,   1},
  {scavengering, state_cleanup_in_room, NULL, process_scavenge_function,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 69, 1, 0,   1},
  {creature_dormant, NULL, NULL, move_check_near_dungeon_heart,
    1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0,  0, 1},
  {creature_in_combat, cleanup_combat, NULL, NULL,
    1, 1, 1, 0,   1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 5, 0, 0, 1, 1, 51, 1, 0, 0},
  {creature_leaving_dungeon, NULL, NULL, NULL, // [50]
    0,  1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 1, 1, 1, 0, 61,   1, 0, 1},
  {creature_leaves, NULL, NULL, NULL,
    0, 1, 1, 0,  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 1, 1, 1, 0, 61, 1, 0, 1},
  {creature_in_hold_audience, cleanup_hold_audience, NULL, NULL,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
  {patrol_here, NULL, NULL, NULL,
    0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
  {patrolling, NULL, NULL, NULL,
    0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,  0, 0, 0, 0, 0, 0, 1},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 1},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 0, 0, 0, 0, 0},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 0, 0, 0, 0, 1},
  {creature_kill_creatures, NULL, NULL, move_check_kill_creatures,
    0, 1, 1,  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 1, 0, 61, 1, 0,   1},
  {NULL, NULL, NULL, NULL, // [60]
    1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0,  0, 0, 0, 0, 0, 0, 1},
  {person_sulking, NULL, NULL, NULL,
    0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 55, 1, 0, 1},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {at_barrack_room, NULL, NULL, move_check_on_head_for_room,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 63, 1, 0,   1},
  {barracking, state_cleanup_in_room, NULL, NULL,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 63, 1, 0,   1},
  {creature_slap_cowers, NULL, NULL, NULL,
    1,   1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0,  0, 1},
  {creature_unconscious, state_cleanup_unconscious, NULL, NULL,
    1,  1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 9, 0, 0, 0, 1, 0, 0,  0, 1},
  {creature_pick_up_unconscious_body, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,  0, 0, 0, 0, 1},
  {imp_toking, NULL, NULL, NULL,
    0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {imp_picks_up_gold_pile, NULL, NULL, NULL, // [70]
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0,  0, 1},
  {move_backwards_to_position, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 1, 0, 0,  0, 0, 1},
  {creature_drop_body_in_prison, state_cleanup_dragging_body, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0,  0, 1},
  {imp_arrives_at_convert_dungeon, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,  0, 0, 0, 1},
  {imp_converts_dungeon, NULL, NULL, NULL,
    0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,  1},
  {creature_wants_salary, NULL, NULL, NULL,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7, 0, 0, 1, 0, 52,   1, 0, 1},
  {creature_take_salary, NULL, NULL, move_check_wait_at_door_for_wage,
    0,  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7, 0, 0, 1, 0, 52,   1, 0, 1},
  {tunneller_doing_nothing, NULL, NULL, NULL,
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,  0, 1},
  {creature_object_combat, cleanup_object_combat, NULL, NULL,
    1, 1,   1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 12, 0, 0, 1, 0, 51, 1, 0,  0},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 1},
  {creature_change_lair, NULL, NULL, move_check_on_head_for_room, // [80]
    0, 0,   1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 58, 1, 0,  1},
  {imp_birth, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {at_temple, NULL, NULL, move_check_on_head_for_room,
    0, 1, 0,  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 68, 1, 0,   1},
  {praying_in_temple, state_cleanup_in_temple, NULL, process_temple_function,
    0, 1, 0, 0,  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 2, 0, 68, 1, 0, 1},
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 1},
  {creature_follow_leader, NULL, NULL, NULL,
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0,  0, 0},
  {creature_door_combat, cleanup_door_combat, NULL, NULL,
    1, 1, 0,  1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 11, 0, 0, 1, 0, 51, 1, 0, 0},
  {creature_combat_flee, NULL, NULL, NULL,
    1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 53,   1, 0, 0},
  {creature_sacrifice, NULL, NULL, NULL,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
  {at_lair_to_sleep, NULL, NULL, NULL,
    0, 1, 0,  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 0, 0, 2, 0, 54, 1, 0,   1},
  {creature_fired, NULL, NULL, NULL, // [90]
    0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 61, 1, 0, 1},
  {creature_being_dropped, state_cleanup_unable_to_fight, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
  {creature_being_sacrificed, cleanup_sacrifice, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
  {creature_scavenged_disappear, NULL, NULL, NULL,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0,  0, 0, 0, 0},
  {creature_scavenged_reappear, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,  0, 0, 0},
  {creature_being_summoned, cleanup_sacrifice, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
  {creature_hero_entering, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,  0, 1},
  {imp_arrives_at_reinforce, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,  0, 1},
  {imp_reinforces, NULL, NULL, NULL,
    0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {arrive_at_alarm, NULL, NULL, NULL,
    1, 1, 1, 0,  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 62, 1, 0, 0},
  {creature_picks_up_spell_object, NULL, NULL, NULL, // [100]
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 1, 0,  0, 0, 0, 1},
  {creature_drops_spell_object_in_library, state_cleanup_dragging_object, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0,  0, 0, 1},
  {creature_picks_up_corpse, NULL, NULL, NULL,
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 1, 0, 0, 0,  0, 1},
  {creature_drops_corpse_in_graveyard, state_cleanup_dragging_object, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0,  0, 0, 1},
  {at_guard_post_room, NULL, NULL, move_check_on_head_for_room,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 50, 1, 0, 0},
  {guarding, state_cleanup_in_room, NULL, NULL,
    0, 1, 0,  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 0, 50, 1, 0, 0},
  {creature_eat, NULL, NULL, NULL,
    0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 59, 1, 0, 1},
  {creature_evacuate_room, NULL, NULL, NULL,
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,  0, 1},
  {creature_wait_at_treasure_room_door,  NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0,  1, 0, 0, 0, 0, 1},
  {at_kinky_torture_room, NULL, NULL, move_check_on_head_for_room,
    1, 1,   1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,  1},
  {kinky_torturing, cleanup_torturing, NULL, process_kinky_function, /// [110]
    1, 1, 1, 1,  1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {mad_killing_psycho, NULL, NULL, NULL,
    0, 1, 0,  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 55, 1, 0,   1},
  {creature_search_for_gold_to_steal_in_room, NULL, NULL, move_check_attack_any_door,
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 55,   1, 0, 1},
  {creature_vandalise_rooms, NULL, NULL, move_check_attack_any_door,
    0, 1,   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 55, 1, 0,  1},
  {creature_steal_gold, NULL, NULL, move_check_attack_any_door,
    0, 1,   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 55, 1, 0,  1},
  {seek_the_enemy, cleanup_seek_the_enemy, NULL, NULL,
    0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,  1},
  {already_at_call_to_arms, NULL, NULL, NULL,
    1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 1, 0,   62, 1, 0, 0},
  {creature_damage_walls, NULL, NULL, NULL,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0,  0, 1},
  {creature_attempt_to_damage_walls, NULL, NULL, move_check_can_damage_wall,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 55,   1, 0, 1},
  {creature_persuade, NULL, NULL, move_check_persuade,
    0, 1, 1, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 1, 0, 55, 1, 0, 1},
  {creature_change_to_chicken, NULL, NULL, NULL, // [120]
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0,  0, 0, 0},
  {creature_change_from_chicken, NULL, NULL, NULL,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0,  0, 0, 0, 0},
  {NULL, NULL, NULL, NULL,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1},
  {creature_cannot_find_anything_to_do, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,  1, 0, 0, 0, 0, 1},
  {creature_piss, NULL, NULL, NULL,
    0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
  {creature_roar, NULL, NULL, NULL,
    0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 1, 0, 55, 1, 0, 1},
  {creature_at_changed_lair, NULL, NULL, NULL,
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0,  0, 1},
  {creature_be_happy, NULL, NULL, NULL,
    0, 0,   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,  1},
  {good_leave_through_exit_door, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0,  0, 0, 0, 1},
  {good_wait_in_exit_door, NULL, NULL, NULL,
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,  0, 1},
  {good_attack_room, NULL, NULL, NULL, // [130]
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
  {creature_search_for_gold_to_steal_in_room, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 1,   1, 0, 0, 0, 0, 0, 1},
  {good_attack_room, NULL, NULL, NULL,
    0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
  {creature_pretend_chicken_setup_move, NULL, NULL, NULL,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1,  0, 0, 0, 0, 0, 0},
  {creature_pretend_chicken_move, NULL, NULL, NULL,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0,   0, 0, 0, 0},
  {creature_attack_rooms, NULL, NULL, move_check_attack_any_door,
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 51, 1, 0, 0},
  {creature_freeze_prisonors, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,  0, 0, 1},
  {creature_explore_dungeon, NULL, NULL, NULL,
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1},
  {creature_eating_at_garden, NULL, NULL, NULL,
    0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 0, 0, 2, 0,   59, 1, 0, 1},
  {creature_leaves_or_dies, NULL, NULL, NULL,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 2, 1, 1, 1, 0,   61, 1, 0, 0},
  {creature_moan, NULL, NULL, NULL, // [140]
    1, 1, 1, 0,  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 1},
  {creature_set_work_room_based_on_position, NULL, NULL, NULL,
    1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,  0, 0, 0, 0, 0, 1},
  {creature_being_scavenged, NULL, NULL, NULL,
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 69, 1, 0, 0},
  {creature_escaping_death, NULL, NULL, NULL,
    1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1,  8, 1, 0, 0,  0, 0, 1, 0, 0},
  {creature_present_to_dungeon_heart, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
  {creature_search_for_spell_to_steal_in_room, NULL, NULL, move_check_attack_any_door,
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 55, 1, 0, 1},
  {creature_pick_up_spell_to_steal, NULL, NULL, move_check_attack_any_door,
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 55, 1, 0, 1},
  // Some redundant NULLs
  {NULL, NULL, NULL, NULL,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

/** GUI States of creatures - from "Creatures" Tab in UI.
 * There are three states:
 * - 0: Idle.
 * - 1: Working.
 * - 2: Fighting.
 */
long const state_type_to_gui_state[] = {
    0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 0, 2, 2, 1, 1, 0,
};

/******************************************************************************/
struct StateInfo *get_thing_active_state_info(struct Thing *thing)
{
  if (thing->active_state >= CREATURE_STATES_COUNT)
    return &states[0];
  return &states[thing->active_state];
}

struct StateInfo *get_thing_continue_state_info(struct Thing *thing)
{
    if (thing->continue_state >= CREATURE_STATES_COUNT)
        return &states[0];
    return &states[thing->continue_state];
}

struct StateInfo *get_thing_state_info_num(CrtrStateId state_id)
{
    if ((state_id < 0) || (state_id >= CREATURE_STATES_COUNT))
        return &states[0];
    return &states[state_id];
}

CrtrStateId get_creature_state_besides_move(const struct Thing *thing)
{
    long i;
    i = thing->active_state;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    return i;
}

CrtrStateId get_creature_state_besides_drag(const struct Thing *thing)
{
    long i;
    i = thing->active_state;
    if (i == CrSt_MoveBackwardsToPosition)
        i = thing->continue_state;
    return i;
}

struct StateInfo *get_creature_state_with_task_completion(struct Thing *thing)
{
  struct StateInfo *stati;
  stati = get_thing_active_state_info(thing);
  if (stati->state_type == CrStTyp_Value6)
      stati = get_thing_continue_state_info(thing);
  return stati;
}

TbBool state_info_invalid(struct StateInfo *stati)
{
  if (stati <= &states[0])
    return true;
  return false;
}

TbBool creature_model_bleeds(unsigned long crmodel)
{
  struct CreatureStats *crstat;
  struct CreatureModelConfig *crconf;
  crstat = creature_stats_get(crmodel);
  if ( censorship_enabled() )
  {
      // If censorship is on, only evil creatures can have blood
      if (!crstat->bleeds)
          return false;
      crconf = &crtr_conf.model[crmodel];
      return ((crconf->model_flags & MF_IsEvil) != 0);
  }
  return crstat->bleeds;
}
/******************************************************************************/
/** Returns type of given creature state.
 *
 * @param thing The source thing.
 * @return Type of the creature state.
 */
long get_creature_state_type(const struct Thing *thing)
{
  long state_type;
  long state;
  state = thing->active_state;
  if ( (state > 0) && (state < sizeof(states)/sizeof(states[0])) )
  {
      state_type = states[state].state_type;
  } else
  {
      state_type = states[0].state_type;
      WARNLOG("Creature active state %d is out of range.",(int)state);
  }
  if (state_type == 6)
  {
      state = thing->continue_state;
      if ( (state > 0) && (state < sizeof(states)/sizeof(states[0])) )
      {
          state_type = states[state].state_type;
      } else
      {
          state_type = states[0].state_type;
          WARNLOG("Creature continue state %d is out of range.",(int)state);
      }
  }
  return state_type;
}

/** Returns GUI Job of given creature.
 *  The GUI Job is a simplified version of creature state which
 *  only takes 3 values: 0-idle, 1-working, 2-fighting.
 *
 * @param thing The source thing.
 * @return GUI state, in range 0..2.
 */
long get_creature_gui_job(const struct Thing *thing)
{
    long state_type;
    state_type = get_creature_state_type(thing);
    if ( (state_type >= 0) && (state_type < sizeof(state_type_to_gui_state)/sizeof(state_type_to_gui_state[0])) )
    {
        return state_type_to_gui_state[state_type];
    } else
    {
        WARNLOG("The %s has invalid state type(%ld)!",thing_model_name(thing),state_type);
        erstat_inc(ESE_BadCreatrState);
        return state_type_to_gui_state[0];
    }
}

TbBool creature_is_doing_lair_activity(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_CreatureSleep)
        return true;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if ((i == CrSt_CreatureGoingHomeToSleep) || (i == CrSt_AtLairToSleep)
      || (i == CrSt_CreatureChooseRoomForLairSite) || (i == CrSt_CreatureAtNewLair) || (i == CrSt_CreatureWantsAHome)
      || (i == CrSt_CreatureChangeLair) || (i == CrSt_CreatureAtChangedLair))
        return true;
    return false;
}

TbBool creature_is_being_dropped(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if (i == CrSt_CreatureBeingDropped)
        return true;
    return false;
}

TbBool creature_is_being_tortured(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if ((i == CrSt_Torturing) || (i == CrSt_AtTortureRoom))
        return true;
    return false;
}

TbBool creature_is_being_sacrificed(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if ((i == CrSt_CreatureSacrifice) || (i == CrSt_CreatureBeingSacrificed))
        return true;
    return false;
}

TbBool creature_is_kept_in_prison(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if ((i == CrSt_CreatureInPrison) || (i == CrSt_CreatureArrivedAtPrison))
        return true;
    return false;
}

TbBool creature_is_being_summoned(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if ((i == CrSt_CreatureBeingSummoned))
        return true;
    return false;
}

TbBool creature_is_training(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if ((i == CrSt_Training) || (i == CrSt_AtTrainingRoom))
        return true;
    return false;
}

TbBool creature_is_doing_dungeon_improvements(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if (states[i].state_type == 10)
        return true;
    return false;
}

TbBool creature_is_doing_garden_activity(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_CreatureEat)
        return true;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if ((i == CrSt_CreatureToGarden) || (i == CrSt_CreatureArrivedAtGarden))
        return true;
    return false;
}

TbBool creature_is_scavengering(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if ((i == CrSt_Scavengering) || (i == CrSt_AtScavengerRoom))
        return true;
    return false;
}

TbBool creature_is_escaping_death(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if (i == CrSt_CreatureEscapingDeath)
        return true;
    return false;
}

TbBool creature_is_taking_salary_activity(const struct Thing *thing)
{
    CrtrStateId i;
    i = thing->active_state;
    if (i == CrSt_CreatureWantsSalary)
        return true;
    if (i == CrSt_MoveToPosition)
        i = thing->continue_state;
    if (i == CrSt_CreatureTakeSalary)
        return true;
    return false;
}

TbBool creature_is_arming_trap(const struct Thing *thing)
{
    CrtrStateId crstate;
    crstate = get_creature_state_besides_move(thing);
    if (crstate == CrSt_CreaturePicksUpTrapObject) {
        return true;
    }
    crstate = get_creature_state_besides_drag(thing);
    if (crstate == CrSt_CreatureArmsTrap) {
        return true;
    }
    return false;
}

TbBool creature_is_kept_in_custody(const struct Thing *thing)
{
    return (creature_is_kept_in_prison(thing) ||
            creature_is_being_tortured(thing) ||
            creature_is_being_sacrificed(thing) ||
            creature_is_being_dropped(thing));
}

TbBool creature_state_is_unset(const struct Thing *thing)
{
    CrtrStateId crstate;
    crstate = get_creature_state_besides_move(thing);
    if (states[crstate].state_type == 0)
        return true;
    return false;
}

short already_at_call_to_arms(struct Thing *creatng)
{
    //return _DK_already_at_call_to_arms(thing);
    internal_set_thing_state(creatng, CrSt_ArriveAtCallToArms);
    return 1;
}

short arrive_at_alarm(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    //return _DK_arrive_at_alarm(thing);
    cctrl = creature_control_get_from_thing(creatng);
    if (cctrl->field_2FA < (unsigned long)game.play_gameturn)
    {
        set_start_state(creatng);
        return 1;
    }
    if (ACTION_RANDOM(4) == 0)
    {
        if ( setup_person_move_close_to_position(creatng, cctrl->field_2F8, cctrl->field_2F9, 0) )
        {
            creatng->continue_state = CrSt_ArriveAtAlarm;
            return 1;
        }
    }
    if ( creature_choose_random_destination_on_valid_adjacent_slab(creatng) )
    {
        creatng->continue_state = CrSt_ArriveAtAlarm;
        return 1;
    }
    return 1;
}

long setup_head_for_room(struct Thing *thing, struct Room *room, unsigned char a3)
{
    struct Coord3d pos;
    if ( !find_first_valid_position_for_thing_in_room(thing, room, &pos) )
        return false;
    return setup_person_move_to_position(thing, pos.x.stl.num, pos.y.stl.num, a3);
}

TbBool attempt_to_destroy_enemy_room(struct Thing *thing, unsigned char stl_x, unsigned char stl_y)
{
    struct CreatureControl *cctrl;
    struct Room *room;
    struct Coord3d pos;
    room = subtile_room_get(stl_x, stl_y);
    if (room_is_invalid(room))
        return false;
    if (thing->owner == room->owner)
        return false;
    if (room_cannot_vandalize(room->kind))
        return false;
    if ( !find_first_valid_position_for_thing_in_room(thing, room, &pos) )
        return false;
    if ( !creature_can_navigate_to_with_storage(thing, &pos, 1) )
        return false;

    if ( !setup_head_for_room(thing, room, 1) )
    {
        ERRORLOG("Cannot do this 'destroy room' stuff");
        return false;
    }
    event_create_event_or_update_nearby_existing_event(subtile_coord_center(room->central_stl_x),
        subtile_coord_center(room->central_stl_y), EvKind_RoomUnderAttack, room->owner, 0);
    if (is_my_player_number(room->owner))
        output_message(SMsg_EnemyDestroyRooms, 400, true);
    thing->continue_state = CrSt_CreatureAttackRooms;
    cctrl = creature_control_get_from_thing(thing);
    if (!creature_control_invalid(cctrl))
        cctrl->target_room_id = room->index;
    return true;
}

short arrive_at_call_to_arms(struct Thing *creatng)
{
    struct Dungeon *dungeon;
    struct Thing *doortng;
    SYNCDBG(18,"Starting");
    //return _DK_arrive_at_call_to_arms(thing);
    dungeon = get_dungeon(creatng->owner);
    if (dungeon->field_884 == 0)
    {
        set_start_state(creatng);
        return 1;
    }
    doortng = check_for_door_to_fight(creatng);
    if (!thing_is_invalid(doortng))
    {
        set_creature_door_combat(creatng, doortng);
        return 2;
    }
    if ( !attempt_to_destroy_enemy_room(creatng, dungeon->field_881, dungeon->field_882) )
    {
      if (ACTION_RANDOM(7) == 0)
      {
          if ( setup_person_move_close_to_position(creatng, dungeon->field_881, dungeon->field_882, 0) )
          {
              creatng->continue_state = CrSt_AlreadyAtCallToArms;
              return 1;
          }
      }
      if ( creature_choose_random_destination_on_valid_adjacent_slab(creatng) )
      {
        creatng->continue_state = CrSt_AlreadyAtCallToArms;
        return 1;
      }
    }
    return 1;
}

TbBool terrain_toxic_for_creature_at_position(const struct Thing *creatng, MapSubtlCoord stl_x, MapSubtlCoord stl_y)
{
    struct CreatureStats *crstat;
    crstat = creature_stats_get_from_thing(creatng);
    // If the position is over lava, and we can't continuously fly, then it's toxic
    if ((crstat->hurt_by_lava > 0) && map_pos_is_lava(stl_x,stl_y)) {
        // Check not only if a creature is now flying, but also whether it's natural ability
        if (((creatng->movement_flags & TMvF_Flying) == 0) || (!crstat->flying))
            return true;
    }
    return false;
}

/**
 * Finds a safe position for creature on one of subtiles of given slab.
 * @param pos The returned position.
 * @param thing The creature to be moved.
 * @param slb_x Given slab to move onto, X coord.
 * @param slb_y Given slab to move onto, Y coord.
 * @param start_stl Starting element when testing subtiles, used for enhanced randomness.
 * @return True if the safe position was found; false otherwise.
 */
TbBool creature_find_safe_position_to_move_within_slab(struct Coord3d *pos, const struct Thing *thing, MapSlabCoord slb_x, MapSlabCoord slb_y, MapSubtlCoord start_stl)
{
    MapSubtlCoord base_x,base_y;
    MapSubtlCoord stl_x,stl_y;
    stl_x = thing->mappos.x.stl.num;
    stl_y = thing->mappos.y.stl.num;
    base_x = slab_subtile(slb_x,0);
    base_y = slab_subtile(slb_y,0);
    long i,k;
    k = start_stl;
    for (i=0; i < 9; i++)
    {
        MapSubtlCoord x,y;
        x = base_x + (k%3);
        y = base_y + (k/3);
        if ((x != stl_x) || (y != stl_y))
        {
            struct Map *mapblk;
            mapblk = get_map_block_at(x,y);
            if ((mapblk->flags & MapFlg_Unkn10) == 0)
            {
                if (!terrain_toxic_for_creature_at_position(thing, x, y))
                {
                    int block_radius;
                    block_radius = subtile_coord(thing_nav_block_sizexy(thing),0) / 2;
                    pos->x.val = subtile_coord_center(x);
                    pos->y.val = subtile_coord_center(y);
                    pos->z.val = get_thing_height_at_with_radius(thing, pos, block_radius);
                    return true;
                }
            }
        }
        k = (k+1) % 9;
    }
    return false;
}

/**
 * Finds any position for creature on one of subtiles of given slab.
 * To be used when finding correct, safe position fails.
 * @param pos The returned position.
 * @param thing The creature to be moved.
 * @param slb_x Given slab to move onto, X coord.
 * @param slb_y Given slab to move onto, Y coord.
 * @param start_stl Starting element when testing subtiles, used for enhanced randomness.
 * @return True if any position was found; false otherwise.
 */
TbBool creature_find_any_position_to_move_within_slab(struct Coord3d *pos, const struct Thing *thing, MapSlabCoord slb_x, MapSlabCoord slb_y, MapSubtlCoord start_stl)
{
    MapSubtlCoord base_x,base_y;
    MapSubtlCoord stl_x,stl_y;
    stl_x = thing->mappos.x.stl.num;
    stl_y = thing->mappos.y.stl.num;
    base_x = 3 * slb_x;
    base_y = 3 * slb_y;
    long i,k;
    k = start_stl;
    for (i=0; i < 9; i++)
    {
        MapSubtlCoord x,y;
        x = base_x + (k%3);
        y = base_y + (k/3);
        if ((x != stl_x) || (y != stl_y))
        {
            pos->x.val = subtile_coord_center(x);
            pos->y.val = subtile_coord_center(y);
            pos->z.val = get_thing_height_at(thing, pos);
            return true;
        }
        k = (k+1) % 9;
    }
    return false;
}

/**
 * Finds a safe, adjacent position in room for a creature.
 * Makes sure the specific room currently occupied is not left.
 * @param thing The creature to be moved.
 * @param room The room inside which the creature should stay.
 * @param pos The adjacent position returned.
 * @return True if a position was found, false if cannot move.
 * @see find_position_around_in_room()
 */
TbBool person_get_somewhere_adjacent_in_room(const struct Thing *thing, const struct Room *room, struct Coord3d *pos)
{
    struct Room *aroom;
    struct Map *mapblk;
    MapSlabCoord slb_x,slb_y;
    struct Coord3d locpos;
    int block_radius;
    long slab_num,slab_base;
    int start_stl;
    long m,n;
    SYNCDBG(17,"Starting for %s index %d",thing_model_name(thing),(long)thing->index);
    //return _DK_person_get_somewhere_adjacent_in_room(thing, room, pos);
    block_radius = subtile_coord(thing_nav_block_sizexy(thing),0) >> 1;
    slb_x = subtile_slab_fast(thing->mappos.x.stl.num);
    slb_y = subtile_slab_fast(thing->mappos.y.stl.num);
    slab_base = get_slab_number(slb_x, slb_y);

    start_stl = ACTION_RANDOM(AROUND_MAP_LENGTH);
    m = ACTION_RANDOM(SMALL_AROUND_SLAB_LENGTH);
    for (n=0; n < SMALL_AROUND_SLAB_LENGTH; n++)
    {
        slab_num = slab_base + small_around_slab[m];
        slb_x = slb_num_decode_x(slab_num);
        slb_y = slb_num_decode_y(slab_num);
        aroom = INVALID_ROOM;
        mapblk = get_map_block_at(3 * slb_x, 3 * slb_y);
        if ((mapblk->flags & MapFlg_IsRoom) != 0)
        {
            aroom = slab_room_get(slb_x, slb_y);
        }
        if (room_exists(aroom) && (aroom->index == room->index))
        {
            if (creature_find_safe_position_to_move_within_slab(&locpos, thing, slb_x, slb_y, start_stl))
            {
                if (!thing_in_wall_at_with_radius(thing, &locpos, block_radius))
                {
                    pos->z.val = locpos.x.val;
                    pos->y.val = locpos.y.val;
                    pos->z.val = locpos.z.val;
                    SYNCDBG(8,"Possible to move %s index %d from (%d,%d) to (%d,%d)", thing_model_name(thing),
                        (int)thing->index, (int)thing->mappos.x.stl.num, (int)thing->mappos.y.stl.num,
                        (int)locpos.x.stl.num, (int)locpos.y.stl.num);
                    return true;
                }
            }
        }
        m = (m + 1) % SMALL_AROUND_SLAB_LENGTH;
    }
    // Cannot find a good position - but at least move within the same slab we're on
    {
        slb_x = subtile_slab_fast(thing->mappos.x.stl.num);
        slb_y = subtile_slab_fast(thing->mappos.y.stl.num);
        aroom = INVALID_ROOM;
        mapblk = get_map_block_at(3 * slb_x, 3 * slb_y);
        if ((mapblk->flags & MapFlg_IsRoom) != 0)
        {
            aroom = slab_room_get(slb_x, slb_y);
        }
        if (room_exists(aroom) && (aroom->index == room->index))
        {
            if (creature_find_safe_position_to_move_within_slab(&locpos, thing, slb_x, slb_y, start_stl))
            {
                if (!thing_in_wall_at_with_radius(thing, &locpos, block_radius))
                {
                    pos->x.val = locpos.x.val;
                    pos->y.val = locpos.y.val;
                    pos->z.val = locpos.z.val;
                    SYNCDBG(8,"Possible to move %s index %d from (%d,%d) to (%d,%d)", thing_model_name(thing),
                        (int)thing->index, (int)thing->mappos.x.stl.num, (int)thing->mappos.y.stl.num,
                        (int)locpos.x.stl.num, (int)locpos.y.stl.num);
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Finds a safe, adjacent position in room for a creature.
 * Allows moving to adjacent room of the same kind and owner.
 *
 * @param pos Position of the creature to be moved.
 * @param owner Room owner to keep.
 * @param rkind Kind of the room inside which the creature should stay.
 * @return Coded subtiles of the new position, or 0 on failure.
 * @see person_get_somewhere_adjacent_in_room()
 */
SubtlCodedCoords find_position_around_in_room(const struct Coord3d *pos, long owner, long rkind)
{
    SubtlCodedCoords stl_num;
    SubtlCodedCoords accepted_stl_num;
    long m,n;
    long dist;
    m = ACTION_RANDOM(AROUND_MAP_LENGTH);
    for (n = 0; n < AROUND_MAP_LENGTH; n++)
    {
        accepted_stl_num = 0;
        stl_num = get_subtile_number(pos->x.stl.num,pos->y.stl.num);
        // Skip the position equal to current position
        if (around_map[m] == 0)
        {
            m = (m + 1) % AROUND_MAP_LENGTH;
            continue;
        }
        // Move radially from of the current position; stop if a room tile
        // of incorrect kind or owner is encoured
        for (dist = 0; dist < 8; dist++)
        {
            struct Room *room;
            struct Map *mapblk;
            struct SlabMap *slb;
            MapSubtlCoord stl_x,stl_y;
            stl_num += around_map[m];
            mapblk = get_map_block_at_pos(stl_num);
            if ( ((mapblk->flags & MapFlg_IsRoom) != 0) && ((mapblk->flags & MapFlg_Unkn10) != 0) )
                break;
            stl_x = stl_num_decode_x(stl_num);
            stl_y = stl_num_decode_y(stl_num);
            slb = get_slabmap_for_subtile(stl_x,stl_y);
            if (slabmap_owner(slb) != owner)
                break;
            room = room_get(slb->room_index);
            if (room->kind != rkind)
                break;
            if (dist > 1)
            {
                // Accept, but don't just break the loop. Wait for larger distance.
                accepted_stl_num = stl_num;
            }
        }
        if (accepted_stl_num > 0)
        {
            return accepted_stl_num;
        }
          m = (m + 1) % AROUND_MAP_LENGTH;
    }
    return 0;
}

short cleanup_hold_audience(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    //return _DK_cleanup_hold_audience(thing);
    cctrl = creature_control_get_from_thing(creatng);
    cctrl->max_speed = calculate_correct_creature_maxspeed(creatng);
    return 0;
}

short cleanup_seek_the_enemy(struct Thing *thing)
{
    struct CreatureControl *cctrl;
    //return _DK_cleanup_seek_the_enemy(thing);
    cctrl = creature_control_get_from_thing(thing);
    cctrl->word_9A = 0;
    cctrl->long_9C = 0;
    return 1;
}

short creature_being_dropped_at_sacrificial_ground(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    struct SlabMap *slb;
    slb = get_slabmap_for_subtile(creatng->mappos.x.stl.num,creatng->mappos.y.stl.num);
    cctrl = creature_control_get_from_thing(creatng);
    if (slabmap_owner(slb) == creatng->owner)
    {
        cctrl->flgfield_1 &= ~CCFlg_NoCompControl;
        set_start_state(creatng);
    }
    if ( creature_will_do_combat(creatng) )
    {
        if ( creature_look_for_combat(creatng) ) {
            return 2;
        }
        if (creature_look_for_enemy_heart_combat(creatng)) {
            return 2;
        }
    }
    if (creature_is_group_member(creatng)) {
        remove_creature_from_group(creatng);
    }
    cctrl->flgfield_1 |= CCFlg_NoCompControl;
    initialise_thing_state(creatng, CrSt_CreatureSacrifice);
    return 2;
}

short creature_being_dropped(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    long stl_x, stl_y;
    struct Room *room;
    struct Thing *leadtng;
    //return _DK_creature_being_dropped(thing);
    cctrl = creature_control_get_from_thing(creatng);
    cctrl->flgfield_1 |= CCFlg_NoCompControl;
    cctrl->instance_use_turn[CrInst_TELEPORT] = game.play_gameturn + 100;
    stl_x = creatng->mappos.x.stl.num;
    stl_y = creatng->mappos.y.stl.num;
    // If dropped on sacrificial ground, process the sacrifice
    if (subtile_has_sacrificial_on_top(stl_x, stl_y))
    {
        return creature_being_dropped_at_sacrificial_ground(creatng);
    }
    // If dropping still in progress, do nothing
    if ( !thing_touching_floor(creatng) && ((creatng->movement_flags & TMvF_Flying) == 0) )
    {
        return 1;
    }
    set_creature_assigned_job(creatng, Job_NULL);
    if (!creature_affected_by_spell(creatng, SplK_Chicken))
    {
        if ((get_creature_model_flags(creatng) & MF_TremblingFat) != 0)
        {
            struct Dungeon *dungeon;
            dungeon = get_dungeon(creatng->owner);
            if (!dungeon_invalid(dungeon))
                dungeon->camera_deviate_jump = 96;
        }
        set_start_state(creatng);
        struct SlabMap *slb;
        slb = get_slabmap_for_subtile(stl_x,stl_y);
        if (slabmap_owner(slb) == creatng->owner)
            cctrl->flgfield_1 &= ~CCFlg_NoCompControl;
        else
            cctrl->flgfield_1 |= CCFlg_NoCompControl;
        check_map_explored(creatng, stl_x, stl_y);
        leadtng = get_group_leader(creatng);
        if (!thing_is_invalid(leadtng))
        {
            if (leadtng != creatng)
            {
                if ( ((leadtng->alloc_flags & TAlF_IsInLimbo) == 0) && ((leadtng->field_1 & TF1_InCtrldLimbo) == 0) )
                {
                    if ( get_2d_box_distance(&creatng->mappos, &leadtng->mappos) > 1536 )
                        remove_creature_from_group(creatng);
                }
            }
        }
        if ( creature_will_do_combat(creatng) )
        {
            if (creature_look_for_combat(creatng)) {
                return 2;
            }
            if (creature_look_for_enemy_heart_combat(creatng)) {
                return 2;
            }
            if (creature_look_for_enemy_door_combat(creatng)) {
                return 2;
            }
        }
        if ((get_creature_model_flags(creatng) & MF_IsSpecDigger) != 0)
        {
            if ((slabmap_owner(slb) == creatng->owner) || (slabmap_owner(slb) == game.neutral_player_num))
            {
                if (check_out_available_imp_drop_tasks(creatng))
                {
                    cctrl->flgfield_1 &= ~CCFlg_NoCompControl;
                    return 2;
                }
            }
        }
    }
    room = get_room_thing_is_on(creatng);
    if (room_is_invalid(room))
    {
        cctrl->flgfield_1 &= ~CCFlg_NoCompControl;
        set_start_state(creatng);
        return 2;
    }
    if (room->owner != creatng->owner)
    {
        if (!enemies_may_work_in_room(room->kind))
        {
            cctrl->flgfield_1 &= ~CCFlg_NoCompControl;
            set_start_state(creatng);
            return 2;
        }
    }
    if ( room->kind == RoK_ENTRANCE || (room->kind == RoK_PRISON || room->kind == RoK_TORTURE) )
    {
        if (creature_is_group_member(creatng)) {
            remove_creature_from_group(creatng);
        }
    }
    if (room->kind == RoK_TEMPLE)
    {
        if (subtile_has_sacrificial_on_top(stl_x, stl_y))
        {
            cctrl->flgfield_1 &= ~CCFlg_NoCompControl;
            set_start_state(creatng);
            return 2;
        }
    }
    if ( creature_affected_by_spell(creatng, SplK_Chicken) && (room->kind != RoK_TEMPLE) )
    {
        set_start_state(creatng);
        return 2;
    }
    if ( !send_creature_to_room(creatng, room) )
    {
        cctrl->flgfield_1 &= ~CCFlg_NoCompControl;
        set_start_state(creatng);
        return 2;
    }
    set_creature_assigned_job(creatng, get_job_for_room(room->kind, false));
    return 2;
}

void anger_set_creature_anger(struct Thing *creatng, long annoy_lv, long reason)
{
    SYNCDBG(8,"Setting to %d",(int)annoy_lv);
    _DK_anger_set_creature_anger(creatng, annoy_lv, reason);
}

TbBool anger_is_creature_livid(const struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    cctrl = creature_control_get_from_thing(creatng);
    if (creature_control_invalid(cctrl))
        return false;
    return ((cctrl->field_66 & 0x02) != 0);
}

TbBool anger_is_creature_angry(const struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    cctrl = creature_control_get_from_thing(creatng);
    if (creature_control_invalid(cctrl))
        return false;
    return ((cctrl->field_66 & 0x01) != 0);
}

TbBool anger_make_creature_angry(struct Thing *creatng, long reason)
{
  struct CreatureStats *crstat;
  struct CreatureControl *cctrl;
  cctrl = creature_control_get_from_thing(creatng);
  crstat = creature_stats_get_from_thing(creatng);
  if ((crstat->annoy_level <= 0) || ((cctrl->field_66 & 0x01) != 0))
    return false;
  anger_set_creature_anger(creatng, crstat->annoy_level, reason);
  return true;
}

short creature_cannot_find_anything_to_do(struct Thing *creatng)
{
  return _DK_creature_cannot_find_anything_to_do(creatng);
}

void set_creature_size_stuff(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    cctrl = creature_control_get_from_thing(creatng);
    if (creature_affected_by_spell(creatng, SplK_Chicken)) {
      creatng->field_46 = 300;
    } else {
      creatng->field_46 = 300 + (300 * cctrl->explevel) / 20;
    }
}

short creature_change_from_chicken(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    //return _DK_creature_change_from_chicken(creatng);
    cctrl = creature_control_get_from_thing(creatng);
    creature_set_speed(creatng, 0);
    if (cctrl->field_282 > 0)
        cctrl->field_282--;
    if (cctrl->field_282 > 0)
    { // Changing under way - gradually modify size of the creature
        creatng->field_4F |= 0x01;
        creatng->field_50 |= 0x01;
        struct Thing *efftng;
        efftng = create_effect_element(&creatng->mappos, 0x3Bu, creatng->owner);
        if (!thing_is_invalid(efftng))
        {
            unsigned long k;
            long n;
            n = (10 - cctrl->field_282) * (300 * cctrl->explevel / 20 + 300) / 10;
            k = get_creature_anim(creatng, 0);
            set_thing_draw(efftng, k, 256, n, -1, 0, 2);
            efftng->field_4F &= ~0x20;
            efftng->field_4F |= 0x10;
        }
        return 0;
    } else
    {
      creatng->field_4F &= ~0x01;
      cctrl->affected_by_spells &= ~0x01;
      cctrl->spell_flags &= ~0x0200;
      set_creature_size_stuff(creatng);
      set_start_state(creatng);
      return 1;
    }
}

short creature_change_to_chicken(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    //return _DK_creature_change_to_chicken(creatng);
    cctrl = creature_control_get_from_thing(creatng);
    creature_set_speed(creatng, 0);
    if (cctrl->field_282 > 0)
        cctrl->field_282--;
    if (cctrl->field_282 > 0)
    {
      creatng->field_50 |= 0x01;
      creatng->field_4F |= 0x01;
      struct Thing *efftng;
      efftng = create_effect_element(&creatng->mappos, 59, creatng->owner);
      if (!thing_is_invalid(efftng))
      {
          unsigned long k;
          k = convert_td_iso(819);
          set_thing_draw(efftng, k, 0, 1200 * cctrl->field_282 / 10 + 300, -1, 0, 2);
          efftng->field_4F &= ~0x20;
          efftng->field_4F |= 0x10;
      }
      return 0;
    }
    cctrl->spell_flags |= CSAfF_Chicken;
    creatng->field_4F &= ~0x01;
    set_creature_size_stuff(creatng);
    creatng->field_1 &= ~0x10;
    creatng->active_state = CrSt_CreaturePretendChickenSetupMove;
    creatng->continue_state = CrSt_Unused;
    cctrl->field_302 = 0;
    clear_creature_instance(creatng);
    return 1;
}

short creature_doing_nothing(struct Thing *creatng)
{
    return _DK_creature_doing_nothing(creatng);
}

void creature_drop_dragged_object(struct Thing *crtng, struct Thing *dragtng)
{
    struct CreatureControl *cctrl;
    cctrl = creature_control_get_from_thing(crtng);
    if (cctrl->dragtng_idx == 0)
    {
        ERRORLOG("The %s isn't dragging anything",thing_model_name(crtng));
    } else
    if (dragtng->index != cctrl->dragtng_idx)
    {
        ERRORLOG("The %s isn't dragging %s",thing_model_name(crtng),thing_model_name(dragtng));
    }
    cctrl->dragtng_idx = 0;
    dragtng->alloc_flags &= ~TAlF_IsDragged;
    dragtng->field_1 &= ~TF1_IsDragged1;
    move_thing_in_map(dragtng, &crtng->mappos);
    if (dragtng->light_id != 0) {
        light_turn_light_on(dragtng->light_id);
    }
}

/**
 * Returns if given slab meets the requirements for a creature to move on it for its own will.
 * @param thing The creature which is moving.
 * @param slb_x The destination slab, X coord.
 * @param slb_y The destination slab, Y coord.
 * @return True if the creature is willing to move on that slab, false otherwise.
 */
TbBool slab_is_valid_for_creature_choose_move(struct Thing *thing, MapSlabCoord slb_x, MapSlabCoord slb_y)
{
    struct SlabMap *slb;
    struct SlabAttr *slbattr;
    MapSubtlCoord base_x,base_y;
    struct Thing *doortng;
    slb = get_slabmap_block(slb_x, slb_y);
    slbattr = get_slab_attrs(slb);
    if ( ((slbattr->flags & SlbAtFlg_Unk02) != 0) || ((slbattr->flags & SlbAtFlg_Unk10) == 0) )
        return true;
    base_x = 3 * slb_x;
    base_y = 3 * slb_y;
    doortng = get_door_for_position(base_x, base_y);
    if (!thing_is_invalid(doortng))
    {
      if ((doortng->owner == thing->owner) && (!doortng->byte_18))
          return true;
    }
    return false;
}

TbBool creature_choose_random_destination_on_valid_adjacent_slab(struct Thing *thing)
{
    MapSlabCoord slb_x,slb_y;
    MapSubtlCoord start_stl;
    long slab_num,slab_base;
    long m,n;
    SYNCDBG(17,"Starting for %s index %d",thing_model_name(thing),(long)thing->index);
    //return _DK_creature_choose_random_destination_on_valid_adjacent_slab(thing);
    slb_x = subtile_slab_fast(thing->mappos.x.stl.num);
    slb_y = subtile_slab_fast(thing->mappos.y.stl.num);
    slab_base = get_slab_number(slb_x, slb_y);

    start_stl = ACTION_RANDOM(9);
    m = ACTION_RANDOM(SMALL_AROUND_SLAB_LENGTH);
    for (n=0; n < SMALL_AROUND_SLAB_LENGTH; n++)
    {
        slab_num = slab_base + small_around_slab[m];
        slb_x = slb_num_decode_x(slab_num);
        slb_y = slb_num_decode_y(slab_num);
        if (slab_is_valid_for_creature_choose_move(thing, slb_x, slb_y))
        {
            struct Coord3d locpos;
            if (creature_find_safe_position_to_move_within_slab(&locpos, thing, slb_x, slb_y, start_stl))
            {
                if (setup_person_move_to_position(thing, locpos.x.stl.num, locpos.y.stl.num, 0))
                {
                    SYNCDBG(8,"Moving thing %d from (%d,%d) to (%d,%d)", thing_model_name(thing),
                        (int)thing->index, (int)thing->mappos.x.stl.num, (int)thing->mappos.y.stl.num,
                        (int)locpos.x.stl.num, (int)locpos.y.stl.num);
                    return true;
                }
            }
        }
        m = (m+1) % SMALL_AROUND_SLAB_LENGTH;
    }
    // Cannot find a good position - but at least move within the same slab we're on
    {
        slb_x = subtile_slab_fast(thing->mappos.x.stl.num);
        slb_y = subtile_slab_fast(thing->mappos.y.stl.num);
        struct Coord3d locpos;
        if (creature_find_any_position_to_move_within_slab(&locpos, thing, slb_x, slb_y, start_stl))
        {
            if (setup_person_move_to_position(thing, locpos.x.stl.num, locpos.y.stl.num, 0))
            {
                SYNCDBG(8,"Forcefully moving %s index %d from (%d,%d) to (%d,%d)", thing_model_name(thing),
                    (int)thing->index, (int)thing->mappos.x.stl.num, (int)thing->mappos.y.stl.num,
                    (int)locpos.x.stl.num, (int)locpos.y.stl.num);
                return true;
            }
        }
    }
    SYNCDBG(8,"Moving %s index %d from (%d,%d) failed",thing_model_name(thing),(int)thing->index,(int)thing->mappos.x.stl.num,(int)thing->mappos.y.stl.num);
    return false;
}

short creature_dormant(struct Thing *creatng)
{
    //return _DK_creature_dormant(thing);
    if (creature_choose_random_destination_on_valid_adjacent_slab(creatng))
    {
      creatng->continue_state = CrSt_CreatureDormant;
      return 1;
    }
    return 0;
}

short creature_escaping_death(struct Thing *creatng)
{
  return _DK_creature_escaping_death(creatng);
}

long get_best_position_outside_room(struct Thing *creatng, struct Coord3d *pos, struct Room *room)
{
    return _DK_get_best_position_outside_room(creatng, pos, room);
}

short creature_evacuate_room(struct Thing *creatng)
{
    struct Coord3d pos;
    //return _DK_creature_evacuate_room(creatng);
    pos.x.val = creatng->mappos.x.val;
    pos.y.val = creatng->mappos.y.val;
    pos.z.val = creatng->mappos.z.val;
    if (!thing_is_on_any_room_tile(creatng))
    {
        set_start_state(creatng);
        return CrCkRet_Continue;
    }
    struct Room *room;
    struct CreatureControl *cctrl;
    room = get_room_thing_is_on(creatng);
    cctrl = creature_control_get_from_thing(creatng);
    if (cctrl->word_9A != room->index)
    {
        set_start_state(creatng);
        return CrCkRet_Continue;
    }
    long ret;
    ret = get_best_position_outside_room(creatng, &pos, room);
    set_creature_assigned_job(creatng, Job_NULL);
    if (ret != 1)
    {
        if (ret == -1)
        {
            set_start_state(creatng);
            return CrCkRet_Continue;
        }
        return CrCkRet_Continue;
    }
    if (!setup_person_move_to_position(creatng, pos.x.stl.num, pos.y.stl.num, 0))
    {
        ERRORLOG("Cannot nav outside room");
        set_start_state(creatng);
        return CrCkRet_Continue;
    }
    creatng->continue_state = CrSt_CreatureEvacuateRoom;
    return CrCkRet_Continue;
}

short creature_explore_dungeon(struct Thing *creatng)
{
    struct Coord3d pos;
    TbBool got_position;
    //return _DK_creature_explore_dungeon(creatng);
    got_position = get_random_position_in_dungeon_for_creature(creatng->owner, 0, creatng, &pos);
    if (!got_position) {
        got_position = get_random_position_in_dungeon_for_creature(creatng->owner, 1, creatng, &pos);
    }
    if (!got_position) {
        ERRORLOG("No random position in dungeon %d",(int)creatng->owner);
        set_start_state(creatng);
        return CrCkRet_Available;
    }
    if (!setup_person_move_to_position(creatng, pos.x.stl.num, pos.y.stl.num, 0))
    {
        WARNLOG("Can't navigate to subtile (%d,%d) for exploring",(int)pos.x.stl.num, (int)pos.y.stl.num);
        set_start_state(creatng);
        return CrCkRet_Available;
    }
    creatng->continue_state = CrSt_CreatureDoingNothing;
    return CrCkRet_Continue;
}

short creature_fired(struct Thing *creatng)
{
    struct Room *room;
    TRACE_THING(creatng);
    //return _DK_creature_fired(creatng);
    room = get_room_thing_is_on(creatng);
    if (!room_exists(room) || (room->kind != RoK_ENTRANCE))
    {
        set_start_state(creatng);
        return CrCkRet_Continue;
    }
    play_creature_sound_and_create_sound_thing(creatng, 4, 2);
    kill_creature(creatng, INVALID_THING, -1, 1, 0, 0);
    return CrCkRet_Deleted;
}

short creature_follow_leader(struct Thing *creatng)
{
    struct Thing *leadtng;
    long i;
    //return _DK_creature_follow_leader(creatng);
    leadtng = get_group_leader(creatng);
    if (!thing_is_invalid(leadtng) || (leadtng->index == creatng->index))
    {
        set_start_state(creatng);
        return 1;
    }
    struct CreatureControl *cctrl;
    cctrl = creature_control_get_from_thing(creatng);
    if ((cctrl->spell_flags & CSAfF_Unkn1000) != 0)
    {
        remove_creature_from_group(creatng);
        set_start_state(creatng);
        return 1;
    }
    struct Coord3d follwr_pos;
    TbBool found_pos;
    long group_len;
    found_pos = false;
    group_len = get_no_creatures_in_group(leadtng);
    if (group_len > 0)
    {
        struct CreatureControl *leadctrl;
        leadctrl = creature_control_get_from_thing(leadtng);
        for (i = 0; i < group_len; i++)
        {
            struct MemberPos *avail_pos;
            avail_pos = &leadctrl->followers_pos[i];
            if (((avail_pos->flags & 0x02) != 0) && ((avail_pos->flags & 0x01) == 0))
            {
                follwr_pos.x.val = subtile_coord_center(stl_num_decode_x(avail_pos->stl_num));
                follwr_pos.y.val = subtile_coord_center(stl_num_decode_y(avail_pos->stl_num));
                follwr_pos.z.val = 0;
                avail_pos->flags |= 0x01;
                found_pos = true;
                break;
            }
        }
    }
    if (!found_pos)
    {
        set_start_state(creatng);
        return 1;
    }
    int amount;
    amount = cctrl->field_307;
    if (amount > 8)
    {
        remove_creature_from_group(creatng);
        return 0;
    }
    if ((amount > 0) && (cctrl->field_303 + 16 > game.play_gameturn))
    {
        return 0;
    }
    cctrl->field_303 = game.play_gameturn;
    MapCoord dist;
    int speed;
    dist = get_2d_box_distance(&creatng->mappos, &follwr_pos);
    speed = get_creature_speed(leadtng);
    // If we're too far from the designated position, do a speed run
    if (dist > 1536)
    {
        speed = 3 * speed;
        if (speed >= 256)
            speed = 256;
        if ( creature_move_to(creatng, &follwr_pos, speed, 0, 0) == -1 )
        {
          cctrl->field_307++;
          return 0;
        }
    } else
    // If we're close, continue moving at normal speed
    if (dist <= 256)
    {
        if (dist <= 0)
        {
            creature_turn_to_face(creatng, &leadtng->mappos);
        } else
        if (creature_move_to(creatng, &follwr_pos, speed, 0, 0) == -1)
        {
            cctrl->field_307++;
            return 0;
        }
    } else
    // If we're in between, move just a bit faster than leader
    {
        speed = 5 * speed / 4;
        if (speed >= 256)
            speed = 256;
        if (creature_move_to(creatng, &follwr_pos, speed, 0, 0) == -1)
        {
            cctrl->field_307++;
            return 0;
        }
    }
    cctrl->field_307 = 0;
    return 0;
}

short creature_in_hold_audience(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    struct Coord3d pos;
    //return _DK_creature_in_hold_audience(creatng);
    int speed;
    speed = get_creature_speed(creatng);
    cctrl = creature_control_get_from_thing(creatng);
    if ((cctrl->field_82 == -1) && (cctrl->instance_id == CrInst_NULL))
    {
        struct Room *room;
        room = get_room_thing_is_on(creatng);
        if (room_is_invalid(room)) {
            return 1;
        }
        if (!find_random_valid_position_for_thing_in_room(creatng, room, &pos)) {
            return 1;
        }
        setup_person_move_to_position(creatng, coord_subtile(pos.x.val), coord_subtile(pos.y.val), 0);
        creatng->continue_state = CrSt_CreatureInHoldAudience;
        cctrl->field_82 = 0;
        return 1;
    }
    long ret;
    ret = creature_move_to(creatng, &cctrl->moveto_pos, speed, cctrl->field_88, 0);
    if (ret != 1)
    {
        if (ret == -1)
        {
            ERRORLOG("Hope we never get here...");
            set_start_state(creatng);
            return 0;
        }
        return 1;
    }
    if (cctrl->field_82 != 0)
    {
        if ((cctrl->field_82 != 1) || (cctrl->instance_id != CrInst_NULL))
            return 1;
        cctrl->field_82 = -1;
    } else
    {
        cctrl->field_82 = 1;
        set_creature_instance(creatng, CrInst_CELEBRATE_SHORT, 1, 0, 0);
    }
    return 1;
}

short creature_kill_creatures(struct Thing *creatng)
{
    struct Dungeon *dungeon;
    //return _DK_creature_kill_creatures(creatng);
    dungeon = get_dungeon(creatng->owner);
    if (dungeon->num_active_creatrs <= 1) {
        set_start_state(creatng);
        return 0;
    }
    struct Thing *thing;
    long crtr_idx;
    crtr_idx = ACTION_RANDOM(dungeon->num_active_creatrs);
    thing = get_player_list_nth_creature_of_model(dungeon->creatr_list_start, 0, crtr_idx);
    if (thing_is_invalid(thing)) {
        set_start_state(creatng);
        return 0;
    }
    if (setup_person_move_to_coord(creatng, &thing->mappos, 0)) {
        creatng->continue_state = CrSt_CreatureKillCreatures;
    }
    return 1;
}

short creature_leaves(struct Thing *creatng)
{
    struct Room *room;
    //return _DK_creature_leaves(creatng);
    room = get_room_thing_is_on(creatng);
    if (room_is_invalid(room) || (room->kind != RoK_ENTRANCE) || (room->owner != creatng->owner))
    {
        internal_set_thing_state(creatng, CrSt_CreatureLeavingDungeon);
        return 1;
    }
    if (!is_neutral_thing(creatng))
    {
        struct Dungeon *dungeon;
        dungeon = get_dungeon(creatng->owner);
        dungeon->total_creatures_left++;
        struct CreatureStats *crstat;
        crstat = creature_stats_get_from_thing(creatng);
        apply_anger_to_all_players_creatures_excluding(creatng->owner, crstat->annoy_others_leaving, 4, creatng);
    }
    kill_creature(creatng, INVALID_THING, -1, 1, 0, 0);
    return CrStRet_Deleted;
}

short creature_leaves_or_dies(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    struct Room *room;
    //return _DK_creature_leaves_or_dies(creatng);
    // If we're on an entrance, then just leave the dungeon
    room = get_room_thing_is_on(creatng);
    if (!room_is_invalid(room) && (room->kind == RoK_ENTRANCE))
    {
        kill_creature(creatng, INVALID_THING, -1, 1, 0, 0);
        return -1;
    }
    // Otherwise, try heading for nearest entrance
    room = find_nearest_room_for_thing(creatng, creatng->owner, 1, 0);
    if (room_is_invalid(room))
    {
        kill_creature(creatng, INVALID_THING, -1, 0, 0, 0);
        return -1;
    }
    if (!setup_random_head_for_room(creatng, room, 0))
    {
        kill_creature(creatng, INVALID_THING, -1, 0, 0, 0);
        return -1;
    }
    creatng->continue_state = CrSt_LeavesBecauseOwnerLost;
    cctrl = creature_control_get_from_thing(creatng);
    cctrl->flgfield_1 |= 0x02;
    return 1;
}

short creature_leaving_dungeon(struct Thing *creatng)
{
    struct Room *room;
    //return _DK_creature_leaving_dungeon(creatng);
    room = find_nearest_room_for_thing(creatng, creatng->owner, 1, 0);
    if (room_is_invalid(room))
    {
        set_start_state(creatng);
        return 0;
    }
    if (!setup_random_head_for_room(creatng, room, 0))
    {
        set_start_state(creatng);
        return 0;
    }
    if (is_my_player_number(creatng->owner))
        output_message(SMsg_CreatureLeaving, 500, 1);
    creatng->continue_state = CrSt_CreatureLeaves;
    return 1;
}

short creature_persuade(struct Thing *creatng)
{
    return _DK_creature_persuade(creatng);
}

void creature_drag_object(struct Thing *thing, struct Thing *dragtng)
{
    struct CreatureControl *cctrl;
    cctrl = creature_control_get_from_thing(thing);
    cctrl->dragtng_idx = dragtng->index;
    dragtng->alloc_flags |= TAlF_IsDragged;
    dragtng->field_1 |= TF1_IsDragged1;
    dragtng->owner = game.neutral_player_num;
    if (dragtng->light_id != 0) {
      light_turn_light_off(dragtng->light_id);
    }
}

TbBool find_random_valid_position_for_thing_in_room_avoiding_object(struct Thing *thing, const struct Room *room, struct Coord3d *pos)
{
    int nav_sizexy;
    long selected;
    unsigned long k;
    long i;
    //return _DK_find_random_valid_position_for_thing_in_room_avoiding_object(thing, room, pos);
    nav_sizexy = thing_nav_block_sizexy(thing) << 8;
    if (room_is_invalid(room) || (room->slabs_count <= 0)) {
        ERRORLOG("Invalid room or number of slabs is zero");
        return 0;
    }
    selected = ACTION_RANDOM(room->slabs_count);
    k = 0;
    i = room->slabs_list;
    // Get the selected index
    while (i != 0)
    {
        // Per room tile code
        if (k >= selected)
        {
            break;
        }
        // Per room tile code ends
        i = get_next_slab_number_in_room(i);
        k++;
    }
    // Sweep rooms starting on that index
    while (i != 0)
    {
        MapSubtlCoord stl_x,stl_y,start_stl;
        long n;
        stl_x = slab_subtile(slb_num_decode_x(i),0);
        stl_y = slab_subtile(slb_num_decode_y(i),0);
        // Per room tile code
        start_stl = ACTION_RANDOM(9);
        for (n=0; n < 9; n++)
        {
            MapSubtlCoord x,y;
            x = start_stl % 3 + stl_x;
            y = start_stl / 3 + stl_y;
            if (get_column_height_at(x, y) == 1)
            {
              if ( !find_base_thing_on_mapwho(TCls_Object, -1, x, y) )
              {
                pos->x.val = subtile_coord_center(x);
                pos->y.val = subtile_coord_center(y);
                pos->z.val = get_thing_height_at_with_radius(thing, pos, nav_sizexy);
                if ( !thing_in_wall_at_with_radius(thing, pos, nav_sizexy) )
                  return 1;
              }
            }
            start_stl = (start_stl + 1) % 9;
        }
        // Per room tile code ends
        if (k >= room->slabs_count)
        {
            k = 0;
            i = room->slabs_list;
        } else {
            k++;
            i = get_next_slab_number_in_room(i);
        }
    }
    ERRORLOG("Could not find valid RANDOM point in room for thing");
    return 0;
}

short creature_present_to_dungeon_heart(struct Thing *creatng)
{
    //return _DK_creature_present_to_dungeon_heart(creatng);
    create_effect(&creatng->mappos, imp_spangle_effects[creatng->owner], creatng->owner);
    thing_play_sample(creatng, 76, 100, 0, 3, 0, 2, 0x100);
    if ( !external_set_thing_state(creatng, CrSt_CreatureDoingNothing) )
      set_start_state(creatng);
    return 1;
}

short creature_pretend_chicken_move(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    long speed;
    //return _DK_creature_pretend_chicken_move(creatng);
    long move_ret;
    cctrl = creature_control_get_from_thing(creatng);
    if ((cctrl->affected_by_spells & CCSpl_Unknown01) != 0)
    {
        return 1;
    }
    speed = get_creature_speed(creatng);
    move_ret = creature_move_to(creatng, &cctrl->moveto_pos, speed, cctrl->field_88, 0);
    if (move_ret == 1)
    {
        internal_set_thing_state(creatng, CrSt_CreaturePretendChickenSetupMove);
    } else
    if (move_ret == -1)
    {
        internal_set_thing_state(creatng, CrSt_CreaturePretendChickenSetupMove);
    }
    return 1;
}

short creature_pretend_chicken_setup_move(struct Thing *creatng)
{
  return _DK_creature_pretend_chicken_setup_move(creatng);
}

/**
 * Gives a gold hoard in a room which can be accessed by given creature.
 * @param creatng The creature which should be able to access the object.
 * @param room Room in which the object lies.
 * @return The goald hoard thing, or invalid thing if not found.
 */
struct Thing *find_gold_hoarde_in_room_for_creature(struct Thing *creatng, struct Room *room)
{
    return find_object_in_room_for_creature_matching_bool_filter(creatng, room, thing_is_gold_hoard);
}

/**
 * Gives a spellbook in a room which can be accessed by given creature.
 * @param creatng The creature which should be able to access the spellbook.
 * @param room Room in which the spellbook lies.
 * @return The spellbook thing, or invalid thing if not found.
 */
struct Thing *find_spell_in_room_for_creature(struct Thing *creatng, struct Room *room)
{
    return find_object_in_room_for_creature_matching_bool_filter(creatng, room, thing_is_spellbook);
}

/**
 * State handler function for stealing gold.
 * Should be invoked when a thief arrives at enemy treasure room.
 * Searches for specific gold hoard to steal from and enters next
 * phase of stealing (CrSt_CreatureStealGold).
 * @param thing The creature who is stealing gold.
 * @return True on success, false if finding gold to steal failed.
 */
short creature_search_for_gold_to_steal_in_room(struct Thing *creatng)
{
    struct SlabMap *slb;
    struct Room *room;
    struct Thing *gldtng;
    //return _DK_creature_search_for_gold_to_steal_in_room(thing);
    slb = get_slabmap_for_subtile(creatng->mappos.x.stl.num,creatng->mappos.y.stl.num);
    room = room_get(slb->room_index);
    if (room_is_invalid(room) || (room->kind != RoK_TREASURE))
    {
        WARNLOG("Cannot steal gold - not on treasure room at (%d,%d)",(int)creatng->mappos.x.stl.num, (int)creatng->mappos.y.stl.num);
        set_start_state(creatng);
        return 0;
    }
    gldtng = find_gold_hoarde_in_room_for_creature(creatng, room);
    if (thing_is_invalid(gldtng))
    {
        WARNLOG("Cannot steal gold - no gold hoard found in treasure room");
        set_start_state(creatng);
        return 0;
    }
    if (!setup_person_move_to_position(creatng, gldtng->mappos.x.stl.num, gldtng->mappos.y.stl.num, 0))
    {
        SYNCDBG(8,"Cannot move to gold at (%d,%d)",(int)gldtng->mappos.x.stl.num, (int)gldtng->mappos.y.stl.num);
    }
    creatng->continue_state = CrSt_CreatureStealGold;
    return 1;
}

short creature_search_for_spell_to_steal_in_room(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    struct SlabMap *slb;
    struct Room *room;
    struct Thing *spltng;
    cctrl = creature_control_get_from_thing(creatng);
    slb = get_slabmap_for_subtile(creatng->mappos.x.stl.num,creatng->mappos.y.stl.num);
    room = room_get(slb->room_index);
    if (room_is_invalid(room) || (room->kind != RoK_LIBRARY))
    {
        WARNLOG("Cannot steal spell - not on library at (%d,%d)",(int)creatng->mappos.x.stl.num, (int)creatng->mappos.y.stl.num);
        set_start_state(creatng);
        return 0;
    }
    spltng = find_spell_in_room_for_creature(creatng, room);
    if (thing_is_invalid(spltng))
    {
        WARNLOG("Cannot steal spell - no spellbook found in library");
        set_start_state(creatng);
        return 0;
    }
    cctrl->pickup_object_id = spltng->index;
    if (!setup_person_move_to_position(creatng, spltng->mappos.x.stl.num, spltng->mappos.y.stl.num, 0))
    {
        SYNCDBG(8,"Cannot move to spell at (%d,%d)",(int)spltng->mappos.x.stl.num, (int)spltng->mappos.y.stl.num);
    }
    creatng->continue_state = CrSt_CreatureStealSpell;
    return 1;
}

short creature_set_work_room_based_on_position(struct Thing *creatng)
{
    //return _DK_creature_set_work_room_based_on_position(thing);
    return 1;
}

short creature_slap_cowers(struct Thing *creatng)
{
  return _DK_creature_slap_cowers(creatng);
}

short creature_steal_gold(struct Thing *creatng)
{
    struct CreatureStats *crstat;
    struct Room *room;
    struct Thing *hrdtng;
    long max_amount,amount;
    //return _DK_creature_steal_gold(thing);
    crstat = creature_stats_get_from_thing(creatng);
    room = get_room_thing_is_on(creatng);
    if (room_is_invalid(room) || (room->kind != RoK_TREASURE))
    {
        WARNLOG("Cannot steal gold - not on treasure room at (%d,%d)",(int)creatng->mappos.x.stl.num, (int)creatng->mappos.y.stl.num);
        set_start_state(creatng);
        return 0;
    }
    hrdtng = find_gold_hoard_at(creatng->mappos.x.stl.num, creatng->mappos.y.stl.num);
    if (thing_is_invalid(hrdtng))
    {
        WARNLOG("Cannot steal gold - no gold hoard at (%d,%d)",(int)creatng->mappos.x.stl.num, (int)creatng->mappos.y.stl.num);
        set_start_state(creatng);
        return 0;
    }
    max_amount = crstat->gold_hold - creatng->creature.gold_carried;
    if (max_amount <= 0)
    {
        set_start_state(creatng);
        return 0;
    }
    // Success! we are able to steal some gold!
    amount = remove_gold_from_hoarde(hrdtng, room, max_amount);
    creatng->creature.gold_carried += amount;
    create_price_effect(&creatng->mappos, creatng->owner, amount);
    SYNCDBG(6,"Stolen %ld gold from hoard at (%d,%d)",amount,(int)creatng->mappos.x.stl.num, (int)creatng->mappos.y.stl.num);
    set_start_state(creatng);
    return 0;
}

short creature_pick_up_spell_to_steal(struct Thing *creatng)
{
    struct Room *room;
    struct CreatureControl *cctrl;
    struct Thing *spelltng;
    struct Coord3d pos;
    TRACE_THING(creatng);
    cctrl = creature_control_get_from_thing(creatng);
    spelltng = thing_get(cctrl->pickup_object_id);
    TRACE_THING(spelltng);
    if ( thing_is_invalid(spelltng) || ((spelltng->field_1 & TF1_IsDragged1) != 0)
      || (get_2d_box_distance(&creatng->mappos, &spelltng->mappos) >= 512))
    {
        set_start_state(creatng);
        return 0;
    }
    room = get_room_thing_is_on(spelltng);
    // Check if we're stealing the spell from a library
    if (!room_is_invalid(room))
    {
        remove_spell_from_library(room, spelltng, creatng->owner);
    }
    pos.x.val = 0;
    pos.y.val = 0;
    //TODO STEAL_SPELLS write the spell stealing code - find hero gate to escape
    SYNCLOG("Stealing spells not implemented - reset");
    set_start_state(creatng);
    return 0;
/*
    creature_drag_object(thing, spelltng);
    if (!setup_person_move_to_position(thing, pos.x.stl.num, pos.y.stl.num, 0))
    {
        SYNCDBG(8,"Cannot move to (%d,%d)",(int)pos.x.stl.num, (int)pos.y.stl.num);
    }
    thing->continue_state = CrSt_GoodReturnsToStart;
    return 1;
*/
}

short creature_take_salary(struct Thing *creatng)
{
    struct Dungeon *dungeon;
    struct Room *room;
    TRACE_THING(creatng);
    SYNCDBG(18,"Starting");
    //return _DK_creature_take_salary(creatng);
    if (!thing_is_on_own_room_tile(creatng))
    {
        internal_set_thing_state(creatng, CrSt_CreatureWantsSalary);
        return 1;
    }
    room = get_room_thing_is_on(creatng);
    dungeon = get_dungeon(creatng->owner);
    if (room_is_invalid(room) || (room->kind != RoK_TREASURE) ||
      ((room->used_capacity <= 0) && (dungeon->offmap_money_owned <= 0)))
    {
        internal_set_thing_state(creatng, CrSt_CreatureWantsSalary);
        return 1;
    }
    long salary;
    salary = calculate_correct_creature_pay(creatng);
    take_money_from_dungeon(creatng->owner, salary, 0);
    {
        struct CreatureControl *cctrl;
        cctrl = creature_control_get_from_thing(creatng);
        if (cctrl->field_48 > 0)
            cctrl->field_48--;
    }
    set_start_state(creatng);
    {
        struct Thing *efftng;
        efftng = create_effect_element(&creatng->mappos, 0x29, creatng->owner);
        if (!thing_is_invalid(efftng))
        {
            efftng->long_13 = salary;
            thing_play_sample(efftng, 32, 100, 0, 3, 0, 2, 256);
        }
    }
    dungeon->lvstats.salary_cost += salary;
    return 1;
}

void make_creature_conscious(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    TRACE_THING(creatng);
    SYNCDBG(18,"Starting");
    cctrl = creature_control_get_from_thing(creatng);
    cctrl->flgfield_1 &= ~CCFlg_Immortal;
    cctrl->flgfield_1 &= ~CCFlg_NoCompControl;
    cctrl->conscious_back_turns = 0;
    if ((creatng->field_1 & TF1_IsDragged1) != 0)
    {
        struct Thing *sectng;
        struct CreatureControl *secctrl;
        sectng = thing_get(cctrl->dragtng_idx);
        TRACE_THING(sectng);
        secctrl = creature_control_get_from_thing(sectng);
        if (!creature_control_invalid(secctrl)) {
            if (secctrl->dragtng_idx <= 0) {
                WARNLOG("The %s is not dragging something",thing_model_name(sectng));
            }
            secctrl->dragtng_idx = 0;
        } else {
            ERRORLOG("The %s has no valid control structure",thing_model_name(sectng));
        }
        if (cctrl->dragtng_idx <= 0) {
            WARNLOG("The %s is not dragged by something",thing_model_name(creatng));
        }
        creatng->field_1 &= ~TF1_IsDragged1;
        cctrl->dragtng_idx = 0;
    }
    set_start_state(creatng);
}

short creature_unconscious(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    TRACE_THING(creatng);
    SYNCDBG(18,"Starting");
    //return _DK_creature_unconscious(creatng);
    cctrl = creature_control_get_from_thing(creatng);
    if (cctrl->conscious_back_turns > 0)
        cctrl->conscious_back_turns--;
    if (cctrl->conscious_back_turns > 0)
    {
        return 0;
    }
    make_creature_conscious(creatng);
    return 1;
}

short creature_vandalise_rooms(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    TRACE_THING(creatng);
    SYNCDBG(18,"Starting");
    //return _DK_creature_vandalise_rooms(creatng);
    cctrl = creature_control_get_from_thing(creatng);
    cctrl->target_room_id = 0;
    struct Room *room;
    room = get_room_thing_is_on(creatng);
    if (room_is_invalid(room))
    {
      set_start_state(creatng);
      return 0;
    }
    if (room_cannot_vandalize(room->kind))
    {
        return 1;
    }
    if (cctrl->instance_id == CrInst_NULL)
    {
        set_creature_instance(creatng, CrInst_ATTACK_ROOM_SLAB, 1, 0, 0);
    }
    return 1;
}

short creature_wait_at_treasure_room_door(struct Thing *creatng)
{
  return _DK_creature_wait_at_treasure_room_door(creatng);
}

short creature_wants_a_home(struct Thing *creatng)
{
  return _DK_creature_wants_a_home(creatng);
}

short creature_wants_salary(struct Thing *creatng)
{
  return _DK_creature_wants_salary(creatng);
}

long setup_head_for_empty_treasure_space(struct Thing *thing, struct Room *room)
{
    return _DK_setup_head_for_empty_treasure_space(thing, room);
}

long setup_random_head_for_room(struct Thing *thing, struct Room *room, unsigned char a3)
{
  return _DK_setup_random_head_for_room(thing, room, a3);
}

struct Room * find_nearest_room_for_thing(struct Thing *thing, char a2, char a3, unsigned char a4)
{
    return _DK_find_nearest_room_for_thing(thing, a2, a3, a4);
}

struct Room *find_nearest_room_for_thing_excluding_two_types(struct Thing *thing, char owner, char a3, char a4, unsigned char a5)
{
    return _DK_find_nearest_room_for_thing_excluding_two_types(thing, owner, a3, a4, a5);
}

void place_thing_in_creature_controlled_limbo(struct Thing *thing)
{
    remove_thing_from_mapwho(thing);
    thing->field_4F |= 0x01;
    thing->field_1 |= TF1_InCtrldLimbo;
}

void remove_thing_from_creature_controlled_limbo(struct Thing *thing)
{
    thing->field_1 &= ~TF1_InCtrldLimbo;
    thing->field_4F &= ~0x01;
    place_thing_in_mapwho(thing);
}

short move_backwards_to_position(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    long i,speed;
    //return _DK_move_backwards_to_position(thing);
    cctrl = creature_control_get_from_thing(creatng);
    speed = get_creature_speed(creatng);
    i = creature_move_to_using_gates(creatng, &cctrl->moveto_pos, speed, -2, cctrl->field_88, 1);
    if (i == 1)
    {
        internal_set_thing_state(creatng, creatng->continue_state);
        creatng->continue_state = 0;
        return 1;
    }
    if (i == -1)
    {
        ERRORLOG("Bad place (%d,%d) to move %s backwards to.",(int)cctrl->moveto_pos.x.val,(int)cctrl->moveto_pos.y.val,thing_model_name(creatng));
        set_start_state(creatng);
        creatng->continue_state = 0;
        return 0;
    }
    return 0;
}

CrCheckRet move_check_attack_any_door(struct Thing *creatng)
{
  return _DK_move_check_attack_any_door(creatng);
}

CrCheckRet move_check_can_damage_wall(struct Thing *creatng)
{
  return _DK_move_check_can_damage_wall(creatng);
}

long creature_can_have_combat_with_creature_on_slab(const struct Thing *creatng, MapSlabCoord slb_x, MapSlabCoord slb_y, struct Thing ** enemytng)
{
    struct Map *map;
    MapSubtlCoord endstl_x,endstl_y;
    MapSubtlCoord stl_x,stl_y;
    long dist;
    endstl_x = slab_subtile(slb_x+1,0);
    endstl_y = slab_subtile(slb_y+1,0);
    for (stl_y = slab_subtile(slb_y,0); stl_y < endstl_y; stl_y++)
    {
        for (stl_x = slab_subtile(slb_x,0); stl_x < endstl_x; stl_x++)
        {
            struct Thing *thing;
            long can_combat;
            long i;
            unsigned long k;
            map = get_map_block_at(stl_x,stl_y);
            k = 0;
            i = get_mapwho_thing_index(map);
            while (i != 0)
            {
                thing = thing_get(i);
                TRACE_THING(thing);
                if (thing_is_invalid(thing))
                {
                    ERRORLOG("Jump to invalid thing detected");
                    break;
                }
                i = thing->next_on_mapblk;
                // Per thing code start
                if ( thing_is_creature(thing) && (thing != creatng) )
                {
                    if ((get_creature_model_flags(thing) & MF_IsSpecDigger) == 0)
                    {
                        dist = get_combat_distance(creatng, thing);
                        can_combat = creature_can_have_combat_with_creature(creatng, thing, dist, 0, 0);
                        if (can_combat > 0) {
                            (*enemytng) = thing;
                            return can_combat;
                        }
                    }
                }
                // Per thing code end
                k++;
                if (k > THINGS_COUNT)
                {
                    ERRORLOG("Infinite loop detected when sweeping things list");
                    break;
                }
            }
        }
    }
    (*enemytng) = INVALID_THING;
    return 0;
}

CrCheckRet move_check_kill_creatures(struct Thing *creatng)
{
    struct Thing * enemytng;
    MapSlabCoord slb_x,slb_y;
    long can_combat;
    //return _DK_move_check_kill_creatures(thing);
    slb_x = creatng->mappos.x.stl.num/3;
    slb_y = creatng->mappos.y.stl.num/3;
    can_combat = creature_can_have_combat_with_creature_on_slab(creatng, slb_x, slb_y, &enemytng);
    if (can_combat > 0) {
        set_creature_in_combat_to_the_death(creatng, enemytng, can_combat);
        return CrCkRet_Continue;
    }
    return CrCkRet_Available;
}

CrCheckRet move_check_near_dungeon_heart(struct Thing *creatng)
{
  return _DK_move_check_near_dungeon_heart(creatng);
}

CrCheckRet move_check_on_head_for_room(struct Thing *creatng)
{
  return _DK_move_check_on_head_for_room(creatng);
}

CrCheckRet move_check_persuade(struct Thing *creatng)
{
  return _DK_move_check_persuade(creatng);
}

CrCheckRet move_check_wait_at_door_for_wage(struct Thing *creatng)
{
  return _DK_move_check_wait_at_door_for_wage(creatng);
}

char new_slab_tunneller_check_for_breaches(struct Thing *creatng)
{
  return _DK_new_slab_tunneller_check_for_breaches(creatng);
}

short patrol_here(struct Thing *creatng)
{
  return _DK_patrol_here(creatng);
}

short patrolling(struct Thing *creatng)
{
  return _DK_patrolling(creatng);
}

short person_sulk_at_lair(struct Thing *creatng)
{
  return _DK_person_sulk_at_lair(creatng);
}

short person_sulk_head_for_lair(struct Thing *creatng)
{
  return _DK_person_sulk_head_for_lair(creatng);
}

short person_sulking(struct Thing *creatng)
{
  return _DK_person_sulking(creatng);
}

/**
 * Returns if the room is a valid place for a thing which hasn't yet started working in it.
 * Used to check if creatures can start working in specific rooms.
 * @param room The work room to be checked.
 * @param rkind Room kind required for work.
 * @param thing The thing which seeks for work room.
 * @return True if the room can be used, false otherwise.
 */
TbBool room_initially_valid_as_type_for_thing(const struct Room *room, RoomKind rkind, const struct Thing *thing)
{
    if (!room_exists(room))
        return false;
    if (room->kind != rkind)
        return false;
    return ((room->owner == thing->owner) || enemies_may_work_in_room(room->kind));
}

/**
 * Returns if the room is a valid place for a thing for thing which is already working in that room.
 * Used to check if creatures are working in correct rooms.
 * @param room The work room to be checked.
 * @param rkind Room kind required for work.
 * @param thing The thing which is working in the room.
 * @return True if the room can still be used, false otherwise.
 */
TbBool room_still_valid_as_type_for_thing(const struct Room *room, RoomKind rkind, const struct Thing *thing)
{
    if (!room_exists(room))
        return false;
    if (room->kind != rkind)
        return false;
    return ((room->owner == thing->owner) || enemies_may_work_in_room(room->kind));
}

/**
 * Returns if it's no longer possible for a creature to work in given room.
 * Used to check if creatures are able to continue working in the rooms they're working.
 * @param room The work room to be checked, usually the one creature stands on.
 * @param rkind Room kind required for work.
 * @param thing The thing which is working in the room.
 * @return True if the room can still be used, false otherwise.
 */
TbBool creature_work_in_room_no_longer_possible_f(const struct Room *room, RoomKind rkind, const struct Thing *thing, const char *func_name)
{
    if (!room_exists(room))
    {
        SYNCLOG("%s: The %s can no longer work in %s because former work room doesn't exist",thing_model_name(thing),room_code_name(rkind));
        // Note that if given room doesn't exist, it do not mean this
        return true;
    }
    if (!room_still_valid_as_type_for_thing(room, rkind, thing))
    {
        WARNLOG("%s: Room %s index %d is not valid %s for %s to work in",func_name,room_code_name(room->kind),(int)room->index,room_code_name(rkind),thing_model_name(thing));
        return true;
    }
    if (!creature_is_working_in_room(thing, room))
    {
        WARNLOG("%s: Room %s index %d is not the %s which %s selected to work in",func_name,room_code_name(room->kind),(int)room->index,room_code_name(rkind),thing_model_name(thing));
        return true;
    }
    return false;
}

void create_effect_around_thing(struct Thing *thing, long eff_kind)
{
  _DK_create_effect_around_thing(thing, eff_kind);
}

void remove_health_from_thing_and_display_health(struct Thing *thing, long delta)
{
  _DK_remove_health_from_thing_and_display_health(thing, delta);
}

long slab_by_players_land(long plyr_idx, MapSlabCoord slb_x, MapSlabCoord slb_y)
{
  return _DK_slab_by_players_land(plyr_idx, slb_x, slb_y);
}

TbBool process_creature_hunger(struct Thing *thing)
{
    struct CreatureControl *cctrl;
    struct CreatureStats *crstat;
    cctrl = creature_control_get_from_thing(thing);
    crstat = creature_stats_get_from_thing(thing);
    if ( (crstat->hunger_rate == 0) || creature_affected_by_spell(thing, SplK_Freeze) )
        return false;
    cctrl->hunger_level++;
    if (cctrl->hunger_level <= crstat->hunger_rate)
        return false;
    if ((game.play_gameturn % game.turns_per_hunger_health_loss) == 0)
        remove_health_from_thing_and_display_health(thing, game.hunger_health_loss);
    return true;
}

TbBool creature_will_attack_creature(const struct Thing *tng1, const struct Thing *tng2)
{
    struct CreatureControl *cctrl1;
    struct CreatureControl *cctrl2;
    struct PlayerInfo *player1;
    struct PlayerInfo *player2;
    struct CreatureStats *crstat1;
    struct Thing *tmptng;

    cctrl1 = creature_control_get_from_thing(tng1);
    cctrl2 = creature_control_get_from_thing(tng2);
    player1 = get_player(tng1->owner);
    player2 = get_player(tng2->owner);

    if ((tng2->owner != tng1->owner) && (tng2->owner != game.neutral_player_num))
    {
       if ((tng1->owner == game.neutral_player_num) || (tng2->owner == game.neutral_player_num)
        || (!player_allied_with(player1, tng2->owner)))
          return true;
       if ((tng2->owner == game.neutral_player_num) || (tng1->owner == game.neutral_player_num)
        || (!player_allied_with(player2, tng1->owner)))
          return true;
    }

    tmptng = thing_get(cctrl1->battle_enemy_idx);
    TRACE_THING(tmptng);
    if  ( (cctrl1->spell_flags & CSAfF_Unkn1000) || (cctrl2->spell_flags & CSAfF_Unkn1000)
        || ((cctrl1->combat_flags) && (tmptng == tng2)) )
    {
        if (tng2 != tng1)
        {
            if ((creature_control_exists(cctrl2)) && ((cctrl2->flgfield_1 & CCFlg_NoCompControl) == 0)
            && ((tng2->alloc_flags & TAlF_IsInLimbo) == 0) && ((tng2->field_1 & TF1_InCtrldLimbo) == 0))
            {
                crstat1 = creature_stats_get_from_thing(tng1);
                if (!creature_affected_by_spell(tng2, SplK_Invisibility))
                    return true;
                if (cctrl2->field_AF > 0)
                    return true;
                if (crstat1->can_see_invisible || creature_affected_by_spell(tng1, SplK_Sight))
                    return true;
            }
        }
    }
    return false;
}

struct Thing *thing_update_enemy_to_fight_with(struct Thing *thing)
{
    struct CreatureControl *cctrl;
    struct Thing *enemytng;
    TRACE_THING(thing);
    cctrl = creature_control_get_from_thing(thing);
    if (cctrl->word_9A != 0)
    {
        enemytng = thing_get(cctrl->word_9A);
        TRACE_THING(enemytng);
        if (((enemytng->alloc_flags & TAlF_Exists) == 0) || (cctrl->long_9C != enemytng->field_9))
        {
          enemytng = NULL;
          cctrl->long_9C = 0;
          cctrl->word_9A = 0;
        }
    } else
    {
        enemytng = NULL;
    }
    if (game.play_gameturn - cctrl->long_A0 > 64)
    {
        cctrl->long_A0 = game.play_gameturn;
        enemytng = find_nearest_enemy_creature(thing);
    }
    if (thing_is_invalid(enemytng))
    {
        cctrl->word_9A = 0;
        return NULL;
    }
    cctrl->word_9A = enemytng->index;
    cctrl->long_9C = enemytng->field_9;
    return enemytng;
}

TbBool wander_point_get_random_pos(struct Wander *wandr, struct Coord3d *pos)
{
  long irnd;
  if ( wandr->field_0 )
  {
    irnd = ACTION_RANDOM(wandr->field_0);
    pos->x.val = wandr->field_18[2*irnd] << 8;
    pos->y.val = wandr->field_18[2*irnd + 1] << 8;
    return true;
  }
  return false;
}

TbBool get_random_position_in_dungeon_for_creature(long plyr_idx, unsigned char a2, struct Thing *thing, struct Coord3d *pos)
{
    struct PlayerInfo *player;
    if (plyr_idx == game.neutral_player_num)
    {
      ERRORLOG("Attempt to get random position in neutral dungeon");
      return false;
    }
    player = get_player(plyr_idx);
    if (player_invalid(player))
    {
      ERRORLOG("Attempt to get random position in invalid dungeon");
      return false;
    }
    if ( a2 )
    {
      if (!wander_point_get_random_pos(&player->wandr1, pos))
        return false;
    } else
    {
      if (!wander_point_get_random_pos(&player->wandr2, pos))
        return false;
    }
    return true;
}

TbBool creature_can_hear_within_distance(struct Thing *thing, long dist)
{
    struct CreatureStats *crstat;
    crstat = creature_stats_get_from_thing(thing);
    return (crstat->hearing) >= (dist/256);
}

/**
 * Enemy seeking function for creatures and heroes.
 * Used for performing SEEK_THE_ENEMY job.
 * @param thing The creature to seek the enemy for.
 * @return
 */
short seek_the_enemy(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    struct Thing *enemytng;
    struct Coord3d pos;
    long dist;
    //return _DK_seek_the_enemy(thing);
    cctrl = creature_control_get_from_thing(creatng);
    enemytng = thing_update_enemy_to_fight_with(creatng);
    if (!thing_is_invalid(enemytng))
    {
        dist = get_2d_box_distance(&enemytng->mappos, &creatng->mappos);
        if (creature_can_hear_within_distance(creatng, dist))
        {
            if (cctrl->instance_id == CrInst_NULL)
            {
              if ((dist < 2304) && (game.play_gameturn-cctrl->field_282 < 20))
              {
                set_creature_instance(creatng, CrInst_CELEBRATE_SHORT, 1, 0, 0);
                thing_play_sample(creatng, 168+UNSYNC_RANDOM(3), 100, 0, 3, 0, 2, 256);
                return 1;
              }
              if (ACTION_RANDOM(4) != 0)
              {
                  if (setup_person_move_close_to_position(creatng, enemytng->mappos.x.stl.num, enemytng->mappos.y.stl.num, 0) )
                  {
                    creatng->continue_state = CrSt_SeekTheEnemy;
                    cctrl->field_282 = game.play_gameturn;
                    return 1;
                  }
              }
              if (creature_choose_random_destination_on_valid_adjacent_slab(creatng))
              {
                  creatng->continue_state = CrSt_SeekTheEnemy;
                  cctrl->field_282 = game.play_gameturn;
              }
            }
            return 1;
        }
        if (ACTION_RANDOM(64) == 0)
        {
            if (setup_person_move_close_to_position(creatng, enemytng->mappos.x.stl.num, enemytng->mappos.y.stl.num, 0))
            {
              creatng->continue_state = CrSt_SeekTheEnemy;
            }
        }
    }
    // No enemy found - do some random movement
    if (ACTION_RANDOM(12) != 0)
    {
        if ( creature_choose_random_destination_on_valid_adjacent_slab(creatng) )
        {
            creatng->continue_state = CrSt_SeekTheEnemy;
            return 1;
        }
    } else
    if (get_random_position_in_dungeon_for_creature(creatng->owner, 1, creatng, &pos))
    {
        if ( setup_person_move_to_position(creatng, pos.x.val >> 8, pos.y.val >> 8, 0) )
        {
            creatng->continue_state = CrSt_SeekTheEnemy;
        }
        return 1;
    }
    set_start_state(creatng);
    return 1;
}

short state_cleanup_dragging_body(struct Thing *creatng)
{
  return _DK_state_cleanup_dragging_body(creatng);
}

short state_cleanup_dragging_object(struct Thing *creatng)
{
  return _DK_state_cleanup_dragging_object(creatng);
}

short state_cleanup_in_room(struct Thing *creatng)
{
  return _DK_state_cleanup_in_room(creatng);
}

short state_cleanup_unable_to_fight(struct Thing *creatng)
{
    struct CreatureControl *cctrl;
    //return _DK_state_cleanup_unable_to_fight(thing);
    cctrl = creature_control_get_from_thing(creatng);
    cctrl->flgfield_1 &= ~CCFlg_NoCompControl;
    return 1;
}

short state_cleanup_unconscious(struct Thing *creatng)
{
  return _DK_state_cleanup_unconscious(creatng);
}

long process_work_speed_on_work_value(struct Thing *thing, long base_val)
{
    struct Dungeon *dungeon;
    struct CreatureControl *cctrl;
    long val;
    cctrl = creature_control_get_from_thing(thing);
    val = base_val;
    if (creature_affected_by_spell(thing, SplK_Speed))
        val = 2 * val;
    if (cctrl->field_21)
        val = 4 * val / 3;
    if (!is_neutral_thing(thing))
    {
        dungeon = get_dungeon(thing->owner);
        if (dungeon->tortured_creatures[thing->model] > 0)
            val = 4 * val / 3;
        if (dungeon->field_888)
            val = 6 * val / 5;
    }
    return val;
}

TbBool check_experience_upgrade(struct Thing *thing)
{
    struct Dungeon *dungeon;
    struct CreatureStats *crstat;
    struct CreatureControl *cctrl;
    long i;
    dungeon = get_dungeon(thing->owner);
    cctrl = creature_control_get_from_thing(thing);
    crstat = creature_stats_get_from_thing(thing);
    i = crstat->to_level[cctrl->explevel] << 8;
    if (cctrl->exp_points < i)
        return false;
    cctrl->exp_points -= i;
    if (cctrl->explevel < dungeon->creature_max_level[thing->model])
    {
      if ((cctrl->explevel < CREATURE_MAX_LEVEL-1) || (crstat->grow_up != 0))
        cctrl->spell_flags |= CSAfF_Unkn4000;
    }
    return true;
}
/******************************************************************************/
TbBool internal_set_thing_state(struct Thing *thing, CrtrStateId nState)
{
  struct CreatureControl *cctrl;
  thing->active_state = nState;
  set_flag_byte(&thing->field_1, 0x10, false);
  thing->continue_state = CrSt_Unused;
  cctrl = creature_control_get_from_thing(thing);
  cctrl->field_302 = 0;
  clear_creature_instance(thing);
  return true;
}

TbBool initialise_thing_state(struct Thing *thing, CrtrStateId nState)
{
    struct CreatureControl *cctrl;
    //return _DK_initialise_thing_state(thing, nState);
    TRACE_THING(thing);
    SYNCDBG(9,"State change %s to %s for %s index %d",creature_state_code_name(thing->active_state), creature_state_code_name(nState), thing_model_name(thing),(int)thing->index);
    cleanup_current_thing_state(thing);
    thing->continue_state = CrSt_Unused;
    thing->active_state = nState;
    thing->field_1 &= ~TF1_Unkn10;
    cctrl = creature_control_get_from_thing(thing);
    if (creature_control_invalid(cctrl))
    {
        ERRORLOG("The %s index %d has invalid control",thing_model_name(thing),(int)thing->index);
        return false;
    }
    cctrl->target_room_id = 0;
    cctrl->field_302 = 0;
    if ((cctrl->flgfield_1 & CCFlg_IsInRoomList) != 0)
    {
        WARNLOG("The %s stays in room list even after cleanup",thing_model_name(thing));
        remove_creature_from_work_room(thing);
    }
    return true;
}

TbBool set_creature_assigned_job(struct Thing *thing, CreatureJob new_job)
{
    struct CreatureControl *cctrl;
    TRACE_THING(thing);
    cctrl = creature_control_get_from_thing(thing);
    if (creature_control_invalid(cctrl))
    {
        ERRORLOG("The %s index %d has invalid control",thing_model_name(thing),(int)thing->index);
        return false;
    }
    cctrl->job_assigned = new_job;
    return true;
}

TbBool cleanup_current_thing_state(struct Thing *thing)
{
    struct StateInfo *stati;
    CreatureStateFunc1 cleanup_cb;
    stati = get_creature_state_with_task_completion(thing);
    cleanup_cb = stati->cleanup_state;
    if (cleanup_cb != NULL)
    {
        cleanup_cb(thing);
        thing->field_1 |= TF1_Unkn10;
    } else
    {
        clear_creature_instance(thing);
    }
    return true;
}

TbBool cleanup_creature_state_and_interactions(struct Thing *thing)
{
    cleanup_current_thing_state(thing);
    set_creature_assigned_job(thing,Job_NULL);
    remove_all_traces_of_combat(thing);
    if (creature_is_group_member(thing)) {
        remove_creature_from_group(thing);
    }
    remove_events_thing_is_attached_to(thing);
    delete_effects_attached_to_creature(thing);
    return true;
}

TbBool can_change_from_state_to(const struct Thing *thing, CrtrStateId curr_state, CrtrStateId next_state)
{
    struct StateInfo *next_stati;
    struct StateInfo *curr_stati;
    curr_stati = get_thing_state_info_num(curr_state);
    if (curr_stati->state_type == 6)
      curr_stati = get_thing_state_info_num(thing->continue_state);
    next_stati = get_thing_state_info_num(next_state);
    if ((curr_stati->field_20) && (!next_stati->field_16))
        return false;
    if ((curr_stati->field_1F) && (!next_stati->field_15))
        return false;
    switch (curr_stati->state_type)
    {
    case 2:
        if ( next_stati->field_11 )
            return true;
        break;
    case 3:
        if ( next_stati->field_12 )
            return true;
        break;
    case 4:
        if ( next_stati->field_10 )
            return true;
        break;
    case 5:
        if ( next_stati->field_13 )
            return true;
        break;
    case 7:
        if ( next_stati->field_14 )
            return true;
        break;
    case 8:
        if ( next_stati->field_17 )
            return true;
        break;
    case 9:
        if ( next_stati->field_18 )
            return true;
        break;
    case 10:
        if ( next_stati->field_19 )
            return true;
        break;
    case 11:
        if ( next_stati->field_1B )
            return true;
        break;
    case 12:
        if ( next_stati->field_1A )
            return true;
        break;
    case 13:
        if ( next_stati->field_1C )
            return true;
        break;
    case 14:
        if ( next_stati->field_1D )
            return true;
        break;
    default:
        return true;
    }
    return false;
}

short set_start_state_f(struct Thing *thing,const char *func_name)
{
    struct PlayerInfo *player;
    long i;
    SYNCDBG(8,"%s: Starting for %s index %d, owner %d, last state %s, stacked %s",func_name,thing_model_name(thing),
        (int)thing->index,(int)thing->owner,creature_state_code_name(thing->active_state),creature_state_code_name(thing->continue_state));
//    return _DK_set_start_state(thing);
    if ((thing->alloc_flags & TAlF_IsControlled) != 0)
    {
      cleanup_current_thing_state(thing);
      initialise_thing_state(thing, CrSt_ManualControl);
      return thing->active_state;
    }
    if (is_neutral_thing(thing))
    {
      cleanup_current_thing_state(thing);
      initialise_thing_state(thing, CrSt_CreatureDormant);
      return thing->active_state;
    }
    if (thing->owner == game.hero_player_num)
    {
        i = creatures[thing->model%CREATURE_TYPES_COUNT].good_start_state;
        cleanup_current_thing_state(thing);
        initialise_thing_state(thing, i);
        return thing->active_state;
    }
    player = get_player(thing->owner);
    if (player->victory_state == VicS_LostLevel)
    {
        cleanup_current_thing_state(thing);
        initialise_thing_state(thing, CrSt_LeavesBecauseOwnerLost);
        return thing->active_state;
    }
    if (creature_affected_by_spell(thing, SplK_Chicken))
    {
        cleanup_current_thing_state(thing);
        initialise_thing_state(thing, CrSt_CreaturePretendChickenSetupMove);
        return thing->active_state;
    }
    i = creatures[thing->model%CREATURE_TYPES_COUNT].evil_start_state;
    initialise_thing_state(thing, i);
    return thing->active_state;
}

TbBool external_set_thing_state(struct Thing *thing, CrtrStateId state)
{
    //return _DK_external_set_thing_state(thing, state);
    if ( !can_change_from_state_to(thing, thing->active_state, state) )
    {
        ERRORDBG(4,"State change %s to %s for %s not allowed",creature_state_code_name(thing->active_state),
            creature_state_code_name(state), thing_model_name(thing));
        return false;
    }
    initialise_thing_state(thing, state);
    return true;
}

void init_creature_state(struct Thing *thing)
{
    struct Room *room;
    if (is_neutral_thing(thing))
    {
        set_start_state(thing);
        return;
    }
    room = get_room_thing_is_on(thing);
    if (!room_is_invalid(room))
    {
        switch (room->kind)
        {
        case RoK_PRISON:
        case RoK_TORTURE:
        case RoK_GUARDPOST:
            if ( send_creature_to_room(thing, room) )
              return;
        default:
            break;
        }
    }
    set_start_state(thing);
}

void process_person_moods_and_needs(struct Thing *thing)
{
    _DK_process_person_moods_and_needs(thing);
}
/******************************************************************************/
