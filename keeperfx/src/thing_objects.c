/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file thing_objects.c
 *     Things of class 'object' handling functions.
 * @par Purpose:
 *     Functions to maintain object things in the game.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     05 Nov 2009 - 01 Jan 2010
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include "thing_objects.h"

#include "globals.h"
#include "bflib_basics.h"
#include "bflib_memory.h"
#include "bflib_math.h"
#include "bflib_sound.h"
#include "config_strings.h"
#include "config_objects.h"
#include "thing_stats.h"
#include "thing_effects.h"
#include "thing_navigate.h"
#include "thing_physics.h"
#include "map_data.h"
#include "map_columns.h"
#include "gui_topmsg.h"
#include "engine_arrays.h"
#include "sounds.h"
#include "game_legacy.h"
#include "keeperfx.hpp"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
long food_moves(struct Thing *heartng);
long food_grows(struct Thing *heartng);
long object_being_dropped(struct Thing *heartng);
TngUpdateRet object_update_dungeon_heart(struct Thing *heartng);
TngUpdateRet object_update_call_to_arms(struct Thing *heartng);
TngUpdateRet object_update_armour(struct Thing *heartng);
TngUpdateRet object_update_object_scale(struct Thing *heartng);
TngUpdateRet object_update_armour2(struct Thing *heartng);
TngUpdateRet object_update_power_sight(struct Thing *heartng);
TngUpdateRet object_update_power_lightning(struct Thing *heartng);

Thing_State_Func object_state_functions[] = {
    NULL,
    food_moves,
    food_grows,
    NULL,
    object_being_dropped,
    NULL,
};

Thing_Class_Func object_update_functions[] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    object_update_dungeon_heart,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    object_update_call_to_arms,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    object_update_armour,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    object_update_object_scale,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    object_update_armour2,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    object_update_power_sight,
    object_update_power_lightning,
    object_update_object_scale,
    object_update_object_scale,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

/**
 * Objects config array.
 *
 * Originally was named objects[].
 */
struct Objects objects_data[] = {
  {0, 0, 0, 0, 0,   0, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 0, 0}, //0
  {0, 0, 0, 0, 0, 930, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 3, 1}, //1
  {0, 0, 1, 0, 1, 962, 0x0100,    0,    0, 300, 0, 1, 2, 0,  0, 2, 1}, //2
  {0, 0, 0, 0, 0, 934, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 1}, //3
  {0, 0, 0, 0, 0, 950, 0x0100,    0,    0, 300, 0, 1, 2, 0,  0, 3, 1}, //4
  {0, 0, 0, 0, 0, 948, 0x0100,0x200,0x200, 525, 0, 0, 2, 0,  0, 0, 0}, //5
  {0, 0, 0, 0, 0, 934, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 1}, //6
  {0, 0, 0, 0, 1, 962, 0x0100,    0,    0, 300, 0, 1, 2, 0,  0, 2, 1}, //7
  {0, 0, 0, 0, 0, 950, 0x0100,    0,    0, 300, 0, 1, 2, 0,  0, 3, 1}, //8
  {2, 0, 0, 0, 0, 893, 0x0008, 0x80, 0x80, 300, 0, 0, 2, 1,  0, 2, 1}, //9
  {1, 0, 0, 0, 0, 819, 0x0100, 0x80, 0x80, 300, 0, 0, 2, 0,  0, 2, 1}, //10
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0}, //11
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 0,   0, 0x0100,    0,    0, 375, 0, 0, 2, 0,  0, 1, 0}, //24
  {0, 0, 0, 0, 0,   0, 0x0100,    0,    0, 300, 0, 0, 5, 0,  0, 0, 0}, //25
  {0, 0, 0, 0, 0, 789, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 2, 1}, //26
  {0, 0, 0, 0, 0, 796, 0x0100,    0,    0, 200, 0, 0, 2, 1,  0, 2, 1}, //27
  {0, 0, 1, 0, 0, 791, 0x0100,    0,    0, 300, 0, 1, 2, 0,  0, 3, 1}, //28
  {0, 0, 0, 0, 0, 793, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 2, 1}, //29
  {0, 0, 0, 0, 0, 905, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 3, 1}, //30
  {0, 0, 1, 0, 0, 795, 0x0100,    0,    0, 375, 0, 0, 2, 1,  0, 2, 1}, //31
  {0, 0, 1, 0, 0, 892, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 2, 1}, //32
  {0, 0, 0, 0, 0, 797, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 2, 1}, //33
  {0, 0, 0, 0, 0, 804, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 2, 1}, //34
  {0, 0, 0, 0, 0, 806, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 2, 1}, //35
  {0, 0, 0, 0, 0, 808, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 2, 1}, //36
  {0, 0, 0, 0, 0, 782, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 2, 0}, //37
  {0, 0, 0, 0, 0, 783, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 2, 0}, //38
  {0, 0, 0, 0, 0, 785, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 2, 0}, //39
  {2, 0, 0, 0, 0, 894, 0x0100, 0x80, 0x80, 300, 0, 0, 2, 1,  0, 2, 1}, //40
  {2, 0, 0, 0, 0, 895, 0x00C0, 0x80, 0x80, 300, 0, 0, 2, 1,  0, 2, 1}, //41
  {2, 0, 0, 0, 0, 896, 0x00C0, 0x80, 0x80, 300, 0, 0, 2, 1,  0, 2, 1}, //42
  {0, 0, 0, 0, 0, 936, 0x0055, 0x80, 0x40, 300, 0, 0, 2, 1,  0, 1, 1}, //43
  {0, 0, 0, 0, 0, 810, 0x0100,    0,    0, 300, 0, 0, 6, 0,  0, 2, 0}, //44
  {0, 0, 0, 0, 0, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0}, //45
  {0, 0, 0, 0, 0, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 0, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 0, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 0, 0, 0, 776, 0x0100,    0,    0, 300, 3, 0, 2, 0,  0, 1, 0}, //49
  {0, 0, 0, 0, 0, 818, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 0, 0}, //50
  {0, 0, 1, 0, 0, 850, 0x0100,    0,    0, 144, 3, 0, 2, 0,  0, 0, 0}, //51
  {0, 0, 0, 0, 0, 936, 0x0080,    0,    0, 375, 0, 0, 2, 0,  0, 1, 1}, //52
  {0, 0, 0, 0, 0, 937, 0x0080,    0,    0, 375, 0, 0, 2, 0,  0, 1, 1}, //53
  {0, 0, 0, 0, 0, 938, 0x0080,    0,    0, 375, 0, 0, 2, 0,  0, 1, 1}, //54
  {0, 0, 0, 0, 0, 939, 0x0080,    0,    0, 375, 0, 0, 2, 0,  0, 1, 1}, //55
  {0, 0, 0, 0, 0, 940, 0x0080,    0,    0, 375, 0, 0, 2, 0,  0, 1, 1}, //56
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0,  1, 2, 0}, //57
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0,  2, 2, 0}, //58
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0,  3, 2, 0}, //59
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0,  4, 2, 0}, //60
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0,  5, 2, 0}, //61
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0,  6, 2, 0}, //62
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0,  7, 2, 0}, //63
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0,  8, 2, 0}, //64
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0,  9, 2, 0}, //65
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0, 10, 2, 0}, //66
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0, 11, 2, 0}, //67
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0, 12, 2, 0}, //68
  {0, 0, 0, 0, 0, 124, 0x0100,    0,    0, 300, 0, 0, 2, 0, 13, 2, 0}, //69
  {0, 0, 0, 0, 0, 158, 0x0100,    0,    0, 300, 0, 0, 2, 0, 14, 2, 0}, //70
  {0, 0, 0, 0, 0, 156, 0x0100,    0,    0, 300, 0, 0, 2, 0, 15, 2, 0}, //71
  {0, 0, 0, 0, 0, 154, 0x0100,    0,    0, 300, 0, 0, 2, 0, 16, 2, 0}, //72
  {0, 0, 0, 0, 0, 152, 0x0100,    0,    0, 300, 0, 0, 2, 0, 17, 2, 0}, //73
  {0, 0, 0, 0, 0, 150, 0x0100,    0,    0, 300, 0, 0, 2, 0, 18, 2, 0}, //74
  {0, 0, 0, 0, 0, 148, 0x0100,    0,    0, 300, 0, 0, 2, 0, 19, 2, 0}, //75
  {0, 0, 0, 0, 0, 146, 0x0100,    0,    0, 300, 0, 0, 2, 0, 20, 2, 0}, //76
  {0, 0, 0, 0, 0, 144, 0x0100,    0,    0, 300, 0, 0, 2, 0, 21, 2, 0}, //77
  {0, 0, 0, 0, 0, 142, 0x0100,    0,    0, 300, 0, 0, 2, 0, 22, 2, 0}, //78
  {0, 0, 0, 0, 0, 152, 0x0100,    0,    0, 300, 0, 0, 2, 0, 23, 2, 0}, //79
  {0, 0, 0, 0, 0, 140, 0x0100,    0,    0, 300, 0, 0, 2, 0, 24, 2, 0}, //80
  {0, 0, 0, 0, 0, 138, 0x0100,    0,    0, 300, 0, 0, 2, 0, 25, 2, 0}, //81
  {0, 0, 0, 0, 0, 136, 0x0100,    0,    0, 300, 0, 0, 2, 0, 26, 2, 0}, //82
  {0, 0, 0, 0, 0, 134, 0x0100,    0,    0, 300, 0, 0, 2, 0, 27, 2, 0}, //83
  {0, 0, 0, 0, 0, 132, 0x0100,    0,    0, 300, 0, 0, 2, 0, 28, 2, 0}, //84
  {0, 0, 0, 0, 0, 128, 0x0100,    0,    0, 300, 0, 0, 2, 0, 29, 2, 0}, //85
  {0, 0, 1, 0, 0, 901, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0}, //86
  {0, 0, 1, 0, 0, 901, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0},
  {0, 0, 1, 0, 0, 901, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0},
  {0, 0, 1, 0, 0, 901, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0},
  {0, 0, 1, 0, 0, 901, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0},
  {0, 0, 1, 0, 0, 901, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0},
  {0, 0, 1, 0, 0, 901, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0},
  {0, 0, 1, 0, 0, 901, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0}, //94
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0}, //100
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0}, //106
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0},
  {0, 0, 1, 0, 0, 114, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0},
  {0, 0, 0, 0, 0, 789, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 1, 0}, //110
  {0, 0, 1, 0, 0, 798, 0x0100,    0,    0, 300, 3, 0, 2, 0,  0, 2, 0}, //111
  {0, 0, 1, 0, 0, 851, 0x0100,    0,    0, 300, 3, 0, 2, 0,  0, 0, 0}, //112
  {0, 0, 1, 0, 0, 130, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 2, 0}, //113
  {0, 0, 1, 0, 0,  98, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 2, 0}, //114
  {0, 0, 1, 0, 0, 102, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 2, 0}, //115
  {0, 0, 1, 0, 0, 104, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 2, 0}, //116
  {0, 0, 1, 0, 0, 106, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 2, 0}, //117
  {0, 0, 1, 0, 0, 108, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 2, 0}, //118
  {0, 0, 1, 0, 0, 100, 0x0080,    0,    0, 300, 0, 0, 2, 0,  0, 2, 0}, //119
  {0, 0, 1, 0, 0, 799, 0x0100,    0,    0, 300, 3, 0, 2, 0,  0, 2, 0}, //120
  {0, 0, 1, 0, 0, 800, 0x0100,    0,    0, 300, 3, 0, 2, 0,  0, 2, 0}, //121
  {0, 0, 1, 0, 0, 801, 0x0100,    0,    0, 300, 3, 0, 2, 0,  0, 2, 0}, //122
  {0, 0, 0, 0, 0,   0, 0x0000,    0,    0,   0, 0, 0, 0, 0,  0, 0, 0}, //123
  {0, 0, 0, 0, 0,   0, 0x0000,    0,    0,   0, 0, 0, 0, 0,  0, 0, 0}, //124
  {0, 0, 0, 0, 0,  46, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 2, 0}, //125
  {0, 0, 0, 0, 0, 126, 0x0100,    0,    0, 300, 0, 0, 2, 0, 30, 2, 0}, //126
  {0, 0, 0, 0, 1, 781, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 0, 0}, //127
  {4, 0, 1, 0, 1, 780, 0x0100,    0,    0, 500, 0, 0, 2, 1,  0, 0, 0}, //128
  {0, 0, 0, 0, 0, 952, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 3, 1}, //129
  {0, 0, 0, 0, 0, 954, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 3, 1}, //130
  {0, 0, 0, 0, 0, 956, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 3, 1}, //131
  {0, 0, 0, 0, 0, 958, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 3, 1}, //132
  {0, 0, 0, 0, 0, 960, 0x0100,    0,    0, 300, 0, 0, 2, 0,  0, 3, 1}, //133
  {0, 0, 0, 0, 1, 777, 0x0100,    0,    0, 300, 0, 0, 2, 1,  0, 1, 0}, //134
  {0, 0, 0, 0, 0,   0, 0x0000,    0,    0,   0, 0, 0, 0, 0,  0, 0, 0},
  {0, 0, 0, 0, 0,   0, 0x0000,    0,    0,   0, 0, 0, 0, 0,  0, 0, 0},
};

ThingModel object_to_special[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

ThingModel object_to_magic[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5,
    6, 7, 8, 9,10,11,12,13, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,14,15,16,
   17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,19, 0,
};

/** Guard flag objects model per player index. Originally named guard_post_objects.
 */
unsigned short player_guardflag_objects[] = {115, 116, 117, 118,  0, 119};
/** Dungeon Heart flame objects model per player index.
 */
unsigned short dungeon_flame_objects[] =    {111, 120, 121, 122,  0,   0};
unsigned short lightning_spangles[] = {83, 90, 91, 92, 0, 0};
unsigned short gold_hoard_objects[] = {52, 53, 54, 55, 56};
unsigned short specials_text[] = {GUIStr_Empty, 420, 421, 422, 423, 424, 425, 426, 427, 0};

struct CallToArmsGraphics call_to_arms_graphics[] = {
    {867, 868, 869},
    {873, 874, 875},
    {879, 880, 881},
    {885, 886, 887},
    {  0,   0,   0}
};

/******************************************************************************/
DLLIMPORT long _DK_move_object(struct Thing *heartng);
DLLIMPORT long _DK_update_object(struct Thing *heartng);
DLLIMPORT long _DK_food_moves(struct Thing *heartng);
DLLIMPORT long _DK_food_grows(struct Thing *heartng);
DLLIMPORT long _DK_object_being_dropped(struct Thing *heartng);
DLLIMPORT long _DK_object_update_dungeon_heart(struct Thing *heartng);
DLLIMPORT long _DK_object_update_call_to_arms(struct Thing *heartng);
DLLIMPORT long _DK_object_update_armour(struct Thing *heartng);
DLLIMPORT long _DK_object_update_object_scale(struct Thing *heartng);
DLLIMPORT long _DK_object_update_armour2(struct Thing *heartng);
DLLIMPORT long _DK_object_update_power_sight(struct Thing *heartng);
DLLIMPORT long _DK_object_update_power_lightning(struct Thing *heartng);
DLLIMPORT long _DK_object_is_gold_pile(struct Thing *heartng);
DLLIMPORT long _DK_object_is_gold(struct Thing *heartng);
DLLIMPORT long _DK_remove_gold_from_hoarde(struct Thing *heartng, struct Room *room, long amount);
DLLIMPORT struct Thing *_DK_create_object(struct Coord3d *pos, unsigned short model, unsigned short owner, long parent_idx);
DLLIMPORT long _DK_thing_is_spellbook(struct Thing *heartng);
DLLIMPORT struct Thing *_DK_get_crate_at_position(long x, long y);
DLLIMPORT struct Thing *_DK_get_spellbook_at_position(long x, long y);
DLLIMPORT struct Thing *_DK_get_special_at_position(long x, long y);
DLLIMPORT long _DK_add_gold_to_hoarde(struct Thing *heartng, struct Room *room, long amount);
DLLIMPORT void _DK_set_call_to_arms_as_birthing(struct Thing *objtng);
DLLIMPORT void _DK_set_call_to_arms_as_rebirthing(struct Thing *objtng);
DLLIMPORT void _DK_set_call_to_arms_as_dying(struct Thing *objtng);
/******************************************************************************/
struct Thing *create_object(const struct Coord3d *pos, unsigned short model, unsigned short owner, long parent_idx)
{
    struct Objects *objdat;
    struct InitLight ilight;
    struct Thing *thing;
    long i,k;
    //thing = _DK_create_object(pos, model, owner, a4);

    if (!i_can_allocate_free_thing_structure(FTAF_FreeEffectIfNoSlots))
    {
        ERRORDBG(3,"Cannot create object model %d for player %d. There are too many things allocated.",(int)model,(int)owner);
        erstat_inc(ESE_NoFreeThings);
        return INVALID_THING;
    }
    thing = allocate_free_thing_structure(FTAF_FreeEffectIfNoSlots);
    if (thing->index == 0) {
        ERRORDBG(3,"Should be able to allocate object %d for player %d, but failed.",(int)model,(int)owner);
        erstat_inc(ESE_NoFreeThings);
        return INVALID_THING;
    }
    thing->class_id = TCls_Object;
    thing->model = model;
    if (parent_idx == -1)
      thing->parent_idx = -1;
    else
      thing->parent_idx = parent_idx;
    LbMemoryCopy(&thing->mappos, pos, sizeof(struct Coord3d));
    struct ObjectConfig *objconf;
    objconf = get_object_model_stats2(model);
    objdat = get_objects_data_for_thing(thing);
    thing->sizexy = objdat->field_9;
    thing->field_58 = objdat->field_B;
    thing->field_5A = objdat->field_9;
    thing->field_5C = objdat->field_B;
    thing->health = saturate_set_signed(objconf->health,16);
    thing->field_20 = objconf->field_4;
    thing->field_23 = 204;
    thing->field_24 = 51;
    thing->field_22 = 0;
    thing->movement_flags |= TMvF_Unknown08;

    set_flag_byte(&thing->movement_flags, TMvF_Unknown40, objconf->field_8);
    thing->owner = owner;
    thing->creation_turn = game.play_gameturn;

    if (!objdat->field_2)
    {
      i = convert_td_iso(objdat->field_5);
      k = 0;
    } else
    {
      i = convert_td_iso(objdat->field_5);
      k = -1;
    }
    set_thing_draw(thing, i, objdat->field_7, objdat->field_D, 0, k, objdat->field_11);
    set_flag_byte(&thing->field_4F, 0x02, objconf->field_5);
    set_flag_byte(&thing->field_4F, 0x01, objdat->field_3 & 0x01);
    set_flag_byte(&thing->field_4F, 0x10, objdat->field_F & 0x01);
    set_flag_byte(&thing->field_4F, 0x20, objdat->field_F & 0x02);
    thing->active_state = objdat->field_0;
    if (objconf->ilght.field_0 != 0)
    {
        LbMemorySet(&ilight, 0, sizeof(struct InitLight));
        LbMemoryCopy(&ilight.mappos, &thing->mappos, sizeof(struct Coord3d));
        ilight.field_0 = objconf->ilght.field_0;
        ilight.field_2 = objconf->ilght.field_2;
        ilight.field_3 = objconf->ilght.field_3;
        ilight.is_dynamic = objconf->ilght.is_dynamic;
        thing->light_id = light_create_light(&ilight);
        if (thing->light_id == 0) {
            SYNCDBG(8,"Cannot allocate light to %s",thing_model_name(thing));
        }
    } else {
        thing->light_id = 0;
    }
    switch (thing->model)
    {
      case 3:
        thing->creature.gold_carried = game.chest_gold_hold;
        break;
      case 5:
        thing->byte_14 = 1;
        light_set_light_minimum_size_to_cache(thing->light_id, 0, 56);
        break;
      case 6:
        thing->creature.gold_carried = game.pot_of_gold_holds;
        break;
      case 33:
        set_flag_byte(&thing->field_4F, 0x10, false);
        set_flag_byte(&thing->field_4F, 0x20, true);
        break;
      case 43:
        thing->creature.gold_carried = game.gold_pile_value;
        break;
      case 49:
        i = get_free_hero_gate_number();
        if (i > 0)
        {
            thing->byte_13 = i;
        } else
        {
            thing->byte_13 = 0;
            ERRORLOG("Could not allocate number for hero gate");
        }
        break;
      default:
        break;
    }
    add_thing_to_its_class_list(thing);
    place_thing_in_mapwho(thing);
    return thing;
}

void destroy_food(struct Thing *thing)
{
    struct Room *room;
    struct Thing *efftng;
    struct Coord3d pos;
    PlayerNumber plyr_idx;
    long i;
    SYNCDBG(8,"Starting");
    plyr_idx = thing->owner;
    if (game.neutral_player_num != plyr_idx) {
        struct Dungeon *dungeon;
        dungeon = get_dungeon(plyr_idx);
        dungeon->lvstats.chickens_wasted++;
    }
    efftng = create_effect(&thing->mappos, TngEff_Unknown49, plyr_idx);
    if (!thing_is_invalid(efftng)) {
        i = UNSYNC_RANDOM(3);
        thing_play_sample(efftng, 112+i, 100, 0, 3, 0, 2, 256);
    }
    pos.x.val = thing->mappos.x.val;
    pos.y.val = thing->mappos.y.val;
    pos.z.val = thing->mappos.z.val + 256;
    create_effect(&thing->mappos, TngEff_Unknown51, plyr_idx);
    create_effect(&pos, TngEff_Unknown07, plyr_idx);
    if (!is_neutral_thing(thing))
    {
        if (thing->word_13 == -1)
        {
          room = get_room_thing_is_on(thing);
          if (!room_is_invalid(room))
          {
            if ((room->kind == RoK_GARDEN) && (room->owner == thing->owner))
            {
                if (room->used_capacity > 0)
                  room->used_capacity--;
                thing->word_13 = game.food_life_out_of_hatchery;
            }
          }
        }
    }
    delete_thing_structure(thing, 0);
}

void destroy_object(struct Thing *thing)
{
    if (object_is_mature_food(thing))
    {
        destroy_food(thing);
    } else
    {
        delete_thing_structure(thing, 0);
    }
}

TbBool thing_is_object(const struct Thing *thing)
{
    if (thing_is_invalid(thing))
      return false;
    if (thing->class_id != TCls_Object)
      return false;
    return true;
}

struct Objects *get_objects_data_for_thing(struct Thing *thing)
{
    unsigned int tmodel;
    tmodel = thing->model;
    if (tmodel >= OBJECT_TYPES_COUNT)
      return &objects_data[0];
    return &objects_data[tmodel];
}

struct Objects *get_objects_data(unsigned int tmodel)
{
    if (tmodel >= OBJECT_TYPES_COUNT)
        return &objects_data[0];
    return &objects_data[tmodel];
}

int box_thing_to_special(const struct Thing *thing)
{
    if (thing_is_invalid(thing))
        return 0;
    if ((thing->class_id != TCls_Object) || (thing->model >= OBJECT_TYPES_COUNT))
        return 0;
    return object_to_special[thing->model];
}

/**
 * Gives power kind associated with given spellbook thing.
 * @param thing The spellbook object thing.
 * @return Power kind, or 0 if the thing is not a spellbook object.
 */
PowerKind book_thing_to_magic(const struct Thing *thing)
{
    if (thing_is_invalid(thing))
        return 0;
    if ( (thing->class_id != TCls_Object) || (thing->model >= OBJECT_TYPES_COUNT) )
        return 0;
    return object_to_magic[thing->model];
}

TbBool thing_is_special_box(const struct Thing *thing)
{
    return (box_thing_to_special(thing) > 0);
}

TbBool thing_is_door_or_trap_box(const struct Thing *thing)
{
    return (crate_thing_to_workshop_item_model(thing) > 0);
}

TbBool thing_is_trap_box(const struct Thing *thing)
{
    return (crate_thing_to_workshop_item_class(thing) == TCls_Trap);
}

TbBool thing_is_door_box(const struct Thing *thing)
{
    return (crate_thing_to_workshop_item_class(thing) == TCls_Door);
}

TbBool thing_is_dungeon_heart(const struct Thing *thing)
{
    if (thing_is_invalid(thing))
        return false;
    if (thing->class_id != TCls_Object)
        return false;
    struct ObjectConfig *objconf;
    objconf = get_object_model_stats2(thing->model);
    return (objconf->is_heart) != 0;
}

TbBool thing_is_mature_food(const struct Thing *thing)
{
    if (thing_is_invalid(thing))
        return false;
    return (thing->class_id == TCls_Object) && (thing->model == 10);
}

TbBool thing_is_spellbook(const struct Thing *thing)
{
    //return _DK_thing_is_spellbook(thing);
    return (book_thing_to_magic(thing) > 0);
}

TbBool object_is_hero_gate(const struct Thing *thing)
{
  return (thing->model == 49);
}

TbBool object_is_infant_food(const struct Thing *thing)
{
  return (thing->model == 40) || (thing->model == 41) || (thing->model == 42);
}

TbBool object_is_growing_food(const struct Thing *thing)
{
  return (thing->model == 9);
}

TbBool object_is_mature_food(const struct Thing *thing)
{
  return (thing->model == 10);
}

TbBool object_is_gold(const struct Thing *thing)
{
    //return _DK_object_is_gold(thing);
    switch (thing->model)
    {
      case 3:
      case 6:
      case 43:
      case 52:
      case 53:
      case 54:
      case 55:
      case 56:
          return true;
      default:
          return false;
    }
}

/**
 * Returns if given thing is a gold hoard.
 * Gold hoards may only exist in treasure rooms.
 * @param thing
 * @return
 */
TbBool object_is_gold_hoard(const struct Thing *thing)
{
    switch (thing->model)
    {
      case 52:
      case 53:
      case 54:
      case 55:
      case 56:
          return true;
      default:
          return false;
    }
}

TbBool object_is_gold_pile(const struct Thing *thing)
{
    //return _DK_object_is_gold_pile(thing);
    switch (thing->model)
    {
      case 3: // Chest of gold
      case 6: // Pot of gold
      case 43: // Gold laying on the ground
      case 128:
          return true;
      default:
          return false;
    }
}

TbBool object_is_gold_laying_on_ground(const struct Thing *thing)
{
    return (thing->model == 43);
}

/**
 * Returns if given thing is a guardpost flag.
 * @param thing
 * @return
 */
TbBool object_is_guard_flag(const struct Thing *thing)
{
    switch (thing->model)
    {
      case 115:
      case 116:
      case 117:
      case 118:
      case 119:
          return true;
      default:
          return false;
    }
}

/**
 * Returns if given object thing is a workshop equipment.
 * @param thing
 * @return
 */
TbBool object_is_workshop_equipment(const struct Thing *thing)
{
  return ((thing->model == 114) || (thing->model == 26));
}

TbBool object_is_unaffected_by_terrain_changes(const struct Thing *thing)
{
    if (thing_is_invalid(thing))
        return false;
    if (thing->class_id != TCls_Object)
        return false;
    struct ObjectConfigStats *objst;
    objst = get_object_model_stats(thing->model);
    return (objst->genre == OCtg_Power);
}

long add_gold_to_hoarde(struct Thing *thing, struct Room *room, long amount)
{
  return _DK_add_gold_to_hoarde(thing, room, amount);
}

/**
 * Finds spellbook in a 3x3 subtiles area around given position.
 * Selects the spellbook which is nearest to center of given subtile.
 * @param stl_x Central search subtile X coord.
 * @param stl_y Central search subtile Y coord.
 * @return The nearest thing, or invalid thing if no match was found.
 */
struct Thing *get_spellbook_at_position(MapSubtlCoord stl_x, MapSubtlCoord stl_y)
{
    //return _DK_get_spellbook_at_position(x, y);
    return get_object_around_owned_by_and_matching_bool_filter(
        subtile_coord_center(stl_x), subtile_coord_center(stl_y), -1, thing_is_spellbook);
}

/**
 * Finds special box in a 3x3 subtiles area around given position.
 * Selects the box which is nearest to center of given subtile.
 * @param stl_x Central search subtile X coord.
 * @param stl_y Central search subtile Y coord.
 * @return The nearest thing, or invalid thing if no match was found.
 */
struct Thing *get_special_at_position(MapSubtlCoord stl_x, MapSubtlCoord stl_y)
{
    //return _DK_get_special_at_position(x, y);
    return get_object_around_owned_by_and_matching_bool_filter(
        subtile_coord_center(stl_x), subtile_coord_center(stl_y), -1, thing_is_special_box);
}

/**
 * Finds crate box in a 3x3 subtiles area around given position.
 * Selects the box which is nearest to center of given subtile.
 * @param stl_x Central search subtile X coord.
 * @param stl_y Central search subtile Y coord.
 * @return The nearest thing, or invalid thing if no match was found.
 */
struct Thing *get_crate_at_position(MapSubtlCoord stl_x, MapSubtlCoord stl_y)
{
    //return _DK_get_crate_at_position(x, y);
    return get_object_around_owned_by_and_matching_bool_filter(
        subtile_coord_center(stl_x), subtile_coord_center(stl_y), -1, thing_is_door_or_trap_box);
}

long food_moves(struct Thing *heartng)
{
  return _DK_food_moves(heartng);
}

long food_grows(struct Thing *heartng)
{
  return _DK_food_grows(heartng);
}

long object_being_dropped(struct Thing *heartng)
{
  return _DK_object_being_dropped(heartng);
}

void update_dungeon_heart_beat(struct Thing *heartng)
{
    long i;
    long long k;
    const long base_heart_beat_rate = 2304;
    static long bounce = 0;
    if (heartng->active_state != ObSt_BeingDestroyed)
    {
        i = (char)heartng->byte_14;
        heartng->field_3E = 0;
        struct ObjectConfig *objconf;
        objconf = get_object_model_stats2(5);
        k = 384 * (long)(objconf->health - heartng->health) / objconf->health;
        k = base_heart_beat_rate / (k + 128);
        light_set_light_intensity(heartng->light_id, light_get_light_intensity(heartng->light_id) + (i*36/k));
        heartng->field_40 += (i*base_heart_beat_rate/k);
        if (heartng->field_40 < 0)
        {
            heartng->field_40 = 0;
            light_set_light_intensity(heartng->light_id, 20);
            heartng->byte_14 = 1;
        }
        if (heartng->field_40 > base_heart_beat_rate-1)
        {
            heartng->field_40 = base_heart_beat_rate-1;
            light_set_light_intensity(heartng->light_id, 56);
            heartng->byte_14 = (unsigned char)-1;
            if ( bounce )
            {
                thing_play_sample(heartng, 151, 100, 0, 3, 1, 6, 256);
            } else
            {
                thing_play_sample(heartng, 150, 100, 0, 3, 1, 6, 256);
            }
            bounce = !bounce;
        }
        k = (((unsigned long long)heartng->field_40 >> 32) & 0xFF) + heartng->field_40;
        heartng->field_48 = (k >> 8) & 0xFF;
        if ( !S3DEmitterIsPlayingSample(heartng->snd_emitter_id, 93, 0) )
          thing_play_sample(heartng, 93, 100, -1, 3, 1, 6, 256);
    }
}

TngUpdateRet object_update_dungeon_heart(struct Thing *heartng)
{
    struct Dungeon *dungeon;
    long i;
    long long k;
    SYNCDBG(18,"Starting");
    //return _DK_object_update_dungeon_heart(thing);
    if ((heartng->health > 0) && (game.dungeon_heart_heal_time != 0))
    {
        struct ObjectConfig *objconf;
        objconf = get_object_model_stats2(5);
        if ((game.play_gameturn % game.dungeon_heart_heal_time) == 0)
        {
            heartng->health += game.dungeon_heart_heal_health;
            if (heartng->health < 0)
            {
              heartng->health = 0;
            } else
            if (heartng->health > objconf->health)
            {
              heartng->health = objconf->health;
            }
        }
        k = ((heartng->health << 8) / objconf->health) << 7;
        i = (saturate_set_signed(k,32) >> 8) + 128;
        heartng->field_46 = i * (long)objects_data[5].field_D >> 8;
        heartng->sizexy = i * (long)objects_data[5].field_9 >> 8;
    } else
    if (heartng->owner != game.neutral_player_num)
    {
        dungeon = get_players_num_dungeon(heartng->owner);
        if (dungeon->field_1060 == 0)
        {
            dungeon->field_1061 = 0;
            dungeon->field_1060 = 1;
            dungeon->essential_pos.x.val = heartng->mappos.x.val;
            dungeon->essential_pos.y.val = heartng->mappos.y.val;
            dungeon->essential_pos.z.val = heartng->mappos.z.val;
        }
    }
    process_dungeon_destroy(heartng);
    SYNCDBG(18,"Beat update");
    if ((heartng->alloc_flags & TAlF_Exists) == 0)
      return 0;
    if ( heartng->byte_13 )
      heartng->byte_13--;
    update_dungeon_heart_beat(heartng);
    return TUFRet_Modified;
}

void set_call_to_arms_as_birthing(struct Thing *objtng)
{
    //_DK_set_call_to_arms_as_birthing(objtng); return;
    int frame;
    switch (objtng->byte_13)
    {
    case 1:
        frame = objtng->field_48;
        break;
    case 2:
        frame = 0;
        break;
    case 3:
    case 4:
        frame = objtng->field_49 - (int)objtng->field_48;
        break;
    default:
        frame = 0;
        break;
    }
    struct CallToArmsGraphics *ctagfx;
    ctagfx = &call_to_arms_graphics[objtng->owner];
    struct Objects *objdat;
    objdat = get_objects_data_for_thing(objtng);
    set_thing_draw(objtng, ctagfx->field_0, 256, objdat->field_D, 0, frame, 2);
    objtng->byte_13 = 1;
    stop_thing_playing_sample(objtng, 83);
    thing_play_sample(objtng, 83, 100, 0, 3, 0, 6, 256);
}

void set_call_to_arms_as_dying(struct Thing *objtng)
{
    _DK_set_call_to_arms_as_dying(objtng); return;
}

void set_call_to_arms_as_rebirthing(struct Thing *objtng)
{
    _DK_set_call_to_arms_as_rebirthing(objtng); return;
}

TngUpdateRet object_update_call_to_arms(struct Thing *thing)
{
    //return _DK_object_update_call_to_arms(thing);
    struct PlayerInfo *player;
    player = get_player(thing->owner);
    if (thing->index != player->field_43C)
    {
        delete_thing_structure(thing, 0);
        return -1;
    }
    struct Dungeon *dungeon;
    dungeon = get_players_dungeon(player);
    struct CallToArmsGraphics *ctagfx;
    ctagfx = &call_to_arms_graphics[player->id_number];
    struct Objects *objdat;
    objdat = get_objects_data_for_thing(thing);
    struct Coord3d pos;

    switch (thing->byte_13)
    {
    case 1:
        if (thing->field_49 - 1 <= thing->field_48)
        {
            thing->byte_13 = 2;
            set_thing_draw(thing, ctagfx->field_4, 256, objdat->field_D, 0, 0, 2u);
            return 1;
        }
        break;
    case 2:
        break;
    case 3:
        if (thing->field_49 - 1 == thing->field_48)
        {
            player->field_43C = 0;
            delete_thing_structure(thing, 0);
            return -1;
        }
        break;
    case 4:
        if (thing->field_49 - 1 == thing->field_48)
        {
            pos.x.val = subtile_coord_center(dungeon->cta_stl_x);
            pos.y.val = subtile_coord_center(dungeon->cta_stl_y);
            pos.z.val = get_thing_height_at(thing, &pos);
            move_thing_in_map(thing, &pos);
            set_thing_draw(thing, ctagfx->field_0, 256, objdat->field_D, 0, 0, 2);
            thing->byte_13 = 1;
            stop_thing_playing_sample(thing, 83);
            thing_play_sample(thing, 83, 100, 0, 3, 0, 6, 256);
        }
        break;
    default:
        break;
    }
    return 1;
}

TngUpdateRet object_update_armour(struct Thing *heartng)
{
    return _DK_object_update_armour(heartng);
}

TngUpdateRet object_update_object_scale(struct Thing *heartng)
{
    return _DK_object_update_object_scale(heartng);
}

TngUpdateRet object_update_armour2(struct Thing *heartng)
{
    return _DK_object_update_armour2(heartng);
}

TngUpdateRet object_update_power_sight(struct Thing *heartng)
{
    return _DK_object_update_power_sight(heartng);
}

#define NUM_ANGLES 16
TngUpdateRet object_update_power_lightning(struct Thing *heartng)
{
    long i;
    //return _DK_object_update_power_lightning(thing);
    unsigned long exist_turns;
    long variation;
    heartng->health = 2;
    exist_turns = game.play_gameturn - heartng->creation_turn;
    variation = NUM_ANGLES * exist_turns;
    for (i=0; i < NUM_ANGLES; i++)
    {
        struct Coord3d pos;
        int angle;
        angle = (variation % NUM_ANGLES) * 2*LbFPMath_PI / NUM_ANGLES;
        if (set_coords_to_cylindric_shift(&pos, &heartng->mappos, 8*variation, angle, 0))
        {
            struct Map *mapblk;
            mapblk = get_map_block_at(pos.x.stl.num, pos.y.stl.num);
            if ((mapblk->flags & MapFlg_IsTall) == 0)
            {
                pos.z.val = get_floor_height_at(&pos) + 128;
                create_effect_element(&pos, lightning_spangles[heartng->owner], heartng->owner);
            }
        }
        variation++;
    }
    struct MagicStats *magstat;
    magstat = &game.keeper_power_stats[PwrK_LIGHTNING];
    if (exist_turns > abs(magstat->strength[heartng->byte_13]))
    {
        delete_thing_structure(heartng, 0);
        return TUFRet_Deleted;
    }
    return TUFRet_Modified;
}
#undef NUM_ANGLES

TngUpdateRet move_object(struct Thing *thing)
{
    struct Coord3d pos;
    TbBool move_allowed;
    SYNCDBG(18,"Starting");
    TRACE_THING(thing);
    //return _DK_move_object(thing);
    move_allowed = get_thing_next_position(&pos, thing);
    if ( !positions_equivalent(&thing->mappos, &pos) )
    {
        if ((!move_allowed) || thing_in_wall_at(thing, &pos))
        {
            long blocked_flags;
            blocked_flags = get_thing_blocked_flags_at(thing, &pos);
            slide_thing_against_wall_at(thing, &pos, blocked_flags);
            remove_relevant_forces_from_thing_after_slide(thing, &pos, blocked_flags);
            if (thing->model == 6)
              thing_play_sample(thing, 79, 100, 0, 3, 0, 1, 256);
        }
        move_thing_in_map(thing, &pos);
    }
    thing->field_60 = get_thing_height_at(thing, &thing->mappos);
    return TUFRet_Modified;
}

TngUpdateRet update_object(struct Thing *thing)
{
    Thing_Class_Func upcallback;
    Thing_State_Func stcallback;
    struct Objects *objdat;
    SYNCDBG(18,"Starting for %s",thing_model_name(thing));
    TRACE_THING(thing);
    //return _DK_update_object(thing);

    upcallback = NULL;
    if (thing->model < sizeof(object_update_functions)/sizeof(object_update_functions[0])) {
        upcallback = object_update_functions[thing->model];
    } else {
        ERRORLOG("Object model %d exceeds update_functions dimensions",(int)thing->model);
    }
    if (upcallback != NULL)
    {
        if (upcallback(thing) <= 0) {
            return TUFRet_Deleted;
        }
    }
    stcallback = NULL;
    if (thing->active_state < sizeof(object_state_functions)/sizeof(object_state_functions[0])) {
        stcallback = object_state_functions[thing->active_state];
    } else {
        ERRORLOG("The %s state %d exceeds state_functions dimensions",thing_model_name(thing),(int)thing->active_state);
    }
    if (stcallback != NULL)
    {
        SYNCDBG(18,"Updating state");
        if (stcallback(thing) <= 0) {
            return TUFRet_Deleted;
        }
    }
    SYNCDBG(18,"Updating position");
    thing->movement_flags &= ~TMvF_IsOnWater;
    thing->movement_flags &= ~TMvF_IsOnLava;
    if ( ((thing->movement_flags & TMvF_Unknown40) == 0) && thing_touching_floor(thing) )
    {
      if (subtile_has_lava_on_top(thing->mappos.x.stl.num, thing->mappos.y.stl.num))
      {
        thing->movement_flags |= TMvF_IsOnLava;
        objdat = get_objects_data_for_thing(thing);
        if ( (objdat->field_12) && !thing_is_dragged_or_pulled(thing) )
        {
            destroy_object(thing);
            return TUFRet_Deleted;
        }
      } else
      if (subtile_has_water_on_top(thing->mappos.x.stl.num, thing->mappos.y.stl.num))
      {
        thing->movement_flags |= TMvF_IsOnWater;
      }
    }
    if ((thing->movement_flags & TMvF_Unknown40) != 0)
        return TUFRet_Modified;
    return move_object(thing);
}

/**
 * Creates a guard post flag object.
 * @param pos Position where the guard post flag is to be created.
 * @param plyr_idx Player who will own the flag.
 * @param parent_idx Slab number associated with the flag.
 * @return Guard flag object thing.
 */
struct Thing *create_guard_flag_object(const struct Coord3d *pos, PlayerNumber plyr_idx, long parent_idx)
{
    struct Thing *thing;
    ThingModel grdflag_kind;
    if (plyr_idx >= sizeof(player_guardflag_objects)/sizeof(player_guardflag_objects[0]))
        grdflag_kind = player_guardflag_objects[NEUTRAL_PLAYER];
    else
        grdflag_kind = player_guardflag_objects[(int)plyr_idx];
    if (grdflag_kind <= 0)
        return INVALID_THING;
    // Guard posts have slab number set as parent
    thing = create_object(pos, grdflag_kind, plyr_idx, parent_idx);
    if (thing_is_invalid(thing))
        return INVALID_THING;
    return thing;
}

struct Thing *create_gold_pot_at(long pos_x, long pos_y, PlayerNumber plyr_idx)
{
    struct Thing *thing;
    struct Coord3d pos;
    pos.x.val = pos_x;
    pos.y.val = pos_y;
    pos.z.val = subtile_coord(3,0);
    thing = create_object(&pos, 6, plyr_idx, -1);
    if (thing_is_invalid(thing))
        return INVALID_THING;
    thing->valuable.gold_stored = game.pot_of_gold_holds;
    return thing;
}

/**
 * Creates a gold hoard object.
 * Note that this function does not create a fully operable object - gold hoard requires room
 * association to be fully functional. This is just a utility sub-function.
 * @param pos Position where the hoard is to be created.
 * @param plyr_idx Player who will own the hoard.
 * @param value The amount of gold to be stored inside the hoard.
 * @return Hoard object thing, which still require to be associated to room.
 */
struct Thing *create_gold_hoard_object(const struct Coord3d *pos, PlayerNumber plyr_idx, long value)
{
    struct Thing *thing;
    long hoard_size_holds,hoard_size,hoard_store;
    hoard_size_holds = 9 * game.pot_of_gold_holds / 5;
    hoard_size = value / hoard_size_holds;
    if (hoard_size >= sizeof(gold_hoard_objects)/sizeof(gold_hoard_objects[0]))
        hoard_size = sizeof(gold_hoard_objects)/sizeof(gold_hoard_objects[0])-1;
    thing = create_object(pos, gold_hoard_objects[hoard_size], plyr_idx, -1);
    if (thing_is_invalid(thing))
        return INVALID_THING;
    hoard_store = 9 * game.pot_of_gold_holds;
    if (hoard_store >= value)
        hoard_store = value;
    thing->valuable.gold_stored = hoard_store;
    return thing;
}

long remove_gold_from_hoarde(struct Thing *thing, struct Room *room, long amount)
{
    return _DK_remove_gold_from_hoarde(thing, room, amount);
}

/**
 * Returns if given thing is a hoard of gold.
 * @note originally was thing_is_gold_hoarde().
 * @param thing
 * @return
 */
TbBool thing_is_gold_hoard(const struct Thing *thing)
{
    if (thing_is_invalid(thing))
        return false;
    if (thing->class_id != TCls_Object)
      return false;
    return object_is_gold_hoard(thing);
}

struct Thing *find_gold_hoard_at(MapSubtlCoord stl_x, MapSubtlCoord stl_y)
{
    struct Thing *thing;
    struct Map *mapblk;
    long i;
    unsigned long k;
    k = 0;
    mapblk = get_map_block_at(stl_x,stl_y);
    i = get_mapwho_thing_index(mapblk);
    while (i != 0)
    {
      thing = thing_get(i);
      if (thing_is_invalid(thing))
      {
        WARNLOG("Jump out of things array");
        break;
      }
      i = thing->next_on_mapblk;
      // Per-thing block
      if (thing_is_gold_hoard(thing))
          return thing;
      // Per-thing block ends
      k++;
      if (k > THINGS_COUNT)
      {
        ERRORLOG("Infinite loop detected when sweeping things list");
        break;
      }
    }
    return INVALID_THING;
}

TbBool add_gold_to_pile(struct Thing *thing, long value)
{
    long scaled_val;
    if (thing_is_invalid(thing))
        return false;
    thing->creature.gold_carried += value;
    if (thing->creature.gold_carried < 0)
        thing->creature.gold_carried = LONG_MAX;
    scaled_val = thing->creature.gold_carried / 2 + 150;
    if ((scaled_val > 600) || (thing->creature.gold_carried >= game.gold_pile_maximum))
      scaled_val = 600;
    thing->field_46 = scaled_val;
    return true;
}

struct Thing *create_gold_pile(struct Coord3d *pos, PlayerNumber plyr_idx, long value)
{
    struct Thing *thing;
    thing = create_object(pos, 43, plyr_idx, -1);
    if (thing_is_invalid(thing)) {
        return INVALID_THING;
    }
    thing->creature.gold_carried = 0;
    add_gold_to_pile(thing, value);
    return thing;
}

struct Thing *drop_gold_pile(long value, struct Coord3d *pos)
{
    struct Thing *thing;
    thing = smallest_gold_pile_at_xy(pos->x.stl.num, pos->y.stl.num);
    if (thing_is_invalid(thing)) {
        thing = create_gold_pile(pos, game.neutral_player_num, value);
    } else {
        add_gold_to_pile(thing, value);
    }
    return thing;
}

void create_rubble_for_dug_block(MapSubtlCoord stl_x, MapSubtlCoord stl_y, MapSubtlCoord stl_z_max, PlayerNumber plyr_idx)
{
    int zmax;
    struct Coord3d pos;
    pos.x.val = subtile_coord_center(stl_x);
    pos.y.val = subtile_coord_center(stl_y);
    pos.z.val = subtile_coord_center(1);
    zmax = subtile_coord(stl_z_max,0);
    while (pos.z.val < zmax)
    {
        create_effect(&pos, 26, plyr_idx);
        pos.z.val += 256;
    }
}
/******************************************************************************/
#ifdef __cplusplus
}
#endif
