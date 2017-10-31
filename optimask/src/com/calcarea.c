/* ========================================================================== */
/* CALCAREA.C    .MEX file to calculate the total area of a group of polygons
 *     (with polygon divide symbol NaN+1j*NaN).
 *
 * USAGE:
 *  (COMPILE in MATLAB: mex calcarea.c; mex calcarea.c -output calcarea.dll;
 *   with C header: mex calcarea.c commath.c comgeom.c -output calcarea.dll;)
 *
 *  polyarea=CALCAREA(polyvec,polynum);
 *
 * where polyvec: input complex vector (X+1j*Y; polygon divide symbol NaN+1j*NaN);
 *   polynum: total number of elements (length) of the input polygon vector;
 *   polyarea: total area of all input polygons;
 *
 * REFERENCE: ICEDPOLY.C; ACADPOLY.C; COMGEOM.H; COMMATH.H; COMCNST.H;
 *
 * COPYRIGHT 2012 Henghua Deng
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.50 $ (20120509/20110926 Henghua Deng) */
/* HISTORY: 20110926(v1.00) from ICEDPOLY.C v11.00 and COMGEOM.H v6.00;
 *  20110929(future improvement may need to remove overlapped polygon areas;)
/* ========================================================================== */

#define MATLAB_MEX_CODE /* #include "mex.h" "matrix.h" in #include "comcnst.h" */
#include "comcnst.h" /* #include <stdio.h><string.h><stdlib.h><math.h><time.h> */
#include "commath.h"
#include "comgeom.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    double *ptr=NULL, *PX=NULL, *PY=NULL; /* pointers */
    unsigned int idxcur=0, idxnan=0, idxend=0;
    /* i--loop values; idxcur/nan/end--indexing for current/NaN/final points; */
    unsigned int nps=0; /* nps--number of points (per piece) */
    double S=0.0; /* total area of all polygons */

    /* check input/output arguments */
    if (nrhs < 2) MsgErr("calcarea.c: input arguments incorrect!\n");
    if (nlhs > 1) MsgErr("calcarea.c: output arguments incorrect!\n");

    /* read input polygon information */
    PX = mxGetPr(prhs[0]); PY = mxGetPi(prhs[0]); /* input coordinates */
    ptr = mxGetPr(prhs[1]); idxend = (unsigned int) ptr[0]; /* last indexing */

    /* ========= loop polygon-pieces and process points ========= */
    /* (In C, while loop continues until condition becomes false.) */
    idxcur = 0; idxnan = 0; /* indexing initialization */
    while (idxcur<idxend) {
        /* find current polygon piece ( !A&&!B==!(A||B) */
        while ( !(isnan(PX[idxnan])||isnan(PY[idxnan])) && (idxnan<idxend) ) idxnan++;
        nps = idxnan - idxcur; /* number of current piece points */
        
        /* i-th point value is (PX[i],PY[i]) or ( *(PX+i), *(PY+i) );
         * i-th point address is (&PX[i],&PY[i]) or (PX+i, PY+i); */
        S += PolyArea(&PX[idxcur],&PY[idxcur],nps); /* PolyArea(X,Y,N) */
        
        /* loop to the next polygon piece */
        idxcur = ++idxnan; /* idxcur = idxnan + 1; idxnan = idxcur; */
    }

    /* plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
       memcpy(mxGetPr(plhs[0]), &S, sizeof(double)); */
    *plhs = mxCreateDoubleScalar(S); /* initialized to value specified */
    return;
}
