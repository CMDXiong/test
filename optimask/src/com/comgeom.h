/* ========================================================================== */
/* COMGEOM.H    COMmon GEOMetrical functions header file
 *
 * CONVENTION: (X[N],Y[N]),(*X,*Y,N) for arbitrary points & arbitrary concavity,
 *   (PX[PN],PY[PN]),(*PX,*PY,PN) for polyline-ordered points & convex polygons;
 *   indexing([0],[1],[2]) for arrays, subscripts(1,2,3) for vertices;
 *   (_x_,_y_,_z_) as (0,1,2) for vector indexing (O'Rourke1998 $1.4.1/Code1.1);
 *   Capital(X,Y,Z): absolute coordinates; lower(x,y,z): relative coordinates;
 *   MANY functions 3 forms: discrete (vertex) form, array form and matrix form;
 *
 * REFERENCE:
 *   [1]. Joseph O'Rourke, Computational Geometry in C (2nd Ed.), Cambridge Univ.
 *     Press, 9/28/1998, http://maven.smith.edu/~orourke/books/compgeom.html;
 *   [2]. http://en.wikipedia.org/wiki ; http://mathworld.wolfram.com/ ;
 *   [3]. http://cgafaq.info/ ; Paul Bourke http://paulbourke.net/geometry/ ;
 *        Daniel Sunday softsurfer.com; Darel Rex Finley alienryderflex.com;
 *   [4]. icedpoly.c v11.00, qdrim.c v16.00, q1rim.c v15.00, qdcvx.c v9.00;
 *
 * COPYRIGHT 2012 Henghua Deng.
 * -------------------------------------------------------------------------- */
/* $REVISION: 8.60$ (20120918 Henghua Deng) */
/* HISTORY: 20110504(v1.00)from qdrim.c v10.60, q1rim.c v9.60;
 * 20110512(v1.12)curve orientation, polygon convex, and polygon area;
 * 20110602(v2.22)20110630(v3.00)optimization; 20110712(v4.00)memory management;
 * 20110921(v5.00)hypot(); 20110926(v5.99)point/line/polygon algorithms;
 * 20110930(v6.00)#ifndef; 20111102(v7.00)isPnPoly(); 20120227(v8.00)ellipse;
/* ========================================================================== */

/* ########################################################################## */
/* ################ geometric type and structure definitions ################ */
/* ########################################################################## */

/* Cartesian Coordinate Point and Vector Types */
typedef double tXY[2]; /* 2D Cartesian Coordinate Type: [0]X; [1]Y; */
typedef double tXYZ[3]; /* 3D Cartesian Coordinate Type: [0]X; [1]Y; [2]Z; */
/* Cartesian Coordinate Line/Segment Type: [0][] start vertex, [1][] end vertex; */
typedef double tLineXY[2][2];  /* 2D Line/Segment Type: [][0]X; [][1]Y; */
typedef double tLineXYZ[2][3]; /* 3D Line/Segment Type: [][0]X; [][1]Y; [][2]Z; */
/* Cartesian Coordinate Triangle Type: [0][] Va; [1][] Vb; [2][] Vc; */
typedef double tTriXY[3][2];  /* 2D Triangle Type: [][0]X; [][1]Y; */
typedef double tTriXYZ[3][3]; /* 3D Triangle Type: [][0]X; [][1]Y; [][2]Z; */
/* Cartesian Coordinate Box Type: [0][] lower-left(LL), [1][] upper-right(UR); */
typedef double tBoxXY[2][2];  /* 2D Triangle Type: [][0]X; [][1]Y; */
typedef double tBoxXYZ[2][3]; /* 3D Triangle Type: [][0]X; [][1]Y; [][2]Z; */
/* Cartesian Coordinate Quadrilateral: [0][] Va; [1][] Vb; [2][] Vc; [3][] Vd; */
typedef double tQuadXY[4][2];  /* 2D Quadrilateral Type: [][0]X; [][1]Y; */
typedef double tQuadXYZ[4][3]; /* 3D Quadrilateral Type: [][0]X; [][1]Y; [][2]Z; */

/* GDSII-compatible polygon (CLOSED or OPEN); GDSII boundary is CLOSED; */
typedef tXY tPOLYGDS[NGDSBDRY]; /* typedef double tPOLYGDS[NGDSBDRY][2]; */
/* usage: tPOLYGDS P; (X,Y) coordinates of i-th vertex: (P[i][_x_], P[i][_y_]) */

/* alias type names */
#define t2XY tBoxXY
#define t3XY tTriXY
#define t4XY tQuadXY
#define t2XYZ tBoxXYZ
#define t3XYZ tTriXYZ
#define t4XYZ tQuadXYZ

/* -------------------------------------------------------------------------- */
/* struct (defaults to public access) - for POD (plain old data) */
/* class (defaults to private access) - if need encapsulation & member functions */
/* http://en.wikipedia.org/wiki/Struct_(C_programming_language) */
/* Usgae 1:
 *   struct PNT {
 *     double X;
 *     double Y;
 *   };
 * typedef struct PNT PNT; /* or PNT_t for data type by convention */
/* Usgae 2:
 *   typedef struct PNT {
 *     double X;
 *     double Y;
 *   } PNT ; */
/* Usage 3: */
typedef struct { /* point 2D */
    double X;
    double Y;
} sPNT2D ;
typedef struct { /* vector 2D */
    double Vx;
    double Vy;
} sVECT2D ;
typedef struct { /* polygon 2D */
    unsigned int N; /* number of vertices */
    double *X;
    double *Y;
} sPLGN2D ;

/* for Delaunay Triangulation (Finite Elment Analysis) */
typedef struct { /* node 2D */
    unsigned int idx; /* global indexing (key) for the current node */
    double X;
    double Y; /* double XY[2]; global coordinate of local vertices */
} sNODE2D ;
typedef struct { /* edge 2D */
    unsigned int idx; /* global indexing (key) for this current edge */
    unsigned int node[2]; /* global indexing of start and end node */
} sEDGE2D ;
typedef struct { /* triangle of 2D Delaunay Triangulation */
/* polylib\ref\PolygonGeom\Polygon_Triangulation\poly2tri - mesh.png */
    unsigned int idx; /* global indexing (key) for the current triangle */
    /* tTriXY XY; /* global coordinate of local vertices */
    unsigned int node[3]; /* global indexing of local nodes */
    unsigned int edge[3]; /* global indexing of local edges */
    unsigned int next[3]; /* global indexing of neighbor triangles */
} sDTRI2D ;

/* -------------------------------------------------------------------------- */

/* area and perimeter of a Circle with radius R */
#define ElipArea(a,b) ( PI*(a)*(b) ) /* ellipse area; perimeter ElipPeri(); */
#define CircArea(R) ( PI*(R)*(R) )
#define CircPeri(R) ( PI2*(R) )
/* area and perimeter of a Box X1Y1-X2Y2 */
#define BoxArea(X1,Y1,X2,Y2) ( fabs(((X2)-(X1))*((Y2)-(Y1))) )
/* #define BoxPeri(X1,Y1,X2,Y2) ( 2*(fabs(((X2)-(X1))+fabs(((Y2)-(Y1))) ) */
double BoxPeri(double X1, double Y1, double X2, double Y2)
{ /* calling syntax: BoxPeri(X1,Y1,X2,Y2); */
    double dX=fabs(X2-X1), dY=fabs(Y2-Y1);
    return(dX+dX+dY+dY);
}

/* ########################################################################## */
/* # functions for triangle/quadrilateral determinant/area/orientation etc. # */
/* ########################################################################## */
/* signed area (determinant) of a triangle (X1,Y1)-(X2,Y2)-(X3,Y3) */
/* sign tells Triangle Orientation & Is a point left/right to an infinite line */
/* TriMat=[1,1,1;x1,x2,x3;y1,y2,y3]; Linear Algebra det(TriMat)=det(JacobMat);
 * JacobMat=[x2-x1, y2-y1; x3-x1, y3-y1]; det(JacobMat)=2*Ae for triangles;
 * CROSS PRODUCT Z = (x2-x1,y2-y1) X (x3-x1,y3-y1) on z-axis; NOTE |Z|=2*Ae; */
/* http://mathworld.wolfram.com/TriangleArea.html & PolygonArea.html*/
/* http://en.wikipedia.org/wiki/Curve_orientation */
/* negative determinant -- negatively oriented (clockwise,CW) curve;
 * positive determinant -- positively oriented (couterclockwise,CCW) curve;
 */
/* CROSS PRODUCT (matrix form of commath.h xprod2()) */
double XPROD2(double P[2][2]) /* XPROD2(t2XY P) or XPROD2(double **P) */
{ /* calling syntax: xprod2(X1,Y1,X2,Y2); */
    return(P[0][_x_]*P[1][_y_]-P[0][_y_]*P[1][_x_]); /* direction Z-axis */
}
/* Triangle Orientation: sign of TWICE of triangle signed area (determinant) */
int TriOrient(double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* TriOrient(X1,Y1,X2,Y2,X3,Y3); +1 couterclockwise, 0 collinear, -1 clockwise; */
    double S=(X2-X1)*(Y3-Y1)-(X3-X1)*(Y2-Y1);
    return( sign(S) ); /* LineLeft(); */
}
int TriORIENT(double *X, double *Y)
{ /* TriORIENT(X,Y); +1 couterclockwise, 0 collinear, -1 clockwise; */
    double S=(X[1]-X[0])*(Y[2]-Y[0])-(X[2]-X[0])*(Y[1]-Y[0]);
    return( sign(S) );
}
int TRIORIENT(double P[3][2]) /* TRIORIENT(tTriXY P) or TRIORIENT(double **P) */
{ /* double P[3][2]; TRIORIENT(P); 1 couterclockwise, 0 collinear, -1 clockwise; */
    double S = (P[1][_x_]-P[0][_x_])*(P[2][_y_]-P[0][_y_])\
              -(P[2][_x_]-P[0][_x_])*(P[1][_y_]-P[0][_y_]);
    return( sign(S) );
}
/* isCCW: TRUE iff triangle is counterclockwised; matrix form of isLeft(); */
bool isCCW(double P[3][2]) /* isCCW(tTriXY P) or isCCW(double **P) */
{/* calling syntax: double P[3][2]; isCCW(P); 1 couterclockwise, 0 otherwise; */
    double S = (P[1][_x_]-P[0][_x_])*(P[2][_y_]-P[0][_y_])\
              -(P[2][_x_]-P[0][_x_])*(P[1][_y_]-P[0][_y_]);
    return( S>0.0 );

}
/* isCW: TRUE iff triangle is clockwised; matrix form of isRight(); */
bool isCW(double P[3][2]) /* isCW(tTriXY P) or isCW(double **P) */
{
    double S = (P[1][_x_]-P[0][_x_])*(P[2][_y_]-P[0][_y_])\
              -(P[2][_x_]-P[0][_x_])*(P[1][_y_]-P[0][_y_]);
    return( S<0.0 );
}

/* triangle signed area & absolute area: http://en.wikipedia.org/wiki/Triangle */
double TriDet2(double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* TriDet2(X1,Y1,X2,Y2,X3,Y3); TWICE of triangle signed area (determinant) */
  /* >0 (X3,Y3) left to infinite line <X1Y1-X2Y2>, 0 on-line, <0 right of line */
    return((X2-X1)*(Y3-Y1)-(X3-X1)*(Y2-Y1));
}
double TriDet(double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* TriDet(X1,Y1,X2,Y2,X3,Y3); triangle signed area (determinant) */
    return(0.5*((X2-X1)*(Y3-Y1)-(X3-X1)*(Y2-Y1))); /* Notes20110513Deng */
}
/* absolute area of a triangle (X1,Y1)-(X2,Y2)-(X3,Y3) */
double TriArea2(double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* TriArea2(X1,Y1,X2,Y2,X3,Y3); TWICE of triangle absolute area */
    return(fabs((X2-X1)*(Y3-Y1)-(X3-X1)*(Y2-Y1)));
}
double TriArea(double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* TriArea(X1,Y1,X2,Y2,X3,Y3); triangle absolute area */
    return(0.5*fabs((X2-X1)*(Y3-Y1)-(X3-X1)*(Y2-Y1)));
}
/* array form of traingle area calculation */
double TriDET2(double *X, double *Y)
{ /* TriDET2(X,Y); TWICE triangle signed area (determinant); X,Y arrays/pointers; */
/* >0 if (X[2],Y[2]) left to line X[0]Y[0]-X[1]Y[1], 0 on-line, <0 right of line */
    return((X[1]-X[0])*(Y[2]-Y[0])-(X[2]-X[0])*(Y[1]-Y[0]));
}
double TriDET(double *X, double *Y)
{ /* TriDET(X,Y); triangle signed area (determinant); X,Y as arrays or pointers; */
    return(0.5*((X[1]-X[0])*(Y[2]-Y[0])-(X[2]-X[0])*(Y[1]-Y[0])));
}
/* absolute area of a triangle (X[0],Y[0])-(X[1],Y[1])-(X[2],Y[2]) */
double TriAREA2(double *X, double *Y)
{ /* TriAREA2(X,Y); TWICE of triangle absolute area; X,Y as arrays or pointers; */
    return(fabs((X[1]-X[0])*(Y[2]-Y[0])-(X[2]-X[0])*(Y[1]-Y[0])));
}
double TriAREA(double *X, double *Y)
{ /* TriAREA(X,Y); triangle absolute area; X,Y as arrays or pointers; */
    return(0.5*fabs((X[1]-X[0])*(Y[2]-Y[0])-(X[2]-X[0])*(Y[1]-Y[0])));
}
/* matrix form of traingle area calculation */
double TRIDET2(double P[3][2]) /* TRIDET2(tTriXY P) or TRIDET2(double **P) */
{ /* double P[3][2]; TRIDET2(P); TWICE triangle signed area (determinant); */
  /* >0 if (P[2]) left to line P[0]-P[1], 0 on-line, <0 right of line */
    return( (P[1][_x_]-P[0][_x_])*(P[2][_y_]-P[0][_y_])\
           -(P[2][_x_]-P[0][_x_])*(P[1][_y_]-P[0][_y_]));
}
double TRIDET(double P[3][2]) /* TRIDET(tTriXY P) or TRIDET(double **P) */
{ /* double P[3][2]; TRIDET(P); triangle signed area (determinant); */
    return(0.5*((P[1][_x_]-P[0][_x_])*(P[2][_y_]-P[0][_y_])\
               -(P[2][_x_]-P[0][_x_])*(P[1][_y_]-P[0][_y_])));
}
double TRIAREA2(double P[3][2]) /* TRIAREA2(tTriXY P) or TRIAREA2(double **P) */
{ /* double P[3][2]; TRIAREA2(P); TWICE triangle absolute area; */
    return(fabs((P[1][_x_]-P[0][_x_])*(P[2][_y_]-P[0][_y_])\
               -(P[2][_x_]-P[0][_x_])*(P[1][_y_]-P[0][_y_])));
}
double TRIAREA(double P[3][2]) /* TRIAREA(tTriXY P) or TRIAREA(double **P) */
{ /* double P[3][2]; TRIAREA(P); triangle absolute area; */
    return(0.5*fabs((P[1][_x_]-P[0][_x_])*(P[2][_y_]-P[0][_y_])\
                   -(P[2][_x_]-P[0][_x_])*(P[1][_y_]-P[0][_y_])));
}

/* centroid (geometric center, barycenter) of a triangle */
/* http://en.wikipedia.org/wiki/Centroid */
void TriCntr(double *CX, double *CY,\
             double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: TriCntr(&CX,&CY,X1,Y1,X2,Y2,X3,Y3); */
    *CX=(X1+X2+X3)/3;
    *CY=(Y1+Y2+Y3)/3;
    return;
}
void TriCNTR(double *CX, double *CY, double *X, double *Y)
{ /* calling syntax: TriCNTR(&CX,&CY,X,Y); or TriCNTR(&CX,&CY,&X[0],&Y[0]); */
    *CX=(X[0]+X[1]+X[2])/3;
    *CY=(Y[0]+Y[1]+Y[2])/3;
    return;
}
double *TRICNTR(double P[3][2]) /* *TRICNTR(tTriXY P) or *TRICNTR(double **P) */
{ /* calling syntax: C=TRICNTR(P); */
    double *C; /* tXY C; */ /* double C[2]; */
    C=malloc(2*sizeof(double)); if (C==NULL) return(NULL);
    C[_x_]=(P[0][_x_]+P[1][_x_]+P[2][_x_])/3;
    C[_y_]=(P[0][_y_]+P[1][_y_]+P[2][_y_])/3;
    return(C);
}
/* perimeter of a triangle */
double TriPeri(double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: TriPeri(X1,Y1,X2,Y2,X3,Y3); */
    return(hypot(X2-X1,Y2-Y1)+hypot(X3-X2,Y3-Y2)+hypot(X1-X3,Y1-Y3));
}
double TriPERI(double *X, double *Y)
{ /* calling syntax: TriPERI(X,Y); or TriPERI(&X[0],&Y[0]); */
    return( hypot(X[1]-X[0],Y[1]-Y[0])+hypot(X[2]-X[1],Y[2]-Y[1])\
           +hypot(X[0]-X[2],Y[0]-Y[2]) );
}
double TRIPERI(double P[3][2]) /* TRIPERI(tTriXY P) or TRIPERI(double **P) */
{ /* calling syntax: TRIPERI(X,Y); or TRIPERI(&X[0],&Y[0]); */
    return( hypot(P[1][_x_]-P[0][_x_],P[1][_y_]-P[0][_y_])\
           +hypot(P[2][_x_]-P[1][_x_],P[2][_y_]-P[1][_y_])\
           +hypot(P[0][_x_]-P[2][_x_],P[0][_y_]-P[2][_y_]) );
}

/* -------------------------------------------------------------------------- */
/* NOTE: indexing([0],[1],[2],[3]) for arrays, subscripts(1,2,3,4) for vertices; */
/* signed area (determinant) of a quadrilateral (X1,Y1)-(X2,Y2)-(X3,Y3)-(X4,Y4) */
/* Dan_Sunday_Area_Triangles_Polygons_2D_3D.pdf */
/* sign tells Quadrilateral Orientation & Is a point left/right to a triangle */
double QuadDet2(double X1, double Y1, double X2, double Y2,\
                double X3, double Y3, double X4, double Y4)
{ /* QuadDet2(X1,Y1,X2,Y2,X3,Y3,X4,Y4); TWICE of quadrilateral signed area */
    return((X3-X1)*(Y4-Y2)-(X4-X2)*(Y3-Y1));
}
double QuadDet(double X1, double Y1, double X2, double Y2,\
               double X3, double Y3, double X4, double Y4)
{ /* QuadDet(X1,Y1,X2,Y2,X3,Y3,X4,Y4); quadrilateral signed area (determinant) */
    return(0.5*((X3-X1)*(Y4-Y2)-(X4-X2)*(Y3-Y1)));
}
/* absolute area of a quadrilateral (X1,Y1)-(X2,Y2)-(X3,Y3)-(X4,Y4) */
double QuadArea2(double X1, double Y1, double X2, double Y2,\
                 double X3, double Y3, double X4, double Y4)
{ /* QuadArea2(X1,Y1,X2,Y2,X3,Y3,X4,Y4); TWICE of quadrilateral absolute area */
    return(fabs((X3-X1)*(Y4-Y2)-(X4-X2)*(Y3-Y1)));
}
double QuadArea(double X1, double Y1, double X2, double Y2,\
                double X3, double Y3, double X4, double Y4)
{ /* QuadArea(X1,Y1,X2,Y2,X3,Y3,X4,Y4); quadrilateral absolute area */
    return(0.5*fabs((X3-X1)*(Y4-Y2)-(X4-X2)*(Y3-Y1)));
}
/* perimeter of a quadrilateral (X1,Y1)-(X2,Y2)-(X3,Y3)-(X4,Y4) */
double QuadPeri(double X1, double Y1, double X2, double Y2,\
                double X3, double Y3, double X4, double Y4)
{ /* QuadPeri(X1,Y1,X2,Y2,X3,Y3,X4,Y4); */
    return( hypot(X2-X1,Y2-Y1)+hypot(X3-X2,Y3-Y2)\
           +hypot(X4-X3,Y4-Y3)+hypot(X1-X4,Y1-Y4) );
}

/* array form for quadrilateral functions */
/* signed area (determinant) of quadrilateral XY[0]-XY[1]-XY[2]-XY[3] */
double QuadDET2(double *X, double *Y)
{ /* QuadDET2(X,Y); TWICE of quadrilateral signed area (determinant) */
    return((X[2]-X[0])*(Y[3]-Y[1])-(X[3]-X[1])*(Y[2]-Y[0]));
}
double QuadDET(double *X, double *Y)
{ /* QuadDET(X,Y); quadrilateral signed area (determinant) */
    return(0.5*((X[2]-X[0])*(Y[3]-Y[1])-(X[3]-X[1])*(Y[2]-Y[0])));
}
/* absolute area of quadrilateral XY[0]-XY[1]-XY[2]-XY[3] */
double QuadAREA2(double *X, double *Y)
{ /* QuadAREA2(X,Y); TWICE of quadrilateral absolute area */
    return(fabs((X[2]-X[0])*(Y[3]-Y[1])-(X[3]-X[1])*(Y[2]-Y[0])));
}
double QuadAREA(double *X, double *Y)
{ /* QuadAREA(X,Y); quadrilateral absolute area; X,Y as arrays or pointers; */
    return(0.5*fabs((X[2]-X[0])*(Y[3]-Y[1])-(X[3]-X[1])*(Y[2]-Y[0])));
}
/* perimeter of a quadrilateral XY[0]-XY[1]-XY[2]-XY[3] */
double QuadPERI(double *X, double *Y)
{ /* QuadPERI(X,Y); X,Y as arrays or pointers; */
    return( hypot(X[1]-X[0],Y[1]-Y[0])+hypot(X[2]-X[1],Y[2]-Y[1])\
           +hypot(X[3]-X[2],Y[3]-Y[2])+hypot(X[0]-X[3],Y[0]-Y[3]) );
}

/* matrix form for quadrilateral functions */
/* signed area (determinant) of quadrilateral XY[0]-XY[1]-XY[2]-XY[3] */
double QUADDET2(double P[4][2]) /* QUADDET2(tQuadXY P) or QUADDET2(double **P) */
{ /* QUADDET2(P); TWICE of quadrilateral signed area (determinant) */
    return((P[2][_x_]-P[0][_x_])*(P[3][_y_]-P[1][_y_])\
          -(P[3][_x_]-P[1][_x_])*(P[2][_y_]-P[0][_y_]));
}
double QUADDET(double P[4][2]) /* QUADDET(tQuadXY P) or QUADDET(double **P) */
{ /* QUADDET(P); quadrilateral signed area (determinant) */
    return(0.5*((P[2][_x_]-P[0][_x_])*(P[3][_y_]-P[1][_y_])\
               -(P[3][_x_]-P[1][_x_])*(P[2][_y_]-P[0][_y_])));
}
/* absolute area of quadrilateral XY[0]-XY[1]-XY[2]-XY[3] */
double QUADAREA2(double P[4][2]) /* QUADAREA2(tQuadXY P) or QUADAREA2(double **P) */
{ /* QUADAREA2(P); TWICE of quadrilateral absolute area */
    return(fabs((P[2][_x_]-P[0][_x_])*(P[3][_y_]-P[1][_y_])\
               -(P[3][_x_]-P[1][_x_])*(P[2][_y_]-P[0][_y_])));

}
double QUADAREA(double P[4][2]) /* QUADAREA(tQuadXY P) or QUADAREA(double **P) */
{ /* calling syntax: double P[4][2]; QUADAREA(P); quadrilateral absolute area; */
    return(0.5*fabs((P[2][_x_]-P[0][_x_])*(P[3][_y_]-P[1][_y_])\
                   -(P[3][_x_]-P[1][_x_])*(P[2][_y_]-P[0][_y_])));
}
/* perimeter of a quadrilateral XY[0]-XY[1]-XY[2]-XY[3] */
double QUADPERI(double P[4][2]) /* QUADPERI(tQuadXY P) or QUADPERI(double **P) */
{ /* calling syntax: double P[4][2]; QUADPERI(P); */
    return( hypot(P[1][_x_]-P[0][_x_],P[1][_y_]-P[0][_y_])\
           +hypot(P[2][_x_]-P[1][_x_],P[2][_y_]-P[1][_y_])\
           +hypot(P[3][_x_]-P[2][_x_],P[3][_y_]-P[2][_y_])\
           +hypot(P[0][_x_]-P[3][_x_],P[0][_y_]-P[3][_y_]) );
}

/* ########################################################################## */
/* functions for polygon determinant/area/orientation/convex etc. */
/* ########################################################################## */
/* signed area (determinant) of 2D simple polygon (non-self-intersecting)
 * (sign tells the curve orientation of (convex) polygon) */
/* http://mathworld.wolfram.com/PolygonArea.html; */
/* http://en.wikipedia.org/wiki/Curve_orientation & Cross_product */
/* negative determinant -- negatively oriented (clockwise,CW) curve;
 * positive determinant -- positively oriented (couterclockwise,CCW) curve; */
double polydet(double *X, double *Y, unsigned int N)
{ /* calling syntax: polydet(X,Y,N); or polydet(&X[0],&Y[0],N) */
    unsigned int i=0, j=N-1; /* MUST N>=3; INITIAL j=N-1(i=0); */
    double S=0.0; /* area; z-component of cross product <O-X,O-Y>; O-origin; */

    if (N<3) return(0.0); /* S=0 collinear */

    /* Algorithm 1: http://mathworld.wolfram.com/PolygonArea.html; */
    /* for (i=0;i<N;i++) {S+=X[i]*Y[j]-X[j]*Y[i]; j=i;} */
    /* Algorithm 2: Darel Rex Finley: http://alienryderflex.com/polygon_area/
     * CORRECT sign vs. (Ultra-Easy Polygon Area Algorithm With C Code.pdf) */
    for ( ; i<N; j=i++ ) S+=(X[i]+X[j])*(Y[i]-Y[j]); /* Eq.1.14, O'Rourke1998 */
    /* Notes20110513Deng; (initial i=0, j=N-1; after each loop j=i; i++;) */

    return(0.5*S); /* Right-Hand-Rule: >0 couterclockwise; <0 clockwise; */
}
/* IMPROVED algorithm: signed area (determinant) of polygon by Daniel Sunday */
/* Dan_Sunday_Area_Triangles_Polygons_2D_3D.pdf; Notes20110923Deng; */
double polyDet(double *X, double *Y, unsigned int N)
{ /* calling syntax: polyDet(X,Y,N); or polyDet(&X[0],&Y[0],N) */
    unsigned int i=1, j=2, k=0;
    double S=0.0;

    if (N<3) return(0.0); /* MUST N>=3 */
    for ( ; j<N; i++,j++,k++ ) S+=X[i]*(Y[j]-Y[k]); /* Notes20110923Deng */
    /* printf("N=%d,(i,j,k)=(%d,%d,%d);\n", N,i,j,k); exit i=N-1, j=N, k=N-2;*/
    S+=X[i]*(Y[0]-Y[k]); S+=X[0]*(Y[1]-Y[i]);
    /* S+=X[N-1]*(Y[0]-Y[N-2]); S+=X[0]*(Y[1]-Y[N-1]); */
    /* extend array: (X[N],Y[N])=(X[0],Y[0]); (X[N+1],Y[N+1])=(X[1],Y[1]); */

    return(0.5*S); /* Right-Hand-Rule: >0 couterclockwise; <0 clockwise; */
}
/* EVEN BETTER CODE (pointers vs. values): polygon signed area (determinant) */
double PolyDet(double *X, double *Y, unsigned int N)
{ /* calling syntax: PolyDet(X,Y,N); or PolyDet(&X[0],&Y[0],N) */
    double *Xi=X+1, *Yj=Y+2, *Yk=Y, *YN=Y+N; /* MUST N>=3; j=i+1, k=i-1; */
    double S=0.0; /* Matlab: S = sum(X([2:end 1]).*(Y([3:end 1 2])-Y))/2; */

    if (N<3) return(0.0);
    /* for ( ; Yj<YN; Xi++,Yj++,Yk++ ) S+=(*Xi)*(*Yj-*Yk); */
    for ( ; Yj<YN; ) S+=(*Xi++)*((*Yj++)-(*Yk++)); /* Notes20110923Deng */
    S+=(*Xi)*(Y[0]-(*Yk++));
    S+=X[0]*(Y[1]-(*Yk));

    return(0.5*S); /* Right-Hand-Rule: >0 couterclockwise; <0 clockwise; */
}
/* absolute area of 2D simple polygon (non-self-intersecting) */
double PolyArea(double *X, double *Y, unsigned int N)
{
    double S=PolyDet(X,Y,N); /* S=polyDet(X,Y,N); S=polydet(X,Y,N); */
    return( (S>0.0)? S : -S );
}
/* orientation of a simple polygon (MOST ROBUST! Collinear points Okay;) */
int PolyORIENT(double *X, double *Y, unsigned int N)
{ /* calling syntax: PolyORIENT(X,Y,N); or PolyORIENT(&X[0],&Y[0],N) */
    double S=PolyDet(X,Y,N); /* S=polyDet(X,Y,N); S=polydet(X,Y,N); */
    return(sign(S)); /* 1 counterclockwise; 0 none(degenerate), -1 clockwise; */
}
/* alternative for the orientation of a simple polygon (NO COLLINEAR points) */
/* O'Rourke1998 pp.12 Fig.1.11: rightmost lowest vertex must be strictly convex; */
/* rightmost-lowest-vertex method faster than signed-area method; BUT should
 * AVOID COINCIDING VERTICES http://cgafaq.info/wiki/Simple_Polygon_Orientation */
/* Daniel Sunday http://softsurfer.com/Archive/algorithm_0101/ */
int polyOrient(double *X, double *Y, unsigned int N)
{ /* calling syntax: polyOrient(X,Y,N); or polyOrient(&X[0],&Y[0],N) */
  /* Return: +1 counterclockwise; =0 none (degenerate), -1 clockwise; */
    unsigned int i=1, i0, i1=0, i2; /* i1: rightmost lowest vertex */
    double S, Xi1=X[0], Yi1=Y[0]; /* XY[i1] rightmost lowest vertex coordinate */

    if (N<3) return(0); /* S=0 collinear */
    for (i=1; i<N; i++) { /* rightmost lowest vertex must be strictly convex */
        if ( (Y[i]<Yi1) || ( (Y[i]==Yi1) && (X[i]>Xi1) ) ) {
            i1=i; Xi1=X[i]; Yi1=Y[i];
        } /* REF.: idxRightYmin() */
    }
    /* find neighbor indexing (NOTE: RESTRICTED TO NONE COINCIDING VERTICES!) */
    if (i1==0) { i0=N-1; i2=1; }
    else if (i1==N-1) { i0=N-2; i2=0; }
    else { i0=i1-1; i2=i1+1; } /* i0--previous, i2--next */
    /* orientation: ccw <=> the edge leaving is left of the entering edge */
    S=(Xi1-X[i0])*(Y[i2]-Y[i0])-(X[i2]-X[i0])*(Yi1-Y[i0]); /* TriDet2(i0,i1,i2) */
    return(sign(S));
}
/* IMPROVED rightmost-lowest-vertex method (allowing COINCIDING VERTICES) */
int PolyOrient(double *X, double *Y, unsigned int N)
{ /* calling syntax: PolyOrient(X,Y,N); or PolyOrient(&X[0],&Y[0],N) */
  /* Return: +1 counterclockwise; =0 none (degenerate), -1 clockwise; */
    unsigned int i=1, i0, i1=0, i2; /* i1: rightmost lowest vertex */
    double S; /* (X[i1],Y[i1]) rightmost lowest vertex coordinate */

    if (N<3) return(0); /* S=0 collinear */
    for (i=1; i<N; i++) { /* rightmost lowest vertex must be strictly convex */
        if ( (Y[i]<Y[i1]) || ( (Y[i]==Y[i1]) && (X[i]>X[i1]) ) ) i1=i;
    } /* REF.: idxRightYmin() */
    /* find neighbor indexing (REMOVE COINCIDING/REPEATED(COLLINEAR) POINTS!) */
    /* In C (ISO 1999), c=a%b has the same sign as dividend a; thus +N if -1; */
    i0 = (i1+N-1) % N; i2 = (i1+1) % N; /* i0--previous, i2--next (cyclic) */
    /* check XY(i0!=i1) & XY(i2!=i1) to avoid infinite loop in special cases */
    while ( (X[i0]==X[i1]) && (Y[i0]==Y[i1]) )
        { i0 = (i0+N-1)%N; if (i0==i1) return(0); }
    while ( (X[i2]==X[i1]) && (Y[i2]==Y[i1]) )
        { i2 = (i2+1)%N; if (i2==i1) return(0); }
    /* orientation: ccw <=> the edge leaving is left of the entering edge */
    S=(X[i1]-X[i0])*(Y[i2]-Y[i0])-(X[i2]-X[i0])*(Y[i1]-Y[i0]); /* TriDet2() */
    return(sign(S));
}
/* Tell if a 2D simple polygon (NO self-intersection/holes) is convex or concave
 * convex -> 1; concave -> -1; return 0 for incomputable (e.g.: collinear); */
int PolyConvex(double *X, double *Y, unsigned int N)
{ /* calling syntax: PolyConvex(X,Y,N); or PolyConvex(&X[0],&Y[0],N); */
  /* http://mathworld.wolfram.com/ConvexPolygon.html; */
  /* OptimalConvexDecomp.pdf: decompose a concave polygon into convex polygons;
   * Bernard Chazelle & David P. Dobkin, "Optimal convex decompositions", 1985 */
    unsigned int i=0, j=1, k=2;
    int flag=0;
    double Z=0.0; /* z-component of cross product <dX,dY>; (TWICE signed area) */

    if (N<3) return(0); /* 0 -- collinear */
    /* Paul_Bourke_1998_Polygon_Orientation_Convexity.pdf, convex.c; */
    for (i=0;i<N;i++) { /* Notes20110513Deng */
        j = (i+1) % N; /* modulus operator ('%') for the remainder */
        k = (i+2) % N; /* cyclic */
        /* Z = (X[j]-X[i])*(Y[k]-Y[j])-(X[k]-X[j])*(Y[j]-Y[i]); /* TriDET2() */
        Z = (X[j]-X[i])*(Y[k]-Y[i])-(X[k]-X[i])*(Y[j]-Y[i]); /* TriDET2() */
        if (Z<0) flag |= 1; /* Bitwise Inclusive OR ('|=') */
        else if (Z>0) flag |= 2; /* binary 1: 01; 2: 10; 3: 11; */
        if (flag==3) return(-1); /* concave == -1; */
    } /* ignore (Z==0) cases (adjacent collinear vertices okay) */
    /* if (flag!=0) return(1); /* convex == 1; */
    /* else return(0); /* collinear == 0; */
    return(flag!=0); /* convex == 1; collinear ==0; concave == -1; */
}
/* perimeter of 2D polygon (CLOSED perimeter for CLOSED/OPEN polygon) */
/* OPEN polyline p1-p2-...-pN; CLOSED polygon p1-p2-...-pN-p1; */
/* (NOTE: for N=2, L=2*SegmentLength (closed polygon p1-p2-p1);) */
double PolyPeri(double *X, double *Y, unsigned int N)
{ /* calling syntax: PolyPeri(X,Y,N); */
    unsigned int i=0, j=N-1; /* MUST N>=2; INITIAL j=N-1(i=0); */
    double L=0.0; /* L: perimeter (CLOSED) */
    if (N<2) return(0); /* otherwise unsigned int j=N-1 becomes infinity */
    for ( i=0,j=N-1; i<N; j=i++ ) /* initial i=0, j=N-1; loop j=i, i++; */
        L += hypot(X[i]-X[j],Y[i]-Y[j]); /* calc CLOSED polygon */
    /* alternative method: j=(i+1)%PN; /* cyclic j guarantee CLOSED polygon */
    return(L);
}
/* centroid (geometric center, barycenter) of 2D simple polygon */
/* (non-self-intersecting) http://en.wikipedia.org/wiki/Centroid */
bool PolyCntr(double *CX, double *CY, double *X, double *Y, unsigned int N)
{ /* calling syntax: PolyCntr(&CX,&CY,X,Y,N); PolyCntr(&CX,&CY,&X[0],&Y[0],N); */
    unsigned int i=0, j=N-1; /* MUST N>=3; INITIAL j=N-1(i=0); */
    double S=PolyDet(X,Y,N), C=0.0;

    if (S==0.0) return(false);
    *CX=0.0; *CY=0.0;
    for ( i=0,j=N-1; i<N; j=i++ ) { /* initial i=0, j=N-1; loop j=i, i++; */
        C=X[i]*Y[j]-X[j]*Y[i];
        *CX += (X[i]+X[j])*C;
        *CY += (Y[i]+Y[j])*C;
    }
    *CX /= 3.0*S; /* 3=6*0.5 */
    *CY /= 3.0*S;
    return(true);
}

/* ########################################################################## */
/* functions for point/line/triangle/polygon relationships */
/* ########################################################################## */
/* check if point (Xv,Yv) is to the left/right of infinite line <X1Y1-X2Y2> */
int LineLeft(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* 1 (Xv,Yv) left to infinite line <X1Y1-X2Y2>, 0 on-line, -1 right of line */
    double S=(X2-X1)*(Yv-Y1)-(Xv-X1)*(Y2-Y1); /* TriDet2(); */
    return( sign(S) ); /* TriOrient(); */
}
int LineRight(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* 1 (Xv,Yv) right to infinite line <X1Y1-X2Y2>, 0 on-line, -1 left of line */
    double Z=(Xv-X1)*(Y2-Y1)-(X2-X1)*(Yv-Y1); /* -TriDet2(); */
    return( sign(Z) );
}
bool isCollinear(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* 1 (Xv,Yv) on the infinite line <X1Y1-X2Y2> (thus collinear); 0 otherwise; */
    return( (X2-X1)*(Yv-Y1)==(Xv-X1)*(Y2-Y1) ); /* fabs()<EPS in isonLine(); */
}
bool isLeft(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* 1 (Xv,Yv) left to infinite line <X1Y1-X2Y2>, 0 on-line or right of line */
    return( ((X2-X1)*(Yv-Y1)-(Xv-X1)*(Y2-Y1))>0 );
}
bool isLeftOn(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* 1 (Xv,Yv) left-to or on the infinite line <X1Y1-X2Y2>, 0 right of line */
    return( ((X2-X1)*(Yv-Y1)-(Xv-X1)*(Y2-Y1))>=0 );
}
bool isRight(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* 1 (Xv,Yv) right to infinite line <X1Y1-X2Y2>, 0 on-line or left of line */
    return( ((Xv-X1)*(Y2-Y1)-(X2-X1)*(Yv-Y1))<0 );
}
bool isRightOn(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* 1 (Xv,Yv) right to or on the infinite line <X1Y1-X2Y2>, 0 left of line */
    return( ((Xv-X1)*(Y2-Y1)-(X2-X1)*(Yv-Y1))<=0 );
}

/* REF.: \polylib\ref\progtips\comgeom_isOn_isIn\isOn.m,isIn.m,isIn(L/H/1/2).m;
 * [  X1<X2  |     Xv    ] * [ X1==X2  |     Xv    ] * [  X1>X2  |     Xv    ]
 * [X1=2,X2=8| 1|2 5 8|9 ] * [X1=5,X2=5| 1|2 5 8|9 ] * [X1=8,X2=2| 1|2 5 8|9 ]
 * [---------|--|-----|--] * [------------|-----|--] * [------------|-----|--]
 * [  X1<=Xv | 0|1 1 1|1 ] * [  X1<=Xv | 0|0 1 1|1 ] * [  X1<=Xv | 0|0 0 1|1 ]
 * [  X2<Xv  | 0|0 0 0|1 ] * [  X2<Xv  | 0|0 0 1|1 ] * [  X2<Xv  | 0|0 1 1|1 ]
 * [  X1<Xv  | 0|0 1 1|1 ] * [  X1<Xv  | 0|0 0 1|1 ] * [  X1<Xv  | 0|0 0 0|1 ]
 * [  X2<=Xv | 0|0 0 1|1 ] * [  X2<=Xv | 0|0 1 1|1 ] * [  X2<=Xv | 0|1 1 1|1 ]
 * Thus, ( (Xmin<=Xv) != (Xmax<Xv) ) TRUE iff Xmin<=Xv<=Xmax; (isOn();)
 *  (( (Xmin<Xv)!=(Xmax<=Xv) ) && (X1!=X2)) TRUE iff  Xmin<Xv<Xmax; (isIn();)
 *        ( (X1<=Xv) != (X2<=Xv) )   TRUE iff Xmin<=Xv<Xmax;  (isInL();)
 *         ( (X1<Xv) != (X2<Xv) )    TRUE iff  Xmin<Xv<=Xmax; (isInH();)
 * REF.: commath.h isopsign(), iseqsign(); */
/* check if Xv is BOUNDED BY (In or On) range [X1,X2]; TRUE iff Xmin<=Xv<=Xmax; */
#define isOn(X1,X2,Xv) ( ((X1)-(Xv))*((X2)-(Xv))<=0 )
/* bool isOn(double X1, double X2, double Xv)
{ /* calling syntax: isOn(X1,X2,Xv); TRUE iff Xmin<=Xv<=Xmax; (1 for boundary); */
    /* return( (X1-Xv)*(X2-Xv)<=0 ); */
/*     return ( ( (X1<=Xv)&&(Xv<=X2) ) || ( (X1>=Xv)&&(Xv>=X2) ) );
    /* return( (X1<=X2)? ( (X1<=Xv)^(X2<Xv) ) : ( (X1<Xv)^(X2<=Xv) ) ); */
/* } */
/* check if Xv is INSIDE (In) range (X1,X2); TRUE iff Xmin<Xv<Xmax; */
#define isIn(X1,X2,Xv) ( ((X1)-(Xv))*((X2)-(Xv))<0 )
/* bool isIn(double X1, double X2, double Xv)
{ /* calling syntax: isIn(X1,X2,Xv); TRUE iff Xmin<Xv<Xmax; (0 for boundary); */
    /* return( (X1-Xv)*(X2-Xv)<0 ); */
/*     return ( ( (X1<Xv)&&(Xv<X2) ) || ( (X1>Xv)&&(Xv>X2) ) );
    /* return( (X1<X2)? ((X1<Xv)^(X2<=Xv)) : ((X1<=Xv)^(X2<Xv)) ); */
/* } */
/* check if Xv is between range ([X1,X2]); situation varies on boundary values;
 * ( isInL-Low(Xmin) for (is)PnPoly(); isInH-High(Xmax); isIn1-X1; isIn2-X2; ) */
#define isInL(X1,X2,Xv) ( ((X1)<=(Xv)) ^ ((X2)<=(Xv)) )
/* bool isInL(double X1, double X2, double Xv)
{ /* isInL(X1,X2,Xv); 1 iff Xmin<=Xv<Xmax; (1 for Xmin, 0 for Xmax; 0 if X1==X2) */
/*     return ( (X1<=Xv) ^ (X2<=Xv) ); /* return ( (X1<=Xv)!=(X2<=Xv) ); XOR; */
    /*return ( (X1>Xv) ^ (X2>Xv) ); /* return ( (X1>Xv)!=(X2>Xv) ); XOR; */
/* } */
#define isInH(X1,X2,Xv) ( ((X1)<(Xv)) ^ ((X2)<(Xv)) )
/* bool isInH(double X1, double X2, double Xv)
{ /* isInH(X1,X2,Xv); 1 iff Xmin<Xv<=Xmax; (0 for Xmin, 1 for Xmax; 0 if X1==X2) */
/*     return ( (X1<Xv) ^ (X2<Xv) ); /* return ( (X1<Xv)!=(X2<Xv) ); XOR; */
    /* return ( (X1>=Xv)^(X2>=Xv) ); /* return ( (X1>=Xv)!=(X2>=Xv) ); XOR;*/
/* } */
#define isIn1(X1,X2,Xv) ( (((X1)-(Xv))*((X2)-(Xv))<0) || ((Xv)==(X1)) )
/* bool isIn1(double X1, double X2, double Xv)
{ /* isIn1(X1,X2,Xv); TRUE iff (Xmin<Xv<Xmax or Xv==X1); (1 for X1, 0 for X2); */
/*     return( (X1<=X2)? ( (X1<=Xv)^(X2<=Xv) ) : ( (X1<Xv)^(X2<Xv) ) );
/* } */
#define isIn2(X1,X2,Xv) ( (((X1)-(Xv))*((X2)-(Xv))<0) || ((Xv)==(X2)) )
/* bool isIn2(double X1, double X2, double Xv)
{ /* isIn2(X1,X2,Xv); TRUE iff (Xmin<Xv<Xmax or Xv==X2); (0 for X1, 1 for X2); */
/*     return( (X1<=X2)? ( (X1<Xv)^(X2<Xv) ) : ( (X1<=Xv)^(X2<=Xv) ) );
/* } */
/* TRUE iff point (Xv,Yv) BOUNDED BY (In or On) closed line segement [X1Y1-X2Y2] */
/* O'Rourke1998 pp.32 Code 1.8 \Comp_Geom_C\Ccode2\tri\tri.c Between() */
bool isOnLseg(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* calling syntax: isOnLseg(X1,Y1,X2,Y2,Xv,Yv); */
    if ( !isCollinear(X1,Y1,X2,Y2,Xv,Yv) ) return(false);
    /* check betweenness on y for vertical line segment; */
    return( (X1==X2)? isOn(Y1,Y2,Xv) : isOn(X1,X2,Xv) ); /* 1: boundary points */
}
/* another version with tolerance EPS (speed slower) */
bool isonLseg(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* calling syntax: isonLseg(X1,Y1,X2,Y2,Xv,Yv); */
    return(fabs(hypot(Xv-X1,Yv-Y1)+hypot(Xv-X2,Yv-Y2)-hypot(X2-X1,Y2-Y1))<EPS);
}
/* check if point (Xv,Yv) is INSIDE (In) closed line segement (X1Y1-X2Y2) */
bool isInLseg(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* calling syntax: isInLseg(X1,Y1,X2,Y2,Xv,Yv); */
    if ( !isCollinear(X1,Y1,X2,Y2,Xv,Yv) ) return(false);
    return( (X1==X2)? isIn(Y1,Y2,Xv) : isIn(X1,X2,Xv) ); /* 0: boundary points */
}
/* check if point (Xv,Yv) is ON an infinite line <X1Y1-X2Y2>; isCollinear(); */
bool isOnLine(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* calling syntax: isOnLine(X1,Y1,X2,Y2,Xv,Yv); ( i.e., isCollinear() ) */
    return( fabs((X2-X1)*(Yv-Y1)-(Xv-X1)*(Y2-Y1))<EPS ); /* ==0 isCollinear() */
} /* version with tolerance EPS ( isCollinear() to strictly compare ==0; ) */

/* -------------------------------------------------------------------------- */
/* Jordan Curve Theorem: each simple closed plane curve divides plane to 2 parts */
/* http://en.wikipedia.org/wiki/Point_in_polygon Ray casting algorithm */
/* Paul Bourke http://paulbourke.net/geometry/insidepoly/ or Randolph Franklin */
/* http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html */
/* or Eric Haines http://erich.realtimerendering.com/ptinpoly/ */
/* or Darel Rex Finley summary http://alienryderflex.com/polygon/ */
bool PnPoly(double *X, double *Y, unsigned int N, double Xv, double Yv)
{ /* calling syntax: PnPoly(X,Y,N,Xv,Yv); or PnPoly(&X[0],&Y[0],N,Xv,Yv); */
    unsigned int i=0, j=N-1;
    bool flag=false; /* 1 for interior points; 0 for exterior points; */
    if (N<2) return(false);
    for ( ; i<N; j=i++ ) { /* Randolph Franklin, or Joseph O'Rourke pp.243 */
        /* Condition 1: ((Y[i]>Yv)!=(Y[j]>Yv)): isInL(Y[i],Y[j],Yv);
               TRUE iff min(Y[i],Y[j])<=Yv<max(Y[i],Y[j]); false for Y[i]==Y[j];
               Y-ray crosses the edge (highest point excluded);
           Condition 2: TRUE iff the edge at the right side of point (Xv,Yv); 
               compare slopes to count same side; disadvantage divide operation;
               IMPROVE to CCW criteria with Yi>Yj adjustment as isPnPoly(); */
        if ( ((Y[i]>Yv)!=(Y[j]>Yv)) && (Xv<(X[j]-X[i])*(Yv-Y[i])/(Y[j]-Y[i])+X[i]) )
            flag=!flag; /* {if (A), B=!B;} equivalent to B=Xor(A,B), B ^= A; */
    }
    return(flag); /* either 0 or 1 for boundary points */
}
/* IMPROVED EXACT Algorithm for PnPoly(); NO divide; Notes20111102Deng; */
/* Check EXACT situation of a point (Xv,Yv) with respect to a Polygon (X,Y,[N]) */
/* (O'Rourke1998 pp.244) char(8-bit integer); EXACT (at the expense of speed) */
/*   0:exterior, 1:interior, 2:on the boundary(edge), 3:on the vertex; */
int PNPoly(double *X, double *Y, unsigned int N, double Xv, double Yv)
{ /* calling syntax: isPnPoly(X,Y,N,Xv,Yv); or isPnPoly(&X[0],&Y[0],N,Xv,Yv); */
    unsigned int i=0, j=N-1;
    bool flag=0;
    /* triangle orientation (TriOrient() or TriDet2()) sequence (Ymin,Ymax,Yv); */
    double S; /* >0 couterclockwise(isLeft), 0 collinear, <0 clockwise; */
    if (N<2) return(0);
    for ( ; i<N; j=i++ ) { /* isInL(Y[i],Y[j],Yv) */
        if ((Y[i]>Yv)!=(Y[j]>Yv)) { /* 0 if Yi==Yj, 0 if Yv==Ymax; 1 if Yv==Ymin; */
            /* S=TriDet2(X[i],Y[i],X[j],Y[j],Xv,Yv); /* TriOrient(): int S; */
            S=(X[j]-X[i])*(Yv-Y[i])-(Xv-X[i])*(Y[j]-Y[i]); /* TriDet2(); */
            /* S==0 iff (Xv,Yv) on polygon boundary (isInL() excludes Yi==Yj); */
            if (S==0.0) return( isInLseg(X[i],Y[i],X[j],Y[j],Xv,Yv)? 2 : 3 );
            if (Y[i]>Y[j]) S = -S; /* reverse orientation if Y[i] higher; */
            /* if (S>0) flag=!flag; /* valid crossing to right-hand-side(RHS) */
            flag ^= (S>0.0); /* Bitwise XOR assignment */
        } else {
            if ( (Y[i]==Y[j]) && isOn(X[i],X[j],Xv) )
                return( isInLseg(X[i],Y[i],X[j],Y[j],Xv,Yv)? 2 : 3 );
        }
    }
    return(flag); /* 0:exterior, 1:interior, 2:boundary(edge), 3:vertex; */
}
/* IMPROVED Algorithm for PnPoly(); NO divide; 1 for boundary; Notes20111102Deng; */
/* Check if a point (Xv,Yv) is BOUNDED BY (On or In) a Polygon (X,Y,[N]) */
bool isPnPoly(double *X, double *Y, unsigned int N, double Xv, double Yv)
{ /* calling syntax: isPnPoly(X,Y,N,Xv,Yv); or isPnPoly(&X[0],&Y[0],N,Xv,Yv); */
    unsigned int i=0, j=N-1;
    bool flag=false; /* 1 for interior points; 0 for exterior points; */
    /* triangle orientation (TriOrient() or TriDet2()) sequence (Ymin,Ymax,Yv); */
    double S; /* >0 couterclockwise(isLeft), 0 collinear, <0 clockwise; */
    if (N<2) return(false);
    for ( ; i<N; j=i++ ) { /* isInL(Y[i],Y[j],Yv) */
        if ((Y[i]>Yv)!=(Y[j]>Yv)) { /* 0 if Yi==Yj, 0 if Yv==Ymax; 1 if Yv==Ymin; */
            /* S=TriDet2(X[i],Y[i],X[j],Y[j],Xv,Yv); /* TriOrient(): int S; */
            S=(X[j]-X[i])*(Yv-Y[i])-(Xv-X[i])*(Y[j]-Y[i]); /* TriDet2(); */
            /* S==0 iff (Xv,Yv) on polygon boundary (isInL() excludes Yi==Yj); */
            if (S==0.0) return(true); /* set TRUE for boundary points */
            if (Y[i]>Y[j]) S = -S; /* reverse orientation if Y[i] higher; */
            /* if (S>0) flag=!flag; /* valid crossing to right-hand-side(RHS) */
            flag ^= (S>0.0); /* Bitwise XOR assignment */
        } else { if ( (Y[i]==Y[j]) && isOn(X[i],X[j],Xv) ) return(true); }
        /* -else{} NOT needed if don't care about points on boundary */
    }
    return(flag); /* already set TRUE for boundary points (deem as interior) */
}
/* Check if a point (Xv,Yv) is INSIDE (In) a Polygon (X,Y,[N]) */
bool isInPoly(double *X, double *Y, unsigned int N, double Xv, double Yv)
{ /* calling syntax: isInPoly(X,Y,N,Xv,Yv); or isInPoly(&X[0],&Y[0],N,Xv,Yv); */
    unsigned int i=0, j=N-1;
    bool flag=false; /* 1 for interior points; 0 for exterior points; */
    /* triangle orientation (TriOrient() or TriDet2()) sequence (Ymin,Ymax,Yv); */
    double S; /* >0 couterclockwise(isLeft), 0 collinear, <0 clockwise; */
    if (N<2) return(false);
    for ( ; i<N; j=i++ ) { /* isInL(Y[i],Y[j],Yv) */
        if ((Y[i]>Yv)!=(Y[j]>Yv)) { /* 0 if Yi==Yj, 0 if Yv==Ymax; 1 if Yv==Ymin; */
            /* S=TriDet2(X[i],Y[i],X[j],Y[j],Xv,Yv); /* TriOrient(): int S; */
            S=(X[j]-X[i])*(Yv-Y[i])-(Xv-X[i])*(Y[j]-Y[i]); /* TriDet2(); */
            /* S==0 iff (Xv,Yv) on polygon boundary (isInL() excludes Yi==Yj); */
            if (S==0.0) return(false); /* set false for boundary points */
            if (Y[i]>Y[j]) S = -S; /* reverse orientation if Y[i] higher; */
            /* if (S>0) flag=!flag; /* valid crossing to right-hand-side(RHS) */
            flag ^= (S>0.0); /* Bitwise XOR assignment */
        } /* -else{if(Y[i]==Y[j])} NOT needed as points on boundary are set 0; */
    }
    return(flag); /* already set false for boundary points (deem as exterior) */
}
/* check if point (Xv,Yv) is ON the boundary of a Polygon (X,Y,[N]) */
bool isOnPoly(double *X, double *Y, unsigned int N, double Xv, double Yv)
{ /* calling syntax: isOnPoly(X,Y,N,Xv,Yv); or isOnPoly(&X[0],&Y[0],N,Xv,Yv); */
    unsigned int i=0, j=N-1;
    for ( ; i<N; j=i++ ) {
        if (isOnLseg(X[i],Y[i],X[j],Y[j],Xv,Yv)) return(true);
    }
    return(false);
}
/* check if a point (Xv,Yv) is BOUNDED BY (On or In) a Triangle X1Y1-X2Y2-X3Y3 */
bool isPnTri(double X1, double Y1, double X2, double Y2, double X3, double Y3,\
             double Xv, double Yv)
{ /* calling syntax: isPnTri(X1,Y1,X2,Y2,X3,Y3,Xv,Yv); */
    double S =TriArea2(X1,Y1,X2,Y2,X3,Y3); /* always S==(S1+S2+S3) if TriDet2(); */
    double S1=TriArea2(X1,Y1,X2,Y2,Xv,Yv);
    double S2=TriArea2(X2,Y2,X3,Y3,Xv,Yv);
    double S3=TriArea2(X3,Y3,X1,Y1,Xv,Yv);
    return( fabs(S1+S2+S3-S)<EPS ); /* S==(S1+S2+S3); 1 for boundary points; */
}
/* check if a point (Xv,Yv) is INSIDE (In) a Triangle X1Y1-X2Y2-X3Y3 */
bool isInTri(double X1, double Y1, double X2, double Y2, double X3, double Y3,\
             double Xv, double Yv)
{ /* calling syntax: isInTri(X1,Y1,X2,Y2,X3,Y3,Xv,Yv); */
    int flag=TriOrient(X1,Y1,X2,Y2,Xv,Yv); /* int() as flag in [-1,0,1]; */
    if (flag!=TriOrient(X2,Y2,X3,Y3,Xv,Yv)) return(false);
    if (flag!=TriOrient(X3,Y3,X1,Y1,Xv,Yv)) return(false); /* LineLeft(); */
    return(true); /* 0 for boundary points */
    /* or check orientation flags: return( (o1 == o2) && (o2 == o3) ); */
}
/* check if a point (Xv,Yv) is ON the boundary of a Triangle X1Y1-X2Y2-X3Y3 */
bool isOnTri(double X1, double Y1, double X2, double Y2, double X3, double Y3,\
             double Xv, double Yv)
{ /* calling syntax: isOnTri(X1,Y1,X2,Y2,X3,Y3,Xv,Yv); */
    return( isOnLseg(X1,Y1,X2,Y2,Xv,Yv) || isOnLseg(X2,Y2,X3,Y3,Xv,Yv)\
         || isOnLseg(X3,Y3,X1,Y1,Xv,Yv) );
}

/* check if a quadrilateral is a convex parallelogram (arbitrary orientation) */
/* Opposite sides of a parallelogram are equal in length (congruent). */
bool isPRL4(double X1, double Y1, double X2, double Y2,\
            double X3, double Y3, double X4, double Y4)
{ /* calling syntax: isPRL4(X1,Y1,X2,Y2,X3,Y3,X4,Y4); */
    double dx12=X2-X1, dx23=X3-X2, dx34=X4-X3, dx41=X1-X4;
    double dy12=Y2-Y1, dy23=Y3-Y2, dy34=Y4-Y3, dy41=Y1-Y4;
    return( (sqsum(dx12,dy12)==sqsum(dx34,dy34))\
         && (sqsum(dx23,dy23)==sqsum(dx41,dy41))\
         && (QuadDet2(X1,Y1,X2,Y2,X3,Y3,X4,Y4)!=0) ); /* NO collinear case */
}
bool isPRLG4(double *X, double *Y)
{ /* calling syntax: isPRLG4(X,Y); or isPRLG4(&X[0],&Y[0]); */
    return( isPRL4(X[0],Y[0],X[1],Y[1],X[2],Y[2],X[3],Y[3]) );
}
/* check if a quadrilateral is a convex rectangle (arbitrary orientation) */
/* A parallelogram is a rectangle if and only if its diagonals are congruent. */
bool isRect(double X1, double Y1, double X2, double Y2,\
            double X3, double Y3, double X4, double Y4)
{ /* calling syntax: isRect(X1,Y1,X2,Y2,X3,Y3,X4,Y4); */
    double dx12=X2-X1, dx23=X3-X2, dx34=X4-X3, dx41=X1-X4; /* sides */
    double dy12=Y2-Y1, dy23=Y3-Y2, dy34=Y4-Y3, dy41=Y1-Y4;
    double dx13=X3-X1, dy13=Y3-Y1, dx24=X4-X2, dy24=Y4-Y2; /* diagonals */
    return( (sqsum(dx13,dy13)==sqsum(dx24,dy24))\
         && (sqsum(dx12,dy12)==sqsum(dx34,dy34))\
         && (sqsum(dx23,dy23)==sqsum(dx41,dy41))\
         && (QuadDet2(X1,Y1,X2,Y2,X3,Y3,X4,Y4)!=0) ); /* NO collinear case */
}
bool isRECT(double *X, double *Y)
{ /* calling syntax: isRECT(X,Y); or isRECT(&X[0],&Y[0]); */
    return( isRect(X[0],Y[0],X[1],Y[1],X[2],Y[2],X[3],Y[3]) );
}
/* check if a quadrilateral is a box (0-degree orientation of a rectangle) */
bool isBox(double X1, double Y1, double X2, double Y2,\
           double X3, double Y3, double X4, double Y4)
{ /* calling syntax: isBox(X1,Y1,X2,Y2,X3,Y3,X4,Y4); */
    /* return( ( (Y2==Y1)^(X2==X1) ) && isRect(X1,Y1,X2,Y2,X3,Y3,X4,Y4) ); */
    /* (adjacent vertices on-axis) AND (opposite vertices off-axis); */
    /* avoid special cases: (1,2,3,4)collinear, (1,2,3)/(2,3,4)collinear etc. */
    return( ((Y2==Y1)^(X2==X1)) && ((Y3==Y2)^(X3==X2))\
         && ((Y4==Y3)^(X4==X3)) && ((Y1==Y4)^(X1==X4))\
         && ((Y3!=Y1)&&(X3!=X1))&&((Y4!=Y2)&&(X4!=X2)) );
}
bool isBOX(double *X, double *Y)
{ /* calling syntax: isBOX(X,Y); or isBOX(&X[0],&Y[0]); */
    return( isBox(X[0],Y[0],X[1],Y[1],X[2],Y[2],X[3],Y[3]) );
}
/* matrix form for quadrilateral shape judgement */
bool ISBOX(double P[4][2]) /* ISBOX(tQuadXY P) or ISBOX(double **P) */
{
    return( isBox(P[0][_x_],P[0][_y_],P[1][_x_],P[1][_y_],\
                  P[2][_x_],P[2][_y_],P[3][_x_],P[3][_y_]) );
}
bool ISRECT(double P[4][2]) /* ISRECT(tQuadXY P) or ISRECT(double **P) */
{
    return( isRect(P[0][_x_],P[0][_y_],P[1][_x_],P[1][_y_],\
                   P[2][_x_],P[2][_y_],P[3][_x_],P[3][_y_]) );
}
bool ISPRLG4(double P[4][2]) /* ISPRLG4(tQuadXY P) or ISPRLG4(double **P) */
{
    return( isPRL4(P[0][_x_],P[0][_y_],P[1][_x_],P[1][_y_],\
                   P[2][_x_],P[2][_y_],P[3][_x_],P[3][_y_]) );
}

/* check if a point (Xv,Yv) is BOUNDED BY (In or On) a Box (X1,Y1,X2,Y2) */
bool isPnBox(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* calling syntax: isPnBox(X1,Y1,X2,Y2,Xv,Yv); */
    return( isOn(X1,X2,Xv) && isOn(Y1,Y2,Yv) ); /* 1 for boundary points */
}
/* check if a point (Xv,Yv) is INSIDE (In) a Box (X1,Y1,X2,Y2) */
bool isInBox(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* calling syntax: isInBox(X1,Y1,X2,Y2,Xv,Yv); */
    return( isIn(X1,X2,Xv) && isIn(Y1,Y2,Yv) ); /* 0 for boundary points */
}
/* check if a point (Xv,Yv) is ON the boundary of a Box (X1,Y1,X2,Y2) */
bool isOnBox(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* calling syntax: isOnBox(X1,Y1,X2,Y2,Xv,Yv); */
    return( ((X1==Xv)||(Xv==X2)) && ((Y1==Yv)||(Yv==Y2)) );
}

/* iff a polygon is approximated by (bounded by) a Circle (OX,OY,R) */
bool ByCIRC(double *OX, double *OY, double *R,\
            double *X, double *Y, const unsigned int N, const double tol)
{ /* syntax: ByCIRC(OX,OY,R,X,Y,N,EPS); or ByCIRC(OX,OY,R,&X[0],&Y[0],N,1e-9); */
    unsigned int i;
    PolyCntr(OX,OY,X,Y,N); /* obtain the origin */
    *R=hypot(X[0]-OX[0],Y[0]-OY[0]); /* obtain the radius */
    for (i=0; i<N; i++) { /* check constant radius within tolerance (tol) */
        if (fabs(hypot(X[i]-OX[0],Y[i]-OY[0])-R[0])>tol) return(false);
    }
    return(true);
}
/* check if a point (Xv,Yv) is BOUNDED BY (On or In) a Circle (OX,OY,R) */
bool isPnCirc(double OX, double OY, double R, double Xv, double Yv)
{ /* calling syntax: isPnCirc(OX,OY,R,Xv,Yv); */
    return( sqsum(Xv-OX,Yv-OY)<=(R*R) );
    /* return( hypot(Xv-OX,Yv-OY)<=R ); /* 1 for boundary points */
}
/* check if a point (Xv,Yv) is INSIDE (In) a Circle (OX,OY,R) */
bool isInCirc(double OX, double OY, double R, double Xv, double Yv)
{ /* calling syntax: isInCirc(OX,OY,R,Xv,Yv); */
    return( sqsum(Xv-OX,Yv-OY)<(R*R) );
    /* return( hypot(Xv-OX,Yv-OY)<R ); /* 0 for boundary points */
}
/* check if a point (Xv,Yv) is ON the boundary of a Circle (OX,OY,R) */
bool isOnCirc(double OX, double OY, double R, double Xv, double Yv)
{ /* calling syntax: isOnCirc(OX,OY,R,Xv,Yv); */
    return( fabs(sqsum(Xv-OX,Yv-OY)-R*R)<EPS ); /* sqsum(Xv-OX,Yv-OY)==(R*R); */
    /* return( hypot(Xv-OX,Yv-OY)==R ); /* fabs(hypot(Xv-OX,Yv-OY)-R)<EPS */
}

/* -------------------------------------------------------------------------- */
/* hypotenuse http://en.wikipedia.org/wiki/Hypot (avoid precision errors) */
/* distance of two points (Euclidean distance) */
double PntDist(double X1, double Y1, double X2, double Y2)
{ /* hypotenuse http://en.wikipedia.org/wiki/Hypot (avoid precision errors) */
    /* double dx=X2-X1, dy=Y2-Y1; */
    /* return(sqrt(dx*dx+dy*dy)); */
    return(hypot(X2-X1,Y2-Y1));
}
double PntDistSQ(double X1, double Y1, double X2, double Y2)
{ /* squared distance (more efficient) of two points */
    double dx=X2-X1, dy=Y2-Y1;
    return(dx*dx+dy*dy);
}
double PNTDIST(tXY P1, tXY P2) /* PNTDIST(double P1[2], double P2[2]) */
{ /* hypotenuse http://en.wikipedia.org/wiki/Hypot (avoid precision errors) */
    return(hypot(P2[_x_]-P1[_x_],P2[_y_]-P1[_y_]));
}
double PNTDISTSQ(tXY P1, tXY P2) /* PNTDISTSQ(double P1[2], double P2[2]) */
{ /* squared distance (more efficient) */
    double dx=P2[_x_]-P1[_x_], dy=P2[_y_]-P1[_y_];
    return(dx*dx+dy*dy);
}
/* normal distance of a vertex point (Xv,Yv) to an infinte line <X1Y1-X2Y2> */
double PntLineDist(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{
    double X2_X1=X2-X1, Y2_Y1=Y2-Y1, Xv_X1=Xv-X1, Yv_Y1=Yv-Y1;
    /* if (PntDist(X1,Y1,X2,Y2)==0) return(PntDist(X1,Y1,Xv,Yv)); */
    /* else return(TriArea2(X1,Y1,X2,Y2,Xv,Yv)/PntDist(X1,Y1,X2,Y2)); */
    if ( (X1==X2) && (Y1==Y2) ) return(hypot(Xv_X1,Yv_Y1));
    else return(fabs(X2_X1*Yv_Y1-Xv_X1*Y2_Y1)/(hypot(X2_X1,Y2_Y1)));
}
/* minimum distance (may not normal) of (Xv,Yv) to line segment [X1Y1-X2Y2] */
double PntLsegDist(double X1, double Y1, double X2, double Y2, double Xv, double Yv)
{ /* REF.: IntxPntLine() */
    double X2_X1=X2-X1, Y2_Y1=Y2-Y1, Xv_X1=Xv-X1, Yv_Y1=Yv-Y1;
    double u, D=hypot(X2_X1,Y2_Y1);
    /* http://paulbourke.net/geometry/pointline/ */
    if (D==0) return(hypot(Xv_X1,Yv_Y1));
    u = ( Xv_X1*X2_X1 + Yv_Y1*Y2_Y1 ) / ( D*D );
    /* 0<=u<=1: normal intersection inside the line segment */ 
    if( (u<0) || (u>1) ) return( mini(hypot(Xv_X1,Yv_Y1),hypot(Xv-X2,Yv-Y2)) );
    else return(fabs(X2_X1*Yv_Y1-Xv_X1*Y2_Y1)/D); /* PntLineDist() */
}

/* http://en.wikipedia.org/wiki/Atan2 */
/* obtain angle [0,2*pi] of two points from start (X1,Y1) to end (X2,Y2); */
double PntAng(double X1, double Y1, double X2, double Y2)
{
    double ang=atan2(Y2-Y1,X2-X1); /* atan2 [-pi,pi]; atan [-pi/2,pi/2]; */
    if (ang<0) ang += PI2; /* ang=ang+PI2; */
    return(ang); /* [0,2*pi] */
}
double PNTANG(tXY P1, tXY P2) /* PNTANG(double P1[2], double P2[2]) */
{
    double ang=atan2(P2[_y_]-P1[_y_],P2[_x_]-P1[_x_]);
    if (ang<0) ang += PI2; /* ang=ang+PI2; */
    return(ang); /* [0,2*pi] */
}
double LINEANG(tLineXY L) /* LINEANG(double L[2][2]) */
{
    double ang=atan2(L[1][_y_]-L[0][_y_],L[1][_x_]-L[0][_x_]);
    if (ang<0) ang += PI2; /* ang=ang+PI2; */
    return(ang); /* [0,2*pi] */
}
/* obtain span angle (range in (0, 2*pi]) of two arc angles */
double ArcSpan(double angH, double angL) /* start (angL); end (angH); */
{ /* fqdrim.m v1.50 fangspan() */
    double angspan=mod(angH-angL,PI2); /* Low to High: counterclockwise */
    if (angspan==0) angspan=PI2; /* range in (0, 2*pi]; */
    return(angspan);
}
/* judge which Quadrant (I,II,III,IV) the angle is in */
unsigned int QDI(double ang)
{ /* return Quadrant number (1,2,3,4); fqdrim.m fqdi(); set angle to [0,2*pi) */
    return((unsigned int)ceil(mod(ang,PI2)*InvPI2)); /* 1/PI_2 = 1*InvPI2 */
}
/* judge if an angle is at Canonical Axis (+x, +y, -x, -y) */
bool isAtAxis(double ang)
{ /* calling syntax: isAtAxis(ang); allow +-EPS angular tolerance; */
    ang=mod(ang,PI2); /* set angle to [0,2*pi) */
    return( (fabs(ang)<EPS) || (fabs(ang-PI_2)<EPS) ||(fabs(ang-PI)<EPS) \
           || (fabs(ang-PI-PI_2)<EPS) || (fabs(ang-PI2)<EPS) );
}
/* tell an angle is at which Axis (0: OFF-axis, 1: +x, 2: +y, 3:-x, 4:-y) */
unsigned int atAxis(double ang)
{
    return( isAtAxis(ang)? QDI(ang+EPS) : 0 ); /* +EPS for angular tolerance */
}

/* tranform polar coordinate (R,ang) to orthogonal coordinate (X,Y) */
void Polar2XY(double *X, double *Y, double OX, double OY, double R, double ang)
{ /* calling syntax: Polar2XY(&X,&Y,OX,OY,R,ang); */
    /* double cosang, sinang; exp1j(&cosang,&sinang,ang); */
    *X=OX+R*cos(ang);
    *Y=OY+R*sin(ang);
    return;
}
/* transform orthogonal coordinate (X,Y) to polar coordinate (R,ang) */
void XY2Polar(double *R, double *ang, double OX, double OY, double X, double Y)
{ /* calling syntax: XY2Polar(&R,&ang,OX,OY,X,Y); */
    *R=hypot(X-OX,Y-OY); /* PntDist(OX,OY,X,Y); */
    *ang=PntAng(OX,OY,X,Y); /* [0,2*pi] */
    return;
}

/* Line-Line: 0-NO intersection/1-SINGLE intersection/2-MULTIPLE intersections*/
/* intersection Xv if horizontal sweep line Yv cuts infinite line <X1Y1-X2Y2> */
int findLineX(double *Xv, double Yv, double X1, double Y1, double X2, double Y2)
{ /* calling syntax: N=findLineX(&Xv,Yv,X1,Y1,X2,Y2); */
    if (Y2==Y1) { /* special and degenerated cases */
        if (Yv==Y1) { /* when Y1==Y2==Yv, set Xv in the middle; */
            *Xv=0.5*(X1+X2); /* 1: degenerated case: input coincidence points */
            return( (X1==X2)? 1 : 2 ); /* 2: MULTIPLE intersections */
        } else { Xv=NULL; return(0); } /* NULL; *Xv=NAN; NO intersection */
    } else { *Xv=X1+(Yv-Y1)/(Y2-Y1)*(X2-X1); return(1); } /* ONE intersection */
}
/* intersection Xv if horizontal sweep line Yv cuts line segment [X1Y1-X2Y2] */
int findLsegX(double *Xv, double Yv, double X1, double Y1, double X2, double Y2)
{ /* calling syntax: N=findLsegX(&Xv,Yv,X1,Y1,X2,Y2); */
    if (isOn(Y1,Y2,Yv)) { /* must between the range */
        if (Y1==Y2) { /* when Y1==Y2==Yv, set Xv in the middle; */
            *Xv=0.5*(X1+X2); /* 1: degenerated case: input coincidence points */
            return( (X1==X2)? 1 : 2 ); /* 2: MULTIPLE intersections */
        } else { *Xv=X1+(Yv-Y1)/(Y2-Y1)*(X2-X1); return(1); } /* 1 intersection */
    } else { Xv=NULL; return(0); } /* NULL; *Xv=NAN; NO intersection */
}
/* intersection Yv when vertical sweep line Xv cuts infinite line <X1Y1-X2Y2> */
int findLineY(double Xv, double *Yv, double X1, double Y1, double X2, double Y2)
{ /* calling syntax: N=findLineY(Xv,&Yv,X1,Y1,X2,Y2); */
    if (X2==X1) { /* special and degenerated cases */
        if (Xv==X1) { /* when X1==X2==Xv, set Yv in the middle; */
            *Yv=0.5*(Y1+Y2); /* 1: degenerated case: input coincidence points */
            return( (Y1==Y2)? 1 : 2 ); /* 2: MULTIPLE intersections */
        } else { Yv=NULL; return(0); } /* NULL; *Yv=NAN; NO intersection */
    } else { *Yv=Y1+(Y2-Y1)/(X2-X1)*(Xv-X1); return(1); } /* ONE intersection */
}
/* intersection Yv if vertical sweep line Xv cuts line segment [X1Y1-X2Y2] */
int findLsegY(double Xv, double *Yv, double X1, double Y1, double X2, double Y2)
{ /* calling syntax: N=findLsegY(Xv,&Yv,X1,Y1,X2,Y2); */
    if (isOn(X1,X2,Xv)) { /* must between the range */
        if (X1==X2) { /* when X1==X2==Xv, set Yv in the middle; */
            *Yv=0.5*(Y1+Y2); /* 1: degenerated case: input coincidence points */
            return( (Y1==Y2)? 1 : 2 ); /* 2: MULTIPLE intersections */
        } else { *Yv=Y1+(Y2-Y1)/(X2-X1)*(Xv-X1); return(1); } /* 1 intersection */
    } else { Yv=NULL; return(0); } /* NULL; *Yv=NAN; NO intersection */
}

/* find normal intersection (X,Y) from (Xv,Yv) to infinite line <X1Y1-X2Y2> */
/* REF.: PntLsegDist(); /* http://paulbourke.net/geometry/pointline/ */
bool IntxPntLine(double *X, double *Y, double X1, double Y1,
                 double X2, double Y2, double Xv, double Yv)
{
    double X2_X1=X2-X1, Y2_Y1=Y2-Y1;
    double u, D=hypot(X2_X1,Y2_Y1);
    if (D==0) { *X=X1; *Y=Y1; return(false); }
    u = ( (Xv-X1)*X2_X1 + (Yv-Y1)*Y2_Y1 ) / ( D*D );
    *X = X1+u*X2_X1;
    *Y = Y1+u*Y2_Y1;
    return(true);
}
/* find normal intersection (X,Y) from (Xv,Yv) to line segment [X1Y1-X2Y2] */
bool IntxPntLseg(double *X, double *Y, double X1, double Y1,
                 double X2, double Y2, double Xv, double Yv)
{
    IntxPntLine(X,Y,X1,Y1,X2,Y2,Xv,Yv); /* X,Y input as pointers */
    return ( isOnLseg(X1,Y1,X2,Y2,*X,*Y) );
}
/* (REF.: fllx.m) http://en.wikipedia.org/wiki/Line-line_intersection */
/* iff infinte lines <X1Y1-X2Y2>&<X3Y3-X4Y4> parallel (OR coincident/overlap) */
bool isParaLine(double X1, double Y1, double X2, double Y2,
                double X3, double Y3, double X4, double Y4)
{ /* calling syntax: isParaLine(X1,Y1,X2,Y2,X3,Y3,X4,Y4); */
    /* Parellel: Cross-Product=0; xprod2(Vx1,Vy1,Vx2,Vy2)=Vx1*Vy2-Vy1*Vy2; */
    return( ((X1-X2)*(Y3-Y4)-(Y1-Y2)*(X3-X4))==0 );
}
/* iff infinte lines <X1Y1-X2Y2> & <X3Y3-X4Y4> intersect (NOT parallel) */
bool isIntxLineLine(double X1, double Y1, double X2, double Y2,
                    double X3, double Y3, double X4, double Y4)
{ /* calling syntax: isIntxLineLine(X1,Y1,X2,Y2,X3,Y3,X4,Y4); */
    /* Parellel: Cross-Product=0; xprod2(Vx1,Vy1,Vx2,Vy2)=Vx1*Vy2-Vy1*Vy2; */
    return( ((X1-X2)*(Y3-Y4)-(Y1-Y2)*(X3-X4))!=0 );
}
/* find intersection (X,Y) of two infinite lines <X1Y1-X2Y2> & <X3Y3-X4Y4> */
int IntxLineLine(double *X, double *Y,
                 double X1, double Y1, double X2, double Y2,
                 double X3, double Y3, double X4, double Y4)
{ /* calling syntax: IntxLineLine(&X,&Y,X1,Y1,X2,Y2,X3,Y3,X4,Y4); */
    double X1_X2=X1-X2, Y1_Y2=Y1-Y2, X3_X4=X3-X4, Y3_Y4=Y3-Y4;
    double X1Y2_Y1X2=X1*Y2-Y1*X2, X3Y4_Y3X4=X3*Y4-Y3*X4;
    double Denom=X1_X2*Y3_Y4-Y1_Y2*X3_X4; /* denominator xprod2(); */
    if (Denom==0.0) { /* parallel OR coincident/overlap/collinear (isParaLine) */
        if (isCollinear(X1,Y1,X2,Y2,X3,Y3)) {
            *X=(X1+X2+X3+X4)/4; *Y=(Y1+Y2+Y3+Y4)/4; /* set (X,Y) in middle */
            return(2); /* coincident: MULTIPLE intersections */
        } else { X=NULL; Y=NULL; return(0); }  /* parallel: NO intersection */
    }
    *X=(X1Y2_Y1X2*X3_X4-X1_X2*X3Y4_Y3X4)/Denom;
    *Y=(X1Y2_Y1X2*Y3_Y4-Y1_Y2*X3Y4_Y3X4)/Denom;
    return(1); /* ONE intersection */
}
/* check if two line segments [X1Y1-X2Y2] & [X3Y3-X4Y4] intersect each other */
/* O'Rourke1998 pp.30 Code 1.7-1.9 IntersectProp(); Between(); Intersect(); */
/* Proper Intersection (Segment unparallel, NO 3 vertices collinear) */
bool isIntxLsegLsegProp(double X1, double Y1, double X2, double Y2,\
                        double X3, double Y3, double X4, double Y4)
{ /* calling syntax: isIntxLsegLsegProp(X1,Y1,X2,Y2,X3,Y3,X4,Y4); */
    /* Segment_Segment_Intersection_UIUC_CS373_2002.pdf; */
    return ( (isLeft(X1,Y1,X3,Y3,X4,Y4)!=isLeft(X2,Y2,X3,Y3,X4,Y4)) \
            ^(isLeft(X1,Y1,X2,Y2,X3,Y3)!=isLeft(X1,Y1,X2,Y2,X4,Y4)) );
}
/* Proper Intersection and Degenerated Intersections */
bool isIntxLsegLseg(double X1, double Y1, double X2, double Y2,\
                    double X3, double Y3, double X4, double Y4)
{ /* calling syntax: isIntxLsegLseg(X1,Y1,X2,Y2,X3,Y3,X4,Y4); */
    if (isIntxLsegLsegProp(X1,Y1,X2,Y2,X3,Y3,X4,Y4)) return(true);
    else return( isOnLseg(X1,Y1,X2,Y2,X3,Y3) || isOnLseg(X1,Y1,X2,Y2,X4,Y4) \
              || isOnLseg(X3,Y3,X4,Y4,X1,Y1) || isOnLseg(X3,Y3,X4,Y4,X2,Y2));
}
/* obtain intersection (X,Y) of two line segments [X1Y1-X2Y2] & [X3Y3-X4Y4] */
bool IntxLsegLseg(double *X, double *Y,
                  double X1, double Y1, double X2, double Y2,
                  double X3, double Y3, double X4, double Y4)
{ /* calling syntax: IntxLsegLseg(&X,&Y,X1,Y1,X2,Y2,X3,Y3,X4,Y4); */
    if (IntxLineLine(X,Y,X1,Y1,X2,Y2,X3,Y3,X4,Y4)==1) /* careful: no overlap */
        return( isOnLseg(X1,Y1,X2,Y2,*X,*Y) && isOnLseg(X3,Y3,X4,Y4,*X,*Y) );
    else return(false);
}

/* find intersections of two circles (O1-R1 & O2-R2) (REF.: fOOx.m) */
/* http://mathworld.wolfram.com/Circle-CircleIntersection.html */
/* http://local.wasp.uwa.edu.au/~pbourke/geometry/2circle/ */
int IntxCircCirc(double *X, double *Y,
                 double OX1, double OY1, double R1,
                 double OX2, double OY2, double R2)
{ /* calling syntax: IntxCircCirc(X,Y,OX1,OY1,R1,OX2,OY2,R2); X,Y as pointers */
    double dOx=OX2-OX1, dOy=OY2-OY1, dO=hypot(dOx,dOy), invdO, dOx_dO, dOy_dO;
    double R1nR2=R1+R2, R1_R2=R1-R2, dR=fabs(R1_R2);
    double Xv, Yv, a, h, sx, sy;

    if ( (dO>R1nR2) || (dO<dR) || (dO==0.0) ) {
         X=NULL; Y=NULL; return(0); /* NULL pointer */
    } /* NO solution if seperated or contained, Inf solution if coincident */

    invdO=1.0/dO; dOx_dO=dOx*invdO; dOy_dO=dOy*invdO;
    /* let d=a+b and height=h, so that a^2+h^2=R1^2; b^2+h^2=R2^2; */
    a=0.5*(R1nR2*R1_R2+dO*dO)*invdO; /* 0.5*(R1*R1-R2*R2+dO*dO)*invdO; */
    h=sqrt((R1+a)*(R1-a)); /* sqrt(R1*R1-a*a); */
    /* (Xv,Yv) is the point where the line between circle intersection points
     * crosses the line between circle centers. */
    Xv=OX1+a*dOx_dO; Yv=OY1+a*dOy_dO;
    /* offsets of the intersection points from (Xv,Yv) */
    sx=h*dOy_dO; sy=h*dOx_dO;
    /* absolute intersection points */
    if ( (sx==0.0) && (sy==0.0) ) { /* one intersection (two circles tangent) */
        X[0]=Xv; Y[0]=Yv;
        return(1);
    } else { /* two intersections */
        X[0]=Xv-sx; Y[0]=Yv+sy;
        X[1]=Xv+sx; Y[1]=Yv-sy;
        return(2);
    } /* \codelib\newlib\draft\COM_C\test\testIntsectCircCirc.c */
} /* http://paulbourke.net/geometry/2circle/tvoght.c */

/* find intersections of a circle (O1-R1) and a line (X1Y1-X2Y2)(REF.: fOIx.m) */
/* (situation different for infinite or segment line) */
/* http://mathworld.wolfram.com/Circle-LineIntersection.html */
int IntxCircLine(double *X, double *Y,
                 double OX, double OY, double R,
                 double X1, double Y1, double X2, double Y2)
{ /* calling syntax: IntxCircLine(X,Y,OX,OY,R,X1,Y1,X2,Y2); X,Y as pointers */
    double dx=X2-X1, dy=Y2-Y1, L=hypot(dx,dy), LL=L*L, RR=R*R;
    double D=(X1-OX)*(Y2-OY)-(X2-OX)*(Y1-OY), Delta=RR*LL-D*D;
    double sqrtDelta=sqrt(Delta), invLL=1/LL, sgndy, Ddx=D*dx, Ddy=D*dy, tx, ty;
    int N=0, signDelta=sign(Delta);

    /* normal distance of circle origin to the infinite line */
    /* H=PntLineDist(X1,Y1,X2,Y2,OX,OY);
    /* if (H>R) { X=NULL; Y=NULL; return(false); } /* NULL pointer */
    switch (signDelta) {
        case -1: /* no intersection */
            X=NULL; Y=NULL; N=0; /* NULL pointer */
            break;
        case 0: /* one intersection (tangent) */
            X[0]=OX+Ddy*invLL;
            Y[0]=OY-Ddx*invLL;
            N=1;
            break;
        case 1: /* two intersections */
            sgndy = (dy<0)? -1: 1; /* sign2(dy); NOT sign(dy) where sign(0)=0;*/
            sqrtDelta=sqrt(Delta);
            tx=sgndy*dx*sqrtDelta;
            ty=sgndy*dy*sqrtDelta; /* fabs(dy)*sqrtDelta; */
            X[0]=OX+(Ddy+tx)*invLL;
            Y[0]=OY-(Ddx-ty)*invLL;
            X[1]=OX+(Ddy-tx)*invLL;
            Y[1]=OY-(Ddx+ty)*invLL;
            N=2;
            break;
    }
    return(N);
}
int IntxCircLseg(double *X, double *Y,
                 double OX, double OY, double R,
                 double X1, double Y1, double X2, double Y2)
{ /* calling syntax: IntxCircLseg(X,Y,OX,OY,R,X1,Y1,X2,Y2); X,Y as pointers */
    int N=IntxCircLine(X,Y,OX,OY,R,X1,Y1,X2,Y2);

    if (N==0) return(0);
    if (N==2) { if (!isOnLseg(X1,Y1,X2,Y2,X[1],Y[1])) N--; }
    if (!isOnLseg(X1,Y1,X2,Y2,X[0],Y[0])) N--;
    return(N);
}
/* ========================================================================== */

/* ########################################################################## */
/* functions for q1rim.c, qdrim.c, qdcvx.c, icedpoly., acadpoly.c, etc. */
/* ########################################################################## */
/* ========================================================================== */
/* align coordinates to minimum resolution GRID; align1/2/3(): 1D/2D/3D; */
void align1(double *X, unsigned int N, double grid)
{ /* calling syntax: align1(X,N,grid); or align1(&X[0],N,grid); */
    unsigned int i;
    double invgrid;
    if ( (grid==0.0) || (N<1) ) return;
    invgrid = (grid==0.001)? 1000.0: 1.0/grid; /* optimize default resolution */
    for (i=0;i<N;i++) X[i]=round(X[i]*invgrid)*grid;
    return;
}
void align2(double *X, double *Y, unsigned int N, double grid)
{ /* calling syntax: align2(X,Y,N,grid); or align2(&X[0],&Y[0],N,grid); */
    unsigned int i;
    double invgrid;
    if ( (grid==0.0) || (N<1) ) return;
    invgrid = (grid==0.001)? 1000.0: 1.0/grid; /* optimize default resolution */
    for (i=0;i<N;i++) {
        X[i]=round(X[i]*invgrid)*grid;
        Y[i]=round(Y[i]*invgrid)*grid;
    }
    return;
}
void align3(double *X, double *Y, double *Z, unsigned int N, double grid)
{ /* calling syntax: align3(X,Y,Z,N,grid); align3(&X[0],&Y[0],&Z[0],N,grid); */
    unsigned int i;
    double invgrid;
    if ( (grid==0.0) || (N<1) ) return;
    invgrid = (grid==0.001)? 1000.0: 1.0/grid; /* optimize default resolution */
    for (i=0;i<N;i++) {
        X[i]=round(X[i]*invgrid)*grid;
        Y[i]=round(Y[i]*invgrid)*grid;
        Z[i]=round(Z[i]*invgrid)*grid;
    }
    return;
}
/* align to resolution GRID with distance statistics; 1/2/3stat(): 1D/2D/3D; */
void align1stat(double *X, unsigned int N, double grid, double *D)
{ /* calling syntax: align1stat(X,N,grid,D); or align1stat(&X[0],N,grid,D); */
    unsigned int i;
    double invgrid, Xt;
    if ( (grid==0.0) || (N<1) ) return;
    invgrid = (grid==0.001)? 1000.0: 1.0/grid; /* optimize default resolution */
    for (i=0;i<N;i++) { Xt=X[i]; X[i]=round(Xt*invgrid)*grid; D[i]=abs(X[i]-Xt); }
    return;
}
void align2stat(double *X, double *Y, unsigned int N, double grid, double *D)
{ /* align2stat(X,Y,N,grid,D); align2stat(&X[0],&Y[0],N,grid,D); */
    unsigned int i;
    double invgrid, Xt, Yt;
    if ( (grid==0.0) || (N<1) ) return;
    invgrid = (grid==0.001)? 1000.0: 1.0/grid; /* optimize default resolution */
    for (i=0;i<N;i++) {
        Xt=X[i]; X[i]=round(Xt*invgrid)*grid;
        Yt=Y[i]; Y[i]=round(Yt*invgrid)*grid;
        D[i]=hypot(X[i]-Xt,Y[i]-Yt);
    }
    return;
}
void align3stat(double *X, double *Y, double *Z, unsigned int N,\
                double grid, double *D)
{ /* align3stat(X,Y,Z,N,grid,D); align3stat(&X[0],&Y[0],&Z[0],N,grid,D); */
    unsigned int i;
    double invgrid, Xt, Yt, Zt;
    if ( (grid==0.0) || (N<1) ) return;
    invgrid = (grid==0.001)? 1000.0: 1.0/grid; /* optimize default resolution */
    for (i=0;i<N;i++) {
        Xt=X[i]; X[i]=round(Xt*invgrid)*grid;
        Yt=Y[i]; Y[i]=round(Yt*invgrid)*grid;
        Zt=Z[i]; Z[i]=round(Zt*invgrid)*grid;
        D[i]=sqrt(sqsum3(X[i]-Xt,Y[i]-Yt,Z[i]-Zt));
    }
    return;
}
/* -------------------------------------------------------------------------- */
/* REMOVE collinear (X,Y) coordinates: delete redundant (repeat & collinear). */
/* Old version judge TriArea(P1,P2,P3)<S0 ( GRID resolution grid (default 1nm),
 * distinguishable triangle minimum area S0=0.5*(GRID/2)^2=GRID*GRID*0.125 );
 * HOWEVER, NEW version judge TriArea(P1,P2,P3)<EPS for IMPROVED resolution; */
/* (REF.: commath.h norepeat() & norep(), icedpoly.c v9.00) */
/* nocollinear()/nocolinear(): with/WITHOUT malloc(); nocolinear() PREFERRED. */
void nocollinear(double *PX, double *PY, unsigned int *PN, double Seps)
{ /* ( 20120417/201106 with malloc()&memcpy() ); calling syntax:
   * nocollinear(PX,PY,&PN,EPS); nocollinear(&PX[0],&PY[0],&PN,EPS); */
    unsigned int NBSZ=1024; /* memory block buffer size; stdio.h BUFSIZ; */
    unsigned int N=*PN, nps=0, i0=0, i1=1, i2=2; /* INITIALIZATION */
    double *GX=NULL, *GY=NULL;

    /* N=*PN; /* or N=PN[0]; */
    if (N<3) { /* valid polygon minimum 3 points (ALGORITHM TESTED 20110601) */
        if ( (N==2) && (PX[1]==PX[0]) && (PY[1]==PY[0]) ) N--; /* REDUNDANT */
        *PN=N; return; /* or PN[0]=N; */
    }

    /* allocate dynamic memory: (mx)malloc(N*size); (mx)calloc(N,size); FREE(); */
    /* Matlab mex file CRASH if malloc() return NULL; AVOID use in SUBfunctions;
     * 20120507 for better management <#define malloc mxMalloc> in comcnst.h; */
    NBSZ = N*sizeof(double); /* calloc() or mxCalloc(N, sizeof(double)); */
    GX = malloc(NBSZ); GY = malloc(NBSZ);
    if ((GX==NULL)||(GY==NULL)) MsgErr("comgeom.h nocollinear() OUT OF MEMORY!\n");
    /* FIRST point */
    /* i0=0; /* adjacent points (i0,i1,i2) */
    GX[nps]=PX[i0]; GY[nps++]=PY[i0];
    /* loop INNER points (check repetition and collinearity) */
    /* i1=i0+1; /* i0=1; */
    while ( (PX[i1]==PX[i0]) && (PY[i1]==PY[i0]) && (i1<N-1) ) i1++;
    i2=i1+1;
    while (i2<N) {
        while ( (TriArea(PX[i0],PY[i0],PX[i1],PY[i1],PX[i2],PY[i2])<Seps)
                 && (i2<N-1) ) { /* ignore repeat and collinear */
            /* i1=i2++; /* i1=i2; i2++; */
            if ( (PX[i2]!=PX[i0])||(PY[i2]!=PY[i0]) ) i1=i2; /* NOT REDUNDANT */
            i2++;
        }
        GX[nps]=PX[i1]; GY[nps++]=PY[i1];
        i0=i1; i1=i2++; /* i0=i1; i1=i2; i2++; */
    } /* after "while (i2<N) loop": i2=N; the LAST point not saved yet; */
    /* LAST (final) vertex (point) */
    i2=N-1; /* or i2=i1; or i2--; after previous loop */
    if (nps>1) { /* nps>=2; (nps-2)>=0; */
        i1=nps-1; i0=nps-2;
        if (TriArea(GX[i0],GY[i0],GX[i1],GY[i1],PX[i2],PY[i2])>=Seps) {
            GX[nps]=PX[i2]; GY[nps++]=PY[i2]; /* ADD the LAST vertex */
        } else {
            if ( (PX[i2]!=GX[i0])||(PY[i2]!=GY[i0]) ) /* NOT REDUNDANT */
                { GX[i1]=PX[i2]; GY[i1]=PY[i2]; } /* RENEW with LAST */
        }
    } else { /* else if (nps==1) { /* (nps-1)==0 */
        if ( (PX[i2]!=GX[0]) || (PY[i2]!=GY[0]) ) {
            GX[nps]=PX[i2]; GY[nps++]=PY[i2]; /* ADD the LAST vertex */
        }
    }
    NBSZ = nps*sizeof(double);
    memcpy(PX, GX, NBSZ); memcpy(PY, GY, NBSZ);
    free(GX); free(GY); GX=GY=NULL;

    *PN=nps; /* or PN[0]=nps; */
    return;
}
void nocolinear(double *PX, double *PY, unsigned int *PN, double Seps)
{ /* 20120417/20110713 IMPROVED nocollinear(); NO malloc(); testnocolinear4.c */
/* calling syntax: nocolinear(PX,PY,&PN,EPS); nocolinear(&PX[0],&PY[0],&PN,EPS); */
    unsigned int N=*PN, nps=1, i0=0, i1=1, i2=2; /* INITIALIZATION */

    /* N=*PN; /* or N=PN[0]; */
    if (N<3) { /* valid polygon minimum 3 points */
        if ( (N==2) && (PX[1]==PX[0]) && (PY[1]==PY[0]) ) N--; /* REDUNDANT */
        *PN=N; return; /* or PN[0]=N; */
    }

    /* FIRST point */
    /* PX[0]=PX[0]; PY[0]=PY[0]; nps=1; */
    /* SECOND point: different than FIRST ( or PntDist()<EPS ) */
    while ( (PX[i1]==PX[i0]) && (PY[i1]==PY[i0]) && (i1<N-1) ) i1++;
    PX[nps]=PX[i1]; PY[nps++]=PY[i1]; /* either Non-Repeated or FINAL */
    /* loop INNER points (check repetition and collinearity) */
    i2=i1+1;
    while (i2<N) {
        i1=nps-1; i0=nps-2; /* with VERIFIED points */
        while ( (TriArea(PX[i0],PY[i0],PX[i1],PY[i1],PX[i2],PY[i2])<Seps)
                 && (i2<N-1) ) {
            if ( (PX[i2]!=PX[i0]) || (PY[i2]!=PY[i0]) ) /* when NOT REDUNDANT */
                { PX[i1]=PX[i2]; PY[i1]=PY[i2]; } /* UPDATE with RECENT */
            i2++; /* check the next point; DO NOT alter i0,i1 value; */
        } /* all points VERIFIED except the final point */
        PX[nps]=PX[i2]; PY[nps++]=PY[i2]; /* ADD this noncollinear point */
        i2++; /* loop to the next; final point saved (but NOT verified); */
    }
    /* LAST (final) vertex (point): no redundant (NOT verified in the loop) */
    switch (nps) {
        case 0: break;
        case 1: break;
        case 2:
            i1=nps-1; i0=nps-2; /* or judge PntDist()<EPS */
            if ( (PX[i1]==PX[i0]) && (PY[i1]==PY[i0]) ) nps--; /* COUNTER-1 */
            break;
        default: /* nps>2 */
            i2=nps-1; i1=nps-2; i0=nps-3; /* to verify the final 3 points */
            if (TriArea(PX[i0],PY[i0],PX[i1],PY[i1],PX[i2],PY[i2])<Seps) {
                if ( (PX[i2]!=PX[i0]) || (PY[i2]!=PY[i0]) ) /* NOT REDUNDANT */
                    { PX[i1]=PX[i2]; PY[i1]=PY[i2]; } /* RENEW with LAST */
                nps--; /* COUNTER-1 */
                if ((PX[i1]==PX[i0])&&(PY[i1]==PY[i0])) nps--; /* REDUNDANT */
            }
            break;
    }
    /* set ZEROS to the remaining storage (not mandatory, but recommended) */
    /* for (i2=nps; i2<N; i2++) { PX[i2] = 0; PY[i2] = 0; } */

    *PN=nps; /* or PN[0]=nps; */
    return;
}

/* -------------------------------------------------------------------------- */
/* Notes20120217DHH get ellipse parameter (a,b) from points (x1,y1) & (x2,y2) */
/* In canonical position (center origin (OX,OY), major axis at X (angaxis=0)),
 * ellipse in Canonical Form xx/aa+yy/bb=1; with xx=x*x; yy=y*y; aa=a*a; bb=b*b;
 * ellipse in polar coordinate: x(t)=a*cos(t), y(t)=b*sin(t), t in [0,2*pi);
 * eccentric anomaly t is NOT angle(ang) of (X(t),Y(t)) to (OX,OY) with X-axis;
 * where tan(ang)=y(t)/x(t)=(b/a)*tan(t); (http://en.wikipedia.org/wiki/Ellipse)
 * and r=sqrt(xx+yy)=(a*b)/sqrt(aa*sin(ang)*sin(ang)+bb*cos(ang)*cos(ang)); */
/* (OX,OY) ellipse origin center; angaxis: global angle of ellipse axis;
 * ang: global angle of point to (OX,OY); (a,b) ellipse (major,minor) axis; */

/* get ellipse parameter for canonical (on-axis) position (local coordinate) */
bool ellipab0(double *a, double *b, double x1, double y1, double x2, double y2)
{ /* calling syntax: ellipab0(&a,&b,x1,y1,x2,y2); center (0,0); on-axis(a,b); */
    double R1=hypot(x1,y1), R2=hypot(x2,y2);
    double xx1=x1*x1, yy1=y1*y1, xx2=x2*x2, yy2=y2*y2;
    if (fabs(R1-R2)<EPS) { a[0]=b[0]=sqrt(R1*R2); return(true); } /* circle */
    if ((xx1==xx2)||(yy1==yy2)) { a=b=NULL; return(false); }
    *a = sqrt(xx1+yy1*(xx1-xx2)/(yy2-yy1));
    *b = sqrt(yy1+xx1*(yy2-yy1)/(xx1-xx2));
    return(true);
}
/* get ellipse parameter for canonical (on-axis) position (Global Coordinate) */
bool Ellipab0(double *a, double *b,\
              double OX, double OY, double X1, double Y1, double X2, double Y2)
{ /* syntax: Ellipab0(&a,&b,OX,OY,X1,Y1,X2,Y2); center (OX,OY); on-axis(a,b); */
    return(ellipab0(a,b,X1-OX,Y1-OY,X2-OX,Y2-OY));
}
/* get ellipse parameter (a,b) for off-axis position (local coordinate) */
bool ellipab(double *a, double *b, double angaxis,\
             double x1, double y1, double x2, double y2)
{ /* syntax: ellipab(&a,&b,angaxis,x1,y1,x2,y2); center (0,0); off-axis (a,b) */
    double x01, y01, x02, y02;
    rotxy0(&x01,&y01,x1,y1,-angaxis); /* commath.h rotxy0() clockwise (-) */
    rotxy0(&x02,&y02,x2,y2,-angaxis); /* rotate to on-axis ellipse */
    return(ellipab0(a,b,x01,y01,x02,y02));
}
/* get ellipse parameter (a,b) for off-axis position (Global Coordinate) */
bool Ellipab(double *a, double *b, double angaxis,\
             double OX, double OY, double X1, double Y1, double X2, double Y2)
{ /* ellipab(&a,&b,angaxis,OX,OY,X1,Y1,X2,Y2); center (OX,OY); off-axis (a,b) */
    return(ellipab(a,b,angaxis,X1-OX,Y1-OY,X2-OX,Y2-OY));
}

/* get ellipse radius at an angle for canonical position */
double ellipr0(double ang, double a, double b)
{ /* calling syntax: r=ellipr0(ang,a,b); on-axis(a,b); */
    return((a*b)/hypot(a*sin(ang),b*cos(ang)));
}
/* get ellipse radius at an angle for off-axis(a,b) */
double ellipr(double angaxis, double ang, double a, double b)
{ /* calling syntax: r=ellipr(angaxis,ang,a,b); off-axis(a,b); */
    return((a*b)/hypot(a*sin(ang-angaxis),b*cos(ang-angaxis)));
}

/* Ramanujan's ellipse circumference; http://en.wikipedia.org/wiki/Ellipse */
double ElipRama(double a, double b)
{ /* calling syntax: L=ElipRama(a,b); */
    double a3=a+a+a, b3=b+b+b;
    return(PI*((a3+b3)-sqrt((a3+b)*(a+b3))));
}
/* better approx ellipse circumference; http://en.wikipedia.org/wiki/Ellipse */
double ElipPeri(double a, double b)
{ /* calling syntax: L=ElipPeri(a,b); */
    double m=(a-b)/(a+b), m3=3*m*m;
    return(PI*(a+b)*(1+m3/(10+sqrt(4-m3))));
} /* ellipse area see ElipArea() */

/* get ellipse point at an angle for canonical position (local coordinate) */
void ellipxy0(double *x, double *y, double ang, double a, double b)
{ /* calling syntax: ellipxy0(&x,&y,ang,a,b); center (0,0); on-axis(a,b); */
    double sinang=sin(ang), cosang=cos(ang), r=(a*b)/hypot(a*sinang,b*cosang);
    *x=r*cosang;
    *y=r*sinang;
    return;
}
/* get ellipse point at an angle for canonical position (Global Coordinate) */
void EllipXY0(double *X, double *Y,\
              double OX, double OY, double ang, double a, double b)
{ /* syntax: EllipXY0(&X,&Y,OX,OY,ang,a,b); center (OX,OY); on-axis(a,b); */
    double sinang=sin(ang), cosang=cos(ang), r=(a*b)/hypot(a*sinang,b*cosang);
    *X=OX+r*cosang;
    *Y=OY+r*sinang;
    return;
}
/* get ellipse point at an angle for off-axis(a,b) (local coordinate) */
void ellipxy(double *x, double *y, double angaxis, double ang, double a, double b)
{ /* syntax: ellipxy(&x,&y,angaxis,ang,a,b); center (0,0); off-axis(a,b); */
    double x0, y0;
    ellipxy0(&x0,&y0,ang-angaxis,a,b); /* calculate on-axis ellipse */
    rotxy0(x,y,x0,y0,angaxis); /* rotate to desired off-axis ellipse */
    return;
}
/* get ellipse point at an angle for off-axis(a,b) (Global Coordinate) */
void EllipXY(double *X, double *Y,\
          double OX, double OY, double angaxis, double ang, double a, double b)
{ /* calling syntax: ellipXY(&X,&Y,OX,OY,angaxis,ang,a,b); */
    ellipxy(X,Y,angaxis,ang,a,b); /* local */
    X[0] += OX; Y[0] += OY; /* Global */
    return;
}

/* get canonical ellipse coordinate x in Quadrant I from y (local coordinate) */
double ellipQ1x(double a, double b, double y)
{ /* calling syntax: x=ellipQ1x(a,b,y); */
    return( (fabs(y)>fabs(b))? 0.0: a*sqrt(1-(y*y)/(b*b)) );
}
/* get canonical ellipse coordinate y in Quadrant I from x (local coordinate) */
double ellipQ1y(double a, double b, double x)
{ /* calling syntax: y=ellipQ1y(a,b,x); */
    return( (fabs(x)>fabs(a))? 0.0: b*sqrt(1-(x*x)/(a*a)) );
}

/* -------------------------------------------------------------------------- */
/* find local coordinate of an arc; R=hypot(x,y)=sqrt(x*x+y*y); tan(theta)=y/x; */
/* Linear R=R(theta): R=Rx+theta/(pi/2)*(Ry-Rx); Rx&Ry at 0&pi/2) in Quadrant I;
 *  thus f(x,y)=(Ry-Rx)*arctan(y/x)-pi/2*(hypot(x,y)-Rx);
 *  Householder's method (http://www.wikipedia.org) (20080508)
 *   1st order--Newton's method (quadratic convergence) x1=x0-f(x0)/f'(x0)
 *   2nd order--Halley's method (cubic convergence) x1=x-2f*f'/(2*f'*f'-f*f'')
 *    (when f''=0 Halley's method degenerates to Newton's method) */
/* Elliptical R=R(theta): (x/Rx)^2+(y/Ry)^2=1; analytical solution available; */
/* Circle is special case of both situations; Detail in Henghua20080507Notes. */
/* find local x coordinate at given local y for Q1 arc (canonical position) */
double arcQ1x(int arcorder, double Rx, double Ry, double y)
{ /* calling syntax: x=arcQ1x(arcorder,Rx,Ry,y); 20120224 from q1rim.c findx();
     result same as x=arcQ1y(arcorder,Ry,Rx,y); */
    unsigned int i=0; /* Ry_Rx=Ry-Rx; R=hypot(x,y); RR=R*R; */
    double x, x1, yy=y*y, err, Ry_Rx, R, RR;
    double f, f1, f2; /* f(x), f'(x), f''(x) */

    /* boundary situations (Notes20081216Deng) and special cases */
    if (y>=Ry) return(0.0);
    if (y<=0.0) return(Rx);
    if (fabs(Rx-Ry)<EPS) return(sqrt(fabs(Rx*Rx-yy))); /* circle */
    /* elliptical R=R(theta); In canonical position: a=Rx; b=Ry; */
    if (arcorder==2) return(Rx*sqrt(1-yy/(Ry*Ry)));

    /* linear R=R(theta) */
    Ry_Rx=Ry-Rx;
    i=0; x=sqrt(fabs((Rx+Ry)*(Rx+Ry)/4-yy)); /* initial guess */
    do {
        R=hypot(x,y); RR=R*R; /* RR=x*x+yy; R=sqrt(RR); */
        f=Ry_Rx*atan(y/x)-PI_2*(R-Rx); /* f(x) */
        f1=-Ry_Rx*y/RR-PI_2*x/R; /* f'(x) */
        /* x1=x-f/f1; /* Newton's method */ 
        f2=Ry_Rx*2*x*y/(RR*RR)-PI_2*yy/(R*RR); /* f''(x) */
        x1=x-2*f*f1/(2*f1*f1-f*f2); /* Halley's method 20080508 */
        err=fabs(x1-x); x=x1; i++;
    } while ((err>1e-9)&&(i<100));
    if (i>=100) {
        printf("arcQ1x(Rx=%.4f,Ry=%.4f,y=%.4f) not converge !",Rx,Ry,y);
        printf("  after %d iterations return (x=%.4f,err=%.4e)\n",i,x,err);
    }
    return(x);
}
/* find local y coordinate at given local x for Q1 arc (canonical position) */
double arcQ1y(int arcorder, double Rx, double Ry, double x)
{ /* calling syntax: y=arcQ1y(arcorder,Rx,Ry,x); 20120224 from q1rim.c findy();
     result same as y=arcQ1x(arcorder,Ry,Rx,x); */
    unsigned int i=0; /* Ry_Rx=Ry-Rx; R=hypot(x,y); RR=R*R; */
    double xx=x*x, y, y1, err, Ry_Rx, R, RR;
    double f, f1, f2; /* f(y), f'(y), f''(y) */

    /* boundary situations (Notes20081216Deng) and special cases */
    if (x>=Rx) return(0.0);
    if (x<=0.0) return(Ry);
    if (fabs(Rx-Ry)<EPS) return(sqrt(fabs(Rx*Rx-xx))); /* circle */
    /* elliptical R=R(theta); In canonical position: a=Rx; b=Ry; */
    if (arcorder==2) return(Ry*sqrt(1-xx/(Rx*Rx)));

    /* linear R=R(theta) */
    Ry_Rx=Ry-Rx;
    i=0; y=sqrt(fabs((Rx+Ry)*(Rx+Ry)/4-xx)); /* initial guess */
    do {
        R=hypot(x,y); RR=R*R; /* RR=xx+y*y; R=sqrt(RR); */
        f=Ry_Rx*atan(y/x)-PI_2*(R-Rx); /* f(y) */
        f1=Ry_Rx*x/RR-PI_2*y/R; /* f'(y) */
        /* y1=y-f/f1; /* Newton's method */ 
        f2=-Ry_Rx*2*x*y/(RR*RR)-PI_2*xx/(R*RR); /* f''(y) */
        y1=y-2*f*f1/(2*f1*f1-f*f2); /* Halley's method 20080508 */
        err=fabs(y1-y); y=y1; i++;
    } while ((err>1e-9)&&(i<100));
    if (i>=100) {
        printf("arcQ1y(Rx=%.4f,Ry=%.4f,x=%.4f) not converge !",Rx,Ry,x);
        printf("  after %d iterations return (y=%.4f,err=%.4e)\n",i,y,err);
    }
    return(y);
}

/* obtain Rx (at 0-deg) and Ry (90-deg) from any two points (p1&p2) on Q1 arc */
void arcQ1RxRy(double *Rx, double *Ry,\
               int arcorder, double R1, double R2, double ang1, double ang2)
{ /* calling syntax: arcQ1RxRy(&Rx,&Ry,arcorder,R1,R2,ang1,ang2); */
    double ratio, x1, y1, x2, y2, x1x1, y1y1, x2x2, y2y2, nmr;
    if (arcorder==1) { /* linear R=R(theta) */
        ratio=(R2-R1)/(ang2-ang1);
        *Rx=R1-ratio*ang1;
        *Ry=R2+ratio*(PI_2-ang2);
    } else { /* elliptical R=R(theta); Notes20081204Deng; */
        x1=R1*cos(ang1); y1=R1*sin(ang1);
        x2=R2*cos(ang2); y2=R2*sin(ang2);
        x1x1=x1*x1; y1y1=y1*y1; x2x2=x2*x2; y2y2=y2*y2;
        nmr=x1x1*y2y2-x2x2*y1y1; /* numerator */
        *Rx=sqrt(nmr/(y2y2-y1y1));
        *Ry=sqrt(nmr/(x1x1-x2x2));
    }
    return;
}
/* get canonical ellipse radius R from theta in Quadrant I */
double arcQ1R(int arcorder, double theta, double Rx, double Ry)
{ /* calling syntax: R=acrQ1R(arcorder,theta,Rx,Ry); 2/PI=1/PI_2=1*InvPI2; */
    if (arcorder==1) return(Rx+theta*InvPI2*(Ry-Rx)); /* linear R=R(theta); */
    else return(Rx*Ry/hypot(Rx*sin(theta),Ry*cos(theta))); /* ellipr(); */
}

/* -------------------------------------------------------------------------- */
/* get Arc/Ellipse points (PX,PY) ([0]~[N-1]) (from High to Low (clockwise)) */
/* Arc/Ellipse canonical position(center origin(OX,OY), major/minor axis X/Y) */
void arcPnts(double *PX, double *PY, unsigned int N, int arcorder, double angaxis,\
             double OX, double OY, double XH, double YH, double XL, double YL)
{ /* calling syntax: arcPnts(PX,PY,N,arcorder,angaxis,OX,OY,XH,YH,XL,YL); */
  /* arcorder: 1 for linear-incremental radius, 2 for elliptical R=R(theta); */
  /* angaxis: angle of ellipse major axis, not needed (set 0) for linear arc; */
    unsigned int i=1, j=0;
    double R, RH, RL, dR, ang, angH, angL, angspan, angres;
    double a, b; /* ellipse parameters */

    if (N<3) { PX[0]=XH; PY[0]=YH; PX[1]=XL; PY[1]=YL; return; }
    XY2Polar(&RH,&angH,OX,OY,XH,YH);
    XY2Polar(&RL,&angL,OX,OY,XL,YL);
    angspan=ArcSpan(angH,angL); /* ArcSpan() range in (0, 2*pi] */
    /* angL=angH-angspan; /* if (angL==PI2) angL=0; */
    angres=angspan/(double)N; /* angular resolution */

    /* arc points */
    PX[0]=XH; PY[0]=YH; /* first point (High) */
    if (fabs(RH-RL)<EPS) { /* constant radius (arc of a circle) */
        for (i=1,j=0; i<N; i++,j++) /* commath.h rotxy() */
            rotxy(&PX[i],&PY[i],PX[j],PY[j],OX,OY,-angres); /* clockwise (-) */
    } else {
        if (arcorder==1) { /* linear-incremental radius */
            dR=(RL-RH)/(double)N; /* dR=(RL-RH)*angres/angspan; */
            ang=angH;
            for (i=1,j=0; i<N; i++,j++){
                R=hypot(PX[j]-OX,PY[j]-OY);
                R+=dR;
                ang-=angres; /* clockwise (-) */
                PX[i]=OX+R*cos(ang);
                PY[i]=OY+R*sin(ang);
            }
        } else { /* elliptical R=R(theta) */
            Ellipab(&a,&b,angaxis,OX,OY,XH,YH,XL,YL);
            ang=angH;
            for (i=1,j=0; i<N; i++,j++){
                ang-=angres; /* clockwise (-) */
                EllipXY(&PX[i],&PY[i],OX,OY,angaxis,ang,a,b);
            }
        }
    }
    PX[N-1]=XL; PY[N-1]=YL; /* adjust last point (Low) */

    return;
}

/* -------------------------------------------------------------------------- */
/* Sort (Descending Order) cartesian coordinates by polar coordinate angles;
 * Bubble Sort FAST for PRE-SORTED; (REF.: GrahamSort(); commath.h sorting;)
 * NOTE: input PX,PY,PN MODIFIED to output due to sorting and nocolinear(); */
void sortangQ1(double *PX, double *PY, unsigned int *PN, double OX, double OY)
{ /* 20110712 WIHOUT malloc(); Quadrant I (Q1); USTABLE due to atan2() accuracy;
   * will work for Q1~Q4 (0-360deg) if PntAng()/atan2() stable; calling syntax:
   * sortangQ1(PX,PY,&PN,OX,OY); or sortangQ1(&PX[0],&PY[0],&PN,OX,OY); */
  /* sortangQ1() unstable with atan2() (INFINITE LOOP on a specific machine).
   * USE SorAngQ1() with isLeft() or SortAngQD() (comments on machine errors);
   * REF.: O'Rourke1998 pp.80 atan2() vs. slope (Quadrant I) or cos() in Q1&Q2; */
    unsigned int N=*PN, i=0, j=1; /* N=PN[0]; */
    double angi=0.0, angj=0.0;
    bool flag=true; /* flag: TRUE means sorting INCOMPLETE */

    if (N<2) return;
    while (flag) {
        flag=false;
        for ( i=0,j=1; j<N; i++,j++ ) { /* sort by angle in polar coordinate */
            angi=PntAng(OX,OY,PX[i],PY[i]);
            angj=PntAng(OX,OY,PX[j],PY[j]);
            if (angi<angj) { /* may consider (angi-angj)<EPS or err as below */
                swap(&PX[i],&PX[j]);
                swap(&PY[i],&PY[j]);
                flag=true;
            }
        }
    }
    /* remove collinear (X,Y) coordinates (delete repeat & collinear) */
    nocolinear(PX,PY,PN,EPS);
    return;
}
/* IMPROVED STABLE angular bubble sort (Descending Order) in Quadrant I (Q1) */
/* works for Quadrant II (Q2) or (Q1+Q2) as well (thus O(n^2) GrahamSort() for
 * non-presorted); HOWEVER do not expand to Q3/Q4 due to sign change; */
void SortAngQ1(double *PX, double *PY, unsigned int PN, double OX, double OY)
{ /* 20111114 SortAngQ1(PX,PY,PN,OX,OY); SortAngQ1(&PX[0],&PY[0],PN,OX,OY); */
    unsigned int i=0, j=1;
    double S; /* >0 couterclockwise(isLeft), 0 collinear, <0 clockwise; */
    bool flag=true; /* flag: TRUE means sorting INCOMPLETE */

    if (PN<2) return;
    while (flag) {
        flag=false;
        for ( i=0,j=1; j<PN; i++,j++ ) { /* sort angles with isLeft()/isCCW() */
            /* if (isLeft(OX,OY,PX[i],PY[i],PX[j],PY[j])) { /* angi<angj; */
            S=(PX[i]-OX)*(PY[j]-OY)-(PX[j]-OX)*(PY[i]-OY); /* TriDet2(); */
            if ( (S>0.0) /* angi<angj; same angle(slope) by radial distance */
              ||(S==0.0)&&(hypot(PX[i]-OX,PY[i]-OY)>hypot(PX[j]-OX,PY[j]-OY))) {
                swap(&PX[i],&PX[j]);
                swap(&PY[i],&PY[j]);
                flag=true;
            }
        }
    }
    return; /* call nocolinear() outside; NOTE input argument PN vs. *PN */
}
/* angular bubble sort (Descending Order) in Any Quadrant (QD) */
/* obtain angles of arc points to sort in [0,2*pi] (after rotation) */
/* (steps 1/2/3: LARGE MACHINE ERROR (sin(),cos(),pi) affects sorting) */
/*   1.exp1j(&ex,&ey,-AngL); /* rotation exp(-1j*AngL)=ex+1j*ey */
/*   2.cprod(&X[i],&Y[i],PX[i],PY[i],ex,ey); /*X+1j*Y=(PX+1j*PY)*exp(-1j*AngL)*/
/*   3.for (i=0;i<N;i++) XY2Polar(&R[i],&ang[i],OX,OY,X[i],Y[i]); */
/* Therefore, use angle-AngL+angerr to normalize angles before comparison.
 * AngL: the minimum angle ( within [0,2*pi) ) of arc points. */
void sortangQD(double *PX, double *PY, unsigned int *PN, double OX, double OY,\
               double AngL, double grid)
{ /* 20110712; Any Quadrant (QD); SortAngQD(PX,PY,&PN,OX,OY,AngL,GRID);
   * or SortAngQD(&PX[0],&PY[0],&PN,OX,OY,AngL,GRID); */
    unsigned int N=*PN, i=0, j=1; /* N=PN[0]; */
    double angi=0.0, angj=0.0;
    double R=100.0, Rmin=100.0, angerr=0.0; /* to avoid sorting machine error */
    bool flag=true; /* flag: TRUE means sorting INCOMPLETE */

    if (N<2) return;
    Rmin=hypot(PX[0]-OX,PY[0]-OY); /* PntDist(OX,OY,PX[0],PY[0]); */
    for (i=1;i<N;i++) {
        R=hypot(PX[i]-OX,PY[i]-OY); /* PntDist(OX,OY,PX[i],PY[i]); */
        if (R<Rmin) Rmin=R; /* minimum radius */
    }
    /* angerr -- angular error from AlignGRID ( 1e-5 for GRID=1nm & R=100um ) */
    /* typical: atan(0.001/100) =9.999999999667e-006rad =0.00057295779511deg */
    angerr=atan(grid/Rmin); /* +angerr: avoid resolution error in sin()&cos() */

    while (flag) {
        flag=false;
        for ( i=0,j=1; j<N; i++,j++ ) { /* sort by angle in polar coordinate */
            /* angi=mod(PntAng(OX,OY,PX[i],PY[i])-AngL+angerr,PI2); */
            /* angj=mod(PntAng(OX,OY,PX[j],PY[j])-AngL+angerr,PI2); */
            angi=mod(atan2(PY[i]-OY,PX[i]-OX)-AngL+angerr,PI2);
            angj=mod(atan2(PY[j]-OY,PX[j]-OX)-AngL+angerr,PI2);
            if (angi<angj) {
                swap(&PX[i],&PX[j]);
                swap(&PY[i],&PY[j]);
                flag=true;
            }
        }
    }
    /* remove collinear (X,Y) coordinates (delete repeat & collinear) */
    nocolinear(PX,PY,PN,EPS);
    return;
}
void SortAngQD(double *PX, double *PY, unsigned int PN, double OX, double OY,\
               double AngL, double angerr)
{ /* 20111114; Any Quadrant (QD); SortAngQD(PX,PY,PN,OX,OY,AngL,angerr);
   * or SortAngQD(&PX[0],&PY[0],PN,OX,OY,AngL,angerr); */
  /* angerr -- angular error due to AlignGRID ( 1e-5 for GRID=1nm & R=100um ) */
  /* typical: atan(0.001/100) =9.999999999667e-006rad =0.00057295779511deg */
  /* angerr=atan(grid/Rmin); /* (+err to avoid resolution error of sin()&cos() */
    unsigned int i=0, j=1;
    double angi=0.0, angj=0.0;
    bool flag=true; /* flag: TRUE means sorting INCOMPLETE */

    if (PN<2) return;
    while (flag) {
        flag=false;
        for ( i=0,j=1; j<PN; i++,j++ ) { /* sort by angle in polar coordinate */
            angi=mod(atan2(PY[i]-OY,PX[i]-OX)-AngL+angerr,PI2);
            angj=mod(atan2(PY[j]-OY,PX[j]-OX)-AngL+angerr,PI2);
            if (angi<angj) {
                swap(&PX[i],&PX[j]);
                swap(&PY[i],&PY[j]);
                flag=true;
            }
        }
    }
    return; /* call nocolinear() outside; NOTE input argument PN vs. *PN */
}

/* ########################################################################## */
/* # functions for polygon operation (shift, flip, mirror, concatenate,...) # */
/* # Ref.: fpolyshf.m, fpolyalt.m, fpolyamp.m, fpolyrot.m, etc. # */
/* ########################################################################## */
/* -------------------------------------------------------------------------- */
void poly2mir(double *X, double *Y, unsigned int N, tLineXY M)
{ /* 20120911 polygon2D to mirror (Mirror a 2D polygon P[0]-P[1]-...-P[N-1] 
     where mirror line is (M[0][_x_],M[0][_y_])to(M[1][_x_],M[1][_y_]) */
     unsigned int i=0;
     double d; /* normal distance of point to the mirror */
     double angmir=LINEANG(M); /* angle of the mirror line */
     double angp=angmir+PI_2, angn=angmir-PI_2, ang; /* normal direction angle */
     double S; /* signed area of triangle P[i]-M[0]-M[1] */
     double Vx, Vy; /* displacement vector to the mirrored point */
     for (i=0; i<N; i++) {
         d=PntLineDist(M[0][_x_],M[0][_y_],M[1][_x_],M[1][_y_],X[i],Y[i]);
         S=TriDet2(M[0][_x_],M[0][_y_],M[1][_x_],M[1][_y_],X[i],Y[i]);
         ang = (S>0)? angn : angp;
         exp1jRang(&Vx,&Vy,d+d,ang);
         X[i] += Vx; Y[i] += Vy;
     }
     return;
}
/* -------------------------------------------------------------------------- */
/* Concatenate two polygons together (sequences maintained) */
void catpoly(double *X, double *Y, unsigned int *N,\
             double *X1, double *Y1, unsigned int N1,\
             double *X2, double *Y2, unsigned int N2)
{ /* calling syntax: catpoly(X,Y,&N,X1,Y1,N1,X2,Y2,N2); */
    unsigned int NBSZ1=N1*sizeof(double), NBSZ2=N2*sizeof(double);
    memcpy(X, X1, NBSZ1); memcpy(Y, Y1, NBSZ1); /* X <-> &X[0] */
    memcpy(&X[N1], X2, NBSZ2); memcpy(&Y[N1], Y2, NBSZ2);
    *N=N1+N2; /* or N[0]=N1+N2; */
    return;
}
/* -------------------------------------------------------------------------- */
/* Concatenate two polygons together with the second sequence reversed */
void catrev2(double *X, double *Y, unsigned int *N,\
             double *X1, double *Y1, unsigned int N1,\
             double *X2, double *Y2, unsigned int N2)
{ /* calling syntax: catrev2(X,Y,&N,X1,Y1,N1,X2,Y2,N2);
   *   equivalent to: revseqxy(X2,Y2,N2); catpoly(X,Y,&N,X1,Y1,N1,X2,Y2,N2); */
    unsigned int NBSZ1=N1*sizeof(double), k=N1, i=N2-1;
    *N=N1+N2; /* or N[0]=N1+N2; */
    /* for (i=0;i<N1;i++) { X[i]=X1[i]; Y[i]=Y2[i]; } */
    memcpy(X, X1, NBSZ1); memcpy(Y, Y1, NBSZ1);
    for (i=N2-1,k=N1;k<*N;i--) { X[k]=X2[i]; Y[k++]=Y2[i]; }
    return;
}

/* add coordinates (X,Y) to output polygon (PX,PY) (see icedpoly.c) */
/* REF.: icedpoly.c v6.00, qdrim.c v10.60 AddPoly(), q1rim.c v9.60 AddPoly();
 * GDSII polygon < 200 vertices (CANNOT=200);
 * icedpoly.c v6.00 ignores N<3 and N>199 NON-COLLINEAER polygon-vertices; */
bool AddPoly(double *X, double *Y, unsigned int N,
             double *PX, double *PY, unsigned int *PN)
{ /* calling syntax: AddPoly(X,Y,N,PX,PY,&PN);
   *   or AddPoly(X,Y,N,&PX[0],&PY[0],&PN); */
    unsigned int i, k=PN[0]; /* k=*PN; */

    if (N<3) return(false);
    for (i=0;i<N;i++) { PX[k]=X[i]; PY[k++]=Y[i]; } /* actual polygon points */
    PX[k]=NAN; PY[k++]=NAN; /* NaN+1j*NaN: polygon divide symbol */

    PN[0]=k; /* *PN=k; */
    return(true);
}
bool AddPOLY(double *X, double *Y, unsigned int N,
             double *PX, double *PY, unsigned int *PN)
{ /* ( 20110714 improved AddPoly() ) calling syntax: AddPOLY(X,Y,N,PX,PY,&PN);
   *   or AddPOLY(X,Y,N,&PX[0],&PY[0],&PN); */
    unsigned int NBSZ=N*sizeof(double); /* memory block buffer size; stdio.h BUFSIZ; */
    unsigned int k0=PN[0], kn=PN[0]+N; /* k0=*PN; kn=*PN+N; */

    if (N<3) return(false);
    memcpy(&PX[k0], X, NBSZ);
    memcpy(&PY[k0], Y, NBSZ);
    PX[kn]=NAN; PY[kn]=NAN; /* NaN+1j*NaN: polygon divide symbol */

    PN[0] += (N+1); /* UPDATE COUNTER ( N points + 1 NaN+1j*NaN ) */
    return(true);
}
/* add box to output as polygon format (see icedpoly.c) */
/* REF.: icedpoly.c v6.00, qdrim.c v10.60 AddBox(), q1rim.c v9.60 AddBox();
 * icedpoly.c v6.00 judges box & zero area; */
void AddBOX(double X1, double Y1, double X2, double Y2,
            double *PX, double *PY, unsigned int *PN)
{ /* calling syntax: AddBOX(X1,Y1,X2,Y2,PX,PY,&PN);
   *   or AddBOX(X,Y,N,&PX[0],&PY[0],&PN); */
    unsigned int k=PN[0]; /* k=*PN; */

    PX[k]=X1; PY[k++]=Y1;
    PX[k]=X2; PY[k++]=Y1;
    PX[k]=X2; PY[k++]=Y2;
    PX[k]=X1; PY[k++]=Y2;
    PX[k]=NAN; PY[k++]=NAN; /* NaN+1j*NaN: polygon divide symbol */

    PN[0]=k; /* *PN=k; */
    return;
}
/* split pie shape (e.g., arc/line + vertex) to <199 in output polygon */
void AddPIE(double *X, double *Y, unsigned int N, double Xv, double Yv,
            double *PX, double *PY, unsigned int *PN)
{ /* calling syntax: AddPIE(ArcX,ArcY,N,Xv,Yv,PX,PY,&PN); or
   *   AddPIE(ArcX,ArcY,N,Xv,Yv,&PX[0],&PY[0],&PN); special case of AddCVX(); */
    unsigned int ns=198, numps=1, k=PN[0]; /* k=*PN; */
    unsigned int i, ips=1, idxstart=0, idxend=N-1; /* loop variables */

    if (N<2) return;

    /* GDSII polygon < 200 vertices (CANNOT=200) */
    ns=198; /* 199-1 number of points per section (-1: ONE extra vertex) */
    numps=(unsigned int)ceil((double)(N-1)/(double)(ns-1)); /* number of pieces */
    idxstart=0; /* first position */
    for (ips=1;ips<=numps;ips++){ /* TOTAL from [0] to [N-1] */
        if (ips==numps) idxend=N-1; /* LAST PIECE MAY NOT FULL; */
        else idxend=ips*ns-ips; /* -ips due to adjacent section joint point */
        for (i=idxstart;i<=idxend;i++) { /* arc subsection */
            PX[k]=X[i]; PY[k++]=Y[i];
        }
        PX[k]=Xv; PY[k++]=Yv; /* arc vertex */
        PX[k]=NAN; PY[k++]=NAN; /* NaN+1j*NaN: polygon divide symbol */
        idxstart=idxend; /* loop: adjacent sections share joint point */
    }

    PN[0]=k; /* *PN=k; */
    return;
}
/* split oversized DouBLe edges to <Nmax (e.g.,<200) in output polygon */
/* REF.: qdcvx.c, qdrim.c, q1rim.c AddPie() & monotoneY()/both_edgeY(); */
bool AddDBL(double *Y, double *Xl, double *Xr, unsigned int N, unsigned int Nmax,
            double *PX, double *PY, unsigned int *PN)
{ /* 20110712; calling syntax: AddDBL(Y,Xl,Xr,N,Nmax,PX,PY,&PN); or
   *   AddDBL(Y,Xl,Xr,N,Nmax,&PX[0],&PY[0],&PN); special case of AddBOTH(); */
  /* input coordinates Y/Xl/Xr must have same N elements; l--left, r--right; */
  /* Nmax (default=199; GDSII polygon < 200 vertices (CANNOT=200)); */
    unsigned int k=PN[0],ips,i,i1,i2, numps,numpntps, idxctr,idxstart,idxend;

    if ( (N<2)||(Nmax<4) ) return(false); /* polygon(not triangle) >=4 vertices */

    /* abitrary number of splices (Notes20080929Deng) */
    /* HALF full splice: [0] to [idxctr] --> Nmax/2 points */
    idxctr=(unsigned int)(floor(0.5*Nmax)-1); /* idxctr: CenTeR (ConTRol) index */
    numps=(unsigned int)ceil((double)(N-1)/(double)idxctr); /* number of splices */
    /* Nmax=198: (N-1,numps)=(98,1),(196,2),(294,3),(392,4),(490,5),(588,6),... */
    for (ips=1;ips<=numps;ips++) { /* ips -- loop index for splices */
        idxstart=(ips-1)*idxctr; /* idxstart/end -- index of start/end point */
        if (ips==numps) idxend=N-1; /* LAST SPLICE MAY NOT FULL */
        else idxend=ips*idxctr;
        numpntps=2*(idxend-idxstart+1); /* number of points for the splice */
        for ( i=idxstart,i1=k,i2=k+numpntps-1; i<=idxend; i++ ) {
            /* i -- loop index for points; i1/i2 -- index for seg1/seg2; */
        /* i1=k+i-idxstart; i2=k+numpntps-1-(i-idxstart); if NO i1++,i2-- in loop */
            PX[i1]=Xl[i]; PY[i1++]=Y[i]; /* seg1 */
            PX[i2]=Xr[i]; PY[i2--]=Y[i]; /* seg2 */
        }
        k+=numpntps; /* renew counter */
        PX[k]=NAN; PY[k++]=NAN; /* NaN+1j*NaN: polygon divide symbol */
    }

    PN[0]=k; /* *PN=k; */
    return(true);
}
/* split oversized both edges (opposite CW/CCW) to <Nmax in output polygon; */
/* Prefer edge1&edge2 no reflex points, but combined polygon may be concave; */
/* AddBOTH() can split any oversized CONVEX polygons to GDSII compliant; */
bool AddBOTH(double *X1, double *Y1, unsigned int N1,\
             double *X2, double *Y2, unsigned int N2, unsigned int Nmax,\
             double *PX, double *PY, unsigned int *PN)
{ /* 20120203; calling syntax: AddBOTH(X1,Y1,N1,X2,Y2,N2,Nmax,PX,PY,&PN); or
   * AddBOTH(X1,Y1,N1,X2,Y2,N2,Nmax,&PX[0],&PY[0],&PN); generalized AddDBL(); */
  /* edge1 points (X1,Y1,N1), edge2 points (X2,Y2,N2), N1=N2 NOT neccessary; */
  /* Nmax (default=199; GDSII polygon < 200 vertices (CANNOT=200)); */
    unsigned int N=N1+N2, k=PN[0]; /* idx(1/2)(s/e) -- indexing of start/end */
    unsigned int i, j, ips, i1s=0, i1e=N1-1, i2s=0, i2e=N2-1;
    double r1=0.5, r2=0.5; /* ratio (weight); 0<r1<1; 0<r2<1; r1+r2=1; */
    unsigned int num1=N1, num2=N2; /* # points per splice; (num1+num2)<=Nmax) */
    unsigned int numps=1; /* number of (section1/2) splices */

    if ((N1<1)||(N2<1)||(N<3)||(Nmax<3)) return(false);

    if (N<=Nmax) { /* single section */
    /* catrev2(X,Y,&N,X1,Y1,N1,X2,Y2,N2); AddPOLY(X,Y,N,PX,PY,PN); return(1); */
        num1=N1; num2=N2; numps=1; /* default set */
    } else { /* multiple sections */
        /* ratio (weight) of edge1/2; 0<r1<1; 0<r2<1; r1+r2=1; */
        r1=(double)N1/(double)N;
        r2=1.0-r1; /* r2=(double)N2/(double)N; */
        /* (Nmax-2) to guarantee num<=Nmax, one extra each side (-2=-1-1); */
        num1=(unsigned int)ceil(r1*(double)(Nmax-2)); /* num1>=1; */
        num2=(unsigned int)ceil(r2*(double)(Nmax-2)); /* num2>=1; */
        /* each side must have minimum 2 points to draw (close) boundary */
        if (num1<2) num1=2; /* num1>=2; */
        if (num2<2) num2=2; /* num2>=2; (num1+num2)<=Nmax guaranteed */
        /* number of splices is determined by the larger side */
        i=(unsigned int)ceil((double)(N1-1)/(double)(num1-1)); /* # edge1 */
        j=(unsigned int)ceil((double)(N2-1)/(double)(num2-1)); /* # edge2 */
        numps=maxi(i,j); /* number of splices (numps>=1 guaranteed) */
    }
    /* abitrary number of splices */
    i1s=0; i2s=0; /* first splice */
    for (ips=1;ips<=numps;ips++) { /* ips -- loop index for splices */
        i1e=ips*(num1-1); i2e=ips*(num2-1); /* num-1 due to joint */
        if (i1e>=N1) i1e=N1-1; /* use the final point when out of range */
        if (i2e>=N2) i2e=N2-1; /* LAST SPLICE MAY NOT FULL */
        /* m1=i1e-i1s+1; m2=i2e-i2s+1; /*  number of local points */
        for (i=i1s; i<=i1e; i++) { /* segment1 */
            PX[k]=X1[i]; PY[k++]=Y1[i];
        }
        for (i=i2s,j=i2e; i<=i2e; i++,j--) { /* segment2 (revseqxy()) */
            PX[k]=X2[j]; PY[k++]=Y2[j]; /* j-- operation (OVERFLOW unsigned) */
        }
        PX[k]=NAN; PY[k++]=NAN; /* NaN+1j*NaN: polygon divide symbol */
        i1s=i1e; i2s=i2e; /* loop to the next splice */
    }

    PN[0]=k; /* *PN=k; */
    return(true);
}
/* split oversized CONVEX polygon to <Nmax (e.g.,<199) in output; */
bool AddCVX(double *X, double *Y, unsigned int N, unsigned int Nmax,\
            double *PX, double *PY, unsigned int *PN)
{ /* 20120203; calling syntax: AddCVX(X,Y,N,Nmax,PX,PY,&PN); or
   *   AddCVX(X,Y,N,Nmax,&PX[0],&PY[0],&PN); generalized AddPIE(); */
  /* Nmax (default=199; GDSII polygon < 200 vertices (CANNOT=200)); */
    unsigned int k=PN[0], i, i0, ix;
    unsigned int num=Nmax-1; /* number of points per splice (-1 common vertex) */

    if ( isnan(X[N-1])||isnan(Y[N-1]) ) N--; /* remove polygon divide symbol */
    /* redundant when the final vertex same as the first vertex */
    if ( (X[0]==X[N-1]) && (Y[0]==Y[N-1]) ) N--;
    if ( (N<3)||(Nmax<3) ) return(false); /* valid polygon >=3 vertices */
    if (N<=Nmax) return(AddPOLY(X,Y,N,PX,PY,PN)); /* AddPOLY() false for N<3 */
    if (PolyConvex(X,Y,N)<=0) {printf("AddCVX() non-convex!\n"); return(false);}

    i0=0; /* i0=idxRightYmin(X,Y,N); for rightmost lowest vertex */
    PX[k]=X[i0]; PY[k++]=Y[i0]; /* start point of first polygon */
    for (i=1;i<N;i++) {
        ix = (i+i0) % N; /* cyclic */
        PX[k]=X[ix]; PY[k++]=Y[ix];
        if ( ( i % num ) == 0 ) { /* when get to maximum capacity */
            PX[k]=NAN; PY[k++]=NAN; /* NaN+1j*NaN: polygon divide symbol */
            PX[k]=X[i0]; PY[k++]=Y[i0]; /* start point of next polygon */
            PX[k]=X[ix]; PY[k++]=Y[ix]; /* joint point of next polygon */
        } /* loop to the next splice */
    }
    if ( ( N % num ) == 0 ) k -= 2; /* final point at maximum capacity */
    else {PX[k]=NAN; PY[k++]=NAN;} /* NaN+1j*NaN: polygon divide symbol */

    PN[0]=k; /* *PN=k; */
    return(true);
}
/* ========================================================================== */

/* ########################################################################## */
/* functions for convex hull and convex polygons */
/* ########################################################################## */
/* (O'Rourke1998 Code3.4, Lemma1.2.1/1.2.7, & pp.70) strictly convex vertex: */
/* rightmost lowest vertex (Ymin) (similarily, leftmost highest vertex (Ymax),
 * lowest rightmost(Xmax), highest leftmost(Xmin)) (REF.: commath.h minmaxidx() */
/* find indexing to rightmost lowest(Ymin) vertex (must be strictly convex) */
/* which is important for: determining polygon orientation, Graham Scan, etc. */
unsigned int idxRightYmin(double *X, double *Y, unsigned int N)
{ /* calling syntax: idxRightYmin(X,Y,N); or idxRightYmin(&X[0],&Y[0],N) */
    unsigned int i=1, idx=0; /* idx: rightmost lowest(Ymin) vertex */
    for (i=1; i<N; i++) {
        if( (Y[i]<Y[idx]) || ( (Y[i]==Y[idx]) && (X[i]>X[idx]) ) ) idx=i;
    }
    return(idx); /* (X[idx],Y[idx]) rightmost lowest vertex coordinate (Ymin) */
}
/* find indexing to leftmost highest(Ymax) vertex (must be strictly convex) */
unsigned int idxLeftYmax(double *X, double *Y, unsigned int N)
{ /* calling syntax: idxLeftYmax(X,Y,N); or idxLeftYmax(&X[0],&Y[0],N) */
    unsigned int i=1, idx=0; /* idx: leftmost highest(Ymax) vertex */
    for (i=1; i<N; i++) {
        if( (Y[i]>Y[idx]) || ( (Y[i]==Y[idx]) && (X[i]<X[idx]) ) ) idx=i;
    }
    return(idx); /* (X[idx],Y[idx]) leftmost highest vertex coordinate (Ymax) */
}
/* find indexing to lowest rightmost(Xmax) vertex (must be strictly convex) */
unsigned int idxLowXmax(double *X, double *Y, unsigned int N)
{ /* calling syntax: idxLowXmax(X,Y,N); or idxLowXmax(&X[0],&Y[0],N) */
    unsigned int i=1, idx=0; /* idx: lowest rightmost(Xmax) vertex */
    for (i=1; i<N; i++) {
        if( (X[i]>X[idx]) || ( (X[i]==X[idx]) && (Y[i]<Y[idx]) ) ) idx=i;
    }
    return(idx); /* (X[idx],Y[idx]) lowest rightmost vertex coordinate (Xmax) */
}
/* find indexing to highest leftmost(Xmin) vertex (must be strictly convex) */
unsigned int idxHighXmin(double *X, double *Y, unsigned int N)
{ /* calling syntax: idxHighXmin(X,Y,N); or idxHighXmin(&X[0],&Y[0],N) */
    unsigned int i=1, idx=0; /* idx: highest leftmost(Xmin) vertex */
    for (i=1; i<N; i++) {
        if( (X[i]<X[idx]) || ( (X[i]==X[idx]) && (Y[i]>Y[idx]) ) ) idx=i;
    }
    return(idx); /* (X[idx],Y[idx]) highest leftmost vertex coordinate (Xmin) */
}
/* indexing to both lowest rightmost(Xmax) and highest leftmost(Xmin) vertex */
/* Upper/Lower Hull (See Andrew's monotone chain convex hull algorithm) */
bool idxHXminLXmax(double *X, double *Y, unsigned int N,\
                   unsigned int *imin, unsigned int *imax)
{ /* idxHXminLXmax(X,Y,N,&imin,&imax); Xmin=X[imin]; Xmax=X[imax]; */
    unsigned int i=1, j=0, k=0;
    for (i=1; i<N; i++) {
        if( (X[i]<X[j]) || ( (X[i]==X[j]) && (Y[i]>Y[j]) ) ) j=i;
        if( (X[i]>X[k]) || ( (X[i]==X[k]) && (Y[i]<Y[k]) ) ) k=i;
    }
    *imin=j; *imax=k;
    return(j!=k); /* otherwise degenerated */
}

/* -------------------------------------------------------------------------- */
/* Sort (Ascending Order) cartesian coordinates by angle (slope) with respect to
 * the rightmost lowest(Ymin) vertex (for convex hull with Graham Scan);
 * REF.: http://en.wikipedia.org/wiki/Graham_scan ; O'Rourke1998 Sect.3.5; */
bool GrahamSort(double *PX, double *PY, unsigned int *PN,\
                double *X, double *Y, unsigned int N)
{ /* calling syntax: GrahamSort(PX,PY,&PN,X,Y,N); see SortAngQ1(); SortAngQD(); */
    unsigned int i, j, k, i0; /* set P[0] as origin (righmost lowest vertex) */
    unsigned int M, *idx; /* M: number of outputs; idx[] for sorted indexing; */
    double T, dx, dy, dxdx;
    double R, *cosang; /* cosine monotonically decreasing in [0,pi] */

    PX=X; PY=Y; *PN=N; if (N<3) return(false); /* initialization */
    i0=idxRightYmin(X,Y,N); /* indexing to the righmost lowest vertex */
    if (i0>0) { /* set P[0] as the base (origin); O'Rourke1998 Code 3.4; */
        T=X[0]; X[0]=X[i0]; X[i0]=T; /* swap(&X[0],&X[i0]); */
        T=Y[0]; Y[0]=Y[i0]; Y[i0]=T; /* swap(&Y[0],&Y[i0]); */
    }

    /* memory allocation */
    idx=malloc(N*sizeof(unsigned int)); /* sorted indexing */
    cosang=malloc(N*sizeof(double));  /* cosine(angle) */
    if ( (idx==NULL) || (cosang==NULL) ) return(false); /* memory fail */

    /* sort angularly from 0 to 180-deg (for angular ties, discard the closer) */
    *cosang=1; /* cosang[0]=1 (0-deg) for origin P[0] (cosang[] in [-1,1]) */
    for (i=1;i<N;i++) { /* start from P[1] */
        /* dx=X[i]-X[0]; dy=Y[i]-Y[0]; R=hypot(dx,dy); *(cosang+i)=dx/R; */
        dx=X[i]-X[0]; dy=Y[i]-Y[0]; dxdx=dx*dx;
        T=(dx<0)? -dxdx : dxdx; R=dxdx+dy*dy; /* squared sum (more efficient) */
        *(cosang+i) = ( (R==0)? -2 : T/R ); /* -2: NOT FEASIBLE cosine value */
    }
    qsortidx(cosang,N,0,idx); /* Quick Sort descending (dir=0) */
    /* for (i=0;i<N;i++) { j=idx[i]; PX[i]=X[j]; PY[i]=Y[j]; } /* if NO ties */
    while ((cosang[N-1]<-1)&&(N>0)) N--; /* reduce storage (delete unfeasible) */
    /* M=N; /* if NO ties, use above loop, skip following O(n) loop. DONE! */

    /* IF store angularly sorted vertices WITHOUT ANGULAR TIES (keep furthest) */
    M=0; PX[M]=X[0]; PY[M++]=Y[0]; /* the first vertex */
    for (i=1;i<N;i++) {
        j=idx[i]; /* current */
        if (cosang[i]==cosang[i-1]) {
            /* isCollinear(X[0],Y[0],X[j],Y[j],X[k],Y[k])) */
            k=idx[i-1]; /* previous */
            if (sqsum(X[j]-X[0],Y[j]-Y[0])>sqsum(X[k]-X[0],Y[k]-Y[0]))
                { M--; PX[M]=X[j]; PY[M++]=Y[j]; } /* update with further vertex */
        } else { PX[M]=X[j]; PY[M++]=Y[j]; } /* add the vertex */
    }

    free(idx); free(cosang); idx=NULL; cosang=NULL; /* free unneeded memory */
    if (i0>0) { /* recover original XY data set */
        T=X[0]; X[0]=X[i0]; X[i0]=T; /* swap(&X[0],&X[i0]); */
        T=Y[0]; Y[0]=Y[i0]; Y[i0]=T; /* swap(&Y[0],&Y[i0]); */
    }
    *PN=M;
    return(M>2); /* false if convex hull impossible */
}
bool GrahamSort2(double *X, double *Y, unsigned int *PN)
{ /* GrahamSort2(X,Y,&N); modified GrahamSort() with input X/Y/N reused; */
    unsigned int i, j, i0; /* set P[0] as origin (righmost lowest vertex) */
    unsigned int N=*PN, M, *idx; /* idx[] for sorted indexing; */
    double T, dx, dy, dxdx;
    double R, *cosang; /* cosine monotonically decreasing in [0,pi] */

    if (N<3) return(false); /* initialization */
    i0=idxRightYmin(X,Y,N); /* indexing to the righmost lowest vertex */
    if (i0>0) { /* set P[0] as the base (origin); O'Rourke1998 Code 3.4; */
        T=X[0]; X[0]=X[i0]; X[i0]=T; /* swap(&X[0],&X[i0]); */
        T=Y[0]; Y[0]=Y[i0]; Y[i0]=T; /* swap(&Y[0],&Y[i0]); */
    }

    /* memory allocation */
    idx=malloc(N*sizeof(unsigned int)); /* sorted indexing */
    cosang=malloc(N*sizeof(double));  /* cosine(angle) */
    if ( (idx==NULL) || (cosang==NULL) ) return(false); /* memory fail */

    /* sort angularly from 0 to 180-deg (for angular ties, discard the closer) */
    *cosang=1; /* cosang[0]=1 (0-deg) for origin P[0] (cosang[] in [-1,1]) */
    for (i=1;i<N;i++) { /* start from P[1] */
        /* dx=X[i]-X[0]; dy=Y[i]-Y[0]; R=hypot(dx,dy); *(cosang+i)=dx/R; */
        dx=X[i]-X[0]; dy=Y[i]-Y[0]; dxdx=dx*dx;
        T=(dx<0)? -dxdx : dxdx; R=dxdx+dy*dy; /* squared sum (more efficient) */
        *(cosang+i) = ( (R==0)? -2 : T/R ); /* -2: NOT FEASIBLE cosine value */
    }
    qsortidx(cosang,N,0,idx); /* Quick Sort descending (dir=0) */
    for (i=0;i<N;i++) { /* if NO ties: arrange vertex to sorted order; */
        j=idx[i]; /* swap according to sorted indexing */
        T=X[i]; X[i]=X[j]; X[j]=T; /* swap(&X[i],&X[j]); */
        T=Y[i]; Y[i]=Y[j]; Y[j]=T; /* swap(&Y[i],&Y[j]); */
    }
    while ((cosang[N-1]<-1)&&(N>0)) N--; /* reduce storage (delete unfeasible) */
    M=N; /* if NO ties, use above loop, skip following O(n) loop. DONE! */

    /* IF store angularly sorted vertices WITHOUT ANGULAR TIES (keep furthest) */
    for (i=1,M=1;i<N;i++) {
        j=i-1;
        if (cosang[i]==cosang[j]) {
            if (sqsum(X[i]-X[0],Y[i]-Y[0])>sqsum(X[j]-X[0],Y[j]-Y[0])) {
                M--; X[M]=X[i]; Y[M++]=Y[i]; /* update with further vertex */
                T=X[i]; X[i]=X[j]; X[j]=T; /* swap(&X[i],&X[j]); */
                T=Y[i]; Y[i]=Y[j]; Y[j]=T; /* swap(&Y[i],&Y[j]); */
                /* loop back see Bubble Sort, but NOT needed for Maximum only; */
            }
        } else { X[M]=X[i]; Y[M++]=Y[i]; } /* add the vertex */
    }

    free(idx); free(cosang); idx=NULL; cosang=NULL; /* free unneeded memory */
    *PN=M;
    return(M>2); /* false if convex hull impossible */
}

/* get convex hull after angular sort http://en.wikipedia.org/wiki/Graham_scan */
bool GrahamScan(double *X, double *Y, unsigned int *N)
{ /* calling syntax: GrahamScan(X,Y,&N); NOTE: input X/Y/N modified at output */
    unsigned int i=2, j=1, k=0; /* always k=j-1; */
    double T;

    if (*N<3) return(false);
    /* Robert Sedgewick & Kevin Wayne, Algorithms, 4th ed., Addison-Wesley */
    for (i=2;i<*N;i++) { /* P[0] and P[1] must be on convext hull */
        while ( (X[j]-X[k])*(Y[i]-Y[k])-(X[i]-X[k])*(Y[j]-Y[k]) <= 0 ) {
            /* if isCollinear(P[k],P[j],P[i]) ignore unnecessary points */
            if (j==1) {
                T=X[i]; X[i]=X[j]; X[j]=T; /* swap(&X[i],&X[j]); */
                T=Y[i]; Y[i]=Y[j]; Y[j]=T; /* swap(&Y[i],&Y[j]); */
                i++;
            } else { j--; k--; }
        } /* http://en.wikipedia.org/wiki/Graham_scan */
        j++; k++; /* Update j&k and swap points[i] to the correct place */
        T=X[i]; X[i]=X[j]; X[j]=T; /* swap(&X[i],&X[j]); */
        T=Y[i]; Y[i]=Y[j]; Y[j]=T; /* swap(&Y[i],&Y[j]); */
    }

    *N=j+1; /* number of points on convex hull; */
    return(*N>2); /* false if convex hull impossible */
}

/* complete algorithm for Graham algorithm to construct convex hull */
bool ConvexGraham(double *PX, double *PY, unsigned int *PN,\
                  double *X, double *Y, unsigned int N)
{/* calling syntax: ConvexGraham(PX,PY,&PN,X,Y,N);
  *   input (X,Y,N) data set; output (PX,PY,PN) convex hull; */
    return( GrahamSort(PX,PY,PN,X,Y,N) && GrahamScan(PX,PY,PN) );
}
bool ConvexGraham2(double *X, double *Y, unsigned int *N)
{/* ConvexGraham2(X,Y,&N); modified ConvexGraham() with input X/Y/N reused;
  *   input (X,Y,N) data set modified to output (X,Y,N) convex hull; */
    return( GrahamSort2(X,Y,N) && GrahamScan(X,Y,N) );
}

/* -------------------------------------------------------------------------- */
/* split convex polygon to Lower(CCW) and Upper(CW) halves in [HXmin,LXmax]) */
/* splitted Line1+revseq(Line2) is CLOSED CCW even if orignial Line is OPEN; */
/* Upper/Lower Hull (See Andrew's monotone chain convex hull algorithm) */
/* SEE: LU decomposition; qdcvx.c v10.0 LineDivDU(); qdstt.c LineDivLR(); */
/* divpolylu() force (PX,PY) CCW, set lower(LX,LY) CCW and upper(UX,UY) CW; */
bool divpolylu(double *LX, double *LY, unsigned int *LN,\
               double *UX, double *UY, unsigned int *UN,\
               double *PX, double *PY, unsigned int PN)
{ /* calling syntax: divpolylu(LX,LY,&LN, UX,UY,&UN, PX,PY,PN); */
    unsigned int i1=0, i2=0;
    int i, imax, imin; /* int due to i-- loop, unsigned int i will CRASH! */
    int S=PolyOrient(PX,PY,PN); /* 1: counterclockwise; -1: clockwise; */
    /* S==0 collinear or self-intersecting or (PN<3) */
    if ( (S==0.0) || (!idxHXminLXmax(PX,PY,PN,&i1,&i2)) )
       { LX=LY=UX=UY=NULL; *LN=*UN=0; return(false); } /* fail if degenerated */
    if (S<0.0) revseqxy(PX,PY,PN); /* make counterclockwise (CCW) */
    imin=(int)i1; imax=(int)i2; i1=0; i2=0;
    if (imax<imin) { /* Notes20110524Deng */
        for (i=imin;i<(int)PN;i++) { LX[i1]=PX[i]; LY[i1++]=PY[i]; }
        for (i=0;i<=imax;i++) { LX[i1]=PX[i]; LY[i1++]=PY[i]; }
        for (i=imin;i>=imax;i--) { UX[i2]=PX[i]; UY[i2++]=PY[i]; }
    } else {
        for (i=imin;i<=imax;i++) { LX[i1]=PX[i]; LY[i1++]=PY[i]; }
        for (i=imin;i>=0;i--) { UX[i2]=PX[i]; UY[i2++]=PY[i]; }
        for (i=PN-1;i>=imax;i--) { UX[i2]=PX[i]; UY[i2++]=PY[i]; }
    } /* vertices arranged from HXmin to LXmax, thus Lower(CCW) & Upper(CW); */
    *LN=i1; *UN=i2;
    return(true);
}
/* DivPolyLU() improved polygon LU decomposition utilizing cyclic property:
 * 20120325 keep (PX,PY) orientation, set lower(LX,LY) CCW & upper(UX,UY) CW; */
bool DivPolyLU(double *LX, double *LY, unsigned int *LN,\
               double *UX, double *UY, unsigned int *UN,\
               double *PX, double *PY, unsigned int PN)
{ /* calling syntax: DivPolyLU(LX,LY,&LN, UX,UY,&UN, PX,PY,PN); */
    unsigned int i, j, k, imin, imax, i1, i2; /* indexing range in [0,PN-1]; */
    int S=PolyOrient(PX,PY,PN); /* 1: counterclockwise; -1: clockwise; */
    /* S==0 collinear or self-intersecting or (PN<3) */
    if ( (S==0) || (!idxHXminLXmax(PX,PY,PN,&imin,&imax)) )
       { LX=LY=UX=UY=NULL; *LN=*UN=0; return(false); } /* fail if degenerated */
    /* In C (ISO 1999), c=a%b same sign as dividend a; +PN guarantee positive; */
    i1=(imin+PN-imax)%PN; i2=(imax+PN-imin)%PN; /* cyclic property: %PN */
    if (S<0) { /* input polygon clockwise (CW) */
        *LN=i1+1; *UN=i2+1; /* k++ same orientation, k-- opposite orientation */
        for (i=0,k=imin+PN; i<=i1; i++,k--) {j=k%PN; LX[i]=PX[j]; LY[i]=PY[j];}
        for (i=0,k=imin+PN; i<=i2; i++,k++) {j=k%PN; UX[i]=PX[j]; UY[i]=PY[j];}
    } else { /* input polygon counterclockwise (CCW) */
        *LN=i2+1; *UN=i1+1; /* +1, <=; k start imin, +PN guarantee positive; */
        for (i=0,k=imin+PN; i<=i2; i++,k++) {j=k%PN; LX[i]=PX[j]; LY[i]=PY[j];}
        for (i=0,k=imin+PN; i<=i1; i++,k--) {j=k%PN; UX[i]=PX[j]; UY[i]=PY[j];}
    } /* vertices arranged from HXmin to LXmax, thus Lower(CCW) & Upper(CW); */
    return(true);
}
/* ========================================================================== */

/* ########################################################################## */
/* functions for photomask layout geometric calculations */
/* ########################################################################## */
/* 20120216; Regular Symmetric S-Bend: a=W/2=R*sin(ang); b=H/2=R(1-cos(ang)); */
/* a*a+b*b=R*R*(sin*sin+1-2*cos+cos*cos)=2R*R*(1-cos)=2R*b; thus R=(aa+bb)/2b */
/* utilizing Euler's Formula sinang*sinang+cosang*cosang=1. Notes20111021DHH; */
/* signs: Radius always positive (R +); ang: - clockwise, + counterclockwise; */
/* W: - is Xe<Xs, + is Xe>Xs; H: - is Ye<Ys, + is Ye>Ys; (s-start, e-end) */
double SBendRang(double *R, double *ang, double W, double H)
{ /* calling syntax: L=SBendRang(&R,&ang,W,H); */
    double R2;
    if ( (W==0.0) || (H==0.0) ) { R=ang=NULL; return(0.0); }
    ang[0] = iseqsign(W,H)? 1.0 : -1.0; W=fabs(W); H=fabs(H);
    R[0]=(W*W+H*H)/(4.0*H); R2=R[0]+R[0]; /* sin()=W/(2R); cos()=1-H/(2R); */
    if (W>R2) { R=ang=NULL; return(0); }
    else { ang[0] *= asin(W/R2); return(R2*fabs(ang[0])); }
}
double SBendWH(double *W, double *H, double R, double ang)
{ /* calling syntax: L=SBendWH(&W,&H,R,ang); */
    double R2=R+R;
    if (R<=0.0) { W=H=NULL; return(0.0); }
    H[0]=sign(ang); ang=fabs(ang);
    W[0]=R2*sin(ang); H[0] *= R2*(1-cos(ang)); return(R2*ang);
}
double SBendRW(double *R, double *W, double ang, double H)
{ /* calling syntax: L=SBendRW(&R,&W,ang,H); */
    if ( H==0.0 ) { R=W=NULL; return(0.0); }
    W[0] = iseqsign(H,ang)? 1.0 : -1.0; ang=fabs(ang); H=fabs(H);
    R[0]=H/(2.0*(1.0-cos(ang))); W[0] *= 2*R[0]*sin(ang); return(2.0*R[0]*ang);
}
double SBendRH(double *R, double *H, double ang, double W)
{ /* calling syntax: L=SBendRH(&W,&R,H,ang); */
    if ( W==0.0 ) { R=H=NULL; return(0.0); }
    H[0] = iseqsign(W,ang)? 1.0 : -1.0; ang=fabs(ang); W=fabs(W);
    R[0]=W/(2.0*sin(ang)); H[0] *= 2.0*R[0]*(1-cos(ang)); return(2.0*R[0]*ang);
}
double SBendWang(double *W, double *ang, double R, double H)
{ /* calling syntax: L=SBendWang(&W,&ang,R,H); */
    double R2=R+R;
    ang[0] = sign(H); H=fabs(H);
    if ( (R<=0.0) || (R2<H) ) { W=ang=NULL; return(0.0); }
    W[0]=sqrt((R2+R2-H)*H); ang[0] *= asin(W[0]/R2); return(R2*ang[0]);
}
double SBendHang(double *H, double *ang, double R, double W)
{ /* calling syntax: L=SBendHang(&H,&ang,R,W); */
    double R2=R+R;
    H[0]=sign(W); W=fabs(W);
    if ( (R<=0.0) || (R2<W) ) { H=ang=NULL; return(0.0); }
    ang[0]=asin(W/R2); H[0] *= R2*(1-cos(ang[0])); return(R2*ang[0]);
}
/* -------------------------------------------------------------------------- */
/* connection parameter (R,theta) for two positions (Notes20111021DHH) */
bool cntRtheta0(double *R, double *theta, tXY pos1, tXY pos2)
{
    double dX=fabs(pos2[_x_]-pos1[_x_]), dY=fabs(pos2[_y_]-pos1[_y_]);
    double dXdX=dX*dX, dYdY=dY*dY;
    if (dX==0) return(false); /* connection is a straight line */
    *R=(dXdX+dYdY)/(dX+dX);
    *theta=acos((dYdY-dXdX)/(dYdY+dXdX)); /* *theta=asin(dY/R); */
    return(true);
}
/* ========================================================================== */
