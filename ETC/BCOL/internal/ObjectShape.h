void RecycleObject(
  ObjectID_t ObjectID
){
  auto ObjectData = &this->ObjectList[ObjectID];
  for(uint32_t i = 0; i < ObjectData->ShapeList.Current; i++){
    auto ShapeData = &((ShapeData_t *)ObjectData->ShapeList.ptr)[i];
    switch(ShapeData->ShapeEnum){
      case ShapeEnum_t::Circle:{
        this->UnlinkRecycleOrphanShape_Circle(ShapeData->ShapeID);
        break;
      }
      case ShapeEnum_t::Rectangle:{
        this->UnlinkRecycleOrphanShape_Rectangle(ShapeData->ShapeID);
        break;
      }
    }
  }
  VEC_free(&ObjectData->ShapeList);
  this->ObjectList.Recycle(ObjectID);
}
