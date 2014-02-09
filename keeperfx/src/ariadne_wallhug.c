/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file ariadne_wallhug.c
 *     Simple wallhug pathfinding functions.
 * @par Purpose:
 *     Functions implementing wallhug pathfinding algorithm.
 * @par Comment:
 *     None.
 * @author   KeeperFX Team
 * @date     11 Mar 2010 - 10 Jan 2014
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include "ariadne_wallhug.h"

#include "globals.h"
#include "bflib_basics.h"
#include "bflib_math.h"
#include "bflib_planar.h"

#include "ariadne.h"
#include "slab_data.h"
#include "map_data.h"
#include "thing_data.h"
#include "thing_physics.h"
#include "engine_camera.h"
#include "config_terrain.h"
#include "creature_control.h"
#include "creature_states.h"
#include "config_creature.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
DLLIMPORT long _DK_slab_wall_hug_route(struct Thing *thing, struct Coord3d *pos, long a3);
DLLIMPORT short _DK_hug_round(struct Thing *creatng, struct Coord3d *pos1, struct Coord3d *pos2, unsigned short a4, long *a5);
DLLIMPORT long _DK_get_angle_of_wall_hug(struct Thing *creatng, long a2, long a3, unsigned char a4);
DLLIMPORT signed char _DK_get_starting_angle_and_side_of_hug(struct Thing *creatng, struct Coord3d *pos, long *a3, unsigned char *a4, long a5, unsigned char direction);
DLLIMPORT long _DK_creature_cannot_move_directly_to_with_collide(struct Thing *creatng, struct Coord3d *pos, long a3, unsigned char a4);
DLLIMPORT long _DK_check_forward_for_prospective_hugs(struct Thing *creatng, struct Coord3d *pos, long a3, long a4, long a5, long direction, unsigned char a7);
DLLIMPORT long _DK_get_map_index_of_first_block_thing_colliding_with_travelling_to(struct Thing *creatng, struct Coord3d *startpos, struct Coord3d *endpos, long a4, unsigned char a5);
DLLIMPORT long _DK_get_next_position_and_angle_required_to_tunnel_creature_to(struct Thing *creatng, struct Coord3d *pos, unsigned char a3);
DLLIMPORT long _DK_dig_to_position(signed char basestl_x, unsigned short basestl_y, unsigned short plyr_idx, unsigned char a4, unsigned char a5);
/******************************************************************************/
struct Around const small_around[] = {
  { 0,-1},
  { 1, 0},
  { 0, 1},
  {-1, 0},
};

struct Around const my_around_eight[] = {
  { 0,-1},
  { 1,-1},
  { 1, 0},
  { 1, 1},
  { 0, 1},
  {-1, 1},
  {-1, 0},
  {-1,-1},
};

short const around_map[] = {-257, -256, -255, -1, 0, 1, 255, 256, 257};

/**
 * Should contain values encoded with get_subtile_number(). */
const unsigned short small_around_pos[] = {
  0xFF00, 0x0001, 0x0100, 0xFFFF,
};

struct Around const mid_around[] = {
  { 0,  0},
  { 0, -1},
  { 1,  0},
  { 0,  1},
  {-1,  0},
  {-1, -1},
  { 1, -1},
  {-1,  1},
  { 1,  1},
};

/******************************************************************************/
/**
 * Computes index in small_around[] array which contains coordinates directing towards given destination.
 * @param srcpos_x Source position X; either map coordinates or subtiles, but have to match type of other coords.
 * @param srcpos_y Source position Y; either map coordinates or subtiles, but have to match type of other coords.
 * @param dstpos_x Destination position X; either map coordinates or subtiles, but have to match type of other coords.
 * @param dstpos_y Destination position Y; either map coordinates or subtiles, but have to match type of other coords.
 * @return Index for small_around[] array.
 */
int small_around_index_in_direction(long srcpos_x, long srcpos_y, long dstpos_x, long dstpos_y)
{
    long i;
    i = ((LbArcTanAngle(dstpos_x - srcpos_x, dstpos_y - srcpos_y) & LbFPMath_AngleMask) + LbFPMath_PI/4);
    return (i >> 9) & 3;
}

TbBool can_step_on_unsafe_terrain_at_position(const struct Thing *creatng, MapSubtlCoord stl_x, MapSubtlCoord stl_y)
{
    struct CreatureStats *crstat;
    crstat = creature_stats_get_from_thing(creatng);
    struct SlabMap *slb;
    slb = get_slabmap_for_subtile(stl_x, stl_y);
    // We can step on lava if it doesn't hurt us or we can fly
    if (slb->kind == SlbT_LAVA) {
        return (crstat->hurt_by_lava <= 0) || ((creatng->movement_flags & TMvF_Flying) != 0);
    }
    return false;
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

TbBool hug_can_move_on(struct Thing *creatng, MapSubtlCoord stl_x, MapSubtlCoord stl_y)
{
    struct SlabMap *slb;
    slb = get_slabmap_for_subtile(stl_x, stl_y);
    if (slabmap_block_invalid(slb))
        return false;
    struct SlabAttr *slbattr;
    slbattr = get_slab_attrs(slb);
    if ((slbattr->flags & SlbAtFlg_IsDoor) != 0)
    {
        struct Thing *doortng;
        doortng = get_door_for_position(stl_x, stl_y);
        if (!thing_is_invalid(doortng) && (doortng->owner == creatng->owner) && !doortng->door.is_locked)
        {
            return true;
        }
    }
    else
    {
        if (slbattr->is_safe_land || can_step_on_unsafe_terrain_at_position(creatng, stl_x, stl_y))
        {
            return true;
        }
    }
    return false;
}

long get_angle_of_wall_hug(struct Thing *creatng, long a2, long a3, unsigned char a4)
{
    return _DK_get_angle_of_wall_hug(creatng, a2, a3, a4);
}

short hug_round(struct Thing *creatng, struct Coord3d *pos1, struct Coord3d *pos2, unsigned short a4, long *a5)
{
    return _DK_hug_round(creatng, pos1, pos2, a4, a5);
}

long slab_wall_hug_route(struct Thing *thing, struct Coord3d *pos, long max_val)
{
    struct Coord3d curr_pos;
    struct Coord3d next_pos;
    struct Coord3d pos3;
    //return _DK_slab_wall_hug_route(thing, pos, max_val);
    curr_pos.x.val = thing->mappos.x.val;
    curr_pos.y.val = thing->mappos.y.val;
    curr_pos.z.val = thing->mappos.z.val;
    curr_pos.x.stl.num = stl_slab_center_subtile(curr_pos.x.stl.num);
    curr_pos.y.stl.num = stl_slab_center_subtile(curr_pos.y.stl.num);
    MapSubtlCoord stl_x, stl_y;
    stl_x = stl_slab_center_subtile(pos->x.stl.num);
    stl_y = stl_slab_center_subtile(pos->y.stl.num);
    pos3.x.val = pos->x.val;
    pos3.y.val = pos->y.val;
    pos3.z.val = pos->z.val;
    pos3.x.stl.num = stl_x;
    pos3.y.stl.num = stl_y;
    next_pos.x.val = curr_pos.x.val;
    next_pos.y.val = curr_pos.y.val;
    next_pos.z.val = curr_pos.z.val;
    int i;
    for (i = 0; i < max_val; i++)
    {
        if ((curr_pos.x.stl.num == stl_x) && (curr_pos.y.stl.num == stl_y)) {
            return i + 1;
        }
        int round_idx;
        round_idx = small_around_index_in_direction(curr_pos.x.stl.num, curr_pos.y.stl.num, stl_x, stl_y);
        if (hug_can_move_on(thing, curr_pos.x.stl.num, curr_pos.y.stl.num))
        {
            next_pos.x.val = curr_pos.x.val;
            next_pos.y.val = curr_pos.y.val;
            next_pos.z.val = curr_pos.z.val;
            curr_pos.x.stl.num += STL_PER_SLB * (int)small_around[round_idx].delta_x;
            curr_pos.y.stl.num += STL_PER_SLB * (int)small_around[round_idx].delta_y;
        } else
        {
            long hug_val;
            hug_val = max_val - i;
            int hug_ret;
            hug_ret = hug_round(thing, &next_pos, &pos3, round_idx, &hug_val);
            if (hug_ret == -1) {
                return -1;
            }
            i += hug_val;
            if (hug_ret == 1) {
                return i + 1;
            }
            curr_pos.x.val = next_pos.x.val;
            curr_pos.y.val = next_pos.y.val;
            curr_pos.z.val = next_pos.z.val;
        }
    }
    return 0;
}

signed char get_starting_angle_and_side_of_hug(struct Thing *creatng, struct Coord3d *pos, long *a3, unsigned char *a4, long a5, unsigned char a6)
{
    return _DK_get_starting_angle_and_side_of_hug(creatng, pos, a3, a4, a5, a6);
}

unsigned short get_hugging_blocked_flags(struct Thing *creatng, struct Coord3d *pos, long a3, unsigned char a4)
{
    unsigned short flags;
    struct Coord3d tmpos;
    flags = 0;
    {
        tmpos.x.val = pos->x.val;
        tmpos.y.val = creatng->mappos.y.val;
        tmpos.z.val = creatng->mappos.z.val;
        if (creature_cannot_move_directly_to_with_collide(creatng, &tmpos, a3, a4) == 4) {
            flags |= 0x01;
        }
    }
    {
        tmpos.x.val = creatng->mappos.x.val;
        tmpos.y.val = pos->y.val;
        tmpos.z.val = creatng->mappos.z.val;
        if (creature_cannot_move_directly_to_with_collide(creatng, &tmpos, a3, a4) == 4) {
            flags |= 0x02;
        }
    }
    if (flags == 0)
    {
        tmpos.x.val = pos->x.val;
        tmpos.y.val = pos->y.val;
        tmpos.z.val = creatng->mappos.z.val;
        if (creature_cannot_move_directly_to_with_collide(creatng, &tmpos, a3, a4) == 4) {
            flags |= 0x04;
        }
    }
    return flags;
}

void set_hugging_pos_using_blocked_flags(struct Coord3d *dstpos, struct Thing *creatng, unsigned short block_flags, int nav_radius)
{
    struct Coord3d tmpos;
    int coord;
    tmpos.x.val = creatng->mappos.x.val;
    tmpos.y.val = creatng->mappos.y.val;
    if (block_flags & 1)
    {
        coord = creatng->mappos.x.val;
        if (dstpos->x.val >= coord)
        {
            tmpos.x.val = coord + nav_radius;
            tmpos.x.stl.pos = 255;
            tmpos.x.val -= nav_radius;
        } else
        {
            tmpos.x.val = coord - nav_radius;
            tmpos.x.stl.pos = 1;
            tmpos.x.val += nav_radius;
        }
    }
    if (block_flags & 2)
    {
        coord = creatng->mappos.y.val;
        if (dstpos->y.val >= coord)
        {
            tmpos.y.val = coord + nav_radius;
            tmpos.y.stl.pos = 255;
            tmpos.y.val -= nav_radius;
        } else
        {
            tmpos.y.val = coord - nav_radius;
            tmpos.y.stl.pos = 1;
            tmpos.y.val += nav_radius;
        }
    }
    if (block_flags & 4)
    {
        coord = creatng->mappos.x.val;
        if (dstpos->x.val >= coord)
        {
            tmpos.x.val = coord + nav_radius;
            tmpos.x.stl.pos = 255;
            tmpos.x.val -= nav_radius;
        } else
        {
            tmpos.x.val = coord - nav_radius;
            tmpos.x.stl.pos = 1;
            tmpos.x.val += nav_radius;
        }
        coord = creatng->mappos.y.val;
        if (dstpos->y.val >= coord)
        {
            tmpos.y.val = coord + nav_radius;
            tmpos.y.stl.pos = 255;
            tmpos.y.val -= nav_radius;
        } else
        {
            tmpos.y.val = coord - nav_radius;
            tmpos.y.stl.pos = 1;
            tmpos.y.val += nav_radius;
        }
    }
    tmpos.z.val = get_thing_height_at(creatng, &tmpos);
    dstpos->x.val = tmpos.x.val;
    dstpos->y.val = tmpos.y.val;
    dstpos->z.val = tmpos.z.val;
}

long creature_cannot_move_directly_to_with_collide(struct Thing *creatng, struct Coord3d *pos, long a3, unsigned char a4)
{
    return _DK_creature_cannot_move_directly_to_with_collide(creatng, pos, a3, a4);
}

TbBool thing_can_continue_direct_line_to(struct Thing *creatng, struct Coord3d *pos1, struct Coord3d *pos2, long a4, long a5, unsigned char a6)
{
    long angle;
    struct Coord3d posa;
    struct Coord3d posb;
    struct Coord3d posc;
    int coord;
    angle = get_angle_xy_to(pos1, pos2);
    posa.x.val = pos1->x.val;
    posa.y.val = pos1->y.val;
    posa.z.val = pos1->z.val;
    posa.x.val += distance_with_angle_to_coord_x(a5, angle);
    posa.y.val += distance_with_angle_to_coord_y(a5, angle);;
    posa.z.val = get_thing_height_at(creatng, &posa);
    coord = pos1->x.val;
    if (coord < posa.x.val) {
        coord += a5;
    } else
    if (coord > posa.x.val) {
        coord -= a5;
    }
    posb.x.val = coord;
    posb.y.val = pos1->y.val;
    posb.z.val = get_thing_height_at(creatng, &posb);
    coord = pos1->y.val;
    if (coord < posa.y.val) {
        coord += a5;
    } else
    if (coord > posa.y.val) {
        coord -= a5;
    }
    posc.y.val = coord;
    posc.x.val = pos1->x.val;
    posc.z.val = get_thing_height_at(creatng, &posc);
    return creature_cannot_move_directly_to_with_collide(creatng, &posb, a4, a6) != 4
        && creature_cannot_move_directly_to_with_collide(creatng, &posc, a4, a6) != 4
        && creature_cannot_move_directly_to_with_collide(creatng, &posa, a4, a6) != 4;
}

long check_forward_for_prospective_hugs(struct Thing *creatng, struct Coord3d *pos, long a3, long a4, long a5, long a6, unsigned char a7)
{
    return _DK_check_forward_for_prospective_hugs(creatng, pos, a3, a4, a5, a6, a7);
}

TbBool find_approach_position_to_subtile(const struct Coord3d *srcpos, MapSubtlCoord stl_x, MapSubtlCoord stl_y, MoveSpeed spacing, struct Coord3d *aproachpos)
{
    struct Coord3d targetpos;
    targetpos.x.val = subtile_coord_center(stl_x);
    targetpos.y.val = subtile_coord_center(stl_y);
    targetpos.z.val = 0;
    long min_dist;
    long n;
    min_dist = LONG_MAX;
    for (n=0; n < SMALL_AROUND_SLAB_LENGTH; n++)
    {
        long dx,dy;
        dx = spacing * (long)small_around[n].delta_x;
        dy = spacing * (long)small_around[n].delta_y;
        struct Coord3d tmpos;
        tmpos.x.val = targetpos.x.val + dx;
        tmpos.y.val = targetpos.y.val + dy;
        tmpos.z.val = 0;
        struct Map *mapblk;
        mapblk = get_map_block_at(tmpos.x.stl.num, tmpos.y.stl.num);
        if ((!map_block_invalid(mapblk)) && ((mapblk->flags & MapFlg_IsTall) == 0))
        {
            long dist;
            dist = get_2d_box_distance(srcpos, &tmpos);
            if (min_dist > dist)
            {
                min_dist = dist;
                aproachpos->x.val = tmpos.x.val;
                aproachpos->y.val = tmpos.y.val;
                aproachpos->z.val = tmpos.z.val;
            }
        }
    }
    return (min_dist < LONG_MAX);
}

long get_map_index_of_first_block_thing_colliding_with_travelling_to(struct Thing *creatng, struct Coord3d *startpos, struct Coord3d *endpos, long a4, unsigned char a5)
{
    return _DK_get_map_index_of_first_block_thing_colliding_with_travelling_to(creatng, startpos, endpos, a4, a5);
}

TbBool navigation_push_towards_target(struct Navigation *navi, struct Thing *creatng, const struct Coord3d *pos, MoveSpeed speed, MoveSpeed nav_radius, unsigned char a3)
{
    navi->field_0 = 2;
    navi->pos_1B.x.val = creatng->mappos.x.val + distance_with_angle_to_coord_x(speed, navi->field_D);
    navi->pos_1B.y.val = creatng->mappos.y.val + distance_with_angle_to_coord_y(speed, navi->field_D);
    navi->pos_1B.z.val = get_thing_height_at(creatng, &navi->pos_1B);
    struct Coord3d pos1;
    pos1.x.val = navi->pos_1B.x.val;
    pos1.y.val = navi->pos_1B.y.val;
    pos1.z.val = navi->pos_1B.z.val;
    check_forward_for_prospective_hugs(creatng, &pos1, navi->field_D, navi->field_1[0], 33, speed, a3);
    if (get_2d_box_distance(&pos1, &creatng->mappos) > 16)
    {
        navi->pos_1B.x.val = pos1.x.val;
        navi->pos_1B.y.val = pos1.y.val;
        navi->pos_1B.z.val = pos1.z.val;
    }
    navi->field_9 = get_2d_box_distance(&creatng->mappos, &navi->pos_1B);
    int cannot_move;
    cannot_move = creature_cannot_move_directly_to_with_collide(creatng, &navi->pos_1B, 33, a3);
    if (cannot_move == 4)
    {
        navi->pos_1B.x.val = creatng->mappos.x.val;
        navi->pos_1B.y.val = creatng->mappos.y.val;
        navi->pos_1B.z.val = creatng->mappos.z.val;
        navi->field_9 = 0;
    }
    navi->field_5 = get_2d_box_distance(&creatng->mappos, pos);
    if (cannot_move == 1)
    {
        SubtlCodedCoords stl_num;
        stl_num = get_map_index_of_first_block_thing_colliding_with_travelling_to(creatng, &creatng->mappos, &navi->pos_1B, 40, 0);
        navi->field_15 = stl_num;
        MapSubtlCoord stl_x, stl_y;
        stl_x = slab_subtile_center(subtile_slab_fast(stl_num_decode_x(stl_num)));
        stl_y = slab_subtile_center(subtile_slab_fast(stl_num_decode_y(stl_num)));
        find_approach_position_to_subtile(&creatng->mappos, stl_x, stl_y, nav_radius + 385, &navi->pos_1B);
        navi->field_D = get_angle_xy_to(&creatng->mappos, &navi->pos_1B);
        navi->field_0 = 3;
    }
    return true;
}

long get_next_position_and_angle_required_to_tunnel_creature_to(struct Thing *creatng, struct Coord3d *pos, unsigned char a3)
{
    struct Navigation *navi;
    int speed;
    {
        struct CreatureControl *cctrl;
        cctrl = creature_control_get_from_thing(creatng);
        navi = &cctrl->navi;
        speed = cctrl->max_speed;
        cctrl->field_2 = 0;
        cctrl->combat_flags = 0;
    }
    a3 |= (1 << creatng->owner);
    //return _DK_get_next_position_and_angle_required_to_tunnel_creature_to(creatng, pos, a3);
    MapSubtlCoord stl_x, stl_y;
    SubtlCodedCoords stl_num;
    struct Coord3d tmpos;
    int nav_radius;
    long angle, i;
    int block_flags, cannot_move;
    struct Map *mapblk;
    switch (navi->field_0)
    {
    case 1:
        if (get_2d_box_distance(&creatng->mappos, &navi->pos_1B) >= navi->field_9) {
            navi->field_4 = 0;
        }
        if (navi->field_4 == 0)
        {
            navi->field_D = get_angle_xy_to(&creatng->mappos, pos);
            navi->pos_1B.x.val = creatng->mappos.x.val + distance_with_angle_to_coord_x(speed, navi->field_D);
            navi->pos_1B.y.val = creatng->mappos.y.val + distance_with_angle_to_coord_y(speed, navi->field_D);
            navi->pos_1B.z.val = get_thing_height_at(creatng, &navi->pos_1B);
            if (get_2d_box_distance(&creatng->mappos, pos) < get_2d_box_distance(&creatng->mappos, &navi->pos_1B))
            {
                navi->pos_1B.x.val = pos->x.val;
                navi->pos_1B.y.val = pos->y.val;
                navi->pos_1B.z.val = pos->z.val;
            }

            cannot_move = creature_cannot_move_directly_to_with_collide(creatng, &navi->pos_1B, 33, a3);
            if (cannot_move == 4)
            {
                struct SlabMap *slb;
                stl_num = get_map_index_of_first_block_thing_colliding_with_travelling_to(creatng, &creatng->mappos, &navi->pos_1B, 40, 0);
                slb = get_slabmap_for_subtile(stl_num_decode_x(stl_num), stl_num_decode_y(stl_num));
                unsigned short ownflag;
                ownflag = 0;
                if (!slabmap_block_invalid(slb)) {
                    ownflag = 1 << slabmap_owner(slb);
                }
                navi->field_19[0] = ownflag;

                if (get_starting_angle_and_side_of_hug(creatng, &navi->pos_1B, &navi->field_D, navi->field_1, 33, a3))
                {
                    block_flags = get_hugging_blocked_flags(creatng, &navi->pos_1B, 33, a3);
                    set_hugging_pos_using_blocked_flags(&navi->pos_1B, creatng, block_flags, thing_nav_sizexy(creatng)/2);
                    if (block_flags == 4)
                    {
                        if ((navi->field_D == 0) || (navi->field_D == 0x0400))
                        {
                            navi->pos_1B.y.val = creatng->mappos.y.val;
                            navi->pos_1B.z.val = get_thing_height_at(creatng, &creatng->mappos);
                        } else
                        if ((navi->field_D == 0x0200) || (navi->field_D == 0x0600)) {
                            navi->pos_1B.x.val = creatng->mappos.x.val;
                            navi->pos_1B.z.val = get_thing_height_at(creatng, &creatng->mappos);
                        }
                    }
                    navi->field_4 = 1;
                } else
                {
                    navi->field_0 = 1;
                    navi->pos_21.x.val = pos->x.val;
                    navi->pos_21.y.val = pos->y.val;
                    navi->pos_21.z.val = pos->z.val;
                    navi->field_1[2] = 0;
                    navi->field_1[1] = 0;
                    navi->field_4 = 0;
                }
            }
            if (cannot_move == 1)
            {
                stl_num = get_map_index_of_first_block_thing_colliding_with_travelling_to(creatng, &creatng->mappos, &navi->pos_1B, 40, 0);
                navi->field_15 = stl_num;
                nav_radius = thing_nav_sizexy(creatng) / 2;
                MapSubtlCoord stl_x, stl_y;
                stl_x = slab_subtile_center(subtile_slab_fast(stl_num_decode_x(stl_num)));
                stl_y = slab_subtile_center(subtile_slab_fast(stl_num_decode_y(stl_num)));
                find_approach_position_to_subtile(&creatng->mappos, stl_x, stl_y, nav_radius + 385, &navi->pos_1B);
                navi->field_D = get_angle_xy_to(&creatng->mappos, &navi->pos_1B);
                navi->field_0 = 3;
                return 1;
            }
        }
        if (navi->field_4 > 0)
        {
            navi->field_4++;
            if (navi->field_4 > 32) {
                ERRORLOG("I've been pushing for a very long time now...");
            }
            if (get_2d_box_distance(&creatng->mappos, &navi->pos_1B) <= 16)
            {
                navi->field_4 = 0;
                navigation_push_towards_target(navi, creatng, pos, speed, thing_nav_sizexy(creatng)/2, a3);
            }
        }
        return 1;
    case 2:
        if (get_2d_box_distance(&creatng->mappos, &navi->pos_1B) > 16)
        {
            if ((get_2d_box_distance(&creatng->mappos, &navi->pos_1B) > navi->field_9)
              || creature_cannot_move_directly_to_with_collide(creatng, &navi->pos_1B, 33, a3))
            {
                navi->field_0 = 1;
                navi->pos_21.x.val = pos->x.val;
                navi->pos_21.y.val = pos->y.val;
                navi->pos_21.z.val = pos->z.val;
                navi->field_1[2] = 0;
                navi->field_1[1] = 0;
                navi->field_4 = 0;
                return 1;
            }
            return 1;
        }
        if ((get_2d_box_distance(&creatng->mappos, pos) < navi->field_5)
          && thing_can_continue_direct_line_to(creatng, &creatng->mappos, pos, 33, 1, a3))
        {
            navi->field_0 = 1;
            navi->pos_21.x.val = pos->x.val;
            navi->pos_21.y.val = pos->y.val;
            navi->pos_21.z.val = pos->z.val;
            navi->field_1[2] = 0;
            navi->field_1[1] = 0;
            navi->field_4 = 0;
            return 1;
        }
        if (creatng->field_52 != navi->field_D) {
            return 1;
        }
        angle = get_angle_of_wall_hug(creatng, 33, speed, a3);
        if (angle != navi->field_D)
        {
          tmpos.x.val = creatng->mappos.x.val + distance_with_angle_to_coord_x(speed, navi->field_D);
          tmpos.y.val = creatng->mappos.y.val + distance_with_angle_to_coord_y(speed, navi->field_D);
          tmpos.z.val = get_thing_height_at(creatng, &tmpos);
          if (creature_cannot_move_directly_to_with_collide(creatng, &tmpos, 33, a3) == 4)
          {
              block_flags = get_hugging_blocked_flags(creatng, &tmpos, 33, a3);
              set_hugging_pos_using_blocked_flags(&tmpos, creatng, block_flags, thing_nav_sizexy(creatng)/2);
              if (get_2d_box_distance(&tmpos, &creatng->mappos) > 16)
              {
                  navi->pos_1B.x.val = tmpos.x.val;
                  navi->pos_1B.y.val = tmpos.y.val;
                  navi->pos_1B.z.val = tmpos.z.val;
                  navi->field_9 = get_2d_box_distance(&creatng->mappos, &navi->pos_1B);
                  return 1;
              }
          }
        }
        if (((angle + 512) & 0x7FF) == navi->field_D)
        {
            if (navi->field_1[2] == 1)
            {
                navi->field_1[1]++;
            } else
            {
                navi->field_1[2] = 1;
                navi->field_1[1] = 1;
            }
        } else
        if (((angle - 512) & 0x7FF) == navi->field_D)
        {
          if (navi->field_1[2] == 2)
          {
              navi->field_1[1]++;
          } else
          {
              navi->field_1[2] = 2;
              navi->field_1[1] = 1;
          }
        } else
        {
          navi->field_1[1] = 0;
          navi->field_1[2] = 0;
        }
        if (navi->field_1[1] >= 4)
        {
            navi->field_0 = 1;
            navi->pos_21.x.val = pos->x.val;
            navi->pos_21.y.val = pos->y.val;
            navi->pos_21.z.val = pos->z.val;
            navi->field_1[2] = 0;
            navi->field_1[1] = 0;
            navi->field_4 = 0;
            return 1;
        }
        navi->field_D = angle;
        navi->pos_1B.x.val = creatng->mappos.x.val + distance_with_angle_to_coord_x(speed, navi->field_D);
        navi->pos_1B.y.val = creatng->mappos.y.val + distance_with_angle_to_coord_y(speed, navi->field_D);
        navi->pos_1B.z.val = get_thing_height_at(creatng, &navi->pos_1B);
        tmpos.x.val = navi->pos_1B.x.val;
        tmpos.y.val = navi->pos_1B.y.val;
        tmpos.z.val = navi->pos_1B.z.val;
        check_forward_for_prospective_hugs(creatng, &tmpos, navi->field_D, navi->field_1[0], 33, speed, a3);
        if (get_2d_box_distance(&tmpos, &creatng->mappos) > 16)
        {
            navi->pos_1B.x.val = tmpos.x.val;
            navi->pos_1B.y.val = tmpos.y.val;
            navi->pos_1B.z.val = tmpos.z.val;
        }
        navi->field_9 = get_2d_box_distance(&creatng->mappos, &navi->pos_1B);
        cannot_move = creature_cannot_move_directly_to_with_collide(creatng, &navi->pos_1B, 33, a3);
        if (cannot_move == 4)
        {
          ERRORLOG("I've been given a shite position");
          tmpos.x.val = creatng->mappos.x.val + distance_with_angle_to_coord_x(speed, navi->field_D);
          tmpos.y.val = creatng->mappos.y.val + distance_with_angle_to_coord_y(speed, navi->field_D);
          tmpos.z.val = get_thing_height_at(creatng, &tmpos);
          if (creature_cannot_move_directly_to_with_collide(creatng, &tmpos, 33, a3) == 4) {
              ERRORLOG("It's even more shit than I first thought");
          }
          navi->field_0 = 1;
          navi->pos_21.x.val = pos->x.val;
          navi->pos_21.y.val = pos->y.val;
          navi->pos_21.z.val = pos->z.val;
          navi->field_1[2] = 0;
          navi->field_1[1] = 0;
          navi->field_4 = 0;
          navi->pos_1B.x.val = creatng->mappos.x.val;
          navi->pos_1B.y.val = creatng->mappos.y.val;
          navi->pos_1B.z.val = creatng->mappos.z.val;
          return 1;
        }
        if (cannot_move != 1)
        {
            navi->field_9 = get_2d_box_distance(&creatng->mappos, &navi->pos_1B);
            return 1;
        }
        stl_num = get_map_index_of_first_block_thing_colliding_with_travelling_to(creatng, &creatng->mappos, &navi->pos_1B, 40, 0);
        navi->field_15 = stl_num;
        nav_radius = thing_nav_sizexy(creatng) / 2;
        stl_x = slab_subtile_center(subtile_slab_fast(stl_num_decode_x(stl_num)));
        stl_y = slab_subtile_center(subtile_slab_fast(stl_num_decode_y(stl_num)));
        find_approach_position_to_subtile(&creatng->mappos, stl_x, stl_y, nav_radius + 385, &navi->pos_1B);
        navi->field_D = get_angle_xy_to(&creatng->mappos, &navi->pos_1B);
        navi->field_1[1] = 0;
        navi->field_1[2] = 0;
        navi->field_9 = get_2d_box_distance(&creatng->mappos, &navi->pos_1B);
        navi->field_0 = 4;
        return 1;
    case 4:
        if (get_2d_box_distance(&creatng->mappos, &navi->pos_1B) > 16)
        {
            if (get_2d_box_distance(&creatng->mappos, &navi->pos_1B) > navi->field_9
             || creature_cannot_move_directly_to_with_collide(creatng, &navi->pos_1B, 33, a3))
            {
                navi->field_0 = 1;
                navi->pos_21.x.val = pos->x.val;
                navi->pos_21.y.val = pos->y.val;
                navi->pos_21.z.val = pos->z.val;
                navi->field_1[2] = 0;
                navi->field_1[1] = 0;
                navi->field_4 = 0;
            }
            navi->field_0 = 4;
            return 1;
        }
        navi->field_0 = 4;
        stl_x = slab_subtile_center(subtile_slab_fast(stl_num_decode_x(navi->field_15)));
        stl_y = slab_subtile_center(subtile_slab_fast(stl_num_decode_y(navi->field_15)));
        tmpos.x.val = subtile_coord_center(stl_x);
        tmpos.y.val = subtile_coord_center(stl_y);
        navi->field_D = get_angle_xy_to(&creatng->mappos, &tmpos);
        navi->field_1[1] = 0;
        navi->field_1[2] = 0;
        navi->field_9 = 0;
        if (get_angle_difference(creatng->field_52, navi->field_D) != 0) {
            return 1;
        }
        navi->field_0 = 6;
        stl_num = get_subtile_number(stl_x,stl_y);
        navi->field_15 = stl_num;
        navi->field_17 = stl_num;
        return 2;
    case 3:
        if (get_2d_box_distance(&creatng->mappos, &navi->pos_1B) > 16)
        {
            navi->field_D = get_angle_xy_to(&creatng->mappos, &navi->pos_1B);
            navi->field_0 = 3;
            return 1;
        }
        navi->field_0 = 3;
        stl_x = slab_subtile_center(subtile_slab_fast(stl_num_decode_x(navi->field_15)));
        stl_y = slab_subtile_center(subtile_slab_fast(stl_num_decode_y(navi->field_15)));
        tmpos.x.val = subtile_coord_center(stl_x);
        tmpos.y.val = subtile_coord_center(stl_y);
        navi->field_D = get_angle_xy_to(&creatng->mappos, &tmpos);
        if (get_angle_difference(creatng->field_52, navi->field_D) != 0) {
            return 1;
        }
        navi->field_0 = 5;
        stl_num = get_subtile_number(stl_x,stl_y);
        navi->field_15 = stl_num;
        navi->field_17 = stl_num;
        return 2;
    case 6:
        stl_x = slab_subtile_center(subtile_slab_fast(stl_num_decode_x(navi->field_15)));
        stl_y = slab_subtile_center(subtile_slab_fast(stl_num_decode_y(navi->field_15)));
        stl_num = get_subtile_number(stl_x,stl_y);
        navi->field_15 = stl_num;
        navi->field_17 = stl_num;
        mapblk = get_map_block_at_pos(navi->field_15);
        if ((mapblk->flags & MapFlg_IsTall) != 0) {
          return 2;
        }
        nav_radius = thing_nav_sizexy(creatng) / 2;
        if (navi->field_1[0] == 1) {
            i = (creatng->field_52 + LbFPMath_PI/4) / (LbFPMath_PI/2) - 1;
        } else {
            i = (creatng->field_52 + LbFPMath_PI/4) / (LbFPMath_PI/2) + 1;
        }
        navi->pos_1B.x.val += (384 - nav_radius) * small_around[i&3].delta_x;
        navi->pos_1B.y.val += (384 - nav_radius) * small_around[i&3].delta_y;
        i = (creatng->field_52 + LbFPMath_PI/4) / (LbFPMath_PI/2);
        navi->pos_1B.x.val += (128) * small_around[i&3].delta_x;
        i = (creatng->field_52) / (LbFPMath_PI/2);
        navi->pos_1B.y.val += (128) * small_around[i&3].delta_y;
        navi->field_0 = 7;
        return 1;
    case 5:
        stl_x = slab_subtile_center(subtile_slab_fast(stl_num_decode_x(navi->field_15)));
        stl_y = slab_subtile_center(subtile_slab_fast(stl_num_decode_y(navi->field_15)));
        stl_num = get_subtile_number(stl_x,stl_y);
        navi->field_15 = stl_num;
        navi->field_17 = stl_num;
        mapblk = get_map_block_at_pos(navi->field_15);
        if ((mapblk->flags & MapFlg_IsTall) != 0) {
            return 2;
        }
        navi->field_0 = 1;
        return 1;
    case 7:
        if (get_2d_box_distance(&creatng->mappos, &navi->pos_1B) > 16)
        {
            return 1;
        }
        if (navi->field_1[0] == 1)
            angle = creatng->field_52 + LbFPMath_PI/2;
        else
            angle = creatng->field_52 - LbFPMath_PI/2;
        navi->field_D = angle & LbFPMath_AngleMask;
        navi->field_0 = 2;
        return 1;
    default:
        break;
    }
    return 1;
}

TbBool slab_good_for_computer_dig_path(const struct SlabMap *slb)
{
    const struct SlabAttr *slbattr;
    slbattr = get_slab_attrs(slb);
    if ( ((slbattr->flags & (SlbAtFlg_Filled|SlbAtFlg_Digable|SlbAtFlg_Valuable)) != 0) || (slb->kind == SlbT_LAVA) )
        return true;
    return false;
}

TbBool is_valid_hug_subtile(MapSubtlCoord stl_x, MapSubtlCoord stl_y, PlayerNumber plyr_idx)
{
    struct SlabMap *slb;
    const struct SlabAttr *slbattr;
    slb = get_slabmap_for_subtile(stl_x, stl_y);
    slbattr = get_slab_attrs(slb);
    if ((slbattr->is_unknflg14) && (slb->kind != SlbT_GEMS))
    {
        struct Map *mapblk;
        mapblk = get_map_block_at(stl_x, stl_y);
        if (((mapblk->flags & MapFlg_Unkn20) == 0) || (slabmap_owner(slb) == plyr_idx)) {
            SYNCDBG(17,"Subtile (%d,%d) rejected based on attrs",(int)stl_x,(int)stl_y);
            return false;
        }
    }
    if (!slab_good_for_computer_dig_path(slb)) {
        SYNCDBG(17,"Subtile (%d,%d) rejected as not good for dig",(int)stl_x,(int)stl_y);
        return false;
    }
    return true;
}

long dig_to_position(PlayerNumber plyr_idx, MapSubtlCoord basestl_x, MapSubtlCoord basestl_y, int direction_around, TbBool revside)
{
    long i,n,nchange;
    //return _DK_dig_to_position(a1, a2, a3, start_side, revside);
    SYNCDBG(14,"Starting for subtile (%d,%d)",(int)basestl_x,(int)basestl_y);
    if (revside) {
      nchange = 1;
    } else {
      nchange = 3;
    }
    n = (direction_around + 4 - nchange) & 3;
    for (i=0; i < 4; i++)
    {
        MapSubtlCoord stl_x,stl_y;
        stl_x = basestl_x + STL_PER_SLB * (int)small_around[n].delta_x;
        stl_y = basestl_y + STL_PER_SLB * (int)small_around[n].delta_y;
        if (!is_valid_hug_subtile(stl_x, stl_y, plyr_idx))
        {
            SYNCDBG(7,"Subtile (%d,%d) accepted",(int)stl_x,(int)stl_y);
            SubtlCodedCoords stl_num;
            stl_num = get_subtile_number(stl_x, stl_y);
            return stl_num;
        }
        n = (n + nchange) & 3;
    }
    return -1;
}

inline void get_hug_side_next_step(MapSubtlCoord dst_stl_x, MapSubtlCoord dst_stl_y, int dirctn, PlayerNumber plyr_idx,
    char *state, MapSubtlCoord *ostl_x, MapSubtlCoord *ostl_y, short *round, int *maxdist)
{
    MapSubtlCoord curr_stl_x, curr_stl_y;
    curr_stl_x = *ostl_x;
    curr_stl_y = *ostl_y;
    unsigned short round_idx;
    round_idx = small_around_index_in_direction(curr_stl_x, curr_stl_y, dst_stl_x, dst_stl_y);
    int dist;
    dist = max(abs(curr_stl_x - dst_stl_x), abs(curr_stl_y - dst_stl_y));
    int dx,dy;
    dx = small_around[round_idx].delta_x;
    dy = small_around[round_idx].delta_y;
    if ((dist <= *maxdist) && !is_valid_hug_subtile(curr_stl_x + STL_PER_SLB*dx, curr_stl_y + STL_PER_SLB*dy, plyr_idx))
    {
        curr_stl_x += STL_PER_SLB*dx;
        curr_stl_y += STL_PER_SLB*dy;
        *state = 1;
        *maxdist = max(abs(curr_stl_x - dst_stl_x), abs(curr_stl_y - dst_stl_y));
    } else
    if (*state == 1)
    {
        *state = 2;
    } else
    {
        int n;
        round_idx = (*round + dirctn) % SMALL_AROUND_LENGTH;
        for (n = 0; n < SMALL_AROUND_LENGTH; n++)
        {
            dx = small_around[round_idx].delta_x;
            dy = small_around[round_idx].delta_y;
            if (!is_valid_hug_subtile(curr_stl_x + STL_PER_SLB*dx, curr_stl_y + STL_PER_SLB*dy, plyr_idx))
            {
                *round = round_idx;
                curr_stl_x += STL_PER_SLB*dx;
                curr_stl_y += STL_PER_SLB*dy;
                break;
            }
            round_idx = (round_idx - dirctn) % SMALL_AROUND_LENGTH;
        }
    }
    *ostl_x = curr_stl_x;
    *ostl_y = curr_stl_y;
}

short get_hug_side_options(MapSubtlCoord src_stl_x, MapSubtlCoord src_stl_y, MapSubtlCoord dst_stl_x, MapSubtlCoord dst_stl_y,
    unsigned short direction, PlayerNumber plyr_idx, MapSubtlCoord *ostla_x, MapSubtlCoord *ostla_y, MapSubtlCoord *ostlb_x, MapSubtlCoord *ostlb_y)
{
    SYNCDBG(4,"Starting");
    MapSubtlCoord stl_b_x, stl_b_y;
    MapSubtlCoord stl_a_x, stl_a_y;
    int maxdist_a,maxdist_b;
    short round_a,round_b;
    char state_a, state_b;
    int dist;
    dist = max(abs(src_stl_x - dst_stl_x), abs(src_stl_y - dst_stl_y));

    state_a = 0;
    stl_a_x = src_stl_x;
    stl_a_y = src_stl_y;
    round_a = (direction + 1) % SMALL_AROUND_LENGTH;
    maxdist_a = dist - 1;

    state_b = 0;
    stl_b_x = src_stl_x;
    stl_b_y = src_stl_y;
    round_b = (direction - 1) % SMALL_AROUND_LENGTH;
    maxdist_b = dist - 1;

    int i;
    for (i = 150; i > 0; i--)
    {
        if ((state_a == 2) && (state_b == 2)) {
            break;
        }
        if (state_a != 2)
        {
            get_hug_side_next_step(dst_stl_x, dst_stl_y, -1, plyr_idx, &state_a, &stl_a_x, &stl_a_y, &round_a, &maxdist_a);
            if ((stl_a_x == dst_stl_x) && (stl_a_y == dst_stl_y)) {
                *ostla_x = stl_a_x;
                *ostla_y = stl_a_y;
                *ostlb_x = stl_b_x;
                *ostlb_y = stl_b_y;
                return 1;
            }
        }
        if (state_b != 2)
        {
            get_hug_side_next_step(dst_stl_x, dst_stl_y, 1, plyr_idx, &state_b, &stl_b_x, &stl_b_y, &round_b, &maxdist_b);
            if ((stl_b_x == dst_stl_x) && (stl_b_y == dst_stl_y)) {
                *ostla_x = stl_a_x;
                *ostla_y = stl_a_y;
                *ostlb_x = stl_b_x;
                *ostlb_y = stl_b_y;
                return 0;
            }
        }
    }
    *ostla_x = stl_a_x;
    *ostla_y = stl_a_y;
    *ostlb_x = stl_b_x;
    *ostlb_y = stl_b_y;
    return 2;
}
/******************************************************************************/
#ifdef __cplusplus
}
#endif
