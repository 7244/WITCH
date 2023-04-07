void Step(
  _f delta
){
  ObjectID_t ObjectID = this->ObjectList.GetNodeFirst();
  while(ObjectID != this->ObjectList.dst){
    this->ObjectList.StartSafeNext(ObjectID);
    auto ObjectData = this->GetObjectData(ObjectID);

    /* bad way */
    if(ObjectData->Flag & ObjectFlag::Constant){
      ObjectID = this->ObjectList.EndSafeNext();
      continue;
    }

    {
      ETC_BCOL_set_DynamicDeltaFunction
    }

    const _vf StepVelocity = ObjectData->Velocity * delta;

    _vf NewObjectPosition = ObjectData->Position + StepVelocity;

    _vf WantedObjectPosition = 0;
    _vf WantedObjectDirection = 0;
    _f WantedObjectCollisionRequesters = 0;

    for(uint32_t ShapeID = 0; ShapeID < ObjectData->ShapeList.Current; ShapeID++){
      auto ShapeData = &((ShapeData_t *)ObjectData->ShapeList.ptr)[ShapeID];

      switch(ShapeData->ShapeEnum){
        case ShapeEnum_t::Circle:{
          #include _WITCH_PATH(ETC/BCOL/internal/Step/Shape/Circle.h)
        }
        case ShapeEnum_t::Rectangle:{
          #include _WITCH_PATH(ETC/BCOL/internal/Step/Shape/Rectangle.h)
        }
      }
    }

    if(WantedObjectCollisionRequesters){
      ObjectData->Position = WantedObjectPosition / WantedObjectCollisionRequesters;

      _vf DirectionAverage = WantedObjectDirection / WantedObjectCollisionRequesters;

      _f VelocityHypotenuse = ObjectData->Velocity.hypotenuse();
      if(VelocityHypotenuse != 0){
        _vf VelocityNormal = ObjectData->Velocity / VelocityHypotenuse;

        _vf CollidedVelocity = fan::math::reflect(VelocityNormal, DirectionAverage);

        ObjectData->Velocity = CollidedVelocity * VelocityHypotenuse;

        #if defined(ETC_BCOL_set_ConstantFriction) || defined(ETC_BCOL_set_ConstantBumpFriction)
          _f ForceThroughNormal = fan::math::dot2(DirectionAverage, VelocityNormal);
          ForceThroughNormal = fan::math::abs(ForceThroughNormal) * VelocityHypotenuse;
        #endif
        #ifdef ETC_BCOL_set_ConstantFriction
          ObjectData->Velocity /= ForceThroughNormal * ETC_BCOL_set_ConstantFriction * delta + 1;
        #endif
        #ifdef ETC_BCOL_set_ConstantBumpFriction
          ObjectData->Velocity -= fan::math::copysign(
            fan::math::min(fan::math::abs(ObjectData->Velocity),
            ForceThroughNormal * ETC_BCOL_set_ConstantBumpFriction * delta),
            ObjectData->Velocity);
        #endif
      }
    }
    else{
      ObjectData->Position = NewObjectPosition;
    }

    gt_Object0Unlinked:
    ObjectID = this->ObjectList.EndSafeNext();
  }

  #if ETC_BCOL_set_StepNumber == 1
    this->StepNumber++;
  #endif
}
