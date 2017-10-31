!--------------------------------------------------------------------------
! Technology information
!---------------------------------------------------------------------------
!
local #cont.to.edge = 1;
local #cont.to.cont = 1;
local #cont.size = 2;
local #cont.step = {%cont.to.cont + %cont.size};
!
!---------------------------------------------------------------------------
! User inputs contact area
!---------------------------------------------------------------------------
!
local #cont.box $prompt="Use mouse to outline contact area" box;
!
!---------------------------------------------------------------------------
! Compute array constants
!---------------------------------------------------------------------------
!
local #cont.edge.correction = {2*%cont.to.edge-%cont.to.cont};
local #cont.dx = {x1(%cont.box)-x0(%cont.box)-%cont.edge.correction};
local #cont.dy = {y1(%cont.box)-y0(%cont.box)-%cont.edge.correction};
local #cont.nx = {int(%cont.dx / %cont.step)};
local #cont.ny = {int(%cont.dy / %cont.step)};
local #cont.offset = { & 
   %cont.to.edge + (%cont.dx-%cont.nx*%cont.step) / 2, & 
   %cont.to.edge + (%cont.dy-%cont.ny*%cont.step) / 2};
!
!---------------------------------------------------------------------------
! Add array
!---------------------------------------------------------------------------
!
unselect push;
local #nx = 0;
local #ny = 0;
local #x.offset = 0;
local #y.offset = 0;

while(%nx < cont.nx){
   #y.offset = 0;
   while(%ny < cont.ny){
      add box layer %cont.layer &
         offset={%cont.offset + (%x.offset, %y.offset)} &
         at (0, 0) (%cont.size, %cont.size)
      #ny = {%ny + 1};
      #y.offset = {%y.offset + %cont.step};
   }
   #nx = {%nx + 1};
   #x.offset = {%x.offset + %cont.step};
}
add array=%cont.cell n=(%cont.nx, %cont.ny) step=(%cont.step, %cont.step) &
   offset=round(%cont.offset) at pos1(%cont.box)
!
!---------------------------------------------------------------------------
! Ungroup array ************ YOU MAY WANT TO REMOVE THESE LINES ************ 
!---------------------------------------------------------------------------
!
select new; ungroup;
select new; ungroup;
