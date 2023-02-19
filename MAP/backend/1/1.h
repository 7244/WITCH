#if WL_CPP == 0
	#error MAP_set_backend 1 needs WL_CPP > 0
#endif
#if WITCH_LIBCPP == 0
	#error MAP_set_backend 1 needs WITCH_LIBCPP > 0
#endif

#include _WITCH_PATH(MEM/MEM.h)

#include <vector>
#include <unordered_map>
#include <string>

typedef struct{
	uintptr_t length;
	uint8_t *data;
}MAP_out_t;

typedef struct{
	std::unordered_map<std::string, std::string> *raw;
}MAP_t;

void MAP_open(MAP_t *map){
	map->raw = new std::unordered_map<std::string, std::string>;
}
void MAP_close(MAP_t* map){
	delete map->raw;
}

MAP_out_t MAP_out(MAP_t *map, uint8_t *in, uintptr_t in_size){
	std::string str(in, in + in_size);
	auto found = map->raw->find(str);
	if(found == map->raw->end()){
		return {0, 0};
	}
	return {found->second.size(), (uint8_t*)found->second.data()};
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
	(*(*map).raw)[std::string(in, in + in_size)] = std::string(out, out + out_size);
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
	map->raw->erase(std::string(in, in + in_size));
}
#define MAP_rm(map_m, in_m, in_size_m) \
	MAP_rm((MAP_t *)(map_m), (uint8_t *)(in_m), (uintptr_t)(in_size_m))
#define MAP_rm_cstr(map_m, in_m) \
	MAP_rm(map_m, in_m, MEM_cstreu(in_m))

uint8_t MAP_its(MAP_t *map){
	return !map->raw->empty();
}

void MAP_dupe(MAP_t *src, MAP_t *dst){
	dst->raw = new std::remove_pointer<decltype(MAP_t::raw)>::type;
	for(auto i : *src->raw){
		(*dst->raw)[i.first] = i.second;
	}
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
	static std::unordered_map<std::string, std::string>::iterator ptr;
	if(!arg->i){
		ptr = arg->map->raw->begin();
	}

	if(ptr == arg->map->raw->end()){
		return 0;
	}

	arg->in = {ptr->first.size(), (uint8_t*)ptr->first.data()};
	arg->out = {ptr->second.size(), (uint8_t*)ptr->second.data()};
	std::advance(ptr, 1);
	arg->i++;

	return 1;
}
#define MAP_traverse(map_m) \
	for(_MAP_traverse_t mv = {(MAP_t *)(map_m), 0}; MAP_traverse(&mv);)
