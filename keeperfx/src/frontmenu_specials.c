/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file frontmenu_specials.c
 *     GUI menus for in-game dungeon special boxes.
 * @par Purpose:
 *     Functions to show and maintain menus appearing when specials are used.
 * @par Comment:
 *     None.
 * @author   KeeperFX Team
 * @date     05 Jan 2009 - 09 Oct 2010
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include "frontmenu_specials.h"
#include "globals.h"
#include "bflib_basics.h"

#include "bflib_guibtns.h"
#include "gui_draw.h"
#include "gui_frontbtns.h"
#include "frontend.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
DLLIMPORT void _DK_select_resurrect_creature(struct GuiButton *gbtn);
DLLIMPORT void _DK_maintain_resurrect_creature_select(struct GuiButton *gbtn);
DLLIMPORT void _DK_draw_resurrect_creature(struct GuiButton *gbtn);
DLLIMPORT void _DK_select_resurrect_creature_up(struct GuiButton *gbtn);
DLLIMPORT void _DK_select_resurrect_creature_down(struct GuiButton *gbtn);
DLLIMPORT void _DK_maintain_resurrect_creature_scroll(struct GuiButton *gbtn);
DLLIMPORT void _DK_select_transfer_creature(struct GuiButton *gbtn);
DLLIMPORT void _DK_draw_transfer_creature(struct GuiButton *gbtn);
DLLIMPORT void _DK_maintain_transfer_creature_select(struct GuiButton *gbtn);
DLLIMPORT void _DK_select_transfer_creature_up(struct GuiButton *gbtn);
DLLIMPORT void _DK_select_transfer_creature_down(struct GuiButton *gbtn);
DLLIMPORT void _DK_maintain_transfer_creature_scroll(struct GuiButton *gbtn);
DLLIMPORT void _DK_choose_hold_audience(struct GuiButton *gbtn);
DLLIMPORT void _DK_choose_armageddon(struct GuiButton *gbtn);
/******************************************************************************/
struct GuiButtonInit resurrect_creature_buttons[] = {
  { 0,  0, 0, 0, NULL,               NULL,        NULL,               0, 999,  10, 999,  10,200, 32, gui_area_text,                     1, 428,  0,       {0},            0, 0, NULL },
  { 0,  0, 0, 0, select_resurrect_creature,NULL,  NULL,               0, 999,  62, 999,  62,250, 26, draw_resurrect_creature,           0, 201,  0,       {0},            0, 0, maintain_resurrect_creature_select },
  { 0,  0, 0, 0, select_resurrect_creature,NULL,  NULL,               1, 999,  90, 999,  90,250, 26, draw_resurrect_creature,           0, 201,  0,       {0},            0, 0, maintain_resurrect_creature_select },
  { 0,  0, 0, 0, select_resurrect_creature,NULL,  NULL,               2, 999, 118, 999, 118,250, 26, draw_resurrect_creature,           0, 201,  0,       {0},            0, 0, maintain_resurrect_creature_select },
  { 0,  0, 0, 0, select_resurrect_creature,NULL,  NULL,               3, 999, 146, 999, 146,250, 26, draw_resurrect_creature,           0, 201,  0,       {0},            0, 0, maintain_resurrect_creature_select },
  { 0,  0, 0, 0, select_resurrect_creature,NULL,  NULL,               4, 999, 174, 999, 174,250, 26, draw_resurrect_creature,           0, 201,  0,       {0},            0, 0, maintain_resurrect_creature_select },
  { 0,  0, 0, 0, select_resurrect_creature,NULL,  NULL,               5, 999, 202, 999, 202,250, 26, draw_resurrect_creature,           0, 201,  0,       {0},            0, 0, maintain_resurrect_creature_select },
  { 1,  0, 0, 0, select_resurrect_creature_up,NULL,NULL,              1, 305,  62, 305,  62, 22, 24, gui_area_new_normal_button,      278, 201,  0,       {0},            0, 0, maintain_resurrect_creature_scroll },
  { 1,  0, 0, 0, select_resurrect_creature_down,NULL,NULL,            2, 305, 204, 305, 204, 22, 24, gui_area_new_normal_button,      280, 201,  0,       {0},            0, 0, maintain_resurrect_creature_scroll },
  { 0,  0, 0, 1, NULL,               NULL,        NULL,               0, 999, 258, 999, 258,100, 32, gui_area_text,                     1, 403,  0,       {0},            0, 0, NULL },
  {-1,  0, 0, 0, NULL,               NULL,        NULL,               0,   0,   0,   0,   0,  0,  0, NULL,                              0,   0,  0,       {0},            0, 0, NULL },
};

struct GuiButtonInit transfer_creature_buttons[] = {
  { 0,  0, 0, 0, NULL,               NULL,        NULL,               0, 999,  10, 999,  10,200, 32, gui_area_text,                     1, 429,  0,       {0},            0, 0, NULL },
  { 0,  0, 0, 0, select_transfer_creature,NULL,   NULL,               0, 999,  62, 999,  62,250, 26, draw_transfer_creature,            0, 201,  0,       {0},            0, 0, maintain_transfer_creature_select },
  { 0,  0, 0, 0, select_transfer_creature,NULL,   NULL,               1, 999,  90, 999,  90,250, 26, draw_transfer_creature,            1, 201,  0,       {0},            0, 0, maintain_transfer_creature_select },
  { 0,  0, 0, 0, select_transfer_creature,NULL,   NULL,               2, 999, 118, 999, 118,250, 26, draw_transfer_creature,            2, 201,  0,       {0},            0, 0, maintain_transfer_creature_select },
  { 0,  0, 0, 0, select_transfer_creature,NULL,   NULL,               3, 999, 146, 999, 146,250, 26, draw_transfer_creature,            3, 201,  0,       {0},            0, 0, maintain_transfer_creature_select },
  { 0,  0, 0, 0, select_transfer_creature,NULL,   NULL,               4, 999, 174, 999, 174,250, 26, draw_transfer_creature,            4, 201,  0,       {0},            0, 0, maintain_transfer_creature_select },
  { 0,  0, 0, 0, select_transfer_creature,NULL,   NULL,               5, 999, 202, 999, 202,250, 26, draw_transfer_creature,            5, 201,  0,       {0},            0, 0, maintain_transfer_creature_select },
  { 1,  0, 0, 0, select_transfer_creature_up,NULL,NULL,               1, 305,  62, 305,  62, 22, 24, gui_area_new_normal_button,      278, 201,  0,       {0},            0, 0, maintain_transfer_creature_scroll },
  { 1,  0, 0, 0, select_transfer_creature_down,NULL,NULL,             2, 305, 204, 305, 204, 22, 24, gui_area_new_normal_button,      280, 201,  0,       {0},            0, 0, maintain_transfer_creature_scroll },
  { 0,  0, 0, 1, NULL,               NULL,        NULL,               0, 999, 258, 999, 258,100, 32, gui_area_text,                     1, 403,  0,       {0},            0, 0, NULL },
  {-1,  0, 0, 0, NULL,               NULL,        NULL,               0,   0,   0,   0,   0,  0,  0, NULL,                              0,   0,  0,       {0},            0, 0, NULL },
};

struct GuiButtonInit hold_audience_buttons[] = {
  { 0,  0, 0, 0, NULL,               NULL,        NULL,               0, 999,  10, 999,  10,155, 32, gui_area_text,                     1, 634,  0,       {0},            0, 0, NULL },
  { 0,  0, 0, 1, NULL,               NULL,        NULL,               0,  38,  24,  40,  58, 46, 32, gui_area_normal_button,           46, 201,  0,       {0},            0, 0, NULL },
  { 0,  0, 0, 1, choose_hold_audience,NULL,       NULL,               0, 116,  24, 118,  58, 46, 32, gui_area_normal_button,           48, 201,  0,       {0},            0, 0, NULL },
  {-1,  0, 0, 0, NULL,               NULL,        NULL,               0,   0,   0,   0,   0,  0,  0, NULL,                              0,   0,  0,       {0},            0, 0, NULL },
};

struct GuiButtonInit armageddon_buttons[] = {
  { 0,  0, 0, 0, NULL,               NULL,        NULL,               0, 999,  10, 999,  10,155, 32, gui_area_text,                     1, 646,  0,       {0},            0, 0, NULL },
  { 0,  0, 0, 1, NULL,               NULL,        NULL,               0,  38,  24,  40,  58, 46, 32, gui_area_normal_button,           46, 201,  0,       {0},            0, 0, NULL },
  { 0,  0, 0, 1, choose_armageddon,  NULL,        NULL,               0, 116,  24, 118,  58, 46, 32, gui_area_normal_button,           48, 201,  0,       {0},            0, 0, NULL },
  {-1,  0, 0, 0, NULL,               NULL,        NULL,               0,   0,   0,   0,   0,  0,  0, NULL,                              0,   0,  0,       {0},            0, 0, NULL },
};

struct GuiButtonInit dungeon_special_buttons[] = {
  {-1,  0, 0, 0, NULL,               NULL,        NULL,               0,   0,   0,   0,   0,  0,  0, NULL,                              0,   0,  0,       {0},            0, 0, NULL },
};

struct GuiMenu hold_audience_menu =
 { 17, 0, 4, hold_audience_buttons,      POS_GAMECTR,POS_GAMECTR,200, 116, gui_pretty_background,       0, NULL,    NULL,                    0, 1, 0,};
struct GuiMenu dungeon_special_menu =
 { 27, 0, 4, dungeon_special_buttons,            160, POS_SCRBTM,480, 86, gui_round_glass_background,  0, NULL,    NULL,                    0, 0, 0,};
struct GuiMenu resurrect_creature_menu =
 { 28, 0, 4, resurrect_creature_buttons, POS_GAMECTR,POS_GAMECTR,350, 300, gui_pretty_background,       0, NULL,    NULL,                    0, 1, 0,};
struct GuiMenu transfer_creature_menu =
 { 29, 0, 4, transfer_creature_buttons,  POS_GAMECTR,POS_GAMECTR,350, 300, gui_pretty_background,       0, NULL,    NULL,                    0, 1, 0,};
struct GuiMenu armageddon_menu =
 { 30, 0, 4, armageddon_buttons,         POS_GAMECTR,POS_GAMECTR,200, 116, gui_pretty_background,       0, NULL,    NULL,                    0, 1, 0,};
/******************************************************************************/
#ifdef __cplusplus
}
#endif
/******************************************************************************/
void select_resurrect_creature(struct GuiButton *gbtn)
{
  _DK_select_resurrect_creature(gbtn);
}

void draw_resurrect_creature(struct GuiButton *gbtn)
{
  _DK_draw_resurrect_creature(gbtn);
}

void select_resurrect_creature_up(struct GuiButton *gbtn)
{
  _DK_select_resurrect_creature_up(gbtn);
}

void select_resurrect_creature_down(struct GuiButton *gbtn)
{
  _DK_select_resurrect_creature_down(gbtn);
}

void select_transfer_creature(struct GuiButton *gbtn)
{
  _DK_select_transfer_creature(gbtn);
}

void draw_transfer_creature(struct GuiButton *gbtn)
{
  _DK_draw_transfer_creature(gbtn);
}

void select_transfer_creature_up(struct GuiButton *gbtn)
{
  _DK_select_transfer_creature_up(gbtn);
}

void select_transfer_creature_down(struct GuiButton *gbtn)
{
  _DK_select_transfer_creature_down(gbtn);
}

void maintain_resurrect_creature_select(struct GuiButton *gbtn)
{
  _DK_maintain_resurrect_creature_select(gbtn);
}

void maintain_resurrect_creature_scroll(struct GuiButton *gbtn)
{
  _DK_maintain_resurrect_creature_scroll(gbtn);
}

void maintain_transfer_creature_select(struct GuiButton *gbtn)
{
  _DK_maintain_transfer_creature_select(gbtn);
}

void maintain_transfer_creature_scroll(struct GuiButton *gbtn)
{
  _DK_maintain_transfer_creature_scroll(gbtn);
}

void choose_hold_audience(struct GuiButton *gbtn)
{
  _DK_choose_hold_audience(gbtn);
}

void choose_armageddon(struct GuiButton *gbtn)
{
  _DK_choose_armageddon(gbtn);
}
/******************************************************************************/
