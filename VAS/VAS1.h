#pragma once

#include _WITCH_PATH(WITCH.h)
#include _WITCH_PATH(A/A.h)

typedef uint32_t VAS1_node_t;

typedef struct{
  uint8_t *ptr;
  VAS1_node_t c;
  VAS1_node_t src;
  VAS1_node_t dst;
  struct{
    VAS1_node_t c;
    VAS1_node_t p;
  }e;
  uint32_t nodesize;
}VAS1_t;

VAS1_node_t *VAS1_road0(VAS1_t *vas, VAS1_node_t node){
  return &((VAS1_node_t *)(vas->ptr + (vas->nodesize * node)))[0];
}
VAS1_node_t *VAS1_road1(VAS1_t *vas, VAS1_node_t node){
  return &((VAS1_node_t *)(vas->ptr + (vas->nodesize * node)))[1];
}
uint8_t *VAS1_out(VAS1_t *vas, VAS1_node_t node){
  return (uint8_t *)&((VAS1_node_t *)(vas->ptr + (vas->nodesize * node)))[2];
}
uintptr_t VAS1_usage(VAS1_t *vas){
  return vas->c - vas->e.p - 2;
}

void VAS1_open(VAS1_t *vas, uint32_t outsize, uint32_t mlimit){
  vas->nodesize = (sizeof(VAS1_node_t) * 2) + outsize;
  vas->c = 2;
  vas->ptr = A_resize(0, (mlimit + vas->c) * vas->nodesize);
  vas->e.c = 0;
  vas->e.p = 0;
  vas->src = 0;
  vas->dst = 1;
  *VAS1_road0(vas, vas->src) = vas->dst;
  *VAS1_road1(vas, vas->dst) = vas->src;
}
void VAS1_close(VAS1_t *vas){
  A_resize(vas->ptr, 0);
}

VAS1_node_t VAS1_NewNode_empty(VAS1_t *vas){
  VAS1_node_t n = vas->e.c;
  vas->e.c = *VAS1_road0(vas, n);
  vas->e.p--;
  return n;
}
VAS1_node_t VAS1_NewNode_alloc(VAS1_t *vas){
  return vas->c++;
}
VAS1_node_t VAS1_NewNode(VAS1_t *vas){
  if(vas->e.p){
    return VAS1_NewNode_empty(vas);
  }
  else{
    return VAS1_NewNode_alloc(vas);
  }
}

VAS1_node_t VAS1_NewNodeFirst_empty(VAS1_t *vas){
  VAS1_node_t n = VAS1_NewNode_empty(vas);
  VAS1_node_t src = vas->src;
  *VAS1_road0(vas, n) = src;
  *VAS1_road1(vas, src) = n;
  vas->src = n;
  return src;
}
VAS1_node_t VAS1_NewNodeFirst_alloc(VAS1_t *vas){
  VAS1_node_t n = VAS1_NewNode_alloc(vas);
  VAS1_node_t src = vas->src;
  *VAS1_road0(vas, n) = src;
  *VAS1_road1(vas, src) = n;
  vas->src = n;
  return src;
}
VAS1_node_t VAS1_NewNodeFirst(VAS1_t *vas){
  if(vas->e.p){
    return VAS1_NewNodeFirst_empty(vas);
  }
  else{
    return VAS1_NewNodeFirst_alloc(vas);
  }
}
VAS1_node_t VAS1_NewNodeLast_empty(VAS1_t *vas){
  VAS1_node_t n = VAS1_NewNode_empty(vas);
  VAS1_node_t dst = vas->dst;
  *VAS1_road1(vas, n) = dst;
  *VAS1_road0(vas, dst) = n;
  vas->dst = n;
  return dst;
}
VAS1_node_t VAS1_NewNodeLast_alloc(VAS1_t *vas){
  VAS1_node_t n = VAS1_NewNode_alloc(vas);
  VAS1_node_t dst = vas->dst;
  *VAS1_road1(vas, n) = dst;
  *VAS1_road0(vas, dst) = n;
  vas->dst = n;
  return dst;
}
VAS1_node_t VAS1_NewNodeLast(VAS1_t *vas){
  if(vas->e.p){
    return VAS1_NewNodeLast_empty(vas);
  }
  else{
    return VAS1_NewNodeLast_alloc(vas);
  }
}

void VAS1_link0(VAS1_t *vas, VAS1_node_t src, VAS1_node_t dst){
  VAS1_node_t next = *VAS1_road0(vas, src);
  *VAS1_road0(vas, src) = dst;
  *VAS1_road1(vas, dst) = src;
  *VAS1_road0(vas, dst) = next;
  *VAS1_road1(vas, next) = dst;
}
void VAS1_link1(VAS1_t *vas, VAS1_node_t src, VAS1_node_t dst){
  VAS1_node_t prev = *VAS1_road1(vas, src);
  *VAS1_road0(vas, prev) = dst;
  *VAS1_road1(vas, dst) = prev;
  *VAS1_road0(vas, dst) = src;
  *VAS1_road1(vas, src) = dst;
}

void VAS1_unlink(VAS1_t *vas, VAS1_node_t node){
  VAS1_node_t next = *VAS1_road0(vas, node);
  VAS1_node_t prev = *VAS1_road1(vas, node);
  *VAS1_road0(vas, prev) = next;
  *VAS1_road1(vas, next) = prev;

  *VAS1_road0(vas, node) = vas->e.c;
  vas->e.c = node;
  vas->e.p++;
}

/*
  undefined behavior if VAS1_usage is 0
  @param VAS1_t *vas
*/
VAS1_node_t VAS1_GetNodeFirst(VAS1_t *vas){
  return *VAS1_road0(vas, vas->src);
}
/*
  undefined behavior if VAS1_usage is 0
  @param VAS1_t *vas
*/
VAS1_node_t VAS1_GetNodeLast(VAS1_t *vas){
  return *VAS1_road1(vas, vas->dst);
}
