!****************
! mergeop.cmd
!****************
!
! Uses the DRC to merge boxes, polygons, and (optionally) wires.
!
default local #dop.op.cut.res = %res.step

unselect text all
unselect lines all
unselect cell * all

while(%n.select==0){
   select layers 1:255 in
   unselect text all
   unselect lines all
}
local #n0.select = %n.select
unselect wires all
local #n.wires = {%n0.select - %n.select};
if(%n.wires!=0){
   undo
   select push
   unselect polygons all
   unselect text all
   $ "%n.wires wire(s) selected.  Convert to polygon(s)?"
   local #convert = $menu drc:convert_wires;
   if(%%convert==1){ ! Yes
      select pop
   }
   else if(%%convert==2){ ! No
      select pop
      unselect wires all
      if(%n.select==0){
         undo
         error "Nothing selected"
      }
   }
   else{ ! %%convert==3 => cancel
      error "Command cancelled"
   }
}
!
! Create rules file name
!
local #int.res = {int(1000*%dop.op.cut.res)}
local #rules.file.name = %tmp^$merge$%int.res
!
! Create and compile .BB file iff it doesn't already exist
!
if(file_exists("%rules.file.name^.BB")==0){
   dos '^mkrule MERGE "%rules.file.name^.RUL" %dop.op.cut.res'
   dos '^d3rules "%rules.file.name^.RUL" hog=2'
   if(file_exists("%rules.file.name^.BB")==0){
      error "Error compiling %rules.file.name^.RUL"
   }
}
local #t0 = %timer;
global #dop.file.name = %tmp^drcop.out
dos '^del "%dop.file.name"'
drc select
dos '^drc3 "%rules.file.name" %cell "%dop.file.name" quickpass hog=8 iced'
if(file_exists("drccrash.log")){
   @drccrash.log
   global #dop.log.crash.message = %drccrash.message
   global #dop.log.crash.code = %drccrash.code
   if(%dop.log.crash.code==4){ ! Recreate then recompile rules file and retry
      dos '^mkrule MERGE "%rules.file.name^.RUL" %dop.op.cut.res'
      dos '^d3rules "%rules.file.name^.RUL" hog=2'
      if(file_exists("%rules.file.name^.BB")==0){
         error "Error compiling %rules.file.name^.RUL"
      }
   }
   local #t0 = %timer;
   dos '^drc3 "%rules.file.name" %cell "%dop.file.name" quickpass hog=8 iced'
   if(file_exists(drccrash.log)){
      if(macro_exists(drccrash.log.file)==0) remove #dop.log.file
      else              global #dop.log.file = %drccrash.log.file
      remove drccrash.*
      error "DRC CRASHED -- %dop.log.crash.message";
   }
}
local #t1 = %timer;
global #dop.id0 = %id.max;
global #undop.file.name = "Do Nothing";
global #undop.enabled = 1;
local #cmd.failed = 0;
local #error.cmd = #cmd.failed = 1;
@%dop.file.name
if(%cmd.failed==1){
   !
   ! The only ways of getting here are if the .CMD files are nested 
   ! more than 32 deep or there is a bug in the DRC.
   !
   unselect push;
   #undop.enabled = 0;
   select ids after %dop.id0;
   if(%n.select!=0) delete
   select pop
   error 
}
global #dop.id1 = %id.max;
select parts all
show file="%tmp^undop.cmd"
#undop.file.name = %tmp^undop.cmd
delete
$ DRC run time={%t1-%t0}
