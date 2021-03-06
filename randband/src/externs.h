/* File: externs.h */

/* Purpose: extern declarations (variables and functions) */

/*
 * Note that some files have their own header files
 * (z-virt.h, z-util.h, z-form.h, term.h, random.h)
 */


/*
 * Automatically generated "variable" declarations
 */

/* tables.c */
extern const s16b ddd[9];
extern const s16b ddx[10];
extern const s16b ddy[10];
extern const s16b ddx_ddd[9];
extern const s16b ddy_ddd[9];
extern const s16b cdd[8];
extern const s16b ddx_cdd[8];
extern const s16b ddy_cdd[8];
extern const char hexsym[16];
extern const char listsym[];
extern cptr color_char;
extern const byte adj_mag_study[];
extern const byte adj_mag_mana[];
extern const byte adj_mag_fail[];
extern const byte adj_mag_stat[];
extern const byte adj_chr_gold[];
extern const byte adj_int_dev[];
extern const byte adj_wis_sav[];
extern const byte adj_dex_dis[];
extern const byte adj_int_dis[];
extern const byte adj_dex_ta[];
extern const byte adj_str_td[];
extern const byte adj_dex_th[];
extern const byte adj_str_wgt[];
extern const byte adj_str_hold[];
extern const byte adj_str_dig[];
extern const byte adj_str_blow[];
extern const byte adj_dex_blow[];
extern const byte adj_dex_safe[];
extern const byte adj_con_fix[];
extern const byte adj_con_mhp[];
extern const byte blows_table[12][12];
extern const owner_type owners[MAX_STORES][MAX_OWNERS];
extern const b_own_type b_owners[MAX_BLDG][MAX_B_OWN];
extern const byte rgold_adj[MAX_RACES][MAX_RACES];
extern const byte extract_energy[200];
extern const s32b player_exp[PY_MAX_LEVEL];
extern player_sex sex_info[MAX_SEXES];
extern player_race race_info[MAX_RACES];
extern player_class class_info[MAX_CLASS];
extern player_magic magic_info[MAX_CLASS];
/* */
extern obj_theme qreward_theme[MAX_CLASS];
/* */
extern const u32b fake_spell_flags[4];
extern const byte realm_choices1[];
extern const byte realm_choices2[];
extern cptr realm_names[];
extern cptr spell_names[8][32];
extern const int deadliness_conversion[512];
extern const byte chest_traps[64];
extern cptr player_title[MAX_CLASS][PY_MAX_LEVEL/5];
extern cptr color_names[16];
extern cptr stat_names[A_MAX];
extern cptr stat_names_reduced[A_MAX];
extern cptr window_flag_desc[32];
extern const int birth_options[OPT_BIRTH + 1];
extern const int server_options[OPT_BIRTH + 1];
extern cptr chaos_patrons[MAX_PATRON];
extern const int chaos_stats[MAX_PATRON];
extern const int chaos_rewards[MAX_PATRON][20];
extern const martial_arts ma_blows[MAX_MA];
extern cptr game_inscriptions[FEEL_MAX];
extern cptr silly_attacks[MAX_SILLY_ATTACK];
extern const field_action f_action[];
extern const mutation_type mutations[MUT_SETS_MAX * MUT_PER_SET];
extern const mutation_type race_powers[MAX_RACE_POWERS];

/* variable.c */
extern cptr copyright[5];
extern byte version_major;
extern byte version_minor;
extern byte version_patch;
extern byte version_extra;
extern byte sf_major;
extern byte sf_minor;
extern byte sf_patch;
extern byte sf_extra;
extern u32b sf_version;
extern u32b sf_xtra;
extern byte z_major;
extern byte z_minor;
extern byte z_patch;
extern u32b sf_when;
extern u16b sf_lives;
extern u16b sf_saves;
extern bool arg_fiddle;
extern bool arg_wizard;
extern bool arg_sound;
extern byte arg_graphics;
extern bool arg_monochrome;
extern bool arg_force_original;
extern bool arg_force_roguelike;
extern bool character_generated;
extern bool character_dungeon;
extern bool character_loaded;
extern bool character_saved;
extern bool character_icky;
extern bool character_xtra;
extern u32b seed_flavor;
extern bool msg_flag;
extern s16b min_hgt;
extern s16b max_hgt;
extern s16b min_wid;
extern s16b max_wid;
extern obj_theme dun_theme;
extern s16b num_repro;
extern s16b object_level;
extern s16b monster_level;
extern s16b base_level;
extern s32b turn;
extern s32b old_turn;
extern bool use_sound;
extern bool use_graphics;
extern bool use_transparency;
extern s16b signal_count;
extern bool inkey_base;
extern bool inkey_xtra;
extern bool inkey_scan;
extern bool inkey_flag;
extern bool shimmer_monsters;
extern bool repair_monsters;
extern s16b o_max;
extern s16b o_cnt;
extern s16b m_max;
extern s16b m_cnt;
extern s16b fld_max;
extern s16b fld_cnt;
extern s16b hack_m_idx;
extern s16b hack_m_idx_ii;
extern s16b *hack_fld_ptr;
extern int total_friends;
extern s32b total_friend_levels;
extern s32b friend_align;
extern int leaving_quest;
extern s16b store_cache_num;
extern store_type **store_cache;
extern char summon_kin_type;
extern bool hack_mutation;
extern bool track_follow;
extern bool track_target;
extern byte hitpoint_warn;
extern byte delay_factor;
extern s16b autosave_freq;
extern byte autosave_t;
extern byte autosave_l;
extern bool cheat_peek;
extern bool cheat_hear;
extern bool cheat_room;
extern bool cheat_xtra;
extern bool cheat_know;
extern bool cheat_live;
extern byte feeling;
extern s16b rating;
extern bool good_item_flag;
extern bool closing_flag;
extern bool fake_monochrome;
extern s16b max_panel_rows, max_panel_cols;
extern s16b panel_row_min, panel_row_max;
extern s16b panel_col_min, panel_col_max;
extern s16b panel_col_prt, panel_row_prt;
extern byte *mp_a;
extern char *mp_c;
#ifdef USE_TRANSPARENCY
extern byte *mp_ta;
extern char *mp_tc;
#endif /* USE_TRANSPARENCY */
extern int player_uid;
extern int player_euid;
extern int player_egid;
extern char player_name[32];
extern char player_base[32];
extern char savefile[1024];
extern s16b view_n;
extern s16b view_y[VIEW_MAX];
extern s16b view_x[VIEW_MAX];
extern s16b temp_n;
extern s16b temp_y[TEMP_MAX];
extern s16b temp_x[TEMP_MAX];
extern s16b lite_n;
extern s16b lite_y[LITE_MAX];
extern s16b lite_x[LITE_MAX];
extern s16b macro__num;
extern cptr *macro__pat;
extern cptr *macro__act;
extern bool *macro__cmd;
extern char *macro__buf;
extern s16b quark__num;
extern cptr *quark__str;
extern u16b quark__tim;
extern u16b *quark__use;
extern option_type option_info[OPT_MAX];
extern u32b window_flag[8];
extern u32b window_mask[8];
extern u32b option_mask[8];
extern term *angband_term[8];
extern char angband_term_name[8][16];
extern byte angband_color_table[256][4];
extern char angband_sound_name[SOUND_MAX][16];
extern cave_type *cave[MAX_HGT];
extern cave_type *(*area)(int, int);
extern u16b *temp_block[WILD_BLOCK_SIZE + 1];
extern cave_type **wild_cache[WILD_BLOCKS];
extern wild_grid_type wild_grid;
extern wild_type **wild;
extern wild_gen_data_type *wild_gen_data;
extern wild_choice_tree_type *wild_choice_tree;
extern byte *wild_temp_dist;
extern object_type *o_list;
extern monster_type *m_list;
extern field_type *fld_list;
extern u16b max_towns;
extern u16b town_count;
extern town_type *town;
extern object_type *inventory;
extern s16b alloc_kind_size;
extern alloc_entry *alloc_kind_table;
extern s16b alloc_race_size;
extern alloc_entry *alloc_race_table;
extern s16b alloc_ego_size;
extern alloc_entry *alloc_ego_table;
extern byte misc_to_attr[256];
extern char misc_to_char[256];
extern byte tval_to_attr[128];
extern char tval_to_char[128];
extern cptr keymap_act[KEYMAP_MODES][256];
extern player_type *p_ptr;
extern player_sex *sp_ptr;
extern player_race *rp_ptr;
extern player_class *cp_ptr;
extern player_magic *mp_ptr;
extern server_type *svr_ptr;
extern header *v_head;
extern vault_type *v_info;
extern char *v_name;
extern char *v_text;
extern header *f_head;
extern feature_type *f_info;
extern char *f_name;
extern char *f_text;
extern header *k_head;
extern object_kind *k_info;
extern char *k_name;
extern char *k_text;
extern header *a_head;
extern char *a_name;
extern char *a_text;
extern header *e_head;
extern ego_item_type *e_info;
extern char *e_name;
extern char *e_text;
extern header *r_head;
extern monster_race *r_info;
extern char *r_name;
extern char *r_text;
extern field_thaum *t_info;
extern cptr ANGBAND_SYS;
extern cptr ANGBAND_GRAF;
extern cptr ANGBAND_DIR;
extern cptr ANGBAND_DIR_APEX;
extern cptr ANGBAND_DIR_BONE;
extern cptr ANGBAND_DIR_DATA;
extern cptr ANGBAND_DIR_EDIT;
extern cptr ANGBAND_DIR_SCRIPT;
extern cptr ANGBAND_DIR_FILE;
extern cptr ANGBAND_DIR_HELP;
extern cptr ANGBAND_DIR_INFO;
extern cptr ANGBAND_DIR_PREF;
extern cptr ANGBAND_DIR_SAVE;
extern cptr ANGBAND_DIR_USER;
extern cptr ANGBAND_DIR_XTRA;
extern bool item_tester_full;
extern byte item_tester_tval;
extern bool (*item_tester_hook)(const object_type *o_ptr);
extern bool (*ang_sort_comp)(const vptr u, const vptr v, int a, int b);
extern void (*ang_sort_swap)(const vptr u, const vptr v, int a, int b);
extern monster_hook_type get_mon_num_hook;
extern monster_hook_type get_mon_num2_hook;
extern byte (*get_obj_num_hook)(int k_idx);
extern u16b max_quests;
extern u16b max_r_idx;
extern u16b max_k_idx;
extern u16b max_v_idx;
extern u16b max_f_idx;
extern u16b max_e_idx;
extern u16b max_o_idx;
extern u16b max_m_idx;
extern u16b max_fld_idx;
extern u16b max_t_idx;
extern s32b max_wild_size;
extern s32b max_wild;
extern u16b max_w_node;
extern u16b max_w_block;
extern quest_type *quest;
extern char quest_text[10][80];
extern int quest_text_line;
extern cptr gf_color[MAX_GF];
extern int highscore_fd;
extern bool monster_terrain_sensitive;
extern int mutant_regenerate_mod;
extern int cost_matrix[500];

/* birth.c */
extern void player_birth(void);

/* cave.c */
extern int distance(int y1, int x1, int y2, int x2);
extern bool is_trap(cave_type *c_ptr);
extern bool is_visible_trap(cave_type *c_ptr);
extern bool los(int y1, int x1, int y2, int x2);
extern void mmove2(int *y, int *x, int y1, int x1, int y2, int x2, int *slope,
	 int *sq);
extern bool projectable(int y1, int x1, int y2, int x2);
extern sint project_path(coord *gp, int y1, int x1, int y2, int x2, u16b flg);
extern bool in_ball_range(int y1, int x1, int y2, int x2);
extern bool in_disintegration_range(int y1, int x1, int y2, int x2);
extern void scatter(int *yp, int *xp, int y, int x, int d);
extern bool player_can_see_bold(int y, int x);
extern bool cave_valid_grid(const cave_type *c_ptr);
extern bool no_lite(void);
#ifdef USE_TRANSPARENCY
extern void map_info(int y, int x, byte *ap, char *cp, byte *tap, char *tcp);
#else /* USE_TRANSPARENCY */
extern void map_info(int y, int x, byte *ap, char *cp);
#endif /* USE_TRANSPARENCY */
extern void move_cursor_relative(int row, int col);
extern void print_rel(char c, byte a, int y, int x);
extern void note_spot(int y, int x);
extern void note_wild_spot(cave_type *c_ptr);
extern void display_dungeon(void);
extern void lite_spot(int y, int x);
extern void prt_map(void);
extern void display_map(int *cy, int *cx);
extern void do_cmd_view_map(void);
extern void forget_view(void);
extern errr vinfo_init(void);
extern void update_view(void);
extern void update_mon_lite(void);
extern void clear_mon_lite(void);
extern void forget_flow(void);
extern void update_flow(void);
extern void map_area(void);
extern void wiz_lite(void);
extern void wiz_dark(void);
extern void cave_set_feat(int y, int x, int feat);

extern void health_track(int m_idx);
extern void monster_race_track(int r_idx);
extern void object_kind_track(int k_idx);
extern void disturb(bool stop_search);

/* cmd1.c */
extern int deadliness_calc(int attack_power);
extern bool test_hit_fire(int chance, int ac, int vis);
extern bool test_hit_norm(int chance, int ac, int vis);
extern s16b tot_dam_aux(const object_type *o_ptr, int tdam, const monster_type *m_ptr);
extern void search(void);
extern bool auto_pickup_okay(object_type *o_ptr);
extern void py_pickup_aux(int o_idx);
extern void carry(int pickup);
extern int calc_to_dmg_bonus(int todamage);
extern void py_attack(int y, int x);
extern void move_player(int dir, int do_pickup);
extern void run_step(int dir);

/* cmd2.c */
extern void do_cmd_go_up(void);
extern void do_cmd_go_down(void);
extern void do_cmd_search(void);
extern void do_cmd_toggle_search(void);
extern int count_traps(int *y, int *x, bool under);
extern void do_cmd_open(void);
extern void do_cmd_close(void);
extern void do_cmd_tunnel(void);
extern void do_cmd_disarm(void);
extern void do_cmd_alter(void);
extern void do_cmd_spike(void);
extern void do_cmd_walk(int pickup);
extern void do_cmd_stay(int pickup);
extern void do_cmd_run(void);
extern void do_cmd_rest(void);
extern void do_cmd_fire(void);
extern void do_cmd_fire_aux(int item, object_type *j_ptr);
extern void do_cmd_throw(void);
extern void do_cmd_throw_aux(int mult);
extern bool do_cmd_open_aux(int y, int x);
extern bool do_cmd_disarm_aux(cave_type *c_ptr, int dir);

/* cmd3.c */
extern void do_cmd_inven(void);
extern void do_cmd_equip(void);
extern void do_cmd_wield(void);
extern void do_cmd_takeoff(void);
extern void do_cmd_drop(void);
extern bool destroy_item_aux(object_type *o_ptr, int amt);
extern void do_cmd_destroy(void);
extern void do_cmd_observe(void);
extern void do_cmd_uninscribe(void);
extern void do_cmd_inscribe(void);
extern void do_cmd_refill(void);
extern void do_cmd_target(void);
extern void do_cmd_look(void);
extern void do_cmd_locate(void);
extern bool ang_sort_comp_hook(const vptr u, const vptr v, int a, int b);
extern void ang_sort_swap_hook(const vptr u, const vptr v, int a, int b);
extern void do_cmd_query_symbol(void);
extern bool research_mon(void);

/* cmd4.c */
extern void do_cmd_redraw(void);
extern void resize_map(void);
extern void redraw_window(void);
extern void do_cmd_message_one(void);
extern void do_cmd_messages(void);
extern void init_options(byte flags);
extern void do_cmd_options(byte flags);
extern void do_cmd_pref(void);
extern void do_cmd_macros(void);
extern void do_cmd_visuals(void);
extern void do_cmd_colors(void);
extern void do_cmd_note(void);
extern void do_cmd_version(void);
extern void do_cmd_feeling(void);
extern void do_cmd_load_screen(void);
extern void do_cmd_save_screen(void);
extern void do_cmd_knowledge(void);
extern void plural_aux(char * Name);
extern void do_cmd_checkquest(void);
extern void do_cmd_time(void);

/* cmd5.c */
extern void do_cmd_browse(void);
extern void do_cmd_browse_aux(const object_type *o_ptr);
extern void do_cmd_study(void);
extern void do_cmd_cast(void);
extern void do_cmd_pray(void);
extern void do_cmd_pet(void);

/* cmd6.c */
extern void do_cmd_eat_food(void);
extern void do_cmd_quaff_potion(void);
extern void do_cmd_read_scroll(void);
extern void do_cmd_aim_wand(void);
extern void do_cmd_use_staff(void);
extern void do_cmd_zap_rod(void);
extern void do_cmd_activate(void);
extern void do_cmd_rerate(void);
extern void ring_of_power(int dir);
extern void do_cmd_use(void);

/* dungeon.c */
extern void play_game(bool new_game);
extern void notice_lite_change(object_type *o_ptr);
extern bool psychometry(void);


/* files.c */
extern void safe_setuid_drop(void);
extern void safe_setuid_grab(void);
extern s16b tokenize(char *buf, s16b num, char **tokens, int mode);
extern void display_player(int mode);
extern void do_cmd_character(void);
extern errr file_character(cptr name, bool full);
extern errr process_pref_file_command(char *buf);
extern errr process_pref_file(cptr name);
extern void print_equippy(void);
extern errr check_time_init(void);
extern errr check_load_init(void);
extern errr check_time(void);
extern errr check_load(void);
extern bool show_file(cptr name, cptr what, int line, int mode);
extern void do_cmd_help(void);
extern void process_player_name(bool sf);
extern void change_player_name(void);
extern void do_cmd_suicide(void);
extern void do_cmd_save_game(int is_autosave);
extern void do_cmd_save_and_exit(void);
extern void close_game(void);
extern void exit_game_panic(void);
extern void signals_ignore_tstp(void);
extern void signals_handle_tstp(void);
extern void signals_init(void);
extern errr get_rnd_line(cptr file_name, int entry, char *output);
extern void get_character_name(void);

/* generate.c */
extern void place_closed_door(int y, int x);
extern void map_panel_size(void);
extern void generate_cave(void);

/* init1.c */
extern errr init_v_info_txt(FILE *fp, char *buf, bool start);
extern errr init_f_info_txt(FILE *fp, char *buf);
extern errr init_k_info_txt(FILE *fp, char *buf);
extern errr init_e_info_txt(FILE *fp, char *buf);
extern errr init_r_info_txt(FILE *fp, char *buf);
extern errr init_w_info_txt(FILE *fp, char *buf);
extern errr init_t_info_txt(FILE *fp, char *buf);
extern errr process_dungeon_file(cptr name, int init_flags);

/* init2.c */
extern cptr err_str[PARSE_ERROR_MAX];
extern errr init_v_info(void);
extern errr init_w_info(void);
extern errr init_t_info(void);
extern void init_file_paths(char *path);
extern void init_angband(void);
#ifdef ALLOW_TEMPLATES
extern s16b error_idx;
extern s16b error_line;
extern u32b fake_name_size;
extern u32b fake_text_size;
#endif /* ALLOW_TEMPLATES */

/* load.c */
extern errr rd_savefile_new(void);

/* melee1.c */
/* melee2.c */
extern bool make_attack_normal(int m_idx);
extern bool make_attack_spell(int m_idx);
extern void process_monsters(int min_energy);
extern void curse_equipment(int chance, int heavy_chance);
extern void mon_take_hit_mon(int m_idx, int dam, bool *fear, cptr note);

/* monster1.c */
extern void screen_roff(int r_idx, int remember);
extern void display_roff(int r_idx);
extern void display_visible(void);
extern void create_name(int type, char *name);

/* monster2.c */
extern cptr horror_desc[MAX_SAN_HORROR];
extern cptr funny_desc[MAX_SAN_FUNNY];
extern cptr funny_comments[MAX_SAN_COMMENT];
extern void delete_monster_idx(int i);
extern void delete_monster(int y, int x);
extern void compact_monsters(int size);
extern void wipe_m_list(void);
extern s16b m_pop(void);
extern void get_mon_num_prep(monster_hook_type monster_hook, monster_hook_type monster_hook2);
extern s16b get_mon_num(int level);
extern void monster_desc(char *desc, const monster_type *m_ptr, int mode);
extern void lore_do_probe(int m_idx);
extern void lore_treasure(int m_idx, int num_item, int num_gold);
extern void update_mon_vis(u16b r_idx, int increment);
extern void update_mon(int m_idx, bool full);
extern void update_monsters(bool full);
extern bool place_monster_aux(int y, int x, int r_idx, bool slp, bool grp, bool friendly, bool pet);
extern bool place_monster(int y, int x, bool slp, bool grp);
extern bool alloc_horde(int y, int x);
extern bool alloc_monster(int dis, bool slp);
extern bool summon_specific(int who, int y1, int x1, int lev, int type, bool group, bool friendly, bool pet);
extern bool summon_named_creature(int oy, int ox, int r_idx, bool slp, bool group_ok, bool pet);
extern bool multiply_monster(int m_idx, bool clone, bool friendly, bool pet);
extern void update_smart_learn(int m_idx, int what);
extern bool place_monster_one(int y, int x, int r_idx, bool slp, bool friendly, bool pet);
extern void monster_drop_carried_objects(monster_type *m_ptr);

/* monster3.c (currently in monster1.c) */
extern bool monster_dungeon(int r_idx);
extern bool monster_quest(int r_idx);
extern bool monster_ocean(int r_idx);
extern bool monster_shore(int r_idx);
extern bool monster_town(int r_idx);
extern bool monster_grass(int r_idx);
extern bool monster_deep_water_dun(int r_idx);
extern bool monster_shallow_water_dun(int r_idx);
extern bool monster_lava_dun(int r_idx);
extern bool monster_acid_dun(int r_idx);
extern bool monster_swamp_dun(int r_idx);
extern bool monster_deep_water_wild(int r_idx);
extern bool monster_shallow_water_wild(int r_idx);
extern bool monster_lava_wild(int r_idx);
extern bool monster_acid_wild(int r_idx);
extern bool monster_swamp_wild(int r_idx);
extern monster_hook_type get_monster_hook(void);
extern monster_hook_type get_monster_hook2(int y, int x);
extern void set_friendly(monster_type *m_ptr);
extern void set_pet(monster_type *m_ptr);
extern void set_hostile(monster_type *m_ptr);
extern void anger_monster(monster_type *m_ptr);
extern bool monster_can_cross_terrain(byte feat, monster_race *r_ptr);
extern bool are_enemies(const monster_type *m_ptr1, const monster_type *m_ptr2);
extern bool monster_living(const monster_race *r_ptr);

/* flavor.c */
extern void get_table_name(char *out_string, bool quotes);
extern void flavor_init(void);
extern void object_desc(char *buf, const object_type *o_ptr, int pref, int mode);
extern void object_desc_store(char *buf, const object_type *o_ptr, int pref, int mode);

/* object1.c */
/* object2.c */

extern void reset_visuals(void);
extern void object_flags(const object_type *o_ptr, u32b *f1, u32b *f2, u32b *f3, u32b *f4, u32b *f5, u32b *f6);
extern void object_flags_known(const object_type *o_ptr, u32b *f1, u32b *f2, u32b *f3, u32b *f4, u32b *f5, u32b *f6);
extern bool identify_fully_aux(const object_type *o_ptr);
extern char index_to_label(int i);
extern s16b label_to_inven(int c);
extern s16b label_to_equip(int c);
extern s16b wield_slot(const object_type *o_ptr);
extern cptr mention_use(int i);
extern cptr describe_use(int i);
extern void inven_item_charges(int item);
extern void inven_item_describe(int item);
extern void inven_item_increase(int item, int num);
extern void inven_item_optimize(int item);
extern void floor_item_charges(int item);
extern void floor_item_describe(int item);
extern void floor_item_increase(int item, int num);
extern void floor_item_optimize(int item);
extern bool inven_carry_okay(const object_type *o_ptr);
extern s16b inven_carry(object_type *o_ptr);
extern s16b inven_takeoff(int item, int amt);
extern void inven_drop(int item, int amt);
extern bool item_tester_hook_artifactable(const object_type *o_ptr);  /*  NEW  */
extern bool item_tester_hook_weapon(const object_type *o_ptr);
extern bool item_tester_hook_melee_weapon(const object_type *o_ptr);
extern bool item_tester_hook_nonsword(const object_type *o_ptr);
extern bool item_tester_hook_ammo(const object_type *o_ptr);
extern bool item_tester_hook_fletcher(const object_type *o_ptr);
extern bool item_tester_hook_armour(const object_type *o_ptr);
extern bool item_tester_hook_soft_armour(const object_type *o_ptr);
extern bool item_tester_hook_hard_armour(const object_type *o_ptr);
extern bool item_tester_hook_helm(const object_type *o_ptr);
extern bool item_tester_hook_pure_hard_armour(const object_type *o_ptr);
extern bool item_tester_hook_weapon_armour(const object_type *o_ptr);
extern bool item_tester_hook_wear(const object_type *o_ptr);
extern bool item_tester_hook_recharge(const object_type *o_ptr);
extern bool item_tester_hook_jewel(const object_type *o_ptr);
extern bool item_tester_hook_tval(const object_type *o_ptr);
extern bool item_tester_hook_is_blessed(const object_type *o_ptr);
extern bool item_tester_hook_is_good(const object_type *o_ptr);
extern bool item_tester_hook_is_great(const object_type *o_ptr);
extern bool item_tester_hook_is_book(const object_type *o_ptr);
extern bool item_tester_okay(const object_type *o_ptr);
extern void display_inven(void);
extern void display_equip(void);
extern void show_inven(void);
extern void show_equip(void);
extern void toggle_inven_equip(void);
extern bool get_item(int *cp, cptr pmt, cptr str, int mode);
extern void excise_object_idx(int o_idx);
extern void delete_object_idx(int o_idx);
extern void delete_object(int y, int x);
extern void delete_object_location(cave_type *c_ptr);
extern void compact_objects(int size);
extern void wipe_o_list(void);
extern s16b o_pop(void);
extern void get_obj_num_prep(void);
extern errr get_obj_store_prep(void);
extern s16b get_obj_num(int level, int min_level);
extern void object_known(object_type *o_ptr);
extern void object_aware(object_type *o_ptr);
extern void object_tried(object_type *o_ptr);
extern s32b flag_cost(const object_type *o_ptr, int plusses);
extern s32b object_value(const object_type *o_ptr);
extern s32b object_value_real(const object_type *o_ptr);
extern void distribute_charges(object_type *o_ptr, object_type *q_ptr, int amt);
extern void reduce_charges(object_type *o_ptr, int amt);
extern bool object_similar(const object_type *o_ptr, const object_type *j_ptr);
extern void object_absorb(object_type *o_ptr, object_type *j_ptr);
extern s16b lookup_kind(int tval, int sval);
extern void object_wipe(object_type *o_ptr);
extern void set_durability(object_type *d_ptr, int dval); /* NEW */
extern void object_prep(object_type *o_ptr, int k_idx);
extern void object_copy(object_type *o_ptr, const object_type *j_ptr);
extern void add_ego_flags(object_type *o_ptr, int ego);
extern bool make_artifact(object_type *o_ptr);
extern void add_ego_power(int power, object_type *o_ptr);
extern void apply_magic(object_type *o_ptr, int lev, int lev_dif, byte flags, bool store);
extern void init_match_hook(byte tval, byte sval);
extern byte kind_is_match(int k_idx);
extern void init_match_theme(obj_theme theme);
extern byte kind_is_theme(int k_idx);
extern bool make_object(object_type *o_ptr, u16b delta_level, obj_theme theme);
extern void place_object(int y, int x, bool good, bool great);
extern bool make_gold(object_type *j_ptr, int coin_type);
extern void place_gold(int y, int x);
extern s16b drop_near(object_type *o_ptr, int chance, int y, int x);
extern void acquirement(int y1, int x1, int num, bool great, bool known);
extern cptr item_activation(const object_type *o_ptr);
extern void combine_pack(void);
extern void reorder_pack(void);
extern bool can_player_destroy_object(object_type *o_ptr);
extern void display_koff(int k_idx);
extern bool scan_floor(int *items, int *item_num, int y, int x, int mode);
extern void show_floor(int y, int x);
extern void set_cost_matrix();

/* racial.c */
extern bool racial_aux(s16b min_level, int cost, int use_stat, int difficulty);
extern void do_cmd_racial_power(void);

/* save.c */
extern bool save_player(void);
extern bool load_player(void);
extern void remove_loc(void);

/* spells1.c */
extern void take_hit(int damage, cptr kb_str, bool Physical);/*  3rd is for Durability */
extern void bolt_pict(int y, int x, int ny, int nx, int typ, byte *a, byte *c);
extern int dist_to_line(int y, int x, int y1, int x1, int y2, int x2);
extern bool project(int who, int rad, int y, int x, int dam, int typ, u16b flg);

/* spells2.c */
extern void message_pain(int m_idx, int dam);
extern void self_knowledge(void);
extern bool detect_traps(void);
extern bool detect_doors(void);
extern bool detect_stairs(void);
extern bool detect_treasure(void);
extern bool detect_objects_gold(void);
extern bool detect_objects_normal(void);
extern bool detect_objects_magic(void);
extern bool detect_monsters_normal(void);
extern bool detect_monsters_invis(void);
extern bool detect_monsters_evil(void);
extern bool detect_monsters_xxx(u32b match_flag);
extern bool detect_monsters_string(cptr);
extern bool detect_monsters_nonliving(void);
extern bool detect_monsters_living(void);
extern bool detect_all(void);
extern bool wall_stone(void);
extern bool speed_monsters(void);
extern bool slow_monsters(void);
extern bool sleep_monsters(void);
extern void aggravate_monsters(int who);
extern bool genocide(int player_cast);
extern bool mass_genocide(int player_cast);
extern bool probing(void);
extern bool banish_evil(int dist);
extern bool dispel_evil(int dam);
extern bool dispel_good(int dam);
extern bool dispel_undead(int dam);
extern bool dispel_monsters(int dam);
extern bool dispel_living(int dam);
extern bool dispel_demons(int dam);
extern bool raise_dead(int y, int x, bool pet);
extern bool turn_undead(void);
extern bool destroy_area(int y1, int x1, int r);
extern bool earthquake(int cy, int cx, int r);
extern void lite_room(int y1, int x1);
extern void unlite_room(int y1, int x1);
extern bool lite_area(int dam, int rad);
extern bool unlite_area(int dam, int rad);
extern bool fire_ball(int typ, int dir, int dam, int rad);
extern bool fire_bolt(int typ, int dir, int dam);
extern void call_chaos(void);
extern bool fire_beam(int typ, int dir, int dam);
extern bool fire_bolt_or_beam(int prob, int typ, int dir, int dam);
extern bool lite_line(int dir);
extern bool drain_life(int dir, int dam);
extern bool drain_gain_life(int dor, int dam);
extern bool death_ray(int dir, int plev);
extern bool wall_to_mud(int dir);
extern bool destroy_door(int dir);
extern bool disarm_trap(int dir);
extern bool wizard_lock(int dir);
extern bool heal_monster(int dir);
extern bool speed_monster(int dir);
extern bool slow_monster(int dir);
extern bool sleep_monster(int dir);
extern bool stasis_monster(int dir);    /* Like sleep, affects undead as well */
extern bool confuse_monster(int dir, int plev);
extern bool stun_monster(int dir, int plev);
extern bool fear_monster(int dir, int plev);
extern bool poly_monster(int dir);
extern bool clone_monster(int dir);
extern bool teleport_monster(int dir);
extern bool door_creation(void);
extern bool trap_creation(void);
extern bool glyph_creation(void);
extern bool destroy_doors_touch(void);
extern bool sleep_monsters_touch(void);
extern bool activate_ty_curse(bool stop_ty, int *count);
extern int activate_hi_summon(void);
extern int summon_cyber(int who, int y, int x);
extern void wall_breaker(void);
extern bool confuse_monsters(int dam);
extern bool charm_monsters(int dam);
extern bool charm_animals(int dam);
extern bool stun_monsters(int dam);
extern bool stasis_monsters(int dam);
extern bool banish_monsters(int dist);
extern bool turn_monsters(int dam);
extern bool turn_evil(int dam);
extern bool deathray_monsters(void);
extern bool charm_monster(int dir, int plev);
extern bool control_one_undead(int dir, int plev);
extern bool charm_animal(int dir, int plev);
extern bool mindblast_monsters(int dam);
extern void report_magics(void);
extern bool teleport_swap(int dir);
extern bool project_hook(int typ, int dir, int dam, u16b flg);
extern bool project_hack(int typ, int dam);

/* spells3.c */
extern bool teleport_away(int m_idx, int dis);
extern void teleport_to_player(int m_idx);
extern void teleport_player(int dis);
extern void teleport_player_to(int ny, int nx);
extern void teleport_player_level(void);
extern bool check_down_wild(void);
extern void recall_player(int turns);
extern void word_of_recall(void);
extern bool apply_disenchant(void);
extern void mutate_player(void);
extern void apply_nexus(const monster_type *m_ptr);
extern void phlogiston(void);
extern void brand_weapon(int brand_type);
extern void call_the_(void);
extern void fetch(int dir, int wgt, bool require_los);
extern void alter_reality(void);
extern bool warding_glyph(void);
extern bool explosive_rune(void);
extern void identify_pack(void);
extern void identify_pack2(void);
extern bool remove_curse(void);
extern bool remove_all_curse(void);
extern bool alchemy(void);
extern void stair_creation(void);
extern bool enchant(object_type *o_ptr, int n, int eflag);
extern bool enchant_spell(int num_hit, int num_dam, int num_ac);
extern bool artifact_scroll(void);
extern bool ident_spell(void);
extern bool mundane_spell(void);
extern void identify_item(object_type *o_ptr);
extern bool identify_fully(void);
extern bool recharge(int num);
extern bool bless_weapon(void);
extern bool potion_smash_effect(int who, int y, int x, int k_idx);
extern void display_spell_list(void);
extern s16b spell_chance(int spell, int realm);
extern bool spell_okay(int spell, bool known, int realm);
extern void print_spells(byte *spells, int num, int y, int x, int realm);
extern bool hates_acid(const object_type *o_ptr);
extern bool hates_elec(const object_type *o_ptr);
extern bool hates_fire(const object_type *o_ptr);
extern bool hates_cold(const object_type *o_ptr);
extern int set_acid_destroy(object_type *o_ptr);
extern int set_elec_destroy(object_type *o_ptr);
extern int set_fire_destroy(object_type *o_ptr);
extern int set_cold_destroy(object_type *o_ptr);
extern int inven_damage(inven_func typ, int perc, int Damage);
extern void acid_dam(int dam, cptr kb_str);
extern void elec_dam(int dam, cptr kb_str);
extern void fire_dam(int dam, cptr kb_str);
extern void cold_dam(int dam, cptr kb_str);
extern bool rustproof(void);
extern bool curse_armor(void);
extern bool curse_weapon(void);
extern bool brand_bolts(void);
extern bool polymorph_monster(int y, int x);
extern bool dimension_door(void);
extern void map_wilderness(int radius, s32b x, s32b y);
extern void sanity_blast(const monster_type *m_ptr);

/* store.c */
extern bool allocate_store(store_type *st_ptr);
extern void do_cmd_store(field_type *f_ptr);
extern void store_init(int town_num, int store_num, byte store_type);

/* bldg.c */
extern bool get_nightmare(int r_idx);
extern void have_nightmare(int r_idx);
extern void do_cmd_bldg(field_type *f_ptr);
extern bool compare_weapons(void);
extern bool enchant_item(s32b cost, bool to_hit, bool to_dam, bool to_ac);
extern void building_recharge(s32b cost);
extern bool building_healer(void);
extern void do_cmd_quest(void);
extern void quest_discovery(int q_idx);
extern int quest_number(int level);
extern int random_quest_number(int level);
extern int number_of_quests(void);
extern void gamble_help(void);
extern void gamble_in_between(void);
extern void gamble_craps(void);
extern void gamble_spin_wheel(void);
extern void gamble_dice_slots(void);
extern bool inn_rest(void);
extern void build_init(int town_num, int build_num, byte build_type);

/* util.c */
extern errr path_parse(char *buf, int max, cptr file);
extern void path_build(char *buf, int max, cptr path, cptr file);
extern FILE *my_fopen(cptr file, cptr mode);
extern FILE *my_fopen_temp(char *buf, int max);
extern errr my_fgets(FILE *fff, char *buf, huge n);
extern errr my_fputs(FILE *fff, cptr buf, huge n);
extern void my_fclose(FILE *fff);
extern errr fd_kill(cptr file);
extern errr fd_move(cptr file, cptr what);
extern errr fd_copy(cptr file, cptr what);
extern int fd_make(cptr file, int mode);
extern int fd_open(cptr file, int flags);
extern errr fd_lock(int fd, int what);
extern errr fd_seek(int fd, huge n);
extern errr fd_chop(int fd, huge n);
extern errr fd_read(int fd, char *buf, huge n);
extern errr fd_write(int fd, cptr buf, huge n);
extern errr fd_close(int fd);
extern void flush(void);
extern void bell(void);
extern void sound(int num);
extern void move_cursor(int row, int col);
extern void text_to_ascii(char *buf, cptr str);
extern void ascii_to_text(char *buf, cptr str);
extern void macro_add(cptr pat, cptr act);
extern sint macro_find_exact(cptr pat);
extern char inkey(void);
extern cptr quark_str(s16b num);
extern s16b quark_add(cptr str);
extern void message_init(void);
extern s16b message_num(void);
extern cptr message_str(int age);
extern byte message_color(int age);
extern void message_add(cptr msg, byte attr);
extern void msg_print(cptr msg);
extern void msg_print_color(byte attr, cptr msg);
extern void msg_format(cptr fmt, ...);
extern void msg_format_color(byte attr, cptr fmt, ...);
extern void screen_save(void);
extern void screen_load(void);
extern void c_put_str(byte attr, cptr str, int row, int col);
extern void put_str(cptr str, int row, int col);
extern void c_prt(byte attr, cptr str, int row, int col);
extern void prt(cptr str, int row, int col);
extern void c_roff(byte attr, cptr str);
extern void roff(cptr str);
extern void clear_from(int row);
extern bool askfor_aux(char *buf, int len);
extern bool get_string(cptr prompt, char *buf, int len);
extern bool get_check(cptr prompt);
extern bool get_com(cptr prompt, char *command);
extern s16b get_quantity(cptr prompt, int max);
extern void pause_line(int row);
extern void request_command(int shopping);
extern bool is_a_vowel(int ch);
extern int get_keymap_dir(char ch);
extern void repeat_push(int what);
extern bool repeat_pull(int *what);
extern void repeat_check(void);

#ifdef SUPPORT_GAMMA
extern byte gamma_table[256];
extern void build_gamma_table(int gamma);
#endif /* SUPPORT_GAMMA */

extern cptr get_default_font(int term_num);

/* xtra1.c */
extern void cnv_stat(int val, char *out_val);
extern s16b modify_stat_value(int value, int amount);
extern void notice_stuff(void);
extern void update_stuff(void);
extern void redraw_stuff(void);
extern void window_stuff(void);
extern void handle_stuff(void);


/* effects.c */
extern bool set_blind(int v);
extern bool set_confused(int v);
extern bool set_poisoned(int v);
extern bool set_afraid(int v);
extern bool set_paralyzed(int v);
extern bool set_image(int v);
extern bool set_fast(int v);
extern bool set_slow(int v);
extern bool set_shield(int v);
extern bool set_blessed(int v);
extern bool set_hero(int v);
extern bool set_shero(int v);
extern bool set_protevil(int v);
extern bool set_invuln(int v);
extern bool set_tim_invis(int v);
extern bool set_tim_infra(int v);
extern bool set_oppose_acid(int v);
extern bool set_oppose_elec(int v);
extern bool set_oppose_fire(int v);
extern bool set_oppose_cold(int v);
extern bool set_oppose_pois(int v);
extern bool set_stun(int v);
extern bool set_cut(int v);
extern bool set_food(int v);
extern bool inc_stat(int stat, int power);
extern bool dec_stat(int stat, int amount, int permanent);
extern bool res_stat(int stat, int power);
extern bool hp_player(int num);
extern bool sp_player(int num);
extern bool do_dec_stat(int stat);
extern bool do_res_stat(int stat, int power);
extern bool do_inc_stat(int stat, int power);
extern bool restore_level(void);
extern bool lose_all_info(void);
extern void gain_exp(s32b amount);
extern void lose_exp(s32b amount);
extern void do_poly_self(void);


/* xtra2.c */
extern void check_experience(void);
extern bool monster_death(int m_idx, bool explode);
extern bool mon_take_hit(int m_idx, int dam, bool *fear, cptr note);
extern void panel_center(void);
extern bool change_panel(int dy, int dx);
extern void verify_panel(void);
extern cptr look_mon_desc(int m_idx);
extern void ang_sort_aux(vptr u, vptr v, int p, int q);
extern void ang_sort(vptr u, vptr v, int n);
extern bool target_able(int m_idx);
extern bool target_okay(void);
extern bool target_set(int mode);
extern bool get_aim_dir(int *dp);
extern bool get_hack_dir(int *dp);
extern bool get_rep_dir(int *dp);
extern int get_chaos_patron(void);
extern void gain_level_reward(int chosen_reward);
extern bool set_wraith_form(int v);
extern bool set_tim_esp(int v);
extern bool tgt_pt(int *x, int *y);
extern void do_poly_wounds(void);
extern int mon_damage_mod(const monster_type *m_ptr, int dam, int type);
extern void exp_for_kill(const monster_race *r_ptr, s32b *new_exp, s32b *new_exp_frac);

/* mspells1.c */
extern bool clean_shot(int y1, int x1, int y2, int x2, bool friendly);

/* mspells2.c */
extern bool monst_spell_monst(int m_idx);

/* artifact.c */
extern bool create_artifact(object_type *o_ptr, bool a_scroll, bool store_made);
extern int random_resistance(object_type *o_ptr, int specific, int artifact_bias);
extern int random_misc(object_type *o_ptr, int artifact_bias);
extern bool activate_effect(object_type *o_ptr);

/* scores.c */
extern void display_scores_aux(int from, int to, int note, const high_score *score);
extern void display_scores(int from, int to);
extern void kingly(void);
extern void enter_score(void);
extern void top_twenty(void);
extern void predict_score(void);
extern void race_legends(void);
extern void race_score(int race_num);
extern void show_highclass(void);

/* mind.c */
extern mindcraft_power mindcraft_powers[MINDCRAFT_MAX];
extern void mindcraft_info(char *p, int power);
extern void do_cmd_mindcraft(void);

/* mutation.c */
extern bool player_has_mut(int mutation);
extern bool gain_mutation(int choose_mut);
extern bool lose_mutation(int choose_mut);
extern void dump_mutations(FILE *OutFile);
extern void do_cmd_knowledge_mutations(void);
extern int count_mutations(void);
extern int calc_mutant_regenerate_mod(void);
extern void mutation_power_aux(const mutation_type *mut_ptr);
extern void mutation_random_aux(const mutation_type *mut_ptr);
extern void mutation_effect(void);


/* obj_kind.c */
extern errr k_info_alloc(void);
extern errr k_info_free(void);
extern object_kind *k_info_add(object_kind *k_info_entry);
extern byte get_object_level(const object_type *o_ptr);
extern cptr get_object_name(const object_type *o_ptr);
extern byte get_object_d_attr(const object_type *o_ptr);
extern byte get_object_x_attr(const object_type *o_ptr);
extern char get_object_d_char(const object_type *o_ptr);
extern char get_object_x_char(const object_type *o_ptr);
extern bool get_object_aware(const object_type *o_ptr);
extern bool get_object_tried(const object_type *o_ptr);
extern bool object_is_potion(const object_type *o_ptr);
extern errr init_object_alloc(void);
extern void k_info_reset(void);

/* wild.c */
extern void select_town_name(char *name, int pop);
extern void light_dark_block(blk_ptr block_ptr, int x, int y);
extern void change_level(int);
extern u16b init_choice_tree(wild_bound_box_type *bound, u16b type);
extern u16b add_node_tree_root(wild_bound_box_type *bound, u16b type);
extern void test_decision_tree(void);
extern void repopulate_wilderness(void);
extern void create_wilderness(void);
extern void move_wild(void);

/* avatar.c */
extern cptr virtue[MAX_VIRTUE];
extern void get_virtues(void);
extern void chg_virtue(int virtue, int amount);
extern void dump_virtues(FILE *OutFile);

/* notes.c */
extern cptr notes_file(void);
extern void output_note(cptr final_note);
extern void add_note(cptr note, char code);
extern void add_note_type(int note_number);

/* fields.c */
extern void excise_field_idx(int fld_idx);
extern void delete_field_idx(int fld_idx);
extern void delete_field_ptr(s16b *fld_id);
extern void delete_field_aux(s16b *fld_idx_ptr);
extern void delete_field(int y, int x);
extern void delete_field_location(cave_type *c_ptr);
extern void compact_fields(int size);
extern void wipe_f_list(void);
extern s16b f_pop(void);
extern void field_wipe(field_type *f_ptr);
extern void field_copy(field_type *f_ptr, field_type *j_ptr);
extern s16b field_add(field_type *f_ptr, s16b *fld_idx2);
extern void field_sort_priority(s16b *fld_idx_ptr);
extern void field_prep(field_type *f_ptr, s16b t_idx);
extern void init_fields(void);
extern s16b *field_is_type(s16b *fld_ptr, byte typ);
extern s16b *field_first_known(s16b *fld_ptr, byte typ);
extern u16b fields_have_flags(s16b fld_idx, u16b info);
extern bool field_detect_type(s16b fld_idx, byte typ);
extern void field_destroy_type(s16b fld_idx, byte typ);
extern s16b place_field(int y, int x, s16b t_idx);
extern bool field_hook_single(s16b *field_ptr, int action, vptr action_struct);
extern void field_hook(s16b *field_ptr, int action, vptr action_struct);
extern bool field_hook_special(s16b *field_ptr, u16b t_idx, vptr action_struct);
extern s16b *field_hook_find(s16b *field_ptr, int action, vptr action_struct);
extern void process_fields(void);
extern void test_field_data_integrity(void);
extern bool field_action_nothing(field_type *f_ptr, vptr nothing);
extern bool field_action_delete(field_type *f_ptr, vptr nothing);
extern bool field_action_glyph_warding(field_type *f_ptr, vptr mon_enter_test);
extern bool field_action_glyph_explode(field_type *f_ptr, vptr mon_enter_test);
extern bool field_action_corpse_decay(field_type *f_ptr, vptr nothing);
extern bool field_action_corpse_raise(field_type *f_ptr, vptr input);
extern bool field_action_corpse_load(field_type *f_ptr, vptr input);
extern bool field_action_corpse_init(field_type *f_ptr, vptr input);
extern bool field_action_corpse_look(field_type *f_ptr, vptr output);
extern bool field_action_wall_tunnel(field_type *f_ptr, vptr input);
extern bool field_action_wall_gf(field_type *f_ptr, vptr input);
extern bool field_action_interact_tunnel(field_type *f_ptr, vptr output);
extern bool field_action_interact_disarm(field_type *f_ptr, vptr output);
extern bool field_action_interact_open(field_type *f_ptr, vptr output);
extern void place_trap(int y, int x);
extern bool field_action_trap_init(field_type *f_ptr, vptr input);
extern bool field_action_trap_disarm(field_type *f_ptr, vptr input);
extern bool field_action_trap_gf(field_type *f_ptr, vptr input);
extern bool field_action_hit_trap_door(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_pit(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_spike(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_poison_pit(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_curse(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_teleport(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_element(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_ba_element(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_gas(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_traps(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_temp_stat(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_perm_stat(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_lose_xp(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_disenchant(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_drop_item(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_mutate(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_new_life(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_no_lite(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_hunger(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_no_gold(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_haste_mon(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_raise_mon(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_drain_magic(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_aggravate(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_summon(field_type *f_ptr, vptr nothing);
extern bool field_action_hit_trap_lose_memory(field_type *f_ptr, vptr nothing);
extern void make_lockjam_door(int y, int x, int power, bool jam);
extern bool field_action_counter_init(field_type *f_ptr, vptr input);
extern bool field_action_door_unlock(field_type *f_ptr, vptr input);
extern bool field_action_door_bash(field_type *f_ptr, vptr input);
extern bool field_action_door_lock_monster(field_type *f_ptr, vptr input);
extern bool field_action_door_jam_monster(field_type *f_ptr, vptr input);
extern bool field_action_door_gf(field_type *f_ptr, vptr input);
extern bool field_action_door_store(field_type *f_ptr, vptr nothing);
extern bool field_action_door_build(field_type *f_ptr, vptr nothing);
extern bool field_action_weaponmaster1(field_type *f_ptr, vptr input);
extern bool field_action_weaponmaster2(field_type *f_ptr, vptr input);
extern bool field_action_recharge1(field_type *f_ptr, vptr input);
extern bool field_action_recharge2(field_type *f_ptr, vptr input);
extern bool field_action_weaponplus1(field_type *f_ptr, vptr input);
extern bool field_action_weaponplus2(field_type *f_ptr, vptr input);
extern bool field_action_armourplus1(field_type *f_ptr, vptr input);
extern bool field_action_armourplus2(field_type *f_ptr, vptr input);
extern bool field_action_mutate1(field_type *f_ptr, vptr input);
extern bool field_action_mutate2(field_type *f_ptr, vptr input);
extern bool field_action_buymap1(field_type *f_ptr, vptr input);
extern bool field_action_buymap2(field_type *f_ptr, vptr input);
extern bool field_action_library1(field_type *f_ptr, vptr input);
extern bool field_action_library2(field_type *f_ptr, vptr input);
extern bool field_action_casino1(field_type *f_ptr, vptr nothing);
extern bool field_action_casino2(field_type *f_ptr, vptr input);
extern bool field_action_inn1(field_type *f_ptr, vptr input);
extern bool field_action_inn2(field_type *f_ptr, vptr input);
extern bool field_action_healer1(field_type *f_ptr, vptr input);
extern bool field_action_healer2(field_type *f_ptr, vptr input);
extern bool field_action_isbook_tester(field_type *f_ptr, vptr input);
extern bool field_action_isweapon_tester(field_type *f_ptr, vptr input);
extern bool field_action_isarmour_tester(field_type *f_ptr, vptr input);
extern bool field_action_isweaparmour_tester(field_type *f_ptr, vptr input);
extern bool field_action_isammo_tester(field_type *f_ptr, vptr input);
extern bool field_action_ispotion_tester(field_type *f_ptr, vptr input);
extern bool field_action_isscroll_tester(field_type *f_ptr, vptr input);
extern bool field_action_isstatue_tester(field_type *f_ptr, vptr input);
extern bool field_action_isfigurine_tester(field_type *f_ptr, vptr input);
extern bool field_action_isfood_tester(field_type *f_ptr, vptr input);
extern bool field_action_isrecharge_tester(field_type *f_ptr, vptr input);
extern bool field_action_isjewel_tester(field_type *f_ptr, vptr input);
extern bool field_action_iswield_tester(field_type *f_ptr, vptr input);
extern bool field_action_isfletcher_tester(field_type *f_ptr, vptr input);
extern bool field_action_issword_tester(field_type *f_ptr, vptr input);
extern bool field_action_isnonsword_tester(field_type *f_ptr, vptr input);
extern bool field_action_isshield_tester(field_type *f_ptr, vptr input);
extern bool field_action_isclothes_tester(field_type *f_ptr, vptr input);
extern bool field_action_ishardarmour_tester(field_type *f_ptr, vptr input);
extern bool field_action_isphardarmour_tester(field_type *f_ptr, vptr input);
extern bool field_action_ishelm_tester(field_type *f_ptr, vptr input);
extern bool field_action_issupplies_tester(field_type *f_ptr, vptr input);

/* compress.c */
extern void test_compress_module(void);

extern bool act_magic(int px, int py, int act_num, int s_lev, int s_fail,
                      int s_base, int s_bonus, int act_mult, int act_div,
                      int cast_penalty, int beam_pct);

/*
 * Hack -- conditional (or "bizarre") externs
 */

#ifdef SET_UID
/* util.c */
extern void user_name(char *buf, int id);
#endif

#ifndef HAS_MEMSET
/* util.c */
extern char *memset(char*, int, huge);
#endif

#ifndef HAS_USLEEP
/* util.c */
extern int usleep(huge usecs);
#endif

#ifdef USE_SCRIPT
extern errr script_execute(char *name);
extern errr init_script(void);
#endif /* USE_SCRIPT */

/*
 * Lair Level Info
 */
extern header *l_head;
extern layout_type *l_info;
extern char *l_name;
extern char *l_text;

