/* object */

struct TraverseObject_t{
  ShapeID_t ShapeID;
  struct{
    uint32_t ShapeIndex;
  }priv;
};

void TraverseObject_init(TraverseObject_t *TraverseObject){
  TraverseObject->priv.ShapeIndex = 0;
}

bool TraverseObject_loop(
  ObjectID_t ObjectID,
  TraverseObject_t *TraverseObject
){
  auto ObjectData = this->GetObjectData(ObjectID);
  if(TraverseObject->priv.ShapeIndex == ObjectData->ShapeList.Current){
    return false;
  }

  TraverseObject->ShapeID = *(ShapeID_t *)&TraverseObject->priv.ShapeIndex;

  TraverseObject->priv.ShapeIndex++;
  return true;
}

/* objects */

struct TraverseObjects_t{
  ObjectID_t ObjectID;
  struct{
    ObjectID_t NextObjectID;
  }priv;
};

void TraverseObjects_init(
  TraverseObjects_t *TraverseObjects
){
  TraverseObjects->ObjectID = this->ObjectList.src;
  auto ObjectNode = this->ObjectList.GetNodeByReference(TraverseObjects->ObjectID);
  TraverseObjects->priv.NextObjectID = ObjectNode->NextNodeReference;
}

bool TraverseObjects_loop(
  TraverseObjects_t *TraverseObjects
){
  if(TraverseObjects->priv.NextObjectID == this->ObjectList.dst){
    return false;
  }
  TraverseObjects->ObjectID = TraverseObjects->priv.NextObjectID;
  auto ObjectNode = this->ObjectList.GetNodeByReference(TraverseObjects->ObjectID);
  TraverseObjects->priv.NextObjectID = ObjectNode->NextNodeReference;
  return true;
}
