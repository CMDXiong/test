VIEW OFF
$ MENU "M1";  KEEP_LIBRARY_CELLS=ASK;  DISPLAY CELL_DEPTH=100;
PATTERN "SAMPLE";  FILL MIXED ON
AUTOPAN ON  PIXELS=100  SECONDS=0.5;   ARROW MODE=EDIT
DISPLAY  CELL_LABELS=ON  OUTLINE_DEPTH=1  EDIT_STACK=OFF  CURSOR 1 SNAP=ON
SPACER OFF  SPACE=0.0  TRACK_LAYERS=OFF  STYLE=1
VIEW LIMIT ON   SCALE=0.500 DEPTH=1 DOTS=0 UNITS=0.0 SHOW_LAYERS 1:*
ARRAY  DRAW_MODE=SIDES  CELL_LIMIT=1024
TEXT  LOWER_CASE=DISABLED  MULTI_LINE_TEXT=DISABLED  ORIENTATIONS=2  DISPLAY_ORIGINS=OFF
USE TEXT_JUSTIFICATION=LB  WIRE_TYPE=2  ARC_TYPE=2  N_SIDES=16
RESOLUTION  STEP=0.500  MODE=SOFT
SNAP  ANGLE=45  STEP=(0.500,0.500)  OFFSET=(0.000,0.000)
NEAR  UNITS=0.05  DOTS=4

COLOR  0 NAME=BLACK       PALETTE=( 0, 0, 0) 
COLOR  1 NAME=WHITE       PALETTE=(63,63,63) LEVEL=16
COLOR  2 NAME=YELLOW      PALETTE=(63,63, 0) LEVEL= 6
COLOR  3 NAME=GREEN       PALETTE=(21,63, 0) LEVEL= 6
COLOR  4 NAME=RED         PALETTE=(63, 0,21) LEVEL= 8
COLOR  5 NAME=CYAN        PALETTE=( 0,42,42) LEVEL= 9
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

COLOR BLACK      ONE_DOT=WHITE    FOUR_DOTS=(WHITE,  WHITE,  WHITE,  WHITE)
COLOR WHITE      ONE_DOT=BLACK    FOUR_DOTS=(BLACK,  BLACK,  BLACK,  BLACK)
COLOR YELLOW     ONE_DOT=YELLOW   FOUR_DOTS=(YELLOW, YELLOW, YELLOW, YELLOW)
COLOR GREEN      ONE_DOT=GREEN    FOUR_DOTS=(GREEN,  GREEN,  GREEN,  GREEN)
COLOR RED        ONE_DOT=RED      FOUR_DOTS=(RED,    RED,    RED,    RED)
COLOR CYAN       ONE_DOT=CYAN     FOUR_DOTS=(CYAN,   CYAN,   CYAN,   CYAN)
COLOR BLUE       ONE_DOT=BLUE     FOUR_DOTS=(BLUE,   BLUE,   BLUE,   BLUE)
COLOR MAGENTA    ONE_DOT=MAGENTA  FOUR_DOTS=(MAGENTA,MAGENTA,MAGENTA,MAGENTA)
COLOR GRAY       ONE_DOT=BLACK    FOUR_DOTS=(BLACK,  WHITE,  WHITE,  WHITE)
COLOR BROWN      ONE_DOT=RED      FOUR_DOTS=(GREEN,  RED,    RED,    YELLOW)
COLOR ORANGE     ONE_DOT=RED      FOUR_DOTS=(RED,    YELLOW, YELLOW, RED)
COLOR PURPLE     ONE_DOT=MAGENTA  FOUR_DOTS=(BLUE,   MAGENTA,MAGENTA,BLUE)
COLOR DIM_RED    ONE_DOT=RED      FOUR_DOTS=(RED,    WHITE,  WHITE,  WHITE)
COLOR DIM_BLUE   ONE_DOT=BLUE     FOUR_DOTS=(BLUE,   WHITE,  WHITE,  WHITE)
COLOR DIM_GREEN  ONE_DOT=GREEN    FOUR_DOTS=(GREEN,  WHITE,  WHITE,  WHITE)
COLOR HI         ONE_DOT=BLACK    FOUR_DOTS=(BLACK,  BLACK,  BLACK,  BLACK)

GRID 1  ON   COLOR=RED     DOTS     STEP=1.0
GRID 2  ON   COLOR=CYAN    CROSSES  STEP=5 
GRID 3  OFF  COLOR=WHITE   LINES    STEP=50000

LAYER *  WIDTH=2.0  SPACE=0.0  YELLOW  PAT=0   NO_PEN
INITIALIZE LAYERS 0:255
LAYER 0   PEN=0
LAYER 1   NAME=NWEL  WIDTH=3.000  SPACE=0.000  BLUE    PAT=0   PEN=16  NO_CIF    NO_STREAM
LAYER 2   NAME=NDIF  WIDTH=3.000  SPACE=0.000  GREEN   PAT=1   PEN=*   NO_CIF    NO_STREAM
LAYER 3   NAME=PDIF  WIDTH=3.000  SPACE=0.000  YELLOW  PAT=1   PEN=*   NO_CIF    NO_STREAM
LAYER 4   NAME=PSEL  WIDTH=3.000  SPACE=0.000  YELLOW  PAT=0   PEN=*   NO_CIF    NO_STREAM
LAYER 5   NAME=POLY  WIDTH=2.000  SPACE=0.000  RED     PAT=1   PEN=*   NO_CIF    NO_STREAM
LAYER 6   NAME=M1    WIDTH=3.000  SPACE=0.000  CYAN    PAT=2   PEN=*   NO_CIF    NO_STREAM
LAYER 7   NAME=M2    WIDTH=4.000  SPACE=0.000  BLUE    PAT=3   PEN=*   NO_CIF    NO_STREAM
LAYER 8   NAME=CONT  WIDTH=2.000  SPACE=0.000  WHITE   PAT=1   PEN=*   NO_CIF    NO_STREAM
LAYER 9   NAME=VIA   WIDTH=3.000  SPACE=0.000  GRAY    PAT=1   PEN=*   NO_CIF    NO_STREAM
GLOBAL KEY.CF9="@UNED"
GLOBAL KEY.F1="RULER"
GLOBAL KEY.F7="DOS"
GLOBAL KEY.F8="@DEEPSHOW"
GLOBAL KEY.F9="DOS"

$ %exec.file successfully completed