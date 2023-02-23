#pragma once

#include _WITCH_PATH(WITCH.h)
#include _WITCH_PATH(A/A.h)

typedef uint32_t VAS2_node_t;

typedef struct{
  uint8_t *ptr;
  VAS2_node_t c;
  struct{
    VAS2_node_t c;
    VAS2_node_t p;
  }e;
  uint32_t nodesize;
}VAS2_t;

uint8_t *VAS2_out(VAS2_t *vas, VAS2_node_t node){
  return (vas->ptr + vas->nodesize * node);
}
uintptr_t VAS2_usage(VAS2_t *vas){
  return vas->c - vas->e.p;
}

void VAS2_open(VAS2_t *vas, uint32_t outsize, uint32_t mlimit){
  if(outsize < sizeof(VAS2_node_t)){
    outsize = sizeof(VAS2_node_t);
  }
  vas->nodesize = outsize;
  vas->c = 0;
  vas->ptr = A_resize(0, mlimit * vas->nodesize);
  vas->e.c = 0;
  vas->e.p = 0;
}
void VAS2_close(VAS2_t *vas){
  A_resize(vas->ptr, 0);
}

VAS2_node_t VAS2_NewNode_empty(VAS2_t *vas){
  VAS2_node_t n = vas->e.c;
  vas->e.c = *(VAS2_node_t *)VAS2_out(vas, n);
  vas->e.p--;
  return n;
}
VAS2_node_t VAS2_NewNode_alloc(VAS2_t *vas){
  return vas->c++;
}
VAS2_node_t VAS2_NewNode(VAS2_t *vas){
  if(vas->e.p){
    return VAS2_NewNode_empty(vas);
  }
  else{
    return VAS2_NewNode_alloc(vas);
  }
}

void VAS2_unlink(VAS2_t *vas, VAS2_node_t node){
  *(VAS2_node_t *)VAS2_out(vas, node) = vas->e.c;
  vas->e.c = node;
  vas->e.p++;
}
