#include _WITCH_PATH(MEM/MEM.h)
#include _WITCH_PATH(VEC/VEC.h)

typedef struct{
	uintptr_t length;
	uint8_t *data;
}MAP_out_t;

typedef struct{
	uintptr_t length;
	uint8_t data[];
}MAP_subnode_t;

typedef struct{
	VEC_t raw;
}MAP_t;

void MAP_open(MAP_t *map){
	VEC_init(&map->raw, 1, A_resize);
}
void MAP_close(MAP_t* map){
	VEC_free(&map->raw);
}

MAP_subnode_t *MAP_query_in(MAP_t *map, uint8_t *in, uintptr_t in_size){
	uintptr_t i = 0;
	while((map->raw.Current - i) >= sizeof(MAP_subnode_t)){
		MAP_subnode_t *subnodein = (MAP_subnode_t *)((uint8_t *)map->raw.ptr + i);
		if(subnodein->length == in_size)
			if(MEM_cmp(in, subnodein->data, in_size))
				return subnodein;
		i += sizeof(MAP_subnode_t) + subnodein->length;
		MAP_subnode_t *subnodeout = (MAP_subnode_t *)((uint8_t *)map->raw.ptr + i);
		i += sizeof(MAP_subnode_t) + subnodeout->length;
	}
	return (MAP_subnode_t *)0;
}

void MAP_rm_node(MAP_t *map, MAP_subnode_t *subnode){
	uint8_t *src = (uint8_t *)subnode + sizeof(MAP_subnode_t) + subnode->length;
	src += sizeof(MAP_subnode_t) + ((MAP_subnode_t *)src)->length;
	uintptr_t size = ((uint8_t *)map->raw.ptr + map->raw.Current) - src;
	MEM_move(src, subnode, size);
	map->raw.Current -= src - (uint8_t *)subnode;
}

MAP_out_t MAP_out(MAP_t *map, uint8_t *in, uintptr_t in_size){
	MAP_subnode_t *query_in = MAP_query_in(map, in, in_size);
	if(!query_in){
		return WITCH_c(MAP_out_t){0, 0};
	}
	query_in = (MAP_subnode_t *)((uint8_t *)query_in + sizeof(MAP_subnode_t) + query_in->length);
	return WITCH_c(MAP_out_t){query_in->length, query_in->data};
}
#define MAP_out(map_m, in_m, in_size_m) \
	MAP_out((MAP_t *)(map_m), (uint8_t *)(in_m), (uintptr_t)(in_size_m))
#define MAP_out_cstr(map_m, in_m) \
	MAP_out((MAP_t *)(map_m), in_m, MEM_cstreu(in_m))

MAP_t *MAP_out_map(MAP_t* map, uint8_t* in, uintptr_t in_size){
	return (MAP_t *)MAP_out(map, in, in_size).data;
}
#define MAP_out_map(map_m, in_m, in_size_m) \
	MAP_out_map((MAP_t *)(map_m), (uint8_t *)(in_m), (uintptr_t)(in_size_m))
#define MAP_out_cstr_map(map_m, in_m) \
	MAP_out_map((MAP_t *)(map_m), in_m, MEM_cstreu(in_m))

void MAP_in(MAP_t *map, uint8_t *in, uintptr_t in_size, uint8_t *out, uintptr_t out_size){
	MAP_subnode_t *query_in = MAP_query_in(map, in, in_size);
	if(query_in){
		MAP_rm_node(map, query_in);
	}

	uintptr_t i = map->raw.Current;
	map->raw.Current += sizeof(MAP_subnode_t) + in_size + sizeof(MAP_subnode_t) + out_size;
	VEC_handle(&map->raw);

	query_in = (MAP_subnode_t *)&((uint8_t *)map->raw.ptr)[i];
	query_in->length = in_size;
	MEM_copy(in, query_in->data, in_size);

	i += sizeof(MAP_subnode_t) + in_size;
	query_in = (MAP_subnode_t *)&((uint8_t *)map->raw.ptr)[i];
	query_in->length = out_size;
	MEM_copy(out, query_in->data, out_size);
}
#define MAP_in(map_m, in_m, in_size_m, out_m, out_size_m) \
	MAP_in((MAP_t *)(map_m), (uint8_t *)(in_m), (uintptr_t)(in_size_m), (uint8_t *)(out_m), (uintptr_t)(out_size_m))
#define MAP_in_cstr(map_m, in_m, out_m, out_size_m) \
	MAP_in(map_m, in_m, MEM_cstreu(in_m), out_m, out_size_m)
#define MAP_in_cstr_out_cstr(map_m, in_m, out_m) \
	MAP_in(map_m, in_m, MEM_cstreu(in_m), out_m, MEM_cstreu(out_m))
#define MAP_in_cstr_out_pui(map_m, in_m, out_m) \
	MAP_in(map_m, in_m, MEM_cstreu(in_m), (out_m).ptr, (out_m).uint)
#define MAP_in_pui_out_pui(map_m, in_m, out_m) \
	MAP_in(map_m, (in_m).ptr, (in_m).uint, (out_m).ptr, (out_m).uint)
#define MAP_in_pui(map_m, in_m, out_m, out_size_m) \
	MAP_in(map_m, (in_m).ptr, (in_m).uint, out_m, out_size_m)

void MAP_in_out_map(MAP_t *map, uint8_t *in, uintptr_t in_size, MAP_t *out){
	MAP_in(map, in, in_size, out, sizeof(MAP_t));
}
#define MAP_in_out_map(map_m, in_m, in_size_m, out_m) \
	MAP_in_out_map((MAP_t *)(map_m), (uint8_t *)(in_m), (uintptr_t)(in_size_m), (MAP_t *)(out_m))
#define MAP_in_cstr_out_map(map_m, in_m, out_m) \
	MAP_in_out_map(map_m, in_m, MEM_cstreu(in_m), out_m)

void MAP_rm(MAP_t *map, uint8_t *in, uintptr_t in_size){
	MAP_subnode_t *query_in = MAP_query_in(map, in, in_size);
	if(query_in){
		MAP_rm_node(map, query_in);
	}
}
#define MAP_rm(map_m, in_m, in_size_m) \
	MAP_rm((MAP_t *)(map_m), (uint8_t *)(in_m), (uintptr_t)(in_size_m))
#define MAP_rm_cstr(map_m, in_m) \
	MAP_rm(map_m, in_m, MEM_cstreu(in_m))

uint8_t MAP_its(MAP_t *map){
	return !!map->raw.Current;
}

void MAP_dupe(MAP_t *src, MAP_t *dst){
	VEC_dupe(&src->raw, &dst->raw);
}

void MAP_move(MAP_t *src, MAP_t *dst){
	*dst = *src;
}

typedef struct{
	MAP_t *map;
	MAP_out_t in;
	MAP_out_t out;
	uintptr_t i;
}_MAP_traverse_t;
uint8_t MAP_traverse(_MAP_traverse_t *arg){
	if((arg->map->raw.Current - arg->i) < sizeof(MAP_subnode_t)){
		return 0;
	}

	MAP_subnode_t *subnodein = (MAP_subnode_t *)((uint8_t *)arg->map->raw.ptr + arg->i);
	arg->i += sizeof(MAP_subnode_t) + subnodein->length;
	MAP_subnode_t *subnodeout = (MAP_subnode_t *)((uint8_t *)arg->map->raw.ptr + arg->i);
	arg->i += sizeof(MAP_subnode_t) + subnodeout->length;

	arg->in = WITCH_c(MAP_out_t){subnodein->length, subnodein->data};
	arg->out = WITCH_c(MAP_out_t){subnodeout->length, subnodeout->data};

	return 1;
}
#define MAP_traverse(map_m) \
	for(_MAP_traverse_t mv = {.map = (MAP_t *)(map_m), .i = 0}; MAP_traverse(&mv);)
