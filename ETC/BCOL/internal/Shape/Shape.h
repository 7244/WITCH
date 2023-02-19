void AddShapeToObject(
  ObjectID_t ObjectID,
  ShapeEnum_t ShapeEnum,
  ShapeID_t ShapeID
){
  auto ObjectData = this->GetObjectData(ObjectID);
  VEC_handle(&ObjectData->ShapeList);
  auto ShapeData = &((ShapeData_t *)ObjectData->ShapeList.ptr)[ObjectData->ShapeList.Current];
  ShapeData->ShapeEnum = ShapeEnum;
  ShapeData->ShapeID = ShapeID;
  ObjectData->ShapeList.Current++;
}

#include _WITCH_PATH(ETC/BCOL/internal/Shape/Circle/Circle.h)
#include _WITCH_PATH(ETC/BCOL/internal/Shape/Rectangle/Rectangle.h)
