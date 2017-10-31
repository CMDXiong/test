!**************************** ABOUT DRCNOW.CMD ****************************
!
! This command file was patterned after PLOTNOW.CMD.  It is far more 
! verbose than necessary.  It is written this way to make it easy to create 
! custom command files.  All you have to do is add lines like:
!
!      local #drc.opt.rules          = "CMOS018";
!      local #drc.opt.pass.mode      = "slow"
!      local #drc.opt.check.rul.file = "" !**** Accept drc command default ****
!      local #drc.opt.ram            = "16384"
!
! at the beginning of the file and you will not be prompted for the 
! associated parameters.
!
! To create a sample list of "drc.opt" macro values remove the ! from the
! "! SHOW USER #DRC.OPT.* FILE=DRC_OPT.SHO" near the end of this file.  Then
! run drcnow.cmd.
!
! Sample contents of DRC_OPT.SHO:
!
!    LOCAL DRC.OPT.CHECK.RUL.FILE="no_rul"
!    LOCAL DRC.OPT.LEVEL="0"
!    LOCAL DRC.OPT.PASS.MODE="slow"
!    LOCAL DRC.OPT.POK.MODE=""
!    LOCAL DRC.OPT.RAM="16384"
!    LOCAL DRC.OPT.RULES="C:\ICWIN\AUXIL\EX2"
!    LOCAL DRC.OPT.RUN.MINIMIZED="1"
!     
!***************************************************************************

view off

default local #drc.opt.rules = $menu _drcnow:rules_list
default local #drc.opt.level = $menu _drcnow:option_levels
!
!  menu entry => drc.opt.level 
!       "all" => 3
!      "most" => 2 (no longer used)
!      "some" => 1
!      "none" => 0
!
if(%drc.opt.level<=2){ ! "most" menu option returns 2
   default local #drc.opt.pass.mode   = "slow"
   default local #drc.opt.run.minimized = 1;
   if(%drc.opt.run.minimized) default local #drc.opt.check.rul.file = "no_rul"
   else                       default local #drc.opt.check.rul.file = ""
   if(%drc.opt.level<=1){ ! "some" menu option returns 1
      if(%drc.opt.level==0){ ! "none" menu option returns 0
         default local #drc.opt.pok.mode = "" ! Whole chip
      }
   }
   if(%drc.opt.level==0){ ! "none" menu option returns 0
      default local #drc.opt.ram = {16 * 1024}
   }
}
default local #drc.opt.pok.mode = $menu _drcnow:pok_modes
default local #drc.opt.pass.mode = $menu _drcnow:pass_modes
default local #drc.opt.run.minimized = $menu _drcnow:window_mode

if(%drc.opt.run.minimized) local #drc.opt.pause = "never"; 
else local #drc.opt.pause = "always"; 

default local #drc.opt.check.rul.file = $menu _drcnow:rules_check_1

if(macro_exists(drc.opt.ram)==0){
   local #drc.opt.ram = $menu _drcnow:ram
   if(cmp("%drc.opt.ram", "%")==0){
      #drc.opt.ram = $prompt "How many Megs of RAM DRC should use?"
      #drc.opt.ram = {1024 * %drc.opt.ram}
   }
}
local #drc.opt.in = cmp(%drc.opt.pok.mode, "IN")==0;
if(%drc.opt.in){
   local #in_box = $PROMPT='Use mouse to define "in box"' BOX
   #drc.opt.pok.mode = "IN %in_box";
}
!
! Set up for undo (undrcnow.cmd)
!
global #drc.undo.mode = -1
!
! Create .POK file
!
drc rules="%drc.opt.rules" %drc.opt.pok.mode
!
! Run DRC
!
local #win.flag="-"
if(%drc.opt.run.minimized) #win.flag="^-"

local #go = 1;
while(%go){
   #go = 0;
   remove #drc.log.*
   local #t0 = %timer;
   dos '%win.flag drc3 "%drc.opt.rules" %cell %cell^.now &
        use=%drc.opt.ram &
        pause=%drc.opt.pause &
        %drc.opt.pass.mode &
        %drc.opt.check.rul.file &
        iced'
   if(file_exists("drccrash.log")){
      @"drccrash.log"
      global #drc.log.crash.message = %drccrash.message
      global #drc.log.crash.code = %drccrash.code
      if(macro_exists(drccrash.log.file)==0) remove #drc.log.file
      else              global #drc.log.file = %drccrash.log.file
      if(macro_exists(drccrash.rules.compile)==0) remove #drc.log.rules.compile
      else              global #drc.log.rules.compile = %drccrash.rules.compile
      remove drccrash.*
      if(macro_exists(#drc.log.rules.compile)){
         pause "%drc.log.crash.message";
         @_get_ans; #prompt="Do you want to recompile [Yn]?"; &
                    #choices="Yn";  #default="Y";
         if(%ret.value==2){
            error "Command cancelled"
         }
         dos '^%drc.log.rules.compile'
         #go = 1;
      }
      else if(%drc.log.crash.code==2){
         @_get_ans; #prompt="%drc.log.crash.message.  Continue anyway [Yn]?" &
                    #choices="Yn";  #default="Y";
         if(%ret.value==2) error "Command Cancelled"
         #drc.opt.check.rul.file = "no_rul";
         #go = 1;
      }
      else{
         error "DRC CRASHED -- %drc.log.crash.message";
      }
   }
}
!
! Run output DRC output command file = %cell.now
!
@%cell^.now 
global #drc.log.timer = {%timer-%t0}
view on;
!
! Remove the ! from the following show command to write the options you 
! have chosen to drc_opt.sho
! 
! SHOW USER #DRC.OPT.* FILE=DRC_OPT.SHO  
!
! Set up to save drc.* macros when exiting ICED or cell
!
global #iced.exit.root=@_exit_iced.cmd
global #iced.exit.subcell=@_exit_iced.cmd
remove #key.drc.macros
if(%drc.log.error.count==0){
   if(%drc.log.bad.poly.layer==0 || %drc.log.bad.polygons==0){
      $ No errors found.    Run time=%drc.log.timer Sec.
   }
   else{
      $ %drc.log.bad.polygons bad polygons.  No other errors.  Run time=%drc.log.timer Sec.
   }
}
else{
   if(%drc.log.bad.poly.layer==0 || %drc.log.bad.polygons==0){
      $ Raw error count=%drc.log.error.count^.  Run time=%drc.log.timer Sec.
   }
   else{
      $ %drc.log.bad.polygons bad polygons.  Raw error count=%drc.log.error.count^.  Run time=%drc.log.timer Sec.
   }
}
if(%drc.opt.in){
   local #pop = %n.select
   if(%pop!=0) unselect push;
   select ids after %drc.undo.id0 all
   unselect %drc.opt.pok.mode
   if(%n.select!=0) delete
   if(%pop) select pop
}

