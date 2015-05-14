/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file player_computer.c
 *     Computer player definitions and activities.
 * @par Purpose:
 *     Defines a computer player control variables and events/checks/processes
 *      functions.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     10 Mar 2009 - 20 Mar 2009
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include "player_computer.h"

#include <limits.h>

#include "globals.h"
#include "bflib_basics.h"
#include "bflib_fileio.h"
#include "bflib_dernc.h"
#include "bflib_memory.h"
#include "bflib_math.h"

#include "config.h"
#include "config_compp.h"
#include "config_terrain.h"
#include "config_creature.h"
#include "creature_states.h"
#include "ariadne_wallhug.h"
#include "spdigger_stack.h"
#include "magic.h"
#include "map_utils.h"
#include "thing_traps.h"
#include "thing_navigate.h"
#include "player_complookup.h"
#include "player_newcomp.h"
#include "power_hand.h"
#include "room_data.h"
#include "game_legacy.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
ComputerType computer_assist_types[] = { 6, 7, 8, 9 };

char const event_pay_day_text[] = "EVENT PAY DAY";
char const event_save_imps_text[] = "EVENT SAVE IMPS";
char const event_check_room_text[] = "EVENT CHECK ROOMS FULL";
char const event_magic_foe_text[] = "EVENT MAGIC FOE";
char const event_check_fighters_text[] = "EVENT CHECK FIGHTERS";
char const event_fight_test_text[] = "EVENT FIGHT TEST";
char const event_fight_text[] = "EVENT FIGHT";
char const event_living_space_full_text[] = "EVENT LIVING SPACE FULL";
char const event_treasure_room_full_text[] = "EVENT TREASURE ROOM FULL";
char const event_heart_under_attack_text[] = "EVENT HEART UNDER ATTACK";
char const event_room_attack_text[] = "EVENT ROOM ATTACK";
char const event_dungeon_breach_text[] = "EVENT DUNGEON BREACH";

char const check_money_text[] = "CHECK MONEY";
char const check_expand_room_text[] = "CHECK EXPAND ROOM";
char const check_avail_trap_text[] = "CHECK AVAILIABLE TRAP";
char const check_neutral_places_text[] = "CHECK FOR NEUTRAL PLACES";
char const check_avail_door_text[] = "CHECK AVAILIABLE DOOR";
char const check_enemy_entrances_text[] = "CHECK FOR ENEMY ENTRANCES";
char const check_for_slap_imp_text[] = "CHECK FOR SLAP IMP";
char const check_for_speed_up_text[] = "CHECK FOR SPEED UP";
char const check_for_quick_attack_text[] = "CHECK FOR QUICK ATTACK";
char const check_to_pretty_text[] = "CHECK TO PRETTY";
char const check_enough_imps_text[] = "CHECK FOR ENOUGH IMPS";
char const move_creature_to_train_text[] = "MOVE CREATURE TO TRAINING";
char const move_creature_to_best_text[] = "MOVE CREATURE TO BEST ROOM";
char const computer_check_hates_text[] = "COMPUTER CHECK HATES";

/******************************************************************************/
DLLIMPORT long _DK_get_computer_money_less_cost(struct Computer2 *comp);
DLLIMPORT long _DK_count_creatures_availiable_for_fight(struct Computer2 *comp, struct Coord3d *pos);
DLLIMPORT struct ComputerTask *_DK_is_there_an_attack_task(struct Computer2 *comp);
DLLIMPORT long _DK_count_creatures_for_defend_pickup(struct Computer2 *comp);
DLLIMPORT long _DK_computer_find_non_solid_block(struct Computer2 *comp, struct Coord3d *pos);
DLLIMPORT long _DK_check_call_to_arms(struct Computer2 *comp);
DLLIMPORT long _DK_computer_finds_nearest_room_to_gold(struct Computer2 *comp, struct Coord3d *pos, struct GoldLookup **gldlook);
DLLIMPORT long _DK_computer_finds_nearest_room_to_pos(struct Computer2 *comp, struct Room **retroom, struct Coord3d *nearpos);

/******************************************************************************/
// Function definition needed to compare pointers - remove pending
long computer_setup_dig_to_gold(struct Computer2 *comp, struct ComputerProcess *cproc);
long computer_check_dig_to_gold(struct Computer2 *comp, struct ComputerProcess *cproc);
long computer_setup_any_room(struct Computer2 *comp, struct ComputerProcess *cproc);

/******************************************************************************/
struct Computer2 *get_computer_player_f(long plyr_idx,const char *func_name)
{
    if ((plyr_idx >= 0) && (plyr_idx < PLAYERS_COUNT))
        return &game.computer[plyr_idx];
    ERRORMSG("%s: Tried to get non-existing computer player %d!",func_name,(int)plyr_idx);
    return INVALID_COMPUTER_PLAYER;
}

TbBool computer_player_invalid(const struct Computer2 *comp)
{
    if (comp == INVALID_COMPUTER_PLAYER)
        return true;
    return (comp < &game.computer[0]);
}

TbBool computer_player_in_emergency_state(const struct Computer2 *comp)
{
    struct Dungeon *dungeon;
    dungeon = comp->dungeon;
    if (get_computer_money_less_cost(comp) < -1000)
        return true;
    if (dungeon->num_active_diggers < 3)
        return true;
    return false;
}

GoldAmount get_computer_money_less_cost(const struct Computer2 *comp)
{
    struct Dungeon *dungeon;
    dungeon = comp->dungeon;
    GoldAmount money_payday, money_mkdigger;
    // As payday need, take amount planned for next payday
    money_payday = dungeon->creatures_total_pay;
    // In case payday expenses are low, require enough money to make special digger
    money_mkdigger = compute_power_price(dungeon->owner, PwrK_MKDIGGER, 0);
    if (money_payday < money_mkdigger)
        money_payday = money_mkdigger;
    return dungeon->total_money_owned - money_payday;
}

long set_autopilot_type(PlayerNumber plyr_idx, long aptype)
{
    setup_a_computer_player(plyr_idx, computer_assist_types[aptype-1]);
    return 1;
}

struct ComputerTask * able_to_build_room_at_task(struct Computer2 *comp, RoomKind rkind, long width_slabs, long height_slabs, long area, long a6)
{
    struct ComputerTask *ctask;
    long i;
    unsigned long k;
    i = comp->task_idx;
    k = 0;
    while (i != 0)
    {
        ctask = get_computer_task(i);
        if (computer_task_invalid(ctask))
        {
            ERRORLOG("Jump to invalid task detected");
            break;
        }
        i = ctask->next_task;
        // Per-task code
        if (((ctask->flags & ComTsk_Unkn0001) != 0) && ((ctask->flags & ComTsk_Unkn0002) != 0))
        {
            unsigned short max_radius;
            struct ComputerTask *roomtask;
            max_radius = ctask->create_room.width / 2;
            if (max_radius <= ctask->create_room.height / 2)
              max_radius = ctask->create_room.height / 2;
            roomtask = able_to_build_room(comp, &ctask->pos_64, rkind, width_slabs, height_slabs, area + max_radius + 1, a6);
            if (!computer_task_invalid(roomtask)) {
                return roomtask;
            }
        }
        // Per-task code ends
        k++;
        if (k > COMPUTER_TASKS_COUNT)
        {
            ERRORLOG("Infinite loop detected when sweeping tasks list");
            break;
        }
    }
    return NULL;
}

/**
 * Checks if we are able to build a room starting out from already build room of given kind.
 * @param comp
 * @param rkind The room kind to be built.
 * @param look_kind The room kind which we'd like to search as starting point.
 * @param width_slabs
 * @param height_slabs
 * @param area
 * @param a6
 * @return
 */
struct ComputerTask * able_to_build_room_from_room(struct Computer2 *comp, RoomKind rkind, RoomKind look_kind, long width_slabs, long height_slabs, long area, long a6)
{
    struct Dungeon *dungeon;
    dungeon = comp->dungeon;
    long i;
    unsigned long k;
    i = dungeon->room_kind[look_kind];
    k = 0;
    while (i != 0)
    {
        struct Room *room;
        room = room_get(i);
        if (room_is_invalid(room))
        {
            ERRORLOG("Jump to invalid room detected");
            break;
        }
        i = room->next_of_owner;
        // Per-room code
        struct Coord3d pos;
        struct ComputerTask *roomtask;
        pos.x.val = subtile_coord_center(room->central_stl_x);
        pos.y.val = subtile_coord_center(room->central_stl_y);
        pos.z.val = subtile_coord(1,0);
        roomtask = able_to_build_room(comp, &pos, rkind, width_slabs, height_slabs, area, a6);
        if (!computer_task_invalid(roomtask)) {
            return roomtask;
        }
        // Per-room code ends
        k++;
        if (k > ROOMS_COUNT)
        {
            ERRORLOG("Infinite loop detected when sweeping rooms list");
            break;
        }
    }
    return INVALID_COMPUTER_TASK;
}

struct ComputerTask *computer_setup_build_room(struct Computer2 *comp, RoomKind rkind, long width_slabs, long height_slabs, long look_randstart)
{
    struct Dungeon *dungeon;
    dungeon = comp->dungeon;
    long max_slabs;
    long area_min,area_max;
    long i;
    max_slabs = height_slabs;
    if (max_slabs < width_slabs)
        max_slabs = width_slabs;
    area_min = (max_slabs + 1) / 2 + 1;
    area_max = area_min / 3 + 2 * area_min;
    if (rkind == RoK_LAIR)
    {
        if (width_slabs*height_slabs < dungeon->max_creatures_attracted)
        {
            i = LbSqrL(dungeon->max_creatures_attracted);
            width_slabs = i + 1;
            height_slabs = i + 1;
        }
    }
    const long arr_length = sizeof(look_through_rooms)/sizeof(look_through_rooms[0]);
    long area;
    for (area=area_min; area < area_max; area++)
    {
        long aparam;
        for (aparam=1; aparam >= 0; aparam--)
        {
            unsigned int lookidx;
            lookidx = look_randstart;
            if (look_randstart < 0)
            {
                lookidx = ACTION_RANDOM(arr_length);
            }
            for (i=0; i < arr_length; i++)
            {
                struct ComputerTask *roomtask;
                int look_kind;
                look_kind = look_through_rooms[lookidx];
                if (look_kind == RoK_UNKN17)
                {
                    roomtask = able_to_build_room_at_task(comp, rkind, width_slabs, height_slabs, area, aparam);
                } else
                {
                    roomtask = able_to_build_room_from_room(comp, rkind, look_kind, width_slabs, height_slabs, area, aparam);
                }
                if (!computer_task_invalid(roomtask)) {
                    return roomtask;
                }
                lookidx = (lookidx + 1) % arr_length;
            }
        }
    }
    SYNCLOG("Player %d dungeon has no place for %s sized %dx%d", (int)dungeon->owner, room_code_name(rkind), (int)width_slabs, (int)height_slabs);
    return INVALID_COMPUTER_TASK;
}

long computer_finds_nearest_room_to_gold_lookup(const struct Dungeon *dungeon, const struct GoldLookup *gldlook, struct Room **nearroom)
{
    struct Room *room;
    long rkind;
    long distance,min_distance;
    struct Coord3d gold_pos;
    *nearroom = INVALID_ROOM;
    gold_pos.x.val = 0;
    gold_pos.y.val = 0;
    gold_pos.z.val = 0;
    gold_pos.x.stl.num = gldlook->x_stl_num;
    gold_pos.y.stl.num = gldlook->y_stl_num;
    min_distance = LONG_MAX;
    distance = LONG_MAX;
    for (rkind=1; rkind < ROOM_TYPES_COUNT; rkind++)
    {
        room = find_room_nearest_to_position(dungeon->owner, rkind, &gold_pos, &distance);
        if (!room_is_invalid(room))
        {
            // Convert to subtiles
            distance = coord_subtile(distance);
            // Decrease the value by gold area radius
            distance -= LbSqrL(gldlook->num_gold_slabs * STL_PER_SLB);
            distance -= LbSqrL(gldlook->num_gem_slabs * STL_PER_SLB * 4);
            // We can accept longer distances if digging directly to treasure room
            if (room->kind == RoK_TREASURE)
                distance -= TREASURE_ROOM_PREFERENCE_WHILE_DIGGING_GOLD;
            if (min_distance > distance)
            {
                *nearroom = room;
                min_distance = distance;
            }
        }
    }
    return min_distance;
}

long computer_finds_nearest_task_to_gold(const struct Computer2 *comp, const struct GoldLookup *gldlook, struct ComputerTask ** near_task)
{
    struct Coord3d task_pos;
    long i;
    unsigned long k;
    struct ComputerTask *ctask;
    long distance,min_distance;
    MapCoordDelta delta_x,delta_y;
    task_pos.x.val = 0;
    task_pos.y.val = 0;
    task_pos.z.val = 0;
    task_pos.x.stl.num = gldlook->x_stl_num;
    task_pos.y.stl.num = gldlook->y_stl_num;
    min_distance = LONG_MAX;
    i = comp->task_idx;
    k = 0;
    while (i != 0)
    {
        ctask = get_computer_task(i);
        if (computer_task_invalid(ctask))
        {
            ERRORLOG("Jump to invalid task detected");
            break;
        }
        i = ctask->next_task;
        // Per-task code
        if ( ((ctask->flags & ComTsk_Unkn0001) != 0) && ((ctask->flags & ComTsk_Unkn0002) != 0) )
        {
            delta_x = ctask->pos_64.x.val - (MapCoordDelta)task_pos.x.val;
            delta_y = ctask->pos_64.y.val - (MapCoordDelta)task_pos.y.val;
            distance = LbDiagonalLength(abs(delta_x), abs(delta_y));
            // Convert to subtiles
            distance = coord_subtile(distance);
            // Decrease the value by gold area radius
            distance -= LbSqrL(gldlook->num_gold_slabs * STL_PER_SLB);
            distance -= LbSqrL(gldlook->num_gem_slabs * STL_PER_SLB * 4);
            if (min_distance > distance)
            {
                *near_task = ctask;
                min_distance = distance;
            }
        }
        // Per-task code ends
        k++;
        if (k > COMPUTER_TASKS_COUNT)
        {
            ERRORLOG("Infinite loop detected when sweeping tasks list");
            break;
        }
    }
    return min_distance;
}

/**
 * Finds nearest place to start digging gold from, and the target GoldLookup to be digged.
 *
 * @param comp Computer player which considers starting the digging.
 * @param pos Resurns position to start digging from.
 * @param gldlookref Returns reference to GoldLookup containing coords of the place to dig to.
 * @return Lower or equal 0 on failure, positive if gold digging is ready to go.
 */
long computer_finds_nearest_room_to_gold(struct Computer2 *comp, struct Coord3d *pos, struct GoldLookup **gldlookref)
{
    struct Dungeon *dungeon;
    struct GoldLookup *gldlook;
    struct GoldLookup *gldlooksel;
    struct Coord3d locpos;
    struct Coord3d *spos;
    long dig_distance;
    long lookups_checked;
    long i;
    SYNCDBG(5,"Starting");
    dungeon = comp->dungeon;
    gldlooksel = NULL;
    *gldlookref = gldlooksel;
    locpos.x.val = 0;
    locpos.y.val = 0;
    locpos.z.val = 0;
    spos = &locpos;
    lookups_checked = 0;
    dig_distance = LONG_MAX;
    for (i=0; i < GOLD_LOOKUP_COUNT; i++)
    {
        gldlook = get_gold_lookup(i);
        if ((gldlook->flags & 0x01) == 0)
            continue;
        SYNCDBG(18,"Valid vein at (%d,%d)",(int)gldlook->x_stl_num,(int)gldlook->y_stl_num);
        if ((gldlook->player_interested[dungeon->owner] & 0x03) != 0)
            continue;
        SYNCDBG(8,"Searching for place to reach (%d,%d)",(int)gldlook->x_stl_num,(int)gldlook->y_stl_num);
        lookups_checked++;
        struct Room *room = INVALID_ROOM;
        long new_dist;
        new_dist = computer_finds_nearest_room_to_gold_lookup(dungeon, gldlook, &room);
        if (dig_distance > new_dist)
        {
            locpos.x.val = subtile_coord_center(room->central_stl_x);
            locpos.y.val = subtile_coord_center(room->central_stl_y);
            locpos.z.val = subtile_coord(1,0);
            spos = &locpos;
            dig_distance = new_dist;
            gldlooksel = gldlook;
            SYNCDBG(8,"Distance from room at (%d,%d) is %d",(int)spos->x.stl.num,(int)spos->y.stl.num,(int)dig_distance);
        }
        struct ComputerTask *ctask = NULL;
        new_dist = computer_finds_nearest_task_to_gold(comp, gldlook, &ctask);
        if (dig_distance > new_dist)
        {
            spos = &ctask->pos_64;
            dig_distance = new_dist;
            gldlooksel = gldlook;
            SYNCDBG(8,"Distance from task at (%d,%d) is %d",(int)spos->x.stl.num,(int)spos->y.stl.num,(int)dig_distance);
        }
    }
    if (gldlooksel == NULL)
    {
        SYNCDBG(8,"Checked %d lookups, but no gold to dig found",(int)lookups_checked);
        if (lookups_checked == 0)
        {
            return -1;
        } else
        {
            return 0;
        }
    }
    SYNCDBG(8,"Best digging start to reach (%d,%d) is on subtile (%d,%d); distance is %d",(int)gldlooksel->x_stl_num,(int)gldlooksel->y_stl_num,(int)spos->x.stl.num,(int)spos->y.stl.num,(int)dig_distance);
    *gldlookref = gldlooksel;
    pos->x.val = spos->x.val;
    pos->y.val = spos->y.val;
    pos->z.val = spos->z.val;
    if (dig_distance < 1)
        dig_distance = 1;
    if (dig_distance > LONG_MAX)
        dig_distance = LONG_MAX;
    return dig_distance;
}

long count_creatures_availiable_for_fight(struct Computer2 *comp, struct Coord3d *pos)
{
    //return _DK_count_creatures_availiable_for_fight(comp, pos);
    struct Dungeon *dungeon;
    struct CreatureControl *cctrl;
    struct Thing *thing;
    unsigned long k;
    int i;
    SYNCDBG(8,"Starting");
    dungeon = comp->dungeon;
    unsigned long count;
    count = 0;
    k = 0;
    i = dungeon->creatr_list_start;
    while (i != 0)
    {
        thing = thing_get(i);
        TRACE_THING(thing);
        cctrl = creature_control_get_from_thing(thing);
        if (thing_is_invalid(thing) || creature_control_invalid(cctrl))
        {
            ERRORLOG("Jump to invalid creature detected");
            break;
        }
        i = cctrl->players_next_creature_idx;
        // Thing list loop body
        if (cctrl->combat_flags == 0)
        {
            if ((pos == NULL) || creature_can_navigate_to(thing, pos, 1)) {
                count++;
            }
        }
        // Thing list loop body ends
        k++;
        if (k > CREATURES_COUNT)
        {
            ERRORLOG("Infinite loop detected when sweeping creatures list");
            break;
        }
    }
    SYNCDBG(19,"Finished");
    return count;
}

TbBool is_there_an_attack_task(const struct Computer2 *comp)
{
    static const ComputerTaskType attack_tasks[] = {
        CTT_DigToAttack,
        CTT_MagicCallToArms,
        CTT_PickupForAttack,
        CTT_MoveCreatureToRoom,
        CTT_MoveCreatureToPos,
        CTT_MoveCreaturesToDefend,
        CTT_None,
    };
    const struct ComputerTask *ctask;
    ctask = get_task_in_progress_in_list(comp, attack_tasks);
    return !computer_task_invalid(ctask);
}

/**
 * Gathers information about players hatred to other players.
 *
 * @param comp Computer player to be considered.
 * @param hates The output hatred array. The array size should be PLAYERS_COUNT.
 *   The function will fill the array with hatred information, and sort it so that
 *   first entry informs of the player toward whom the hatred is highest.
 */
void get_opponent(struct Computer2 *comp, struct THate hates[])
{
    SYNCDBG(7,"Starting");
    struct Dungeon *dungeon;
    dungeon = comp->dungeon;
    long i;
    // Initialize hate struct
    for (i=0; i < PLAYERS_COUNT; i++)
    {
        struct THate *hate;
        hate = &hates[i];
        struct OpponentRelation *oprel;
        oprel = &comp->opponent_relations[i];
        hate->amount = oprel->hate_amount;
        hate->plyr_idx = i;
        hate->pos_near = NULL;
        hate->distance_near = LONG_MAX;
    }
    // Sort the hates, using basic sorting algorithm
    for (i=0; i < PLAYERS_COUNT; i++)
    {
        long n;
        for (n=0; n < PLAYERS_COUNT-1; n++)
        {
            struct THate *hat1;
            hat1 = &hates[n];
            struct THate *hat2;
            hat2 = &hates[n+1];
            if (hat2->amount > hat1->amount)
            {
                // Switch hates so larger one is first
                struct THate tmp;
                LbMemoryCopy(&tmp,hat2,sizeof(struct THate));
                LbMemoryCopy(hat2,hat1,sizeof(struct THate));
                LbMemoryCopy(hat1,&tmp,sizeof(struct THate));
            }
        }
    }
    // Get position of a possible attack - select one of minimum distance to our heart
    struct Thing *heartng;
    heartng = get_player_soul_container(dungeon->owner);
    MapSubtlCoord dnstl_x, dnstl_y;
    dnstl_x = heartng->mappos.x.stl.num;
    dnstl_y = heartng->mappos.y.stl.num;
    for (i=0; i < PLAYERS_COUNT; i++)
    {
        struct THate *hate;
        hate = &hates[i];
        struct OpponentRelation *oprel;
        oprel = &comp->opponent_relations[hate->plyr_idx];
        int ptidx;
        ptidx = oprel->field_4;
        if (ptidx > 0)
          ptidx--;
        long n;
        for (n=0; n < COMPUTER_SPARK_POSITIONS_COUNT; n++)
        {
            struct Coord3d *pos;
            pos = &oprel->pos_A[ptidx];
            if ((pos->x.val > 0) && (pos->y.val > 0))
            {
                if (search_spiral(pos, hate->plyr_idx, 25, xy_walkable) == 25)
                {
                    pos->x.val = 0;
                } else
                if (find_from_task_list_by_subtile(dungeon->owner, pos->x.stl.num, pos->y.stl.num) >= 0)
                {
                    pos->x.val = 0;
                } else
                {
                    long dist;
                    dist = abs((MapSubtlCoord)pos->x.stl.num - dnstl_x) + abs((MapSubtlCoord)pos->y.stl.num - dnstl_y);
                    if (hate->distance_near >= dist)
                    {
                        hate->distance_near = dist;
                        hate->pos_near = pos;
                    }
                }
            }
            ptidx = (ptidx + 1) % COMPUTER_SPARK_POSITIONS_COUNT;
        }
    }
}

long computer_finds_nearest_room_to_pos(struct Computer2 *comp, struct Room **retroom, struct Coord3d *nearpos)
{
    return _DK_computer_finds_nearest_room_to_pos(comp, retroom, nearpos);
}

long setup_computer_attack(struct Computer2 *comp, struct ComputerProcess *cproc, struct Coord3d *pos, long victim_plyr_idx)
{
    struct Room *room;
    SYNCDBG(8,"Starting player %d attack on %d",(int)comp->dungeon->owner,(int)victim_plyr_idx);
    if (!computer_finds_nearest_room_to_pos(comp, &room, pos)) {
        SYNCDBG(7,"Cannot find owned room near (%d,%d), giving up",(int)pos->x.stl.num,(int)pos->y.stl.num);
        return 0;
    }
    struct Coord3d startpos, endpos;
    startpos.x.val = subtile_coord_center(stl_slab_center_subtile(room->central_stl_x));
    startpos.y.val = subtile_coord_center(stl_slab_center_subtile(room->central_stl_y));
    startpos.z.val = subtile_coord(1,0);
    endpos.x.val = pos->x.val;
    endpos.y.val = pos->y.val;
    endpos.z.val = pos->z.val;
    long parent_cproc_idx;
    parent_cproc_idx = computer_process_index(comp, cproc);
    if (!create_task_dig_to_attack(comp, startpos, endpos, victim_plyr_idx, parent_cproc_idx)) {
        SYNCDBG(7,"Cannot create task to dig to (%d,%d), giving up",(int)pos->x.stl.num,(int)pos->y.stl.num);
        return 0;
    }
    SYNCDBG(7,"Attack setup complete for destination (%d,%d)",(int)pos->x.stl.num,(int)pos->y.stl.num);
    return 1;
}

long count_entrances(const struct Computer2 *comp, PlayerNumber plyr_idx)
{
    const struct Room *room;
    const struct Dungeon *dungeon;
    long i;
    unsigned long k;
    long count;
    dungeon = comp->dungeon;
    count = 0;
    i = game.entrance_room_id;
    k = 0;
    while (i != 0)
    {
        room = room_get(i);
        if (room_is_invalid(room))
        {
            ERRORLOG("Jump to invalid room detected");
            break;
        }
        i = room->next_of_kind;
        // Per-room code
        if ((room->player_interested[dungeon->owner] & 0x01) == 0)
        {
            if ((plyr_idx < 0) || (room->owner == plyr_idx))
                count++;
        }
        // Per-room code ends
        k++;
        if (k > ROOMS_COUNT)
        {
            ERRORLOG("Infinite loop detected when sweeping rooms list");
            break;
        }
    }
    return count;
}

long count_creatures_in_dungeon(const struct Dungeon *dungeon)
{
    return count_player_list_creatures_of_model(dungeon->creatr_list_start, 0);
}

long count_diggers_in_dungeon(const struct Dungeon *dungeon)
{
    return count_player_list_creatures_of_model(dungeon->digger_list_start, 0);
}

/**
 * Returns amount of traps in workshop of given manufacturable model.
 * @param dungeon
 * @param trmodel
 */
long buildable_traps_amount(struct Dungeon *dungeon, ThingModel trmodel)
{
    if ((trmodel < 1) || (trmodel >= TRAP_TYPES_COUNT))
        return 0;
    if ((dungeon->trap_build_flags[trmodel] & MnfBldF_Manufacturable) != 0)
    {
        return dungeon->trap_amount_stored[trmodel];
    }
    return 0;
}

/**
 * Retrieves count of different trap kinds of which given dungeon has at least given number.
 * @param dungeon
 * @param base_amount
 */
long get_number_of_trap_kinds_with_amount_at_least(struct Dungeon *dungeon, long base_amount)
{
    long i,kinds;
    kinds = 0;
    for (i=1; i < TRAP_TYPES_COUNT; i++)
    {
        if (buildable_traps_amount(dungeon, i) >= base_amount)
        {
            kinds++;
        }
    }
    return kinds;
}

/**
 * Retrieves count of different trap kinds of which given dungeon has at least given number.
 * @param dungeon
 * @param base_amount
 */
long get_nth_of_trap_kinds_with_amount_at_least(struct Dungeon *dungeon, long base_amount, long n)
{
    long i;
    for (i=1; i < TRAP_TYPES_COUNT; i++)
    {
        if (buildable_traps_amount(dungeon, i) >= base_amount)
        {
            if (n <= 0)
                return i;
            n--;
        }
    }
    return 0;
}

/**
 * Retrieves best kind of trap to place in dungeon.
 * @param dungeon The dungeon which owns the spare traps.
 * @param allow_last Accept kind of which only one box exists.
 * @param kind_preselect If possible, try to use given kind; only if it's not accessible, check others.
 */
long computer_choose_best_trap_kind_to_place(struct Dungeon *dungeon, long allow_last, ThingModel kind_preselect)
{
    long kinds_multiple,kinds_single;
    // If there are multiple buildable traps of preselected kind
    if ((kind_preselect > 0) && (buildable_traps_amount(dungeon, kind_preselect) >= 2))
        return kind_preselect;
    // No pre-selection - check if there are other multiple traps
    kinds_multiple = get_number_of_trap_kinds_with_amount_at_least(dungeon, 2);
    if (kinds_multiple > 0) {
        SYNCDBG(18,"Returning one of %d plentiful traps",(int)kinds_multiple);
        return get_nth_of_trap_kinds_with_amount_at_least(dungeon, 2, ACTION_RANDOM(kinds_multiple));
    }
    // If there are no multiple traps, and we're not allowing to spend last one
    if (!allow_last)
        return 0;
    // If there are buildable traps of preselected kind
    if ((kind_preselect > 0) && (buildable_traps_amount(dungeon, kind_preselect) >= 1))
        return kind_preselect;
    kinds_single = get_number_of_trap_kinds_with_amount_at_least(dungeon, 1);
    if (kinds_single > 0) {
        SYNCDBG(18,"Returning one of %d single traps",(int)kinds_single);
        return get_nth_of_trap_kinds_with_amount_at_least(dungeon, 1, ACTION_RANDOM(kinds_single));
    }
    return 0;
}

int computer_find_more_trap_place_locations_around_room(struct Computer2 *comp, const struct Room *room)
{
    //TODO implement finding trap locations
    return 0;
}

int computer_find_more_trap_place_locations(struct Computer2 *comp)
{
    struct Dungeon *dungeon;
    SYNCDBG(8,"Starting");
    dungeon = comp->dungeon;
    int num_added;
    num_added = 0;
    RoomKind rkind;
    rkind = ACTION_RANDOM(ROOM_TYPES_COUNT);
    int m;
    for (m = 0; m < ROOM_TYPES_COUNT; m++, rkind=(rkind+1)%ROOM_TYPES_COUNT)
    {
        int i;
        unsigned long k;
        k = 0;
        i = dungeon->room_kind[rkind];
        while (i != 0)
        {
            struct Room *room;
            room = room_get(i);
            if (room_is_invalid(room))
            {
                ERRORLOG("Jump to invalid room detected");
                break;
            }
            i = room->next_of_owner;
            // Per-room code
            int nadded;
            nadded = computer_find_more_trap_place_locations_around_room(comp, room);
            if (nadded < 0)
                break;
            num_added += nadded;
            // Per-room code ends
            k++;
            if (k > ROOMS_COUNT)
            {
              ERRORLOG("Infinite loop detected when sweeping rooms list");
              break;
            }
        }
    }
    return num_added;
}

TbBool computer_get_trap_place_location_and_update_locations(struct Computer2 *comp, ThingModel trapmodel, struct Coord3d *retloc)
{
    struct Dungeon *dungeon;
    dungeon = comp->dungeon;
    long i;
    for (i=0; i < COMPUTER_TRAP_LOC_COUNT; i++)
    {
        struct Coord3d *location;
        location = &comp->trap_locations[i];
        // Check if the entry has coords stored
        if ((location->x.val <= 0) && (location->y.val <= 0))
            continue;
        MapSlabCoord slb_x,slb_y;
        slb_x = subtile_slab(location->x.stl.num);
        slb_y = subtile_slab(location->y.stl.num);
        struct SlabMap *slb;
        slb = get_slabmap_block(slb_x,slb_y);
        if (slabmap_block_invalid(slb)) {
            ERRORLOG("Trap location contained off-map point (%d,%d)",(int)location->x.stl.num,(int)location->y.stl.num);
            location->x.val = 0;
            location->y.val = 0;
            continue;
        }
        if (can_place_trap_on(dungeon->owner, location->x.stl.num, location->y.stl.num))
        { // If it's our owned claimed ground, give it a try
            retloc->x.val = location->x.val;
            retloc->y.val = location->y.val;
            retloc->z.val = 0;
            location->x.val = 0;
            location->y.val = 0;
            return true;
        } else
        if (slb->kind != SlbT_PATH)
        { // If it would be a path, we could wait for someone to claim it; but if it's not..
            if (find_from_task_list_by_slab(dungeon->owner, slb_x, slb_y) < 0)
            { // If we have no intention of doing a task there - remove it from list
                WARNLOG("Removing player %d trap location (%d,%d) because of %s there",(int)dungeon->owner,(int)location->x.stl.num,(int)location->y.stl.num,slab_code_name(slb->kind));
                location->x.val = 0;
                location->y.val = 0;
                continue;
            }
        }
    }
    return false;
}

long computer_check_for_place_trap(struct Computer2 *comp, struct ComputerCheck * check)
{
    struct Dungeon *dungeon;
    SYNCDBG(8,"Starting");
    dungeon = comp->dungeon;
    if (dungeon_invalid(dungeon) || !player_has_heart(dungeon->owner)) {
        SYNCDBG(7,"Computer players %d dungeon in invalid or has no heart",(int)dungeon->owner);
        return CTaskRet_Unk4;
    }
    long kind_chosen;
    kind_chosen = computer_choose_best_trap_kind_to_place(dungeon, check->param1, check->param2);
    if (kind_chosen <= 0)
        return CTaskRet_Unk4;
    struct Coord3d pos;
    if (!computer_get_trap_place_location_and_update_locations(comp, kind_chosen, &pos))
    {
        // update list of locations and try to get location again
        if (computer_find_more_trap_place_locations(comp) <= 0) {
            SYNCDBG(7,"Computer players %d could not find any new locations for traps",(int)dungeon->owner);
            return CTaskRet_Unk4;
        }
        if (!computer_get_trap_place_location_and_update_locations(comp, kind_chosen, &pos)) {
            SYNCDBG(7,"Computer players %d could not find place for trap",(int)dungeon->owner);
            return CTaskRet_Unk4;
        }
    }
    TbResult ret;
    // Only allow to place trap at position where there's no traps already
    SYNCDBG(8,"Trying to place %s trap at (%d,%d)",trap_code_name(kind_chosen),(int)pos.x.stl.num,(int)pos.y.stl.num);
    ret = try_game_action(comp, dungeon->owner, GA_PlaceTrap, 0, pos.x.stl.num, pos.y.stl.num, kind_chosen, 0);
    if (ret > Lb_OK)
      return CTaskRet_Unk1;
    return CTaskRet_Unk4;
}

/**
 * Picks creatures which are currently training or scavenging and places them into lair.
 * @param comp
 * @param room
 * @param thing_idx
 * @param tasks_limit
 * @return Gives the amount of creatures moved.
 */
long computer_pick_training_or_scavenging_creatures_and_place_on_room(struct Computer2 *comp, struct Room *room, long thing_idx, long tasks_limit)
{
    struct CreatureControl *cctrl;
    struct Thing *thing;
    long new_tasks;
    unsigned long k;
    long i;
    new_tasks = 0;
    // Sweep through creatures list
    i = thing_idx;
    k = 0;
    while (i != 0)
    {
      thing = thing_get(i);
      if (thing_is_invalid(thing))
      {
        ERRORLOG("Jump to invalid thing detected");
        break;
      }
      cctrl = creature_control_get_from_thing(thing);
      i = cctrl->players_next_creature_idx;
      // Per creature code
      if (creature_is_training(thing) || creature_is_scavengering(thing)) // originally, only CrSt_Training and CrSt_Scavengering were accepted
      {
        if (!create_task_move_creature_to_subtile(comp, thing, room->central_stl_x, room->central_stl_y, CrSt_CreatureDoingNothing))
          break;
        new_tasks++;
        if (new_tasks >= tasks_limit)
          break;
      }
      // Per creature code ends
      k++;
      if (k > THINGS_COUNT)
      {
        ERRORLOG("Infinite loop detected when sweeping things list");
        break;
      }
    }
    return new_tasks;
}

/** Picks creatures and workers from given dungeon who are doing expensive jobs, then places them on lair.
 *
 * @param comp Computer player who controls the target dungeon.
 * @param tasks_limit Max amount of computer tasks to create.
 * @return Amount of new computer tasks created.
 */
long computer_pick_expensive_job_creatures_and_place_on_lair(struct Computer2 *comp, long tasks_limit)
{
    struct Dungeon *dungeon;
    struct Room *room;
    long new_tasks;
    dungeon = comp->dungeon;
    room = room_get(dungeon->room_kind[RoK_LAIR]);
    new_tasks = 0;
    // If we don't have lair, then don't even bother
    if (room_is_invalid(room)) {
        return new_tasks;
    }
    // If we can't pick up creatures, admit the failure now
    if (computer_able_to_use_magic(comp, PwrK_HAND, 1, 1) != 1) {
        return new_tasks;
    }
    // Sweep through creatures list
    new_tasks += computer_pick_training_or_scavenging_creatures_and_place_on_room(comp, room, dungeon->creatr_list_start, tasks_limit);
    if (new_tasks >= tasks_limit)
        return new_tasks;
    // Sweep through workers list
    new_tasks += computer_pick_training_or_scavenging_creatures_and_place_on_room(comp, room, dungeon->digger_list_start, tasks_limit-new_tasks);
    return new_tasks;
}

/**
 * Checks how much money the player lacks for next payday.
 * @param comp Computer player who controls the target dungeon.
 * @param check The check being executed; param1 is low gold value, param2 is critical gold value.
 */

long computer_check_for_money(struct Computer2 *comp, struct ComputerCheck * check)
{
    GoldAmount money_left;
    struct ComputerProcess *cproc;
    struct Dungeon *dungeon;
    long ret;
    long i;
    SYNCDBG(18,"Starting");
    ret = CTaskRet_Unk4;
    dungeon = comp->dungeon;
    if (dungeon_invalid(dungeon) || !player_has_heart(dungeon->owner)) {
        SYNCDBG(7,"Computer players %d dungeon in invalid or has no heart",(int)dungeon->owner);
        return CTaskRet_Unk4;
    }
    // Check how much money we will have left after payday (or other expenses)
    money_left = get_computer_money_less_cost(comp);
    // Try increasing priority of digging for gold process
    if ((money_left < check->param2) || (money_left < check->param1))
    {
        SYNCDBG(8,"Increasing player %d gold dig process priority",(int)dungeon->owner);
        for (i=0; i <= COMPUTER_PROCESSES_COUNT; i++)
        {
            cproc = &comp->processes[i];
            if ((cproc->flags & ComProc_Unkn0002) != 0)
                break;
            //TODO COMPUTER_PLAYER comparing function pointers is a bad practice
            if (cproc->func_check == computer_check_dig_to_gold)
            {
                cproc->priority++;
                if (game.play_gameturn - cproc->last_run_turn > 20) {
                    cproc->last_run_turn = 0;
                }
            }
        }
    }
    // Try selling traps and doors
    if ((money_left < check->param2) && dungeon_has_room(dungeon, RoK_WORKSHOP))
    {
        if (dungeon_has_any_buildable_traps(dungeon) || dungeon_has_any_buildable_doors(dungeon) ||
            player_has_deployed_trap_of_model(dungeon->owner, -1) || player_has_deployed_door_of_model(dungeon->owner, -1, 0))
        {
            if (!is_task_in_progress(comp, CTT_SellTrapsAndDoors))
            {
                SYNCDBG(8,"Creating task to sell player %d traps and doors",(int)dungeon->owner);
                if (create_task_sell_traps_and_doors(comp, 5, check->param2 - money_left)) {
                    ret = CTaskRet_Unk1;
                }
            }
        }
    }
    // Power hand tasks are exclusive, so select randomly
    int pwhand_task_choose;
    pwhand_task_choose = ACTION_RANDOM(101);
    // Move creatures away from rooms which cost a lot to use
    if ((money_left < check->param1) && (pwhand_task_choose < 33))
    {
        int num_to_move;
        num_to_move = 3;
        if (!is_task_in_progress_using_hand(comp) && (computer_able_to_use_magic(comp, PwrK_HAND, 1, num_to_move) == CTaskRet_Unk1))
        {
            SYNCDBG(8,"Creating task to pick player %d creatures from expensive jobs",(int)dungeon->owner);
            if (computer_pick_expensive_job_creatures_and_place_on_lair(comp, num_to_move) > 0) {
                ret = CTaskRet_Unk1;
            }
        }
    }
    // Drop imps on gold/gems mining sites
    if ((money_left < check->param1) && (pwhand_task_choose < 66) && dungeon_has_room(dungeon, RoK_TREASURE))
    {
        int num_to_move;
        num_to_move = 3;
        // If there's already task in progress which uses hand, then don't add more
        // content of the hand could be used by wrong task by mistake
        if (!is_task_in_progress_using_hand(comp) && (computer_able_to_use_magic(comp, PwrK_HAND, 1, num_to_move) == CTaskRet_Unk1))
        {
            MapSubtlCoord stl_x, stl_y;
            int tsk_id;
            stl_x = -1;
            stl_y = -1;
            // Find a gold digging site which could use a worker
            tsk_id = get_random_mining_undug_area_position_for_digger_drop(dungeon->owner, &stl_x, &stl_y);
            if (tsk_id >= 0)
            {
                struct Coord3d pos;
                pos.x.val = subtile_coord_center(stl_x);
                pos.y.val = subtile_coord_center(stl_y);
                pos.z.val = subtile_coord(1,0);
                SYNCDBG(8,"Creating task to move player %d diggers near gold to mine",(int)dungeon->owner);
                if (move_imp_to_mine_here(comp, &pos, num_to_move) > 0) {
                    ret = CTaskRet_Unk1;
                }
            }
        }
    }
    // Move any gold laying around to treasure room
    if ((money_left < check->param1) && dungeon_has_room(dungeon, RoK_TREASURE))
    {
        int num_to_move;
        num_to_move = 10;
        // If there's already task in progress which uses hand, then don't add more
        // content of the hand could be used by wrong task by mistake
        if (!is_task_in_progress_using_hand(comp) && (computer_able_to_use_magic(comp, PwrK_HAND, 1, num_to_move) == CTaskRet_Unk1))
        {
            SYNCDBG(8,"Creating task to move neutral gold to treasury");
            if (create_task_move_gold_to_treasury(comp, num_to_move, 2*dungeon->creatures_total_pay)) {
                ret = CTaskRet_Unk1;
            }
        }
    }
    return ret;
}

long count_creatures_for_defend_pickup(struct Computer2 *comp)
{
    return _DK_count_creatures_for_defend_pickup(comp);
}

/**
 * Modifies given position into nearest one where thing can be dropped.
 * @param comp
 * @param pos
 */
TbBool computer_find_non_solid_block(const struct Computer2 *comp, struct Coord3d *pos)
{
    //return _DK_computer_find_non_solid_block(comp, pos);
    unsigned long n,k;
    for (n = 0; n < MID_AROUND_LENGTH; n++)
    {
        MapSubtlCoord arstl_x, arstl_y;
        arstl_x = pos->x.stl.num + STL_PER_SLB*start_at_around[n].delta_x;
        arstl_y = pos->y.stl.num + STL_PER_SLB*start_at_around[n].delta_y;
        for (k = 0; k < MID_AROUND_LENGTH; k++)
        {
            MapSubtlCoord sstl_x, sstl_y;
            sstl_x = arstl_x + start_at_around[k].delta_x;
            sstl_y = arstl_y + start_at_around[k].delta_y;
            if (can_drop_thing_here(sstl_x, sstl_y, comp->dungeon->owner, 0) == 1)
            {
              pos->x.val = subtile_coord_center(sstl_x);
              pos->y.val = subtile_coord_center(sstl_y);
              return true;
            }
        }
    }
    return false;
}

long computer_able_to_use_magic(struct Computer2 *comp, PowerKind pwkind, long pwlevel, long amount)
{
    struct Dungeon *dungeon;
    dungeon = comp->dungeon;
    if (!is_power_available(dungeon->owner, pwkind)) {
        return CTaskRet_Unk4;
    }
    if (pwlevel >= MAGIC_OVERCHARGE_LEVELS)
        pwlevel = MAGIC_OVERCHARGE_LEVELS;
    if (pwlevel < 0)
        pwlevel = 0;
    GoldAmount money, price;
    money = get_computer_money_less_cost(comp);
    price = compute_power_price(dungeon->owner, pwkind, pwlevel);
    if ((price > 0) && (amount * price > money)) {
        return CTaskRet_Unk0;
    }
    return CTaskRet_Unk1;
}

long check_call_to_arms(struct Computer2 *comp)
{
    SYNCDBG(8,"Starting for player %d",(int)comp->dungeon->owner);
    long ret;
    ret = 1;
    if (comp->dungeon->cta_start_turn != 0)
    {
        const struct ComputerTask *ctask;
        long i;
        unsigned long k;
        i = comp->task_idx;
        k = 0;
        while (i != 0)
        {
            ctask = get_computer_task(i);
            if (computer_task_invalid(ctask))
            {
                ERRORLOG("Jump to invalid task detected");
                break;
            }
            i = ctask->next_task;
            // Per-task code
            if ((ctask->flags & ComTsk_Unkn0001) != 0)
            {
                if ((ctask->ttype == CTT_MagicCallToArms) && (ctask->task_state == 2))
                {
                    if (ret == 1) {
                        SYNCDBG(8,"Found existing CTA task");
                        ret = 0;
                    }
                    if (ctask->field_60 + ctask->lastrun_turn - (long)game.play_gameturn < ctask->field_60 - ctask->field_60/10) {
                        SYNCDBG(8,"Less than 90% turns");
                        ret = -1;
                        break;
                    }
                }
            }
            // Per-task code ends
            k++;
            if (k > COMPUTER_TASKS_COUNT)
            {
                ERRORLOG("Infinite loop detected when sweeping tasks list");
                break;
            }
        }
    }
    return ret;
}

TbBool setup_a_computer_player(PlayerNumber plyr_idx, long comp_model)
{
    struct ComputerProcessTypes *cpt;
    struct ComputerProcess *cproc;
    struct ComputerProcess *newproc;
    struct ComputerCheck *ccheck;
    struct ComputerCheck *newchk;
    struct ComputerEvent *event;
    struct ComputerEvent *newevnt;
    struct OpponentRelation *oprel;
    struct Computer2 *comp;
    long i;
    if ((plyr_idx >= PLAYERS_COUNT) || (plyr_idx == game.hero_player_num)
        || (plyr_idx == game.neutral_player_num)) {
        WARNLOG("Tried to setup player %d which can't be used this way",(int)plyr_idx);
        return false;
    }
    comp = get_computer_player(plyr_idx);
    if (computer_player_invalid(comp)) {
        ERRORLOG("Tried to setup player %d which has no computer capability",(int)plyr_idx);
        return false;
    }
    LbMemorySet(comp, 0, sizeof(struct Computer2));
    cpt = get_computer_process_type_template(comp_model);
    comp->dungeon = get_players_num_dungeon(plyr_idx);
    comp->model = comp_model;
    if (dungeon_invalid(comp->dungeon)) {
        WARNLOG("Tried to setup player %d which has no dungeon",(int)plyr_idx);
        comp->dungeon = INVALID_DUNGEON;
        comp->model = 0;
        return false;
    }
    comp->field_18 = cpt->field_C;
    comp->field_14 = cpt->field_8;
    comp->max_room_build_tasks = cpt->max_room_build_tasks;
    comp->field_2C = cpt->field_14;
    comp->field_20 = cpt->field_18;
    comp->field_C = 1;
    comp->task_state = CTaskSt_Select;

    for (i=0; i < PLAYERS_COUNT; i++)
    {
        oprel = &comp->opponent_relations[i];
        oprel->field_0 = 0;
        oprel->field_4 = 0;
        if (i == plyr_idx) {
            oprel->hate_amount = LONG_MIN;
        } else {
            oprel->hate_amount = 0;
        }
    }
    comp->field_1C = cpt->field_4;

    for (i=0; i < COMPUTER_PROCESSES_COUNT; i++)
    {
        cproc = cpt->processes[i];
        newproc = &comp->processes[i];
        if ((cproc == NULL) || (cproc->name == NULL))
        {
          newproc->name = NULL;
          break;
        }
        // Modifying original ComputerProcessTypes structure - I don't like it!
        //TODO COMPUTER_PLAYER comparing function pointers is a bad practice
        if (cproc->func_setup == computer_setup_any_room)
        {
          if (cproc->confval_5 >= 0)
            cproc->confval_5 = get_room_look_through(cproc->confval_5);
        }
        LbMemoryCopy(newproc, cproc, sizeof(struct ComputerProcess));
        newproc->parent = cproc;
    }
    newproc = &comp->processes[i];
    newproc->flags |= ComProc_Unkn0002;

    for (i=0; i < COMPUTER_CHECKS_COUNT; i++)
    {
        ccheck = &cpt->checks[i];
        newchk = &comp->checks[i];
        if ((ccheck == NULL) || (ccheck->name == NULL))
        {
            newchk->name = NULL;
            break;
        }
        LbMemoryCopy(newchk, ccheck, sizeof(struct ComputerCheck));
    }
    // Note that we don't have special, empty check at end of array
    // The check with 0x02 flag identifies end of active checks
    // (the check with 0x02 flag is invalid - only previous checks are in use)
    //newchk = &comp->checks[i];
    newchk->flags |= ComTsk_Unkn0002;

    for (i=0; i < COMPUTER_EVENTS_COUNT; i++)
    {
        event = &cpt->events[i];
        newevnt = &comp->events[i];
        if ((event == NULL) || (event->name == NULL))
        {
            newevnt->name = NULL;
            break;
        }
        LbMemoryCopy(newevnt, event, sizeof(struct ComputerEvent));
    }
    return true;
}

void computer_check_events(struct Computer2 *comp)
{
    struct Dungeon * dungeon;
    struct ComputerEvent * cevent;
    struct Event * event;
    long i,n;
    SYNCDBG(17,"Starting");
    dungeon = comp->dungeon;
    for (i=0; i < COMPUTER_EVENTS_COUNT; i++)
    {
        cevent = &comp->events[i];
        if (cevent->name == NULL)
            break;
        switch (cevent->cetype)
        {
        case 0:
            if ((long)game.play_gameturn < (cevent->last_test_gameturn + cevent->test_interval)) {
                break;
            }
            for (n=0; n < EVENTS_COUNT; n++)
            {
                event = &game.event[n];
                if ( ((event->flags & EvF_Exists) != 0) &&
                      (event->owner == dungeon->owner) &&
                      (event->kind == cevent->mevent_kind) )
                {
                    if (cevent->func_event(comp, cevent, event) == 1) {
                        SYNCDBG(5,"Player %d reacted on %s",(int)dungeon->owner,cevent->name);
                        cevent->last_test_gameturn = game.play_gameturn;
                    }
                }
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            if ((long)game.play_gameturn < (cevent->last_test_gameturn + cevent->test_interval)) {
                break;
            }
            {
                if (cevent->func_test(comp,cevent) == 1) {
                    SYNCDBG(5,"Player %d reacted on %s",(int)dungeon->owner,cevent->name);
                }
                // Update test turn no matter if event triggered something
                cevent->last_test_gameturn = game.play_gameturn;
            }
            break;
        default:
            ERRORLOG("Unhandled Computer Event Type %d",(int)cevent->cetype);
            break;
        }
    }
}

static int counter_check_for_door_attacks[KEEPER_COUNT] = { 0, 1, 2, 3 };
static int counter_check_for_claims[KEEPER_COUNT] = { 0, 1, 2, 3 };
static int counter_check_for_imprison_tendency[KEEPER_COUNT] = { 0, 1, 2, 3 };
static int counter_check_prison_management[KEEPER_COUNT] = { 0, 1, 2, 3 };
static int counter_check_new_digging[KEEPER_COUNT] = { 0, 1, 2, 3 };

TbBool process_checks(struct Computer2 *comp)
{
    struct ComputerCheck *ccheck;
    long delta;
    long i;
    SYNCDBG(17,"Starting");
    for (i=0; i < COMPUTER_CHECKS_COUNT; i++)
    {
        ccheck = &comp->checks[i];
        if (comp->tasks_did <= 0)
            break;
        if ((ccheck->flags & ComChk_Unkn0002) != 0)
            break;
        if ((ccheck->flags & ComChk_Unkn0001) == 0)
        {
            delta = (game.play_gameturn - ccheck->last_run_turn);
            if ((delta > ccheck->turns_interval) && (ccheck->func != NULL))
            {
                SYNCDBG(8,"Executing check %ld, \"%s\"",i,ccheck->name);
                ccheck->func(comp, ccheck);
                ccheck->last_run_turn = game.play_gameturn;
            }
        }
    }

	//checks not entered into regular system yet
	//TODO: generalize and use config
	if (++counter_check_for_claims[comp->dungeon->owner] >= 51)
	{
		counter_check_for_claims[comp->dungeon->owner] = 0;
		computer_check_for_claims(comp);
	}
	if (++counter_check_for_door_attacks[comp->dungeon->owner] >= 151)
	{
		counter_check_for_door_attacks[comp->dungeon->owner] = 0;
		computer_check_for_door_attacks(comp);
	}
	if (++counter_check_for_imprison_tendency[comp->dungeon->owner] >= 39)
	{
		counter_check_for_imprison_tendency[comp->dungeon->owner] = 0;
		computer_check_for_imprison_tendency(comp);
	}
	if (++counter_check_prison_management[comp->dungeon->owner] >= 41)
	{
		counter_check_prison_management[comp->dungeon->owner] = 0;
		computer_check_prison_management(comp);
	}
#ifdef NEW_DIGBUILD
	if (++counter_check_new_digging[comp->dungeon->owner] >= 31)
	{
		counter_check_new_digging[comp->dungeon->owner] = 0;
		computer_check_new_digging(comp);
	}
#endif

    return true;
}

TbBool process_processes_and_task(struct Computer2 *comp)
{
  struct ComputerProcess *cproc;
  Comp_Process_Func callback;
  int i;
  SYNCDBG(17,"Starting");
  for (i=comp->tasks_did; i > 0; i--)
  {
    if (comp->tasks_did <= 0)
        return false;
    if ((game.play_gameturn % comp->field_18) == 0)
        process_tasks(comp);
    switch (comp->task_state)
    {
    case CTaskSt_Wait:
        comp->gameturn_wait--;
        if (comp->gameturn_wait <= 0)
        {
            comp->gameturn_wait = comp->gameturn_delay;
            set_next_process(comp);
        }
        break;
    case CTaskSt_Select:
        set_next_process(comp);
        break;
    case CTaskSt_Perform:
        if ((comp->ongoing_process > 0) && (comp->ongoing_process <= COMPUTER_PROCESSES_COUNT))
        {
            callback = NULL;
            cproc = get_computer_process(comp, comp->ongoing_process);
            if (cproc != NULL) {
                callback = cproc->func_task;
            } else {
                ERRORLOG("Invalid computer process %d referenced",(int)comp->ongoing_process);
            }
            if (callback != NULL) {
                callback(comp,cproc);
            }
        } else
        {
            ERRORLOG("No Process %d for a computer player",(int)comp->ongoing_process);
            comp->task_state = CTaskSt_Wait;
        }
        break;
    default:
        ERRORLOG("Invalid task state %d",(int)comp->task_state);
        break;
    }
  }
  return true;
}

void process_computer_player2(PlayerNumber plyr_idx)
{
    struct Computer2 *comp;
    SYNCDBG(7,"Starting for player %d",(int)plyr_idx);
    if (plyr_idx >= PLAYERS_COUNT) {
        return;
    }
    comp = get_computer_player(plyr_idx);
    if (computer_player_invalid(comp)) {
        ERRORLOG("Player %d has no computer capability",(int)plyr_idx);
        return;
    }
    if (dungeon_invalid(comp->dungeon)) {
        ERRORLOG("Computer player %d has invalid dungeon",(int)plyr_idx);
        return;
    }
    if ((comp->field_14 != 0) && (comp->field_2C <= game.play_gameturn))
      comp->tasks_did = 1;
    else
      comp->tasks_did = 0;
    if (comp->tasks_did <= 0) {
        return;
    }
    computer_check_events(comp);
    process_checks(comp);
    process_processes_and_task(comp);
    if ((comp->tasks_did < 0) || (comp->tasks_did > 1)) {
        ERRORLOG("Computer player %d performed %d tasks instead of up to one",(int)plyr_idx,(int)comp->tasks_did);
    }
}

struct ComputerProcess *computer_player_find_process_by_func_setup(PlayerNumber plyr_idx,Comp_Process_Func func_setup)
{
  struct ComputerProcess *cproc;
  struct Computer2 *comp;
  comp = get_computer_player(plyr_idx);
  if (computer_player_invalid(comp)) {
      ERRORLOG("Player %d has no computer capability",(int)plyr_idx);
      return NULL;
  }
  cproc = &comp->processes[0];
  while ((cproc->flags & ComProc_Unkn0002) == 0)
  {
      if (cproc->func_setup == func_setup)
      {
          return cproc;
      }
      cproc++;
  }
  return NULL;
}

TbBool computer_player_demands_gold_check(PlayerNumber plyr_idx)
{
  struct ComputerProcess *dig_process;
  //TODO COMPUTER_PLAYER comparing function pointers is a bad practice
  dig_process = computer_player_find_process_by_func_setup(plyr_idx,computer_setup_dig_to_gold);
  // If this computer player has no gold digging process
  if (dig_process == NULL)
  {
      SYNCDBG(18,"Player %d has no digging ability.",(int)plyr_idx);
      return false;
  }
  if ((dig_process->flags & ComProc_Unkn0004) == 0)
  {
      SYNCDBG(18,"Player %d isn't interested in digging.",(int)plyr_idx);
      return false;
  }
  SYNCDBG(8,"Player %d wants to start digging.",(int)plyr_idx);
  // If the computer player needs to dig for gold
  if (gameadd.turn_last_checked_for_gold+GOLD_DEMAND_CHECK_INTERVAL < game.play_gameturn)
  {
      dig_process->flags &= ~ComProc_Unkn0004;
      return true;
  }
  return false;
}

void process_computer_players2(void)
{
    struct PlayerInfo *player;
    struct Dungeon *dungeon;
    TbBool needs_gold_check;
    int i;
    needs_gold_check = false;
#ifdef PETTER_AI
    SAI_run_shared();
#endif
    for (i=0; i < PLAYERS_COUNT; i++)
    {
        player = get_player(i);
        dungeon = get_players_dungeon(player);
        if (!player_exists(player) || dungeon_invalid(dungeon))
            continue;
        if (((player->allocflags & PlaF_CompCtrl) != 0) || ((dungeon->computer_enabled & 0x01) != 0))
        {
          if (player->field_2C == 1)
          {
#ifdef PETTER_AI
            SAI_run_for_player(i);
#else
            process_computer_player2(i);
            if (computer_player_demands_gold_check(i))
            {
                needs_gold_check = true;
            }
#endif
          }
        }
    }
    if (needs_gold_check)
    {
      SYNCDBG(0,"Computer players demand gold check.");
      gameadd.turn_last_checked_for_gold = game.play_gameturn;
      check_map_for_gold();
    } else
    if (gameadd.turn_last_checked_for_gold > game.play_gameturn)
    {
      gameadd.turn_last_checked_for_gold = 0;
    }
}

void setup_computer_players2(void)
{
  struct PlayerInfo *player;
  int i;
  gameadd.turn_last_checked_for_gold = game.play_gameturn;
  check_map_for_gold();
  computer_setup_new_digging();
  for (i=0; i < COMPUTER_TASKS_COUNT; i++)
  {
    LbMemorySet(&game.computer_task[i], 0, sizeof(struct ComputerTask));
  }
  for (i=0; i < PLAYERS_COUNT; i++)
  {
    player = get_player(i);
    if (player_exists(player))
    {
      if (player->field_2C == 1)
      {
        setup_a_computer_player(i, 7);
      }
    }
  }
}

void restore_computer_player_after_load(void)
{
    struct Computer2 *comp;
    struct PlayerInfo *player;
    struct ComputerProcessTypes *cpt;
    long plyr_idx;
    long i;
    SYNCDBG(7,"Starting");
    for (plyr_idx=0; plyr_idx < PLAYERS_COUNT; plyr_idx++)
    {
        player = get_player(plyr_idx);
        comp = get_computer_player(plyr_idx);
        if (computer_player_invalid(comp)) {
            ERRORLOG("Player %d has no computer capability",(int)plyr_idx);
            continue;
        }
        if (!player_exists(player)) {
            LbMemorySet(comp, 0, sizeof(struct Computer2));
            comp->dungeon = INVALID_DUNGEON;
            continue;
        }
        if (player->field_2C != 1)
        {
            LbMemorySet(comp, 0, sizeof(struct Computer2));
            comp->dungeon = get_players_dungeon(player);
            continue;
        }
        comp->dungeon = get_players_dungeon(player);
        cpt = get_computer_process_type_template(comp->model);

        for (i=0; i < COMPUTER_PROCESSES_COUNT; i++)
        {
            if (cpt->processes[i] == NULL)
                break;
            //if (cpt->processes[i]->name == NULL)
            //    break;
            SYNCDBG(12,"Player %ld process %ld is \"%s\"",plyr_idx,i,cpt->processes[i]->name);
            comp->processes[i].name = cpt->processes[i]->name;
            comp->processes[i].parent = cpt->processes[i];
            comp->processes[i].func_check = cpt->processes[i]->func_check;
            comp->processes[i].func_setup = cpt->processes[i]->func_setup;
            comp->processes[i].func_task = cpt->processes[i]->func_task;
            comp->processes[i].func_complete = cpt->processes[i]->func_complete;
            comp->processes[i].func_pause = cpt->processes[i]->func_pause;
        }
        for (i=0; i < COMPUTER_CHECKS_COUNT; i++)
        {
            if (cpt->checks[i].name == NULL)
              break;
            SYNCDBG(12,"Player %ld check %ld is \"%s\"",plyr_idx,i,cpt->checks[i].name);
            comp->checks[i].name = cpt->checks[i].name;
            comp->checks[i].func = cpt->checks[i].func;
        }
        for (i=0; i < COMPUTER_EVENTS_COUNT; i++)
        {
            if (cpt->events[i].name == NULL)
              break;
            comp->events[i].name = cpt->events[i].name;
            comp->events[i].func_event = cpt->events[i].func_event;
            comp->events[i].func_test = cpt->events[i].func_test;
            comp->events[i].process = cpt->events[i].process;
        }
    }
}

/******************************************************************************/
#ifdef __cplusplus
}
#endif
