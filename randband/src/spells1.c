/* File: spells1.c */

/* Purpose: Spell projection */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

/*
 * Array of monsters who have died from the result of a spell effect.
 *
 * This is used to prevent the stack-smash when too many monsters
 * die in an explosion chain-reaction.  (This is used as a circular
 * queue.)
 */
static s16b mon_d_head = 0;
static s16b mon_d_tail = 0;
static s16b mon_d_m_idx[DEATH_MAX];



/* ToDo: Make this global */
/* 1/x chance of reducing stats (for elemental attacks) */
#define HURT_CHANCE 16


/*
 * Get a legal "multi-hued" color for drawing "spells"
 */
static byte mh_attr(int max)
{
	switch (randint1(max))
	{
		case  1: return (TERM_RED);
		case  2: return (TERM_GREEN);
		case  3: return (TERM_BLUE);
		case  4: return (TERM_YELLOW);
		case  5: return (TERM_ORANGE);
		case  6: return (TERM_VIOLET);
		case  7: return (TERM_L_RED);
		case  8: return (TERM_L_GREEN);
		case  9: return (TERM_L_BLUE);
		case 10: return (TERM_UMBER);
		case 11: return (TERM_L_UMBER);
		case 12: return (TERM_SLATE);
		case 13: return (TERM_WHITE);
		case 14: return (TERM_L_WHITE);
		case 15: return (TERM_L_DARK);
	}

	return (TERM_WHITE);
}


/*
 * Return a color to use for the bolt/ball spells
 */
static byte spell_color(int type)
{
	/* Check if A.B.'s new graphics should be used (rr9) */
	if (streq(ANGBAND_GRAF, "new"))
	{
		/* Analyze */
		switch (type)
		{
			case GF_MISSILE:        return (0x0F);
			case GF_ACID:           return (0x04);
			case GF_ELEC:           return (0x02);
			case GF_FIRE:           return (0x00);
			case GF_COLD:           return (0x01);
			case GF_POIS:           return (0x03);
			case GF_HOLY_FIRE:      return (0x00);
			case GF_HELL_FIRE:      return (0x00);
			case GF_MANA:           return (0x0E);
			case GF_ARROW:          return (0x0F);
			case GF_WATER:          return (0x04);
			case GF_NETHER:         return (0x07);
			case GF_CHAOS:          return (mh_attr(15));
			case GF_DISENCHANT:     return (0x05);
			case GF_NEXUS:          return (0x0C);
			case GF_CONFUSION:      return (mh_attr(4));
			case GF_SOUND:          return (0x09);
			case GF_SHARDS:         return (0x08);
			case GF_FORCE:          return (0x09);
			case GF_INERTIA:        return (0x09);
			case GF_GRAVITY:        return (0x09);
			case GF_TIME:           return (0x09);
			case GF_LITE_WEAK:      return (0x06);
			case GF_LITE:           return (0x06);
			case GF_DARK_WEAK:      return (0x07);
			case GF_DARK:           return (0x07);
			case GF_PLASMA:         return (0x0B);
			case GF_METEOR:         return (0x00);
			case GF_ICE:            return (0x01);
			case GF_ROCKET:         return (0x0F);
			case GF_DEATH_RAY:      return (0x07);
			case GF_NUKE:           return (mh_attr(2));
			case GF_DISINTEGRATE:   return (0x05);
			case GF_PSI:
			case GF_PSI_DRAIN:
			case GF_TELEKINESIS:
			case GF_DOMINATION:
						return (0x09);
		}
	}
	/* Normal tiles or ASCII */
	else if (use_color && !ironman_moria)
	{
		byte a;
		char c;

		/* Lookup the default colors for this type */
		cptr s = gf_color[type];

		/* Oops */
		if (!s) return (TERM_WHITE);

		/* Pick a random color */
		c = s[randint0(strlen(s))];

		/* Lookup this color */
		a = strchr(color_char, c) - color_char;

		/* Invalid color (note check for < 0 removed, gave a silly
		 * warning because bytes are always >= 0 -- RG) */
		if (a > 15) return (TERM_WHITE);

		/* Use this color */
		return (a);
	}

	/* Standard "color" */
	return (TERM_WHITE);
}


/*
 * Find the attr/char pair to use for a spell effect
 *
 * It is moving (or has moved) from (x,y) to (nx,ny).
 *
 * If the distance is not "one", we (may) return "*".
 */
void bolt_pict(int y, int x, int ny, int nx, int typ, byte *a, byte *c)
{
	int base;

	byte k;

	/* No motion (*) */
	if ((ny == y) && (nx == x)) base = 0x30;

	/* Vertical (|) */
	else if (nx == x) base = 0x40;

	/* Horizontal (-) */
	else if (ny == y) base = 0x50;

	/* Diagonal (/) */
	else if ((ny - y) == (x - nx)) base = 0x60;

	/* Diagonal (\) */
	else if ((ny - y) == (nx - x)) base = 0x70;

	/* Weird (*) */
	else base = 0x30;

	/* Basic spell color */
	k = spell_color(typ);

	/* Obtain attr/char */
	*a = misc_to_attr[base + k];
	*c = misc_to_char[base + k];
}


/*
 * Mega-Hack -- track "affected" monsters (see "project()" comments)
 */
static int project_m_n;
static int project_m_x;
static int project_m_y;



/*
 * We are called from "project()" to "damage" terrain features
 *
 * We are called both for "beam" effects and "ball" effects.
 *
 * The "r" parameter is the "distance from ground zero".
 *
 * Note that we determine if the player can "see" anything that happens
 * by taking into account: blindness, line-of-sight, and illumination.
 *
 * We return "TRUE" if the effect of the projection is "obvious".
 *
 * XXX XXX XXX We also "see" grids which are "memorized", probably a hack
 *
 * XXX XXX XXX Perhaps we should affect doors?
 */
static bool project_f(int who, int r, int y, int x, int dam, int typ)
{
	cave_type       *c_ptr = area(y, x);

	bool obvious = FALSE;
	bool known = player_can_see_bold(y, x);

	/* XXX XXX XXX */
	who = who ? who : 0;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	/* Analyze the type */
	switch (typ)
	{
		/* Ignore most effects */
		case GF_ACID:
		case GF_ELEC:
		case GF_FIRE:
		case GF_COLD:
		case GF_PLASMA:
		case GF_METEOR:
		case GF_ICE:
		case GF_SHARDS:
		case GF_FORCE:
		case GF_SOUND:
		case GF_MANA:
		case GF_HOLY_FIRE:
		case GF_HELL_FIRE:
		case GF_DISINTEGRATE:
		case GF_PSI:
		case GF_PSI_DRAIN:
		case GF_TELEKINESIS:
		case GF_DOMINATION:
		{
			break;
		}

		/* Destroy Doors (and traps) */
		case GF_KILL_DOOR:
		{
			/* Fields can block destruction */
			if (fields_have_flags(c_ptr->fld_idx, FIELD_INFO_PERM)) break;

			/* Destroy all open doors */
			if ((c_ptr->feat == FEAT_OPEN) || (c_ptr->feat == FEAT_BROKEN) ||
				(c_ptr->feat == FEAT_CLOSED))
			{
				/* Check line of sight */
				if (known)
				{
					/* Message */
					msg_print("There is a bright flash of light!");
					obvious = TRUE;
				}

				if (c_ptr->feat == FEAT_CLOSED)
				{
					/* Update some things */
					p_ptr->update |= (PU_VIEW | PU_FLOW | PU_MONSTERS |
						 PU_MON_LITE);
				}

				/* Now is floor */
				c_ptr->feat = FEAT_FLOOR;

				/* Forget the door */
				c_ptr->info &= ~(CAVE_MARK);

				/* Note + Lite the spot */
				note_spot(y, x);
			}

			break;
		}

		/* Destroy Traps (and Locks) */
		case GF_KILL_TRAP:
		{
			/* Fields can block destruction */
			if (fields_have_flags(c_ptr->fld_idx, FIELD_INFO_PERM)) break;

			/* Reveal secret doors */
			if (c_ptr->feat == FEAT_SECRET)
			{
				/* Pick a door */
				place_closed_door(y, x);

				/* Check line of sight */
				if (known)
				{
					obvious = TRUE;
				}
			}
			break;
		}

		case GF_JAM_DOOR: /* Jams a door (as if with a spike) */
		{
			if (c_ptr->feat == FEAT_CLOSED)
			{
				make_lockjam_door(y, x, 1, TRUE);

				/* Check line of sight */
				if (known)
				{
					/* Message */
					msg_print("The door seems stuck.");
					obvious = TRUE;
				}
			}
			break;
		}

		/* Destroy walls (and doors) */
		case GF_KILL_WALL:
		{
			/* Non-walls (etc) */
			if (cave_floor_grid(c_ptr)) break;

			/* Permanent walls */
			if (c_ptr->feat >= FEAT_PERM_EXTRA) break;

			/* Fields can block destruction */
			if (fields_have_flags(c_ptr->fld_idx, FIELD_INFO_PERM)) break;

			/* Terrain */
			if (c_ptr->feat >= FEAT_TREES)
			{
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
					msg_print("It disappears!");
					obvious = TRUE;
				}

				/* Forget the wall */
				c_ptr->info &= ~(CAVE_MARK);

				/* Destroy the wall */
				cave_set_feat(y, x, FEAT_DIRT);
			}

			/* Granite */
			if (c_ptr->feat >= FEAT_WALL_EXTRA)
			{
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
					msg_print("The wall turns into mud!");
					obvious = TRUE;
				}

				/* Forget the wall */
				c_ptr->info &= ~(CAVE_MARK);

				/* Destroy the wall */
				cave_set_feat(y, x, FEAT_FLOOR);
			}

			/* Quartz / Magma with treasure */
			else if (c_ptr->feat >= FEAT_MAGMA_H)
			{
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
					msg_print("The vein turns into mud!");
					msg_print("You have found something!");
					obvious = TRUE;
				}

				/* Forget the wall */
				c_ptr->info &= ~(CAVE_MARK);

				/* Destroy the wall */
				cave_set_feat(y, x, FEAT_FLOOR);

				/* Place some gold */
				place_gold(y, x);
			}

			/* Quartz / Magma */
			else if (c_ptr->feat >= FEAT_MAGMA)
			{
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
					msg_print("The vein turns into mud!");
					obvious = TRUE;
				}

				/* Forget the wall */
				c_ptr->info &= ~(CAVE_MARK);

				/* Destroy the wall */
				cave_set_feat(y, x, FEAT_FLOOR);
			}

			/* Rubble */
			else if (c_ptr->feat == FEAT_RUBBLE)
			{
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
					msg_print("The rubble turns into mud!");
					obvious = TRUE;
				}

				/* Forget the wall */
				c_ptr->info &= ~(CAVE_MARK);

				/* Destroy the rubble */
				cave_set_feat(y, x, FEAT_FLOOR);

				/* Hack -- place an object */
				if (randint0(100) < 10)
				{
					/* Found something */
					if (player_can_see_bold(y, x))
					{
						msg_print("There was something buried in the rubble!");
						obvious = TRUE;
					}

					/* Place gold */
					place_object(y, x, FALSE, FALSE);
				}
			}

			/* Destroy doors (and secret doors) */
			else if ((c_ptr->feat == FEAT_OPEN) || (c_ptr->feat == FEAT_SECRET))
			{
				/* Hack -- special message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
					msg_print("The door turns into mud!");
					obvious = TRUE;
				}

				/* Forget the wall */
				c_ptr->info &= ~(CAVE_MARK);

				/* Destroy the feature */
				cave_set_feat(y, x, FEAT_FLOOR);
			}

			/* Update some things */
			p_ptr->update |= (PU_VIEW | PU_FLOW | PU_MONSTERS | PU_MON_LITE);

			break;
		}

		/* Make doors */
		case GF_MAKE_DOOR:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_grid(c_ptr)) break;

			/* Not under the player */
			if ((x == p_ptr->px) && (y == p_ptr->py)) break;

			/* Create a closed door */
			cave_set_feat(y, x, FEAT_CLOSED);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			/* Update some things */
			p_ptr->update |= (PU_VIEW | PU_MONSTERS | PU_MON_LITE);

			break;
		}

		/* Make traps */
		case GF_MAKE_TRAP:
		{
			/* Require a "naked" floor grid */
			if ((c_ptr->o_idx != 0) || (c_ptr->m_idx != 0)) break;

			/* Require not a wall grid */
			if (!cave_los_grid(c_ptr)) break;

			/* Place a trap */
			place_trap(y, x);

			break;
		}

		case GF_MAKE_GLYPH:
		{
			/* Require a "naked" floor grid */
			if ((c_ptr->o_idx != 0) || (c_ptr->m_idx != 0)) break;

			/* Add the glyph here as a field */
			(void)place_field(y, x, FT_GLYPH_WARDING);

			break;
		}

		case GF_STONE_WALL:
		{
			/* Require a "naked" floor grid */
			if ((c_ptr->o_idx != 0) || (c_ptr->m_idx != 0)) break;

			/* Place a wall */
			cave_set_feat(y, x, FEAT_WALL_EXTRA);

			/* Update some things */
			p_ptr->update |= (PU_VIEW | PU_MONSTERS | PU_MON_LITE);

			break;
		}

		/* Lite up the grid */
		case GF_LITE_WEAK:
		case GF_LITE:
		{
			/* Turn on the light */
			c_ptr->info |= (CAVE_GLOW);

			/* Notice + Redraw */
			note_spot(y, x);

			/* Observe */
			if (player_can_see_bold(y, x)) obvious = TRUE;

			/* Mega-Hack -- Update the monster in the affected grid */
			/* This allows "spear of light" (etc) to work "correctly" */
			if (c_ptr->m_idx) update_mon(c_ptr->m_idx, FALSE);

			break;
		}

		/* Darken the grid */
		case GF_DARK_WEAK:
		case GF_DARK:
		{
			/* Notice */
			if (player_can_see_bold(y, x)) obvious = TRUE;

			/* Turn off the light. */
			c_ptr->info &= ~(CAVE_GLOW);

			/* Hack -- Forget "boring" grids */
			if (c_ptr->feat == FEAT_FLOOR)
			{
				/* Forget */
				c_ptr->info &= ~(CAVE_MARK);

				/* Notice + Redraw */
				note_spot(y, x);
			}
			else
			{
				/* Redraw */
				lite_spot(y, x);
			}

			/* Mega-Hack -- Update the monster in the affected grid */
			/* This allows "spear of light" (etc) to work "correctly" */
			if (c_ptr->m_idx) update_mon(c_ptr->m_idx, FALSE);

			/* All done */
			break;
		}
	}

	/* Return "Anything seen?" */
	return (obvious);
}



/*
 * We are called from "project()" to "damage" objects
 *
 * We are called both for "beam" effects and "ball" effects.
 *
 * Perhaps we should only SOMETIMES damage things on the ground.
 *
 * The "r" parameter is the "distance from ground zero".
 *
 * Note that we determine if the player can "see" anything that happens
 * by taking into account: blindness, line-of-sight, and illumination.
 *
 * XXX XXX XXX We also "see" grids which are "memorized", probably a hack
 *
 * We return "TRUE" if the effect of the projection is "obvious".
 */
static bool project_o(int who, int r, int y, int x, int dam, int typ)
{
	cave_type *c_ptr = area(y,x);

	s16b this_o_idx, next_o_idx = 0;

	bool obvious = FALSE;
	bool known = player_can_see_bold(y, x);

	u32b f1, f2, f3, f4, f5, f6;

	char o_name[80];

	int k_idx = 0;
	bool is_potion = FALSE;


	/* XXX XXX XXX */
	who = who ? who : 0;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		bool is_art = FALSE;
		bool ignore = FALSE;
		bool plural = FALSE;
		bool do_kill = FALSE;

		cptr note_kill = NULL;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Extract the flags */
		object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6);
   /*****   NEEDS   REWORKING   *****/

		/* Get the "plural"-ness */
		if (o_ptr->number > 1) plural = TRUE;

		/* Check for artifact */
		if (o_ptr->flags3 & TR3_INSTA_ART) is_art = TRUE;

		/* Analyze the type */
		switch (typ)
		{
			/* Acid -- Lots of things */
			case GF_ACID:
			{
				if (hates_acid(o_ptr))
				{
					do_kill = TRUE;
					note_kill = (plural ? " melt!" : " melts!");
					if (f3 & (TR3_IGNORE_ACID)) ignore = TRUE;
				}
				break;
			}

			/* Elec -- Rings and Wands */
			case GF_ELEC:
			{
				if (hates_elec(o_ptr))
				{
					do_kill = TRUE;
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
					if (f3 & (TR3_IGNORE_ELEC)) ignore = TRUE;
				}
				break;
			}

			/* Fire -- Flammable objects */
			case GF_FIRE:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
					note_kill = (plural ? " burn up!" : " burns up!");
					if (f3 & (TR3_IGNORE_FIRE)) ignore = TRUE;
				}
				break;
			}

			/* Cold -- potions and flasks */
			case GF_COLD:
			{
				if (hates_cold(o_ptr))
				{
					note_kill = (plural ? " shatter!" : " shatters!");
					do_kill = TRUE;
					if (f3 & (TR3_IGNORE_COLD)) ignore = TRUE;
				}
				break;
			}

			/* Fire + Elec */
			case GF_PLASMA:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
					note_kill = (plural ? " burn up!" : " burns up!");
					if (f3 & (TR3_IGNORE_FIRE)) ignore = TRUE;
				}
				if (hates_elec(o_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
					if (f3 & (TR3_IGNORE_ELEC)) ignore = TRUE;
				}
				break;
			}

			/* Fire + Cold */
			case GF_METEOR:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
					note_kill = (plural ? " burn up!" : " burns up!");
					if (f3 & (TR3_IGNORE_FIRE)) ignore = TRUE;
				}
				if (hates_cold(o_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
					note_kill = (plural ? " shatter!" : " shatters!");
					if (f3 & (TR3_IGNORE_COLD)) ignore = TRUE;
				}
				break;
			}

			/* Hack -- break potions and such */
			case GF_ICE:
			case GF_SHARDS:
			case GF_FORCE:
			case GF_SOUND:
			{
				if (hates_cold(o_ptr))
				{
					note_kill = (plural ? " shatter!" : " shatters!");
					do_kill = TRUE;
				}
				break;
			}

			/* Mana and Chaos -- destroy everything */
			case GF_MANA:
			{
				do_kill = TRUE;
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
				break;
			}

			case GF_DISINTEGRATE:
			{
				do_kill = TRUE;
				note_kill = (plural ? " evaporate!" : " evaporates!");
				break;
			}

			case GF_CHAOS:
			{
				do_kill = TRUE;
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
				if (f2 & (TR2_RES_CHAOS)) ignore = TRUE;
				break;
			}

			/* Holy Fire and Hell Fire -- destroys cursed non-artifacts */
			case GF_HOLY_FIRE:
			case GF_HELL_FIRE:
			{
				if (cursed_p(o_ptr))
				{
					do_kill = TRUE;
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
				}
				break;
			}

			/* Unlock chests */
			case GF_KILL_TRAP:
			case GF_KILL_DOOR:
			{
				/* Chests are noticed only if trapped or locked */
				if (o_ptr->tval == TV_CHEST)
				{
					/* Disarm/Unlock traps */
					if (o_ptr->pval > 0)
					{
						/* Disarm or Unlock */
						o_ptr->pval = (0 - o_ptr->pval);

						/* Identify */
						object_known(o_ptr);

						/* Notice */
						if (known && o_ptr->marked)
						{
							msg_print("Click!");
							obvious = TRUE;
						}
					}
				}

				break;
			}
		}


		/* Attempt to destroy the object */
		if (do_kill)
		{
			/* Effect "observed" */
			if (known && o_ptr->marked)
			{
				obvious = TRUE;
				object_desc(o_name, o_ptr, FALSE, 0);
			}

			/* Artifacts, and other objects, get to resist */
			if (is_art || ignore)
			{
				/* Observe the resist */
				if (known && o_ptr->marked)
				{
					msg_format("The %s %s unaffected!",
							o_name, (plural ? "are" : "is"));
				}
			}

			/* Kill it */
			else
			{
				/* Describe if needed */
				if (known && o_ptr->marked && note_kill)
				{
					msg_format("The %s%s", o_name, note_kill);
				}

				k_idx = o_ptr->k_idx;
				is_potion = object_is_potion(o_ptr);


				/* Delete the object */
				delete_object_idx(this_o_idx);

				/* Potions produce effects when 'shattered' */
				if (is_potion)
				{
					(void)potion_smash_effect(who, y, x, k_idx);
				}

				/* Redraw */
				lite_spot(y, x);
			}
		}
	}

	/* Return "Anything seen?" */
	return (obvious);
}


/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to a monster.
 *
 * This routine takes a "source monster" (by index) which is mostly used to
 * determine if the player is causing the damage, and a "radius" (see below),
 * which is used to decrease the power of explosions with distance, and a
 * location, via integers which are modified by certain types of attacks
 * (polymorph and teleport being the obvious ones), a default damage, which
 * is modified as needed based on various properties, and finally a "damage
 * type" (see below).
 *
 * Note that this routine can handle "no damage" attacks (like teleport) by
 * taking a "zero" damage, and can even take "parameters" to attacks (like
 * confuse) by accepting a "damage", using it to calculate the effect, and
 * then setting the damage to zero.  Note that the "damage" parameter is
 * divided by the radius, so monsters not at the "epicenter" will not take
 * as much damage (or whatever)...
 *
 * Note that "polymorph" is dangerous, since a failure in "place_monster()"'
 * may result in a dereference of an invalid pointer.  XXX XXX XXX
 *
 * Various messages are produced, and damage is applied.
 *
 * Just "casting" a substance (i.e. plasma) does not make you immune, you must
 * actually be "made" of that substance, or "breathe" big balls of it.
 *
 * We assume that "Plasma" monsters, and "Plasma" breathers, are immune
 * to plasma.
 *
 * We assume "Nether" is an evil, necromantic force, so it doesn't hurt undead,
 * and hurts evil less.  If can breath nether, then it resists it as well.
 *
 * Damage reductions use the following formulas:
 *   Note that "dam = dam * 6 / (randint1(6) + 6);"
 *     gives avg damage of .655, ranging from .858 to .500
 *   Note that "dam = dam * 5 / (randint1(6) + 6);"
 *     gives avg damage of .544, ranging from .714 to .417
 *   Note that "dam = dam * 4 / (randint1(6) + 6);"
 *     gives avg damage of .444, ranging from .556 to .333
 *   Note that "dam = dam * 3 / (randint1(6) + 6);"
 *     gives avg damage of .327, ranging from .427 to .250
 *   Note that "dam = dam * 2 / (randint1(6) + 6);"
 *     gives something simple.
 *
 * In this function, "result" messages are postponed until the end, where
 * the "note" string is appended to the monster name, if not NULL.  So,
 * to make a spell have "no effect" just set "note" to NULL.  You should
 * also set "notice" to FALSE, or the player will learn what the spell does.
 *
 * We attempt to return "TRUE" if the player saw anything "useful" happen.
 */
static bool project_m(int who, int r, int y, int x, int dam, int typ)
{
	int tmp;

	cave_type *c_ptr = area(y,x);

	monster_type *m_ptr = &m_list[c_ptr->m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	char killer[80];

	cptr name = (r_name + r_ptr->name);

	/* Is the monster "seen"? */
	bool seen = m_ptr->ml;

	/* Were the effects "obvious" (if seen)? */
	bool obvious = FALSE;

	/* Can the player know about this effect? */
	bool known = (m_ptr->cdis <= MAX_SIGHT);

	/* Can the player see the source of this effect? */
	bool see_s = ((who <= 0) || m_list[who].ml);

	/* Were the effects "irrelevant"? */
	bool skipped = FALSE;

	/* Gets the monster angry at the source of the effect? */
	bool get_angry = FALSE;

	/* Polymorph setting (true or false) */
	int do_poly = 0;

	/* Teleport setting (max distance) */
	int do_dist = 0;

	/* Confusion setting (amount to confuse) */
	int do_conf = 0;

	/* Stunning setting (amount to stun) */
	int do_stun = 0;

	/* Sleep amount (amount to sleep) */
	int do_sleep = 0;

	/* Fear amount (amount to fear) */
	int do_fear = 0;

	bool heal_leper = FALSE;

	/* Hold the monster name */
	char m_name[80];

	/* Assume no note */
	cptr note = NULL;

	/* Assume a default death */
	cptr note_dies = " dies.";

	/* Nobody here */
	if (!c_ptr->m_idx) return (FALSE);

	/* Never affect projector */
	if (who && (c_ptr->m_idx == who)) return (FALSE);

	/*
	 * Don't affect already dead monsters
	 * This prevents problems with chain reactions of exploding monsters
	 */
	if (m_ptr->hp < 0) return (FALSE);

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	/* Get the monster name (BEFORE polymorphing) */
	monster_desc(m_name, m_ptr, 0);


	/* Some monsters get "destroyed" */
	if (!monster_living(r_ptr))
	{
		/* Special note at death */
		note_dies = " is destroyed.";
	}

	/* Analyze the damage type */
	switch (typ)
	{
		/* Magic Missile -- pure damage */
		case GF_MISSILE:
		{
			if (seen) obvious = TRUE;
			break;
		}

		/* Acid */
		case GF_ACID:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & (RF3_IM_ACID))
			{
				note = " resists a lot.";
				dam /= 9;
				if (seen) r_ptr->r_flags3 |= (RF3_IM_ACID);
			}
			break;
		}

		/* Electricity */
		case GF_ELEC:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & (RF3_IM_ELEC))
			{
				note = " resists a lot.";
				dam /= 9;
				if (seen) r_ptr->r_flags3 |= (RF3_IM_ELEC);
			}
			break;
		}

		/* Fire damage */
		case GF_FIRE:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & (RF3_IM_FIRE))
			{
				note = " resists a lot.";
				dam /= 9;
				if (seen) r_ptr->r_flags3 |= (RF3_IM_FIRE);
			}
			break;
		}

		/* Cold */
		case GF_COLD:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & (RF3_IM_COLD))
			{
				note = " resists a lot.";
				dam /= 9;
				if (seen) r_ptr->r_flags3 |= (RF3_IM_COLD);
			}
			break;
		}

		/* Poison */
		case GF_POIS:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & RF3_IM_POIS)
			{
				note = " resists a lot.";
				dam /= 9;
				if (seen) r_ptr->r_flags3 |= (RF3_IM_POIS);
			}
			break;
		}

		/* Nuclear waste */
		case GF_NUKE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flags3 & RF3_IM_POIS)
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
				if (seen) r_ptr->r_flags3 |= (RF3_IM_POIS);
			}
			else if (one_in_(3)) do_poly = TRUE;
			break;
		}

		/* Hellfire -- hurts Evil */
		case GF_HELL_FIRE:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & RF3_EVIL)
			{
				dam *= 2;
				note = " is hit hard.";
				if (seen) r_ptr->r_flags3 |= (RF3_EVIL);
			}
			break;
		}

		/* Holy Fire -- hurts Evil, Good are immune, others _resist_ */
		case GF_HOLY_FIRE:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & RF3_GOOD)
			{
				dam = 0;
				note = " is immune.";
				if (seen) r_ptr->r_flags3 |= RF3_GOOD;
			}
			else if (r_ptr->flags3 & RF3_EVIL)
			{
				dam *= 2;
				note = " is hit hard.";
				if (seen) r_ptr->r_flags3 |= RF3_EVIL;
			}
			else
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
			}
			break;
		}

		/* Arrow -- XXX no defense */
		case GF_ARROW:
		{
			if (seen) obvious = TRUE;
			break;
		}

		/* Plasma -- XXX perhaps check ELEC or FIRE */
		case GF_PLASMA:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & RF3_RES_PLAS)
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
				if (seen)
					r_ptr->r_flags3 |= (RF3_RES_PLAS);
			}
			break;
		}

		/* Nether -- see above */
		case GF_NETHER:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & RF3_UNDEAD)
			{
				note = " is immune.";
				dam = 0;
				if (seen) r_ptr->r_flags3 |= (RF3_UNDEAD);
			}
			else if (r_ptr->flags3 & RF3_RES_NETH)
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);

				if (seen) r_ptr->r_flags3 |= (RF3_RES_NETH);
			}
			else if (r_ptr->flags3 & RF3_EVIL)
			{
				dam /= 2;
				note = " resists somewhat.";
				if (seen) r_ptr->r_flags3 |= (RF3_EVIL);
			}
			break;
		}

		/* Water (acid) damage -- Water spirits/elementals are immune */
		case GF_WATER:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->d_char == 'E') &&
			   (prefix(name, "W") ||
			   (strstr((r_name + r_ptr->name), "Unmaker"))))
			{
				note = " is immune.";
				dam = 0;
			}
			else if (r_ptr->flags3 & RF3_RES_WATE)
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
				if (seen) r_ptr->r_flags3 |= (RF3_RES_WATE);
			}
			break;
		}

		/* Chaos -- Chaos breathers resist */
		case GF_CHAOS:
		{
			if (seen) obvious = TRUE;
			do_poly = TRUE;
			do_conf = (rand_range(5, 16) + r) / (r + 1);
			if ((r_ptr->flags4 & RF4_BR_CHAO) ||
			   ((r_ptr->flags3 & RF3_DEMON) && one_in_(3)))
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
				do_poly = FALSE;
			}
			break;
		}

		/* Shards -- Shard breathers resist */
		case GF_SHARDS:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags4 & RF4_BR_SHAR)
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
			}
			break;
		}

		/* Rocket: Shard resistance helps */
		case GF_ROCKET:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flags4 & RF4_BR_SHAR)
			{
				note = " resists somewhat.";
				dam /= 2;
			}
			break;
		}


		/* Sound -- Sound breathers resist */
		case GF_SOUND:
		{
			if (seen) obvious = TRUE;
			do_stun = (rand_range(10, 25) + r) / (r + 1);
			if (r_ptr->flags4 & RF4_BR_SOUN)
			{
				note = " resists.";
				dam *= 2; dam /= rand_range(7, 12);
			}
			break;
		}

		/* Confusion */
		case GF_CONFUSION:
		{
			if (seen) obvious = TRUE;
			do_conf = (rand_range(10, 25) + r) / (r + 1);
			if (r_ptr->flags4 & RF4_BR_CONF)
			{
				note = " resists.";
				dam *= 2; dam /= rand_range(7, 12);
			}
			else if (r_ptr->flags3 & RF3_NO_CONF)
			{
				note = " resists somewhat.";
				dam /= 2;
			}
			break;
		}

		/* Disenchantment -- Breathers and Disenchanters resist */
		case GF_DISENCHANT:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & RF3_RES_DISE)
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
				if (seen) r_ptr->r_flags3 |= (RF3_RES_DISE);
			}
			break;
		}

		/* Nexus -- Breathers and Existers resist */
		case GF_NEXUS:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & RF3_RES_NEXU)
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
				if (seen) r_ptr->r_flags3 |= (RF3_RES_NEXU);
			}
			break;
		}

		/* Force */
		case GF_FORCE:
		{
			if (seen) obvious = TRUE;
			do_stun = (randint1(15) + r) / (r + 1);
			if (r_ptr->flags4 & RF4_BR_WALL)
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
			}
			break;
		}

		/* Inertia -- breathers resist */
		case GF_INERTIA:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags4 & (RF4_BR_INER))
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
			}
			else
			{
				/* Powerful monsters can resist */
				if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				    (r_ptr->level > randint1(dam * 3)))
				{
					obvious = FALSE;
				}
				/* Normal monsters slow down */
				else
				{
					if (m_ptr->mspeed > 60) m_ptr->mspeed -= 10;
					note = " starts moving slower.";
				}
			}
			break;
		}

		/* Time -- breathers resist */
		case GF_TIME:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags4 & (RF4_BR_TIME))
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
			}
			break;
		}

		/* Gravity -- breathers resist */
		case GF_GRAVITY:
		{
			bool resist_tele = FALSE;

			if (seen) obvious = TRUE;

			if (r_ptr->flags3 & (RF3_RES_TELE))
			{
				if (r_ptr->flags1 & (RF1_UNIQUE))
				{
					if (seen) r_ptr->r_flags3 |= RF3_RES_TELE;
					note = " is unaffected!";
					resist_tele = TRUE;
				}
				else if (r_ptr->level > randint1(150))
				{
					if (seen) r_ptr->r_flags3 |= RF3_RES_TELE;
					note = " resists!";
					resist_tele = TRUE;
				}
			}

			if (!resist_tele) do_dist = 10;
			else do_dist = 0;

			if (r_ptr->flags4 & (RF4_BR_GRAV))
			{
				note = " resists.";
				dam *= 3; dam /= rand_range(7, 12);
				do_dist = 0;
			}
			else
			{
				/* 1. slowness */
				/* Powerful monsters can resist */
				if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				    (r_ptr->level > randint1(dam * 3)))
				{
					obvious = FALSE;
				}
				/* Normal monsters slow down */
				else
				{
					if (m_ptr->mspeed > 60) m_ptr->mspeed -= 10;
					note = " starts moving slower.";
				}

				/* 2. stun */
				do_stun = damroll((p_ptr->lev / 10) + 3, (dam)) + 1;

				/* Attempt a saving throw */
				if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				    (r_ptr->level > randint1(dam * 3)))
				{
					/* Resist */
					do_stun = 0;
					/* No obvious effect */
					note = " is unaffected!";
					obvious = FALSE;
				}
			}
			break;
		}

		/* Pure damage */
		case GF_MANA:
		{
			if (seen) obvious = TRUE;
			break;
		}


		/* Pure damage */
		case GF_DISINTEGRATE:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags3 & RF3_HURT_ROCK)
			{
				if (seen) r_ptr->r_flags3 |= (RF3_HURT_ROCK);
				note = " loses some skin!";
				note_dies = " evaporates!";
				dam *= 2;
			}

			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				if (r_ptr->level > randint0(p_ptr->lev * 3))
				{
					note = " resists.";
					dam /= 8;
				}
			}
			break;
		}

		case GF_PSI:
		{
			if (seen) obvious = TRUE;

			/* PSI only works if the monster can see you! -- RG */
			if (!(c_ptr->info & CAVE_VIEW))
			{
				dam = 0;
				note = " can't see you, and isn't affected!";
			}

			if (r_ptr->flags2 & RF2_EMPTY_MIND)
			{
				dam = 0;
				note = " is immune!";

				/* Memorize a flag */
				if (seen) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
			}
			else if ((r_ptr->flags2 & RF2_STUPID) ||
						(r_ptr->flags2 & RF2_WEIRD_MIND) ||
						(r_ptr->flags3 & RF3_ANIMAL) ||
						(r_ptr->level > randint1(6 * dam)))
			{
				dam /= 3;
				note = " resists.";

				/*
				 * Powerful demons & undead can turn a mindcrafter's
				 * attacks back on them
				 */
				if (((r_ptr->flags3 & RF3_UNDEAD) ||
					  (r_ptr->flags3 & RF3_DEMON)) &&
					  (r_ptr->level > p_ptr->lev) && one_in_(2))
				{
					note = NULL;
					msg_format("%^s%s corrupted mind backlashes your attack!",
					    m_name, (seen ? "'s" : "s"));
					/* Saving throw */
					if (randint0(100) < p_ptr->skill_sav)
					{
						msg_print("You resist the effects!");
					}
					else
					{
						/* Injure +/- confusion */
						monster_desc(killer, m_ptr, 0x88);
						take_hit(dam, killer, FALSE);  /* has already been /3 */
						if (one_in_(4))
						{
							switch (randint1(4))
							{
								case 1:
									(void) set_confused(p_ptr->confused + 3 + randint1(dam));
									break;
								case 2:
									(void)set_stun(p_ptr->stun + randint1(dam));
									break;
								case 3:
								{
									if (r_ptr->flags3 & RF3_NO_FEAR)
										note = " is unaffected.";
									else
										(void)set_afraid(p_ptr->afraid + 3 + randint1(dam));
									break;
								}
								default:
									if (!p_ptr->free_act)
										(void)set_paralyzed(p_ptr->paralyzed + randint1(dam));
									break;
							}
						}
					}
					dam = 0;
				}
			}

			if ((dam > 0) && one_in_(4))
			{
				switch (randint1(4))
				{
					case 1:
						do_conf = 3 + randint1(dam);
						break;
					case 2:
						do_stun = 3 + randint1(dam);
						break;
					case 3:
						do_fear = 3 + randint1(dam);
						break;
					default:
						note = " falls asleep!";
						do_sleep = 3 + randint1(dam);
						break;
				}
			}

			note_dies = " collapses, a mindless husk.";
			break;
		}

		case GF_PSI_DRAIN:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags2 & RF2_EMPTY_MIND)
			{
				dam = 0;
				note = " is immune!";

				/* Memorize a flag */
				if (seen) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
			}
			else if ((r_ptr->flags2 & RF2_STUPID) ||
			         (r_ptr->flags2 & RF2_WEIRD_MIND) ||
			         (r_ptr->flags3 & RF3_ANIMAL) ||
						(r_ptr->level > randint1(6 * dam)))
			{
				dam /= 3;
				note = " resists.";

				/*
				 * Powerful demons & undead can turn a mindcrafter's
				 * attacks back on them
				 */
				if (((r_ptr->flags3 & RF3_UNDEAD) ||
				     (r_ptr->flags3 & RF3_DEMON)) &&
				     (r_ptr->level > p_ptr->lev) && one_in_(2))
				{
					note = NULL;
					msg_format("%^s%s corrupted mind backlashes your attack!",
					    m_name, (seen ? "'s" : "s"));
					/* Saving throw */
					if (randint0(100) < p_ptr->skill_sav)
					{
						msg_print("You resist the effects!");
					}
					else
					{
						/* Injure + mana drain */
						monster_desc(killer, m_ptr, 0x88);
						msg_print("Your psychic energy is drained!");
						p_ptr->csp = MAX(0, p_ptr->csp - damroll(5, dam) / 2);
						p_ptr->redraw |= PR_MANA;
						p_ptr->window |= (PW_SPELL);
						take_hit(dam, killer, FALSE);  /* has already been /3 */
					}
					dam = 0;
				}
			}
			else if (dam > 0)
			{
				int b = damroll(5, dam) / 4;
				msg_format("You convert %s%s pain into psychic energy!",
				    m_name, (seen ? "'s" : "s"));
				b = MIN(p_ptr->msp, p_ptr->csp + b);
				p_ptr->csp = b;
				p_ptr->redraw |= PR_MANA;
				p_ptr->window |= (PW_SPELL);
			}

			note_dies = " collapses, a mindless husk.";
			break;
		}

		case GF_TELEKINESIS:
		{
			if (seen) obvious = TRUE;
			do_dist = 7;
			/* 1. stun */
			do_stun = damroll((p_ptr->lev / 10) + 3, dam) + 1;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
			    (r_ptr->level > randint1(dam) * 2))
			{
				/* Resist */
				do_stun = 0;
				/* No obvious effect */
				obvious = FALSE;
			}
			break;
		}

		/* Meteor -- powerful magic missile */
		case GF_METEOR:
		{
			if (seen) obvious = TRUE;
			break;
		}

		case GF_DOMINATION:
		{
			if (!is_hostile(m_ptr)) break;
			if (seen) obvious = TRUE;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
			    (r_ptr->flags1 & RF1_QUESTOR) ||
			    (r_ptr->flags3 & RF3_NO_CONF) ||
			    (r_ptr->level > randint1(dam * 3)))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (seen) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				do_conf = 0;

				/*
				 * Powerful demons & undead can turn a mindcrafter's
				 * attacks back on them
				 */
				if (((r_ptr->flags3 & RF3_UNDEAD) ||
				     (r_ptr->flags3 & RF3_DEMON)) &&
				     (r_ptr->level > p_ptr->lev) && one_in_(2))
				{
					note = NULL;
					msg_format("%^s%s corrupted mind backlashes your attack!",
					    m_name, (seen ? "'s" : "s"));
					/* Saving throw */
					if (randint0(100) < p_ptr->skill_sav)
					{
						msg_print("You resist the effects!");
					}
					else
					{
						/* Confuse, stun, terrify */
						switch (randint1(4))
						{
							case 1:
								(void)set_stun(p_ptr->stun + dam / 2);
								break;
							case 2:
								(void)set_confused(p_ptr->confused + dam / 2);
								break;
							default:
							{
								if (r_ptr->flags3 & RF3_NO_FEAR)
									note = " is unaffected.";
								else
									(void)set_afraid(p_ptr->afraid + dam);
							}
						}
					}
				}
				else
				{
					/* No obvious effect */
					note = " is unaffected!";
					obvious = FALSE;
				}
			}
			else
			{
				if ((dam > 29) && (randint1(100) < dam))
				{
					note = " is in your thrall!";
					set_pet(m_ptr);
				}
				else
				{
					switch (randint1(4))
					{
						case 1:
							do_stun = dam / 2;
							break;
						case 2:
							do_conf = dam / 2;
							break;
						default:
							do_fear = dam;
					}
				}
			}

			/* No "real" damage */
			dam = 0;
			break;
		}



		/* Ice -- Cold + Cuts + Stun */
		case GF_ICE:
		{
			if (seen) obvious = TRUE;
			do_stun = rand_range(2, 16) / (r + 1);
			if (r_ptr->flags3 & RF3_IM_COLD)
			{
				note = " resists a lot.";
				dam /= 9;
				if (seen) r_ptr->r_flags3 |= (RF3_IM_COLD);
			}
			break;
		}


		/* Drain Life */
		case GF_OLD_DRAIN:
		{
			if (seen) obvious = TRUE;

			if (!monster_living(r_ptr))
			{
				if (r_ptr->flags3 & RF3_UNDEAD)
				{
					if (seen) r_ptr->r_flags3 |= (RF3_UNDEAD);
				}

				if (r_ptr->flags3 & (RF3_DEMON))
				{
					if (seen) r_ptr->r_flags3 |= (RF3_DEMON);
				}

				note = " is unaffected!";
				obvious = FALSE;
				dam = 0;
			}

			break;
		}

		/* Drain Life + give it to the player */
		case GF_NEW_DRAIN:
		{
			if (seen) obvious = TRUE;

			if (!monster_living(r_ptr))
			{
				if (r_ptr->flags3 & RF3_UNDEAD)
				{
					if (seen) r_ptr->r_flags3 |= (RF3_UNDEAD);
				}

				if (r_ptr->flags3 & (RF3_DEMON))
				{
					if (seen) r_ptr->r_flags3 |= (RF3_DEMON);
				}

				note = " is unaffected!";
				obvious = FALSE;
				dam = 0;
			}
			else
			{
				u16b hp = dam;

				/* Cannot drain more than monsters life */
				if (m_ptr->hp < dam) hp = m_ptr->hp;

				/* Cannot drain more than 100hp at a time */
				if (hp > 100) hp = 100;

				/* Give the player the hit points */
				(void)hp_player(hp);
			}

			break;
		}


		/* Death Ray */
		case GF_DEATH_RAY:
		{
			if (seen) obvious = TRUE;

			if ((r_ptr->flags3 & RF3_UNDEAD) ||
			    (r_ptr->flags3 & RF3_NONLIVING))
			{
				if (r_ptr->flags3 & RF3_UNDEAD)
				{
					if (seen) r_ptr->r_flags3 |= (RF3_UNDEAD);
				}

				note = " is immune.";
				obvious = FALSE;
				dam = 0;
			}
			else if (((r_ptr->flags1 & RF1_UNIQUE) && !one_in_(666)) ||
				 (r_ptr->level > randint1(dam / 30)))
			{
				note = " resists!";
				obvious = FALSE;
				dam = 0;
			}

			break;
		}

		/* Polymorph monster (Use "dam" as "power") */
		case GF_OLD_POLY:
		{
			if (seen) obvious = TRUE;

			/* Attempt to polymorph (see below) */
			do_poly = TRUE;

			/* Powerful monsters can resist */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
			    (r_ptr->flags1 & RF1_QUESTOR) ||
			    (r_ptr->level > randint1(dam * 3)))
			{
				note = " is unaffected!";
				do_poly = FALSE;
				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;

			break;
		}


		/* Clone monsters (Ignore "dam") */
		case GF_OLD_CLONE:
		{
			bool friendly = FALSE;
			bool pet = FALSE;

			if (seen) obvious = TRUE;
			if (is_friendly(m_ptr) && !one_in_(3))
				friendly = TRUE;
			if (is_pet(m_ptr) && !one_in_(3))
				pet = TRUE;

			/* Heal fully */
			m_ptr->hp = m_ptr->maxhp;

			/* Speed up */
			if (m_ptr->mspeed < 150) m_ptr->mspeed += 10;

			/* Attempt to clone. */
			if (multiply_monster(c_ptr->m_idx, TRUE, friendly, pet))
			{
				note = " spawns!";
			}

			/* No "real" damage */
			dam = 0;

			break;
		}


		/* Heal Monster (use "dam" as amount of healing) */
		case GF_OLD_HEAL:
		{
			if (seen) obvious = TRUE;

			/* Wake up */
			m_ptr->csleep = 0;

			/* Heal */
			m_ptr->hp += dam;

			/* No overflow */
			if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

			chg_virtue(V_VITALITY, 1);

			if (r_ptr->flags1 & RF1_UNIQUE)
				chg_virtue(V_INDIVIDUALISM, 1);

			if (is_friendly(m_ptr))
				chg_virtue(V_HONOUR, 1);
			else if (!(r_ptr->flags3 & RF3_EVIL))
			{
				if (r_ptr->flags3 & RF3_GOOD)
					chg_virtue(V_COMPASSION, 2);
				else
					chg_virtue(V_COMPASSION, 1);
			}

			if (strstr((r_name + r_ptr->name),"leper"))
			{
				heal_leper = TRUE;
				chg_virtue(V_COMPASSION, 5);
			}

			if (r_ptr->flags3 & RF3_ANIMAL)
				chg_virtue(V_NATURE, 1);

			/* Redraw (later) if needed */
			if (p_ptr->health_who == c_ptr->m_idx) p_ptr->redraw |= (PR_HEALTH);

			/* Message */
			note = " looks healthier.";

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Speed Monster (Ignore "dam") */
		case GF_OLD_SPEED:
		{
			if (seen) obvious = TRUE;

			/* Speed up */
			if (m_ptr->mspeed < 150)
			{
				m_ptr->mspeed += (40 - m_ptr->mspeed + r_ptr->speed) / 4;
			}

			note = " starts moving faster.";

			if (r_ptr->flags1 & RF1_UNIQUE)
				chg_virtue(V_INDIVIDUALISM, 1);
			if (is_friendly(m_ptr))
				chg_virtue(V_HONOUR, 1);

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Slow Monster (Use "dam" as "power") */
		case GF_OLD_SLOW:
		{
			if (seen) obvious = TRUE;

			/* Powerful monsters can resist */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
			    (r_ptr->level > randint1(dam  * 3)))
			{
				note = " is unaffected!";
				obvious = FALSE;
			}

			/* Normal monsters slow down */
			else
			{
				if (m_ptr->mspeed > 60)
				{
					m_ptr->mspeed -= (40 + m_ptr->mspeed - r_ptr->speed) / 4;

				}
				note = " starts moving slower.";
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Sleep (Use "dam" as "power") */
		case GF_OLD_SLEEP:
		{
			if (seen) obvious = TRUE;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
			    (r_ptr->flags3 & RF3_NO_SLEEP) ||
			    (r_ptr->level > randint1(dam * 3)))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & RF3_NO_SLEEP)
				{
					if (seen) r_ptr->r_flags3 |= (RF3_NO_SLEEP);
				}

				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
			}
			else
			{
				/* Go to sleep (much) later */
				note = " falls asleep!";
				do_sleep = 500;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Sleep (Use "dam" as "power") */
		case GF_STASIS:
		{
			if (seen) obvious = TRUE;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
			    (r_ptr->level > randint1(dam * 4)))
			{
				note = " is unaffected!";
				obvious = FALSE;
			}
			else
			{
				/* Go to sleep (much) later */
				note = " is suspended!";
				do_sleep = 500;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Charm monster */
		case GF_CHARM:
		{
			dam += (adj_con_fix[p_ptr->stat_ind[A_CHR]] - 1);

			if (seen) obvious = TRUE;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
			    (r_ptr->flags1 & RF1_QUESTOR) ||
			    (r_ptr->flags3 & RF3_NO_CONF) ||
			    (r_ptr->level > randint1(dam * 3)))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (seen) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
			}
			else if (p_ptr->aggravate)
			{
				note = " hates you too much!";
			}
			else
			{
				note = " suddenly seems friendly!";
				set_pet(m_ptr);

				chg_virtue(V_INDIVIDUALISM, -1);
				if (r_ptr->flags3 & RF3_ANIMAL)
					chg_virtue(V_NATURE, 1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Control undead */
		case GF_CONTROL_UNDEAD:
		{
			if (seen) obvious = TRUE;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
				 (r_ptr->flags1 & RF1_QUESTOR) ||
			  (!(r_ptr->flags3 & RF3_UNDEAD)) ||
				 (r_ptr->level > randint1(dam * 3)))
			{
				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
			}
			else if (p_ptr->aggravate)
			{
				note = " hates you too much!";
			}
			else
			{
				note = " is in your thrall!";
				set_pet(m_ptr);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Tame animal */
		case GF_CONTROL_ANIMAL:
		{
			if (seen) obvious = TRUE;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				 (r_ptr->flags1 & (RF1_QUESTOR)) ||
			  (!(r_ptr->flags3 & (RF3_ANIMAL))) ||
				 (r_ptr->flags3 & (RF3_NO_CONF)) ||
				 (r_ptr->level > randint1(dam * 3)))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_CONF))
				{
					if (seen) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
			}
			else if (p_ptr->aggravate)
			{
				note = " hates you too much!";
			}
			else
			{
				note = " is tamed!";
				set_pet(m_ptr);

				if (r_ptr->flags3 & RF3_ANIMAL)
					chg_virtue(V_NATURE, 1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Confusion (Use "dam" as "power") */
		case GF_OLD_CONF:
		{
			if (seen) obvious = TRUE;

			/* Get confused later */
			do_conf = damroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
			    (r_ptr->flags3 & (RF3_NO_CONF)) ||
			    (r_ptr->level > randint1(dam * 3)))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_CONF))
				{
					if (seen) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				do_conf = 0;

				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		case GF_STUN:
		{
			if (seen) obvious = TRUE;

			do_stun = damroll((p_ptr->lev / 10) + 3, (dam)) + 1;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
			    (r_ptr->level > randint1(dam * 3)))
			{
				/* Resist */
				do_stun = 0;

				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}




		/* Lite, but only hurts susceptible creatures */
		case GF_LITE_WEAK:
		{
			/* Hurt by light */
			if (r_ptr->flags3 & (RF3_HURT_LITE))
			{
				/* Obvious effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				if (seen) r_ptr->r_flags3 |= (RF3_HURT_LITE);

				/* Special effect */
				note = " cringes from the light!";
				note_dies = " shrivels away in the light!";
			}

			/* Normally no damage */
			else
			{
				/* No damage */
				dam = 0;
			}

			break;
		}



		/* Lite -- opposite of Dark */
		case GF_LITE:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags4 & (RF4_BR_LITE))
			{
				note = " resists.";
				dam *= 2; dam /= (rand_range(7, 12));
			}
			else if (r_ptr->flags3 & (RF3_HURT_LITE))
			{
				if (seen) r_ptr->r_flags3 |= (RF3_HURT_LITE);
				note = " cringes from the light!";
				note_dies = " shrivels away in the light!";
				dam *= 2;
			}
			break;
		}


		/* Dark -- opposite of Lite */
		case GF_DARK:
		{
			if (seen) obvious = TRUE;

			/* Likes darkness... */
			if ((r_ptr->flags4 & (RF4_BR_DARK)) ||
			    (r_ptr->flags3 & RF3_ORC) ||
			    (r_ptr->flags3 & RF3_HURT_LITE))
			{
				note = " resists.";
				dam *= 2; dam /= (rand_range(7, 12));
			}
			break;
		}


		/* Stone to Mud */
		case GF_KILL_WALL:
		{
			/* Hurt by rock remover */
			if (r_ptr->flags3 & (RF3_HURT_ROCK))
			{
				/* Notice effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				if (seen) r_ptr->r_flags3 |= (RF3_HURT_ROCK);

				/* Cute little message */
				note = " loses some skin!";
				note_dies = " dissolves!";
			}

			/* Usually, ignore the effects */
			else
			{
				/* No damage */
				dam = 0;
			}

			break;
		}


		/* Teleport undead (Use "dam" as "power") */
		case GF_AWAY_UNDEAD:
		{
			/* Only affect undead */
			if (r_ptr->flags3 & (RF3_UNDEAD))
			{
				bool resists_tele = FALSE;

				if (r_ptr->flags3 & (RF3_RES_TELE))
				{
					if (r_ptr->flags1 & (RF1_UNIQUE))
					{
						if (seen) r_ptr->r_flags3 |= RF3_RES_TELE;
						note = " is unaffected!";
						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(150))
					{
						if (seen) r_ptr->r_flags3 |= RF3_RES_TELE;
						note = " resists!";
						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
					if (seen) obvious = TRUE;
					if (seen) r_ptr->r_flags3 |= (RF3_UNDEAD);
					do_dist = dam;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Teleport evil (Use "dam" as "power") */
		case GF_AWAY_EVIL:
		{
			/* Only affect evil */
			if (r_ptr->flags3 & (RF3_EVIL))
			{
				bool resists_tele = FALSE;

				if (r_ptr->flags3 & (RF3_RES_TELE))
				{
					if (r_ptr->flags1 & (RF1_UNIQUE))
					{
						if (seen) r_ptr->r_flags3 |= RF3_RES_TELE;
						note = " is unaffected!";
						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(150))
					{
						if (seen) r_ptr->r_flags3 |= RF3_RES_TELE;
						note = " resists!";
						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
					if (seen) obvious = TRUE;
					if (seen) r_ptr->r_flags3 |= (RF3_EVIL);
					do_dist = dam;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Teleport monster (Use "dam" as "power") */
		case GF_AWAY_ALL:
		{
			bool resists_tele = FALSE;

			if (r_ptr->flags3 & (RF3_RES_TELE))
			{
				if (r_ptr->flags1 & (RF1_UNIQUE))
				{
					if (seen) r_ptr->r_flags3 |= RF3_RES_TELE;
					note = " is unaffected!";
					resists_tele = TRUE;
				}
				else if (r_ptr->level > randint1(150))
				{
					if (seen) r_ptr->r_flags3 |= RF3_RES_TELE;
					note = " resists!";
					resists_tele = TRUE;
				}
			}

			if (!resists_tele)
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Prepare to teleport */
				do_dist = dam;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn undead (Use "dam" as "power") */
		case GF_TURN_UNDEAD:
		{
			/* Only affect undead */
			if (r_ptr->flags3 & (RF3_UNDEAD))
			{
				/* Learn about type */
				if (seen) r_ptr->r_flags3 |= (RF3_UNDEAD);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Apply some fear */
				do_fear = damroll(3, (dam / 2)) + 1;

				/* Attempt a saving throw */
				if (r_ptr->level > randint1(dam * 3))
				{
					/* No obvious effect */
					note = " is unaffected!";
					obvious = FALSE;
					do_fear = 0;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn evil (Use "dam" as "power") */
		case GF_TURN_EVIL:
		{
			/* Only affect evil */
			if (r_ptr->flags3 & (RF3_EVIL))
			{
				/* Learn about type */
				if (seen) r_ptr->r_flags3 |= (RF3_EVIL);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Apply some fear */
				do_fear = damroll(3, (dam / 2)) + 1;

				/* Attempt a saving throw */
				if (r_ptr->level > randint1(dam * 3))
				{
					/* No obvious effect */
					note = " is unaffected!";
					obvious = FALSE;
					do_fear = 0;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn monster (Use "dam" as "power") */
		case GF_TURN_ALL:
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Apply some fear */
			do_fear = damroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
			    (r_ptr->flags3 & (RF3_NO_FEAR)) ||
			    (r_ptr->level > randint1(dam * 3)))
			{
				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
				do_fear = 0;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Dispel undead */
		case GF_DISP_UNDEAD:
		{
			/* Only affect undead */
			if (r_ptr->flags3 & (RF3_UNDEAD))
			{
				/* Learn about type */
				if (seen) r_ptr->r_flags3 |= (RF3_UNDEAD);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
				note = " shudders.";
				note_dies = " dissolves!";
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}


		/* Dispel evil */
		case GF_DISP_EVIL:
		{
			/* Only affect evil */
			if (r_ptr->flags3 & (RF3_EVIL))
			{
				/* Learn about type */
				if (seen) r_ptr->r_flags3 |= (RF3_EVIL);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
				note = " shudders.";
				note_dies = " dissolves!";
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel good */
		case GF_DISP_GOOD:
		{
			/* Only affect good */
			if (r_ptr->flags3 & (RF3_GOOD))
			{
				/* Learn about type */
				if (seen) r_ptr->r_flags3 |= (RF3_GOOD);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
				note = " shudders.";
				note_dies = " dissolves!";
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel living */
		case GF_DISP_LIVING:
		{
			/* Only affect non-undead */
			if (monster_living(r_ptr))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
				note = " shudders.";
				note_dies = " dissolves!";
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel demons */
		case GF_DISP_DEMON:
		{
			/* Only affect demons */
			if (r_ptr->flags3 & (RF3_DEMON))
			{
				/* Learn about type */
				if (seen) r_ptr->r_flags3 |= (RF3_DEMON);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
				note = " shudders.";
				note_dies = " dissolves!";
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel monster */
		case GF_DISP_ALL:
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Message */
			note = " shudders.";
			note_dies = " dissolves!";

			break;
		}


		/* Default */
		default:
		{
			/* Irrelevant */
			skipped = TRUE;

			/* No damage */
			dam = 0;

			break;
		}
	}


	/* Absolutely no effect */
	if (skipped) return (FALSE);


	/* "Unique" monsters cannot be polymorphed */
	if (r_ptr->flags1 & (RF1_UNIQUE)) do_poly = FALSE;

	/* Quest monsters cannot be polymorphed */
	if (r_ptr->flags1 & RF1_QUESTOR) do_poly = FALSE;

	/* "Unique" and "quest" monsters can only be "killed" by the player. */
	if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags1 & RF1_QUESTOR) ||
		 (r_ptr->flags3 & RF3_UNIQUE_7))
	{
		if (who && (dam > m_ptr->hp)) dam = m_ptr->hp;
	}

	/* Modify the damage */
	dam = mon_damage_mod(m_ptr, dam, 0);

	/* Check for death */
	if (dam > m_ptr->hp)
	{
		/* Extract method of death */
		note = note_dies;
	}

	/* Mega-Hack -- Handle "polymorph" -- monsters get a saving throw */
	else if (do_poly && (randint1(150) > r_ptr->level))
	{
		if (polymorph_monster(y, x))
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Turn off the damage */
			dam = 0;

			/* Hack -- Get new monster */
			m_ptr = &m_list[c_ptr->m_idx];

			/* Hack -- Get new race */
			r_ptr = &r_info[m_ptr->r_idx];

			/* Show the polymorph message (note is not used) */
			msg_format("%^s changes!", m_name);

			/* Get the monster name (AFTER polymorphing) */
			monster_desc(m_name, m_ptr, 0);
		}
		else
		{
			/* No polymorph */
			note = " is unaffected!";
		}
	}

	/* Handle "teleport" */
	else if (do_dist)
	{
		/* Obvious */
		if (seen) obvious = TRUE;

		/* Message */
		note = " disappears!";

		chg_virtue(V_VALOUR, -1);

		/* Teleport */
		(void)teleport_away(c_ptr->m_idx, do_dist);

		/* Hack -- get new location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Hack -- get new grid */
		c_ptr = area(y,x);
	}

	/* Sound and Impact breathers never stun */
	else if (do_stun &&
	    !(r_ptr->flags4 & (RF4_BR_SOUN)) &&
	    !(r_ptr->flags4 & (RF4_BR_WALL)))
	{
		/* Obvious */
		if (seen) obvious = TRUE;

		/* Get confused */
		if (m_ptr->stunned)
		{
			note = " is more dazed.";
			tmp = m_ptr->stunned + (do_stun / 2);
		}
		else
		{
			note = " is dazed.";
			tmp = do_stun;
		}

		/* Apply stun */
		m_ptr->stunned = (tmp < 200) ? tmp : 200;

		/* Get angry */
		get_angry = TRUE;
	}

	/* Confusion and Chaos breathers (and sleepers) never confuse */
	else if (do_conf &&
		 !(r_ptr->flags3 & (RF3_NO_CONF)) &&
		 !(r_ptr->flags4 & (RF4_BR_CONF)) &&
		 !(r_ptr->flags4 & (RF4_BR_CHAO)))
	{
		/* Obvious */
		if (seen) obvious = TRUE;

		/* Already partially confused */
		if (m_ptr->confused)
		{
			note = " looks more confused.";
			tmp = m_ptr->confused + (do_conf / 2);
		}

		/* Was not confused */
		else
		{
			note = " looks confused.";
			tmp = do_conf;
		}

		/* Apply confusion */
		m_ptr->confused = (tmp < 200) ? tmp : 200;

		/* Get angry */
		get_angry = TRUE;
	}

	/* Look to see if we've spotted a mimic */
	if ((m_ptr->smart & SM_MIMIC) && obvious)
	{
		/* Toggle flag */
		m_ptr->smart &= ~(SM_MIMIC);

		/* It is in the monster list now if visible */
		if (m_ptr->ml) update_mon_vis(m_ptr->r_idx, 1);

		/* We've spotted it */
		msg_format("You've found %s!", m_name);
	}


	/* Fear */
	if (do_fear)
	{
		/* Increase fear */
		tmp = m_ptr->monfear + do_fear;

		/* Set fear */
		m_ptr->monfear = (tmp < 200) ? tmp : 200;

		/* Get angry */
		get_angry = TRUE;
	}


	/* If another monster did the damage, hurt the monster by hand */
	if (who)
	{
		/* Redraw (later) if needed */
		if (p_ptr->health_who == c_ptr->m_idx) p_ptr->redraw |= (PR_HEALTH);

		/* Wake the monster up */
		m_ptr->csleep = 0;

		/* Hurt the monster */
		m_ptr->hp -= dam;

		/* Dead monster */
		if (m_ptr->hp < 0)
		{
			bool sad = FALSE;

			s16b old_m_d_head = mon_d_head;

			if (is_pet(m_ptr) && !(m_ptr->ml))
				sad = TRUE;

			/* Increase the number of dying monsters */
			mon_d_head++;

			/* Go back to the start of the queue */
			if (mon_d_head >= DEATH_MAX) mon_d_head = 0;

			if (mon_d_head == mon_d_tail)
			{
				/*
				 * We have greater than the maximum number of monsters dying,
				 * revert to the old number, and do not queue this one.
				 */
				mon_d_head = old_m_d_head;

				/* Hack XXX Die, but do not explode and call project() */
				(void)monster_death(c_ptr->m_idx, FALSE);

				/* Delete the monster */
				delete_monster_idx(c_ptr->m_idx);
			}
			else
			{
				/* Queue the monster */
				mon_d_m_idx[old_m_d_head] = c_ptr->m_idx;
			}

			/* Give detailed messages if destroyed */
			if (known && note)
			{
				if (see_s)
				{
					msg_format("%^s%s", m_name, note);
				}
				else
				{
					p_ptr->mon_fight = TRUE;
				}
			}

			if (sad)
			{
				msg_print("You feel sad for a moment.");
			}
		}

		/* Damaged monster */
		else
		{
			/* Give detailed messages if visible or destroyed */
			if (note && seen)
			{
				msg_format("%^s%s", m_name, note);
			}
			/* Hack -- Pain message */
			else if (see_s)
			{
				message_pain(c_ptr->m_idx, dam);
			}

			/* Hack -- handle sleep */
			if (do_sleep) m_ptr->csleep = do_sleep;
		}
	}


	else if (heal_leper)
	{
		msg_format("%^s is healed!", m_name);

		/* Note that lepers do not glow - so no update for mon_lite */

		/* Remove the leper */
		delete_monster_idx(c_ptr->m_idx);
	}
	/* If the player did it, give him experience, check fear */
	else
	{
		bool fear = FALSE;

		/* Hurt the monster, check for fear and death */
		if (mon_take_hit(c_ptr->m_idx, dam, &fear, note_dies))
		{
			/* Dead monster */
		}

		/* Damaged monster */
		else
		{
			/* HACK - anger the monster before showing the sleep message */
			if (do_sleep) anger_monster(m_ptr);

			/* Give detailed messages if visible or destroyed */
			if (note && seen)
			{
				msg_format("%^s%s", m_name, note);
			}
			/* Hack -- Pain message */
			else if (see_s)
			{
				message_pain(c_ptr->m_idx, dam);
			}
			else
			{
				p_ptr->mon_fight = TRUE;
			}

			/* Anger monsters */
			if (((dam > 0) || get_angry) && !do_sleep)
				anger_monster(m_ptr);

			/* Take note */
			if ((fear || do_fear) && (m_ptr->ml))
			{
				/* Sound */
				sound(SOUND_FLEE);

				/* Message */
				msg_format("%^s flees in terror!", m_name);
			}

			/* Hack -- handle sleep */
			if (do_sleep) m_ptr->csleep = do_sleep;
		}
	}


	/* XXX XXX XXX Verify this code */

	/* Update the monster */
	update_mon(c_ptr->m_idx, FALSE);

	/* Redraw the monster grid */
	lite_spot(y, x);


	/* Update monster recall window */
	if (p_ptr->monster_race_idx == m_ptr->r_idx)
	{
		/* Window stuff */
		p_ptr->window |= (PW_MONSTER);
	}


	/* Track it */
	project_m_n++;
	project_m_x = x;
	project_m_y = y;


	/* Return "Anything seen?" */
	return (obvious);
}


/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to the player.
 *
 * This routine takes a "source monster" (by index), a "distance", a default
 * "damage", and a "damage type".  See "project_m()" above.
 *
 * If "rad" is non-zero, then the blast was centered elsewhere, and the damage
 * is reduced (see "project_m()" above).  This can happen if a monster breathes
 * at the player and hits a wall instead.
 *
 * NOTE (Zangband): 'Bolt' attacks can be reflected back, so we need
 * to know if this is actually a ball or a bolt spell
 *
 *
 * We return "TRUE" if any "obvious" effects were observed.  XXX XXX Actually,
 * we just assume that the effects were obvious, for historical reasons.
 */
static bool project_p(int who, int r, int y, int x, int dam, int typ, int a_rad)
{
	int k = 0;

	/* Hack -- assume obvious */
	bool obvious = TRUE;

	/* Player blind-ness */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	/* Player needs a "description" (he is blind) */
	bool fuzzy = FALSE;

	/* Source monster */
	monster_type *m_ptr;

	/* Monster name (for attacks) */
	char m_name[80];

	/* Monster name (for damage) */
	char killer[80];

	/* Hack -- messages */
	cptr act = NULL;

	cave_type *c_ptr;

	/* Player is not here */
	if ((x != p_ptr->px) || (y != p_ptr->py)) return (FALSE);

	/* Player cannot hurt himself */
	if (!who) return (FALSE);


	if (p_ptr->reflect && !a_rad && !one_in_(10))
	{
		int t_y, t_x;
		int max_attempts = 10;

		if (blind) msg_print("Something bounces!");
		else msg_print("The attack bounces!");

		/* Choose 'new' target */
		while (TRUE)
		{
			t_y = m_list[who].fy + rand_range(-1, 1);
			t_x = m_list[who].fx + rand_range(-1, 1);
			max_attempts--;


			/* paranoia */
			if (!max_attempts) break;

			/* not off edge */
			if (!in_bounds2(t_y, t_x)) continue;

			c_ptr = area(t_y, t_x);

			/* Hack - exit if can see the reflection */
			if (player_has_los_grid(c_ptr)) break;
		}

		if (max_attempts < 1)
		{
			t_y = m_list[who].fy;
			t_x = m_list[who].fx;
		}

		(void)project(0, 0, t_y, t_x, dam, typ, (PROJECT_STOP | PROJECT_KILL));

		disturb(TRUE);
		return TRUE;
	}

	/* XXX XXX XXX */
	/* Limit maximum damage */
	if (dam > 1600) dam = 1600;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	/* If the player is blind, be more descriptive */
	if (blind) fuzzy = TRUE;


	/* Get the source monster */
	m_ptr = &m_list[who];

	/* Get the monster name */
	monster_desc(m_name, m_ptr, 0);

	/* Get the monster's real name */
	monster_desc(killer, m_ptr, 0x88);


	/* Analyze the damage */
	switch (typ)
	{
		/* Standard damage -- hurts inventory too */
		case GF_ACID:
		{
			if (fuzzy) msg_print("You are hit by acid!");
			acid_dam(dam, killer);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_FIRE:
		{
			if (fuzzy) msg_print("You are hit by fire!");
			fire_dam(dam, killer);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_COLD:
		{
			if (fuzzy) msg_print("You are hit by cold!");
			cold_dam(dam, killer);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_ELEC:
		{
			if (fuzzy) msg_print("You are hit by lightning!");
			elec_dam(dam, killer);
			break;
		}

		/* Standard damage -- also poisons player */
		case GF_POIS:
		{
			if (fuzzy) msg_print("You are hit by poison!");
			if (p_ptr->resist_pois) dam = (dam + 2) / 3;
			if (p_ptr->oppose_pois) dam = (dam + 2) / 3;

			if ((!(p_ptr->oppose_pois || p_ptr->resist_pois)) &&
			     one_in_(HURT_CHANCE))
			{
				(void)do_dec_stat(A_CON);
			}

			take_hit(dam, killer, TRUE);

			if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
			{
				(void)set_poisoned(p_ptr->poisoned + randint0(dam) + 10);
			}
			break;
		}

		/* Standard damage -- also poisons / mutates player */
		case GF_NUKE:
		{
			if (fuzzy) msg_print("You are hit by radiation!");
			if (p_ptr->resist_pois) dam = (2 * dam + 2) / 5;
			if (p_ptr->oppose_pois) dam = (2 * dam + 2) / 5;
			take_hit(dam, killer, TRUE);
			if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
			{
				(void)set_poisoned(p_ptr->poisoned + randint0(dam) + 10);

				if (one_in_(5))
				{
					msg_print("You undergo a freakish metamorphosis!");
					if (one_in_(4))
						do_poly_self();
					else
						mutate_player();
				}

				if (one_in_(6))
				{
					(void)inven_damage(set_acid_destroy, 2, dam);
				}
			}
			break;
		}

		/* Standard damage */
		case GF_MISSILE:
		{
			if (fuzzy) msg_print("You are hit by something!");
			take_hit(dam, killer, TRUE);
			break;
		}

		/* Holy Orb -- Player only takes partial damage */
		case GF_HOLY_FIRE:
		{
			if (fuzzy) msg_print("You are hit by something!");
			if (p_ptr->realm1 == REALM_LIFE || p_ptr->realm2 == REALM_LIFE)
				dam /= 2;
			else if (p_ptr->realm1 == REALM_DEATH || p_ptr->realm2 == REALM_DEATH)
				dam *= 2;
			take_hit(dam, killer, TRUE);
			break;
		}

		case GF_HELL_FIRE:
		{
			if (fuzzy) msg_print("You are hit by something!");
			if (p_ptr->realm1 == REALM_DEATH || p_ptr->realm2 == REALM_DEATH)
				dam /= 2;
			else if (p_ptr->realm1 == REALM_LIFE || p_ptr->realm2 == REALM_LIFE)
				dam *= 2;
			take_hit(dam, killer, TRUE);
			break;
		}

		/* Arrow -- XXX no dodging */
		case GF_ARROW:
		{
			if (fuzzy) msg_print("You are hit by something sharp!");
			take_hit(dam, killer, TRUE);
			break;
		}

		/* Plasma -- XXX No resist */
		case GF_PLASMA:
		{
			if (fuzzy) msg_print("You are hit by something *HOT*!");
			take_hit(dam, killer, TRUE);

			if (!p_ptr->resist_sound)
			{
				(void)set_stun(p_ptr->stun +
					 randint1((dam > 40) ? 35 : (dam * 3 / 4 + 5)));
			}

			if (!(p_ptr->resist_fire ||
			      p_ptr->oppose_fire ||
			      p_ptr->immune_fire))
			{
				(void)inven_damage(set_acid_destroy, 3, dam);
			}

			break;
		}

		/* Nether -- drain experience */
		case GF_NETHER:
		{
			if (fuzzy) msg_print("You are hit by nether forces!");

			if (p_ptr->resist_nethr)
			{
				if (p_ptr->prace != RACE_SPECTRE)
					dam *= 6; dam /= rand_range(7, 12);
			}
			else
			{
				if (p_ptr->hold_life && (randint0(100) < 75))
				{
					msg_print("You keep hold of your life force!");
				}
				else if (p_ptr->hold_life)
				{
					msg_print("You feel your life slipping away!");
					lose_exp(200 + (p_ptr->exp / 1000) * MON_DRAIN_LIFE);
				}
				else
				{
					msg_print("You feel your life draining away!");
					lose_exp(200 + (p_ptr->exp / 100) * MON_DRAIN_LIFE);
				}
			}

			if (p_ptr->prace == RACE_SPECTRE)
			{
				msg_print("You feel invigorated!");
				(void)hp_player(dam / 4);
			}
			else
			{
				take_hit(dam, killer, TRUE);
			}

			break;
		}

		/* Water -- stun/confuse */
		case GF_WATER:
		{
			if (fuzzy) msg_print("You are hit by something wet!");
			if (!p_ptr->resist_sound)
			{
				(void)set_stun(p_ptr->stun + randint1(40));
			}
			if (!p_ptr->resist_confu)
			{
				(void)set_confused(p_ptr->confused + rand_range(5, 10));
			}

			if (one_in_(5))
			{
				(void)inven_damage(set_cold_destroy, 3, dam);
			}

			take_hit(dam, killer, TRUE);
			break;
		}

		/* Chaos -- many effects */
		case GF_CHAOS:
		{
			if (fuzzy) msg_print("You are hit by a wave of anarchy!");
			if (p_ptr->resist_chaos)
			{
				dam *= 6; dam /= rand_range(7, 12);
			}
			if (!p_ptr->resist_confu)
			{
				(void)set_confused(p_ptr->confused + rand_range(20, 30));
			}
			if (!p_ptr->resist_chaos)
			{
				(void)set_image(p_ptr->image + randint1(10));
				if (one_in_(3))
				{
					msg_print("Your body is twisted by chaos!");
					(void)gain_mutation(0);
				}
			}
			if (!p_ptr->resist_nethr && !p_ptr->resist_chaos)
			{
				if (p_ptr->hold_life && (randint0(100) < 75))
				{
					msg_print("You keep hold of your life force!");
				}
				else if (p_ptr->hold_life)
				{
					msg_print("You feel your life slipping away!");
					lose_exp(500 + (p_ptr->exp / 1000) * MON_DRAIN_LIFE);
				}
				else
				{
					msg_print("You feel your life draining away!");
					lose_exp(5000 + (p_ptr->exp / 100) * MON_DRAIN_LIFE);
				}
			}
			if (!p_ptr->resist_chaos || one_in_(9))
			{
				(void)inven_damage(set_elec_destroy, 2, dam);
				(void)inven_damage(set_fire_destroy, 2, dam);
			}
			take_hit(dam, killer, TRUE);
			break;
		}

		/* Shards -- mostly cutting */
		case GF_SHARDS:
		{
			if (fuzzy) msg_print("You are hit by something sharp!");
			if (p_ptr->resist_shard)
			{
				dam *= 6; dam /= rand_range(7, 12);
			}
			else
			{
				(void)set_cut(p_ptr->cut + dam);
			}

			if (!p_ptr->resist_shard || one_in_(13))
			{
				(void)inven_damage(set_cold_destroy, 2, dam);
			}

			take_hit(dam, killer, TRUE);
			break;
		}

		/* Sound -- mostly stunning */
		case GF_SOUND:
		{
			if (fuzzy) msg_print("You are hit by a loud noise!");
			if (p_ptr->resist_sound)
			{
				dam *= 5; dam /= rand_range(7, 12);
			}
			else
			{
				(void)set_stun(p_ptr->stun +
					 randint1((dam > 90) ? 35 : (dam / 3 + 5)));
			}

			if (!p_ptr->resist_sound || one_in_(13))
			{
				(void)inven_damage(set_cold_destroy, 2, dam);
			}

			take_hit(dam, killer, TRUE);
			break;
		}

		/* Pure confusion */
		case GF_CONFUSION:
		{
			if (fuzzy) msg_print("You are hit by something puzzling!");
			if (p_ptr->resist_confu)
			{
				dam *= 5; dam /= rand_range(7, 12);
			}
			if (!p_ptr->resist_confu)
			{
				(void)set_confused(p_ptr->confused + rand_range(10, 30));
			}
			take_hit(dam, killer, FALSE);
			break;
		}

		/* Disenchantment -- see above */
		case GF_DISENCHANT:
		{
			if (fuzzy) msg_print("You are hit by something static!");
			if (p_ptr->resist_disen)
			{
				dam *= 6; dam /= rand_range(7, 12);
			}
			else
			{
				(void)apply_disenchant();
			}
			take_hit(dam, killer, TRUE);
			break;
		}

		/* Nexus -- see above */
		case GF_NEXUS:
		{
			if (fuzzy) msg_print("You are hit by something strange!");
			if (p_ptr->resist_nexus)
			{
				dam *= 6; dam /= rand_range(7, 12);
			}
			else
			{
				apply_nexus(m_ptr);
			}
			take_hit(dam, killer, TRUE);
			break;
		}

		/* Force -- mostly stun */
		case GF_FORCE:
		{
			if (fuzzy) msg_print("You are hit by kinetic force!");
			if (!p_ptr->resist_sound)
			{
				(void)set_stun(p_ptr->stun + randint1(20));
			}
			take_hit(dam, killer, TRUE);
			break;
		}


		/* Rocket -- stun, cut */
		case GF_ROCKET:
		{
			if (fuzzy) msg_print("There is an explosion!");
			if (!p_ptr->resist_sound)
			{
				(void)set_stun(p_ptr->stun + randint1(20));
			}
			if (p_ptr->resist_shard)
			{
				dam /= 2;
			}
			else
			{
				(void)set_cut(p_ptr->cut + (dam / 2));
			}

			if (!p_ptr->resist_shard || one_in_(12))
			{
				(void)inven_damage(set_cold_destroy, 3, dam);
			}

			take_hit(dam, killer, TRUE);
			break;
		}

		/* Inertia -- slowness */
		case GF_INERTIA:
		{
			if (fuzzy) msg_print("You are hit by something slow!");
			(void)set_slow(p_ptr->slow + rand_range(4, 8));
			take_hit(dam, killer, TRUE);
			break;
		}

		/* Lite -- blinding */
		case GF_LITE:
		{
			if (fuzzy) msg_print("You are hit by something!");
			if (p_ptr->resist_lite)
			{
				dam *= 4; dam /= rand_range(7, 12);
			}
			else if (!blind && !p_ptr->resist_blind)
			{
				(void)set_blind(p_ptr->blind + rand_range(2, 7));
			}
			if (p_ptr->prace == RACE_VAMPIRE)
			{
				msg_print("The light scorches your flesh!");
				dam *= 2;
			}
			take_hit(dam, killer, TRUE);

			if (p_ptr->wraith_form)
			{
				p_ptr->wraith_form = 0;
				msg_print("The light forces you out of your incorporeal shadow form.");
				p_ptr->redraw |= PR_MAP;
				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);
				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			}

			break;
		}

		/* Dark -- blinding */
		case GF_DARK:
		{
			if (fuzzy) msg_print("You are hit by something!");
			if (p_ptr->resist_dark)
			{
				dam *= 4; dam /= rand_range(7, 12);

				if (p_ptr->prace == RACE_VAMPIRE) dam = 0;
			}
			else if (!blind && !p_ptr->resist_blind)
			{
				(void)set_blind(p_ptr->blind + rand_range(2, 7));
			}
			if (p_ptr->wraith_form) (void)hp_player(dam);
			else take_hit(dam, killer, TRUE);
			break;
		}

		/* Time -- bolt fewer effects XXX */
		case GF_TIME:
		{
			if (fuzzy) msg_print("You are hit by a blast from the past!");

			switch (randint1(10))
			{
				case 1: case 2: case 3: case 4: case 5:
				{
					msg_print("You feel life has clocked back.");
					lose_exp(100 + (p_ptr->exp / 100) * MON_DRAIN_LIFE);
					break;
				}

				case 6: case 7: case 8: case 9:
				{
					switch (randint1(6))
					{
						case 1: k = A_STR; act = "strong"; break;
						case 2: k = A_INT; act = "bright"; break;
						case 3: k = A_WIS; act = "wise"; break;
						case 4: k = A_DEX; act = "agile"; break;
						case 5: k = A_CON; act = "hale"; break;
						case 6: k = A_CHR; act = "beautiful"; break;
					}

					msg_format("You're not as %s as you used to be...", act);

					p_ptr->stat_cur[k] = (p_ptr->stat_cur[k] * 3) / 4;
					if (p_ptr->stat_cur[k] < 3) p_ptr->stat_cur[k] = 3;
					p_ptr->update |= (PU_BONUS);
					break;
				}

				case 10:
				{
					msg_print("You're not as powerful as you used to be...");

					for (k = 0; k < A_MAX; k++)
					{
						p_ptr->stat_cur[k] = (p_ptr->stat_cur[k] * 3) / 4;
						if (p_ptr->stat_cur[k] < 3) p_ptr->stat_cur[k] = 3;
					}
					p_ptr->update |= (PU_BONUS);
					break;
				}
			}

			take_hit(dam, killer, TRUE);
			break;
		}

		/* Gravity -- stun plus slowness plus teleport */
		case GF_GRAVITY:
		{
			if (fuzzy) msg_print("You are hit by something heavy!");
			msg_print("Gravity warps around you.");
			teleport_player(5);
			if (!p_ptr->ffall)
				(void)set_slow(p_ptr->slow + rand_range(4, 8));
			if (!(p_ptr->resist_sound || p_ptr->ffall))
			{
				(void)set_stun(p_ptr->stun +
					 randint1((dam > 90) ? 35 : (dam / 3 + 5)));
			}
			if (p_ptr->ffall)
			{
				dam = (dam * 2) / 3;
			}

			if (!p_ptr->ffall || one_in_(13))
			{
				(void)inven_damage(set_cold_destroy, 2, dam);
			}

			take_hit(dam, killer, TRUE);
			break;
		}

		/* Standard damage */
		case GF_DISINTEGRATE:
		{
			if (fuzzy) msg_print("You are hit by pure energy!");
			take_hit(dam, killer, TRUE);
			break;
		}

		case GF_OLD_HEAL:
		{
			if (fuzzy) msg_print("You are hit by something invigorating!");
			(void)hp_player(dam);
			dam = 0;
			break;
		}

		case GF_OLD_SPEED:
		{
			if (fuzzy) msg_print("You are hit by something!");
			(void)set_fast(p_ptr->fast + randint1(5));
			dam = 0;
			break;
		}

		case GF_OLD_SLOW:
		{
			if (fuzzy) msg_print("You are hit by something slow!");
			(void)set_slow(p_ptr->slow + rand_range(4, 8));
			break;
		}

		case GF_OLD_SLEEP:
		{
			if (p_ptr->free_act) break;
			if (fuzzy) msg_print("You fall asleep!");

			if (ironman_nightmare)
			{
				msg_print("A horrible vision enters your mind.");

				/* Pick a nightmare */
				get_mon_num_prep(get_nightmare, NULL);

				/* Have some nightmares */
				have_nightmare(get_mon_num(MAX_DEPTH));

				/* Remove the monster restriction */
				get_mon_num_prep(NULL, NULL);
			}

			(void)set_paralyzed(p_ptr->paralyzed + dam);
			dam = 0;
			break;
		}

		/* Pure damage */
		case GF_MANA:
		{
			if (fuzzy) msg_print("You are hit by an aura of magic!");
			take_hit(dam, killer, TRUE);
			break;
		}

		/* Pure damage */
		case GF_METEOR:
		{
			if (fuzzy) msg_print("Something falls from the sky on you!");
			take_hit(dam, killer, TRUE);
			if (!p_ptr->resist_shard || one_in_(13))
			{
				if (!p_ptr->immune_fire) (void)inven_damage(set_fire_destroy, 2, dam);
				(void)inven_damage(set_cold_destroy, 2, dam);
			}

			break;
		}

		/* Ice -- cold plus stun plus cuts */
		case GF_ICE:
		{
			if (fuzzy) msg_print("You are hit by something sharp and cold!");
			cold_dam(dam, killer);
			if (!p_ptr->resist_shard)
			{
				(void)set_cut(p_ptr->cut + damroll(5, 8));
			}
			if (!p_ptr->resist_sound)
			{
				(void)set_stun(p_ptr->stun + randint1(15));
			}

			if ((!(p_ptr->resist_cold || p_ptr->oppose_cold)) || one_in_(12))
			{
				if (!p_ptr->immune_cold) (void)inven_damage(set_cold_destroy, 3, dam);
			}

			break;
		}

		/* Death Ray */
		case GF_DEATH_RAY:
		{
			if (fuzzy) msg_print("You are hit by something extremely cold!");

			switch (p_ptr->prace)
			{
				/* Some races are immune */
				case RACE_GOLEM:
				case RACE_SKELETON:
				case RACE_ZOMBIE:
				case RACE_VAMPIRE:
				case RACE_SPECTRE:
				case RACE_GHOUL:
				{
					dam = 0;
					break;
				}
				/* Hurt a lot */
				default:
				{
					take_hit(dam, killer, TRUE);
					break;
				}
			}

			break;
		}


		/* Default */
		default:
		{
			/* No damage */
			dam = 0;

			break;
		}
	}


	/* Disturb */
	disturb(TRUE);


	/* Return "Anything seen?" */
	return (obvious);
}


/*
 * Find the distance from (x, y) to a line.
 */
int dist_to_line(int y, int x, int y1, int x1, int y2, int x2)
{
	/* Vector from (x, y) to (x1, y1) */
	int py = y1 - y;
	int px = x1 - x;

	/* Normal vector */
	int ny = x2 - x1;
	int nx = y1 - y2;

	/* Length of N */
	int d = distance(y1, x1, y2, x2);

	/* Component of P on N */
	d = ((d) ? ((py * ny + px * nx) / d) : 0);

	/* Absolute value */
	return ((d >= 0) ? d : 0 - d);
}




/*
 * Generic "beam"/"bolt"/"ball" projection routine.
 *
 * Input:
 *   who: Index of "source" monster (zero for "player")
 *   rad: Radius of explosion (0 = beam/bolt, 1 to 9 = ball)
 *   y,x: Target location (or location to travel "towards")
 *   dam: Base damage roll to apply to affected monsters (or player)
 *   typ: Type of damage to apply to monsters (and objects)
 *   flg: Extra bit flags (see PROJECT_xxxx in "defines.h")
 *
 * Return:
 *   TRUE if any "effects" of the projection were observed, else FALSE
 *
 * Allows a monster (or player) to project a beam/bolt/ball of a given kind
 * towards a given location (optionally passing over the heads of interposing
 * monsters), and have it do a given amount of damage to the monsters (and
 * optionally objects) within the given radius of the final location.
 *
 * A "bolt" travels from source to target and affects only the target grid.
 * A "beam" travels from source to target, affecting all grids passed through.
 * A "ball" travels from source to the target, exploding at the target, and
 *   affecting everything within the given radius of the target location.
 *
 * Traditionally, a "bolt" does not affect anything on the ground, and does
 * not pass over the heads of interposing monsters, much like a traditional
 * missile, and will "stop" abruptly at the "target" even if no monster is
 * positioned there, while a "ball", on the other hand, passes over the heads
 * of monsters between the source and target, and affects everything except
 * the source monster which lies within the final radius, while a "beam"
 * affects every monster between the source and target, except for the casting
 * monster (or player), and rarely affects things on the ground.
 *
 * Two special flags allow us to use this function in special ways, the
 * "PROJECT_HIDE" flag allows us to perform "invisible" projections, while
 * the "PROJECT_JUMP" flag allows us to affect a specific grid, without
 * actually projecting from the source monster (or player).
 *
 * The player will only get "experience" for monsters killed by himself
 * Unique monsters can only be destroyed by attacks from the player
 *
 * Only 1024 grids can be affected per projection.  This affects the maximum
 * possible effect per projection.
 *
 * One can project in a given "direction" by combining PROJECT_THRU with small
 * offsets to the initial location (see "line_spell()"), or by calculating
 * "virtual targets" far away from the player.
 *
 * One can also use PROJECT_THRU to send a beam/bolt along an angled path,
 * continuing until it actually hits something (useful for "stone to mud").
 *
 * Bolts and Beams explode INSIDE walls, so that they can destroy doors.
 *
 * Balls must explode BEFORE hitting walls, or they would affect monsters
 * on both sides of a wall.
 *
 * We "pre-calculate" the blast area only in part for efficiency.
 * More importantly, this lets us do "explosions" from the "inside" out.
 * This results in a more logical distribution of "blast" treasure.
 * It also produces a better (in my opinion) animation of the explosion.
 * It could be (but is not) used to have the treasure dropped by monsters
 * in the middle of the explosion fall "outwards", and then be damaged by
 * the blast as it spreads outwards towards the treasure drop location.
 *
 * Walls and doors are included in the blast area, so that they can be
 * "burned" or "melted" in later versions.
 *
 * This algorithm is intended to maximize simplicity, not necessarily
 * efficiency, since this function is not a bottleneck in the code.
 *
 * We apply the blast effect from ground zero outwards, in several passes,
 * first affecting features, then objects, then monsters, then the player.
 * This allows walls to be removed before checking the object or monster
 * in the wall, and protects objects which are dropped by monsters killed
 * in the blast, and allows the player to see all affects before he is
 * killed or teleported away.  The semantics of this method are open to
 * various interpretations, but they seem to work well in practice.
 *
 * We process the blast area from ground-zero outwards to allow for better
 * distribution of treasure dropped by monsters, and because it provides a
 * pleasing visual effect at low cost.
 *
 * Note that the damage done by "ball" explosions decreases with distance.
 * This decrease is rapid, grids at radius "dist" take "1/dist" damage.
 *
 * Notice the "napalm" effect of "beam" weapons.  First they "project" to
 * the target, and then the damage "flows" along this beam of destruction.
 * The damage at every grid is the same as at the "center" of a "ball"
 * explosion, since the "beam" grids are treated as if they ARE at the
 * center of a "ball" explosion.
 *
 * Currently, specifying "beam" plus "ball" means that locations which are
 * covered by the initial "beam", and also covered by the final "ball", except
 * for the final grid (the epicenter of the ball), will be "hit twice", once
 * by the initial beam, and once by the exploding ball.  For the grid right
 * next to the epicenter, this results in 150% damage being done.  The center
 * does not have this problem, for the same reason the final grid in a "beam"
 * plus "bolt" does not -- it is explicitly removed.  Simply removing "beam"
 * grids which are covered by the "ball" will NOT work, as then they will
 * receive LESS damage than they should.  Do not combine "beam" with "ball".
 *
 * The array "gy[],gx[]" with current size "grids" is used to hold the
 * collected locations of all grids in the "blast area" plus "beam path".
 *
 * Note the rather complex usage of the "gm[]" array.  First, gm[0] is always
 * zero.  Second, for N>1, gm[N] is always the index (in gy[],gx[]) of the
 * first blast grid (see above) with radius "N" from the blast center.  Note
 * that only the first gm[1] grids in the blast area thus take full damage.
 * Also, note that gm[rad+1] is always equal to "grids", which is the total
 * number of blast grids.
 *
 * Note that once the projection is complete, (y2,x2) holds the final location
 * of bolts/beams, and the "epicenter" of balls.
 *
 * Note also that "rad" specifies the "inclusive" radius of projection blast,
 * so that a "rad" of "one" actually covers 5 or 9 grids, depending on the
 * implementation of the "distance" function.  Also, a bolt can be properly
 * viewed as a "ball" with a "rad" of "zero".
 *
 * Note that if no "target" is reached before the beam/bolt/ball travels the
 * maximum distance allowed (MAX_RANGE), no "blast" will be induced.  This
 * may be relevant even for bolts, since they have a "1x1" mini-blast.
 *
 * Note that for consistency, we "pretend" that the bolt actually takes "time"
 * to move from point A to point B, even if the player cannot see part of the
 * projection path.  Note that in general, the player will *always* see part
 * of the path, since it either starts at the player or ends on the player.
 *
 * Hack -- we assume that every "projection" is "self-illuminating".
 *
 * Hack -- when only a single monster is affected, we automatically track
 * (and recall) that monster, unless "PROJECT_JUMP" is used.
 *
 * Note that all projections now "explode" at their final destination, even
 * if they were being projected at a more distant destination.  This means
 * that "ball" spells will *always* explode.
 *
 * Note that we must call "handle_stuff()" after affecting terrain features
 * in the blast radius, in case the "illumination" of the grid was changed,
 * and "update_view()" and "update_monsters()" need to be called.
 */
bool project(int who, int rad, int y, int x, int dam, int typ, u16b flg)
{
	int i, t, dist;

	int y1, x1;
	int y2, x2;

	int dist_hack = 0;

	int y_saver, x_saver; /* For reflecting monsters */

	int msec = delay_factor * delay_factor * delay_factor;

	/* Assume the player sees nothing */
	bool notice = FALSE;

	/* Assume the player has seen nothing */
	bool visual = FALSE;

	/* Assume the player has seen no blast grids */
	bool drawn = FALSE;

	/* Assume to be a normal ball spell */
	bool breath = FALSE;

	/* Is the player blind? */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	/* Number of grids in the "path" */
	int path_n = 0;

	/* Actual grids in the "path" */
	coord path_g[512];

	/* Number of grids in the "blast area" (including the "beam" path) */
	int grids = 0;

	/* Coordinates of the affected grids */
	int gx[1024], gy[1024];

	/* Encoded "radius" info (see above) */
	byte gm[32];

	/* Actual radius encoded in gm[] */
	int gm_rad = rad;

	bool jump = FALSE;

	cave_type *c_ptr;

	/* Are there no monsters queued to die? */
	bool mon_explode = (mon_d_head == mon_d_tail) ? TRUE : FALSE;


	/* Hack -- some weapons always stop at monsters */
	if (typ == GF_ROCKET) flg |= PROJECT_STOP;

	/* Hack -- Jump to target */
	if (flg & (PROJECT_JUMP))
	{
		x1 = x;
		y1 = y;

		/* Default "destination" */
		y2 = y;
		x2 = x;

		/* Clear the flag */
		flg &= ~(PROJECT_JUMP);

		jump = TRUE;
	}

	/* Start at player */
	else if (who <= 0)
	{
		x1 = p_ptr->px;
		y1 = p_ptr->py;

		/* Default "destination" */
		y2 = y;
		x2 = x;
	}

	/* Start at monster */
	else if (who > 0)
	{
		if (ironman_los)
		{
			/*
			 * Start at player, and go to monster
			 * This means that monsters always can hit the player
			 * if the player can see them
			 */
			y1 = y;
			x1 = x;

			x2 = m_list[who].fx;
			y2 = m_list[who].fy;
		}
		else
		{
			/* Start at monster, and go to player */
			y2 = y;
			x2 = x;

			x1 = m_list[who].fx;
			y1 = m_list[who].fy;
		}
	}

	/* Hack -- verify stuff */
	if (flg & (PROJECT_THRU))
	{
		if ((x1 == x2) && (y1 == y2))
		{
			flg &= ~(PROJECT_THRU);
		}
	}

	/* Handle a breath attack */
	if (rad < 0)
	{
		rad = 0 - rad;
		breath = TRUE;
		flg |= PROJECT_HIDE;
	}

	/* Calculate the projection path */
	path_n = project_path(path_g, y1, x1, y2, x2, flg);

	/* Do we need to invert the path? */
	if (ironman_los && !jump && (who > 0))
	{
		for (i = 0; i < path_n / 2; i++)
		{
			/* Swap y coords */
			t = path_g[i].y;
			path_g[i].y = path_g[path_n - 1 - i].y;
			path_g[path_n - 1 - i].y = t;

			/* Swap x coords */
			t = path_g[i].x;
			path_g[i].x = path_g[path_n - 1 - i].x;
			path_g[path_n - 1 - i].x = t;

			/* Swap the initial and final coords */
			t = y1;
			y1 = y2;
			y2 = t;

			t = x1;
			x1 = x2;
			x2 = t;
		}
	}

	/* Hack -- Assume there will be no blast (max radius 32) */
	for (dist = 0; dist < 32; dist++) gm[dist] = 0;

	/* Initial grid */
	y = y1;
	x = x1;
	y_saver = y1;
	x_saver = x1;
	dist = 0;

	/* Collect beam grids */
	if (flg & (PROJECT_BEAM))
	{
		gy[grids] = y;
		gx[grids] = x;
		grids++;
	}

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		int oy = y;
		int ox = x;

		int ny = path_g[i].y;
		int nx = path_g[i].x;

		c_ptr = area(ny, nx);

		/* Hack -- Balls explode before reaching walls */
		if (!cave_floor_grid(c_ptr) && (rad > 0)) break;

		/* Require fields do not block magic */
		if (fields_have_flags(c_ptr->fld_idx, FIELD_INFO_NO_MAGIC)) break;

		/* Advance */
		y = ny;
		x = nx;

		/* Collect beam grids */
		if (flg & (PROJECT_BEAM))
		{
			gy[grids] = y;
			gx[grids] = x;
			grids++;
		}

		/* Only do visuals if requested */
		if (!blind && !(flg & (PROJECT_HIDE)))
		{

			/* Only do visuals if the player can "see" the bolt */
			if (panel_contains(y, x) && player_has_los_grid(c_ptr))
			{
				byte a, c;

				/* Obtain the bolt pict */
				bolt_pict(oy, ox, y, x, typ, &a, &c);

				/* Visual effects */
				print_rel(c, a, y, x);
				move_cursor_relative(y, x);

				if (fresh_before) Term_fresh();

				/* Delay */
				Term_xtra(TERM_XTRA_DELAY, msec);

				/* Show it */
				lite_spot(y, x);
				if (fresh_before) Term_fresh();

				/* Display "beam" grids */
				if (flg & (PROJECT_BEAM))
				{
					/* Obtain the explosion pict */
					bolt_pict(y, x, y, x, typ, &a, &c);

					/* Visual effects */
					print_rel(c, a, y, x);
				}

				/* Hack -- Activate delay */
				visual = TRUE;
			}

			/* Hack -- delay anyway for consistency */
			else if (visual)
			{
				/* Delay for consistency */
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}
	}


	/* Save the "blast epicenter" */
	y2 = y;
	x2 = x;

	/* Start the "explosion" */
	gm[0] = 0;

	/* Hack -- make sure beams get to "explode" */
	gm[1] = grids;

	dist_hack = dist;
	dist = path_n;

	/* If we found a "target", explode there */
	if (dist <= MAX_RANGE)
	{
		/* Mega-Hack -- remove the final "beam" grid */
		if ((flg & (PROJECT_BEAM)) && (grids > 0)) grids--;

		/*
		 * Create a conical breath attack
		 *
		 *         ***
		 *     ********
		 * D********@**
		 *     ********
		 *         ***
		 */
		if (breath)
		{
			int by, bx;
			int brad = 0;
			int bdis = 0;
			int cdis;
			int sl = 0;
			int sq = 0;

			/* Not done yet */
			bool done = FALSE;

			flg &= ~(PROJECT_HIDE);

			by = y1;
			bx = x1;

			while (bdis <= dist + rad)
			{
				/* Travel from center outward */
				for (cdis = 0; cdis <= brad; cdis++)
				{
					/* Scan the maximal blast area of radius "cdis" */
					for (y = by - cdis; y <= by + cdis; y++)
					{
						for (x = bx - cdis; x <= bx + cdis; x++)
						{
							/* Ignore "illegal" locations */
							if (!in_bounds(y, x)) continue;

							/* Enforce a circular "ripple" */
							if (distance(y1, x1, y, x) != bdis) continue;

							/* Enforce an arc */
							if (distance(by, bx, y, x) != cdis) continue;

							/* The blast is stopped by walls */
							if (!in_ball_range(by, bx, y, x)) continue;

							/* Save this grid */
							gy[grids] = y;
							gx[grids] = x;
							grids++;
						}
					}
				}

				/* Encode some more "radius" info */
				gm[bdis + 1] = grids;

				/* Stop moving */
				if ((by == y2) && (bx == x2)) done = TRUE;

				/* Finish */
				if (done)
				{
					bdis++;
					continue;
				}

				/* Ripple outwards */
				mmove2(&by, &bx, y1, x1, y2, x2, &sl, &sq);

				/* Find the next ripple */
				bdis++;

				/* Increase the size */
				brad = (rad * bdis) / dist;
			}

			/* Store the effect size */
			gm_rad = bdis;
		}

		else
		{
			/* Determine the blast area, work from the inside out */
			for (dist = 0; dist <= rad; dist++)
			{
				/* Scan the maximal blast area of radius "dist" */
				for (y = y2 - dist; y <= y2 + dist; y++)
				{
					for (x = x2 - dist; x <= x2 + dist; x++)
					{
						/* Ignore "illegal" locations */
						if (!in_bounds2(y, x)) continue;

						/* Enforce a "circular" explosion */
						if (distance(y2, x2, y, x) != dist) continue;

						if (typ == GF_DISINTEGRATE)
						{
							/* Disintegration balls explosions are stopped by perma-walls */
							if (!in_disintegration_range(y2, x2, y, x)) continue;

							c_ptr = area(y, x);

							if (fields_have_flags(c_ptr->fld_idx, FIELD_INFO_PERM)) continue;

							/* Delete fields on the square */
							delete_field_location(c_ptr);

							if (cave_valid_grid(c_ptr) &&
								(c_ptr->feat < FEAT_PATTERN_START ||
								 c_ptr->feat > FEAT_PATTERN_XTRA2) &&
								(c_ptr->feat < FEAT_DEEP_WATER ||
								 c_ptr->feat > FEAT_GRASS))
							{
								if ((c_ptr->feat == FEAT_TREES) ||
									(c_ptr->feat == FEAT_PINE_TREE) ||
									(c_ptr->feat == FEAT_SNOW_TREE))
									cave_set_feat(y, x, FEAT_GRASS);
								else
									cave_set_feat(y, x, FEAT_FLOOR);
							}

							/* Update some things -- similar to GF_KILL_WALL */
							p_ptr->update |= (PU_VIEW | PU_FLOW
								 | PU_MONSTERS | PU_MON_LITE);
						}
						else
						{
							/* Ball explosions are stopped by walls/fields */
							if (!in_ball_range(y2, x2, y, x)) continue;
						}

						/* Save this grid */
						gy[grids] = y;
						gx[grids] = x;
						grids++;
					}
				}

				/* Encode some more "radius" info */
				gm[dist+1] = grids;
			}
		}
	}


	/* Speed -- ignore "non-explosions" */
	if (!grids) return (FALSE);


	/* Display the "blast area" if requested */
	if (!blind && !(flg & (PROJECT_HIDE)))
	{
		/* Then do the "blast", from inside out */
		for (t = 0; t <= gm_rad; t++)
		{
			/* Dump everything with this radius */
			for (i = gm[t]; i < gm[t+1]; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				c_ptr = area(y, x);

				/* Only do visuals if the player can "see" the blast */
				if (panel_contains(y, x) && player_has_los_grid(c_ptr))
				{
					byte a, c;

					drawn = TRUE;

					/* Obtain the explosion pict */
					bolt_pict(y, x, y, x, typ, &a, &c);

					/* Visual effects -- Display */
					print_rel(c, a, y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(y2, x2);

			/* Flush each "radius" seperately */
			if (fresh_before) Term_fresh();

			/* Delay (efficiently) */
			if (visual || drawn)
			{
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}

		/* Flush the erasing */
		if (drawn)
		{
			/* Erase the explosion drawn above */
			for (i = 0; i < grids; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				c_ptr = area(y, x);

				/* Hack -- Erase if needed */
				if (panel_contains(y, x) && player_has_los_grid(c_ptr))
				{
					lite_spot(y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(y2, x2);

			/* Flush the explosion */
			if (fresh_before) Term_fresh();
		}
	}

	/* Check features */
	if (flg & (PROJECT_GRID))
	{
		field_magic_target f_m_t;

		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for features */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, y2, x2);

				/* Affect the grid */
				if (project_f(who, d, y, x, dam, typ)) notice = TRUE;

				/* Store information into structure to pass to action */
				f_m_t.who = who;
				f_m_t.dist = d;
				f_m_t.dam = dam;
				f_m_t.typ = typ;
				f_m_t.notice = notice;
				f_m_t.known = player_can_see_bold(y, x);

				/* Affect fields on the grid */
				field_hook(&area(y, x)->fld_idx,
					FIELD_ACT_MAGIC_TARGET, (vptr) &f_m_t);

				/* Restore notice variable */
				notice = f_m_t.notice;
			}
			else
			{
				/* Affect the grid */
				if (project_f(who, dist, y, x, dam, typ)) notice = TRUE;

				/* Store information into structure to pass to action */
				f_m_t.who = who;
				f_m_t.dist = dist;
				f_m_t.dam = dam;
				f_m_t.typ = typ;
				f_m_t.notice = notice;
				f_m_t.known = player_can_see_bold(y, x);

				/* Affect fields on the grid */
				field_hook(&area(y, x)->fld_idx,
					FIELD_ACT_MAGIC_TARGET, (vptr) &f_m_t);

				/* Restore notice variable */
				notice = f_m_t.notice;
			}
		}
	}

	/* Update stuff if needed */
	if (p_ptr->update) update_stuff();


	/* Check objects */
	if (flg & (PROJECT_ITEM))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for objects */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, y2, x2);

				/* Affect the object in the grid */
				if (project_o(who, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the object in the grid */
				if (project_o(who, dist, y, x, dam, typ)) notice = TRUE;
			}
		}
	}


	/* Check monsters */
	if (flg & (PROJECT_KILL))
	{
		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for monsters */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist + 1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			if (grids > 1)
			{
				/* Find the closest point in the blast */
				if (breath)
				{
					int d = dist_to_line(y, x, y1, x1, y2, x2);

					/* Affect the monster in the grid */
					if (project_m(who, d, y, x, dam, typ)) notice = TRUE;
				}
				else
				{
					/* Affect the monster in the grid */
					if (project_m(who, dist, y, x, dam, typ)) notice = TRUE;
				}
			}
			else
			{
				monster_race *ref_ptr = &r_info[m_list[area(y,x)->m_idx].r_idx];

				if ((ref_ptr->flags2 & RF2_REFLECTING) && !one_in_(10) &&
					 (dist_hack > 1))
				{
					int t_y, t_x;
					int max_attempts = 10;

					/* Choose 'new' target */
					do
					{
						t_y = y_saver + rand_range(-1, 1);
						t_x = x_saver + rand_range(-1, 1);
						max_attempts--;
					}

					while (max_attempts && in_bounds2(t_y, t_x) &&
					    !(los(y, x, t_y, t_x)));

					if (max_attempts < 1)
					{
						t_y = y_saver;
						t_x = x_saver;
					}

					if (m_list[area(y,x)->m_idx].ml)
					{
						msg_print("The attack bounces!");
						ref_ptr->r_flags2 |= RF2_REFLECTING;
					}

					/* Recursion... */
					(void)project(area(y, x)->m_idx, 0, t_y, t_x,  dam, typ, flg);
				}
				else
				{
					if (project_m(who, dist, y, x, dam, typ)) notice = TRUE;
				}
			}
		}

		/* Player affected one monster (without "jumping") */
		if (!who && (project_m_n == 1) && !jump)
		{
			/* Location */
			x = project_m_x;
			y = project_m_y;

			/* Track if possible */
			if (area(y,x)->m_idx > 0)
			{
				monster_type *m_ptr = &m_list[area(y,x)->m_idx];

				/* Hack -- auto-recall */
				if (m_ptr->ml) monster_race_track(m_ptr->r_idx);

				/* Hack - auto-track */
				if (m_ptr->ml) health_track(area(y,x)->m_idx);
			}
		}
	}


	/* Check player */
	if (flg & (PROJECT_KILL))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for player */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, y2, x2);

				/* Affect the player */
				if (project_p(who, d, y, x, dam, typ, rad)) notice = TRUE;
			}
			else
			{
				/* Affect the player */
				if (project_p(who, dist, y, x, dam, typ, rad)) notice = TRUE;
			}
		}
	}

	if (mon_explode)
	{
		/*
		 * Run through the queue of monsters waiting to die,
		 * calling monster_death() for each.
		 *
		 * This prevents chain reactions of explosions from
		 * causing a stack smash, by only having one level
		 * of recursion.
		 *
		 * Monster_death() might call project() again - but
		 * if that happens, then mon_explode is FALSE, and
		 * it will fall back to us to process the monsters
		 * killed by that explosion.
		 */

		while (mon_d_tail != mon_d_head)
		{
			/* Generate treasure, etc */
			(void)monster_death(mon_d_m_idx[mon_d_tail], TRUE);

			/* Delete the monster */
			delete_monster_idx(mon_d_m_idx[mon_d_tail]);

			/*
			 * Increase mon_d_tail after the death.
			 * (This means that mon_explode will be FALSE in other
			 * calls to this function.)
			 */
			mon_d_tail++;

			/* Cycle back to the start */
			if (mon_d_tail >= DEATH_MAX) mon_d_tail = 0;
		}
	}

	/* Return "something was noticed" */
	return (notice);
}
