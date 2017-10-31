!--------------------------------------------------------------------------
! Technology information
!---------------------------------------------------------------------------
!
local #cont.to.edge = 1;
local #cont.to.cont = 1;
local #cont.size = 2;
local #cont.step = {%cont.to.cont + %cont.size};
local #cont.cell = ZZ;
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
   (%cont.to.edge + (%cont.dx-%cont.nx*%cont.step) / 2, &
    %cont.to.edge + (%cont.dy-%cont.ny*%cont.step) / 2)}
!
!--------------------------------------------------------------------------
! Set up for uned.cmd -- Part I
!--------------------------------------------------------------------------
!
global #ed.id0 = %id.max
global #ed.file.name = mkcont.out
global #uned.file.name = "do nothing"
!
!--------------------------------------------------------------------------
! Save initial select state
!--------------------------------------------------------------------------
!
local #n.select.pop = %n.select
if(%n.select.pop!=0) unselect push
!
!---------------------------------------------------------------------------
! Add array
!---------------------------------------------------------------------------
!
add array=%cont.cell n=(%cont.nx, %cont.ny) step=(%cont.step, %cont.step) &
   offset=round(%cont.offset) at pos1(%cont.box)
!
!---------------------------------------------------------------------------
! Ungroup array ************ YOU MAY WANT TO REMOVE THESE LINES ************ 
!---------------------------------------------------------------------------
!
select new; ungroup;
select new; ungroup;
!
!--------------------------------------------------------------------------
! Set up for uned.cmd -- Part II
!--------------------------------------------------------------------------
!
global #ed.id1 = %id.max
select ids after %ed.id0
unselect ids after %ed.id1
show file=%ed.file.name
!
!--------------------------------------------------------------------------
! Restore initial select state
!--------------------------------------------------------------------------
!
if(%n.select.pop!=0) unselect pop
else unselect all
