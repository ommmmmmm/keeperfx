/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file thing_data.c
 *     Thing struct support functions.
 * @par Purpose:
 *     Functions to maintain thing structure.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     17 Jun 2010 - 07 Jul 2010
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include "thing_data.h"

#include "globals.h"
#include "bflib_basics.h"
#include "bflib_sound.h"
#include "bflib_memory.h"
#include "thing_stats.h"

#include "keeperfx.hpp"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
DLLIMPORT void _DK_delete_thing_structure(struct Thing *thing, long a2);
DLLIMPORT struct Thing *_DK_allocate_free_thing_structure(unsigned char a1);
DLLIMPORT unsigned char _DK_i_can_allocate_free_thing_structure(unsigned char a1);
DLLIMPORT unsigned char _DK_creature_remove_lair_from_room(struct Thing *thing, struct Room *room);

/******************************************************************************/
struct Thing *allocate_free_thing_structure_f(unsigned char allocflags, const char *func_name)
{
    struct Thing *thing;
    long i;
    //return _DK_allocate_free_thing_structure(allocflags);

    // Get a thing from "free things list"
    i = game.free_things_start_index;
    // If there is no free thing, try to free an effect
    if (i >= THINGS_COUNT-1)
    {
        if ((allocflags & TAF_FreeEffectIfNoSlots) != 0)
        {
            thing = thing_get(game.thing_lists[TngList_EffectElems].index);
            if (!thing_is_invalid(thing))
            {
                delete_thing_structure(thing, 0);
            } else
            {
#if (BFDEBUG_LEVEL > 0)
                ERRORMSG("%s: Cannot free up effect element to allocate new thing!",func_name);
#endif
            }
        }
        i = game.free_things_start_index;
    }
    // Now, if there is still no free thing (we couldn't free any)
    if (i >= THINGS_COUNT-1)
    {
#if (BFDEBUG_LEVEL > 0)
        ERRORMSG("%s: Cannot allocate new thing, no free slots!",func_name);
#endif
        return INVALID_THING;
    }
    // And if there is free one, allocate it
    thing = thing_get(game.free_things[i]);
#if (BFDEBUG_LEVEL > 0)
    if (thing_exists(thing)) {
        ERRORMSG("%s: Found existing thing %d in free things list!",func_name,(int)i);
    }
#endif
    LbMemorySet(thing, 0, sizeof(struct Thing));
    if (thing_is_invalid(thing)) {
        ERRORMSG("%s: Got invalid thing slot instead of free one!",func_name);
        return INVALID_THING;
    }
    thing->field_0 |= TF_Exists;
    thing->index = game.free_things[i];
    game.free_things[game.free_things_start_index] = 0;
    game.free_things_start_index++;
    return thing;
}

TbBool i_can_allocate_free_thing_structure(unsigned char allocflags)
{
    //return _DK_i_can_allocate_free_thing_structure(allocflags);
    // Check if there are free slots
    if (game.free_things_start_index < THINGS_COUNT-1)
        return true;
    // Check if there are effect slots that could be freed
    if ((allocflags & TAF_FreeEffectIfNoSlots) != 0)
    {
        if (game.thing_lists[TngList_EffectElems].index > 0)
            return true;
    }
    // Couldn't find free slot - fail
    if ((allocflags & TAF_LogFailures) != 0)
    {
        ERRORLOG("Cannot allocate thing structure.");
        things_stats_debug_dump();
    }
    return false;
}

unsigned char creature_remove_lair_from_room(struct Thing *thing, struct Room *room)
{
    return _DK_creature_remove_lair_from_room(thing, room);
}

void delete_thing_structure_f(struct Thing *thing, long a2, const char *func_name)
{
    struct CreatureControl *cctrl;
    struct Room *room;
    //_DK_delete_thing_structure(thing, a2); return;
    cctrl = creature_control_get_from_thing(thing);
    if ((thing->field_0 & TF_Unkn08) != 0) {
        remove_first_creature(thing);
    }
    if (!a2)
    {
        if (thing->light_id != 0) {
            light_delete_light(thing->light_id);
            thing->light_id = 0;
        }
    }
    if (!creature_control_invalid(cctrl))
    {
      if ( !a2 )
      {
          room = room_get(cctrl->lair_room_id);
          if (!room_is_invalid(room)) {
              creature_remove_lair_from_room(thing, room);
          }
          if (creature_is_group_member(thing)) {
              remove_creature_from_group(thing);
          }
      }
      delete_control_structure(cctrl);
    }
    if (thing->snd_emitter_id != 0) {
        S3DDestroySoundEmitterAndSamples(thing->snd_emitter_id);
        thing->snd_emitter_id = 0;
    }
    remove_thing_from_its_class_list(thing);
    remove_thing_from_mapwho(thing);
    if (thing->index > 0) {
        game.free_things_start_index--;
        game.free_things[game.free_things_start_index] = thing->index;
    } else {
#if (BFDEBUG_LEVEL > 0)
        ERRORMSG("%s: Performed deleting of thing with bad index!",func_name);
#endif
    }
    LbMemorySet(thing, 0, sizeof(struct Thing));
}

/**
 * Returns thing of given array index.
 * @param tng_idx
 * @return Returns thing, or invalid thing pointer if not found.
 */
struct Thing *thing_get(long tng_idx)
{
    if ((tng_idx > 0) && (tng_idx < THINGS_COUNT)) {
        return game.things.lookup[tng_idx];
    }
    if ((tng_idx < -1) || (tng_idx >= THINGS_COUNT)) {
        ERRORLOG("Request of invalid thing (no %ld) intercepted",tng_idx);
    }
    return INVALID_THING;
}

long thing_get_index(const struct Thing *thing)
{
    long tng_idx;
    tng_idx = (thing - game.things.lookup[0]);
    if ((tng_idx > 0) && (tng_idx < THINGS_COUNT))
        return tng_idx;
    return 0;
}

short thing_is_invalid(const struct Thing *thing)
{
    return (thing <= game.things.lookup[0]) || (thing > game.things.lookup[THINGS_COUNT-1]) || (thing == NULL);
}

TbBool thing_exists_idx(long tng_idx)
{
    return thing_exists(thing_get(tng_idx));
}

TbBool thing_exists(const struct Thing *thing)
{
    if (thing_is_invalid(thing))
        return false;
    if ((thing->field_0 & TF_Exists) == 0)
        return false;
#if (BFDEBUG_LEVEL > 0)
    if (thing->index != (thing-thing_get(0)))
        WARNLOG("Incorrectly indexed thing (%d) at pos %d",(int)thing->index,(int)(thing-thing_get(0)));
    if ((thing->class_id < 1) || (thing->class_id >= THING_CLASSES_COUNT))
        WARNLOG("Thing %d is of invalid class %d",(int)thing->index,(int)thing->class_id);
#endif
    return true;
}

TbBool thing_touching_floor(const struct Thing *thing)
{
    return (thing->field_60 == thing->mappos.z.val);
}

struct PlayerInfo *get_player_thing_is_controlled_by(const struct Thing *thing)
{
    if ((thing->field_0 & 0x20) == 0)
        return INVALID_PLAYER;
    return get_player(thing->owner);
}

/******************************************************************************/
#ifdef __cplusplus
}
#endif
