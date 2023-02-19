#pragma once

/* MAP functions
	open = open map
	close = close map
	out = get output by input
	out_map = get map output by input
	in = register output to input (overwrite if already exists)
	in_out_map = same with in except it used for register set inside set
	rm = remove input (output will gone too)
	its = Is There Something, used for check if MAP has something inside
	dupe = dupe MAP and return duped MAP
	move = move MAP from srcvar to dstvar (its undefined behavior to use srcvar after move without init)
	traverse = get all input and output what MAP has in clambda
*/

/* algorithms
┌────lan────┬────lib────┬─────┐
│ num │  C  │ CPP │  C  │ CPP │
├─────┼─────┼─────┼─────┤─────┤
│  0  │  x  │     │     │     │
│─────┼─────┼─────┼─────┼─────│
│  1  │     │  x  │     │  x  │
└─────┴─────┴─────┴─────┴─────┘
*/

#include _WITCH_PATH(WITCH.h)

#ifndef MAP_set_backend
	#if WL_CPP && WITCH_LIBCPP
		#define MAP_set_backend 1
	#else
		#define MAP_set_backend 0
	#endif
#endif

#if MAP_set_backend == 0
	#include _WITCH_PATH(MAP/backend/0/0.h)
#elif MAP_set_backend == 1
	#include _WITCH_PATH(MAP/backend/1/1.h)
#else
	#error ?
#endif
