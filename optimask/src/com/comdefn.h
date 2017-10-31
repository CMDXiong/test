/* ========================================================================== */
/* COMDEFN.H    COMmon DEFiNition structures/classes/functions header file
 *
 * REFERENCE: icedpoly.c v11.00, qdrim.c v16.00, q1rim.c v15.00, qdcvx.c v9.00;
 *  [1]. Joseph O'Rourke, Computational Geometry in C (2nd Ed.), Cambridge Univ.
 *       Press, 9/28/1998, http://maven.smith.edu/~orourke/books/compgeom.html ;
 *  [2]. Daniel Sunday, softSurfer (www.softsurfer.com);
 *  [3]. http://www.mochima.com/articles/cuj_geometry_article/geometry_demo.zip
 *  [4]. ICED; AutoCAD; CleWin; LEdit; Artwork_Rule_MASK_AutoCAD_DXF_GDSII.pdf;
 *
 * COPYRIGHT 2011 Henghua Deng.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.60$ (20120509 Henghua Deng) */
/* HISTORY: 20110927(v1.00)from Reference codes; 20110928(v1.10)typedef struct;
 * 20110930(v1.20)#ifndef; 20120502(v1.60)operations for list containers;
/* ========================================================================== */

#ifndef uint8
  typedef unsigned char uint8; /* unsigned 8-bit integer [0~255] (2^8-1=255) */
#endif

/* -------------------------------------------------------------------------- */
/* http://en.wikipedia.org/wiki/Cartesian_coordinate_system */
/* http://en.wikipedia.org/wiki/Ordered_pair */
#define DIMN 2 /* two-dimensional (2D) */
typedef double cord[DIMN]; /* Cartesian Coordinate Ordered Pair: [0]X; [1]Y; */

/* MBB: http://en.wikipedia.org/wiki/Minimum_bounding_box */
/* MBR: http://en.wikipedia.org/wiki/Minimum_bounding_rectangle */
typedef double bbox[2*DIMN]; /* bounding box: [0/1]-X0/Y0(LL); [2/3]-X1/Y1(UR); */
/* Bounding Box in Adobe EPS format: %%BoundingBox: llx lly urx ury */
/* objects cannot intersect if their minimum bounding boxes do not intersect */
typedef double blim[2*DIMN]; /* min(X), max(X), min(Y), max(Y); */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* new memory-allocation O'Rourke1998 code 1.4 \Ccode2\dt\macros.h */
#define NEWP(p,type) if ( (p=(type *)malloc(sizeof(type))) == NULL ) {\
      printf("NEWP: OUT OF MEMORY!\n");\
      exit(EXIT_FAILURE);\
    }
/* add p(source) before head(target); doubly-linked CIRCULAR list; NO COUNTERS; */
#define ADDP(head,p) if (head) {\ /* insert cell p prior to head */
      p->next = head;\
      p->prev = head->prev;\
      head->prev->next = p;\ /* item before p; head->prev->prev unaffected */
      head->prev = p;\ /* item after p; head->next unaffected */
    } else {\ /* added cell is the only one in the list */
      head = p;\ /* when for linear list: prev==NULL start; next==NULL end; */
      head->next = head->prev = p;\
    }
/* add p before head; UPDATE COUNTER N++; */
#define ADDN(N,head,p) { if (head) N++; else N=1; ADDP(head,p); }
/* delete p from list at head; NO COUNTERS; */
#define DELP(head,p) if (head) {\
      if ( head == head->next ) head = NULL;\
      else if ( p == head ) head = head->next;\
      p->next->prev = p->prev;\
      p->prev->next = p->next;\
    } /* keep p in memory; NEED TO BE CAREFUL OF floating (garbage) memory; */
/* delete p from list at head; UPDATE COUNTER N--; */
#define DELN(N,head,p) {DELP(head,p); if (N) N--;}
/* RELease(delete) p from list at head; RELease p memory; NO COUNTERS; */
#define RELP(head,p) { DELP(head,p); FREE(p); }
/* RELease(delete) p from list at head; RELease p memory; UPDATE COUNTER N--; */
#define RELN(N,head,p) { DELN(N,head,p); FREE(p); }
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* basic layer preoperties for photomask GDSII files */
typedef struct { /* RGB color: http://en.wikipedia.org/wiki/RGB_color_model */
    uint8 R;       /* red [0-255] */
    uint8 G;       /* green [0-255] */
    uint8 B;       /* blue [0-255] */
} RGB ; /* additive RGB triplet (r,g,b); USAGE: RGB teal={24,167,181}; */

typedef struct { /* CMYK color:  http://en.wikipedia.org/wiki/CMYK_color_model */
    uint8 C;       /* cyan [0-255]/255 (0-100%) */
    uint8 M;       /* magenta [0-255]/255 (0-100%) */
    uint8 Y;       /* yellow [0-255]/255 (0-100%) */
    uint8 K;       /* key (black) [0-255]/255 (0-100%) */
} CMYK ; /* subtractive CMYK palette; USAGE: CMYK teal={1.0,0.0,0.50,0.20}; */

typedef struct { /* layer structure (refer to KLayout,L-Edit,AutoCAD,CleWin) */
    uint ID;       /* indexing */
    uint8 GDSNUM;  /* GDSII layer number 1-255; (0: current or unassigned;) */
    char name[255];/* char *name; customized layer name; <255 letters; */
    char info[255];/* additional information (such as description) */
    RGB  color;    /* layer color */
    RGB  cbdry;    /* polygon boundary color */
    uint8 patt;    /* area fill pattern 0-255 */
    uint8 anim;    /* animation (0:none; 1:scroll; 2: blink; 3:/blink; etc.) */
    uint8 line;    /* boundary line thickness (pt)*/
    uint8 opaq;    /* transparency/opaqueness [0-255]/255; */
    bool FILL=1; /* 0:only show boundary line; 1:show the area fill pattern; */
    bool HIDE=0; /* 0:show(on); 1:hide(off)(blank), CANNOT see/select/edit; */
    bool PROT=0; /* 0:unprot; 1:protect(freeze), can see, CANNOT select/edit; */
    bool LOCK=0; /* 0:unlock; 1:locked, can see, can select, CANNOT edit; */
} LAYER ;
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* basic geometric shapes (GDSII ENTITIES) for photomask files */
/* ID -- be UNIQUE for each object/item; (REF.: IC Editors (ICED)) */
/* Call in GROUP/CELL: type of each item: B-BOX; P-POLY(GON); W-WIRE; L-LINE; */
typedef struct { /* box */
    uint8 layer;   /* LAYER.ID */
    uint ID;       /* indexing */
    /* cord P0;       /* lower left (LL) corner */
    /* cord P1;       /* upper right (UR) corner */
    bbox MBB;      /* minimum bounding box: MBB[0,1,2,3]=(XLL,YLL,XUR,YUR); */
    bool HIDE=0; /* 0:show(on); 1:hide(off)(blank), CANNOT see/select/edit; */
    bool PROT=0; /* 0:unprot; 1:protect(freeze), can see, CANNOT select/edit; */
    bool LOCK=0; /* 0:unlock; 1:locked, can see, can select, CANNOT edit; */
} BOX ;

typedef struct { /* polygon */
    uint8 layer;   /* LAYER.ID */
    uint ID;       /* indexing */
    uint N;        /* number of vertices */
    double *X;     /* X-coordinates [0] to [N-1] */
    double *Y;     /* Y-coordinates [0] to [N-1] */
    bbox MBB;      /* minimum bounding box: MBB[0,1,2,3]=(XLL,YLL,XUR,YUR); */
    bool HIDE=0; /* 0:show(on); 1:hide(off)(blank), CANNOT see/select/edit; */
    bool PROT=0; /* 0:unprot; 1:protect(freeze), can see, CANNOT select/edit; */
    bool LOCK=0; /* 0:unlock; 1:locked, can see, can select, CANNOT edit; */
} POLY ;

typedef struct { /* GDSII path (LEdit wire, AutoCAD polyline with width) */
    uint8 layer;   /* LAYER.ID */
    uint ID;       /* indexing */
    uint N;        /* number of vertices */
    double *X;     /* X-coordinates [0] to [N-1] */
    double *Y;     /* Y-coordinates [0] to [N-1] */
    double W;      /* width of the path (wire) */
    bbox MBB;      /* minimum bounding box: MBB[0,1,2,3]=(XLL,YLL,XUR,YUR); */
    bool HIDE=0; /* 0:show(on); 1:hide(off)(blank), CANNOT see/select/edit; */
    bool PROT=0; /* 0:unprot; 1:protect(freeze), can see, CANNOT select/edit; */
    bool LOCK=0; /* 0:unlock; 1:locked, can see, can select, CANNOT edit; */
} WIRE ;

typedef struct { /* line (width=0, illustration only; ignored on final mask) */
    uint8 layer;   /* LAYER.ID */
    uint ID;       /* indexing */
    uint N;        /* number of vertices */
    double *X;     /* X-coordinates [0] to [N-1] */
    double *Y;     /* Y-coordinates [0] to [N-1] */
    bbox MBB;      /* minimum bounding box: MBB[0,1,2,3]=(XLL,YLL,XUR,YUR); */
    bool HIDE=0; /* 0:show(on); 1:hide(off)(blank), CANNOT see/select/edit; */
    bool PROT=0; /* 0:unprot; 1:protect(freeze), can see, CANNOT select/edit; */
    bool LOCK=0; /* 0:unlock; 1:locked, can see, can select, CANNOT edit; */
} LINE ;
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* groups, cells, and instances (GDSII BLOCKS/REFERENCES) for photomask files */
typedef struct { /* group of boxes/polygons/paths(wires) */
    uint ID;       /* group indexing */
    uint N;        /* number of items */
    uint *ITEMS;   /* list of [0] to [N-1] items (using the indexing values) */
    char *TYPE;    /* item type : C-CELL; B-BOX; P-POLY(GON); W-WIRE; L-LINE; */
    bbox MBB;      /* minimum bounding box: MBB[0,1,2,3]=(XLL,YLL,XUR,YUR); */
    bool HIDE=0; /* 0:show(on); 1:hide(off)(blank), CANNOT see/select/edit; */
    bool PROT=0; /* 0:unprot; 1:protect(freeze), can see, CANNOT select/edit; */
    bool LOCK=0; /* 0:unlock; 1:locked, can see, can select, CANNOT edit; */
} GROUP ;
/* Artwork_Rule_MASK_AutoCAD_DXF_GDSII.pdf; CELL: GDSII BLOCK; Instance(LEdit):
 * reference(GDSII); array reference: AutoCAD MINSERT; GDSII AREF; */
typedef struct { /* MASK CELL: group of groups and items (polygon/box/wire) */
    uint ID;       /* CELL indexing */
    char name[255];/* char *name; cell name; <255 letters; GDSII <32;*/
    char ver[255]; /* version ID */
    char timegen[255]; /* date and time generated */
    char timemod[255]; /* date and time modified */
    char author[255]; /* author info */
    char copyright[255]; /* copyright info (year and organization etc.) */
    char info[255]; /* additional information (such as description) */
    uint8 LEVEL;   /* hiearchy level (limit to 1-255); */
    uint8 NLAY;    /* number of layers used 1-255 */
    uint8 *LAYS;   /* list of layers used */
    uint N;        /* number of items included */
    uint *ITEMS;   /* list of [0] to [N-1] items (using the indexing values) */
    char *TYPE;    /* item type : C-CELL; B-BOX; P-POLY(GON); W-WIRE; L-LINE; */
    double W;      /* cell width W (dX) */
    double H;      /* cell height H (dY) */
    bbox MBB;      /* minimum bounding box: MBB[0,1,2,3]=(XLL,YLL,XUR,YUR); */
    bool HIDE=0; /* 0:show(on); 1:hide(off)(blank), CANNOT see/select/edit; */
    bool PROT=0; /* 0:unprot; 1:protect(freeze), can see, CANNOT select/edit; */
    bool LOCK=0; /* 0:unlock; 1:locked, can see, can select, CANNOT edit; */
} CELL ;
/* -------------------------------------------------------------------------- */
typedef struct { /* the action of instance (calling) a cell */
    uint ID;       /* instance indexing */
    uint idxcell;  /* the indexing of CELL (idxn) to be instanced */
    char name[255];/* instance name (other than cell name); <255 letters; */
    cord ORG;      /* origin (translation/offset/shift) (default {0,0}); */
    cord AMP;      /* scale/amplification {ampx,ampy} */
    uint8 MIRROR;  /* mirror (0:none, 1:X,2:Y,3:-X,4:-Y, 5:Y=X,6:Y=-X, 7:ORG; */
    double ROTDEG; /* rotation angle (in degree (recommend 0-360deg)) */
    uint REPN[DIMN]={1,1}; /* repeat of the instance at {X,Y} direction */
    cord REPdXY={0,0}; /* {dX,dY} for the repeat of instance (GDSII AREF) */
} INSTCELL ;
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* basic (2D) geometric objects for generating/operating polygons */
/* http://en.wikipedia.org/wiki/Cartesian_plane */
typedef struct { /* infinite line or line segment (zero thickness) */
    uint ID;       /* indexing */
    cord P0;       /* start point */
    cord P1;       /* end point */
} SEG ;

typedef struct { /* triangle */
    uint ID;       /* indexing */
    cord P0;       /* 1st point */
    cord P1;       /* 2nd point */
    cord P2;       /* 3rd point */
} TRI ;

typedef struct { /* Parallelogram (including RECTANGLE&SQUARE where ang=PI_2) */
    uint ID;       /* indexing */
    cord ORG;      /* origin */
    double L[2];   /* lengths of two edges */
    double ang;    /* angle between two edges (the other angle = PI-ang) */
    double ROTDEG; /* rotation angle in degree (0-360deg) (from horizontal base) */
} PRL4 ;

typedef struct { /* N-side equilateral polygons */
    uint ID;       /* indexing */
    cord ORG;      /* origin */
    double R;      /* radius */
    uint8  N;      /* number of sides */
    double ROTDEG; /* rotation angle in degree (0-360deg) (from horizontal base) */
} NGON ; /* Equilateral-Triangle3,Square4,Pentagon5,Hexagon6,Heptagon7,Octagon8 */

typedef struct { /* circle from 0 to 360deg; DISK: closed circle; */
    uint ID;       /* indexing */
    cord ORG;      /* origin */
    double R;      /* radius */
} CIRC ;

typedef struct { /* annulus; http://en.wikipedia.org/wiki/Annulus_(mathematics) */
    uint ID;       /* indexing */
    cord ORG;      /* origin */
    double R[2];   /* radius R[0] inner; R[1] outer */
} RING ;

typedef struct { /* 3D; torus (pl. tori); http://en.wikipedia.org/wiki/Torus */
    uint ID;       /* indexing */
    double ORG[3]; /* origin (X,Y,Z) */
    double R[2];   /* radius R[0] inner; R[1] outer */
} TORUS ;

typedef struct { /* pie shape from ang0 to ang1 deg */
    uint ID;       /* indexing */
    cord ORG;      /* origin */
    double R;      /* radius */
    double ang[2]; /* ang[0] start angle; ang[1] stop angle; */
} PIEDISK ;

typedef struct { /* pie shaped annulus (ring) from ang0 to ang1 deg */
    uint ID;       /* indexing */
    cord ORG;      /* origin */
    double R[2];   /* radius R[0] inner; R[1] outer */
    double ang[2]; /* ang[0] start angle; ang[1] stop angle; */
} PIERING ;

typedef struct { /* ellipse from 0 to 360deg; ELIPDISK: closed ellipse; */
    uint ID;       /* indexing */
    cord ORG;      /* origin */
    double ang;    /* orientation (angle of ellipse (major) axis, angaxis) */
    double a;      /* major axis */
    double b;      /* minor axis */
} ELIP ;

typedef struct { /* TEXT */
    uint ID;       /* indexing */
    cord pos;      /* position */
    double ang;    /* orientation angle */
    /* font type: c-alibri, d-deplof, g-ms_gothic, l-lucida_sans,
     *   r-times_new_roman, t-tahoma; */
    char type;     /* font type: c,d,g,l,r,t, etc. */
    double size;   /* font size */
    double gap;    /* font spacing */
} TEXT ;

typedef struct { /* measurement ruler (ignored on final mask) */
    uint ID;       /* indexing */
    cord P0;       /* 1st point */
    cord P1;       /* 2nd point */
    double major;  /* major scale */
    double minor;  /* minor scale */
} RULER ;


/* http://en.wikipedia.org/wiki/Wafer_(electronics) */
/* Detailed dimensions in Virginia_Semi_Wafer_Specifications_200206.pdf */
/* D:Diameter; T:Thickness; Primary Flat Location: crystallographic plane{110};
 * F: Primary(Major)Flat Length; F2: Secondary(Minor)Flat Length; 0: N/A;
 * if Flat Length N/A, approximate pie angle ~36deg Major, ~18deg Minor; */
enum WFR01 {D=25.4, T=0, F=0, F2=0}; /* 1inch:25.4mm; */
enum WFR02 {D=50.8, T=279, F=15.88, F2=8}; /* 2inch:50.8mm; */
enum WFR03 {D=76.2, T=381, F=22.22, F2=11.18}; /* 3inch:76.2mm; */
enum WFR04 {D=100, T=525, F=32.5, F2=18.0}; /* 4inch:100mm; */
enum WFR05 {D=125, T=625, F=42.5, F2=27.5}; /* 5inch:130mm; 4.9inch:125mm; */
enum WFR06 {D=150, T=675, F=57.5, F2=37.5}; /* 5.9inch:150mm; */
enum WFR08 {D=200, T=725, F=0, F2=0}; /* 7.9inch:200mm; */
enum WFR12 {D=300, T=775, F=0, F2=0}; /* 11.8inch:300mm; */
enum WFR18 {D=450, T=925, F=0, F2=0}; /* 18inch:450mm; (expected) */
typedef struct { /* wafer shape (circular with flat bottom) */
    uint ID;       /* indexing */
    cord ORG;      /* origin */
    double D;      /* wafer size (diameter in inches) */
    double Rg[2];  /* guard ring radius R[0] inner; R[1] outer */
} WAFER ;
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* basic geometrical relationships (such as in Polygon Triangulation) */
/* vertex, edge and face ($4.3.1/Code4.1 of [1]O'Rourke) */
typedef struct { /* vertice */
    uint ID;       /* indexing */
    cord XY;       /* coordinate */
    bool ear;      /* TRUE iff an ear */
} VRTX ;

typedef struct { /* edge */
    uint ID;      /* indexing */
    cord P0;      /* start point */
    cord P1;      /* end point */
    uint adjl;    /* adjacent face (left) */
    uint adjr;    /* adjacent face (right) */
    uint prev;    /* previous edge */
    uint next;    /* next edge */
} EDGE ;

struct FACE { /* face (useful in 3D) */
    uint ID;      /* indexing */
};
/* -------------------------------------------------------------------------- */
