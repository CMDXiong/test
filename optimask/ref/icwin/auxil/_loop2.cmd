! See _LOOP.CMD for additional comments
!
! The default loop.op uses the SHELL command so you can modify each editable 
! subcell.  You must type RETURN to continue to the next subcell.  You can
! exit the loop at any time by typing:
!
!     #loop.quit=1
!     return
!
view off;
global loop.n = 1;
default local #safe.op = &
   "XSELECT OFF; UNSELECT ALL; UNPROTECT ALL; UNBLANK ALL;"
default local #loop.op = "VIEW ALL; SHELL";
!
! The default #loop.level=3.0 means only edit cells in direct edit directories
!
default local #loop.level=3.0
if(%loop.level<1) #loop.level = 1;
if(%loop.level>3) #loop.level = 3;
!
! By default, use the highest valid edit mode for each subcell.  If 
! #loop.level=3, only #loop.edit.mode.3 will be used.  The empty string 
! implies direct edit mode.
!
default local #loop.edit.mode.1="VIEW_ONLY=TRUE"
default local #loop.edit.mode.2="LOCAL_COPY=TRUE"
default local #loop.edit.mode.2.5="CHECK_OUT=TRUE"
default local #loop.edit.mode.3=""
local #edit.level=0;
local #depth=0;
local #cell.no ={cell(%cell)};

remove #loop.quit

mark_subcells
!
! The values for the cell.depth.* macro are computed by the mark_subcells
! command, which must precede the following line.
!
local #max.depth=%cell.depth.%cell.no;
!
! _LOOP2.CMD visit cells with cell.depth==0 (i.e. cells that don't contain 
! any subcells) first.  Then it visits cells with cell.depth==1 (i.e cells 
! that only contain subcells with depth==0) and so on.
!
while(%depth < %max.depth){
   #loop.n = 1;
   while(%loop.n <= %max.cell){
      #edit.level = %subcell.edit.%loop.n;
      if(macro_exists(#loop.quit)) return
      if(%edit.level>=%loop.level && %cell.depth.%loop.n==%depth){
         edit cell %cell.name.%loop.n %loop.edit.mode.%edit.level
         %safe.op;
         %loop.op;
         leave;
      }
      #loop.n = {%loop.n + 1};
   }
   #depth = {%depth+1};
}
!
! Do main cell last
!
%safe.op ! top level cell
%loop.op 
