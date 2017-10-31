/* ========================================================================== */
/* ICEDPOLY.C    .MEX file to export IC Editors (ICED) commands (GDSII compliant)
 *     for input polygons (with polygon divide symbol NaN+1j*NaN).
 *   (1). To reduce file size, use ICED abbreviations, remove trailing zeros
 *       (with %.12g format), and use "add box" command wherever possible.
 *       (In ICED: SELECT ALL; SHOW NOID F="file.CMD"; for full version.
 *        to get GDSII file: sel al; stream map; uns al; DOS rename *.SF *.GDS;)
 *   (2). Ignore redundant (adjacently repeated and collinear) points
 *       (ICED can detect and merge collinear points).
 *        ignore the redundant final vertex (when same as the first vertex)
 *       (ICED only need p1-p2-...-pN to draw closed polygon p1-p2-...-pN-p1).
 *   (3). Control warning for polygons with over Nmax (e.g. >199) vertices
 *       (e.g., NOT write to ICED command file; Write to polyout if exists).
 *
 * USAGE:
 *  (COMPILE in MATLAB: mex icedpoly.c; mex icedpoly.c -output icedpoly.dll;
 *   with C header: mex icedpoly.c commath.c comgeom.c -output icedpoly.dll;)
 *
 *  polyout=ICEDPOLY(file,layer,polyvec,polynum, GRID, Nmax, Smin, CCW);
 *
 * where file: ICED command file name (*.cmd), Append TEXT(ASCII) mode;
 *   layer: GDSII layer number (1 to 255) (layers set in icedfile.m if layer=0)
 *     (AUTOCAD layer=0 means current layer; adopt the concept here for ICED);
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
 * REFERENCE: ICEDFILE.M; ICEDPOLY.C; ACADPOLY.C; ICPOLY2.C; QDRIM.C; Q1RIM.C;
 *
 * COPYRIGHT 2012 Henghua Deng
 * -------------------------------------------------------------------------- */
/* $REVISION: 14.50 $ (20120509 Henghua Deng) */
/* HISTORY: 20101025 from ICPOLY2.C v1.20; 20101103~12 optimization/correction;
 *  20101109(v1.71)no repeated neighbors; (v1.81)no collinear neighbors;
 *  20110125(v3.00)enable polygon output; 20110425(v5.00)allow input final piece
 *   without NaN+1j*NaN polygon divide symbol; ignore redundant final vertex;
 *  20110503(v6.00)expand control; 20110511(v6.11)append text(ASCII) mode;
 *  20110602(v8.12)comgeom.c v2.50 nocollinear(); 20110713(v10.00)align option;
 *  20110714(v10.00)comgeom.h v4.00 nocolinear() WITHOUT malloc();
 *  20110926(v11.00)comcnst.h, commath.h, comgeom.h v6.00; minimum area control;
 *  20110929(v11.00)#ifndef NAN, isnan, round; 20111209(v12.00)polygons CCW;
 *  20120201(v13.00)layer=0 (current); Nmax=0/1/negative controls (fracture);
 *  20120320(v14.00)isBOX() simplification; 20120509(v14.50)com*.h Matlab mex;
/* ========================================================================== */

#define MATLAB_MEX_CODE /* #include "mex.h" "matrix.h" in #include "comcnst.h" */
#include "comcnst.h" /* #include <stdio.h><string.h><stdlib.h><math.h><time.h> */
#include "commath.h"
#include "comgeom.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    FILE *fid=NULL;
    char *filename=NULL;
    /* flagBox/S0/GRID: box, zero area (S=0), align to grid; */
    bool flagBox=false, flagS0=false, flagGRID=false;
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
    if (nrhs < 4) MsgErr("icedpoly.c: input arguments incorrect!\n");
    if (nlhs > 1) MsgErr("icedpoly.c: output arguments incorrect!\n");
    /* check polyvec dimension */
    if (mxGetM(prhs[2]) == 0) { /* NO data to write! */
        if (nlhs > 0) plhs[0]=mxCreateDoubleMatrix(0,0,mxCOMPLEX); /* empty */
        return;
    }
    if ( !mxIsComplex(prhs[2]) || (mxGetN(prhs[2])!=1) )
        MsgErr("icedpoly.c: polyvec must be a Nx1 complex vector!\n");

    /* open the file for writing (append) */
    N = mxGetM(prhs[0]) * mxGetN(prhs[0]) + 1; /* +1 due to '\0' char end */
    filename = malloc(N*sizeof(char)); /* calloc/mxCalloc(N, sizeof(char)); */
    mxGetString(prhs[0], filename, N);
    fid = fopen(filename,"at"); /* Append TEXT(ASCII) Mode (PC default BINARY) */
    if ( (filename==NULL) || (fid==NULL) )
        MsgErr("icedpoly.c: cannot open file!\n");

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
    if (nrhs < 6) Nmax=NGDSPOLY; /* GDSII-compliant OPEN polygon <200 vertices */
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
        if ( (X==NULL)||(Y==NULL) ) MsgErr("icedpoly.c OUT OF MEMORY!\n");
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
            flagS0 = true;
            printf("icedpoly.c: ignore oversized polygon (GDSII<200 vertices)!");
            printf("\t layer %d N=%d (>%d) vertices (%.12g,%.12g):(%.12g,%.12g)!\n",
                layer, nps, Nmax, GX[0], GY[0], GX[nps-1], GY[nps-1]);
        } /* NOT DONE: Nmax=-|Nmax| to fracture polygons with >|Nmax| vetices */
        
        /* write to ICED command file */
        if (!flagS0) {
            if (flagBox) {
                if (layer==0) fprintf(fid, "ad b"); /* add box current layer */
                else fprintf(fid, "ad b l%d", layer); /* add box to layer */
                fprintf(fid, " %.12g %.12g %.12g %.12g\n", GX[0], GY[0], GX[2], GY[2]);
            } else { 
                if (layer==0) fprintf(fid, "ad p"); /* add polygon current layer */
                else fprintf(fid, "ad p l%d", layer); /* add polygon to layer */
                for (i=0; i<nps; i++) fprintf(fid, " %.12g %.12g", GX[i], GY[i]);
                fprintf(fid, "\n");
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
