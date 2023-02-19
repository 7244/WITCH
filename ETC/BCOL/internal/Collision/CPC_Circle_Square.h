void CPC_Circle_Square(
  _vf p0,
  _f p0Size,
  _vf p1,
  _f p1Size,
  _vf *op0,
  _vf *oDirection
){
  _vf Sp0 = p0 - p1;
  _vf SPp0 = Sp0.abs();
  if(SPp0.x > p1Size && SPp0.y > p1Size){
    _vf Corner = SPp0 - p1Size;
    _f Divider = fan::math::sqrt(Corner.y * Corner.y + Corner.x * Corner.x);
    _vf CircleOffset = Corner / Divider;
    *op0 = p1 + fan::copysign(CircleOffset * p0Size + p1Size, Sp0);
    *oDirection = fan::copysign(CircleOffset, Sp0);
  }
  else{
    if(SPp0.x <= p1Size){
      op0->y = p1.y + fan::copysign(p1Size + p0Size, Sp0.y);
      op0->x = p0.x;
      oDirection->y = fan::copysign(1, Sp0.y);
      oDirection->x = 0;
    }
    else if(SPp0.y <= p1Size){
      op0->y = p0.y;
      op0->x = p1.x + fan::copysign(p1Size + p0Size, Sp0.x);
      oDirection->y = 0;
      oDirection->x = fan::copysign(1, Sp0.x);
    }
  }
}
