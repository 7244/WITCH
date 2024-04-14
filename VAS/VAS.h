#pragma once

#include _WITCH_PATH(WITCH.h)
#include _WITCH_PATH(VEC/VEC.h)

#ifndef VAS_set_type_node
  #define VAS_set_type_node uint32_t
#endif

typedef VAS_set_type_node VAS_node_t;

typedef struct{
  VEC_t nodes;
  VAS_node_t src;
  VAS_node_t dst;
  struct{
    VAS_node_t c;
    VAS_node_t p;
  }e;
  uint32_t nodesize;
}VAS_t;

VAS_node_t *VAS_road0(VAS_t *vas, VAS_node_t node){
  return &((VAS_node_t *)(vas->nodes.ptr + (vas->nodesize * node)))[0];
}
VAS_node_t *VAS_road1(VAS_t *vas, VAS_node_t node){
  return &((VAS_node_t *)(vas->nodes.ptr + (vas->nodesize * node)))[1];
}
uint8_t *VAS_out(VAS_t *vas, VAS_node_t node){
  return (uint8_t *)&((VAS_node_t *)(vas->nodes.ptr + (vas->nodesize * node)))[2];
}
uintptr_t VAS_usage(VAS_t *vas){
  return vas->nodes.Current - vas->e.p - 2;
}

void VAS_open(VAS_t *vas, uint32_t outsize){
  vas->nodesize = (sizeof(VAS_node_t) * 2) + outsize;
  VEC_init(&vas->nodes, vas->nodesize, A_resize);
  vas->e.c = 0;
  vas->e.p = 0;
  VEC_handle0(&vas->nodes, 2);
  vas->src = 0;
  vas->dst = 1;
  *VAS_road0(vas, vas->src) = vas->dst;
  *VAS_road1(vas, vas->dst) = vas->src;
}
void VAS_close(VAS_t *vas){
  VEC_free(&vas->nodes);
}

VAS_node_t VAS_NewNode_empty(VAS_t *vas){
  VAS_node_t n = vas->e.c;
  vas->e.c = *VAS_road0(vas, n);
  vas->e.p--;
  return n;
}
VAS_node_t VAS_NewNode_alloc(VAS_t *vas){
  VEC_handle(&vas->nodes);
  return vas->nodes.Current++;
}
VAS_node_t VAS_NewNode(VAS_t *vas){
  if(vas->e.p){
    return VAS_NewNode_empty(vas);
  }
  else{
    return VAS_NewNode_alloc(vas);
  }
}

VAS_node_t VAS_NewNodeFirst_empty(VAS_t *vas){
  VAS_node_t n = VAS_NewNode_empty(vas);
  VAS_node_t src = vas->src;
  *VAS_road0(vas, n) = src;
  *VAS_road1(vas, src) = n;
  vas->src = n;
  return src;
}
VAS_node_t VAS_NewNodeFirst_alloc(VAS_t *vas){
  VAS_node_t n = VAS_NewNode_alloc(vas);
  VAS_node_t src = vas->src;
  *VAS_road0(vas, n) = src;
  *VAS_road1(vas, src) = n;
  vas->src = n;
  return src;
}
VAS_node_t VAS_NewNodeFirst(VAS_t *vas){
  if(vas->e.p){
    return VAS_NewNodeFirst_empty(vas);
  }
  else{
    return VAS_NewNodeFirst_alloc(vas);
  }
}
VAS_node_t VAS_NewNodeLast_empty(VAS_t *vas){
  VAS_node_t n = VAS_NewNode_empty(vas);
  VAS_node_t dst = vas->dst;
  *VAS_road1(vas, n) = dst;
  *VAS_road0(vas, dst) = n;
  vas->dst = n;
  return dst;
}
VAS_node_t VAS_NewNodeLast_alloc(VAS_t *vas){
  VAS_node_t n = VAS_NewNode_alloc(vas);
  VAS_node_t dst = vas->dst;
  *VAS_road1(vas, n) = dst;
  *VAS_road0(vas, dst) = n;
  vas->dst = n;
  return dst;
}
VAS_node_t VAS_NewNodeLast(VAS_t *vas){
  if(vas->e.p){
    return VAS_NewNodeLast_empty(vas);
  }
  else{
    return VAS_NewNodeLast_alloc(vas);
  }
}

void VAS_link0(VAS_t *vas, VAS_node_t src, VAS_node_t dst){
  VAS_node_t next = *VAS_road0(vas, src);
  *VAS_road0(vas, src) = dst;
  *VAS_road1(vas, dst) = src;
  *VAS_road0(vas, dst) = next;
  *VAS_road1(vas, next) = dst;
}
void VAS_link1(VAS_t *vas, VAS_node_t src, VAS_node_t dst){
  VAS_node_t prev = *VAS_road1(vas, src);
  *VAS_road0(vas, prev) = dst;
  *VAS_road1(vas, dst) = prev;
  *VAS_road0(vas, dst) = src;
  *VAS_road1(vas, src) = dst;
}

void VAS_unlink(VAS_t *vas, VAS_node_t node){
  VAS_node_t next = *VAS_road0(vas, node);
  VAS_node_t prev = *VAS_road1(vas, node);
  *VAS_road0(vas, prev) = next;
  *VAS_road1(vas, next) = prev;

  *VAS_road0(vas, node) = vas->e.c;
  vas->e.c = node;
  vas->e.p++;
}

/*
  undefined behavior if VAS_usage is 0
  @param VAS_t *vas
*/
VAS_node_t VAS_GetNodeFirst(VAS_t *vas){
  return *VAS_road0(vas, vas->src);
}
/*
  undefined behavior if VAS_usage is 0
  @param VAS_t *vas
*/
VAS_node_t VAS_GetNodeLast(VAS_t *vas){
  return *VAS_road1(vas, vas->dst);
}
