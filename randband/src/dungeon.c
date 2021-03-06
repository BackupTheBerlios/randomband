/* File: dungeon.c */

/* Purpose: Angband game engine */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

#define TY_CURSE_CHANCE 100
#define CHAINSWORD_NOISE 100


/*
 * Return a "feeling" (or NULL) about an item.  Method 1 (Heavy).
 */
static byte value_check_aux1(const object_type *o_ptr)
{
	/* Artifacts */
	if (o_ptr->flags3 & TR3_INSTA_ART)
	{
		/* Cursed/Broken */
		if (cursed_p(o_ptr) || broken_p(o_ptr)) return FEEL_TERRIBLE;

		/* Normal */
		return FEEL_SPECIAL;
	}

	/* Ego-Items */
	if (ego_item_p(o_ptr))
	{
		/* Cursed/Broken */
		if (o_ptr->flags3 & TR3_CURSED || broken_p(o_ptr))
		{
			return FEEL_WORTHLESS;
		}
		else if (cursed_p(o_ptr))
		{
			return FEEL_TAINTED;
		}

		/* Normal */
		return FEEL_EXCELLENT;
	}

	/* Broken items */
	if (broken_p(o_ptr)) return FEEL_BROKEN;

	/* Good bonus */
	if ((o_ptr->to_a > 0) || (o_ptr->to_h + o_ptr->to_d > 0))
	{
		/* Cursed good item? */
		if (cursed_p(o_ptr)) return FEEL_DUBIOUS;

		/* Normal good item */
		return FEEL_GOOD;
	}

	/* Cursed items */
	if (cursed_p(o_ptr)) return FEEL_CURSED;

	/* Worthless is "bad" */
	if (!object_value(o_ptr)) return FEEL_BAD;

	/* Default to "average" */
	return FEEL_AVERAGE;
}

/*
 * Return a "feeling" (or NULL) about an item.  Method 1 (Heavy).
 */
/*
 * Magic Stuff Check Heavy
 */

static byte value_check_aux1_1(const object_type *o_ptr)
{
   s32b TrueVal;

   TrueVal = 0;

   TrueVal = object_value_real(o_ptr);

	/* Artifacts */
	if (o_ptr->flags3 & TR3_INSTA_ART)
	{
		/* Cursed/Broken */
		if (cursed_p(o_ptr) || broken_p(o_ptr)) return FEEL_TERRIBLE;

		/* Normal */
		return FEEL_SPECIAL;
	}

	/* Cursed items */
	if (cursed_p(o_ptr)) return FEEL_CURSED;

	/* Worthless is "bad" */
	if (TrueVal < 1) return FEEL_BAD;

	/* Good bonus */
   if (TrueVal > 0 && TrueVal < 500) return FEEL_AVERAGE;

   if (TrueVal > 499 && TrueVal < 2500) return FEEL_GOOD;

   if (TrueVal > 2499) return FEEL_EXCELLENT;

	/* Default to "average" */
	return FEEL_AVERAGE;
}


/*
 * Return a "feeling" (or NULL) about an item.  Method 2 (Light).
 */
static byte value_check_aux2(const object_type *o_ptr)
{
	/* Cursed items (all of them) */
	if (cursed_p(o_ptr)) return FEEL_CURSED;

	/* Broken items (all of them) */
	if (broken_p(o_ptr)) return FEEL_BROKEN;

	/* Artifacts -- except cursed/broken ones */
	if (o_ptr->flags3 & TR3_INSTA_ART) return FEEL_GOOD;

	/* Ego-Items -- except cursed/broken ones */
	if (ego_item_p(o_ptr)) return FEEL_GOOD;

	/* Good armor bonus */
	if (o_ptr->to_a > 0) return FEEL_GOOD;

	/* Good weapon bonuses */
	if (o_ptr->to_h + o_ptr->to_d > 0) return FEEL_GOOD;

	/* Worthless is "bad" */
	if (!object_value(o_ptr)) return FEEL_BAD;

	/* No feeling */
	return FEEL_NONE;
}

/*
 * Return a "feeling" (or NULL) about an item.  Method 2 (Light).
 */
/*
 * Magic Stuff Check Light
 */
static byte value_check_aux2_1(const object_type *o_ptr)
{
   s32b TrueVal;

   TrueVal = 0;

   TrueVal = object_value_real(o_ptr);

   if (TrueVal < 1) return FEEL_BAD;

   if (TrueVal > 0 && TrueVal < 1000) return FEEL_AVERAGE;

   if (TrueVal > 999) return FEEL_GOOD;

	return FEEL_NONE;
}


/*
 * Sense the Magic Stuff (Completely made up)  :)
 */
static void sense_inventory2(void)
{
	int         i;
	int         plev = p_ptr->lev;
	bool        heavy = FALSE;
	byte        feel;
	object_type *o_ptr;
	char        o_name[80];

#ifdef USE_SCRIPT
	int         result;
#endif /* USE_SCRIPT */


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (p_ptr->confused) return;

#ifdef USE_SCRIPT

	result = sense_inventory_callback();

	if (result == -1)
		heavy = TRUE;

	if (!result)

#endif /* USE_SCRIPT */

	{
		/* Analyze the class */
		switch (p_ptr->pclass)
		{
			case CLASS_WARRIOR:
			{
				/* Good sensing */
				if (!one_in_(240000L / (plev + 5))) return;

				/* Done */
				break;
			}

			case CLASS_MAGE:
			case CLASS_HIGH_MAGE:
			{
				/* Very bad (light) sensing */
				if (!one_in_(9000L / (plev * plev + 40))) return;

				/* Heavy sensing */
				heavy = TRUE;

				break;
			}

			case CLASS_PRIEST:
			{
				/* Good (light) sensing */
				if (!one_in_(20000L / (plev * plev + 40))) return;

				/* Heavy sensing */
				heavy = TRUE;

				break;
			}

			case CLASS_ROGUE:
			{
				/* Okay sensing */
				if (!one_in_(15000L / (plev * plev + 40))) return;

				/* Done */
				break;
			}

			case CLASS_RANGER:
			{
				/* Bad sensing */
				if (!one_in_(40000L / (plev * plev + 40))) return;

				/* Changed! */
				heavy = TRUE;

				/* Done */
				break;
			}

			case CLASS_PALADIN:
			{
				/* Bad sensing */
				if (!one_in_(35353L / (plev * plev + 40))) return;

				/* Done */
				break;
			}

			case CLASS_WARRIOR_MAGE:
			{
				/* Bad sensing */
				if (!one_in_(37500L / (plev * plev + 40))) return;

				/* Done */
				break;
			}

			case CLASS_MINDCRAFTER:
			{
				/* Bad sensing */
				if (!one_in_(55000L / (plev * plev + 40))) return;

				/* Done */
				break;
			}

			case CLASS_CHAOS_WARRIOR:
			{
				/* Bad sensing */
				if (!one_in_(40000L / (plev * plev + 40))) return;

				/* Done */
				break;
			}

			case CLASS_MONK:
			{
				/* Okay sensing */
				if (!one_in_(20000L / (plev * plev + 40))) return;

				/* Heavy sensing */
				heavy = TRUE;

				/* Done */
				break;
			}

			case CLASS_CHI_WARRIOR:
			{
				/* Very bad (light) sensing */
				if (!one_in_(9000L / (plev * plev + 40))) return;

				/* Heavy sensing */
				heavy = TRUE;

				break;
			}
		}
	}

	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;

		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		switch (o_ptr->tval)
		{
			case TV_AMULET:
			case TV_RING:
			case TV_STAFF:
			case TV_WAND:
			case TV_ROD:
			case TV_SCROLL:
			case TV_POTION:
			{
				okay = TRUE;
				break;
			}
		}

		/* Skip non-sense machines */
		if (!okay) continue;

		/* We know about it already, do not tell us again */
		if (o_ptr->ident & (IDENT_SENSE)) continue;

		/* It is fully known, no information needed */
		if (object_known_p(o_ptr)) continue;

		/* Occasional failure on inventory items */
		if ((i < INVEN_WIELD) && !one_in_(5)) continue;

		/* Good luck */
		if ((p_ptr->muta3 & MUT3_GOOD_LUCK) && !one_in_(13))
		{
			heavy = TRUE;
		}

		/* Check for a feeling */
		feel = (heavy ? value_check_aux1_1(o_ptr) : value_check_aux2_1(o_ptr));

		/* Skip non-feelings */
		if (!feel) continue;

		/* Bad luck */
		if ((p_ptr->muta3 & MUT3_BAD_LUCK) && !one_in_(13))
		{
			switch (feel)
			{
				case FEEL_TERRIBLE:
				{
					feel = FEEL_SPECIAL;
					break;
				}
				case FEEL_WORTHLESS:
				{
					feel = FEEL_EXCELLENT;
					break;
				}
				case FEEL_CURSED:
				{
					feel = one_in_(3) ? FEEL_AVERAGE : FEEL_GOOD;
					break;
				}
				case FEEL_AVERAGE:
				{
					feel = one_in_(2) ? FEEL_BAD : FEEL_GOOD;
					break;
				}
				case FEEL_GOOD:
				case FEEL_BAD:
				{
					feel = one_in_(3) ? FEEL_AVERAGE : FEEL_CURSED;
					break;
				}
				case FEEL_EXCELLENT:
				{
					feel = FEEL_WORTHLESS;
					break;
				}
				case FEEL_SPECIAL:
				{
					feel = FEEL_TERRIBLE;
					break;
				}
			}
		}

		/* Stop everything */
		if (disturb_minor) disturb(FALSE);

		/* Get an object description */
		object_desc(o_name, o_ptr, FALSE, 0);

		/* Message (equipment) */
		if (i >= INVEN_WIELD)
		{
			msg_format("You feel the %s (%c) you are %s %s %s...",
						  o_name, index_to_label(i), describe_use(i),
						  ((o_ptr->number == 1) ? "is" : "are"),
						game_inscriptions[feel]);
		}

		/* Message (inventory) */
		else
		{
			msg_format("You feel the %s (%c) in your pack %s %s...",
			           o_name, index_to_label(i),
			           ((o_ptr->number == 1) ? "is" : "are"),
					   game_inscriptions[feel]);
		}

		/* We have "felt" it */
		o_ptr->ident |= (IDENT_SENSE);

		/* Set the "inscription" */
		o_ptr->feeling = feel;

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);
	}
}



/*
 * Sense the inventory
 *
 *   Class 0 = Warrior --> fast and heavy
 *   Class 1 = Mage    --> slow and light
 *   Class 2 = Priest  --> fast but light
 *   Class 3 = Rogue   --> okay and heavy
 *   Class 4 = Ranger  --> slow but heavy  (changed!)
 *   Class 5 = Paladin --> slow but heavy
 */
static void sense_inventory(void)
{
	int         i;
	int         plev = p_ptr->lev;
	bool        heavy = FALSE;
	byte        feel;
	object_type *o_ptr;
	char        o_name[80];

#ifdef USE_SCRIPT
	int         result;
#endif /* USE_SCRIPT */


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (p_ptr->confused) return;

#ifdef USE_SCRIPT

	result = sense_inventory_callback();

	if (result == -1)
		heavy = TRUE;

	if (!result)

#endif /* USE_SCRIPT */

	{
		/* Analyze the class */
		switch (p_ptr->pclass)
		{
			case CLASS_WARRIOR:
			{
				/* Good sensing */
				if (!one_in_(9000L / (plev * plev + 40))) return;

				/* Heavy sensing */
				heavy = TRUE;

				/* Done */
				break;
			}

			case CLASS_MAGE:
			case CLASS_HIGH_MAGE:
			{
				/* Very bad (light) sensing */
				if (!one_in_(240000L / (plev + 5))) return;

				/* Done */
				break;
			}

			case CLASS_PRIEST:
			{
				/* Good (light) sensing */
				if (!one_in_(10000L / (plev * plev + 40))) return;

				/* Done */
				break;
			}

			case CLASS_ROGUE:
			{
				/* Okay sensing */
				if (!one_in_(20000L / (plev * plev + 40))) return;

				/* Heavy sensing */
				heavy = TRUE;

				/* Done */
				break;
			}

			case CLASS_RANGER:
			{
				/* Bad sensing */
				if (!one_in_(95000L / (plev * plev + 40))) return;

				/* Changed! */
				heavy = TRUE;

				/* Done */
				break;
			}

			case CLASS_PALADIN:
			{
				/* Bad sensing */
				if (!one_in_(77777L / (plev * plev + 40))) return;

				/* Heavy sensing */
				heavy = TRUE;

				/* Done */
				break;
			}

			case CLASS_WARRIOR_MAGE:
			{
				/* Bad sensing */
				if (!one_in_(75000L / (plev * plev + 40))) return;

				/* Done */
				break;
			}

			case CLASS_MINDCRAFTER:
			{
				/* Bad sensing */
				if (!one_in_(55000L / (plev * plev + 40))) return;

				/* Done */
				break;
			}

			case CLASS_CHAOS_WARRIOR:
			{
				/* Bad sensing */
				if (!one_in_(80000L / (plev * plev + 40))) return;

				/* Changed! */
				heavy = TRUE;

				/* Done */
				break;
			}

			case CLASS_MONK:
			{
				/* Okay sensing */
				if (!one_in_(20000L / (plev * plev + 40))) return;

				/* Done */
				break;
			}

			case CLASS_CHI_WARRIOR:
			{
				/* Very bad (light) sensing */
				if (!one_in_(9000L / (plev * plev + 40))) return;

				/* Heavy sensing */
				heavy = TRUE;

				break;
			}
		}
	}


	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;

		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		switch (o_ptr->tval)
		{
			case TV_SHOT:
			case TV_ARROW:
			case TV_BOLT:
			case TV_BOW:
			case TV_DIGGING:
			case TV_HAFTED:
			case TV_POLEARM:
			case TV_SWORD:
			case TV_BOOTS:
			case TV_GLOVES:
			case TV_HELM:
			case TV_CROWN:
			case TV_SHIELD:
			case TV_CLOAK:
			case TV_SOFT_ARMOR:
			case TV_HARD_ARMOR:
			case TV_DRAG_ARMOR:
			{
				okay = TRUE;
				break;
			}
			case TV_FIGURINE:
			{
				if (!heavy)
					okay = TRUE;
				break;
			}
		}

		/* Skip non-sense machines */
		if (!okay) continue;

		/* We know about it already, do not tell us again */
		if (o_ptr->ident & (IDENT_SENSE)) continue;

		/* It is fully known, no information needed */
		if (object_known_p(o_ptr)) continue;

		/* Occasional failure on inventory items */
		if ((i < INVEN_WIELD) && !one_in_(5)) continue;

		/* Good luck */
		if ((p_ptr->muta3 & MUT3_GOOD_LUCK) && !one_in_(13))
		{
			heavy = TRUE;
		}

		/* Check for a feeling */
		feel = (heavy ? value_check_aux1(o_ptr) : value_check_aux2(o_ptr));

		/* Skip non-feelings */
		if (!feel) continue;

		/* Bad luck */
		if ((p_ptr->muta3 & MUT3_BAD_LUCK) && !one_in_(13))
		{
			switch (feel)
			{
				case FEEL_TERRIBLE:
				{
					feel = FEEL_SPECIAL;
					break;
				}
				case FEEL_WORTHLESS:
				{
					feel = FEEL_EXCELLENT;
					break;
				}
				case FEEL_CURSED:
				{
					feel = one_in_(3) ? FEEL_AVERAGE : FEEL_GOOD;
					break;
				}
				case FEEL_AVERAGE:
				{
					feel = one_in_(2) ? FEEL_BAD : FEEL_GOOD;
					break;
				}
				case FEEL_GOOD:
				case FEEL_BAD:
				{
					feel = one_in_(3) ? FEEL_AVERAGE : FEEL_CURSED;
					break;
				}
				case FEEL_EXCELLENT:
				{
					feel = FEEL_WORTHLESS;
					break;
				}
				case FEEL_SPECIAL:
				{
					feel = FEEL_TERRIBLE;
					break;
				}
			}
		}

		/* Stop everything */
		if (disturb_minor) disturb(FALSE);

		/* Get an object description */
		object_desc(o_name, o_ptr, FALSE, 0);

		/* Message (equipment) */
		if (i >= INVEN_WIELD)
		{
			msg_format("You feel the %s (%c) you are %s %s %s...",
						  o_name, index_to_label(i), describe_use(i),
						  ((o_ptr->number == 1) ? "is" : "are"),
						game_inscriptions[feel]);
		}

		/* Message (inventory) */
		else
		{
			msg_format("You feel the %s (%c) in your pack %s %s...",
			           o_name, index_to_label(i),
			           ((o_ptr->number == 1) ? "is" : "are"),
					   game_inscriptions[feel]);
		}

		/* We have "felt" it */
		o_ptr->ident |= (IDENT_SENSE);

		/* Set the "inscription" */
		o_ptr->feeling = feel;

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);
	}
}


/*
 * Go to any level (ripped off from wiz_jump)
 */
static void pattern_teleport(void)
{
	int min_level = 0;
	int max_level = 99;

	/* Ask for level */
	if (get_check("Teleport level? "))
	{
		char	ppp[80];
		char	tmp_val[160];

		/* Only downward in ironman mode */
		if (ironman_downward)
			min_level = p_ptr->depth;

		/* Maximum level */
		if (p_ptr->depth > 100)
			max_level = MAX_DEPTH - 1;
		else if (p_ptr->depth == 100)
			max_level = 100;

		/* Prompt */
		sprintf(ppp, "Teleport to level (%d-%d): ", min_level, max_level);

		/* Default */
		sprintf(tmp_val, "%d", p_ptr->depth);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 10)) return;

		/* Extract request */
		p_ptr->command_arg = atoi(tmp_val);
	}
	else if (get_check("Normal teleport? "))
	{
		teleport_player(200);
		return;
	}
	else
	{
		return;
	}

	/* Paranoia */
	if (p_ptr->command_arg < min_level) p_ptr->command_arg = min_level;

	/* Paranoia */
	if (p_ptr->command_arg > max_level) p_ptr->command_arg = max_level;

	/* Accept request */
	msg_format("You teleport to dungeon level %d.", p_ptr->command_arg);

	if (autosave_l) do_cmd_save_game(TRUE);

	/* Change level */
	p_ptr->depth = p_ptr->command_arg;

	/* Leaving */
	p_ptr->leaving = TRUE;
}


static void wreck_the_pattern(void)
{
	int px = p_ptr->px;
	int py = p_ptr->py;

	int to_ruin, r_y, r_x;

	if (area(py, px)->feat == FEAT_PATTERN_XTRA2)
	{
		/* Ruined already */
		return;
	}

	msg_print("You bleed on the Pattern!");
	msg_print("Something terrible happens!");

	if (!p_ptr->invuln)
		take_hit(damroll(10, 8), "corrupting the Pattern", TRUE);

	to_ruin = rand_range(35, 80);

	while (to_ruin--)
	{
		scatter(&r_y, &r_x, py, px, 4);

		if ((area(r_y, r_x)->feat >= FEAT_PATTERN_START) &&
		    (area(r_y, r_x)->feat < FEAT_PATTERN_XTRA2))
		{
			cave_set_feat(r_y, r_x, FEAT_PATTERN_XTRA2);
		}
	}

	cave_set_feat(py, px, FEAT_PATTERN_XTRA2);
}


/*
 * Returns TRUE if we are on the Pattern...
 */
static bool pattern_effect(void)
{
	cave_type *c_ptr = area(p_ptr->py, p_ptr->px);

	if ((c_ptr->feat < FEAT_PATTERN_START) ||
	    (c_ptr->feat > FEAT_PATTERN_XTRA2))
		return FALSE;

	if ((p_ptr->prace == RACE_AMBERITE) &&
	    (p_ptr->cut > 0) && one_in_(10))
	{
		wreck_the_pattern();
	}

	if (c_ptr->feat == FEAT_PATTERN_END)
	{
		(void)set_poisoned(0);
		(void)set_image(0);
		(void)set_stun(0);
		(void)set_cut(0);
		(void)set_blind(0);
		(void)set_afraid(0);
		(void)do_res_stat(A_STR, 200);
		(void)do_res_stat(A_INT, 200);
		(void)do_res_stat(A_WIS, 200);
		(void)do_res_stat(A_DEX, 200);
		(void)do_res_stat(A_CON, 200);
		(void)do_res_stat(A_CHR, 200);
		(void)restore_level();
		(void)hp_player(1000);
		c_ptr->feat = FEAT_PATTERN_OLD;
		msg_print("This section of the Pattern looks less powerful.");
	}


	/*
	 * We could make the healing effect of the
	 * Pattern center one-time only to avoid various kinds
	 * of abuse, like luring the win monster into fighting you
	 * in the middle of the pattern...
	 */

	else if (c_ptr->feat == FEAT_PATTERN_OLD)
	{
		/* No effect */
	}
	else if (c_ptr->feat == FEAT_PATTERN_XTRA1)
	{
		pattern_teleport();
	}
	else if (c_ptr->feat == FEAT_PATTERN_XTRA2)
	{
		if (!p_ptr->invuln)
		take_hit(200, "walking the corrupted Pattern", TRUE);
	}
	else
	{
		if ((p_ptr->prace == RACE_AMBERITE) && one_in_(2))
			return TRUE;
		else if (!p_ptr->invuln)
			take_hit(damroll(1, 3), "walking the Pattern", TRUE);
	}

	return TRUE;
}


/*
 * Regenerate hit points				-RAK-
 */
static void regenhp(int percent)
{
	u32b    new_chp, new_chp_frac;
	int     old_chp;

	/* Save the old hitpoints */
	old_chp = p_ptr->chp;

	/* Extract the new hitpoints */
	new_chp = ((u32b)p_ptr->mhp) * percent + PY_REGEN_HPBASE;
	p_ptr->chp += (s16b)(new_chp >> 16);   /* div 65536 */

	/* check for overflow */
	if ((p_ptr->chp < 0) && (old_chp > 0)) p_ptr->chp = MAX_SHORT;
	new_chp_frac = (new_chp & 0xFFFF) + p_ptr->chp_frac;	/* mod 65536 */
	if (new_chp_frac >= 0x10000L)
	{
		p_ptr->chp_frac = (u16b)(new_chp_frac - 0x10000L);
		p_ptr->chp++;
	}
	else
	{
		p_ptr->chp_frac = (u16b)new_chp_frac;
	}

	/* Fully healed */
	if (p_ptr->chp >= p_ptr->mhp)
	{
		p_ptr->chp = p_ptr->mhp;
		p_ptr->chp_frac = 0;
	}

	/* Notice changes */
	if (old_chp != p_ptr->chp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}
}


/*
 * Regenerate mana points				-RAK-
 */
static void regenmana(int percent)
{
	u32b        new_mana, new_mana_frac;
	int                   old_csp;

	old_csp = p_ptr->csp;
	new_mana = ((u32b)p_ptr->msp) * percent + PY_REGEN_MNBASE;
	p_ptr->csp += (s16b)(new_mana >> 16);	/* div 65536 */

	/* check for overflow */
	if ((p_ptr->csp < 0) && (old_csp > 0))
	{
		p_ptr->csp = MAX_SHORT;
	}
	new_mana_frac = (new_mana & 0xFFFF) + p_ptr->csp_frac;	/* mod 65536 */
	if (new_mana_frac >= 0x10000L)
	{
		p_ptr->csp_frac = (u16b)(new_mana_frac - 0x10000L);
		p_ptr->csp++;
	}
	else
	{
		p_ptr->csp_frac = (u16b)(new_mana_frac);
	}

	/* Must set frac to zero even if equal */
	if (p_ptr->csp >= p_ptr->msp)
	{
		p_ptr->csp = p_ptr->msp;
		p_ptr->csp_frac = 0;
	}

	/* Redraw mana */
	if (old_csp != p_ptr->csp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);
	}
}


/*
 * Regenerate the monsters (once per 100 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
static void regen_monsters(void)
{
	int i, frac;


	/* Regenerate everyone */
	for (i = 1; i < m_max; i++)
	{
		/* Check the i'th monster */
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];


		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Allow regeneration (if needed) */
		if (m_ptr->hp < m_ptr->maxhp)
		{
			/* Hack -- Base regeneration */
			frac = m_ptr->maxhp / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & RF2_REGENERATE) frac *= 2;

			/* Hack -- Regenerate */
			m_ptr->hp += frac;

			/* Do not over-regenerate */
			if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

			/* Redraw (later) if needed */
			if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);
		}
	}
}


void notice_lite_change(object_type *o_ptr)
{
	/* Hack -- notice interesting fuel steps */
	if ((o_ptr->timeout < 100) || (!(o_ptr->timeout % 100)))
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
	}

	/* Hack -- Special treatment when blind */
	if (p_ptr->blind)
	{
		/* Hack -- save some light for later */
		if (o_ptr->timeout == 0) o_ptr->timeout++;
	}

	/* The light is now out */
	else if (o_ptr->timeout == 0)
	{
		disturb(FALSE);
		msg_print("Your light has gone out!");
	}

	/* The light is getting dim */
	else if ((o_ptr->timeout < 100) && (!(o_ptr->timeout % 10)))
	{
		if (disturb_minor) disturb(FALSE);
		msg_print("Your light is growing faint.");
	}
}


/*
 * Forcibly pseudo-identify an object in the inventory
 * (or on the floor)
 *
 * note: currently this function allows pseudo-id of any object,
 * including silly ones like potions & scrolls, which always
 * get '{average}'. This should be changed, either to stop such
 * items from being pseudo-id'd, or to allow psychometry to
 * detect whether the unidentified potion/scroll/etc is
 * good (Cure Light Wounds, Restore Strength, etc) or
 * bad (Poison, Weakness etc) or 'useless' (Slime Mold Juice, etc).
 */
bool psychometry(void)
{
	int             item;
	object_type     *o_ptr;
	char            o_name[80];
	byte            feel;
	cptr            q, s;


	/* Get an item */
	q = "Meditate on which item? ";
	s = "You have nothing appropriate.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* It is fully known, no information needed */
	if (object_known_p(o_ptr))
	{
		msg_print("You cannot find out anything more about that.");
		return TRUE;
	}

	/* Check for a feeling */
	feel = value_check_aux1(o_ptr);

	/* Get an object description */
	object_desc(o_name, o_ptr, FALSE, 0);

	/* Skip non-feelings */
	if (!feel)
	{
		msg_format("You do not perceive anything unusual about the %s.", o_name);
		return TRUE;
	}

	msg_format("You feel that the %s %s %s...",
			   o_name, ((o_ptr->number == 1) ? "is" : "are"),
			   game_inscriptions[feel]);

	/* We have "felt" it */
	o_ptr->ident |= (IDENT_SENSE);

	/* "Inscribe" it */
	o_ptr->feeling = feel;

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER | PW_PLAYRES);

	/* Something happened */
	return (TRUE);
}


/*
 * If player has inscribed the object with "!!", let him know when it's
 * recharged. -LM-
 */
static void recharged_notice(const object_type *o_ptr)
{
	char o_name[80];

	cptr s;

	/* No inscription */
	if (!o_ptr->inscription) return;

	/* Find a '!' */
	s = strchr(quark_str(o_ptr->inscription), '!');

	/* Process notification request. */
	while (s)
	{
		/* Find another '!' */
		if (s[1] == '!')
		{
			/* Describe (briefly) */
			object_desc(o_name, o_ptr, FALSE, 0);

			/* Notify the player */
			if (o_ptr->number > 1)
				msg_format("Your %s are recharged.", o_name);
			else msg_format("Your %s is recharged.", o_name);

			/* Done. */
			return;
		}

		/* Keep looking for '!'s */
		s = strchr(s + 1, '!');
	}
}


/*
 * Handle certain things once every 10 game turns
 */
static void process_world(void)
{
	int i;
	s32b regen_amount;
	bool cave_no_regen = FALSE;
	int upkeep_factor = 0;

	u16b x, y;

	object_type *o_ptr;
	u32b f1 = 0 , f2 = 0 , f3 = 0, f4 = 0, f5 = 0, f6 = 0;
	int temp;
	object_kind *k_ptr;
	cave_type *c_ptr = area(p_ptr->py, p_ptr->px);
	const mutation_type *mut_ptr;

	/* Announce the level feeling */
	if ((turn - old_turn == 1000) && (p_ptr->depth)) do_cmd_feeling();

	/* Every 10 game turns */
	if (turn % 10) return;


	/*** Check the Time and Load ***/

	if (!(turn % 1000))
	{
		/* Check time and load */
		if ((0 != check_time()) || (0 != check_load()))
		{
			/* Warning */
			if (closing_flag <= 2)
			{
				/* Disturb */
				disturb(FALSE);

				/* Count warnings */
				closing_flag++;

				/* Message */
				msg_print("The gates to ANGBAND are closing...");
				msg_print("Please finish up and/or save your game.");
			}

			/* Slam the gate */
			else
			{
				/* Message */
				msg_print("The gates to ANGBAND are now closed.");

				/* Stop playing */
				p_ptr->playing = FALSE;

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
		}
	}

	/*** Attempt timed autosave ***/
	if (autosave_t && autosave_freq)
	{
		if (!(turn % ((s32b)autosave_freq * 10)))
			do_cmd_save_game(TRUE);
	}

	if (p_ptr->mon_fight)
	{
		msg_print("You hear noise.");
	}

	/*** Handle the wilderness/town (sunshine) ***/

	/* While in town/wilderness */
	if (!p_ptr->depth && !p_ptr->inside_quest)
	{
		/* Hack -- Daybreak/Nighfall in town */
		if (!(turn % ((10L * TOWN_DAWN) / 2)))
		{
			bool dawn;

			/* Check for dawn */
			dawn = (!(turn % (10L * TOWN_DAWN)));

			/* Day breaks */
			if (dawn)
			{
				/* Message */
				msg_print("The sun has risen.");

				/* Light up or darken the area */
				for (y = 0; y < WILD_GRID_SIZE; y++)
				{
					for (x = 0; x < WILD_GRID_SIZE; x++)
					{
						/* Lighten wilderness block */
						light_dark_block(wild_grid.block_ptr[y][x],
						 (u16b)(x + wild_grid.x_min / 16),
						  (u16b)(y + wild_grid.y_min / 16));
					}
				}
			}

			/* Night falls */
			else
			{
				/* Message */
				msg_print("The sun has fallen.");

				/* Light up or darken the area */
				for (y = 0; y < WILD_GRID_SIZE; y++)
				{
					for (x = 0; x < WILD_GRID_SIZE; x++)
					{
						/* Darken wilderness block */
						light_dark_block(wild_grid.block_ptr[y][x],
						 (u16b)(x + wild_grid.x_min / 16),
						  (u16b)(y + wild_grid.y_min / 16));
					}
				}
			}

			/* Update the monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}


	/*** Process the monsters ***/

	/* Check for creature generation. */
	if (one_in_(MAX_M_ALLOC_CHANCE) && !p_ptr->inside_quest)
	{
		/* Make a new monster */
		(void)alloc_monster(MAX_SIGHT + 5, FALSE);
	}

	/* Hack -- Check for creature regeneration */
	if (!(turn % 100)) regen_monsters();


	/*** Damage over Time ***/

	/* Take damage from poison */
	if (p_ptr->poisoned && !p_ptr->invuln)
	{
		/* Take damage */
		take_hit(1, "poison", FALSE);
	}


	/* (Vampires) Take damage from sunlight */
	if (p_ptr->prace == RACE_VAMPIRE)
	{
		if (!p_ptr->depth && !p_ptr->resist_lite && !p_ptr->invuln &&
		    (!((turn / ((10L * TOWN_DAWN) / 2)) % 2)))
		{
			if (c_ptr->info & CAVE_GLOW)
			{
				/* Take damage */
				msg_print("The sun's rays scorch your undead flesh!");
				take_hit(1, "sunlight", FALSE);
				cave_no_regen = TRUE;
			}
		}
	}

	if ((c_ptr->feat == FEAT_SHAL_LAVA) &&
		!p_ptr->invuln && !p_ptr->immune_fire && !p_ptr->ffall)
	{
		int damage = p_ptr->lev;

		if (p_ptr->resist_fire) damage = damage / 3;
		if (p_ptr->oppose_fire) damage = damage / 3;

		if (damage)
		{
			/* Take damage */
			msg_print("The lava burns you!");
			take_hit(damage, "shallow lava", TRUE);
			cave_no_regen = TRUE;
		}
	}

	else if ((c_ptr->feat == FEAT_DEEP_LAVA) &&
		!p_ptr->invuln && !p_ptr->immune_fire)
	{
		int damage = p_ptr->lev * 2;
		cptr message;
		cptr hit_from;

		if (p_ptr->resist_fire) damage = damage / 3;
		if (p_ptr->oppose_fire) damage = damage / 3;

		if (p_ptr->ffall)
		{
			damage = damage / 5;

			message = "The heat burns you!";
			hit_from = "flying over deep lava";
		}
		else
		{
			message = "The lava burns you!";
			hit_from = "deep lava";
		}

		if (damage)
		{
			/* Take damage */
			msg_print(message);
			take_hit(damage, hit_from, TRUE);

			cave_no_regen = TRUE;
		}
	}

	if ((c_ptr->feat == FEAT_SHAL_ACID) &&
		!p_ptr->invuln && !p_ptr->immune_acid && !p_ptr->ffall)
	{
		int damage = p_ptr->lev;

		if (p_ptr->resist_acid) damage = damage / 3;
		if (p_ptr->oppose_acid) damage = damage / 3;

		if (damage)
		{
			/* Take damage */
			msg_print("The acid burns you!");
			take_hit(damage, "shallow acid", TRUE);
			cave_no_regen = TRUE;
		}
	}

	else if ((c_ptr->feat == FEAT_DEEP_ACID) &&
		!p_ptr->invuln && !p_ptr->immune_acid)
	{
		int damage = p_ptr->lev * 2;
		cptr message;
		cptr hit_from;

		if (p_ptr->resist_acid) damage = damage / 3;
		if (p_ptr->oppose_acid) damage = damage / 3;

		if (p_ptr->ffall)
		{
			damage = damage / 5;

			message = "The fumes burn you!";
			hit_from = "flying over deep acid";
		}
		else
		{
			message = "The acid burns you!";
			hit_from = "deep acid";
		}

		if (damage)
		{
			/* Take damage */
			msg_print(message);
			take_hit(damage, hit_from, TRUE);

			cave_no_regen = TRUE;
		}
	}

	if ((c_ptr->feat == FEAT_SHAL_SWAMP) &&
		!p_ptr->invuln && !p_ptr->resist_pois && !p_ptr->ffall)
	{
		int damage = p_ptr->lev;

		if (p_ptr->oppose_pois) damage = damage / 3;

		if (damage)
		{
			/* Take damage */
			msg_print("The plants poison you!");
			take_hit(damage, "swamp", FALSE);
			cave_no_regen = TRUE;
		}
	}

	else if ((c_ptr->feat == FEAT_DEEP_SWAMP) &&
		!p_ptr->invuln)
	{
		int damage = p_ptr->lev * 2;
		cptr message;
		cptr hit_from;

		if (p_ptr->resist_pois) damage = damage / 3;
		if (p_ptr->oppose_pois) damage = damage / 3;

		if (p_ptr->ffall)
		{
			damage = damage / 5;

			message = "The fumes poison you!";
			hit_from = "flying over thick swamp";
		}
		else
		{
			message = "The fumes poison you!";
			hit_from = "thick swamp";
		}

		if (damage)
		{
			/* Take damage */
			msg_print(message);
			take_hit(damage, hit_from, FALSE);

			cave_no_regen = TRUE;
		}
	}

	else if (((c_ptr->feat == FEAT_DEEP_WATER) ||
		(c_ptr->feat == FEAT_OCEAN_WATER)) && !p_ptr->ffall)
	{
		if (p_ptr->total_weight > ((adj_str_wgt[p_ptr->stat_ind[A_STR]] * 100) / 2))
		{
			/* Take damage */
			msg_print("You are drowning!");
			take_hit(randint1(p_ptr->lev), "drowning", FALSE);
			cave_no_regen = TRUE;
		}
	}

	/* Spectres -- take damage when moving through walls */
	/*
	 * Added: ANYBODY takes damage if inside through walls
	 * without wraith form -- NOTE: Spectres will never be
	 * reduced below 0 hp by being inside a stone wall; others
	 * WILL BE!
	 */
	if (!cave_floor_grid(c_ptr))
	{
		/* Player can walk through semi-transparent terrain */
		if ((c_ptr->feat & 0x60) == 0x60)
		{
			/* Do nothing */
		}
		else if ((c_ptr->feat & 0x80) == 0x80)
		{
			/* Player can walk through the "slow floor" terrains. */

			/* Do nothing */
		}
		else if (!p_ptr->invuln && !p_ptr->wraith_form &&
		    ((p_ptr->chp > (p_ptr->lev / 5)) || !p_ptr->pass_wall))
		{
			cptr dam_desc;

			cave_no_regen = TRUE;

			if (p_ptr->pass_wall)
			{
				msg_print("Your molecules feel disrupted!");
				dam_desc = "density";
			}
			else
			{
				msg_print("You are being crushed!");
				dam_desc = "solid rock";
			}

			take_hit(1 + (p_ptr->lev / 5), dam_desc, FALSE);
		}
	}

	/*
	 * Fields you are standing on may do something.
	 */
	field_hook(&c_ptr->fld_idx, FIELD_ACT_PLAYER_ON, NULL);

	/* Nightmare mode activates the TY_CURSE at midnight */
	if (ironman_nightmare)
	{
		s32b len = 10L * TOWN_DAWN;
		s32b tick = turn % len + len / 4;

		int hour = (24 * tick / len) % 24;
		int min = (1440 * tick / len) % 60;
		int prev_min = (1440 * (tick - 10) / len) % 60;

		/* Require exact minute */
		if (min != prev_min)
		{
			/* Every 15 minutes after 11:00 pm */
			if ((hour == 23) && !(min % 15))
			{
				/* Disturbing */
				disturb(FALSE);

				switch (min / 15)
				{
					case 0:
					{
						msg_print("You hear a distant bell toll ominously.");
						break;
					}
					case 1:
					{
						msg_print("A distant bell sounds twice.");
						break;
					}
					case 2:
	{
						msg_print("A distant bell sounds three times.");
						break;
					}
					case 3:
					{
						msg_print("A distant bell tolls four times.");
						break;
					}
				}
			}

			/* TY_CURSE activates at mignight! */
			if (!hour && !min)
			{
				int count = 0;

				disturb(TRUE);
				msg_print("A distant bell tolls many times, fading into an deathly silence.");
				(void)activate_ty_curse(FALSE, &count);
			}
		}
	}

	/* Take damage from cuts */
	if (p_ptr->cut && !p_ptr->invuln)
	{
		/* Mortal wound or Deep Gash */
		if (p_ptr->cut > 200)
		{
			i = 3;
		}

		/* Severe cut */
		else if (p_ptr->cut > 100)
		{
			i = 2;
		}

		/* Other cuts */
		else
		{
			i = 1;
		}

		/* Take damage */
		take_hit(i, "a fatal wound", FALSE);
	}


	/*** Check the Food, and Regenerate ***/

	/* Digest normally */
	if (p_ptr->food < PY_FOOD_MAX)
	{
		/* Every 100 game turns */
		if (!(turn % 100))
		{
			/* Basic digestion rate based on speed */
			if (p_ptr->pspeed > 199) i = 49;
			else if (p_ptr->pspeed < 0) i = 1;
			else i = extract_energy[p_ptr->pspeed];

			i *= 2;

			/* Regeneration takes more food */
			if (p_ptr->regenerate) i += 30;

			/* Slow digestion takes less food */
			if (p_ptr->slow_digest) i -= 10;

			/* Minimal digestion */
			if (i < 1) i = 1;

			/* Digest some food */
			(void)set_food(p_ptr->food - i);
		}
	}

	/* Digest quickly when gorged */
	else
	{
		/* Digest a lot of food */
		(void)set_food(p_ptr->food - 100);
	}

	/* Starve to death (slowly) */
	if (p_ptr->food < PY_FOOD_STARVE)
	{
		/* Calculate damage */
		i = (PY_FOOD_STARVE - p_ptr->food) / 10;

		/* Take damage */
		if (!p_ptr->invuln) take_hit(i, "starvation", FALSE);
	}

	/* Default regeneration */
	regen_amount = PY_REGEN_NORMAL;

	/* Getting Weak */
	if (p_ptr->food < PY_FOOD_WEAK)
	{
		/* Lower regeneration */
		if (p_ptr->food < PY_FOOD_STARVE)
		{
			regen_amount = 0;
		}
		else if (p_ptr->food < PY_FOOD_FAINT)
		{
			regen_amount = PY_REGEN_FAINT;
		}
		else
		{
			regen_amount = PY_REGEN_WEAK;
		}

		/* Getting Faint */
		if (p_ptr->food < PY_FOOD_FAINT)
		{
			/* Faint occasionally */
			if (!p_ptr->paralyzed && (randint0(100) < 10))
			{
				/* Message */
				msg_print("You faint from the lack of food.");
				disturb(TRUE);

				/* Hack -- faint (bypass free action) */
				(void)set_paralyzed(p_ptr->paralyzed + randint1(5));
			}
		}
	}


	/* Are we walking the pattern? */
	if (pattern_effect())
	{
		cave_no_regen = TRUE;
	}
	else
	{
		/* Regeneration ability */
		if (p_ptr->regenerate)
		{
			regen_amount = regen_amount * 2;
		}
	}


	/* Searching or Resting */
	if (p_ptr->searching || p_ptr->resting)
	{
		regen_amount = regen_amount * 2;
	}

	if (total_friends)
	{
		if (total_friends > 1 + (p_ptr->lev / cp_ptr->pet_upkeep_div))
		{
			upkeep_factor = total_friend_levels;

			/* Bounds checking */
			if (upkeep_factor > 95) upkeep_factor = 95;
			if (upkeep_factor < 5) upkeep_factor = 5;
		}
	}

	/* Regenerate the mana */
	if (p_ptr->csp < p_ptr->msp)
	{
		if (upkeep_factor)
		{
			s16b upkeep_regen = (((100 - upkeep_factor) * regen_amount) / 100);
			regenmana(upkeep_regen);
		}
		else
		{
			regenmana(regen_amount);
		}
	}

	/* Poisoned or cut yields no healing */
	if (p_ptr->poisoned) regen_amount = 0;
	if (p_ptr->cut) regen_amount = 0;

	/* Special floor -- Pattern, in a wall -- yields no healing */
	if (cave_no_regen) regen_amount = 0;

	regen_amount = (regen_amount * mutant_regenerate_mod) / 100;

	/* Regenerate Hit Points */
	if (p_ptr->chp < p_ptr->mhp)
	{
		regenhp(regen_amount);
	}


	/*** Timeout Various Things ***/

	/* Hack -- Hallucinating */
	if (p_ptr->image)
	{
		(void)set_image(p_ptr->image - 1);
	}

	/* Blindness */
	if (p_ptr->blind)
	{
		(void)set_blind(p_ptr->blind - 1);
	}

	/* Times see-invisible */
	if (p_ptr->tim_invis)
	{
		(void)set_tim_invis(p_ptr->tim_invis - 1);
	}

	/* Timed esp */
	if (p_ptr->tim_esp)
	{
		(void)set_tim_esp(p_ptr->tim_esp - 1);
	}

	/* Timed infra-vision */
	if (p_ptr->tim_infra)
	{
		(void)set_tim_infra(p_ptr->tim_infra - 1);
	}

	/* Paralysis */
	if (p_ptr->paralyzed)
	{
		(void)set_paralyzed(p_ptr->paralyzed - 1);
	}

	/* Confusion */
	if (p_ptr->confused)
	{
		(void)set_confused(p_ptr->confused - 1);
	}

	/* Afraid */
	if (p_ptr->afraid)
	{
		(void)set_afraid(p_ptr->afraid - 1);
	}

	/* Fast */
	if (p_ptr->fast)
	{
		(void)set_fast(p_ptr->fast - 1);
	}

	/* Slow */
	if (p_ptr->slow)
	{
		(void)set_slow(p_ptr->slow - 1);
	}

	/* Protection from evil */
	if (p_ptr->protevil)
	{
		(void)set_protevil(p_ptr->protevil - 1);
	}

	/* Invulnerability */
	if (p_ptr->invuln)
	{
		(void)set_invuln(p_ptr->invuln - 1);
	}

	/* Wraith form */
	if (p_ptr->wraith_form)
	{
		(void)set_wraith_form(p_ptr->wraith_form - 1);
	}

	/* Heroism */
	if (p_ptr->hero)
	{
		(void)set_hero(p_ptr->hero - 1);
	}

	/* Super Heroism */
	if (p_ptr->shero)
	{
		(void)set_shero(p_ptr->shero - 1);
	}

	/* Blessed */
	if (p_ptr->blessed)
	{
		(void)set_blessed(p_ptr->blessed - 1);
	}

	/* Shield */
	if (p_ptr->shield)
	{
		(void)set_shield(p_ptr->shield - 1);
	}

	/* Oppose Acid */
	if (p_ptr->oppose_acid)
	{
		(void)set_oppose_acid(p_ptr->oppose_acid - 1);
	}

	/* Oppose Lightning */
	if (p_ptr->oppose_elec)
	{
		(void)set_oppose_elec(p_ptr->oppose_elec - 1);
	}

	/* Oppose Fire */
	if (p_ptr->oppose_fire)
	{
		(void)set_oppose_fire(p_ptr->oppose_fire - 1);
	}

	/* Oppose Cold */
	if (p_ptr->oppose_cold)
	{
		(void)set_oppose_cold(p_ptr->oppose_cold - 1);

	}

	/* Oppose Poison */
	if (p_ptr->oppose_pois)
	{
		(void)set_oppose_pois(p_ptr->oppose_pois - 1);
	}


	/*** Poison and Stun and Cut ***/

	/* Poison */
	if (p_ptr->poisoned)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Apply some healing */
		(void)set_poisoned(p_ptr->poisoned - adjust);
	}

	/* Stun */
	if (p_ptr->stun)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Apply some healing */
		(void)set_stun(p_ptr->stun - adjust);
	}

	/* Cut */
	if (p_ptr->cut)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Hack -- Truly "mortal" wound */
		if (p_ptr->cut > 1000) adjust = 0;

		/* Apply some healing */
		(void)set_cut(p_ptr->cut - adjust);
	}

	/*** Process mutation effects ***/
	for (i = MUT_PER_SET; i < MUT_PER_SET * 2; i++)
	{
		mut_ptr = &mutations[i];

		/*
		 * Do we have this mutation and
		 * is it truly a randomly activating one?
		 */
		if (player_has_mut(i) && (mut_ptr->chance > 0))
		{
			mutation_random_aux(mut_ptr);
		}
	}


	/*** Process Inventory ***/

	/* Handle experience draining */
	if (p_ptr->exp_drain)
	{
		if ((randint0(100) < 10) && (p_ptr->exp > 0))
		{
			p_ptr->exp--;
			p_ptr->max_exp--;
			check_experience();
		}
	}

	/* Process equipment */
	for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
	{
		/* Get the object */
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6);

		/* TY Curse */
		if ((f3 & TR3_TY_CURSE) && one_in_(TY_CURSE_CHANCE))
		{
			int count = 0;

			(void)activate_ty_curse(FALSE, &count);
		}

		/*
		 * Hack: Uncursed teleporting items (e.g. Trump Weapons)
		 * can actually be useful!
		 */
		if ((f3 & TR3_TELEPORT) && one_in_(100))
		{
			if (cursed_p(o_ptr) && !p_ptr->anti_tele)
			{
				disturb(FALSE);

				/* Teleport player */
				teleport_player(40);
			}
			else
			{
				if (!disturb_other || (o_ptr->inscription &&
				    (strchr(quark_str(o_ptr->inscription),'.'))))
				{
					/* Do nothing */
					/* msg_print("Teleport aborted.") */ ;
				}
				else if (get_check("Teleport? "))
				{
					disturb(FALSE);
					teleport_player(50);
				}
			}
		}

      /*  !!!GLOWSTONE HACK!!!  */
   	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval >= SV_YELLOW_GLOWSTONE))
		{
   		/* Calculate torch radius */
			p_ptr->update |= (PU_TORCH);
		}

		/* Recharge activatable objects */
		if (o_ptr->timeout > 0)
		{
			/* Recharge */
			o_ptr->timeout--;

			/* Lights are special */
			if ((o_ptr->tval == TV_LITE) && !(o_ptr->flags3 & TR3_LITE))
			{
				/* Notice interesting fuel steps */
				notice_lite_change(o_ptr);

				/* Calculate torch radius */
				p_ptr->update |= (PU_TORCH);
			}

			/* Notice changes */
			else if (!o_ptr->timeout)
			{
				recharged_notice(o_ptr);

				/* Window stuff */
				p_ptr->window |= (PW_EQUIP);
			}
		}
	}

	/*
	 * Recharge rods.  Rods now use timeout to control charging status,
	 * and each charging rod in a stack decreases the stack's timeout by
	 * one per turn. -LM-
	 */
	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
		k_ptr = &k_info[o_ptr->k_idx];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Must have a timeout */
		if (!o_ptr->timeout) continue;

		/* Examine all charging rods or stacks of charging rods. */
		if (o_ptr->tval == TV_ROD)
		{
			/* Determine how many rods are charging. */
			temp = (o_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval;
			if (temp > o_ptr->number) temp = o_ptr->number;

			/* Decrease timeout by that number. */
			o_ptr->timeout -= temp;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;

			/* Notice changes, provide message if object is inscribed. */
			if (temp > (o_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval)
			{
				recharged_notice(o_ptr);

				/* Combine pack */
				p_ptr->notice |= (PN_COMBINE);

				/* Window stuff */
				p_ptr->window |= (PW_INVEN);
			}
		}
	}

	/* Feel the inventory */
	sense_inventory();
	sense_inventory2();


	/*** Process Objects ***/

	/* Process objects */
	for (i = 1; i < o_max; i++)
	{
		/* Access object */
		o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Exit if not in dungeon */
		if (o_ptr->held_m_idx) continue;

		field_hook(&area(o_ptr->iy, o_ptr->ix)->fld_idx,
			 FIELD_ACT_OBJECT_ON, (vptr) o_ptr);

		if (!o_ptr->timeout) continue;

		/* Recharge rods on the ground.  No messages. */
		if (o_ptr->tval == TV_ROD)
		{
			/* Charge it */
			o_ptr->timeout -= o_ptr->number;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;
		}
	}

	/*
	* Cycle ultra-quick R"bool"G to prevent periodic patterns
	* in the illumination in a forest after dark.
	*/

	quick_rand_add();


	/*** Involuntary Movement ***/

	/* Delayed Word-of-Recall */
	if (p_ptr->word_recall)
	{
		/*
		 * HACK: Autosave BEFORE resetting the recall counter (rr9)
		 * The player is yanked up/down as soon as
		 * he loads the autosaved game.
		 */
		if (autosave_l && (p_ptr->word_recall == 1))
			do_cmd_save_game(TRUE);

		/* Count down towards recall */
		p_ptr->word_recall--;

		p_ptr->redraw |= (PR_STATUS);

		/* Activate the recall */
		if (!p_ptr->word_recall)
		{
			/* Disturbing! */
			disturb(FALSE);

			/* Determine the level */
			if (p_ptr->depth || p_ptr->inside_quest)
			{
				msg_print("You feel yourself yanked upwards!");

				p_ptr->depth = 0;

				leaving_quest = p_ptr->inside_quest;

				/* Leaving an 'only once' quest marks it as failed */
				if (leaving_quest &&
					(quest[leaving_quest].flags & QUEST_FLAG_ONCE) &&
					(quest[leaving_quest].status == QUEST_STATUS_TAKEN))
				{
					quest[leaving_quest].status = QUEST_STATUS_FAILED;
				}

				p_ptr->inside_quest = 0;
				p_ptr->leaving = TRUE;
			}
			else
			{
				msg_print("You feel yourself yanked downwards!");

				/* New depth */
				p_ptr->depth = p_ptr->max_depth;

				if (p_ptr->depth < 1) p_ptr->depth = 1;

				/* Nightmare mode makes recall more dangerous */
				if (ironman_nightmare && one_in_(666))
				{
					if (p_ptr->depth < 50)
					{
						p_ptr->depth *= 2;
					}
					else if (p_ptr->depth < 99)
					{
						p_ptr->depth = (p_ptr->depth + 99) / 2;
					}
					else if (p_ptr->depth > 100)
					{
						p_ptr->depth = MAX_DEPTH - 1;
					}
				}

				/* Leaving */
				p_ptr->leaving = TRUE;
			}

			/* Sound */
			sound(SOUND_TPLEVEL);
		}
	}
}



/*
 * Verify use of "wizard" mode
 */
static bool enter_wizard_mode(void)
{
	/* Ask first time */
#if 0
	if (!(p_ptr->noscore & 0x0002))
#else
	if (!p_ptr->noscore)
#endif
	{
		/* Mention effects */
		msg_print("Wizard mode is for debugging and experimenting.");
		msg_print("The game will not be scored if you enter wizard mode.");
		msg_print(NULL);

		/* Verify request */
		if (!get_check("Are you sure you want to enter wizard mode? "))
		{
			return (FALSE);
		}

		/* Mark savefile */
		p_ptr->noscore |= 0x0002;
	}

	/* Success */
	return (TRUE);
}


#ifdef ALLOW_WIZARD

/*
 * Verify use of "debug" commands
 */
static bool enter_debug_mode(void)
{
	/* Ask first time */
#if 0
	if (!(p_ptr->noscore & 0x0008))
#else
	if (!p_ptr->noscore)
#endif
	{
		/* Mention effects */
		msg_print("The debug commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use debug commands.");
		msg_print(NULL);

		/* Verify request */
		if (!get_check("Are you sure you want to use debug commands? "))
		{
			return (FALSE);
		}

		/* Mark savefile */
		p_ptr->noscore |= 0x0008;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Debug Routines
 */
extern void do_cmd_debug(void);

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

/*
 * Verify use of "borg" commands
 */
static bool enter_borg_mode(void)
{
	/* Ask first time */
	if (!(p_ptr->noscore & 0x0010))
	{
		/* Mention effects */
		msg_print("The borg commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use borg commands.");
		msg_print(NULL);

		/* Verify request */
		if (!get_check("Are you sure you want to use borg commands? "))
		{
			return (FALSE);
		}

		/* Mark savefile */
		p_ptr->noscore |= 0x0010;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Ben Borg
 */
extern void do_cmd_borg(void);

#endif /* ALLOW_BORG */



/*
 * Parse and execute the current command
 * Give "Warning" on illegal commands.
 *
 * XXX XXX XXX Make some "blocks"
 */
static void process_command(void)
{
	/* Handle repeating the last command */
	repeat_check();

#ifdef USE_SCRIPT
	if (process_command_callback(p_ptr->command_cmd))
		return;
#endif /* USE_SCRIPT */

	/* Parse the command */
	switch (p_ptr->command_cmd)
	{
		/* Ignore */
		case ESCAPE:
		case ' ':
		{
			break;
		}

		/* Ignore return */
		case '\r':
		{
			break;
		}

		/*** Wizard Commands ***/

		/* Toggle Wizard Mode */
		case KTRL('W'):
		{
			if (p_ptr->wizard)
			{
				p_ptr->wizard = FALSE;
				msg_print("Wizard mode off.");
			}
			else if (enter_wizard_mode())
			{
				p_ptr->wizard = TRUE;
				msg_print("Wizard mode on.");
			}

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Redraw "title" */
			p_ptr->redraw |= (PR_TITLE);

			break;
		}


#ifdef ALLOW_WIZARD

		/* Special "debug" commands */
		case KTRL('A'):
		{
			/* Enter debug mode */
			if (enter_debug_mode())
			{
				do_cmd_debug();
			}
			break;
		}

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

		/* Special "borg" commands */
		case KTRL('Z'):
		{
			/* Enter borg mode */
			if (enter_borg_mode())
			{
				do_cmd_borg();
			}

			break;
		}

#endif /* ALLOW_BORG */



		/*** Inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			do_cmd_wield();
			break;
		}

		/* Take off equipment */
		case 't':
		{
			do_cmd_takeoff();
			break;
		}

		/* Drop an item */
		case 'd':
		{
			do_cmd_drop();
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy();
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip();
			break;
		}

		/* Inventory list */
		case 'i':
		{
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
		case 'I':
		{
			do_cmd_observe();
			break;
		}

		/* Hack -- toggle windows */
		case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}


		/*** Standard "Movement" Commands ***/

		/* Alter a grid */
		case '+':
		{
			do_cmd_alter();
			break;
		}

		/* Dig a tunnel */
		case 'T':
		{
			do_cmd_tunnel();
			break;
		}

		/* Move (usually pick up things) */
		case ';':
		{
			do_cmd_walk(FALSE);
			break;
		}

		/* Move (usually do not pick up) */
		case '-':
		{
			do_cmd_walk(TRUE);
			break;
		}


		/*** Running, Resting, Searching, Staying */

		/* Begin Running -- Arg is Max Distance */
		case '.':
		{
			do_cmd_run();
			break;
		}

		/* Stay still (usually pick things up) */
		case ',':
		{
			do_cmd_stay(always_pickup);
			break;
		}

		/* Stay still (usually do not pick up) */
		case 'g':
		{
			do_cmd_stay(!always_pickup);
			break;
		}

		/* Rest -- Arg is time */
		case 'R':
		{
			do_cmd_rest();
			break;
		}

		/* Search for traps/doors */
		case 's':
		{
			do_cmd_search();
			break;
		}

		/* Toggle search mode */
		case 'S':
		{
			do_cmd_toggle_search();
			break;
		}


		/*** Stairs and Doors and Chests and Traps ***/

		/* Go up staircase */
		case '<':
		{
			do_cmd_go_up();
			break;
		}

		/* Go down staircase */
		case '>':
		{
			do_cmd_go_down();
			break;
		}

		/* Open a door or chest */
		case 'o':
		{
			do_cmd_open();
			break;
		}

		/* Close a door */
		case 'c':
		{
			do_cmd_close();
			break;
		}

		/* Jam a door with spikes */
		case 'j':
		{
			do_cmd_spike();
			break;
		}

		/* Disarm a trap or chest */
		case 'D':
		{
			do_cmd_disarm();
			break;
		}


		/*** Magic and Prayers ***/

		/* Gain new spells/prayers */
		case 'G':
		{
			do_cmd_study();
			break;
		}

		/* Browse a book */
		case 'b':
		{
			do_cmd_browse();
			break;
		}

		/* Cast a spell */
		case 'm':
		{

			if (p_ptr->anti_magic)
			{
				cptr which_power = "magic";
				if (p_ptr->pclass == CLASS_MINDCRAFTER)
					which_power = "psionic powers";
				else if (mp_ptr->spell_book == TV_SPELL_BOOK)
					which_power = "prayer";

				msg_format("An anti-magic shell disrupts your %s!", which_power);

				p_ptr->energy_use = 0;
			}
			else
			{
				if (p_ptr->pclass == CLASS_MINDCRAFTER)
					do_cmd_mindcraft();
				else
					do_cmd_cast();
			}

			break;
		}

		/* Issue a pet command */
		case 'p':
		{
			do_cmd_pet();
			break;
		}

		/*** Use various objects ***/

		/* Inscribe an object */
		case '{':
		{
			do_cmd_inscribe();
			break;
		}

		/* Uninscribe an object */
		case '}':
		{
			do_cmd_uninscribe();
			break;
		}

		/* Activate an artifact */
		case 'A':
		{
			do_cmd_activate();
			break;
		}

		/* Eat some food */
		case 'E':
		{
			do_cmd_eat_food();
			break;
		}

		/* Fuel your lantern/torch */
		case 'F':
		{
			do_cmd_refill();
			break;
		}

		/* Fire an item */
		case 'f':
		{
			do_cmd_fire();
			break;
		}

		/* Throw an item */
		case 'v':
		{
			do_cmd_throw();
			break;
		}

		/* Aim a wand */
		case 'a':
		{
			do_cmd_aim_wand();
			break;
		}

		/* Zap a rod */
		case 'z':
		{
			if (use_command && rogue_like_commands)
			{
				do_cmd_use();
			}
			else
			{
				do_cmd_zap_rod();
			}
			break;
		}

		/* Quaff a potion */
		case 'q':
		{
			do_cmd_quaff_potion();
			break;
		}

		/* Read a scroll */
		case 'r':
		{
			do_cmd_read_scroll();
			break;
		}

		/* Use a staff */
		case 'u':
		{
			if (use_command && !rogue_like_commands)
			{
				do_cmd_use();
			}
			else
			{
				do_cmd_use_staff();
			}
			break;
		}

		/* Use racial power */
		case 'U':
		{
			do_cmd_racial_power();
			break;
		}


		/*** Looking at Things (nearby or on map) ***/

		/* Full dungeon map */
		case 'M':
		{
			do_cmd_view_map();
			break;
		}

		/* Locate player on map */
		case 'L':
		{
			do_cmd_locate();
			break;
		}

		/* Look around */
		case 'l':
		{
			do_cmd_look();
			break;
		}

		/* Target monster or location */
		case '*':
		{
			do_cmd_target();
			break;
		}



		/*** Help and Such ***/

		/* Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

		/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol();
			break;
		}

		/* Character description */
		case 'C':
		{
			do_cmd_character();
			break;
		}


		/*** System Commands ***/

		/* Hack -- User interface */
		case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
		case '"':
		{
			do_cmd_pref();
			break;
		}

		/* Interact with macros */
		case '@':
		{
			do_cmd_macros();
			break;
		}

		/* Interact with visuals */
		case '%':
		{
			do_cmd_visuals();
			break;
		}

		/* Interact with colors */
		case '&':
		{
			do_cmd_colors();
			break;
		}

		/* Interact with options */
		case '=':
		{
			do_cmd_options(OPT_FLAG_SERVER | OPT_FLAG_PLAYER);
			do_cmd_redraw();
			break;
		}


		/*** Misc Commands ***/

		/* Take notes */
		case ':':
		{
			do_cmd_note();
			break;
		}

		/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
		case KTRL('F'):
		{
			do_cmd_feeling();
			break;
		}

		/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages();
			break;
		}

		/* Show quest status -KMW- */
		case KTRL('Q'):
		{
			do_cmd_checkquest();
			break;
		}

		/* Redraw the screen */
		case KTRL('R'):
		{
			do_cmd_redraw();
			break;
		}

#ifndef VERIFY_SAVEFILE

		/* Hack -- Save and don't quit */
		case KTRL('S'):
		{
			do_cmd_save_game(FALSE);
			break;
		}

#endif /* VERIFY_SAVEFILE */

		case KTRL('T'):
		{
			do_cmd_time();
			break;
		}

		/* Save and quit */
		case KTRL('X'):
		{
			do_cmd_save_and_exit();
			break;
		}

		/* Quit (commit suicide) */
		case 'Q':
		{
			do_cmd_suicide();
			break;
		}

		/* Check artifacts, uniques, objects */
		case '~':
		case '|':
		{
			do_cmd_knowledge();
			break;
		}

		/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

		/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen();
			break;
		}

		/* Hack -- Unknown command */
		default:
		{
			if (one_in_(2))
			{
				char error_m[1024];
				sound(SOUND_ILLEGAL);
				if (!get_rnd_line("error.txt", 0, error_m))
					msg_print(error_m);
			}
			else
				prt("Type '?' for help.", 0, 0);
			break;
		}
	}
}




/*
 * Process the player
 *
 * Notice the annoying code to handle "pack overflow", which
 * must come first just in case somebody manages to corrupt
 * the savefiles by clever use of menu commands or something.
 */
static void process_player(void)
{
	int i;

	if (hack_mutation)
	{
		msg_print("You feel different!");
		(void)gain_mutation(0);
		hack_mutation = FALSE;
	}

	/*** Check for interupts ***/

	/* Complete resting */
	if (p_ptr->resting < 0)
	{
		/* Basic resting */
		if (p_ptr->resting == -1)
		{
			/* Stop resting */
			if ((p_ptr->chp == p_ptr->mhp) &&
			    (p_ptr->csp >= p_ptr->msp))
			{
				disturb(FALSE);
			}
		}

		/* Complete resting */
		else if (p_ptr->resting == -2)
		{
			/* Stop resting */
			if ((p_ptr->chp == p_ptr->mhp) &&
			    (p_ptr->csp == p_ptr->msp) &&
			    !p_ptr->blind && !p_ptr->confused &&
			    !p_ptr->poisoned && !p_ptr->afraid &&
			    !p_ptr->stun && !p_ptr->cut &&
			    !p_ptr->slow && !p_ptr->paralyzed &&
			    !p_ptr->image && !p_ptr->word_recall)
			{
				disturb(FALSE);
			}
		}
	}

	/* Handle "abort" */
	if (!avoid_abort)
	{
		/* Check for "player abort" */
		if (p_ptr->running ||
		    p_ptr->command_rep ||
		    p_ptr->resting)
		{
			/* Do not wait */
			inkey_scan = TRUE;

			/* Check for a key */
			if (inkey())
			{
				/* Flush input */
				flush();

				/* Disturb */
				disturb(FALSE);

				/* Hack -- Show a Message */
				msg_print("Cancelled.");
			}
		}
	}


	/*** Handle actual user input ***/

	/* Repeat until energy is reduced */
	while (TRUE)
	{
		/* Notice stuff (if needed) */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff (if needed) */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff (if needed) */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff (if needed) */
		if (p_ptr->window) window_stuff();


		/* Place the cursor on the player */
		move_cursor_relative(p_ptr->py, p_ptr->px);

		/* Refresh (optional) */
		if (fresh_before) Term_fresh();


		/* Hack -- Pack Overflow */
		if (inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			char o_name[80];

			object_type *o_ptr;

			/* Access the slot to be dropped */
			o_ptr = &inventory[item];

			/* Disturbing */
			disturb(FALSE);

			/* Warning */
			msg_print("Your pack overflows!");

			/* Describe */
			object_desc(o_name, o_ptr, TRUE, 3);

			/* Message */
			msg_format("You drop %s (%c).", o_name, index_to_label(item));

			/* Drop it (carefully) near the player */
			(void)drop_near(o_ptr, 0, p_ptr->py, p_ptr->px);

			/* Modify, Describe, Optimize */
			inven_item_increase(item, -255);
			inven_item_describe(item);
			inven_item_optimize(item);

			/* Notice stuff (if needed) */
			if (p_ptr->notice) notice_stuff();

			/* Update stuff (if needed) */
			if (p_ptr->update) update_stuff();

			/* Redraw stuff (if needed) */
			if (p_ptr->redraw) redraw_stuff();

			/* Redraw stuff (if needed) */
			if (p_ptr->window) window_stuff();
		}


		/* Hack -- cancel "lurking browse mode" */
		if (!p_ptr->command_new) p_ptr->command_see = FALSE;


		/* Assume free turn */
		p_ptr->energy_use = 0;


		/* Paralyzed or Knocked Out */
		if (p_ptr->paralyzed || (p_ptr->stun >= 100))
		{
			/* Take a turn */
			p_ptr->energy_use = 100;
		}

		/* Resting */
		else if (p_ptr->resting)
		{
			/* Timed rest */
			if (p_ptr->resting > 0)
			{
				/* Reduce rest count */
				p_ptr->resting--;

				/* Redraw the state */
				p_ptr->redraw |= (PR_STATE);
			}

			/* Take a turn */
			p_ptr->energy_use = 100;
		}

		/* Running */
		else if (p_ptr->running)
		{
			/* Take a step */
			run_step(0);
		}

		/* Repeated command */
		else if (p_ptr->command_rep)
		{
			/* Count this execution */
			p_ptr->command_rep--;

			/* Redraw the state */
			p_ptr->redraw |= (PR_STATE);

			/* Redraw stuff */
			redraw_stuff();

			/* Hack -- Assume messages were seen */
			msg_flag = FALSE;

			/* Clear the top line */
			prt("", 0, 0);

			/* Process the command */
			process_command();
		}

		/* Normal command */
		else
		{
			/* Place the cursor on the player */
			move_cursor_relative(p_ptr->py, p_ptr->px);

			/* Get a command (normal) */
			request_command(FALSE);

			/* Process the command */
			process_command();
		}


		/*** Clean up ***/

		/* Significant */
		if (p_ptr->energy_use)
		{
			/* Use some energy */
			p_ptr->energy -= p_ptr->energy_use;


			/* Hack -- constant hallucination */
			if (p_ptr->image) p_ptr->redraw |= (PR_MAP);


			/* Shimmer monsters if needed */
			if (!avoid_other && shimmer_monsters)
			{
				/* Clear the flag */
				shimmer_monsters = FALSE;

				/* Shimmer multi-hued monsters */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					monster_race *r_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					/* Access the monster race */
					r_ptr = &r_info[m_ptr->r_idx];

					/* Skip non-multi-hued monsters */
					if (!(r_ptr->flags1 & RF1_ATTR_MULTI)) continue;

					/* Reset the flag */
					shimmer_monsters = TRUE;

					/* Redraw regardless */
					lite_spot(m_ptr->fy, m_ptr->fx);
				}
			}


			/* Handle monster detection */
			if (repair_monsters)
			{
				/* Reset the flag */
				repair_monsters = FALSE;

				/* Rotate detection flags */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					/* Nice monsters get mean */
					m_ptr->mflag &= ~(MFLAG_NICE);

					/* Handle memorized monsters */
					if (m_ptr->mflag & MFLAG_MARK)
					{
						/* Maintain detection */
						if (m_ptr->mflag & MFLAG_SHOW)
						{
							/* Forget flag */
							m_ptr->mflag &= ~(MFLAG_SHOW);

							/* Still need repairs */
							repair_monsters = TRUE;
						}

						/* Remove detection */
						else
						{
							/* Forget flag */
							m_ptr->mflag &= ~(MFLAG_MARK);

							/* Update the monster */
							update_mon(i, FALSE);
						}
					}
				}
			}
		}


		/* Hack -- notice death */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/* Used up energy for this turn */
		if (p_ptr->energy_use) break;
	}
}


/*
 * Add energy to player and monsters.
 * Those with the most energy move first.
 * (This prevents monsters like Morgoth getting double moves
 * when he is at a lower speed than the player.)
 */
static void process_energy(void)
{
	int i, speed, e;
	monster_type *m_ptr;

	/*** Apply energy to player ***/
	if (p_ptr->pspeed > 199) i = 49;
	else if (p_ptr->pspeed < 0) i = 1;
	else i = extract_energy[p_ptr->pspeed];

	p_ptr->energy += i;

	/* Give energy to all monsters */
	for (i = m_max - 1; i >= 1; i--)
	{
		/* Access the monster */
		m_ptr = &m_list[i];

		/* Ignore "dead" monsters */
		if (!m_ptr->r_idx) continue;

		speed = m_ptr->mspeed;

		/* Monsters move quickly in Nightmare mode */
		if (ironman_nightmare)
		{
			speed = MIN(199, m_ptr->mspeed + 5);
		}

		e = extract_energy[speed];

		/* Give this monster some energy */
		m_ptr->energy += e;
	}

	/* Can the player move? */
	while (p_ptr->energy >= 100 && !p_ptr->leaving)
	{
		/* process monster with even more energy first */
		process_monsters(p_ptr->energy + 1);

		/* Process the player while still alive */
		if (!p_ptr->leaving)
		{
			process_player();
		}
	}

	/* Process the fields */
	process_fields();
}


/*
 * Interact with the current dungeon level.
 *
 * This function will not exit until the level is completed,
 * the user dies, or the game is terminated.
 */
static void dungeon(void)
{
	int quest_num;

	cave_type *c_ptr;

	/* Set the base level */
	base_level = p_ptr->depth;

	/* Not leaving */
	p_ptr->leaving = FALSE;

	/* Reset the "command" vars */
	p_ptr->command_cmd = 0;
	p_ptr->command_new = 0;
	p_ptr->command_rep = 0;
	p_ptr->command_arg = 0;
	p_ptr->command_dir = 0;


	/* Cancel the target */
	p_ptr->target_who = 0;

	/* Cancel the health bar */
	health_track(0);


	/* Check visual effects */
	shimmer_monsters = TRUE;
	repair_monsters = TRUE;

	/* Disturb */
	disturb(TRUE);

	/* Get index of current quest (if any) */
	quest_num = quest_number(p_ptr->depth);

	/* Inside a quest? */
	if (quest_num)
	{
		/* Mark the quest monster */
		r_info[quest[quest_num].r_idx].flags1 |= RF1_QUESTOR;
	}

	/* Track maximum player level */
	if (p_ptr->max_lev < p_ptr->lev)
	{
		p_ptr->max_lev = p_ptr->lev;
	}


	/* Track maximum dungeon level (if not in quest -KMW-) */
	if ((p_ptr->max_depth < p_ptr->depth) && !p_ptr->inside_quest)
	{
		p_ptr->max_depth = p_ptr->depth;
	}

	/* No stairs down from Quest */
	if (quest_number(p_ptr->depth))
	{
		p_ptr->create_down_stair = FALSE;
	}

	/* Paranoia -- no stairs from town or wilderness */
	if (!p_ptr->depth) p_ptr->create_down_stair = p_ptr->create_up_stair = FALSE;

	/* Option -- no connected stairs */
	if (!dungeon_stair) p_ptr->create_down_stair = p_ptr->create_up_stair = FALSE;

	/* Nightmare mode is no fun... */
	if (ironman_nightmare) p_ptr->create_down_stair = p_ptr->create_up_stair = FALSE;

	/* Option -- no up stairs */
	if (ironman_downward) p_ptr->create_down_stair = p_ptr->create_up_stair = FALSE;

	/* Make a stairway. */
	if (p_ptr->create_up_stair || p_ptr->create_down_stair)
	{
		/* Place a stairway */
		c_ptr = area(p_ptr->py, p_ptr->px);
		if (cave_valid_grid(c_ptr))
		{
			/* XXX XXX XXX */
			delete_object(p_ptr->py, p_ptr->px);

			/* Make stairs */
			if (p_ptr->create_down_stair)
			{
				cave_set_feat(p_ptr->py, p_ptr->px, FEAT_MORE);
			}
			else
			{
				cave_set_feat(p_ptr->py, p_ptr->px, FEAT_LESS);
			}

			/* Mark the stairs as known */
			c_ptr->info |= (CAVE_MARK);
		}

		/* Cancel the stair request */
		p_ptr->create_down_stair = p_ptr->create_up_stair = FALSE;
	}


	/* Center the panel */
	panel_center();

	/* Flush messages */
	msg_print(NULL);


	/* Enter "xtra" mode */
	character_xtra = TRUE;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER | PW_PLAYRES);

	/* Window stuff */
	p_ptr->window |= (PW_MONSTER | PW_MESSAGE | PW_VISIBLE);

	/* Redraw dungeon */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Calculate torch radius */
	p_ptr->update |= (PU_TORCH);

	/* Update stuff */
	update_stuff();

	/* Redraw stuff */
	redraw_stuff();

	/* Redraw stuff */
	window_stuff();

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_FLOW | PU_DISTANCE | PU_MON_LITE);

	/* Update stuff */
	update_stuff();

	/* Redraw stuff */
	redraw_stuff();

	/* Leave "xtra" mode */
	character_xtra = FALSE;

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Combine / Reorder the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Notice stuff */
	notice_stuff();

	/* Update stuff */
	update_stuff();

	/* Redraw stuff */
	redraw_stuff();

	/* Window stuff */
	window_stuff();

	/* Refresh */
	Term_fresh();

	/* Hack -- notice death or departure */
	if (!p_ptr->playing || p_ptr->is_dead) return;

	/* Print quest message if appropriate */
	if (!p_ptr->inside_quest)
		quest_discovery(random_quest_number(p_ptr->depth));

	/*** Process this dungeon level ***/

	/* Reset the monster generation level */
	monster_level = base_level;

	/* Reset the object generation level */
	object_level = base_level;

	/* Main loop */
	while (TRUE)
	{
		int i;

		/* Hack -- Compact the monster list occasionally */
		if (m_cnt + 32 > max_m_idx) compact_monsters(64);

		/* Hack -- Compress the monster list occasionally */
		if (m_cnt + 32 < m_max) compact_monsters(0);


		/* Hack -- Compact the object list occasionally */
		if (o_cnt + 32 > max_o_idx) compact_objects(64);

		/* Hack -- Compress the object list occasionally */
		if (o_cnt + 32 < o_max) compact_objects(0);


		/* Hack -- Compact the field list occasionally */
		if (fld_cnt + 32 > max_fld_idx) compact_fields(64);

		/* Hack -- Compress the field list occasionally */
		if (fld_cnt + 32 < fld_max) compact_fields(0);

		/*
		 * Add energy to player and monsters.
		 * Those with the most energy move first.
		 */
		process_energy();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Similar slot? */
		for (i = 0; i < INVEN_PACK; i++)
		{
			object_type *j_ptr = &inventory[i];

			/* Skip non-objects */
			if (!j_ptr->k_idx) continue;
		}

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(p_ptr->py, p_ptr->px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Process all of the monsters */
		process_monsters(100);

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(p_ptr->py, p_ptr->px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/* Process the world */
		process_world();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Window stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(p_ptr->py, p_ptr->px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;


		/* Count game turns */
		turn++;
	}

	/* Inside a quest and non-unique questor? */
	if (quest_num && !(r_info[quest[quest_num].r_idx].flags1 & RF1_UNIQUE))
	{
		/* Un-mark the quest monster */
		r_info[quest[quest_num].r_idx].flags1 &= ~RF1_QUESTOR;
	}
}


/*
 * Load some "user pref files"
 *
 * Modified by Arcum Dagsson to support
 * separate macro files for different realms.
 */
static void load_all_pref_files(void)
{
	char buf[1024];

	/* Access the "race" pref file */
	sprintf(buf, "%s.prf", rp_ptr->title);

	/* Process that file */
	(void)process_pref_file(buf);

	/* Access the "class" pref file */
	sprintf(buf, "%s.prf", cp_ptr->title);

	/* Process that file */
	(void)process_pref_file(buf);

	/* Access the "character" pref file */
	sprintf(buf, "%s.prf", player_base);

	/* Process that file */
	(void)process_pref_file(buf);

	/* Access the "realm 1" pref file */
	if (p_ptr->realm1 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm1]);

		/* Process that file */
		(void)process_pref_file(buf);
	}

	/* Access the "realm 2" pref file */
	if (p_ptr->realm2 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm2]);

		/* Process that file */
		(void)process_pref_file(buf);
	}
}


/*
 * Actually play a game
 *
 * If the "new_game" parameter is true, then, after loading the
 * savefile, we will commit suicide, if necessary, to allow the
 * player to start a new game.
 */
void play_game(bool new_game)
{
	int i;

	hack_mutation = FALSE;

	/* Hack -- Character is "icky" */
	character_icky = TRUE;

   set_cost_matrix();

	/* Verify main term */
	if (!angband_term[0])
	{
		quit("main window does not exist");
	}

	/* Make sure main term is active */
	Term_activate(angband_term[0]);

	if (!angband_term[0]) quit("Main term does not exist!");

	/* Initialise the resize hooks */
	angband_term[0]->resize_hook = resize_map;

	for (i = 1; i < 8; i++)
	{
		/* Does the term exist? */
		if (angband_term[i])
		{
			/* Add the redraw on resize hook */
			angband_term[i]->resize_hook = redraw_window;
		}
	}

	/* Verify minimum size */
	if ((Term->hgt < 24) || (Term->wid < 80))
	{
		quit("main window is too small");
	}

	/* Hack -- turn off the cursor */
	(void)Term_set_cursor(0);

	/*
	 * Initialize wilderness info
	 * This needs to be done before old savefiles are loaded.
	 */
	if (init_w_info()) quit("Cannot initialize wilderness");

	/* Initialize field info */
	if (init_t_info()) quit("Cannot initialize fields");


	/* Attempt to load */
	if (!load_player())
	{
		/* Oops */
		quit("broken savefile");
	}

	/* Nothing loaded */
	if (!character_loaded)
	{
		/* Make new player */
		new_game = TRUE;

		/* The dungeon is not ready */
		character_dungeon = FALSE;
	}

	/* Hack -- Default base_name */
	if (!player_base[0])
	{
		strcpy(player_base, "PLAYER");
	}

	/* Init the RNG */
	if (Rand_quick || new_game)
	{
		u32b seed;

		/* Basic seed */
		seed = (time(NULL));

#ifdef SET_UID

		/* Mutate the seed on Unix machines */
		seed = ((seed >> 3) * (getpid() * 2));

#endif

		/* Use the complex RNG */
		Rand_quick = FALSE;

		/* Seed the "complex" RNG */
		Rand_state_init(seed);
	}

	/* Set or clear "rogue_like_commands" if requested */
	if (arg_force_original) rogue_like_commands = FALSE;
	if (arg_force_roguelike) rogue_like_commands = TRUE;

	/* Roll new character */
	if (new_game)
	{
		/* Roll up a new character */
		player_birth();

		/* Hack -- enter the world */
		if ((p_ptr->prace == RACE_VAMPIRE) ||
		    (p_ptr->prace == RACE_SKELETON) ||
		    (p_ptr->prace == RACE_ZOMBIE) ||
		    (p_ptr->prace == RACE_SPECTRE) ||
		    (p_ptr->prace == RACE_GHOUL))
		{
			/* Undead start just after midnight */
			turn = (30L * TOWN_DAWN) / 4 + 1;
		}
		else
		{
			turn = 1;
		}

		p_ptr->depth = 0;

		/* Create a new wilderness for the player */
		create_wilderness();

		/* Start in town */
		p_ptr->inside_quest = 0;

		/* Add monsters to the wilderness */
		repopulate_wilderness();

		/* The dungeon is ready */
		character_dungeon = TRUE;

		/* Hack -- seed for flavors */
		seed_flavor = randint0(0x10000000);
	}

	/* Reset the visual mappings */
	reset_visuals();

	/* Normal machine (process player name) */
	if (savefile[0])
	{
		process_player_name(FALSE);
	}

	/* Weird machine (process player name, pick savefile name) */
	else
	{
		process_player_name(TRUE);
	}

	/* Hack - if note file exists, load it */
	if (!new_game && take_notes)
	{
		add_note_type(NOTE_ENTER_DUNGEON);
	}

	/* Flash a message */
	prt("Please wait...", 0, 0);

	/* Flush the message */
	Term_fresh();


	/* Hack -- Enter wizard mode */
	if (arg_wizard && enter_wizard_mode()) p_ptr->wizard = TRUE;

	/* Flavor the objects */
	flavor_init();

	/* Load the "pref" files */
	load_all_pref_files();

	/*
	 * Set or clear "rogue_like_commands" if requested
	 * (Do it again, because of loading the pref files
	 *  can stomp on the options.)
	 */
	if (arg_force_original) rogue_like_commands = FALSE;
	if (arg_force_roguelike) rogue_like_commands = TRUE;

	/* Initialize vault info */
	if (init_v_info()) quit("Cannot initialize vaults");

	/* React to changes */
	Term_xtra(TERM_XTRA_REACT, 0);

	/* Generate a dungeon level if needed */
	if (!character_dungeon) generate_cave();


	/* Character is now "complete" */
	character_generated = TRUE;

	/* Hack -- Character is no longer "icky" */
	character_icky = FALSE;


	/* Start game */
	p_ptr->playing = TRUE;

	/* Hack -- Enforce "delayed death" */
	if (p_ptr->chp < 0) p_ptr->is_dead = TRUE;

	/* Resize / init the map */
	map_panel_size();

	/* Verify the (possibly resized) panel */
	verify_panel();

	/* Enter "xtra" mode */
	character_xtra = TRUE;

	/* Need to recalculate some transient things */
	p_ptr->update |= (PU_BONUS | PU_SPELLS);

	/* Update some stuff not stored in the savefile any more */
	p_ptr->update |= (PU_VIEW | PU_MON_LITE);

	/* Update stuff */
	update_stuff();

	/* Leave "xtra" mode */
	character_xtra = FALSE;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER | PW_PLAYRES);

	/* Window stuff */
	p_ptr->window |= (PW_MONSTER);

	/* Window stuff */
	window_stuff();

	/* Process */
	while (TRUE)
	{
		/* Process the level */
		dungeon();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Window stuff */
		if (p_ptr->window) window_stuff();


		/* Cancel the target */
		p_ptr->target_who = 0;

		/* Cancel the health bar */
		health_track(0);

		/* Forget the view */
		forget_view();

		/* Handle "quit and save" */
		if (!p_ptr->playing && !p_ptr->is_dead) break;

		/* Erase the old cave */
		wipe_o_list();
		wipe_m_list();
		wipe_f_list();

		change_level(p_ptr->depth);

		/* XXX XXX XXX */
		msg_print(NULL);

		/* Accidental Death */
		if (p_ptr->playing && p_ptr->is_dead)
		{
			/* Mega-Hack -- Allow player to cheat death */
			if ((p_ptr->wizard || cheat_live) && !get_check("Die? "))
			{
				/* Mark social class, reset age, if needed */
				if (p_ptr->sc) p_ptr->sc = p_ptr->age = 0;

				/* Increase age */
				p_ptr->age++;

				/* Mark savefile */
				p_ptr->noscore |= 0x0001;

				/* Message */
				msg_print("You invoke wizard mode and cheat death.");
				msg_print(NULL);

				/* Restore hit points */
				p_ptr->chp = p_ptr->mhp;
				p_ptr->chp_frac = 0;

				/* Restore spell points */
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;

				/* Hack -- Healing */
				(void)set_blind(0);
				(void)set_confused(0);
				(void)set_poisoned(0);
				(void)set_afraid(0);
				(void)set_paralyzed(0);
				(void)set_image(0);
				(void)set_stun(0);
				(void)set_cut(0);

				/* Hack -- Prevent starvation */
				(void)set_food(PY_FOOD_MAX - 1);

				/* Hack -- cancel recall */
				if (p_ptr->word_recall)
				{
					/* Message */
					msg_print("A tension leaves the air around you...");
					msg_print(NULL);

					/* Hack -- Prevent recall */
					p_ptr->word_recall = 0;
					p_ptr->redraw |= (PR_STATUS);
				}

				/* Note cause of death XXX XXX XXX */
				(void)strcpy(p_ptr->died_from, "Cheating death");

				/* Do not die */
				p_ptr->is_dead = FALSE;

				p_ptr->depth = 0;
				change_level(p_ptr->depth);

				leaving_quest = 0;
				p_ptr->inside_quest = 0;

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
		}

		/* Handle "death" */
		if (p_ptr->is_dead) break;

		/* Make a new level */
		generate_cave();
	}

	/* Close stuff */
	close_game();

	/* Quit */
	quit(NULL);
}
