/* File: save.c */

/* Purpose: interact with savefiles */

#include "angband.h"


#ifdef FUTURE_SAVEFILES

/*
 * XXX XXX XXX Ignore this for now...
 *
 * The basic format of Angband 2.8.0 (and later) savefiles is simple.
 *
 * The savefile itself is a "header" (4 bytes) plus a series of "blocks",
 * plus, perhaps, some form of "footer" at the end.
 *
 * The "header" contains information about the "version" of the savefile.
 * Conveniently, pre-2.8.0 savefiles also use a 4 byte header, though the
 * interpretation of the "sf_extra" byte is very different.  Unfortunately,
 * savefiles from Angband 2.5.X reverse the sf_major and sf_minor fields,
 * and must be handled specially, until we decide to start ignoring them.
 *
 * Each "block" is a "type" (2 bytes), plus a "size" (2 bytes), plus "data",
 * plus a "check" (2 bytes), plus a "stamp" (2 bytes).  The format of the
 * "check" and "stamp" bytes is still being contemplated, but it would be
 * nice for one to be a simple byte-checksum, and the other to be a complex
 * additive checksum of some kind.  Both should be zero if the block is empty.
 *
 * Standard types:
 *   TYPE_BIRTH --> creation info
 *   TYPE_OPTIONS --> option settings
 *   TYPE_MESSAGES --> message recall
 *   TYPE_PLAYER --> player information
 *   TYPE_SPELLS --> spell information
 *   TYPE_INVEN --> player inven/equip
 *   TYPE_STORES --> store information
 *   TYPE_RACES --> monster race data
 *   TYPE_KINDS --> object kind data
 *   TYPE_UNIQUES --> unique info
 *   TYPE_ARTIFACTS --> artifact info
 *   TYPE_QUESTS --> quest info
 *
 * Dungeon information:
 *   TYPE_DUNGEON --> dungeon info
 *   TYPE_FEATURES --> dungeon features
 *   TYPE_OBJECTS --> dungeon objects
 *   TYPE_MONSTERS --> dungeon monsters
 *
 * Question:
 *   Should there be a single "block" for info about all the stores, or one
 *   "block" for each store?  Or one "block", which contains "sub-blocks" of
 *   some kind?  Should we dump every "sub-block", or just the "useful" ones?
 */


/*
 * XXX XXX XXX
 */
#define TYPE_OPTIONS 17362


/*
 * Hack -- current savefile
 */
static int data_fd = -1;


/*
 * Hack -- current block type
 */
static u16b data_type;

/*
 * Hack -- current block size
 */
static u16b data_size;

/*
 * Hack -- pointer to the data buffer
 */
static byte *data_head;

/*
 * Hack -- pointer into the data buffer
 */
static byte *data_next;



/*
 * Hack -- write the current "block" to the savefile
 */
static errr wr_block(void)
{
	errr err;

	byte fake[4];

	/* Save the type and size */
	fake[0] = (byte)(data_type);
	fake[1] = (byte)(data_type >> 8);
	fake[2] = (byte)(data_size);
	fake[3] = (byte)(data_size >> 8);

	/* Dump the head */
	err = fd_write(data_fd, (char*)&fake, 4);

	/* Dump the actual data */
	err = fd_write(data_fd, (char*)data_head, data_size);

	/* XXX XXX XXX */
	fake[0] = 0;
	fake[1] = 0;
	fake[2] = 0;
	fake[3] = 0;

	/* Dump the tail */
	err = fd_write(data_fd, (char*)&fake, 4);

	/* Hack -- reset */
	data_next = data_head;

	/* Wipe the data block */
	C_WIPE(data_head, 65535, byte);

	/* Success */
	return (0);
}



/*
 * Hack -- add data to the current block
 */
static void put_byte(byte v)
{
	*data_next++ = v;
}

/*
 * Hack -- add data to the current block
 */
static void put_char(char v)
{
	put_byte((byte)(v));
}

/*
 * Hack -- add data to the current block
 */
static void put_u16b(u16b v)
{
	*data_next++ = (byte)(v);
	*data_next++ = (byte)(v >> 8);
}

/*
 * Hack -- add data to the current block
 */
static void put_s16b(s16b v)
{
	put_u16b((u16b)(v));
}

/*
 * Hack -- add data to the current block
 */
static void put_u32b(u32b v)
{
	*data_next++ = (byte)(v);
	*data_next++ = (byte)(v >> 8);
	*data_next++ = (byte)(v >> 16);
	*data_next++ = (byte)(v >> 24);
}

/*
 * Hack -- add data to the current block
 */
static void put_s32b(s32b v)
{
	put_u32b((u32b)(v));
}

/*
 * Hack -- add data to the current block
 */
static void put_string(char *str)
{
	while ((*data_next++ = *str++) != '\0');
}



/*
 * Write a savefile for Angband 2.8.0
 */
static errr wr_savefile(void)
{
	int     i;

	u32b    now;

	byte    tmp8u;
	u16b    tmp16u;

	errr    err;

	byte    fake[4];


	/*** Hack -- extract some data ***/

	/* Hack -- Acquire the current time */
	now = time((time_t*)(NULL));

	/* Note the operating system */
	sf_xtra = 0L;

	/* Note when the file was saved */
	sf_when = now;

	/* Note the number of saves */
	sf_saves++;


	/*** Actually write the file ***/

	/* Open the file XXX XXX XXX */
	data_fd = -1;

	/* Dump the version */
	fake[0] = (byte)(FAKE_VER_MAJOR);
	fake[1] = (byte)(FAKE_VER_MINOR);
	fake[2] = (byte)(FAKE_VER_PATCH);
	fake[3] = (byte)(VERSION_EXTRA);


	/* Dump the data */
	err = fd_write(data_fd, (char*)&fake, 4);


	/* Make array XXX XXX XXX */
	C_MAKE(data_head, 65535, byte);

	/* Hack -- reset */
	data_next = data_head;

	/* Dump the "options" */
	put_options();

	/* Set the type */
	data_type = TYPE_OPTIONS;

	/* Set the "options" size */
	data_size = (data_next - data_head);

	/* Write the block */
	wr_block();

	/* XXX XXX XXX */

	/* Dump the "final" marker XXX XXX XXX */
	/* Type zero, Size zero, Contents zero, etc */


	/* XXX XXX XXX Check for errors */


	/* Kill array XXX XXX XXX */
	C_KILL(data_head, 65535, byte);


	/* Success */
	return (0);
}





/*
 * Hack -- read the next "block" from the savefile
 */
static errr rd_block(void)
{
	errr err;

	byte fake[4];

	/* Read the head data */
	err = fd_read(data_fd, (char*)&fake, 4);

	/* Extract the type and size */
	data_type = (fake[0] | ((u16b)fake[1] << 8));
	data_size = (fake[2] | ((u16b)fake[3] << 8));

	/* Wipe the data block */
	C_WIPE(data_head, 65535, byte);

	/* Read the actual data */
	err = fd_read(data_fd, (char*)data_head, data_size);

	/* Read the tail data */
	err = fd_read(data_fd, (char*)&fake, 4);

	/* XXX XXX XXX Verify */

	/* Hack -- reset */
	data_next = data_head;

	/* Success */
	return (0);
}


/*
 * Hack -- get data from the current block
 */
static void get_byte(byte *ip)
{
	byte d1;
	d1 = (*data_next++);
	(*ip) = (d1);
}

/*
 * Hack -- get data from the current block
 */
static void get_char(char *ip)
{
	get_byte((byte*)ip);
}

/*
 * Hack -- get data from the current block
 */
static void get_u16b(u16b *ip)
{
	u16b d0, d1;
	d0 = (*data_next++);
	d1 = (*data_next++);
	(*ip) = (d0 | (d1 << 8));
}

/*
 * Hack -- get data from the current block
 */
static void get_s16b(s16b *ip)
{
	get_u16b((u16b*)ip);
}

/*
 * Hack -- get data from the current block
 */
static void get_u32b(u32b *ip)
{
	u32b d0, d1, d2, d3;
	d0 = (*data_next++);
	d1 = (*data_next++);
	d2 = (*data_next++);
	d3 = (*data_next++);
	(*ip) = (d0 | (d1 << 8) | (d2 << 16) | (d3 << 24));
}

/*
 * Hack -- get data from the current block
 */
static void get_s32b(s32b *ip)
{
	get_u32b((u32b*)ip);
}



/*
 * Read a savefile for Angband 2.8.0
 */
static errr rd_savefile(void)
{
	bool    done = FALSE;

	byte    fake[4];


	/* Open the savefile */
	data_fd = fd_open(savefile, O_RDONLY);

	/* No file */
	if (data_fd < 0) return (1);

	/* Strip the first four bytes (see below) */
	if (fd_read(data_fd, (char*)(fake), 4)) return (1);


	/* Make array XXX XXX XXX */
	C_MAKE(data_head, 65535, byte);

	/* Hack -- reset */
	data_next = data_head;


	/* Read blocks */
	while (!done)
	{
		/* Read the block */
		if (rd_block()) break;

		/* Analyze the type */
		switch (data_type)
		{
			/* Done XXX XXX XXX */
			case 0:
			{
				done = TRUE;
				break;
			}

			/* Grab the options */
			case TYPE_OPTIONS:
			{
				if (get_options()) err = -1;
				break;
			}
		}

		/* XXX XXX XXX verify "data_next" */
		if (data_next - data_head > data_size) break;
	}


	/* XXX XXX XXX Check for errors */


	/* Kill array XXX XXX XXX */
	C_KILL(data_head, 65535, byte);


	/* Success */
	return (0);
}


#endif /* FUTURE_SAVEFILES */




/*
 * Some "local" parameters, used to help write savefiles
 */

static FILE     *fff;           /* Current save "file" */

static byte     xor_byte;       /* Simple encryption */

static u32b     v_stamp = 0L;   /* A simple "checksum" on the actual values */
static u32b     x_stamp = 0L;   /* A simple "checksum" on the encoded bytes */



/*
 * These functions place information into a savefile a byte at a time
 */

static void sf_put(byte v)
{
	/* Encode the value, write a character */
	xor_byte ^= v;
	(void)putc((int)xor_byte, fff);

	/* Maintain the checksum info */
	v_stamp += v;
	x_stamp += xor_byte;
}

static void wr_byte(byte v)
{
	sf_put(v);
}

static void wr_u16b(u16b v)
{
	sf_put((byte)(v & 0xFF));
	sf_put((byte)((v >> 8) & 0xFF));
}

static void wr_s16b(s16b v)
{
	wr_u16b((u16b)v);
}

static void wr_u32b(u32b v)
{
	sf_put((byte)(v & 0xFF));
	sf_put((byte)((v >> 8) & 0xFF));
	sf_put((byte)((v >> 16) & 0xFF));
	sf_put((byte)((v >> 24) & 0xFF));
}

static void wr_s32b(s32b v)
{
	wr_u32b((u32b)v);
}

static void wr_string(cptr str)
{
	while (*str)
	{
		wr_byte(*str);
		str++;
	}
	wr_byte(*str);
}


/*
 * These functions write info in larger logical records
 */


/*
 * Write an "item" record
 */
static void wr_item(const object_type *o_ptr)
{
	wr_s16b(o_ptr->k_idx);

	/* Location */
	wr_s16b(o_ptr->iy);
	wr_s16b(o_ptr->ix);

	wr_byte(o_ptr->tval);
	wr_byte(o_ptr->sval);
	wr_s16b(o_ptr->pval);
   wr_s16b(o_ptr->pval2);

	wr_byte(o_ptr->discount);
	wr_byte(o_ptr->number);
	wr_s16b(o_ptr->weight);

	wr_s16b(o_ptr->timeout);

	wr_s16b(o_ptr->to_h);
	wr_s16b(o_ptr->to_d);
	wr_s16b(o_ptr->to_a);
	wr_s16b(o_ptr->ac);
	wr_byte(o_ptr->dd);
	wr_byte(o_ptr->ds);

	wr_byte(o_ptr->ident);

	wr_byte(o_ptr->marked);

   wr_u16b(o_ptr->ego_num);

	wr_u32b(o_ptr->flags1);
	wr_u32b(o_ptr->flags2);
	wr_u32b(o_ptr->flags3);
	wr_u32b(o_ptr->flags4);
	wr_u32b(o_ptr->flags5);
	wr_u32b(o_ptr->flags6);

	/* Held by monster index */
	wr_s16b(o_ptr->held_m_idx);

	/* Feelings */
	wr_byte(o_ptr->feeling);

	/* Save the inscription (if any) */
	if (o_ptr->inscription)
	{
		wr_string(quark_str(o_ptr->inscription));
	}
	else
	{
		wr_string("");
	}

	/* If it is a named item, save the name */
	if (o_ptr->xtra_name)
	{
		wr_string(quark_str(o_ptr->xtra_name));
	}
	else
	{
		wr_string("");
	}

	/* No Python object */
	wr_s32b(0);

	/* The new flags */
	wr_s32b(o_ptr->cost);

	wr_u16b(o_ptr->activate);

	wr_u32b(o_ptr->kn_flags1);
	wr_u32b(o_ptr->kn_flags2);
	wr_u32b(o_ptr->kn_flags3);
	wr_u32b(o_ptr->kn_flags4);
	wr_u32b(o_ptr->kn_flags5);
	wr_u32b(o_ptr->kn_flags6);

   wr_byte(o_ptr->OCraftLevel);
   wr_byte(o_ptr->CCraftLevel);
   wr_u32b(o_ptr->O_Durability);
   wr_u32b(o_ptr->C_Durability);

   wr_byte(o_ptr->aware);
}


/*
 * Write a "monster" record
 */
static void wr_monster(const monster_type *m_ptr)
{
	wr_s16b(m_ptr->r_idx);
	wr_s16b(m_ptr->fy);
	wr_s16b(m_ptr->fx);
	wr_s16b(m_ptr->hp);
	wr_s16b(m_ptr->maxhp);
	wr_s16b(m_ptr->csleep);
	wr_byte(m_ptr->mspeed);
	wr_byte(m_ptr->energy);
	wr_byte(m_ptr->stunned);
	wr_byte(m_ptr->confused);
	wr_byte(m_ptr->monfear);
	wr_byte(m_ptr->invulner);
	wr_u32b(m_ptr->smart);
	wr_byte(0);
}

/*
 * Write a "field" record
 */
static void wr_field(const field_type *f_ptr)
{
	int i;

	wr_s16b(f_ptr->t_idx);

	/* Location */
	wr_s16b(f_ptr->fy);
	wr_s16b(f_ptr->fx);

	/* Info flags */
	wr_u16b(f_ptr->info);

	/* Counter */
	wr_s16b(f_ptr->counter);

	/* Data */
	for (i = 0; i < 8; i++)
	{
		wr_byte(f_ptr->data[i]);
	}

	/* No Python object */
	wr_s32b(0);
}




/*
 * Write a "lore" record
 */
static void wr_lore(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Count sights/deaths/kills */
	wr_s16b(r_ptr->r_sights);
	wr_s16b(r_ptr->r_deaths);
	wr_s16b(r_ptr->r_pkills);
	wr_s16b(r_ptr->r_tkills);

	/* Count wakes and ignores */
	wr_byte(r_ptr->r_wake);
	wr_byte(r_ptr->r_ignore);

	/* Extra stuff */
	wr_byte(r_ptr->r_xtra1);
	wr_byte(r_ptr->r_xtra2);

	/* Count drops */
	wr_byte(r_ptr->r_drop_gold);
	wr_byte(r_ptr->r_drop_item);

	/* Count spells */
	wr_byte(r_ptr->r_cast_inate);
	wr_byte(r_ptr->r_cast_spell);

	/* Count blows of each type */
	wr_byte(r_ptr->r_blows[0]);
	wr_byte(r_ptr->r_blows[1]);
	wr_byte(r_ptr->r_blows[2]);
	wr_byte(r_ptr->r_blows[3]);

	/* Memorize flags */
	wr_u32b(r_ptr->r_flags1);
	wr_u32b(r_ptr->r_flags2);
	wr_u32b(r_ptr->r_flags3);
	wr_u32b(r_ptr->r_flags4);
	wr_u32b(r_ptr->r_flags5);
	wr_u32b(r_ptr->r_flags6);


	/* Monster limit per level */
	wr_byte(r_ptr->max_num);

	/* Later (?) */
	wr_byte(0);
	wr_byte(0);
	wr_byte(0);
}


/*
 * Write an "xtra" record
 */
static void wr_xtra(int k_idx)
{
	byte tmp8u = 0;

	object_kind *k_ptr = &k_info[k_idx];

	if (k_ptr->aware) tmp8u |= 0x01;
	if (k_ptr->tried) tmp8u |= 0x02;

	wr_byte(tmp8u);
}


/*
 * Write a "store" record
 */
static void wr_store(const store_type *st_ptr)
{
	int j;

	/* Save the "open" counter */
	wr_u32b(st_ptr->store_open);

	/* Save the "insults" */
	wr_s16b(st_ptr->insult_cur);

	/* Save the current owner */
	wr_byte(st_ptr->owner);

	/* Save the stock size */
	wr_byte(st_ptr->stock_num);

	/* Save the "haggle" info */
	wr_s16b(st_ptr->good_buy);
	wr_s16b(st_ptr->bad_buy);

	/* Position in the town */
	wr_u16b(st_ptr->x);
	wr_u16b(st_ptr->y);

	/* Type of store */
	wr_byte(st_ptr->type);

	wr_s32b(st_ptr->last_visit);

	/* Save the stock */
	for (j = 0; j < st_ptr->stock_num; j++)
	{
		/* Save each item in stock */
		wr_item(&st_ptr->stock[j]);
	}
}


/*
 * Write RNG state
 */
static void wr_randomizer(void)
{
	int i;

	/* Zero */
	wr_u16b(0);

	/* Place */
	wr_u16b(Rand_place);

	/* State */
	for (i = 0; i < RAND_DEG; i++)
	{
		wr_u32b(Rand_state[i]);
	}
}


/*
 * Write the "options"
 */
static void wr_options(void)
{
	int i, n;

	u16b c;
	u32b flag = 0;

	/*** Oops ***/

	/* Oops */
	for (i = 0; i < 4; i++) wr_u32b(0L);


	/*** Special Options ***/

	/* Write "delay_factor" */
	wr_byte(delay_factor);

	/* Write "hitpoint_warn" */
	wr_byte(hitpoint_warn);


	/*** Cheating options ***/

	c = 0;

	if (p_ptr->wizard) c |= 0x0002;

	if (cheat_peek) c |= 0x0100;
	if (cheat_hear) c |= 0x0200;
	if (cheat_room) c |= 0x0400;
	if (cheat_xtra) c |= 0x0800;
	if (cheat_know) c |= 0x1000;
	if (cheat_live) c |= 0x2000;

	wr_u16b(c);

	/* Autosave info */
	wr_byte(autosave_l);
	wr_byte(autosave_t);
	wr_s16b(autosave_freq);

	/*** Normal options ***/

	/* Analyze the options */
	for (n = 0; n < 8; n++)
	{
		/* Analyze the options */
		for (i = 0; i < 32; i++)
		{
			if (option_info[n * 32 + i].o_val)
			{
				/* Set flag */
				flag |= (1L << i);
			}
			else
			{
				/* Clear flag */
				flag &= ~(1L << i);
			}
		}

		/* Dump the flag */
		wr_u32b(flag);
	}

	/* Oops */
	for (i = 0; i < 8; i++) wr_u32b(option_mask[i]);


	/*** Window options ***/

	/* Dump the flags */
	for (i = 0; i < 8; i++) wr_u32b(window_flag[i]);

	/* Dump the masks */
	for (i = 0; i < 8; i++) wr_u32b(window_mask[i]);
}


/*
 * Hack -- Write the "ghost" info
 */
static void wr_ghost(void)
{
	int i;

	/* Name */
	wr_string("Broken Ghost");

	/* Hack -- stupid data */
	for (i = 0; i < 60; i++) wr_byte(0);
}


/*
 * Write some "extra" info
 */
static void wr_extra(void)
{
	int i;

	wr_string(player_name);

	wr_string(p_ptr->died_from);

	for (i = 0; i < 4; i++)
	{
		wr_string(p_ptr->history[i]);
	}

	/* Race/Class/Gender/Spells */
	wr_byte(p_ptr->prace);
	wr_byte(p_ptr->pclass);
	wr_byte(p_ptr->psex);
	wr_byte(p_ptr->realm1);
	wr_byte(p_ptr->realm2);
	wr_byte(0);	/* oops */

	wr_byte(p_ptr->hitdie);
	wr_u16b(p_ptr->expfact);

	wr_s16b(p_ptr->age);
	wr_s16b(p_ptr->ht);
	wr_s16b(p_ptr->wt);

	/* Dump the stats (maximum and current) */
	for (i = 0; i < A_MAX; ++i) wr_s16b(p_ptr->stat_max[i]);
	for (i = 0; i < A_MAX; ++i) wr_s16b(p_ptr->stat_cur[i]);

	/* Ignore the transient stats */
	for (i = 0; i < 12; ++i) wr_s16b(0);

	wr_u32b(p_ptr->au);

	wr_u32b(p_ptr->max_exp);
	wr_u32b(p_ptr->exp);
	wr_u16b(p_ptr->exp_frac);
	wr_s16b(p_ptr->lev);

	wr_s16b(p_ptr->town_num); /* -KMW- */

	/* Write arena and rewards information -KMW- */
	wr_s16b(0);
	wr_s16b(0);
	wr_s16b(p_ptr->inside_quest);
	wr_byte(0);
	wr_byte(0);

	/* oldpy and px are not required any more. */
	wr_s16b(0);
	wr_s16b(0);

	/* Save building rewards - not used. */
	wr_s16b(0);

	wr_s16b(p_ptr->mhp);
	wr_s16b(p_ptr->chp);
	wr_u16b(p_ptr->chp_frac);

	wr_s16b(p_ptr->msp);
	wr_s16b(p_ptr->csp);
	wr_u16b(p_ptr->csp_frac);

	/* Max Player and Dungeon Levels */
	wr_s16b(p_ptr->max_lev);
	wr_s16b(p_ptr->max_depth);

	/* More info */
	wr_s16b(0);     /* oops */
	wr_s16b(0);     /* oops */
	wr_s16b(0);     /* oops */
	wr_s16b(0);     /* oops */
	wr_s16b(p_ptr->sc);
	wr_s16b(0);     /* oops */

	wr_s16b(0);             /* old "rest" */
	wr_s16b(p_ptr->blind);
	wr_s16b(p_ptr->paralyzed);
	wr_s16b(p_ptr->confused);
	wr_s16b(p_ptr->food);
	wr_s16b(0);     /* old "food_digested" */
	wr_s16b(0);     /* old "protection" */
	wr_s16b(p_ptr->energy);
	wr_s16b(p_ptr->fast);
	wr_s16b(p_ptr->slow);
	wr_s16b(p_ptr->afraid);
	wr_s16b(p_ptr->cut);
	wr_s16b(p_ptr->stun);
	wr_s16b(p_ptr->poisoned);
	wr_s16b(p_ptr->image);
	wr_s16b(p_ptr->protevil);
	wr_s16b(p_ptr->invuln);
	wr_s16b(p_ptr->hero);
	wr_s16b(p_ptr->shero);
	wr_s16b(p_ptr->shield);
	wr_s16b(p_ptr->blessed);
	wr_s16b(p_ptr->tim_invis);
	wr_s16b(p_ptr->word_recall);
	wr_s16b(p_ptr->see_infra);
	wr_s16b(p_ptr->tim_infra);
	wr_s16b(p_ptr->oppose_fire);
	wr_s16b(p_ptr->oppose_cold);
	wr_s16b(p_ptr->oppose_acid);
	wr_s16b(p_ptr->oppose_elec);
	wr_s16b(p_ptr->oppose_pois);
	wr_s16b(p_ptr->tim_esp);
	wr_s16b(p_ptr->wraith_form);
	wr_s16b(p_ptr->resist_magic);
	wr_s16b(p_ptr->tim_xtra1);
	wr_s16b(p_ptr->tim_xtra2);
	wr_s16b(p_ptr->tim_xtra3);
	wr_s16b(p_ptr->tim_xtra4);
	wr_s16b(p_ptr->tim_xtra5);
	wr_s16b(p_ptr->tim_xtra6);
	wr_s16b(p_ptr->tim_xtra7);
	wr_s16b(p_ptr->tim_xtra8);

	wr_s16b(p_ptr->chaos_patron);
	wr_u32b(p_ptr->muta1);
	wr_u32b(p_ptr->muta2);
	wr_u32b(p_ptr->muta3);

	for (i = 0; i < MAX_PLAYER_VIRTUES; i++)
	{
		wr_s16b(p_ptr->virtues[i]);
	}

	for (i = 0; i < MAX_PLAYER_VIRTUES; i++)
	{
		wr_s16b(p_ptr->vir_types[i]);
	}

	wr_byte(p_ptr->confusing);
	wr_byte(0);     /* oops */
	wr_byte(0);     /* oops */
	wr_byte(0);     /* oops */
	wr_byte(p_ptr->searching);
	wr_byte(maximize_mode);
	wr_byte(preserve_mode);
	wr_byte(0);

	/* Future use */
/*	for (i = 0; i < 12; i++) wr_u32b(0L);*/
	for (i = 0; i < 8; i++) wr_u32b(0L);
   /* Used Now */
   wr_u16b(p_ptr->BaseSeed);
   wr_u16b(p_ptr->TypeSeed);
   wr_u16b(p_ptr->UniqueSeed);
   wr_u16b(0);
   wr_u16b(p_ptr->ArtLites);
   wr_u16b(p_ptr->ArtWeapons);
   wr_u16b(p_ptr->ArtArmor);
   wr_u16b(p_ptr->ArtJewelry);


	/* Ignore some flags */
	wr_u32b(0L);    /* oops */
	wr_u32b(0L);    /* oops */
	wr_u32b(0L);    /* oops */


	/* Write the "object seeds" */
	wr_u32b(seed_flavor);


	/* Special stuff */
	wr_u16b(p_ptr->panic_save);
	wr_u16b(p_ptr->total_winner);
	wr_u16b(p_ptr->noscore);


	/* Write death */
	wr_byte(p_ptr->is_dead);

	/* Write feeling */
	wr_byte(feeling);

	/* Turn of last "feeling" */
	wr_s32b(old_turn);

	/* Current turn */
	wr_s32b(turn);

	/* Trap detection status */
	wr_byte(p_ptr->detected);

	/* Coords of last trap detection spell */
	wr_s16b(p_ptr->detecty);
	wr_s16b(p_ptr->detectx);
}

/*
 * Save the dungeon or wilderness
 */

static void save_map(int ymax, int ymin, int xmax, int xmin)
{
	int y, x;

	byte tmp8u;

	byte count;
	byte prev_char;

	cave_type *c_ptr;

	/*** Simple "Run-Length-Encoding" of cave ***/

	/* Note that this will induce two wasted bytes */
	count = 0;
	prev_char = 0;

	/* Dump the cave */
	for (y = ymin; y < ymax; y++)
	{
		for (x = xmin; x < xmax; x++)
		{
			/* Get the cave */
			c_ptr = area(y, x);

			/* Extract a byte */
			tmp8u = c_ptr->info;

			/* If the run is broken, or too full, flush it 	*/
			if ((tmp8u != prev_char) || (count == MAX_UCHAR))
			{
				wr_byte((byte)count);
				wr_byte((byte)prev_char);
				prev_char = tmp8u;
				count = 1;
			}

			/* Continue the run */
			else
			{
				count++;
			}
		}
	}

	/* Flush the data (if any) */
	if (count)
	{
		wr_byte((byte)count);
		wr_byte((byte)prev_char);
	}


	/*** Simple "Run-Length-Encoding" of cave ***/

	/* Note that this will induce two wasted bytes */
	count = 0;
	prev_char = 0;

	/* Dump the cave */
	for (y = ymin; y < ymax; y++)
	{
		for (x = xmin; x < xmax; x++)
		{
			/* Get the cave */
			c_ptr = area(y,x);

			/* Extract a byte */
			tmp8u = c_ptr->feat;

			/* If the run is broken, or too full, flush it */
			if ((tmp8u != prev_char) || (count == MAX_UCHAR))
			{
				wr_byte((byte)count);
				wr_byte((byte)prev_char);
				prev_char = tmp8u;
				count = 1;
			}

			/* Continue the run */
			else
			{
				count++;
			}
		}
	}

	/* Flush the data (if any) */
	if (count)
	{
		wr_byte((byte)count);
		wr_byte((byte)prev_char);
	}


	/*** Simple "Run-Length-Encoding" of cave ***/

	/* Note that this will induce two wasted bytes */
	count = 0;
	prev_char = 0;

	/* Dump the cave */
	for (y = ymin; y < ymax; y++)
	{
		for (x = xmin; x < xmax; x++)
		{
			/* Get the cave */
			c_ptr = area(y,x);

			/* Extract a byte - the "mimic" feat variable has been removed. */
			tmp8u = 0;

			/* If the run is broken, or too full, flush it */
			if ((tmp8u != prev_char) || (count == MAX_UCHAR))
			{
				wr_byte((byte)count);
				wr_byte((byte)prev_char);
				prev_char = tmp8u;
				count = 1;
			}

			/* Continue the run */
			else
			{
				count++;
			}
		}
	}

	/* Flush the data (if any) */
	if (count)
	{
		wr_byte((byte)count);
		wr_byte((byte)prev_char);
	}

}

/*
 * Save wilderness data
 */
static void save_wild_data(void)
{
	int i, j;

	/* Save bounds */
	wr_u16b(wild_grid.y_max);
	wr_u16b(wild_grid.x_max);
	wr_u16b(wild_grid.y_min);
	wr_u16b(wild_grid.x_min);
	wr_byte(wild_grid.y);
	wr_byte(wild_grid.x);

	/* Save cache status */
	wr_byte(wild_grid.cache_count);

	/* Save wilderness seed */
	wr_u32b(wild_grid.wild_seed);

	/* Save wilderness map */
	for (i = 0; i < max_wild; i++)
	{
		for (j = 0; j < max_wild; j++)
		{
			/* Terrain */
			wr_u16b(wild[j][i].done.wild);

			/* Town / Dungeon / Specials */
			wr_byte(wild[j][i].done.town);

			/* Info flag */
			wr_byte(wild[j][i].done.info);

			/* Monster Gen type */
			wr_byte(wild[j][i].done.mon_gen);

			/* Monster Probability */
			wr_byte(wild[j][i].done.mon_prob);
		}
	}
}

/*
 * Write the current dungeon
 */
static void wr_dungeon(void)
{
	int i;

	int cur_wid = max_wid;
	int cur_hgt = max_hgt;

	/*** Basic info ***/

	/* Dungeon specific info follows */
	wr_u16b(p_ptr->depth);
	wr_u16b(base_level);
	wr_u16b(num_repro);
	wr_u16b(p_ptr->py);
	wr_u16b(p_ptr->px);
	wr_u16b(max_hgt);
	wr_u16b(max_wid);
	wr_u16b(max_panel_rows);
	wr_u16b(max_panel_cols);

	/* Save wilderness data */
	save_wild_data();

	if (p_ptr->depth)
	{
		/* Save dungeon map */
		save_map(max_hgt, min_hgt, max_wid, min_wid);

		/* Hack - the player is not in this dungeon */
		character_dungeon = FALSE;

		/* Save wilderness map */
		change_level(0);

		save_map(wild_grid.y_max, wild_grid.y_min,
		         wild_grid.x_max, wild_grid.x_min);

		change_level(p_ptr->depth);

		/* The character is back in the dungeon */
		character_dungeon = TRUE;

		/* Restore bounds */
		max_hgt = cur_hgt;
		max_wid = cur_wid;
	}
	else
	{
		save_map(wild_grid.y_max, wild_grid.y_min,
		         wild_grid.x_max, wild_grid.x_min);
	}


	/* Compact the objects */
	compact_objects(0);

	/* Compact the monsters */
	compact_monsters(0);

	/* Compact the fields */
	compact_fields(0);

	/*** Dump objects ***/

	/* Total objects */
	wr_u16b(o_max);

	/* Dump the objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Dump it */
		wr_item(o_ptr);
	}


	/*** Dump the monsters ***/


	/* Total monsters */
	wr_u16b(m_max);

	/* Dump the monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Dump it */
		wr_monster(m_ptr);
	}

	/*** Dump the fields ***/

	/* Total fields */
	wr_u16b(fld_max);

	/* Dump the fields */
	for (i = 1; i < fld_max; i++)
	{
		field_type *f_ptr = &fld_list[i];

		/* Dump it */
		wr_field(f_ptr);
	}
}



/*
 * Actually write a save-file
 */
static bool wr_savefile_new(void)
{
	int        i, j;

	u32b              now;

	byte            tmp8u;
	u16b            tmp16u;


	/* Guess at the current time */
	now = time((time_t *)0);


	/* Note the operating system */
	sf_xtra = 0L;

	/* Note when the file was saved */
	sf_when = now;

	/* Note the number of saves */
	sf_saves++;


	/*** Actually write the file ***/

	/* Dump the file header */
	xor_byte = 0;
	wr_byte(FAKE_VER_MAJOR);
	xor_byte = 0;
	wr_byte(FAKE_VER_MINOR);
	xor_byte = 0;
	wr_byte(FAKE_VER_PATCH);
	xor_byte = 0;

	tmp8u = (byte)randint0(256);
	wr_byte(tmp8u);


	/* Reset the checksum */
	v_stamp = 0L;
	x_stamp = 0L;

	/* Write the savefile version */
	wr_u32b(SAVEFILE_VERSION);

	/* Operating system */
	wr_u32b(sf_xtra);


	/* Time file last saved */
	wr_u32b(sf_when);

	/* Number of past lives */
	wr_u16b(sf_lives);

	/* Number of times saved */
	wr_u16b(sf_saves);


	/* Space */
	wr_u32b(0L);
	wr_u32b(0L);


	/* Write the RNG state */
	wr_randomizer();


	/* Write the boolean "options" */
	wr_options();


	/* Dump the number of "messages" */
	tmp16u = message_num();
	if (compress_savefile && (tmp16u > 40)) tmp16u = 40;
	wr_u16b(tmp16u);

	/* Dump the messages and colors (oldest first!) */
	for (i = tmp16u - 1; i >= 0; i--)
	{
		wr_string(message_str(i));
		wr_byte(message_color(i));
	}


	/* Dump the monster lore */
	tmp16u = max_r_idx;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++) wr_lore(i);


	/* Dump the object memory */
	tmp16u = max_k_idx;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++) wr_xtra(i);

	/* Dump the towns */
	tmp16u = max_towns;
	wr_u16b(tmp16u);

	/* Dump the quests */
	tmp16u = max_quests;
	wr_u16b(tmp16u);

	for (i = 0; i < max_quests; i++)
	{
		/* Save status for every quest */
		wr_s16b(quest[i].status);

		/* And the dungeon level too */
		/* (prevents problems with multi-level quests) */
		wr_s16b(quest[i].level);

		/* Save quest status if quest is running */
		if (quest[i].status == QUEST_STATUS_TAKEN)
		{
			wr_s16b(quest[i].cur_num);
			wr_s16b(quest[i].max_num);
			wr_s16b(quest[i].type);
			wr_s16b(quest[i].r_idx);
			wr_s16b(quest[i].k_idx);
			wr_byte(quest[i].flags);
		}
	}

	/* Dump the position in the wilderness */
	wr_s32b(p_ptr->wilderness_x);
	wr_s32b(p_ptr->wilderness_y);

	wr_s32b((s32b) max_wild);
	wr_s32b((s32b) max_wild);

	/* Write the "extra" information */
	wr_extra();

	/* Dump the "player hp" entries */
	tmp16u = PY_MAX_LEVEL;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++)
	{
		wr_s16b(p_ptr->player_hp[i]);
	}


	/* Write spell data */
	wr_u32b(p_ptr->spell_learned1);
	wr_u32b(p_ptr->spell_learned2);
	wr_u32b(p_ptr->spell_worked1);
	wr_u32b(p_ptr->spell_worked2);
	wr_u32b(p_ptr->spell_forgotten1);
	wr_u32b(p_ptr->spell_forgotten2);

	/* Dump the ordered spells */
	for (i = 0; i < 64; i++)
	{
		wr_byte(p_ptr->spell_order[i]);
	}


	/* Write the inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Dump index */
		wr_u16b((u16b)i);

		/* Dump object */
		wr_item(o_ptr);
	}

	/* Add a sentinel */
	wr_u16b(0xFFFF);

	/* Note the towns */
	tmp16u = town_count;
	wr_u16b(tmp16u);

	/* Dump the town data */
	for (i = 1; i < town_count; i++)
	{
		/* RNG seed */
		wr_u32b(town[i].seed);

		/* Number of stores */
		wr_byte(town[i].numstores);

		/* Type */
		wr_u16b(town[i].type);
		wr_byte(town[i].pop);

		/* Gates */
		wr_byte(town[i].gates_x[0]);
		wr_byte(town[i].gates_x[1]);
		wr_byte(town[i].gates_x[2]);
		wr_byte(town[i].gates_x[3]);

		wr_byte(town[i].gates_y[0]);
		wr_byte(town[i].gates_y[1]);
		wr_byte(town[i].gates_y[2]);
		wr_byte(town[i].gates_y[3]);

		/* Location */
		wr_byte(town[i].x);
		wr_byte(town[i].y);

		/* Name */
		wr_string(town[i].name);

		/* Dump the stores of all towns */
		for (j = 0; j < town[i].numstores; j++)
		{
			wr_store(&town[i].store[j]);
		}
	}

	/* Write the pet command settings */
	wr_s16b(p_ptr->pet_follow_distance);
	wr_byte(p_ptr->pet_open_doors);
	wr_byte(p_ptr->pet_pickup_items);

	/* Player is not dead, write the dungeon */
	if (!p_ptr->is_dead)
	{
		/* Dump the dungeon */
		wr_dungeon();

		/* Dump the ghost */
		wr_ghost();

#ifdef USE_SCRIPT
		{
			cptr callbacks = save_game_callback();
			if (callbacks && *callbacks)
			{
				wr_s32b(strlen(callbacks));
				wr_string(callbacks);
				string_free(callbacks);
			}
			else
			{
				/* No scripts */
				wr_s32b(0);
			}
		}
#else /* USE_SCRIPT */

		/* No scripts */
		wr_s32b(0);

#endif /* USE_SCRIPT */

	}


	/* Write the "value check-sum" */
	wr_u32b(v_stamp);

	/* Write the "encoded checksum" */
	wr_u32b(x_stamp);


	/* Error in save */
	if (ferror(fff) || (fflush(fff) == EOF)) return FALSE;

	/* Successful save */
	return TRUE;
}


/*
 * Medium level player saver
 *
 * XXX XXX XXX Angband 2.8.0 will use "fd" instead of "fff" if possible
 */
static bool save_player_aux(char *name)
{
	bool    ok = FALSE;
	int     fd;
	int     mode = 0644;


	/* No file yet */
	fff = NULL;


	/* File type is "SAVE" */
	FILE_TYPE(FILE_TYPE_SAVE);


	/* Create the savefile */
	fd = fd_make(name, mode);

	/* File is okay */
	if (fd >= 0)
	{
		/* Close the "fd" */
		(void)fd_close(fd);

		/* Open the savefile */
		fff = my_fopen(name, "wb");

		/* Successful open */
		if (fff)
		{
			/* Write the savefile */
			if (wr_savefile_new()) ok = TRUE;

			/* Attempt to close it */
			my_fclose(fff);
		}

		/* Remove "broken" files */
		if (!ok) (void)fd_kill(name);
	}


	/* Failure */
	if (!ok) return (FALSE);

	/* Successful save */
	character_saved = TRUE;

	/* Success */
	return (TRUE);
}



/*
 * Attempt to save the player in a savefile
 */
bool save_player(void)
{
	int             result = FALSE;

	char    safe[1024];


#ifdef SET_UID

# ifdef SECURE

	/* Get "games" permissions */
	beGames();

# endif

#endif


	/* New savefile */
	strcpy(safe, savefile);
	strcat(safe, ".new");

#ifdef VM
	/* Hack -- support "flat directory" usage on VM/ESA */
	strcpy(safe, savefile);
	strcat(safe, "n");
#endif /* VM */

	/* Remove it */
	(void)fd_kill(safe);

	/* Attempt to save the player */
	if (save_player_aux(safe))
	{
		char temp[1024];

		/* Old savefile */
		strcpy(temp, savefile);
		strcat(temp, ".old");

#ifdef VM
		/* Hack -- support "flat directory" usage on VM/ESA */
		strcpy(temp, savefile);
		strcat(temp, "o");
#endif /* VM */

		/* Remove it */
		(void)fd_kill(temp);

		/* Preserve old savefile */
		(void)fd_move(savefile, temp);

		/* Activate new savefile */
		(void)fd_move(safe, savefile);

		/* Remove preserved savefile */
		(void)fd_kill(temp);

		/* Hack -- Pretend the character was loaded */
		character_loaded = TRUE;

#ifdef VERIFY_SAVEFILE

		/* Lock on savefile */
		strcpy(temp, savefile);
		strcat(temp, ".lok");

		/* Remove lock file */
		fd_kill(temp);

#endif

		/* Success */
		result = TRUE;
	}


#ifdef SET_UID

# ifdef SECURE

	/* Drop "games" permissions */
	bePlayer();

# endif

#endif


	/* Return the result */
	return (result);
}



/*
 * Attempt to Load a "savefile"
 *
 *
 * On multi-user systems, you may only "read" a savefile if you will be
 * allowed to "write" it later, this prevents painful situations in which
 * the player loads a savefile belonging to someone else, and then is not
 * allowed to save his game when he quits.
 *
 * We return "TRUE" if the savefile was usable, and we set the global
 * flag "character_loaded" if a real, living, character was loaded.
 *
 * Note that we always try to load the "current" savefile, even if
 * there is no such file, so we must check for "empty" savefile names.
 */
bool load_player(void)
{
	int             fd = -1;

	errr    err = 0;

	byte    vvv[4];

#ifdef VERIFY_TIMESTAMP
	struct stat     statbuf;
#endif

	cptr    what = "generic";


	/* Paranoia */
	turn = 0;

	/* Paranoia */
	p_ptr->is_dead = FALSE;


	/* Allow empty savefile name */
	if (!savefile[0]) return (TRUE);


#if !defined(MACINTOSH) && !defined(WINDOWS) && !defined(VM)

	/* XXX XXX XXX Fix this */

	/* Verify the existance of the savefile */
	if (access(savefile, 0) < 0)
	{
		/* Give a message */
		msg_print("Savefile does not exist.");
		msg_print(NULL);

		/* Allow this */
		return (TRUE);
	}

#endif


#ifdef VERIFY_SAVEFILE

	/* Verify savefile usage */
	if (!err)
	{
		FILE *fkk;

		char temp[1024];

		/* Extract name of lock file */
		strcpy(temp, savefile);
		strcat(temp, ".lok");

		/* Check for lock */
		fkk = my_fopen(temp, "r");

		/* Oops, lock exists */
		if (fkk)
		{
			/* Close the file */
			my_fclose(fkk);

			/* Message */
			msg_print("Savefile is currently in use.");
			msg_print(NULL);

			/* Oops */
			return (FALSE);
		}

		/* Create a lock file */
		fkk = my_fopen(temp, "w");

		/* Dump a line of info */
		fprintf(fkk, "Lock file for savefile '%s'\n", savefile);

		/* Close the lock file */
		my_fclose(fkk);
	}

#endif


	/* Okay */
	if (!err)
	{
		/* Open the savefile */
		fd = fd_open(savefile, O_RDONLY);

		/* No file */
		if (fd < 0) err = -1;

		/* Message (below) */
		if (err) what = "Cannot open savefile";
	}

	/* Process file */
	if (!err)
	{

#ifdef VERIFY_TIMESTAMP
		/* Get the timestamp */
		(void)fstat(fd, &statbuf);
#endif

		/* Read the first four bytes */
		if (fd_read(fd, (char*)(vvv), 4)) err = -1;

		/* What */
		if (err) what = "Cannot read savefile";

		/* Close the file */
		(void)fd_close(fd);
	}

	/* Process file */
	if (!err)
	{

		/* Extract version */
		z_major = vvv[0];
		z_minor = vvv[1];
		z_patch = vvv[2];
		sf_extra = vvv[3];
		sf_major = 2;
		sf_minor = 8;
		sf_patch = 1;


		/* Pre-2.1.0: Assume 2.0.6 (same as 2.0.0 - 2.0.5) */
		if ((z_major == sf_major) &&
		    (z_minor == sf_minor) &&
		    (z_patch == sf_patch))
		{
			z_major = 2;
			z_minor = 0;
			z_patch = 6;
		}

		/* Very old savefiles */
		if ((sf_major == 5) && (sf_minor == 2))
		{
			sf_major = 2;
			sf_minor = 5;
		}

		/* Extremely old savefiles */
		if (sf_major > 2)
		{
			sf_major = 1;
		}

		/* Clear screen */
		Term_clear();

		/* Parse "new" savefiles */
		if ((sf_major == 2) && (sf_minor >= 7))
		{
			/* Attempt to load */
			err = rd_savefile_new();
		}

		/* Parse "future" savefiles */
		else
		{
			/* Error XXX XXX XXX */
			err = -1;
		}

		/* Message (below) */
		if (err) what = "Cannot parse savefile";
	}

	/* Paranoia */
	if (!err)
	{
		/* Invalid turn */
		if (!turn) err = -1;

		/* Message (below) */
		if (err) what = "Broken savefile";
	}

#ifdef VERIFY_TIMESTAMP
	/* Verify timestamp */
	if (!err && !arg_wizard)
	{
		/* Hack -- Verify the timestamp */
		if (sf_when > (statbuf.st_ctime + 100) ||
		    sf_when < (statbuf.st_ctime - 100))
		{
			/* Message */
			what = "Invalid timestamp";

			/* Oops */
			err = -1;
		}
	}
#endif


	/* Okay */
	if (!err)
	{
		/* Give a conversion warning */
		if ((FAKE_VER_MAJOR != z_major) ||
		    (FAKE_VER_MINOR != z_minor) ||
		    (FAKE_VER_PATCH != z_patch))
		{
			if (z_major == 2 && z_minor == 0 && z_patch == 6)
			{
				msg_print("Converted a 2.0.* savefile.");
			}
			else
			{
				/* Message */
				msg_format("Converted a %d.%d.%d savefile.",
				    z_major, z_minor, z_patch);
			}
			msg_print(NULL);
		}

		/* Player is dead */
		if (p_ptr->is_dead)
		{
			/* Player is no longer "dead" */
			p_ptr->is_dead = FALSE;

			/* Cheat death */
			if (arg_wizard)
			{
				/* A character was loaded */
				character_loaded = TRUE;

				/* Done */
				return (TRUE);
			}

			/* Count lives */
			sf_lives++;

			/* Forget turns */
			turn = old_turn = 0;

			/* Done */
			return (TRUE);
		}

		/* A character was loaded */
		character_loaded = TRUE;

		/* Still alive */
		if (p_ptr->chp >= 0)
		{
			/* Reset cause of death */
			(void)strcpy(p_ptr->died_from, "(alive and well)");
		}

		/* Success */
		return (TRUE);
	}


#ifdef VERIFY_SAVEFILE

	/* Verify savefile usage */
	if (TRUE)
	{
		char temp[1024];

		/* Extract name of lock file */
		strcpy(temp, savefile);
		strcat(temp, ".lok");

		/* Remove lock */
		fd_kill(temp);
	}

#endif


	/* Message */
	msg_format("Error (%s) reading %d.%d.%d savefile.",
		   what, z_major, z_minor, z_patch);
	msg_print(NULL);

	/* Oops */
	return (FALSE);
}


void remove_loc(void)
{
#ifdef VERIFY_SAVEFILE
	char temp[1024];
#endif /* VERIFY_SAVEFILE */

#ifdef SET_UID
# ifdef SECURE

	/* Get "games" permissions */
	beGames();

# endif /* SECURE */
#endif /* SET_UID */

#ifdef VERIFY_SAVEFILE

	/* Lock on savefile */
	strcpy(temp, savefile);
	strcat(temp, ".lok");

	/* Remove lock file */
	fd_kill(temp);

#endif /* VERIFY_SAVEFILE */

#ifdef SET_UID
# ifdef SECURE

	/* Drop "games" permissions */
	bePlayer();

# endif /* SECURE */
#endif /* SET_UID */

}
