/* ========================================================================== */
/* ACADPOLY.C    .MEX file to export  AutoCAD (ACAD) scripts (GDSII compliant)
 *     for input polygons (with polygon divide symbol NaN+1j*NaN).
 *   (1). To reduce file size, remove trailing zeros (with %.12g format),
 *        AutoCAD boxes are drawn with closed polylines (pline).
 *   (2). Ignore redundant (adjacently repeated and collinear) points
 *        ignore the redundant final vertex (when same as the first vertex)
 *       (AutoCAD "pline p1-p2-...-pN C" for closed polygon p1-p2-...-pN-p1).
 *   (3). Control warning for polygons with over Nmax (e.g. >199) vertices
 *       (e.g., write to AutoCAD script file; Write to polyout if exists).
 *
 * USAGE:
 *  (COMPILE in MATLAB: mex acadpoly.c; mex acadpoly.c -output acadpoly.dll;
 *   with C header: mex acadpoly.c commath.c comgeom.c -output acadpoly.dll;)
 *
 *  polyout=ACADPOLY(file,layer,polyvec,polynum, GRID, Nmax, Smin, CCW);
 *
 * where file: AutoCAD script file name (*.scr), Append TEXT(ASCII) mode;
 *   layer: GDSII layer number (1 to 255) (layers set in acadfile.m)
 *       (NOT used in acadpoly.c, except for warning oversized polygons);
 *   polyvec: input complex vector (X+1j*Y; polygon divide symbol NaN+1j*NaN);
 *   polynum: total number of elements (length) of the input polygon vector;
 *   GRID (optional,default=0.001) resolution GRID (default minimum 1nm);
 *     (without GRID input, coordinates are NOT aligned to GRID;)
 *   Nmax (optional,default NGDSPOLY=199) maximum number of polygon vertices(>=3)
 *     (GDSII boundary entity is CLOSED polygon of <= 200 vertices (first==last);
 *     SET OPEN polygon 199 vertices; Artwork_Rule_MASK_AutoCAD_DXF_GDSII.pdf;)
 *     set Nmax=0 to always write to ICED command file for oversized polygons;
 *     set Nmax=1 to fracture (NOT DONE) to GDSII compliant (<=199 vertices);
 *     set Nmax=-|Nmax| (<0) to fracture (NOT DONE) >|Nmax| verices (||=abs());
 *   Smin (optional, default=EPS): minimum area criteria for polygons;
 *   CCW (optional, default=0): 0 keep orientation; 1 make all CCW; -1 make CW;
 *   polyout (optional): output polygon without redundant points;
 *
 * REFERENCE: ACADFILE.M; ACADPOLY.C; ICEDPOLY.C; ICPOLY2.C; QDRIM.C; Q1RIM.C;
 *
 * COPYRIGHT 2012 Henghua Deng
 * -------------------------------------------------------------------------- */
/* $REVISION: 14.50 $ (20120505 Henghua Deng) */
/* HISTORY: 20101025 from ICPOLY2.C v1.20; 20101103~12 optimization/correction;
 *  20101109(v1.71)no repeated neighbors; (v1.81)no collinear neighbors;
 *  20110125(v3.00)enable polygon output; 20110425(v5.00)allow input final piece
 *   without NaN+1j*NaN polygon divide symbol; ignore redundant final vertex;
 *  20110503(v6.00)expand control; 20110511(v6.11)append text(ASCII) mode;
 *  20110510(v6.22)ACADPOLY.C from ICEDPOLY.C v6.11;
 *  20110602(v8.22)comgeom.c v2.50 nocollinear(); 20110713(v10.08)align option;
 *  20110714(v10.14)comgeom.h v4.00 nocolinear() WITHOUT malloc();
 *  20110926(v11.22)comcnst.h, commath.h, comgeom.h v6.00; minimum area control;
 *  20110929(v11.00)#ifndef NAN, isnan, round; 20111209(v12.25)polygons CCW;
 *  20120201(v13.00)layer=0 (current); Nmax=0/1/negative controls (fracture);
 *  20120320(v14.00)isBOX() simplification; 20120509(v14.50)com*.h Matlab mex;
 * ========================================================================== */

#define MATLAB_MEX_CODE /* #include "mex.h" "matrix.h" in #include "comcnst.h" */
#include "comcnst.h" /* #include <stdio.h><string.h><stdlib.h><math.h><time.h> */
#include "commath.h"
#include "comgeom.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    FILE *fid=NULL;
    char *filename=NULL;
    /* flagBox/S0/GRID: box, zero area (S=0), align to grid; */
    bool flagBox=false, flagS0=false, flagBig=false, flagGRID=false;
    double *ptr=NULL, *PX=NULL, *PY=NULL, *X=NULL, *Y=NULL; /* pointers */
    double *GX=NULL, *GY=NULL; /* pointers to on-GRID (PX,PY); noncollinear; */
    unsigned int NBSZ=1024; /* memory block buffer size; stdio.h BUFSIZ; */
    unsigned int layer=1, i, idxcur=0, idxnan=0, idxend=0;
    /* i--loop values; idxcur/nan/end--indexing for current/NaN/final points; */
    unsigned int nps=0, N=0; /* number of points: nps:per piece, N:total; */
    int Nmax=NGDSPOLY; /* GDSII-compliant OPEN polygon <200 vertices */
    int CCW=0; /* 0 keep polygon orientation; 1 make all CCW; -1 make all CW; */
    double S=0.0; /* signed area to check/make polygons orientation(CCW/CW) */
    double Smin=EPS; /* minimum area criteria for polygons; or Smin=S0; */
    double GRID=0.001; /* resolution grid (default 1nm) */
    /* double S0; /* distinguishable triangle minimum area S0=0.5*(GRID/2)^2 */

    /* check input/output arguments */
    if (nrhs < 4) MsgErr("acadpoly.c: input arguments incorrect!\n");
    if (nlhs > 1) MsgErr("acadpoly.c: output arguments incorrect!\n");
    /* check polyvec dimension */
    if (mxGetM(prhs[2]) == 0) { /* NO data to write! */
        if (nlhs > 0) plhs[0]=mxCreateDoubleMatrix(0,0,mxCOMPLEX); /* empty */
        return;
    }
    if ( !mxIsComplex(prhs[2]) || (mxGetN(prhs[2])!=1) )
        MsgErr("acadpoly.c: polyvec must be a Nx1 complex vector!\n");

    /* open the file for writing (append) */
    N = mxGetM(prhs[0]) * mxGetN(prhs[0]) + 1; /* +1 due to '\0' char end */
    filename = malloc(N*sizeof(char)); /* calloc/mxCalloc(N, sizeof(char)); */
    mxGetString(prhs[0], filename, N);
    fid = fopen(filename,"at"); /* Append TEXT(ASCII) Mode (PC default BINARY) */
    if ( (filename==NULL) || (fid==NULL) )
        MsgErr("acadpoly.c: cannot open file!\n");

    /* read input polygon information */
    ptr = mxGetPr(prhs[1]); layer = (unsigned int) ptr[0]; /* GDSII layer */
    PX = mxGetPr(prhs[2]); PY = mxGetPi(prhs[2]); /* input coordinates */
    ptr = mxGetPr(prhs[3]); idxend = (unsigned int) ptr[0];

    /* read input control parameters */
    /* GRID (optional. default=0.001) Resolution GRID (default 1nm) */
    if (nrhs < 5) { GRID = 0.001; flagGRID=false; }
    else { ptr= mxGetPr(prhs[4]); GRID = ptr[0]; if (GRID>0) flagGRID=true; }
    /* inverse of GRID: 1/GRID <=> 1*InvGRID; */
    /* S0=GRID*GRID*0.125; /* distinguishable triangle area 0.5*(GRID/2)^2 */
    /* Nmax (optional,default=199) maximum number of vertices in each polygon;*/
    if (nrhs < 6) Nmax = NGDSPOLY; /* GDSII-compliant OPEN polygon <200 vertices */
    else {ptr = mxGetPr(prhs[5]); Nmax =(int)ptr[0]; if (Nmax==1) Nmax = -199;}
    if (nrhs < 7) Smin = EPS; /* minimum area; or use S0=GRID*GRID*0.125; */
    else {ptr = mxGetPr(prhs[6]); Smin = ptr[0];}
    if (nrhs < 8) CCW = 0; /* 0 keep orientation; 1 make all CCW; -1 make CW; */

    /* initialization for processing output polygon coordinates */
    N = 0; /* number of output points */
    /* allocate dynamic memory: (mx)malloc(N*size); (mx)calloc(N,size); FREE(); */
    /* Matlab mex file CRASH if malloc() return NULL; AVOID use in SUBfunctions;
     * 20120507 for better management <#define malloc mxMalloc> in comcnst.h; */
    if (nlhs == 1) {
        NBSZ = idxend*sizedouble;
        X = malloc(NBSZ); Y = malloc(NBSZ);
        if ( (X==NULL)||(Y==NULL) ) MsgErr("acadpoly.c OUT OF MEMORY!\n");
    }

    /* ========= loop polygon-pieces and process points ========= */
    /* (In C, while loop continues until condition becomes false.) */
    idxcur = 0; idxnan = 0; /* indexing initialization */
    while (idxcur<idxend) {
        /* find current polygon piece ( !A&&!B==!(A||B) */
        while ( !(isnan(PX[idxnan])||isnan(PY[idxnan])) && (idxnan<idxend) ) idxnan++;
        nps = idxnan - idxcur; /* number of current piece points */
        
        /* (IMPORTANT) i-th point is (GX[i],GY[i]) or ( *(GX+i), *(GY+i) ); */
        GX=&PX[idxcur]; GY=&PY[idxcur]; /* pointer to current start point; */
        
        /* align to resolution grid (if required) */
        if (flagGRID) align2(GX,GY,nps,GRID);
        
        /* remove collinear (X,Y) coordinates; nocollinear()/nocolinear(); */
        nocolinear(GX,GY,&nps,EPS); /* nps value may be reduced */
        /* redundant when the final vertex same as the first vertex */
        if (nps>1) { if ( (GX[0]==GX[nps-1]) && (GY[0]==GY[nps-1]) ) nps--; }
        
        /* judge if polygon is a box or has zero area */
        flagBox = (nps==4)? isBOX(GX,GY) : false;
        S=PolyDet(GX,GY,nps); /* PolyDet()=0 for nps<3; See TriDET, QuadDET; */
        flagS0 = (fabs(S)<Smin);
        if (issignop(CCW,S)) revseqxy(GX,GY,nps); /* to desired orientation */
        
        /* GDSII-compliant OPEN polygon <200 vertices */
        if ( (Nmax>2) && (nps>(unsigned int)Nmax) ) {
            flagBig = true;
            printf("acadpoly.c: exists oversized polygon (GDSII<200 vertices)!");
            printf("\t layer %d N=%d (>%d) vertices (%.12g,%.12g):(%.12g,%.12g)!\n",
                layer, nps, Nmax, GX[0], GY[0], GX[nps-1], GY[nps-1]);
        } else { flagBig = false; }
        /* NOT DONE: Nmax=-|Nmax| to fracture polygons with >|Nmax| vetices */
        
        /* write to AutoCAD script file */
        if (!flagS0) {
            if (flagBox) {
                fprintf(fid, "pline %.12g,%.12g %.12g,%.12g %.12g,%.12g %.12g,%.12g C\n",
                    GX[0], GY[0], GX[1], GY[1], GX[2], GY[2], GX[3], GY[3]);
            } else {
                if (flagBig) fprintf(fid, "; oversized polygon (N=%d):\n",nps);
                fprintf(fid, "pline %.12g,%.12g", GX[0], GY[0]);
                for (i=1; i<nps; i++)
                    fprintf(fid, " %.12g,%.12g", GX[i], GY[i]);
                fprintf(fid, " C\n"); /* C -- close polygon*/
            }
        }
        
        /* output polygon vector (even though zero area (S=0) ) */
        if (nlhs == 1) { /* REF.: comgeom.h v5.00 AddPoly() */
            for (i=0; i<nps; i++) {
                X[N] = GX[i]; Y[N++] = GY[i];
            }
            X[N]=NAN; Y[N++]=NAN; /* NaN+1j*NaN: polygon divide symbol */
            /* N increased by nps vertices and one NaN+1j*NaN */
        }
        
        /* loop to the next polygon piece */
        idxcur = ++idxnan; /* idxcur = idxnan + 1; idxnan = idxcur; */
    }

    /* output optimized polygon vectors */
    if ( nlhs == 1 ) {
        plhs[0] = mxCreateDoubleMatrix(N, 1, mxCOMPLEX);
        NBSZ = N*sizedouble;
        ptr = mxGetPr(plhs[0]); memcpy(ptr, X, NBSZ);
        ptr = mxGetPi(plhs[0]); memcpy(ptr, Y, NBSZ);
        free(X); free(Y); X=Y=NULL;
    }

    fclose(fid); free(filename); fid=NULL; filename=NULL;
    return;
}
