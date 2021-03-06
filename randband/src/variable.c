/* File: variable.c */

/* Purpose: Angband variables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Hack -- Link a copyright message into the executable
 */
cptr copyright[5] =
{
	"Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
	"",
	"This software may be copied and distributed for educational, research,",
	"and not for profit purposes provided that this copyright and statement",
	"are included in all such copies."
};


/*
 * Executable version
 */
byte version_major = VERSION_MAJOR;
byte version_minor = VERSION_MINOR;
byte version_patch = VERSION_PATCH;
byte version_extra = VERSION_EXTRA;

/*
 * Savefile version
 */
byte sf_major;			/* Savefile's "version_major" */
byte sf_minor;			/* Savefile's "version_minor" */
byte sf_patch;			/* Savefile's "version_patch" */
byte sf_extra;			/* Savefile's "version_extra" */
u32b sf_version;		/* Savefile's "version" */

byte z_major;           /* Savefile version for Zangband */
byte z_minor;
byte z_patch;

/*
 * Savefile information
 */
u32b sf_xtra;			/* Operating system info */
u32b sf_when;			/* Time when savefile created */
u16b sf_lives;			/* Number of past "lives" with this file */
u16b sf_saves;			/* Number of "saves" during this life */

/*
 * Run-time arguments
 */
bool arg_fiddle;			/* Command arg -- Request fiddle mode */
bool arg_wizard;			/* Command arg -- Request wizard mode */
bool arg_sound;				/* Command arg -- Request special sounds */
byte arg_graphics;			/* Command arg -- Request graphics mode */
bool arg_monochrome;		/* Command arg -- Request monochrome mode */
bool arg_force_original;	/* Command arg -- Request original keyset */
bool arg_force_roguelike;	/* Command arg -- Request roguelike keyset */

/*
 * Various things
 */
bool character_generated;	/* The character exists */
bool character_dungeon;		/* The character has a dungeon */
bool character_loaded;		/* The character was loaded from a savefile */
bool character_saved;		/* The character was just saved to a savefile */

bool character_icky;		/* The game is in an icky full screen mode */
bool character_xtra;		/* The game is in an icky startup mode */

u32b seed_flavor;		/* Hack -- consistent object colors */

bool msg_flag;			/* Used in msg_print() for "buffering" */

s16b min_hgt;			/* Current y bounds of area() */
s16b max_hgt;
s16b min_wid;			/* Current x bounds of area() */
s16b max_wid;

obj_theme dun_theme;	/* Current dungeon object theme */
s16b num_repro;			/* Current reproducer count */

s16b object_level;		/* Current object creation level */
s16b monster_level;		/* Current monster creation level */
s16b base_level;        /* Base dungeon level */

s32b turn;				/* Current game turn */
s32b old_turn;			/* Turn when level began (feelings) */

bool use_sound;			/* The "sound" mode is enabled */
bool use_graphics;		/* The "graphics" mode is enabled */

bool use_transparency = FALSE; /* Use transparent tiles */

s16b signal_count;		/* Hack -- Count interupts */

bool inkey_base;		/* See the "inkey()" function */
bool inkey_xtra;		/* See the "inkey()" function */
bool inkey_scan;		/* See the "inkey()" function */
bool inkey_flag;		/* See the "inkey()" function */

bool shimmer_monsters;	/* Hack -- optimize multi-hued monsters */

bool repair_monsters;	/* Hack -- optimize detect monsters */

/* Is this needed? */
bool hack_mutation;

s16b o_max = 1;			/* Number of allocated objects */
s16b o_cnt = 0;			/* Number of live objects */

s16b m_max = 1;			/* Number of allocated monsters */
s16b m_cnt = 0;			/* Number of live monsters */

s16b fld_max = 1;			/* Number of allocated fields */
s16b fld_cnt = 0;			/* Number of live fields */

s16b hack_m_idx = 0;	/* Hack -- see "process_monsters()" */
s16b hack_m_idx_ii = 0;
s16b *hack_fld_ptr = NULL; /* Hack -- see "fields.c" */

/* Can we get rid of this at all? */
char summon_kin_type;   /* Hack, by Julian Lighton: summon 'relatives' */

/* This probably can be moved to player_type */
int total_friends = 0;
s32b total_friend_levels = 0;
s32b friend_align = 0;

/* Is this needed? */
int leaving_quest = 0;

s16b store_cache_num = 0;	/* Number of stores with stock */
store_type **store_cache;	/* The cache of store stocks */



/* Special options */

byte hitpoint_warn;		/* Hitpoint warning (0 to 9) */

byte delay_factor;		/* Delay factor (0 to 9) */

byte autosave_l;        /* Autosave before entering new levels */
byte autosave_t;        /* Timed autosave */
s16b autosave_freq;     /* Autosave frequency */

/* Cheating options */
bool cheat_peek;
bool cheat_hear;
bool cheat_room;
bool cheat_xtra;
bool cheat_know;
bool cheat_live;


/*
 * Dungeon variables
 */

byte feeling;			/* Most recent feeling */
s16b rating;			/* Level's current rating */

bool good_item_flag;	/* True if "Artifact" on this level */

bool closing_flag;		/* Dungeon is closing */

bool fake_monochrome;	/* Use fake monochrome for effects */


/*
 * Dungeon size info
 */

s16b max_panel_rows, max_panel_cols;
s16b panel_row_min, panel_row_max;
s16b panel_col_min, panel_col_max;
s16b panel_col_prt, panel_row_prt;

byte *mp_a = NULL;
char *mp_c = NULL;

#ifdef USE_TRANSPARENCY
byte *mp_ta = NULL;
char *mp_tc = NULL;
#endif /* USE_TRANSPARENCY */


/*
 * User info
 */
int player_uid;
int player_euid;
int player_egid;

/*
 * Current player's character name
 */
char player_name[32];

/*
 * Stripped version of "player_name"
 */
char player_base[32];


/*
 * Buffer to hold the current savefile name
 */
char savefile[1024];



/*
 * Array of grids viewable to the player (see "cave.c")
 */
s16b view_n;
s16b view_y[VIEW_MAX];
s16b view_x[VIEW_MAX];

/*
 * Array of grids for use by various functions (see "cave.c")
 */
s16b temp_n;
s16b temp_y[TEMP_MAX];
s16b temp_x[TEMP_MAX];


/*
 * Array of grids for use in monster lighting effects (see "cave.c")
 */
s16b lite_n = 0;
s16b lite_y[LITE_MAX];
s16b lite_x[LITE_MAX];



/*
 * Number of active macros.
 */
s16b macro__num;

/*
 * Array of macro patterns [MACRO_MAX]
 */
cptr *macro__pat;

/*
 * Array of macro actions [MACRO_MAX]
 */
cptr *macro__act;

/*
 * Array of macro types [MACRO_MAX]
 */
bool *macro__cmd;

/*
 * Current macro action [1024]
 */
char *macro__buf;


/*
 * The number of quarks
 */
s16b quark__num;

/*
 * The pointers to the quarks [QUARK_MAX]
 */
cptr *quark__str;

/*
 * Current Quark time - for least recently used algorithm
 */
u16b quark__tim;

/*
 * Time of last access for each quark
 */
u16b *quark__use;


/*
 * The array of window options
 */
u32b window_flag[8];
u32b window_mask[8];

/* Normal option masks */
u32b option_mask[8];


/*
 * The array of window pointers
 */
term *angband_term[8];


/*
 * Standard window names
 */
char angband_term_name[8][16] =
{
	VERSION_NAME,
	"Term-1",
	"Term-2",
	"Term-3",
	"Term-4",
	"Term-5",
	"Term-6",
	"Term-7"
};


/*
 * Global table of color definitions
 */
/*
byte angband_color_table[256][4] =
{
	{0x00, 0x00, 0x00, 0x00},	*//* TERM_DARK */
/*	{0x00, 0xFF, 0xFF, 0xFF},	*//* TERM_WHITE */
/*	{0x00, 0x80, 0x80, 0x80},	*//* TERM_SLATE */
/*	{0x00, 0xFF, 0x80, 0x00},	*//* TERM_ORANGE */
/*	{0x00, 0xC0, 0x00, 0x00},	*//* TERM_RED */
/*	{0x00, 0x00, 0x80, 0x40},	*//* TERM_GREEN */
/*	{0x00, 0x00, 0x00, 0xFF},	*//* TERM_BLUE */
/*	{0x00, 0x80, 0x40, 0x00},	*//* TERM_UMBER */
/*	{0x00, 0x40, 0x40, 0x40},	*//* TERM_L_DARK */
/*	{0x00, 0xC0, 0xC0, 0xC0},	*//* TERM_L_WHITE */
/*	{0x00, 0xFF, 0x00, 0xFF},	*//* TERM_VIOLET */
/*	{0x00, 0xFF, 0xFF, 0x00},	*//* TERM_YELLOW */
/*	{0x00, 0xFF, 0x00, 0x00},	*//* TERM_L_RED */
/*	{0x00, 0x00, 0xFF, 0x00},	*//* TERM_L_GREEN */
/*	{0x00, 0x00, 0xFF, 0xFF},	*//* TERM_L_BLUE */
/*	{0x00, 0xC0, 0x80, 0x40}	*//* TERM_L_UMBER */
/*};*/

byte angband_color_table[256][4] =
{
{ 0, 0, 0, 0 },
{ 0, 255, 255, 255 },
{ 0, 144, 144, 144 },
{ 0, 255, 128, 0 },
{ 0, 192, 0, 0 },
{ 0, 0, 128, 64 },
{ 0, 0, 0, 255 },
{ 0, 160, 80, 16 },
{ 0, 112, 112, 112 },
{ 0, 128, 64, 192 },
{ 0, 255, 0, 255 },
{ 0, 255, 255, 0 },
{ 0, 255, 0, 0 },
{ 0, 0, 255, 0 },
{ 0, 0, 255, 255 },
{ 0, 192, 128, 64 },
{ 0, 255, 255, 255 },
{ 0, 255, 255, 255 },
{ 0, 255, 255, 255 },
{ 0, 255, 255, 255 },
{ 0, 237, 237, 237 },
{ 0, 219, 219, 219 },
{ 0, 201, 201, 201 },
{ 0, 171, 171, 171 },
{ 0, 162, 162, 162 },
{ 0, 153, 153, 153 },
{ 0, 144, 144, 144 },
{ 0, 134, 134, 134 },
{ 0, 124, 124, 124 },
{ 0, 114, 114, 114 },
{ 0, 255, 158, 63 },
{ 0, 255, 148, 42 },
{ 0, 255, 138, 21 },
{ 0, 255, 128, 0 },
{ 0, 237, 119, 0 },
{ 0, 219, 110, 0 },
{ 0, 201, 101, 0 },
{ 0, 207, 63, 63 },
{ 0, 202, 42, 42 },
{ 0, 197, 21, 21 },
{ 0, 192, 0, 0 },
{ 0, 179, 0, 0 },
{ 0, 166, 0, 0 },
{ 0, 153, 0, 0 },
{ 0, 63, 158, 109 },
{ 0, 42, 148, 94 },
{ 0, 21, 138, 79 },
{ 0, 0, 128, 64 },
{ 0, 0, 119, 60 },
{ 0, 0, 110, 56 },
{ 0, 0, 101, 52 },
{ 0, 63, 63, 255 },
{ 0, 42, 42, 255 },
{ 0, 21, 21, 255 },
{ 0, 0, 0, 255 },
{ 0, 0, 0, 237 },
{ 0, 0, 0, 219 },
{ 0, 0, 0, 201 },
{ 0, 181, 122, 73 },
{ 0, 174, 108, 54 },
{ 0, 167, 94, 35 },
{ 0, 160, 80, 16 },
{ 0, 149, 75, 15 },
{ 0, 138, 70, 14 },
{ 0, 127, 65, 13 },
{ 0, 145, 145, 145 },
{ 0, 134, 134, 134 },
{ 0, 123, 123, 123 },
{ 0, 112, 112, 112 },
{ 0, 104, 104, 104 },
{ 0, 96, 96, 96 },
{ 0, 88, 88, 88 },
{ 0, 158, 109, 207 },
{ 0, 148, 94, 202 },
{ 0, 138, 79, 197 },
{ 0, 128, 64, 192 },
{ 0, 119, 60, 179 },
{ 0, 110, 56, 166 },
{ 0, 101, 52, 153 },
{ 0, 255, 63, 255 },
{ 0, 255, 42, 255 },
{ 0, 255, 21, 255 },
{ 0, 255, 0, 255 },
{ 0, 237, 0, 237 },
{ 0, 219, 0, 219 },
{ 0, 201, 0, 201 },
{ 0, 255, 255, 63 },
{ 0, 255, 255, 42 },
{ 0, 255, 255, 21 },
{ 0, 255, 255, 0 },
{ 0, 237, 237, 0 },
{ 0, 219, 219, 0 },
{ 0, 201, 201, 0 },
{ 0, 255, 63, 63 },
{ 0, 255, 42, 42 },
{ 0, 255, 21, 21 },
{ 0, 255, 0, 0 },
{ 0, 237, 0, 0 },
{ 0, 219, 0, 0 },
{ 0, 201, 0, 0 },
{ 0, 63, 255, 63 },
{ 0, 42, 255, 42 },
{ 0, 21, 255, 21 },
{ 0, 0, 255, 0 },
{ 0, 0, 237, 0 },
{ 0, 0, 219, 0 },
{ 0, 0, 201, 0 },
{ 0, 63, 255, 255 },
{ 0, 42, 255, 255 },
{ 0, 21, 255, 255 },
{ 0, 0, 255, 255 },
{ 0, 0, 237, 237 },
{ 0, 0, 219, 219 },
{ 0, 0, 201, 201 },
{ 0, 207, 158, 109 },
{ 0, 202, 148, 94 },
{ 0, 197, 138, 79 },
{ 0, 192, 128, 64 },
{ 0, 179, 119, 60 },
{ 0, 166, 110, 56 },
{ 0, 153, 101, 52 }
};

/*
byte angband_color_table[256][4] =
{
   { 0, 0, 0, 0 },
   { 0, 255, 255, 255 },
   { 0, 144, 144, 144 },
   { 0, 255, 128, 0 },
   { 0, 192, 0, 0 },
   { 0, 0, 128, 64 },
   { 0, 0, 0, 255 },
   { 0, 160, 80, 16 },
   { 0, 112, 112, 112 },
   { 0, 128, 64, 192 },
   { 0, 255, 0, 255 },
   { 0, 255, 255, 0 },
   { 0, 255, 0, 0 },
   { 0, 0, 255, 0 },
   { 0, 0, 255, 255 },
   { 0, 192, 128, 64 },
   { 0, 255, 255, 255 },
   { 0, 255, 255, 255 },
   { 0, 255, 255, 255 },
   { 0, 255, 255, 255 },
   { 0, 255, 255, 255 },
   { 0, 219, 219, 219 },
   { 0, 183, 183, 183 },
   { 0, 147, 147, 147 },
   { 0, 216, 216, 216 },
   { 0, 198, 198, 198 },
   { 0, 180, 180, 180 },
   { 0, 162, 162, 162 },
   { 0, 144, 144, 144 },
   { 0, 124, 124, 124 },
   { 0, 104, 104, 104 },
   { 0, 84, 84, 84 },
   { 0, 255, 208, 168 },
   { 0, 255, 188, 126 },
   { 0, 255, 168, 84 },
   { 0, 255, 148, 42 },
   { 0, 255, 128, 0 },
   { 0, 219, 110, 0 },
   { 0, 183, 92, 0 },
   { 0, 147, 74, 0 },
   { 0, 232, 168, 168 },
   { 0, 222, 126, 126 },
   { 0, 212, 84, 84 },
   { 0, 202, 42, 42 },
   { 0, 192, 0, 0 },
   { 0, 166, 0, 0 },
   { 0, 140, 0, 0 },
   { 0, 114, 0, 0 },
   { 0, 168, 208, 184 },
   { 0, 126, 188, 154 },
   { 0, 84, 168, 124 },
   { 0, 42, 148, 94 },
   { 0, 0, 128, 64 },
   { 0, 0, 110, 56 },
   { 0, 0, 92, 48 },
   { 0, 0, 74, 40 },
   { 0, 168, 168, 255 },
   { 0, 126, 126, 255 },
   { 0, 84, 84, 255 },
   { 0, 42, 42, 255 },
   { 0, 0, 0, 255 },
   { 0, 0, 0, 219 },
   { 0, 0, 0, 183 },
   { 0, 0, 0, 147 },
   { 0, 216, 192, 168 },
   { 0, 202, 164, 130 },
   { 0, 188, 136, 92 },
   { 0, 174, 108, 54 },
   { 0, 160, 80, 16 },
   { 0, 138, 70, 14 },
   { 0, 116, 60, 12 },
   { 0, 94, 50, 10 },
   { 0, 200, 200, 200 },
   { 0, 178, 178, 178 },
   { 0, 156, 156, 156 },
   { 0, 134, 134, 134 },
   { 0, 112, 112, 112 },
   { 0, 96, 96, 96 },
   { 0, 80, 80, 80 },
   { 0, 64, 64, 64 },
   { 0, 208, 184, 232 },
   { 0, 188, 154, 222 },
   { 0, 168, 124, 212 },
   { 0, 148, 94, 202 },
   { 0, 128, 64, 192 },
   { 0, 110, 56, 166 },
   { 0, 92, 48, 140 },
   { 0, 74, 40, 114 },
   { 0, 255, 168, 255 },
   { 0, 255, 126, 255 },
   { 0, 255, 84, 255 },
   { 0, 255, 42, 255 },
   { 0, 255, 0, 255 },
   { 0, 219, 0, 219 },
   { 0, 183, 0, 183 },
   { 0, 147, 0, 147 },
   { 0, 255, 255, 168 },
   { 0, 255, 255, 126 },
   { 0, 255, 255, 84 },
   { 0, 255, 255, 42 },
   { 0, 255, 255, 0 },
   { 0, 219, 219, 0 },
   { 0, 183, 183, 0 },
   { 0, 147, 147, 0 },
   { 0, 255, 168, 168 },
   { 0, 255, 126, 126 },
   { 0, 255, 84, 84 },
   { 0, 255, 42, 42 },
   { 0, 255, 0, 0 },
   { 0, 219, 0, 0 },
   { 0, 183, 0, 0 },
   { 0, 147, 0, 0 },
   { 0, 168, 255, 168 },
   { 0, 126, 255, 126 },
   { 0, 84, 255, 84 },
   { 0, 42, 255, 42 },
   { 0, 0, 255, 0 },
   { 0, 0, 219, 0 },
   { 0, 0, 183, 0 },
   { 0, 0, 147, 0 },
   { 0, 168, 255, 255 },
   { 0, 126, 255, 255 },
   { 0, 84, 255, 255 },
   { 0, 42, 255, 255 },
   { 0, 0, 255, 255 },
   { 0, 0, 219, 219 },
   { 0, 0, 183, 183 },
   { 0, 0, 147, 147 },
   { 0, 232, 208, 184 },
   { 0, 222, 188, 154 },
   { 0, 212, 168, 124 },
   { 0, 202, 148, 94 },
   { 0, 192, 128, 64 },
   { 0, 166, 110, 56 },
   { 0, 140, 92, 48 },
   { 0, 114, 74, 40 },
};
*/

/*
 * Standard sound names
 */
char angband_sound_name[SOUND_MAX][16] =
{
	"",
	"hit",
	"miss",
	"flee",
	"drop",
	"kill",
	"level",
	"death",
	"study",
	"teleport",
	"shoot",
	"quaff",
	"zap",
	"walk",
	"tpother",
	"hitwall",
	"eat",
	"store1",
	"store2",
	"store3",
	"store4",
	"dig",
	"opendoor",
	"shutdoor",
	"tplevel",
	"scroll",
	"buy",
	"sell",
	"warn",
	"rocket",
	"n_kill",
	"u_kill",
	"quest",
	"heal",
	"x_heal",
	"bite",
	"claw",
	"m_spell",
	"summon",
	"breath",
	"ball",
	"m_heal",
	"atkspell",
	"evil",
	"touch",
	"sting",
	"crush",
	"slime",
	"wail",
	"winner",
	"fire",
	"acid",
	"elec",
	"cold",
	"illegal",
	"fail",
	"wakeup",
	"invuln",
	"fall",
	"pain",
	"destitem",
	"moan",
	"show",
	"unused",
	"explode",
};


/*
 * The array of "cave grids" [MAX_WID][MAX_HGT].
 * Not completely allocated, that would be inefficient
 * Not completely hardcoded, that would overflow memory
 */
cave_type *cave[MAX_HGT];

/*
 * The function pointer that is used to access the dungeon / wilderness.
 * It points to a simple function when in the dungeon, that evaluates
 * cave[y][x]
 * In the wilderness, things are more complicated.
 */

cave_type *(*area)(int, int);

/*
 * Variables used to access the scrollable wilderness.
 * This is designed to be as fast as possible - whilst using as little
 * RAM as possible to store a massive wilderness.
 *
 * The wilderness is generated "on the fly" as the player moves around it.
 * To save time - blocks of 16x16 squares are saved in a cache so they
 * don't need to be redone if the player moves back and forth.
 */

/* block used to generate plasma fractal for random wilderness */
u16b *temp_block[WILD_BLOCK_SIZE + 1];

/* cache of blocks near the player */
cave_type **wild_cache[WILD_BLOCKS];

/* grid of blocks around the player */
wild_grid_type wild_grid;

/* The wilderness itself */
wild_type **wild;

/* Description of wilderness block types */
wild_gen_data_type *wild_gen_data;

/* The decision tree for working out what block type to pick */
wild_choice_tree_type *wild_choice_tree;

byte *wild_temp_dist;

/*
 * The array of dungeon items [max_o_idx]
 */
object_type *o_list;

/*
 * The array of dungeon monsters [max_m_idx]
 */
monster_type *m_list;

/*
 * The array of fields [max_fld_idx]
 */
field_type *fld_list;



/*
 * Maximum number of towns
 */
u16b max_towns;

/*
 * Number of towns used.
 */
u16b town_count;

/*
 * The towns [max_towns]
 */
town_type *town;


/*
 * The player's inventory [INVEN_TOTAL]
 */
object_type *inventory;


/*
 * The size of "alloc_kind_table" (at most max_k_idx * 4)
 */
s16b alloc_kind_size;

/*
 * The entries in the "kind allocator table"
 */
alloc_entry *alloc_kind_table;


/*
 * The size of "alloc_race_table" (at most max_r_idx)
 */
s16b alloc_race_size;

/*
 * The entries in the "race allocator table"
 */
alloc_entry *alloc_race_table;


/*
 * The size of the "alloc_ego_table" (at most max_e_idx)
 */
s16b alloc_ego_size;

/*
 * The entries in the "ego item allocator table"
 */
alloc_entry *alloc_ego_table;


/*
 * Specify attr/char pairs for visual special effects
 * Be sure to use "index & 0x7F" to avoid illegal access
 *
 * Can we decrease the size to 128??
 */
byte misc_to_attr[256];
char misc_to_char[256];


/*
 * Specify attr/char pairs for inventory items (by tval)
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
byte tval_to_attr[128];
char tval_to_char[128];


/*
 * Keymaps for each "mode" associated with each keypress.
 */
cptr keymap_act[KEYMAP_MODES][256];



/*** Player information ***/

/*
 * Static player info record
 */
player_type p_body;

/*
 * Pointer to the player info
 */
player_type *p_ptr = &p_body;

/*
 * Pointer to the player tables
 * (sex, race, class, magic)
 */
player_sex *sp_ptr;
player_race *rp_ptr;
player_class *cp_ptr;
player_magic *mp_ptr;

/**** Server Information ****/

server_type s_body;

/*
 * Pointer to the server information
 */
server_type *svr_ptr = &s_body;



/*
 * The vault generation arrays
 */
header *v_head;
vault_type *v_info;
char *v_name;
char *v_text;

/*
 * The terrain feature arrays
 */
header *f_head;
feature_type *f_info;
char *f_name;
char *f_text;

/*
 * The object kind arrays
 */
header *k_head;
object_kind *k_info;
char *k_name;
char *k_text;

/*
 * The artifact arrays
 */
header *a_head;
char *a_name;
char *a_text;

/*
 * The ego-item arrays
 */
header *e_head;
ego_item_type *e_info;
char *e_name;
char *e_text;


/*
 * The monster race arrays
 */
header *r_head;
monster_race *r_info;
char *r_name;
char *r_text;

/*
 * The field thaumatergical array
 */
field_thaum *t_info;

/*
 * Hack -- The special Angband "System Suffix"
 * This variable is used to choose an appropriate "pref-xxx" file
 */
cptr ANGBAND_SYS = "xxx";

/*
 * Hack -- The special Angband "Graphics Suffix"
 * This variable is used to choose an appropriate "graf-xxx" file
 */
cptr ANGBAND_GRAF = "old";

/*
 * Path name: The main "lib" directory
 * This variable is not actually used anywhere in the code
 */
cptr ANGBAND_DIR;

/*
 * High score files (binary)
 * These files may be portable between platforms
 */
cptr ANGBAND_DIR_APEX;

/*
 * Bone files for player ghosts (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_BONE;

/*
 * Binary image files for the "*_info" arrays (binary)
 * These files are not portable between platforms
 */
cptr ANGBAND_DIR_DATA;

/*
 * Textual template files for the "*_info" arrays (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_EDIT;

/*
 * Script files
 * These files are portable between platforms.
 */
cptr ANGBAND_DIR_SCRIPT;

/*
 * Various extra files (ascii)
 * These files may be portable between platforms
 */
cptr ANGBAND_DIR_FILE;

/*
 * Help files (normal) for the online help (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_HELP;

/*
 * Help files (spoilers) for the online help (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_INFO;

/*
 * Default user "preference" files (ascii)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_PREF;

/*
 * Savefiles for current characters (binary)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_SAVE;

/*
 * User "preference" files (ascii)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_USER;

/*
 * Various extra files (binary)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_XTRA;


/* Can these inventory hacks be cleaned up somehow? */

/*
 * Total Hack -- allow all items to be listed (even empty ones)
 * This is only used by "do_cmd_inven_e()" and is cleared there.
 */
bool item_tester_full;


/*
 * Here is a "pseudo-hook" used during calls to "get_item()" and
 * "show_inven()" and "show_equip()", and the choice window routines.
 */
byte item_tester_tval;


/*
 * Here is a "hook" used during calls to "get_item()" and
 * "show_inven()" and "show_equip()", and the choice window routines.
 */
bool (*item_tester_hook)(const object_type*);


/*
 * Current "comp" function for ang_sort()
 */
bool (*ang_sort_comp)(const vptr u, const vptr v, int a, int b);


/*
 * Current "swap" function for ang_sort()
 */
void (*ang_sort_swap)(const vptr u, const vptr v, int a, int b);



/*
 * Hack -- function hooks to restrict "get_mon_num_prep()" function
 */
monster_hook_type get_mon_num_hook;

/* This is a mess. :-/ */
monster_hook_type get_mon_num2_hook;


/*
 * Hack -- function hook to restrict "get_obj_num_prep()" function
 */
byte (*get_obj_num_hook)(int k_idx);


/*
 * Maximum number of quests
 */
u16b max_quests;

/*
 * Maximum number of monsters in r_info.txt
 */
u16b max_r_idx;

/*
 * Maximum number of items in k_info.txt
 */
u16b max_k_idx;

/*
 * Maximum number of vaults in v_info.txt
 */
u16b max_v_idx;

/*
 * Maximum number of terrain features in f_info.txt
 */
u16b max_f_idx;

/*
 * Maximum number of ego-items in e_info.txt
 */
u16b max_e_idx;

/*
 * Maximum number of objects in the level
 */
u16b max_o_idx;

/*
 * Maximum number of monsters in the level
 */
u16b max_m_idx;

/*
 * Maximum number of fields on the level
 */
u16b max_fld_idx;

/*
 * Maximum number of field types
 */
u16b max_t_idx;


/*
 * Maximum size of the wilderness
 */
s32b max_wild_size;

/*
 * Current size of the wilderness
 */
s32b max_wild;

/*
 * Maximum number of nodes in the wilderness decision tree
 */
u16b max_w_node;

/*
 * Maximum number of types of wilderness block.
 */
u16b max_w_block;


/* Get rid of the quest stuff? */

/*
 * Quest info
 */
quest_type *quest;

/*
 * Quest text
 */
char quest_text[10][80];

/*
 * Current line of the quest text
 */
int quest_text_line;

/*
 * Default spell color table (quark index)
 */
cptr gf_color[MAX_GF];



/* Get rid of this? */

/*
 * Flags for initialization
 */
int init_flags;


/*
 * The "highscore" file descriptor, if available.
 */
int highscore_fd = -1;

/*
 * Should the monster allocation fail with inappropriate terrain?
 *
 * This hack is only used by the polymoph function... this probably
 * could be removed, and that function done a different way.
 */
bool monster_terrain_sensitive = TRUE;


/* Get rid of this... */
int mutant_regenerate_mod = 100;

/*  New Cost Table System  */
int cost_matrix[500];

