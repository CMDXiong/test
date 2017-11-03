#File: 		Script.tcl
#Function:	Tcl/Tk interface for indexer. The program indexes sgml and html files to an index file.
#		This index file can be either of html or sgml type
#Author:	St. Visser
#Company:	Hollandse Signaal Apparaten B.V.
#Date:		01/10/98

#Update:	20/11/98, by St. Visser
		
# Main window
wm title . "Indexer 1.2" 
wm geometry . 250x150
#*********************************************************************************
# Name:       dialog_position
# Function:   Calculates where to display the dialog
#*********************************************************************************
proc dialog_position { dlg } {

   set x [expr 50 + [winfo x .]]
   set y [expr 50 + [winfo y .]]

   wm geometry $dlg +$x+$y
}



#*********************************************************************************
# Name:       ReadSGMLlist
# Function:   Creates a dialog to draw a Polygon
#*********************************************************************************
proc ReadSGMLlist {} {
				global l_SGML
				global sgmdir
				set len [llength $l_SGML]
				for {set i 0} {$i < $len} {incr i} {
				set file [lindex $l_SGML $i] 
				addsgmlfile $file $sgmdir/ }

}

#*********************************************************************************
# Name:       SetResultFile File
# Function:   Creates a dialog to draw a Polygon
#*********************************************************************************
proc ChooseFile {} {


   global selected
   global IndexName
   global CreateFile
   global ResultFile
   global sgmdir

   toplevel .p -class Dialog
   wm title .p "Set Result File"
   wm transient .p .
   dialog_position .p


   foreach i {f1 f2 f3} {
      frame .p.$i -bd 2
      entry .p.$i.entry -relief sunken -width 25
      label .p.$i.label
      pack .p.$i.entry -side right
      pack .p.$i.label -side left
   }

   .p.f1.label config -text "Directory:"
   .p.f1.entry insert 0 $sgmdir
   .p.f2.label config -text "File name:"
   .p.f2.entry insert 0 $ResultFile
   .p.f3.label config -text "Index name:"
   .p.f3.entry insert 0 $IndexName


   pack .p.f1 .p.f2 .p.f3 -side top -fill x

   global Type
   radiobutton .p.html -text "HTML" -variable Type -value "HTML"
   radiobutton .p.sgml -text "SGML" -variable Type -value "SGML"
   pack .p.html .p.sgml

   frame .p.buttons
   pack .p.buttons -side bottom -fill x -pady 20
   button .p.buttons.ok -text "Ok" -command {   
		global l_SGML
		global selected
		global ResultFile
		global CreateFile
                global IndexName
                global sgmdir
		set sgmdir [.p.f1.entry get]
		set ResultFile [.p.f2.entry get]
		set CreateFile "$sgmdir/$ResultFile.$Type"
		set IndexName [.p.f3.entry get]
      destroy .p 
   }

   button .p.buttons.cancel -text "Cancel" -command {   
      destroy .p
   }
   pack .p.buttons.ok .p.buttons.cancel -side left -padx 5

   focus .p.f1.entry
   bind .p <Return> {.p.buttons.ok invoke}
}



#*********************************************************************************
# Name:       AddSgml
# Function:   Creates a dialog to select add an Files to the list
#*********************************************************************************

proc AddSgm {} {

   global l_SGMLselection
   global sgmdir
   global selected

   global sd_close
   set sd_close -1
   

   toplevel .d -class Dialog
   wm title .d "Choose SGML/HTML-Files"
   wm transient .d .
   dialog_position .d

#   set otherdir $sgmdir
   frame .d.currentdir -bd 2
 #   entry .d.currentdir.entry -relief sunken -width 20 -textvariable otherdir
   label .d.currentdir.label -text "Directory: $sgmdir" -padx 5 -pady 5
 #  button .d.currentdir.add -text "Read" -command { 
 #			.d.frame2.listbox delete 0 end
 #			set SgmFiles [glob -nocomplain "$otherdir/{*.htm,*.sgm,*.html,*.sgml}"]
 #  			if { $SgmFiles != "" } {
 #     		foreach SgmFilename $SgmFiles {
 #		            .d.frame2.listbox insert end [file tail $SgmFilename]
 #		      	}
 #  			}
 #  }

   pack .d.currentdir.label -side left
#  pack .d.currentdir.entry -side left
#  pack .d.currentdir.add -side right
#  bind .d.currentdir.entry <Return> { .d.currentdir.add invoke}
#   $d.currentdir.entry insert 0 $sgmdir
   pack .d.currentdir -side top -fill x 

   frame .d.frame1
   label .d.frame1.label -text "SGML files:"
   label .d.frame1.label2 -text "Selection:"
   pack .d.frame1.label  -side left -padx 10
   pack .d.frame1.label2  -side right -padx 10
   pack .d.frame1 -side top -fill x

   frame .d.frame2
   listbox .d.frame2.listbox -height 8 -width 20 -yscrollcommand ".d.frame2.scr set" -selectmode extended
   scrollbar .d.frame2.scr -command ".d.frame2.listbox yview"



   .d.frame2.listbox selection set 0


   listbox .d.frame2.listbox2 -height 8 -width 20 -yscrollcommand ".d.frame2.scr2 set" -selectmode single
   scrollbar .d.frame2.scr2 -command ".d.frame2.list2 yview"

   global l_SGML
   set len [llength $l_SGML]
   for {set i 0} {$i < $len} {incr i} {
			set file [lindex $l_SGML $i]
			.d.frame2.listbox2 insert end $file
   } 

   pack .d.frame2.listbox  -side left 
   pack .d.frame2.scr  -side left -fill y
   pack .d.frame2.listbox2  -side left 
   pack .d.frame2.scr2  -side left -fill y
   pack .d.frame2 -padx 10
   
   .d.frame2.listbox configure -background white 

   set SgmFiles [glob -nocomplain "$sgmdir/{*.htm,*.sgm,*.html,*.sgml}"]
   if { $SgmFiles != "" } {
      foreach SgmFilename $SgmFiles {
            .d.frame2.listbox insert end [file tail $SgmFilename]
      }
   }

   frame .d.frame3 
   pack .d.frame3 -side bottom

   button .d.frame3.ok -text "Ok" -command { set sd_close 1 } 
   button .d.frame3.cancel -text "Cancel" -command  { set sd_close 0 }
   button .d.frame3.add -text "Add" -command  { set test [.d.frame2.listbox curselection]
								set len [llength $test]
				
								for {set i 0} {$i <	 $len} {incr i} {
								   set Number [lindex $test $i]
								   set Item [.d.frame2.listbox get $Number]
								   .d.frame2.listbox2 insert end "$Item"
								}
   }
   button .d.frame3.del -text "Delete" -command { .d.frame2.listbox2 delete [.d.frame2.listbox2 curselection] }
								
   pack .d.frame3.ok .d.frame3.cancel .d.frame3.add .d.frame3.del -side left -pady 10 -padx 5


  focus .d
  tkwait variable sd_close
 
  if { $sd_close == 1 } { 				
				global l_SGML
				set l_SGML ""

		
 				set test [.d.frame2.listbox2 get 0 end]
				set len [llength $test]
				set lall [llength $l_SGML]

				if {$lall < $len} {
				for {set i $lall} {$i < $len} {incr i} {
							set Item [lindex $test $i]
							set l_SGML [linsert $l_SGML end $Item]
				  }
				}
				global Count
				set Count $len
				update
				lsort -ascii $l_SGML
#				if {$len > 0} { ReadSGMLlist }
   }
				
   destroy .d

}


#*********************************************************************************
# Global vars
#*********************************************************************************

set l_SGML ""


#*********************************************************************************
# Create status bar
#*********************************************************************************

set menustatus " "

frame .statusBar -borderwidth 0
label .statusBar.menustatus -textvariable menustatus -relief sunken -anchor w 
pack .statusBar.menustatus \
     -side left -padx 2 -expand yes -fill both
pack .statusBar -side bottom -fill x -pady 2

#*********************************************************************************
# Create menu
#*********************************************************************************




menu .menu -tearoff 0 

set m .menu.file
menu $m -tearoff 0
.menu add cascade -label "File" -menu $m -underline 0
$m add command -label "Exit" -underline 1 -accelerator "Ctrl+X" -command { destroy . }  

set s .menu.selection
menu $s -tearoff 0
.menu add cascade -label "Selection" -menu $s -underline 0
$s add command -label "SGML/HTML Files" -underline 11 -command { AddSgm }
									     
set r .menu.result
menu $r -tearoff 0
.menu add cascade -label "Result" -menu $r -underline 0
$r add command -label "Set Result File" -underline 0 -command { ChooseFile
									    .outfile -text selected 
}


bind . <Control-x> { destroy . }

. configure -menu .menu

set CreateFile "None"
set IndexName   "None"
set ResultFile "None"
set Type	   "HTML"
set Count      0


frame .info -relief groove -bd 3
frame .button -relief groove -bd 3

frame .info.fileinfo
frame .info.typeinfo
frame .info.selectedinfo

label .info.fileinfo.text -text "File: "
label .info.fileinfo.resultfile -textvariable ResultFile -text "None"  

label .info.typeinfo.text -text "Type: "
label .info.typeinfo.type -textvariable Type -text             "None" 

label .info.selectedinfo.text -text "Selected: "
label .info.selectedinfo.count -textvariable Count -text       "0"  

button .button.parse -text "Create"  -command { 
						ReadSGMLlist
						setfile $CreateFile $IndexName
						parsefile $Type $sgmdir}
label .button.file -textvariable CreateFile -text "None" -foreground black

pack .info.fileinfo.text .info.fileinfo.resultfile  -side left -anchor w
pack .info.typeinfo.text .info.typeinfo.type -side left -anchor w
pack .info.selectedinfo.text .info.selectedinfo.count  -side left -anchor w

pack .info.fileinfo .info.typeinfo .info.selectedinfo

pack .info -anchor w  -fill x
pack .button.parse .button.file -side left -padx 5 -pady 5
pack .button -anchor w -fill x 


bind Menu <<MenuSelect>> {
    global $menustatus
    if {[catch {%W entrycget active -label} label]} {
	set label " "
    }
    set menustatus $label
    update idletasks
}


set selected "None"
set sgmdir [pwd]

