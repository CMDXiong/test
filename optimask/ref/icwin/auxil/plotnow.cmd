!**************************** ABOUT PLOTNOW.CMD ****************************
!
! This command file is far more verbose than necessary.  It is written
! this way to make it easy to create custom command files.  All you have
! to do is add lines like:
!
!      local #plot.opt.pdf    ="CJET300";
!      local #plot.opt.size   ="size=A"
!      local #plot.opt.pattern="pattern=SAMPLE"
!      local #plot.opt.rot    ="" !**** Accept PLOT command default ****
!      local #plot.opt.filter ="" !**** Accept PLOT command default ****
!      local #plot.opt.dev    ="LPT1:"
!
! at the beginning of the file and you will not be prompted for the 
! associated parameters.
!
! To create a sample list of "plot.opt" macro values remove the ! from the
! "! SHOW USER=plot.opt.* FILE=PLOT_OPT.SHO" at the end of this file.  Then
! run plotnow.cmd.
!
! Sample contents of PLOT_OPT.SHO:
!
!       LOCAL PLOT.OPT.DEV="\\db2\hp6l"
!       LOCAL PLOT.OPT.SCALE="all=*"
!       LOCAL PLOT.OPT.CELL_LABELS=""
!       LOCAL PLOT.OPT.TEXT_ORIENT=""
!       LOCAL PLOT.OPT.ARRAY_MODE=""
!       LOCAL PLOT.OPT.OUTLINE_DEPTH=""
!       LOCAL PLOT.OPT.CELL_DEPTH=""
!       LOCAL PLOT.OPT.FILTER=""
!       LOCAL PLOT.OPT.ROT=""
!       LOCAL PLOT.OPT.TEXT_HIGHLIGHTS="text_highlights=on"
!       LOCAL PLOT.OPT.NX_NY="nx=1 ny=1"
!       LOCAL PLOT.OPT.LAYERS=""
!       LOCAL PLOT.OPT.DOTS="dots=1"
!       LOCAL PLOT.OPT.SIZE="size=A"
!       LOCAL PLOT.OPT.PATTERN="pattern=NONE"
!       LOCAL PLOT.OPT.PDF="DJET300"
!       LOCAL PLOT.OPT.LEVEL="0"
!
! This file also saves the device name so that you only have to type it in
! once per edit session.
!
! A much simpler command file, which works almost as well, would be:
!
!     local #plot.ops=$menu m1:plotter_list
!     plot %plot.opts
!     spawn "mkplot %cell /d /i"
!     
!***************************************************************************

view off

default local #plot.opt.pdf     = $menu _plotnow:plotter_list
default local #plot.opt.pattern = $menu _plotnow:pattern_list
default local #plot.opt.size    = $menu _plotnow:size_list
default local #plot.opt.level   = $menu _plotnow:option_levels

if(%plot.opt.level<=2){ ! "most" menu option returns 2
   default local #plot.opt.filter        = "" ! => accept PLOT command default
   default local #plot.opt.cell_depth    = ""
   default local #plot.opt.outline_depth = ""
   default local #plot.opt.array_mode    = ""
   default local #plot.opt.text_orient   = ""
   default local #plot.opt.cell_labels   = ""

   if(%plot.opt.level<=1){ ! "some" menu option returns 1
      default local #plot.opt.layers    = ""
      default local #plot.opt.rot       = ""

      if(%plot.opt.level==0){ ! "none" menu option returns 0
         default local #plot.opt.text_highlights = "text_highlights=on"
         default local #plot.opt.nx_ny     = "nx=1 ny=1"
         default local #plot.opt.scale     = "all=*"
         default local #plot.opt.dots      = "dots=1"
      }
   }
}
   
default local #plot.opt.dots            = $menu _plotnow:dots_list
default local #plot.opt.layers          = $menu _plotnow:plot_layers
default local #plot.opt.nx_ny           = $menu _plotnow:nx_ny
default local #plot.opt.text_highlights = $menu _plotnow:halos
default local #plot.opt.rot             = $menu _plotnow:rotate
default local #plot.opt.filter          = $menu _plotnow:filter
default local #plot.opt.cell_depth      = $menu _plotnow:cell_depth
default local #plot.opt.outline_depth   = $menu _plotnow:outline_depth
default local #plot.opt.array_mode      = $menu _plotnow:array_mode
default local #plot.opt.text_orient     = $menu _plotnow:text_orient
default local #plot.opt.cell_labels     = $menu _plotnow:cell_labels
default local #plot.opt.scale           = $menu _plotnow:scale

default local #file = %tmp^%cell;
if(file_exists("%file^.vec")){
   local #nn=1
   while(file_exists("%file^-%nn^.vec")){
      #nn = {%nn + 1}
   }
   #file = %file^-%nn
}

if(macro_exists(#plot.now.dev.%plot.opt.pdf)){
   local #plot.opt.dev = %plot.now.dev.%plot.opt.pdf
}
else if(file_exists("%home.1^settings\%plot.opt.pdf^.cmd")){
   !
   ! The following .CMD file was written by a previous call to MKPLOT and 
   ! should contain a command to set #mkplot.dev.  The user may have editted 
   ! the .CMD file, so handle syntax errors.
   !
   local #error.cmd=remove(#mkplot.dev);
   @"%home.1^settings\%plot.opt.pdf^.cmd"
   remove error.cmd
   if(macro_exists(#mkplot.dev)) local #plot.opt.dev = %mkplot.dev
}
if(macro_exists(#plot.opt.dev)){
  @_get_dev;  #default = %plot.opt.dev;
}
else @_get_dev;
local #plot.opt.dev = %ret.value;

global #plot.now.dev.%plot.opt.pdf = %plot.opt.dev

plot plotter=%plot.opt.pdf %plot.opt.pattern %plot.opt.size %plot.opt.dots & 
   %plot.opt.layers %plot.opt.nx_ny %plot.opt.text_highlights %plot.opt.rot &
   %plot.opt.filter %plot.opt.cell_depth %plot.opt.outline_depth &
   %plot.opt.array_mode %plot.opt.text_orient %plot.opt.cell_labels &
   file="%file" %plot.opt.scale 

spawn '^mkplot "%file" "%plot.opt.dev" /i'

! SHOW USER=plot.opt.* FILE=PLOT_OPT.SHO
