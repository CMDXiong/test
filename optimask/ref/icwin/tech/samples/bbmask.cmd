! Excessively careful computation of the current cell's bounding box for 
! maskable components.  Only boxes, polygons and wires on mask layers 
! contribute to the result.
!
! The result placed in the global macro:
!
global #mask.bounds=""
!
!********************** Technology Dependent Parameters ***********************
!
! Each layer (except layer 0) must be either a mask.layer or a non.mask.layer.
!
! Example:
!   local #mask.layers=1:3+7:21
!   local #non.mask.layers=4:6+22:255
!
! If all layers are mask.layers use:
!   local #mask.layers=1:255
!   local #non.mask.layers=""
!
    local #mask.layers=1:255    ! You will probably want to change 
    local #non.mask.layers=""   ! these two lines
!
! The cell %scratch.cell is used for scratch work.  Be sure to use a cell name 
! that isn't going to be used for anything else.
!
   global #scratch.cell = "$scratch$" 
!
!     Note the quotes around $scratch$.  
!     They are required so ICED won't be confused by the initial $.  While 
!     quoting a macro string is rarely required, it doesn't hurt either -- 
!     so when in doubt "quote it".
!
! %eps is a small non-zero number that defines the "edge width"
!
   local #eps=.01     ! You may want to change this line
!
! Normally bbmask only ungroups cells near the edge of the chip.  This is
! usually the fastest procedure.  However, if there are large numbers of
! small cells at the chip edge that don't contain any "maskable" components
! this can be very slow.  In order to avoid this case we specify a maximum
! number of passes after which bbmask ungroups all cells instead of just
! edge cells
!
   local #max.pass=10 ! You may want to change this line
!
!******************************************************************************
!
log screen=off
!
xselect off;
unblank all; unprotect all; unselect all;

select layers 0+%mask.layers all
show select file=%tmp^work.cmd
unselect all

edit cell %scratch.cell
!
! Delete anything left over from previous runs
!
unblank all; unprotect all; select all; delete;
@%tmp^work.cmd
!
! We want to make sure that the bounding box isn't being determined by 
! unmaskable components in an "edge cell".  We will work until all edge.ok
! flags are 1 (true).  For now, preset them to 0 (false).
!
local #edge.ok.1 = 0; ! left 
local #edge.ok.2 = 0; ! top
local #edge.ok.3 = 0; ! right
local #edge.ok.4 = 0; ! bottom
!
local #pass = 0
!
while(1){
   select text all;
   select line all;
   if(cmp("%non.mask.layers", "")!=0) select layers %non.mask.layers-0 all
   delete

   #mask.bounds = %cell.box   
   #pass = {%pass+1}
   $$ Cell: %cell   Bounds: %mask.bounds  Pass: %pass
   local #x0={x0(%mask.bounds)};
   local #y0={y0(%mask.bounds)};
   local #x1={x1(%mask.bounds)};
   local #y1={y1(%mask.bounds)};
   !
   ! Define narrow boxes along edges of cell
   !
   local #edge.1 = (%x0, %y0) ({%x0+%eps}, %y1) ! left
   local #edge.2 = (%x0  {%y1-%eps}) (%x1, %y1) ! top
   local #edge.3 = ({%x1-%eps}, %y0) (%x1, %y1) ! right
   local #edge.4 = (%x0, %y0) (%x1, {%y0+%eps}) ! bottom
   !
   ! An edge is ok if there is a maskable component that overlaps it
   !
   local #i = 1
   while(%i <= 4){
      if(%edge.ok.%i==0){
         select layers %mask.layers-0 in %edge.%i
         unselect text all
         unselect line all
         #edge.ok.%i = {%n.select!=0} ! Note {}'s are required so that ICED 
            ! knows this is an expression.  If you leave them out ICED will
            ! interpret the line as "#edge.ok.%i = %n.select" and the ! in
            ! !=0 as the start of a comment.
         unselect all
      }
      #i = {%i + 1}
   }
   !
   ! We are done if all four edges are ok
   !
   if(%edge.ok.1+%edge.ok.2+%edge.ok.3+%edge.ok.4 == 4){ ! only loop exit
      quit y; ! quite cell without saving
      return; $ Cell: %cell   Bounds: %mask.bounds  %pass passes required
   }
   !
   ! Not done - select and ungroup cells that overlap edges
   !
   local #i = 1
   if(%pass <= %max.pass){
      while(%i <= 4){
         if(%edge.ok.%i==0) select cell * in %edge.%i
         #i = {%i + 1}
      }
   }
   else select cell * all;
   if(%n.select==0) error "No maskable components?"
   ungroup; pack;
}
