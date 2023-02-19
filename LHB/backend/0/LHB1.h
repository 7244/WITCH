#include _WITCH_PATH(VAS/VAS.h)

typedef VAS_node_t LHB1_node_t;

typedef struct{
	VAS_t vas;
}LHB1_t;

bool LHB1_is_node_invalid(LHB1_t *lhb, LHB1_node_t node){
	return node == (LHB1_node_t)-1;
}

void LHB1_open(LHB1_t *lhb, uintptr_t size){
	VAS_open(&lhb->vas, sizeof(uint64_t) + size);
}
void LHB1_close(LHB1_t *lhb){
	VAS_close(&lhb->vas);
}

LHB1_node_t LHB1_lo(LHB1_t *lhb, uint64_t req_value){
	VAS_node_t node = *VAS_road0(&lhb->vas, lhb->vas.src);
	while(node != lhb->vas.dst){
		uint64_t node_value = *(uint64_t *)VAS_out(&lhb->vas, node);
		if(node_value > req_value){
			node = *VAS_road1(&lhb->vas, node);
			if(node == lhb->vas.src){
				/* first element is bigger than req_value */
				return (LHB1_node_t)-1;
			}
			return node;
		}
		node = *VAS_road0(&lhb->vas, node);
	}
	if(!VAS_usage(&lhb->vas)){
		return (LHB1_node_t)-1;
	}
	return *VAS_road1(&lhb->vas, node);
}

LHB1_node_t LHB1_hi(LHB1_t *lhb, uint64_t req_value){
	VAS_node_t node = *VAS_road1(&lhb->vas, lhb->vas.dst);
	while(node != lhb->vas.src){
		uint64_t node_value = *(uint64_t *)VAS_out(&lhb->vas, node);
		if(node_value < req_value){
			node = *VAS_road0(&lhb->vas, node);
			if(node == lhb->vas.dst){
				/* first element is smaller than req_value */
				return (LHB1_node_t)-1;
			}
			return node;
		}
		node = *VAS_road1(&lhb->vas, node);
	}
	if(!VAS_usage(&lhb->vas)){
		return (LHB1_node_t)-1;
	}
	return *VAS_road0(&lhb->vas, node);
}

LHB1_node_t LHB1_lohi(LHB1_t *lhb, uint64_t req_value, uint64_t *res_value){
	VAS_node_t node = *VAS_road0(&lhb->vas, lhb->vas.src);
	while(node != lhb->vas.dst){
		uint64_t node_value = *(uint64_t *)VAS_out(&lhb->vas, node);
		if(node_value > req_value){
			node = *VAS_road1(&lhb->vas, node);
			if(node == lhb->vas.src){
				/* first element is bigger than req_value */
				node = *VAS_road0(&lhb->vas, node);
				*res_value = *(uint64_t *)VAS_out(&lhb->vas, node);
				return node;
			}
			return node;
		}
		node = *VAS_road0(&lhb->vas, node);
	}
	if(!VAS_usage(&lhb->vas)){
		return (LHB1_node_t)-1;
	}
	*res_value = *(uint64_t *)VAS_out(&lhb->vas, node);
	return *VAS_road1(&lhb->vas, node);
}

LHB1_node_t LHB1_hilo(LHB1_t *lhb, uint64_t req_value, uint64_t *res_value){
	VAS_node_t node = *VAS_road1(&lhb->vas, lhb->vas.dst);
	while(node != lhb->vas.src){
		uint64_t node_value = *(uint64_t *)VAS_out(&lhb->vas, node);
		if(node_value < req_value){
			node = *VAS_road0(&lhb->vas, node);
			if(node == lhb->vas.dst){
				/* first element is smaller than req_value */
				node = *VAS_road1(&lhb->vas, node);
				*res_value = *(uint64_t *)VAS_out(&lhb->vas, node);
				return node;
			}
			return node;
		}
		node = *VAS_road1(&lhb->vas, node);
	}
	if(!VAS_usage(&lhb->vas)){
		return (LHB1_node_t)-1;
	}
	*res_value = *(uint64_t *)VAS_out(&lhb->vas, node);
	return *VAS_road0(&lhb->vas, node);
}

LHB1_node_t LHB1_link_next(LHB1_t *lhb, LHB1_node_t node, uint64_t v){
	VAS_node_t new_node = VAS_NewNode(&lhb->vas);
	*(uint64_t *)VAS_out(&lhb->vas, new_node) = v;
	VAS_link0(&lhb->vas, node, new_node);
	return new_node;
}
LHB1_node_t LHB1_link_prev(LHB1_t *lhb, LHB1_node_t node, uint64_t v){
	VAS_node_t new_node = VAS_NewNode(&lhb->vas);
	*(uint64_t *)VAS_out(&lhb->vas, new_node) = v;
	VAS_link1(&lhb->vas, node, new_node);
	return new_node;
}

LHB1_node_t LHB1_link_src(LHB1_t *lhb, uint64_t v){
	VAS_node_t node = VAS_NewNodeFirst(&lhb->vas);
	*(uint64_t *)VAS_out(&lhb->vas, node) = v;
	return node;
}
LHB1_node_t LHB1_link_dst(LHB1_t *lhb, uint64_t v){
	VAS_node_t node = VAS_NewNodeLast(&lhb->vas);
	*(uint64_t *)VAS_out(&lhb->vas, node) = v;
	return node;
}

LHB1_node_t LHB1_link(LHB1_t *lhb, uint64_t v){
	LHB1_node_t node = LHB1_lo(lhb, v);
	if(LHB1_is_node_invalid(lhb, node)){
		return LHB1_link_src(lhb, v);
	}
	return LHB1_link_next(lhb, node, v);
}

uint64_t LHB1_out_num(LHB1_t *lhb, LHB1_node_t node){
	return *(uint64_t *)VAS_out(&lhb->vas, node);
}
uint8_t *LHB1_out(LHB1_t *lhb, LHB1_node_t node){
	return VAS_out(&lhb->vas, node) + sizeof(uint64_t);
}

LHB1_node_t LHB1_node_begin(LHB1_t *lhb){
	return *VAS_road0(&lhb->vas, lhb->vas.src);
}
LHB1_node_t LHB1_node_end(LHB1_t *lhb){
	return lhb->vas.dst;
}
LHB1_node_t LHB1_node_rbegin(LHB1_t *lhb){
	return *VAS_road1(&lhb->vas, lhb->vas.dst);
}
LHB1_node_t LHB1_node_rend(LHB1_t *lhb){
	return lhb->vas.src;
}
LHB1_node_t LHB1_node_iterate(LHB1_t *lhb, LHB1_node_t node){
	return *VAS_road0(&lhb->vas, node);
}
LHB1_node_t LHB1_node_riterate(LHB1_t *lhb, LHB1_node_t node){
	return *VAS_road1(&lhb->vas, node);
}
