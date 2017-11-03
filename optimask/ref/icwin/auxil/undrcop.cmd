!************************
!     undrcop.cmd
!************************
!
! This command file is used to undo DRCOPs.  DRCOP's are performed by 
! BOOLOP.CMD, MERGEOP.CMD, BLOATOP.CMD, etc..  UNDRCOP.CMD is patterned 
! after UNED.CMD.
!
! Check undop.enabled flag
!
default global #undop.enabled=0;
if(%undop.enabled==0) error UNDRCOP uninitialized or disabled 
!
! Save select state of any components currently selected
!
local #n.select.pop = %n.select
if(%n.select.pop!=0) unselect push
!
! Undoing undrcop.cmd involves the following steps:
!
! 1: Delete components added by %dop.file.name.
! 2: Add the components deleted after the DRCOP by executing 
!    the commands in %undop.file.name. Update dop.id0 and
!    dop.id1 to record the range of id_numbers for the 
!    added components.
! 3: Update the macros dop.file.name, and undop.file.name,  
!    i.e. swap the roles of the files named by these macros.
!
! Step 1: Delete components added by %dop.file.name.
!
select ids after %dop.id0
unselect ids after %dop.id1
xselect off
delete
!
! Step 2: Replace comonents deleted by dop.cmd.
!   Update macros dop.id0 and dop.id1 for use in
!   future calls to undop.cmd.
!
! The macro undop.file.name normally holds a file name.  
! If, instead, it contains the words "do nothing" then
! we shouldn't execute the @%undop.file.name command.
!
#dop.id0=%id.max
if(cmp("%undop.file.name", "do nothing")!=0){
   @"%undop.file.name"; log screen=off
}
#dop.id1=%id.max
!
! Step 3:  Prepare for future calls to undrcop.cmd by 
!  swapping the roles of the files named in 
!  dop.file.name and undop.file.name.  
!
local #junk=%dop.file.name
#dop.file.name="%undop.file.name"
#undop.file.name=%junk
!
! Restore state of components selected at the beginning of this .CMD file
!
if(%n.select.pop!=0) unselect pop
