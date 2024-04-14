#include _WITCH_PATH(VAS/VAS.h)

typedef VAS_node_t LHB_node_t;

typedef struct{
  VAS_t vas;
}LHB_t;

bool LHB_is_node_invalid(LHB_t *lhb, LHB_node_t node){
  return node == (LHB_node_t)-1;
}

void LHB_open(LHB_t *lhb, uintptr_t size){
  VAS_open(&lhb->vas, sizeof(uint32_t) + size);
}
void LHB_close(LHB_t *lhb){
  VAS_close(&lhb->vas);
}

LHB_node_t LHB_lo(LHB_t *lhb, uint32_t req_value){
  VAS_node_t node = *VAS_road0(&lhb->vas, lhb->vas.src);
  while(node != lhb->vas.dst){
    uint32_t node_value = *(uint32_t *)VAS_out(&lhb->vas, node);
    if(node_value > req_value){
      node = *VAS_road1(&lhb->vas, node);
      if(node == lhb->vas.src){
        /* first element is bigger than req_value */
        return (LHB_node_t)-1;
      }
      return node;
    }
    node = *VAS_road0(&lhb->vas, node);
  }
  if(!VAS_usage(&lhb->vas)){
    return (LHB_node_t)-1;
  }
  return *VAS_road1(&lhb->vas, node);
}

LHB_node_t LHB_hi(LHB_t *lhb, uint32_t req_value){
  VAS_node_t node = *VAS_road1(&lhb->vas, lhb->vas.dst);
  while(node != lhb->vas.src){
    uint32_t node_value = *(uint32_t *)VAS_out(&lhb->vas, node);
    if(node_value < req_value){
      node = *VAS_road0(&lhb->vas, node);
      if(node == lhb->vas.dst){
        /* first element is smaller than req_value */
        return (LHB_node_t)-1;
      }
      return node;
    }
    node = *VAS_road1(&lhb->vas, node);
  }
  if(!VAS_usage(&lhb->vas)){
    return (LHB_node_t)-1;
  }
  return *VAS_road0(&lhb->vas, node);
}

LHB_node_t LHB_lohi(LHB_t *lhb, uint32_t req_value, uint32_t *res_value){
  VAS_node_t node = *VAS_road0(&lhb->vas, lhb->vas.src);
  while(node != lhb->vas.dst){
    uint32_t node_value = *(uint32_t *)VAS_out(&lhb->vas, node);
    if(node_value > v){
      node = *VAS_road1(&lhb->vas, node);
      if(node == lhb->vas.src){
        /* first element is bigger than req_value */
        node = *VAS_road0(&lhb->vas, node);
        *res_value = *(uint32_t *)VAS_out(&lhb->vas, node);
        return node;
      }
      return node;
    }
    node = *VAS_road0(&lhb->vas, node);
  }
  if(!VAS_usage(&lhb->vas)){
    return (LHB_node_t)-1;
  }
  *res_value = *(uint32_t *)VAS_out(&lhb->vas, node);
  return *VAS_road1(&lhb->vas, node);
}

LHB_node_t LHB_hilo(LHB_t *lhb, uint32_t v, uint32_t *res_value){
  VAS_node_t node = *VAS_road1(&lhb->vas, lhb->vas.dst);
  while(node != lhb->vas.src){
    uint32_t node_value = *(uint32_t *)VAS_out(&lhb->vas, node);
    if(node_value < v){
      node = *VAS_road0(&lhb->vas, node);
      if(node == lhb->vas.dst){
        /* first element is smaller than req_value */
        node = *VAS_road1(&lhb->vas, node);
        *res_value = *(uint32_t *)VAS_out(&lhb->vas, node);
        return node;
      }
      return node;
    }
    node = *VAS_road1(&lhb->vas, node);
  }
  if(!VAS_usage(&lhb->vas)){
    return (LHB_node_t)-1;
  }
  *res_value = *(uint32_t *)VAS_out(&lhb->vas, node);
  return *VAS_road0(&lhb->vas, node);
}

LHB_node_t LHB_link_next(LHB_t *lhb, LHB_node_t node, uint32_t v){
  VAS_node_t new_node = VAS_NewNode(&lhb->vas);
  *(uint32_t *)VAS_out(&lhb->vas, new_node) = v;
  VAS_link0(&lhb->vas, node, new_node);
  return new_node;
}
LHB_node_t LHB_link_prev(LHB_t *lhb, LHB_node_t node, uint32_t v){
  VAS_node_t new_node = VAS_NewNode(&lhb->vas);
  *(uint32_t *)VAS_out(&lhb->vas, new_node) = v;
  VAS_link1(&lhb->vas, node, new_node);
  return new_node;
}

LHB_node_t LHB_link_src(LHB_t *lhb, uint32_t v){
  VAS_node_t node = VAS_NewNodeFirst(&lhb->vas);
  *(uint32_t *)VAS_out(&lhb->vas, node) = v;
  return node;
}
LHB_node_t LHB_link_dst(LHB_t *lhb, uint32_t v){
  VAS_node_t node = VAS_NewNodeLast(&lhb->vas);
  *(uint32_t *)VAS_out(&lhb->vas, node) = v;
  return node;
}

LHB_node_t LHB_link(LHB_t *lhb, uint32_t v){
  LHB_node_t node = LHB_lo(lhb, v);
  if(LHB_is_node_invalid(lhb, node)){
    return LHB_link_src(lhb, v);
  }
  return LHB_link_next(lhb, node, v);
}

uint32_t LHB_out_num(LHB_t *lhb, LHB_node_t node){
  return *(uint32_t *)VAS_out(&lhb->vas, node);
}
uint8_t *LHB_out(LHB_t *lhb, LHB_node_t node){
  return VAS_out(&lhb->vas, node) + sizeof(uint32_t);
}

LHB_node_t LHB_node_begin(LHB_t *lhb){
  return *VAS_road0(&lhb->vas, lhb->vas.src);
}
LHB_node_t LHB_node_end(LHB_t *lhb){
  return lhb->vas.dst;
}
LHB_node_t LHB_node_rbegin(LHB_t *lhb){
  return *VAS_road1(&lhb->vas, lhb->vas.dst);
}
LHB_node_t LHB_node_rend(LHB_t *lhb){
  return lhb->vas.src;
}
LHB_node_t LHB_node_iterate(LHB_t *lhb, LHB_node_t node){
  return *VAS_road0(&lhb->vas, node);
}
LHB_node_t LHB_node_riterate(LHB_t *lhb, LHB_node_t node){
  return *VAS_road1(&lhb->vas, node);
}
