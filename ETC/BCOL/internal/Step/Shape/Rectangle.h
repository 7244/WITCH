auto RectangleData = this->ShapeData_Rectangle_Get(ShapeData->ShapeID);

_vf NewPosition = NewObjectPosition + RectangleData->Position;

_vf WantedPosition = 0;
_vf WantedDirection = 0;
_f WantedCollisionRequesters = 0;

#if ETC_BCOL_set_SupportGrid == 1
  const _f GridBlockSize = this->GridBlockSize;
  const _f GridBlockSize_D2 = GridBlockSize / 2;

  sint32_t RectangleMiddleGridY = fan::math::floor(NewPosition.y / GridBlockSize);

  {
    _f RectangleLeftX = NewPosition.x - RectangleData->Size.x;
    _f RectangleRightX = NewPosition.x + RectangleData->Size.x;
    sint32_t RectangleLeftGridX = fan::math::floor(RectangleLeftX / GridBlockSize);
    sint32_t RectangleRightGridX = fan::math::floor(RectangleRightX / GridBlockSize);
    for(sint32_t RectangleGridX = RectangleLeftGridX; RectangleGridX <= RectangleRightGridX; RectangleGridX++){
      Contact_Grid_t Contact;
      Contact.Flag = 0;
      this->PreSolve_Grid_cb(
        this,
        ObjectID,
        ShapeEnum_t::Rectangle,
        ShapeData->ShapeID,
        {RectangleGridX, RectangleMiddleGridY},
        &Contact);
      if(Contact.Flag & Contact_Grid_Flag::EnableContact); else{
        continue;
      };
      _vf oRectangle;
      _vf oDirection;
      CPC_Rectangle_Square(
        NewPosition,
        RectangleData->Size,
        {RectangleGridX * GridBlockSize + GridBlockSize_D2, RectangleMiddleGridY * GridBlockSize + GridBlockSize_D2},
        GridBlockSize_D2,
        &oRectangle,
        &oDirection);
      WantedPosition += oRectangle;
      WantedDirection += oDirection;
      WantedCollisionRequesters++;
    }
  }

  {
    _f RectangleTopY = NewPosition.y - RectangleData->Size.y;
    sint32_t RectangleTopGridY = fan::math::floor(RectangleTopY / GridBlockSize);
    for(sint32_t RectangleGridY = RectangleMiddleGridY; RectangleGridY > RectangleTopGridY;){
      _f RectangleLeftX = NewPosition.x - RectangleData->Size.x;
      _f RectangleRightX = NewPosition.x + RectangleData->Size.x;
      sint32_t RectangleLeftGridX = fan::math::floor(RectangleLeftX / GridBlockSize);
      sint32_t RectangleRightGridX = fan::math::floor(RectangleRightX / GridBlockSize);
      RectangleGridY--;
      for(sint32_t RectangleGridX = RectangleLeftGridX; RectangleGridX <= RectangleRightGridX; RectangleGridX++){
        Contact_Grid_t Contact;
        this->PreSolve_Grid_cb(
          this,
          ObjectID,
          ShapeEnum_t::Rectangle,
          ShapeData->ShapeID,
          {RectangleGridX, RectangleGridY},
          &Contact);
        if(Contact.Flag & Contact_Grid_Flag::EnableContact); else{
          continue;
        };
        _vf oRectangle;
        _vf oDirection;
        CPC_Rectangle_Square(
          NewPosition,
          RectangleData->Size,
          {RectangleGridX * GridBlockSize + GridBlockSize_D2, RectangleGridY * GridBlockSize + GridBlockSize_D2},
          GridBlockSize_D2,
          &oRectangle,
          &oDirection);
        WantedPosition += oRectangle;
        WantedDirection += oDirection;
        WantedCollisionRequesters++;
      }
    }
  }

  {
    _f RectangleBottomY = NewPosition.y + RectangleData->Size.y;
    sint32_t RectangleBottomGridY = fan::math::floor(RectangleBottomY / GridBlockSize);
    for(sint32_t RectangleGridY = RectangleMiddleGridY; RectangleGridY < RectangleBottomGridY;){
      RectangleGridY++;
      _f RectangleLeftX = NewPosition.x - RectangleData->Size.x;
      _f RectangleRightX = NewPosition.x + RectangleData->Size.x;
      sint32_t RectangleLeftGridX = fan::math::floor(RectangleLeftX / GridBlockSize);
      sint32_t RectangleRightGridX = fan::math::floor(RectangleRightX / GridBlockSize);
      for(sint32_t RectangleGridX = RectangleLeftGridX; RectangleGridX <= RectangleRightGridX; RectangleGridX++){
        Contact_Grid_t Contact;
        this->PreSolve_Grid_cb(
          this,
          ObjectID,
          ShapeEnum_t::Rectangle,
          ShapeData->ShapeID,
          {RectangleGridX, RectangleGridY},
          &Contact);
        if(Contact.Flag & Contact_Grid_Flag::EnableContact); else{
          continue;
        };
        _vf oRectangle;
        _vf oDirection;
        CPC_Rectangle_Square(
          NewPosition,
          RectangleData->Size,
          {RectangleGridX * GridBlockSize + GridBlockSize_D2, RectangleGridY * GridBlockSize + GridBlockSize_D2},
          GridBlockSize_D2,
          &oRectangle,
          &oDirection);
        WantedPosition += oRectangle;
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

          CPCU_Rectangle_Circle_t CData;
          CPCU_Rectangle_Circle_Pre(
            NewPosition,
            {RectangleData->Size.x, RectangleData->Size.y},
            WorldPosition,
            CircleData_->Size,
            &CData);

          if(!CPCU_Rectangle_Circle_IsThereCollision(&CData)){
            break;
          }

          Contact_Shape_t Contact;
          this->PreSolve_Shape_cb(
            this,
            ObjectID,
            ShapeEnum_t::Rectangle,
            ShapeData->ShapeID,
            ObjectID_,
            ShapeEnum_t::Circle,
            ShapeID_,
            &Contact);
          if(Contact.Flag & Contact_Shape_Flag::EnableContact); else{
            break;
          };

          _vf oPosition;
          _vf oDirection;
          CPCU_Rectangle_Circle_Solve(
            NewPosition,
            RectangleData->Size,
            WorldPosition,
            CircleData_->Size,
            &CData,
            &oPosition,
            &oDirection);

          WantedPosition += oPosition;
          WantedDirection += oDirection;
          WantedCollisionRequesters++;

          break;
        }
        case ShapeEnum_t::Rectangle:{
          /* TODO */
          break;
        }
      }
    }
  }
#endif

if(WantedCollisionRequesters){
  WantedObjectPosition += WantedPosition - RectangleData->Position * WantedCollisionRequesters;
  WantedObjectDirection += WantedDirection;
  WantedObjectCollisionRequesters += WantedCollisionRequesters;
}

break;
