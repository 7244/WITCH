auto CircleData = this->ShapeData_Circle_Get(ShapeData->ShapeID);

_vf NewPosition = NewObjectPosition + CircleData->Position;

_vf WantedPosition = 0;
_vf WantedDirection = 0;
_f WantedCollisionRequesters = 0;

#if ETC_BCOL_set_SupportGrid == 1
  const _f GridBlockSize = this->GridBlockSize;
  const _f GridBlockSize_D2 = GridBlockSize / 2;

  sint32_t CircleMiddleGridY = fan::math::floor(NewPosition.y / GridBlockSize);

  {
    _f CircleLeftX = NewPosition.x - CircleData->Size;
    _f CircleRightX = NewPosition.x + CircleData->Size;
    sint32_t CircleLeftGridX = fan::math::floor(CircleLeftX / GridBlockSize);
    sint32_t CircleRightGridX = fan::math::floor(CircleRightX / GridBlockSize);
    for(sint32_t CircleGridX = CircleLeftGridX; CircleGridX <= CircleRightGridX; CircleGridX++){
      Contact_Grid_t Contact;
      Contact.Flag = 0;
      this->PreSolve_Grid_cb(
        this,
        ObjectID,
        ShapeEnum_t::Circle,
        ShapeData->ShapeID,
        {CircleGridX, CircleMiddleGridY},
        &Contact);
      if(this->ObjectList.CheckSafeNext(0) != ObjectID){
        goto gt_Object0Unlinked;
      }
      if(Contact.Flag & Contact_Grid_Flag::EnableContact); else{
        continue;
      };

      _vf oCircle;
      _vf oDirection;
      CPC_Circle_Square(
        NewPosition,
        CircleData->Size,
        {CircleGridX * GridBlockSize + GridBlockSize_D2, CircleMiddleGridY * GridBlockSize + GridBlockSize_D2},
        GridBlockSize_D2,
        &oCircle,
        &oDirection);

      #ifdef ETC_BCOL_set_PostSolve_Grid
        ContactResult_Grid_t ContactResult;
      #endif
      #ifdef ETC_BCOL_set_PostSolve_Grid_CollisionNormal
        ContactResult.Normal = oDirection;
      #endif
      #ifdef ETC_BCOL_set_PostSolve_Grid
        this->PostSolve_Grid_cb(
          this,
          ObjectID,
          ShapeEnum_t::Circle,
          ShapeData->ShapeID,
          {CircleGridX, CircleMiddleGridY},
          &ContactResult);
        if(this->ObjectList.CheckSafeNext(0) != ObjectID){
          goto gt_Object0Unlinked;
        }
      #endif

      WantedPosition += oCircle;
      WantedDirection += oDirection;
      WantedCollisionRequesters++;
    }
  }

  {
    _f CircleTopY = NewPosition.y - CircleData->Size;
    sint32_t CircleTopGridY = fan::math::floor(CircleTopY / GridBlockSize);
    for(sint32_t CircleGridY = CircleMiddleGridY; CircleGridY > CircleTopGridY;){
      _f CircleY = (_f)CircleGridY * GridBlockSize;
      _f CircleOffsetY = CircleY - NewPosition.y;
      _f Magic = fan::math::sqrt(fan::math::abs(CircleData->Size * CircleData->Size - CircleOffsetY * CircleOffsetY));
      _f CircleLeftX = NewPosition.x - Magic;
      _f CircleRightX = NewPosition.x + Magic;
      sint32_t CircleLeftGridX = fan::math::floor(CircleLeftX / GridBlockSize);
      sint32_t CircleRightGridX = fan::math::floor(CircleRightX / GridBlockSize);
      CircleGridY--;
      for(sint32_t CircleGridX = CircleLeftGridX; CircleGridX <= CircleRightGridX; CircleGridX++){
        Contact_Grid_t Contact;
        this->PreSolve_Grid_cb(
          this,
          ObjectID,
          ShapeEnum_t::Circle,
          ShapeData->ShapeID,
          {CircleGridX, CircleGridY},
          &Contact);
        if(this->ObjectList.CheckSafeNext(0) != ObjectID){
          goto gt_Object0Unlinked;
        }
        if(Contact.Flag & Contact_Grid_Flag::EnableContact); else{
          continue;
        };

        _vf oCircle;
        _vf oDirection;
        CPC_Circle_Square(
          NewPosition,
          CircleData->Size,
          {CircleGridX * GridBlockSize + GridBlockSize_D2, CircleGridY * GridBlockSize + GridBlockSize_D2},
          GridBlockSize_D2,
          &oCircle,
          &oDirection);

        #ifdef ETC_BCOL_set_PostSolve_Grid
          ContactResult_Grid_t ContactResult;
        #endif
        #ifdef ETC_BCOL_set_PostSolve_Grid_CollisionNormal
          ContactResult.Normal = oDirection;
        #endif
        #ifdef ETC_BCOL_set_PostSolve_Grid
          this->PostSolve_Grid_cb(
            this,
            ObjectID,
            ShapeEnum_t::Circle,
            ShapeData->ShapeID,
            {CircleGridX, CircleGridY},
            &ContactResult);
          if(this->ObjectList.CheckSafeNext(0) != ObjectID){
            goto gt_Object0Unlinked;
          }
        #endif

        WantedPosition += oCircle;
        WantedDirection += oDirection;
        WantedCollisionRequesters++;
      }
    }
  }

  {
    _f CircleBottomY = NewPosition.y + CircleData->Size;
    sint32_t CircleBottomGridY = fan::math::floor(CircleBottomY / GridBlockSize);
    for(sint32_t CircleGridY = CircleMiddleGridY; CircleGridY < CircleBottomGridY;){
      CircleGridY++;
      _f CircleY = (_f)CircleGridY * GridBlockSize;
      _f CircleOffsetY = CircleY - NewPosition.y;
      _f Magic = fan::math::sqrt(fan::math::abs(CircleData->Size * CircleData->Size - CircleOffsetY * CircleOffsetY));
      _f CircleLeftX = NewPosition.x - Magic;
      _f CircleRightX = NewPosition.x + Magic;
      sint32_t CircleLeftGridX = fan::math::floor(CircleLeftX / GridBlockSize);
      sint32_t CircleRightGridX = fan::math::floor(CircleRightX / GridBlockSize);
      for(sint32_t CircleGridX = CircleLeftGridX; CircleGridX <= CircleRightGridX; CircleGridX++){
        Contact_Grid_t Contact;
        this->PreSolve_Grid_cb(
          this,
          ObjectID,
          ShapeEnum_t::Circle,
          ShapeData->ShapeID,
          {CircleGridX, CircleGridY},
          &Contact);
        if(this->ObjectList.CheckSafeNext(0) != ObjectID){
          goto gt_Object0Unlinked;
        }
        if(Contact.Flag & Contact_Grid_Flag::EnableContact); else{
          continue;
        };

        _vf oCircle;
        _vf oDirection;
        CPC_Circle_Square(
          NewPosition,
          CircleData->Size,
          {CircleGridX * GridBlockSize + GridBlockSize_D2, CircleGridY * GridBlockSize + GridBlockSize_D2},
          GridBlockSize_D2,
          &oCircle,
          &oDirection);

        #ifdef ETC_BCOL_set_PostSolve_Grid
          ContactResult_Grid_t ContactResult;
        #endif
        #ifdef ETC_BCOL_set_PostSolve_Grid_CollisionNormal
          ContactResult.Normal = oDirection;
        #endif
        #ifdef ETC_BCOL_set_PostSolve_Grid
          this->PostSolve_Grid_cb(
            this,
            ObjectID,
            ShapeEnum_t::Circle,
            ShapeData->ShapeID,
            {CircleGridX, CircleGridY},
            &ContactResult);
          if(this->ObjectList.CheckSafeNext(0) != ObjectID){
            goto gt_Object0Unlinked;
          }
        #endif

        WantedPosition += oCircle;
        WantedDirection += oDirection;
        WantedCollisionRequesters++;
      }
    }
  }
#endif

#if ETC_BCOL_set_DynamicToDynamic == 1
  TraverseObjects_t TraverseObjects_;
  this->TraverseObjects_init(&TraverseObjects_);
  while(this->TraverseObjects_loop(&TraverseObjects_)){
    auto ObjectID_ = TraverseObjects_.ObjectID;
    if(ObjectID_ == ObjectID){
      continue;
    }
    auto ObjectData_ = this->GetObjectData(ObjectID_);

    TraverseObject_t TraverseObject_;
    this->TraverseObject_init(&TraverseObject_);
    while(this->TraverseObject_loop(ObjectID_, &TraverseObject_)){
      auto ShapeID_ = TraverseObject_.ShapeID;
      auto ShapeData_ = this->GetObject_ShapeData(ObjectID_, ShapeID_);

      switch(ShapeData_->ShapeEnum){
        case ShapeEnum_t::Circle:{
          auto CircleData_ = this->ShapeData_Circle_Get(ShapeData_->ShapeID);

          _vf WorldPosition = ObjectData_->Position + CircleData_->Position;

          _vf Difference = NewPosition - WorldPosition;
          _f Hypotenuse = Difference.hypotenuse();
          _f CombinedSize = CircleData->Size + CircleData_->Size;
          if(Hypotenuse >= CombinedSize){
            break;
          }

          Contact_Shape_t Contact;
          this->PreSolve_Shape_cb(
            this,
            ObjectID,
            ShapeEnum_t::Circle,
            ShapeData->ShapeID,
            ObjectID_,
            ShapeEnum_t::Circle,
            ShapeID_,
            &Contact);
          if(ObjectList.CheckSafeNext(0) != ObjectID){
            goto gt_Object0Unlinked;
          }
          if(Contact.Flag & Contact_Shape_Flag::EnableContact); else{
            break;
          };

          if(Hypotenuse != 0){
            _vf Direction = Difference / Hypotenuse;
            WantedPosition += NewPosition + Direction * (CombinedSize - Hypotenuse);
            WantedDirection += Direction;
            WantedCollisionRequesters++;
          }
          break;
        }
        case ShapeEnum_t::Rectangle:{
          auto RectangleData_ = this->ShapeData_Rectangle_Get(ShapeData_->ShapeID);

          _vf WorldPosition = ObjectData_->Position + RectangleData_->Position;

          CPCU_Circle_Rectangle_t CData;
          CPCU_Circle_Rectangle_Pre(
            NewPosition,
            CircleData->Size,
            WorldPosition,
            RectangleData_->Size,
            &CData);

          if(!CPCU_Circle_Rectangle_IsThereCollision(&CData)){
            break;
          }

          Contact_Shape_t Contact;
          this->PreSolve_Shape_cb(
            this,
            ObjectID,
            ShapeEnum_t::Circle,
            ShapeData->ShapeID,
            ObjectID_,
            ShapeEnum_t::Rectangle,
            ShapeID_,
            &Contact);
          if(ObjectList.CheckSafeNext(0) != ObjectID){
            goto gt_Object0Unlinked;
          }
          if(Contact.Flag & Contact_Shape_Flag::EnableContact); else{
            break;
          };

          _vf oPosition;
          _vf oDirection;
          CPCU_Circle_Rectangle_Solve(
            NewPosition,
            CircleData->Size,
            WorldPosition,
            RectangleData_->Size,
            &CData,
            &oPosition,
            &oDirection);

          WantedPosition += oPosition;
          WantedDirection += oDirection;
          WantedCollisionRequesters++;

          break;
        }
      }
    }
  }
#endif

if(WantedCollisionRequesters){
  WantedObjectPosition += WantedPosition - CircleData->Position * WantedCollisionRequesters;
  WantedObjectDirection += WantedDirection;
  WantedObjectCollisionRequesters += WantedCollisionRequesters;
}

break;
