! Don Rogge has contributed capacitor and resistor generator .CMD files.  You
! can download his .CMD files from the "User Contributions" page on our web
! site.  
!
!----------------------------------------------------------------------------
! Set technology dependent parameters
!----------------------------------------------------------------------------
!
LOCAL ohms_per_square   = 40;
LOCAL unstretched_len   = 6;
LOCAL unstretched_width = 4;
LOCAL res$cell          = RES;
LOCAL scratch           = RES$TMP$;
!
!----------------------------------------------------------------------------
! Prompt for parameters for this resistor
!----------------------------------------------------------------------------
!
LOCAL ohms      $PROMPT="Enter target resistance:"
LOCAL width     $PROMPT="Enter channel width:"
LOCAL rot_code=-1;
while(CMP(%rot_code, 0)!=0 && CMP(%rot_code, 1)!=0){
   #rot_code  $PROMPT="Enter rotation code (0=>Horizontal 1=>Vertical):"
}
!
!----------------------------------------------------------------------------
! Compute values
!----------------------------------------------------------------------------
!
LOCAL stretch.y = ROUND(%width - %unstretched_width);
LOCAL stretch.x = ROUND(%ohms * %width / %ohms_per_square &
                           - %unstretched_len);
!
!----------------------------------------------------------------------------
! Build resistor cell
!---------------------------------------------------------------------------- 
!
EDIT CELL %scratch
XSELECT OFF;
SELECT ALL; DELETE;
ADD CELL %res$cell AT (0, 0)
SELECT CELL * ALL; UNGROUP;
SELECT SIDE IN (0, -10000) (10000, 10000)
MOVE SIDE X %stretch.x
UNSELECT ALL
SELECT SIDE IN (-10000, 0) (10000, 10000)
MOVE SIDE Y %stretch.y
UNSELECT ALL
EXIT
!
!----------------------------------------------------------------------------
! Add and ungroup resistor cell
!---------------------------------------------------------------------------- 
!
UNSELECT PUSH
ADD CELL %scratch R%rot_code
SELECT NEW
UNGROUP
UNSELECT POP
