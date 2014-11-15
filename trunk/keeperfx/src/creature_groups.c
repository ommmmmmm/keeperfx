/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file creature_groups.c
 *     Creature grouping and groups support functions.
 * @par Purpose:
 *     Functions to creature_groups.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     11 Mar 2010 - 12 May 2010
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include "creature_groups.h"

#include "globals.h"
#include "bflib_basics.h"

#include "bflib_math.h"
#include "thing_list.h"
#include "thing_creature.h"
#include "thing_physics.h"
#include "creature_control.h"
#include "creature_states.h"
#include "config_creature.h"
#include "room_jobs.h"
#include "game_legacy.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
DLLIMPORT long _DK_get_highest_experience_level_in_group(struct Thing *creatng);
DLLIMPORT void _DK_leader_find_positions_for_followers(struct Thing *creatng);

/******************************************************************************/
CrtrExpLevel get_highest_experience_level_in_group(struct Thing *grptng)
{
    //return _DK_get_highest_experience_level_in_group(grptng);
    struct CreatureControl *cctrl;
    cctrl = creature_control_get_from_thing(grptng);
    CrtrExpLevel best_explevel;
    struct Thing *ctng;
    best_explevel = 0;
    long i;
    unsigned long k;
    i = cctrl->group_info & TngGroup_LeaderIndex;
    k = 0;
    while (i > 0)
    {
        ctng = thing_get(i);
        TRACE_THING(ctng);
        cctrl = creature_control_get_from_thing(ctng);
        if (creature_control_invalid(cctrl))
            break;
        // Per-thing code
        if (best_explevel < cctrl->explevel) {
            best_explevel = cctrl->explevel;
        }
        // Per-thing code ends
        i = cctrl->next_in_group;
        k++;
        if (k > CREATURES_COUNT)
        {
            ERRORLOG("Infinite loop detected when sweeping creatures group");
            break;
        }
    }
    return best_explevel;
}

long get_no_creatures_in_group(const struct Thing *grptng)
{
    struct CreatureControl *cctrl;
    struct Thing *ctng;
    long i;
    unsigned long k;
    cctrl = creature_control_get_from_thing(grptng);
    i = cctrl->group_info & TngGroup_LeaderIndex;
    if (i == 0) {
        // No group - just one creature
        return 1;
    }
    k = 0;
    while (i > 0)
    {
        ctng = thing_get(i);
        TRACE_THING(ctng);
        cctrl = creature_control_get_from_thing(ctng);
        if (creature_control_invalid(cctrl))
            break;
        i = cctrl->next_in_group;
        k++;
        if (k > CREATURES_COUNT)
        {
            ERRORLOG("Infinite loop detected when sweeping creatures group");
            break;
        }
    }
    return k;
}

struct Thing *get_last_creature_in_group(const struct Thing *grptng)
{
    struct CreatureControl *cctrl;
    struct Thing *ctng;
    long i;
    unsigned long k;
    cctrl = creature_control_get_from_thing(grptng);
    i = cctrl->group_info & TngGroup_LeaderIndex;
    if (i == 0) {
        // No group - just one creature
        return NULL;
    }
    k = 0;
    while (i > 0)
    {
        ctng = thing_get(i);
        TRACE_THING(ctng);
        cctrl = creature_control_get_from_thing(ctng);
        if (creature_control_invalid(cctrl))
            break;
        i = cctrl->next_in_group;
        k++;
        if (k > CREATURES_COUNT)
        {
            ERRORLOG("Infinite loop detected when sweeping creatures group");
            break;
        }
    }
    return ctng;
}

TbBool add_creature_to_group(struct Thing *crthing, struct Thing *grthing)
{
    struct Thing *pvthing;
    pvthing = get_last_creature_in_group(grthing);
    if ((grthing->index == crthing->index) || (grthing->owner != crthing->owner)) {
        return false;
    }
    struct CreatureControl *crctrl;
    long i;
    crctrl = creature_control_get_from_thing(crthing);
    i = crctrl->group_info & TngGroup_LeaderIndex;
    if (i != 0) {
        remove_creature_from_group(crthing);
    }
    if (thing_exists(pvthing))
    {
        // If we already have a group, place the new creature at end
        struct CreatureControl *pvctrl;
        crctrl = creature_control_get_from_thing(crthing);
        crctrl->prev_in_group = pvthing->index;
        pvctrl = creature_control_get_from_thing(pvthing);
        pvctrl->next_in_group = crthing->index;
        crctrl->group_info ^= (crctrl->group_info ^ pvctrl->group_info) & TngGroup_LeaderIndex;
        crctrl->next_in_group = 0;
    } else
    {
        // If there's no group, create new one made of both creatures
        struct CreatureControl *grctrl;
        crctrl = creature_control_get_from_thing(crthing);
        crctrl->prev_in_group = grthing->index;
        grctrl = creature_control_get_from_thing(grthing);
        grctrl->next_in_group = crthing->index;
        crctrl->group_info ^= (crctrl->group_info ^ grthing->index) & TngGroup_LeaderIndex;
        grctrl->group_info ^= (grctrl->group_info ^ grthing->index) & TngGroup_LeaderIndex;
        crctrl->next_in_group = 0;
        crctrl->group_info &= TngGroup_LeaderIndex;
    }
    crthing->alloc_flags |= TAlF_IsFollowingLeader;
    return true;
}

struct Party *get_party_of_name(const char *prtname)
{
    struct Party *party;
    int i;
    for (i = 0; i < game.script.creature_partys_num; i++)
    {
        party = &game.script.creature_partys[i];
        if (strcasecmp(party->prtname, prtname) == 0)
            return party;
    }
    return NULL;
}

int get_party_index_of_name(const char *prtname)
{
    struct Party *party;
    int i;
    for (i = 0; i < game.script.creature_partys_num; i++)
    {
        party = &game.script.creature_partys[i];
        if (strcasecmp(party->prtname, prtname) == 0)
            return i;
    }
    return -1;
}

TbBool create_party(char *prtname)
{
    struct Party *party;
    if (game.script.creature_partys_num >= CREATURE_PARTYS_COUNT)
    {
        SCRPTERRLOG("Too many partys in script");
        return false;
    }
    party = (&game.script.creature_partys[game.script.creature_partys_num]);
    strncpy(party->prtname, prtname, sizeof(party->prtname));
    party->members_num = 0;
    game.script.creature_partys_num++;
    return true;
}

TbBool add_member_to_party_name(const char *prtname, long crtr_model, long crtr_level, long carried_gold, long objctv_id, long countdown)
{
    struct Party *party;
    struct PartyMember *member;
    party = get_party_of_name(prtname);
    if (party == NULL)
    {
      SCRPTERRLOG("Party of requested name, '%s', is not defined", prtname);
      return false;
    }
    if (party->members_num >= GROUP_MEMBERS_COUNT)
    {
      SCRPTERRLOG("Too many creatures in party '%s' (limit is %d members)",
          prtname, GROUP_MEMBERS_COUNT);
      return false;
    }
    member = &(party->members[party->members_num]);
    set_flag_byte(&(member->flags), TrgF_DISABLED, false);
    member->crtr_kind = crtr_model;
    member->carried_gold = carried_gold;
    member->crtr_level = crtr_level-1;
    member->field_6F = 1;
    member->objectv = objctv_id;
    member->countdown = countdown;
    party->members_num++;
    return true;
}

TbBool make_group_member_leader(struct Thing *leadtng)
{
    struct Thing *prvtng;
    prvtng = get_group_leader(leadtng);
    if (thing_is_invalid(prvtng))
        return false;
    if (prvtng->index != leadtng->index)
    {
        remove_creature_from_group(leadtng);
        add_creature_to_group_as_leader(leadtng, prvtng);
        return true;
    }
    return false;
}

long process_obey_leader(struct Thing *thing)
{
    struct Thing *leadtng;
    leadtng = get_group_leader(thing);
    if (thing_is_invalid(leadtng)) {
        set_start_state(thing);
        return 1;
    }
    if ((leadtng->alloc_flags & TAlF_IsControlled) != 0)
    {
        if (thing->active_state != CrSt_CreatureFollowLeader) {
            external_set_thing_state(thing, CrSt_CreatureFollowLeader);
        }
        return 1;
    }
    struct CreatureControl *cctrl;
    struct CreatureControl *leadctrl;
    struct StateInfo *stati;
    stati = get_creature_state_with_task_completion(leadtng);
    switch (stati->field_21)
    {
    case 1:
        if (thing->active_state != CrSt_CreatureFollowLeader) {
            external_set_thing_state(thing, CrSt_CreatureFollowLeader);
        }
        break;
    case 2:
        cctrl = creature_control_get_from_thing(thing);
        leadctrl = creature_control_get_from_thing(leadtng);
        if ((cctrl->work_room_id != leadctrl->work_room_id) && (cctrl->target_room_id != leadctrl->work_room_id))
        {
            struct Room *room;
            room = get_room_creature_works_in(leadtng);
            struct CreatureStats *crstat;
            crstat = creature_stats_get_from_thing(thing);
            CreatureJob jobpref;
            jobpref = get_job_for_room(room->kind, JoKF_None, crstat->job_primary|crstat->job_secondary);
            cleanup_current_thing_state(thing);
            send_creature_to_room(thing, room, jobpref);
        }
        break;
    default:
        break;
    }
    return 1;
}

void creature_follower_pos_add(struct Thing *creatng, int ifollow, const struct Coord3d *pos)
{
    struct MemberPos *avail_pos;
    struct CreatureControl *cctrl;
    cctrl = creature_control_get_from_thing(creatng);
    avail_pos = &cctrl->followers_pos[ifollow];
    avail_pos->stl_num = get_subtile_number(pos->x.stl.num, pos->y.stl.num);
    avail_pos->flags |= 0x02;
}

void leader_find_positions_for_followers(struct Thing *thing)
{
    //_DK_leader_find_positions_for_followers(thing);
    int group_count;
    group_count = get_no_creatures_in_group(thing);
    struct CreatureControl *cctrl;
    cctrl = creature_control_get_from_thing(thing);
    if (((cctrl->group_info >> 12) == group_count) && (game.play_gameturn & 0x1F))
    {
        int i;
        for (i= 0; i < GROUP_MEMBERS_COUNT; i++)
        {
          cctrl->followers_pos[i].flags &= ~0x01;
        }
        return;
    }
    cctrl->group_info = (group_count << 12) ^ ((cctrl->group_info ^ (group_count << 12)) & TngGroup_MemberCount);
    memset(cctrl->followers_pos, 0, sizeof(cctrl->followers_pos));

    int len_xv, len_yv;
    int len_xh, len_yh;
    len_xv = LbSinL(thing->field_52 + LbFPMath_PI) << 8 >> 16;
    len_yv = -((LbCosL(thing->field_52 + LbFPMath_PI) << 8) >> 8) >> 8;
    len_xh = LbSinL(thing->field_52 - LbFPMath_PI/2) << 8 >> 16;
    len_yh = -((LbCosL(thing->field_52 - LbFPMath_PI/2) << 8) >> 8) >> 8;

    int ih, iv, ivmax;
    ivmax = 2 * group_count;
    int ifollow;
    ifollow = 0;

    int shift_xh, shift_yh;
    int shift_xv, shift_yv;
    int shift_xh_beg, shift_yh_beg;
    int delta_xh, delta_yh;
    int delta_xv, delta_yv;

    delta_yh = 2 * len_yh;
    delta_xh = 2 * len_xh;
    shift_yv = 2 * len_yv;
    delta_yv = 2 * len_yv;
    shift_yh_beg = -2 * len_yh;
    shift_xh_beg = -2 * len_xh;
    shift_xv = 2 * len_xv;
    delta_xv = 2 * len_xv;
    for (iv = 2; iv <= ivmax; iv += 2)
    {
        shift_yh = shift_yh_beg;
        shift_xh = shift_xh_beg;
        for (ih = -2; ih <= 2; ih += 2)
        {
            int mcor_x, mcor_y;
            mcor_x = thing->mappos.x.val + shift_xh + shift_xv;
            mcor_y = thing->mappos.y.val + shift_yv + shift_yh;
            if ((coord_slab(mcor_x) > 0) && (coord_slab(mcor_x) < map_tiles_x))
            {
                if ((coord_slab(mcor_y) > 0) && (coord_slab(mcor_y) < map_tiles_y))
                {
                    struct Coord3d pos;
                    pos.x.val = mcor_x;
                    pos.y.val = mcor_y;
                    pos.z.val = get_floor_height_at(&pos);
                    if (!thing_in_wall_at(thing, &pos))
                    {
                        creature_follower_pos_add(thing, ifollow, &pos);
                        ifollow++;
                        // If we're not able to store more coordinates, quit now
                        if (ifollow >= group_count) {
                          return;
                        }
                    }
                }
            }
            shift_yh += delta_yh;
            shift_xh += delta_xh;
        }
        shift_yv += delta_yv;
        shift_xv += delta_xv;
    }

    shift_yv = len_yv;
    delta_yh = 2 * len_yh;
    delta_yv = 2 * len_yv;
    delta_xh = 2 * len_xh;
    shift_xv = len_xv;
    delta_xv = 2 * len_xv;
    shift_yh_beg = -len_yh;
    shift_xh_beg = -len_xh;
    for (iv = 1; iv <= ivmax; iv += 2)
    {
        shift_yh = shift_yh_beg;
        shift_xh = shift_xh_beg;
        for (ih = -1; ih <= 2; ih += 2)
        {
            int mcor_x, mcor_y;
            mcor_x = thing->mappos.x.val + shift_xh + shift_xv;
            mcor_y = thing->mappos.y.val + shift_yv + shift_yh;
            if ((coord_slab(mcor_x) > 0) && (coord_slab(mcor_x) < map_tiles_x))
            {
                if ((coord_slab(mcor_y) > 0) && (coord_slab(mcor_y) < map_tiles_y))
                {
                    struct Coord3d pos;
                    pos.x.val = mcor_x;
                    pos.y.val = mcor_y;
                    pos.z.val = get_floor_height_at(&pos);
                    if (!thing_in_wall_at(thing, &pos))
                    {
                        creature_follower_pos_add(thing, ifollow, &pos);
                        ifollow++;
                        // If we're not able to store more coordinates, quit now
                        if (ifollow >= group_count) {
                          return;
                        }
                    }
                }
            }
            shift_yh += delta_yh;
            shift_xh += delta_xh;
        }
        shift_yv += delta_yv;
        shift_xv += delta_xv;
    }
}
/******************************************************************************/
#ifdef __cplusplus
}
#endif
