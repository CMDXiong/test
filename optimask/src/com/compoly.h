/* ========================================================================== */
/* COMPOLY.H    COMmon POLYgon class header file with C++
 *
 * REFERENCE: C standard library
 *
 * COPYRIGHT 2012 Henghua Deng.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20120920 Henghua Deng) */
/* HISTORY: 20110920(v1.00)initial version;
 * 20110920(v1.01);
/* ========================================================================== */

/* need include headers if used standalone */
//#include "comcnst.h" /* #include <stdio.h><string.h><stdlib.h><math.h><time.h> */
//#include "commath.h"
//#include "comgeom.h"

/* The number of diagonals in a polygon = 1/2 N(N-3); The number of triangles
* (when you draw all the diagonals from one vertex) in a polygon = (N - 2); */
/* polygon type: 1 convex, -1 concave, 0 regular simple, 2 self intersection
 *     3/4/5/6.../N equilateral N-gon; */
enum PLGNTYPE {CONCAVE=(-1),REGULAR=0,CONVEX=1,SELFINTX=2,PLGN3=3,POLY4=5};
class PolyXY { /* general 2D polygon */
  private:
    unsigned int N=200; /* Number of Vertices: default GDSII compliant N<=200 */
    tXY *XY; /* 2D XY coordinates (pointer type) */
    int8 type=0; /* polygon type: see comments */
    double S=0; /* Signed Area (>0 couterclockwise, 0 collinear, <= clockwise) */
    double L=0; /* Perimeter (>0) */
  public:
    /* PolyDet(double *X, double *Y, unsigned int N); */
    PolyXY(tXY *XY, unsigned int N)
    {
      S=PolyDet(&XY[_x_],&XY[_y_],N);
    }
    ~PolyXY()
    {
    }

    
};
class PolyGDS2 : public PolyXY { /* GDSII-compliant 2D polygon */

  N=200;
//   
//   private:
//     tXY XY[200]; /* 2D XY coordinates (vector); GDSII compliant N<=200; */
//     int8 type=0; /* polygon type: see comments */
//     double S=0; /* Signed Area (>0 couterclockwise, 0 collinear, <= clockwise) */
//     double L=0; /* Perimeter (>0) */

  public:
    /* PolyDet(double *X, double *Y, unsigned int N); */
    
    
    PolyGDS2(tXY *XY, unsigned int N)
    {
      //...
    }
    ~PolyGDS2()
    {
    }
    
};











int main()
{
   int i = 0;
   PolyGDS2 p;
   
   
}
