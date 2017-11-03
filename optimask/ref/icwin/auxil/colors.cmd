! Avoid name conflicts by erasing names of existing colors.

COLOR  8 NO_NAME; COLOR  9 NO_NAME; COLOR 10 NO_NAME; COLOR 11 NO_NAME; 
COLOR 12 NO_NAME; COLOR 13 NO_NAME; COLOR 14 NO_NAME; COLOR 15 NO_NAME; 

! Define new color and palette values.

COLOR  0 NAME=BLACK       PALETTE=( 0, 0, 0) 
COLOR  1 NAME=WHITE       PALETTE=(63,63,63) LEVEL=16 
COLOR  2 NAME=YELLOW      PALETTE=(63,63, 0) LEVEL= 6
COLOR  3 NAME=GREEN       PALETTE=(21,63, 0) LEVEL= 6
COLOR  4 NAME=RED         PALETTE=(63, 0,21) LEVEL= 8
COLOR  5 NAME=CYAN        PALETTE=( 0,42,42) LEVEL=10
COLOR  6 NAME=BLUE        PALETTE=( 0, 0,63) LEVEL=10
COLOR  7 NAME=MAGENTA     PALETTE=(63, 0,63) LEVEL= 8
COLOR  8 NAME=GRAY        PALETTE=(42,42,42) LEVEL=14
COLOR  9 NAME=BROWN       PALETTE=(32,16, 0) LEVEL= 8
COLOR 10 NAME=ORANGE      PALETTE=(63,31, 0) LEVEL= 8
COLOR 11 NAME=PURPLE      PALETTE=(21, 0,14) LEVEL= 3 
COLOR 12 NAME=DIM_RED     PALETTE=(22, 0, 0) LEVEL= 3 
COLOR 13 NAME=DIM_BLUE    PALETTE=( 0, 0,22) LEVEL= 3 
COLOR 14 NAME=DIM_GREEN   PALETTE=( 0,22, 0) LEVEL= 3 
COLOR 15 NAME=HI          PALETTE=(63,63,63) LEVEL=15

! Define one_dot and four_dot "palettes" for plotting

COLOR BLACK     ONE_DOT=WHITE    FOUR_DOTS=(WHITE,   WHITE,   WHITE,   WHITE)
COLOR WHITE     ONE_DOT=BLACK    FOUR_DOTS=(BLACK,   BLACK,   BLACK,   BLACK)
COLOR RED       ONE_DOT=RED      FOUR_DOTS=(RED,     RED,     RED,     RED)
COLOR YELLOW    ONE_DOT=YELLOW   FOUR_DOTS=(YELLOW,  YELLOW,  YELLOW,  YELLOW)
COLOR CYAN      ONE_DOT=CYAN     FOUR_DOTS=(CYAN,    CYAN,    CYAN,    CYAN)
COLOR BLUE      ONE_DOT=BLUE     FOUR_DOTS=(BLUE,    BLUE,    BLUE,    BLUE)
COLOR GREEN     ONE_DOT=GREEN    FOUR_DOTS=(GREEN,   GREEN,   GREEN,   GREEN)
COLOR MAGENTA   ONE_DOT=MAGENTA  FOUR_DOTS=(MAGENTA, MAGENTA, MAGENTA, MAGENTA)
COLOR GRAY      ONE_DOT=BLACK    FOUR_DOTS=(BLACK,   WHITE,   WHITE,   WHITE)
COLOR BROWN     ONE_DOT=RED      FOUR_DOTS=(GREEN,   RED,     RED,     YELLOW)
COLOR ORANGE    ONE_DOT=RED      FOUR_DOTS=(RED,     YELLOW,  YELLOW,  RED)
COLOR PURPLE    ONE_DOT=MAGENTA  FOUR_DOTS=(BLUE,    MAGENTA, MAGENTA, BLUE)
COLOR DIM_RED   ONE_DOT=RED      FOUR_DOTS=(RED,     WHITE,   WHITE,   WHITE)
COLOR DIM_BLUE  ONE_DOT=BLUE     FOUR_DOTS=(BLUE,    WHITE,   WHITE,   WHITE)
COLOR DIM_GREEN ONE_DOT=GREEN    FOUR_DOTS=(GREEN,   WHITE,   WHITE,   WHITE)
COLOR HI        ONE_DOT=BLACK    FOUR_DOTS=(BLACK,   BLACK,   BLACK,   BLACK)

! Suggested uses in CMOS processes. 
!
! Use high-level (foreground) colors WHITE and GRAY for contact layers.
!
! Use low-level low-intensity (background) colors PURPLE, DIM_RED, DIM_BLUE,
! and DIM_GREEN for wells and selects.
!
! Use low- to medium-level high intensity colors for diffusion layers.
!
! Use medium- to high-level high intensity colors for interconnect layers.
!
! Reserve color 15 for node outliner.

$$ COLORS.CMD successfully completed
