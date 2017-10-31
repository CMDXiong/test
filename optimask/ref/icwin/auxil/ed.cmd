!*****************
!     ed.cmd
!*****************

remove #ed.cmd    ! Patch for obsolete .CMD files -- Do not remove
remove #uned.cmd  ! Patch for obsolete .CMD files -- Do not remove

global #ed.file.name = %tmp^work.cmd 
global #uned.file.name = %tmp^unwork.cmd 
global #ed.id0=%id.max
global #ed.id1=-1
global #uned.enabled=0
default local #editor="-notepad"
local  #failed=0;

select parts all
show file=%uned.file.name
select new
select parts all
show file=%ed.file.name
dos %editor %ed.file.name

local #error.cmd="#failed=1"
@%ed.file.name
#ed.id1=%id.max
if(%failed==0){
   #uned.enabled=1
   #ed.id1=%id.max
   delete
   return
}

while(1){
   !
   ! Undo botched job
   !
   unselect push
   select ids after %ed.id0 all
   xselect off
   delete
   select pop

   error  ! Post error
   pause
   !
   ! Ask if user wants to correct error
   !
   @_get_ans;        &
      #choices="YN"; &
      #prompt="Do you want to re-edit file [YN]?"; 
   if(%ret.value!=1) return;
   !
   ! Yes, he wants to
   ! Repost error so it will be visible while user edits file
   !
   error
   dos "%editor %ed.file.name"

   #failed=0
   local #error.cmd="#failed=1"
   #ed.id0=%id.max
   @%ed.file.name
   if(%failed==0){
      #uned.enabled=1
      delete
      #ed.id1=%id.max
      return;
   }
}   
