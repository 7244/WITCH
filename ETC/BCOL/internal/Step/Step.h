void Step(
  _f delta
){
  ObjectID_t ObjectID0 = this->ObjectList.GetNodeFirst();
  while(ObjectID0 != this->ObjectList.dst){
    this->ObjectList.StartSafeNext(ObjectID0);
    auto ObjectData0 = this->GetObjectData(ObjectID0);

    /* bad way */
    if(ObjectData0->Flag & ObjectFlag::Constant){
      ObjectID0 = this->ObjectList.EndSafeNext();
      continue;
    }

    {
      ETC_BCOL_set_DynamicDeltaFunction
    }

    const _vf StepVelocity = ObjectData0->Velocity * delta;

    _vf NewObjectPosition = ObjectData0->Position + StepVelocity;

    _vf WantedObjectPosition = 0;
    _vf WantedObjectDirection = 0;
    _f WantedObjectCollisionRequesters = 0;

    for(uint32_t ShapeID0 = 0; ShapeID0 < ObjectData0->ShapeList.Current; ShapeID0++){
      auto ShapeData = &((ShapeData_t *)ObjectData0->ShapeList.ptr)[ShapeID0];

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
      ObjectData0->Position = WantedObjectPosition / WantedObjectCollisionRequesters;

      _vf DirectionAverage = WantedObjectDirection / WantedObjectCollisionRequesters;

      _f VelocityHypotenuse = ObjectData0->Velocity.hypotenuse();
      if(VelocityHypotenuse != 0){
        _vf VelocityNormal = ObjectData0->Velocity / VelocityHypotenuse;

        _vf CollidedVelocity = fan::math::reflect(VelocityNormal, DirectionAverage);

        ObjectData0->Velocity = CollidedVelocity * VelocityHypotenuse;

        #if defined(ETC_BCOL_set_ConstantFriction) || defined(ETC_BCOL_set_ConstantBumpFriction)
          _f ForceThroughNormal = fan::math::dot2(DirectionAverage, VelocityNormal);
          ForceThroughNormal = fan::math::abs(ForceThroughNormal) * VelocityHypotenuse;
        #endif
        #ifdef ETC_BCOL_set_ConstantFriction
          ObjectData0->Velocity /= ForceThroughNormal * ETC_BCOL_set_ConstantFriction * delta + 1;
        #endif
        #ifdef ETC_BCOL_set_ConstantBumpFriction
          ObjectData0->Velocity -= fan::math::copysign(
            fan::math::min(fan::math::abs(ObjectData0->Velocity),
            ForceThroughNormal * ETC_BCOL_set_ConstantBumpFriction * delta),
            ObjectData0->Velocity);
        #endif
      }
    }
    else{
      ObjectData0->Position = NewObjectPosition;
    }

    gt_Object0Unlinked:
    ObjectID0 = this->ObjectList.EndSafeNext();
  }

  #if ETC_BCOL_set_StepNumber == 1
    this->StepNumber++;
  #endif
}
