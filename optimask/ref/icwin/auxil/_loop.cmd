! LOOP is intended to be launched with a command like:
!
! @LOOP; #LOOP.OP=@DO_SOMETHING_USEFUL
!
! which will DO_SOMETHING_USEFUL in the current cell and each direct editable 
! subcell of the current cell.
! 
VIEW OFF;
GLOBAL loop.n = 1;
!
! It's normally a good idea to execute the following commands in each subcell:
!
DEFAULT LOCAL #safe.op = &
   "XSELECT OFF; UNSELECT ALL; UNPROTECT ALL; UNBLANK ALL;"
!
! For test purposes:
!
DEFAULT LOCAL #loop.op = "VIEW ALL;  VIEW ON;  VIEW OFF;  PAUSE 1";

MARK_SUBCELLS
WHILE(%loop.n <= %max.cell){
   $$ subcell.edit.%loop.n=%subcell.edit.%loop.n
   if(%subcell.edit.%loop.n==3){
      EDIT CELL %cell.name.%loop.n;
      %safe.op;
      %loop.op;
      LEAVE;
   }
   #loop.n = {%loop.n + 1};
}
%safe.op ! top level cell
%loop.op ! top level cell

