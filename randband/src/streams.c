/*
 * File: streams.c
 * Purpose: Used by dungeon generation. This file holds all the
 * functions that are applied to a level after the rest has been
 * generated, ie streams and level destruction.
 */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#include "generate.h"
#include "streams.h"
#include "grid.h"


/*
 * Recursive fractal algorithm to place water through the dungeon.
 */
static void recursive_river(int x1, int y1, int x2, int y2, int feat1, int feat2, int width)
{
	int dx, dy, length, l, x, y;
	int changex, changey;
	int ty, tx;
	bool done;
	cave_type *c_ptr;

	length = distance(x1, y1, x2, y2);

	if (length > 4)
	{
		/*
		 * Divide path in half and call routine twice.
		 * There is a small chance of splitting the river
		 */
		dx = (x2 - x1) / 2;
		dy = (y2 - y1) / 2;

		if (dy != 0)
		{
			/* perturbation perpendicular to path */
			changex = randint1(ABS(dy)) * 2 - ABS(dy);
		}
		else
		{
			changex = 0;
		}

		if (dx != 0)
		{
			/* perturbation perpendicular to path */
			changey = randint1(ABS(dx)) * 2 - ABS(dx);
		}
		else
		{
			changey = 0;
		}

		if (!in_bounds(y1 + dy + changey, x1 + dx + changex))
		{
			changex = 0;
			changey = 0;
		}

		/* construct river out of two smaller ones */
		recursive_river(x1, y1, x1 + dx + changex, y1 + dy + changey, feat1, feat2, width);
		recursive_river(x1 + dx + changex, y1 + dy + changey, x2, y2, feat1, feat2, width);

		/* Split the river some of the time - junctions look cool */
		if (one_in_(DUN_WAT_CHG) && (width > 0))
		{
			recursive_river(x1 + dx + changex, y1 + dy + changey,
			                x1 + 8 * (dx + changex), y1 + 8 * (dy + changey),
			                feat1, feat2, width - 1);
		}
	}
	else
	{
		/* Actually build the river */
		for (l = 0; l < length; l++)
		{
			x = x1 + l * (x2 - x1) / length;
			y = y1 + l * (y2 - y1) / length;

			done = FALSE;

			while (!done)
			{
				for (ty = y - width - 1; ty <= y + width + 1; ty++)
				{
					for (tx = x - width - 1; tx <= x + width + 1; tx++)
					{
						if (!in_bounds(ty, tx)) continue;

						c_ptr = &cave[ty][tx];

						if (c_ptr->feat == feat1) continue;
						if (c_ptr->feat == feat2) continue;

						if (distance(ty, tx, y, x) > rand_spread(width, 1)) continue;

						/* Do not convert permanent features */
						if (cave_perma_grid(c_ptr)) continue;

						/* Making a door on top of fields is problematical */
						delete_field_location(c_ptr);
						
						/*
						 * Clear previous contents, add feature
						 * The border mainly gets feat2, while the center gets feat1
						 */
						if (distance(ty, tx, y, x) > width)
							c_ptr->feat = feat2;
						else
							c_ptr->feat = feat1;

						/* Lava terrain glows */
						if ((feat1 == FEAT_DEEP_LAVA) ||  (feat1 == FEAT_SHAL_LAVA))
						{
							c_ptr->info |= CAVE_GLOW;
						}

						/* Hack -- don't teleport here */
						c_ptr->info |= CAVE_ICKY;
					}
				}

				done = TRUE;
			}
		}
	}
}


/*
 * Places water /lava through dungeon.
 */
void add_river(int feat1, int feat2)
{
	int y2, x2;
	int y1 = 0, x1 = 0;
	int wid;


	/* Hack -- Choose starting point */
	y2 = rand_range(min_hgt + 1, max_hgt - 2);
	x2 = rand_range(min_wid + 1, max_wid - 2);

	/* Hack -- Choose ending point somewhere on boundary */
	switch (randint1(4))
	{
		case 1:
		{
			/* top boundary */
			x1 = rand_range(min_wid + 1, max_wid - 2);
			y1 = min_hgt + 1;
			break;
		}
		case 2:
		{
			/* left boundary */
			x1 = min_wid + 1;
			y1 = rand_range(min_hgt + 1, max_hgt - 2);
			break;
		}
		case 3:
		{
			/* right boundary */
			x1 = max_wid - 2;
			y1 = rand_range(min_hgt + 1, max_hgt - 2);
			break;
		}
		case 4:
		{
			/* bottom boundary */
			x1 = rand_range(min_wid + 1, max_wid - 2);
			y1 = max_hgt - 2;
			break;
		}
	}

	wid = randint1(DUN_WAT_RNG);
	recursive_river(x1, y1, x2, y2, feat1, feat2, wid);

	/* Hack - Save the location as a "room" */
	if (dun->cent_n < CENT_MAX)
	{
		dun->cent[dun->cent_n].y = y2;
		dun->cent[dun->cent_n].x = x2;
		dun->cent_n++;
	}
}


/*
 * Places "streamers" of rock through dungeon
 *
 * Note that their are actually six different terrain features used
 * to represent streamers.  Three each of magma and quartz, one for
 * basic vein, one with hidden gold, and one with known gold.  The
 * hidden gold types are currently unused.
 */
void build_streamer(int feat, int chance)
{
	int		i, tx, ty;
	int		y, x, dir;
	int dummy = 0;

	cave_type *c_ptr;

	/* Hack -- Choose starting point */
	y = rand_spread(max_hgt / 2, (max_hgt / 2 > 10? 10: max_hgt / 2));
	x = rand_spread(max_wid / 2, (max_wid / 2 > 15? 15: max_wid / 2));

	/* Choose a random compass direction */
	dir = ddd[randint0(8)];

	/* Place streamer into dungeon */
	while (dummy < SAFE_MAX_ATTEMPTS)
	{
		dummy++;

		/* One grid per density */
		for (i = 0; i < DUN_STR_DEN; i++)
		{
			int d = DUN_STR_RNG;

			/* Pick a nearby grid */
			while (1)
			{
				ty = rand_spread(y, d);
				tx = rand_spread(x, d);
				if (!in_bounds2(ty, tx)) continue;
				break;
			}

			/* Access the grid */
			c_ptr = &cave[ty][tx];

			/* Only convert "granite" walls */
			if (c_ptr->feat < FEAT_WALL_EXTRA) continue;
			if (c_ptr->feat > FEAT_WALL_SOLID) continue;

			/* Clear previous contents, add proper vein type */
			c_ptr->feat = feat;

			/* Hack -- Add some (known) treasure */
			if (one_in_(chance)) c_ptr->feat += 0x04;
			
			/*
			 * So this means that all the treasure is known as soon as it is
			 * seen or detected...  Why do the FEAT_MAGMA_H and FEAT_QUARTZ_H
			 * terrain types exist?  If they are never made, then the "mimic"
			 * feature struct field can be removed, and so can some code in
			 * map_info() - which will speed the game up significantly.
			 */
		}

		if (dummy >= SAFE_MAX_ATTEMPTS)
		{
			if (cheat_room)
			{
				msg_print("Warning! Could not place streamer!");
			}
			return;
		}


		/* Advance the streamer */
		y += ddy[dir];
		x += ddx[dir];

		/* Quit before leaving the dungeon */
		if (!in_bounds(y, x)) break;
	}
}


/*
 * Put trees near a hole in the dungeon roof  (rubble on ground + up stairway)
 * This happens in real world lava tubes.
 */
void place_trees(int x, int y)
{
	int i, j;
	cave_type *c_ptr;

	/* place trees/ rubble in ovalish distribution */
	for (i = x - 3; i < x + 4; i++)
	{
		for (j = y - 3; j < y + 4; j++)
		{
			c_ptr = &cave[j][i];

			/* Want square to be in the circle and accessable. */
			if (in_bounds(j, i) && (distance(j, i, y, x) < 4) && !cave_perma_grid(c_ptr))
			{
				/* Adding to grids with fields is problematical */
				delete_field_location(c_ptr);
				
				/*
				 * Clear previous contents, add feature
				 * The border mainly gets trees, while the center gets rubble
				 */
				if ((distance(j, i, y, x) > 1) || one_in_(4))
				{
					if (randint1(100) < 75)
						cave[j][i].feat = FEAT_TREES;
				}
				else
				{
					cave[j][i].feat = FEAT_RUBBLE;
				}

				/* Light area since is open above */
				cave[j][i].info |= (CAVE_GLOW | CAVE_ROOM);
			}
		}
	}

	/* No up stairs in ironman mode */
	if (!ironman_downward && one_in_(3))
	{
		/* up stair */
		cave[y][x].feat = FEAT_LESS;
	}

	/* Hack - Save the location as a "room" */
	if (dun->cent_n < CENT_MAX)
	{
		dun->cent[dun->cent_n].y = y;
		dun->cent[dun->cent_n].x = x;
		dun->cent_n++;
	}
}


/*
 * Build a destroyed level
 */
void destroy_level(void)
{
	int y1, x1, y, x, k, t, n;

	cave_type *c_ptr;

	/* Note destroyed levels */
	if (cheat_room) msg_print("Destroyed Level");

	/* Drop a few epi-centers (usually about two) */
	for (n = 0; n < randint1(5); n++)
	{
		/* Pick an epi-center */
		x1 = rand_range(min_wid + 5, max_wid - 1 - 5);
		y1 = rand_range(min_hgt + 5, max_hgt - 1 - 5);

		/* Big area of affect */
		for (y = (y1 - 15); y <= (y1 + 15); y++)
		{
			for (x = (x1 - 15); x <= (x1 + 15); x++)
			{
				/* Skip illegal grids */
				if (!in_bounds(y, x)) continue;

				/* Extract the distance */
				k = distance(y1, x1, y, x);

				/* Stay in the circle of death */
				if (k >= 16) continue;

				/* Delete the monster (if any) */
				delete_monster(y, x);

				/* Access the grid */
				c_ptr = &cave[y][x];

				/* Destroy valid grids */
				if (cave_valid_grid(c_ptr))
				{
					/* Delete objects */
					delete_object(y, x);
					
					/* Delete all fields */
					delete_field_location(c_ptr);

					/* Wall (or floor) type */
					t = randint0(200);

					/* Granite */
					if (t < 20)
					{
						/* Create granite wall */
						c_ptr->feat = FEAT_WALL_EXTRA;
					}

					/* Quartz */
					else if (t < 70)
					{
						/* Create quartz vein */
						c_ptr->feat = FEAT_QUARTZ;
					}

					/* Magma */
					else if (t < 100)
					{
						/* Create magma vein */
						c_ptr->feat = FEAT_MAGMA;
					}

					/* Floor */
					else
					{
						/* Create floor */
						c_ptr->feat = FEAT_FLOOR;
					}

					/* No longer part of a room or vault */
					c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

					/* No longer illuminated or known */
					c_ptr->info &= ~(CAVE_MARK | CAVE_GLOW);
				}
			}
		}
	}
}


/*
 * Builds a cave system in the center of the dungeon.
 */
void build_cavern(void)
{
	int grd, roug, cutoff, xsize, ysize, x0, y0;
	bool done;

	done = FALSE;

	/* Make a cave the size of the dungeon */
	xsize = max_wid - 1;
	ysize = max_hgt - 1;
	x0 = xsize / 2;
	y0 = ysize / 2;

	/* Paranoia: make size even */
	xsize = x0 * 2;
	ysize = y0 * 2;

	while (!done)
	{
		/* testing values for these parameters: feel free to adjust */
		grd = rand_range(4, 8);

		/* want average of about 16 */
		roug = randint1(8) * randint1(4);

		/* about size/2 */
		cutoff = xsize / 2;

		 /* make it */
		generate_hmap(y0 + 1, x0 + 1, xsize, ysize, grd, roug, cutoff);

		/* Convert to normal format+ clean up */
		done = generate_lake(y0 + 1, x0 + 1, xsize, ysize,
			 cutoff, cutoff, cutoff, LAKE_CAVERN);
	}
}

/*
 * makes a lake/collapsed cave system in the center of the dungeon
 */
void build_lake(int type)
{
	int grd, roug, xsize, ysize, x0, y0;
	bool done = FALSE;
	int c1, c2, c3;

	/* paranoia - exit if lake type out of range. */
	if ((type < 1) || (type > 7))
	{
		msg_format("Invalid lake type (%d)", type);
		return;
	}

	/* Make the size of the dungeon */
	xsize = max_wid - 1;
	ysize = max_hgt - 1;
	x0 = xsize / 2;
	y0 = ysize / 2;

	/* Paranoia: make size even */
	xsize = x0 * 2;
	ysize = y0 * 2;

	while (!done)
	{
		/* testing values for these parameters: feel free to adjust */
		grd = rand_range(3, 7);

		/* want average of about 16 */
		roug = randint1(8) * randint1(4);

		/* Make up size of various componants */
		/* Floor */
		c3 = 3 * xsize / 4;

		/* Deep water/lava */
		c1 = randint0(c3 / 2) + randint0(c3 / 2) - 5;

		/* Shallow boundary */
		c2 = (c1 + c3) / 2;

		/* make it */
		generate_hmap(y0 + 1, x0 + 1, xsize, ysize, grd, roug, c3);

		/* Convert to normal format+ clean up */
		done = generate_lake(y0 + 1, x0 + 1, xsize, ysize, c1, c2, c3, type);
	}
}
