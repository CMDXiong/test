!*****************
!     uned.cmd
!*****************
!
! The following two block-if's are a patch for 
! obsolete .CMD files. Do not remove!
!
if(macro_exists(ed.cmd)){
   global #ed.file.name=%ed.cmd
   remove #ed.cmd
}
if(macro_exists(#uned.cmd)){
   global #uned.file.name=%uned.cmd
   remove #uned.cmd
}
!
! Check uned.enable flag
!
default global #uned.enabled=0;
if(%uned.enabled==0) error UNED uninitialized or disabled 
!
! Save select state of any components currently selected
!
local #n.select.pop = %n.select
if(%n.select.pop!=0) unselect push
!
! Undoing ed.cmd involves the following steps:
!
! 1: Delete components added by %ed.file.name.
! 2: Add the components deleted by ed.cmd by executing 
!    the commands in %uned.file.name. Update ed.id0 and
!    ed.id1 to record the range of id_numbers for the 
!    added components.
! 3: Update the macros ed.file.name, and uned.file.name,  
!    i.e. swap the roles of the files named by these macros.
!
if(%ed.id1==-1){ ! Implies ed.cmd crashed before 
   ! updating ed.id1.
   ! Normally, components with id_numbers in the range:
   !
   !               %ed.id0 < id_number <= %ed.id1
   !
   ! were added during the execution of ed.cmd.  
   ! But we got here because ed.cmd crashed before
   ! updating ed.id1.  We must patch things up as best 
   ! we can.  We will set ed.id1 equal to the current 
   ! value of id.max.  This means we will delete all 
   ! components added after the start of ed.cmd 
   ! instead of just deleting components added during
   ! the execution of ed.cmd.
   !
   #ed.id1=%id.max
   !
   if(%ed.id1>%ed.id0){
      select ids after %ed.id0
      !
      ! To prepare for future calls to uned.cmd, we
      ! must create a .CMD file that can replace the
      ! components we are about to delete. The string
      ! "do_nothing" is a flag for the next execution 
      ! of uned.cmd.  
      !
      if(%n.select>0){
         show file="%ed.file.name"
         unselect all
      }
      else #ed.file.name=do_nothing
   }
   else #ed.file.name=do_nothing
}
!
! Step 1: Delete components added by %ed.file.name.
!
select ids after %ed.id0
unselect ids after %ed.id1
xselect off
if(cmp("%ed.file.name", "uninitialized")==0){
   #ed.file.name = %tmp^ed.file
   show file="%ed.file.name"
}
delete
!
! Step 2: Replace comonents deleted by ed.cmd.
!   Update macros ed.id0 and ed.id1 for use in
!   future calls to uned.cmd.
!
! The macro uned.file.name normally holds a file name.  
! It may also contain the words "do_nothing". Executing
! the command "@do_nothing" does nothing. 
!
#ed.id0=%id.max
@"%uned.file.name"; log screen=off
#ed.id1=%id.max
!
! Step 3:  Prepare for future calls to uned.cmd by 
!  swapping the roles of the files named in 
!  ed.file.name and uned.file.name.  
!
local #junk=%ed.file.name
#ed.file.name="%uned.file.name"
#uned.file.name=%junk
!
! Restore state of components selected at the beginning of this .CMD file
!
if(%n.select.pop!=0) unselect pop
