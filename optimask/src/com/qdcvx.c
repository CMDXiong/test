/* ========================================================================== */
/* QDCVX.C    .MEX file to fracture arbitrary CONVEX polygon. (GDSII compliant)
 * (fracture to monotone polygons http://en.wikipedia.org/wiki/Monotone_polygon)
 *
 * USAGE:
 *  (COMPILE in MATLAB: mex qdcvx.c; mex q1rim.c -output qdcvx.dll;
 *   with C header: mex qdcvx.c commath.c comgeom.c -output qdcvx.dll;)
 *
 *  [polys, line1,line2] = qdcvx(polypts,numpt, segres, GRID,MASKGRID, debug);
 *
 *  where polypts: complex (X+1j*Y) vector of polyline(CLOSED/OPEN) coordinates;
 *   numpt: number of polyline points;
 *   segres (optional,default=1.0): segment resolution (default 1 micrometre),
 *     NO snap if segres<GRID (e.g., when segres set as 0);
 *   GRID (optional,default=0.001) resolution GRID (default minimum 1nm);
 *   MASKGRID (optional,default=32.768) MASK fracture GRID (A/D 15bits -->
 *     2^15=32768nm) Horizontally (MASKGRID/32=2^10=1024nm Vertically);
 *     (when MASKGRID<=0, NO fracturing, but snapping and GDSII compliant;)
 *   debug (optional,default=0,value(0,1,2)) controls display information;
 * polys(complex)--output fractured polygons(GDSII compliant);
 * line1/line2(optional,complex)--splitted lower/upper halves in [Xmin,Xmax];
 *
 * REFERENCE: qdstt.c, stpoly.c, qdrim.c, q1rim.c, icedpoly.c, acadpoly.c;
 *
 * COPYRIGHT 2012 Henghua Deng.
 * -------------------------------------------------------------------------- */
/* $REVISION: 12.50$ (20120509 Henghua Deng) */
/* HISTORY: 20090220(v1.00) stpoly.c; 20100831(v2.00)global GRID/MASKGRID input;
 *  20110428(v3.00)qdstt.c; 20110620(v5.00)qdcvx.c(new I/O, no statistics);
 *  20110708(v8.00)memory; 20110922(v9.00)com v6.0; 20120325(v11.50)DivPolyLU();
 *  20120416(v12.00)degenerate; MASKGRID<=0 option; memory management;
/* ========================================================================== */

#define MATLAB_MEX_CODE /* #include "mex.h" "matrix.h" in #include "comcnst.h" */
#include "comcnst.h" /* #include <stdio.h><string.h><stdlib.h><math.h><time.h> */
#include "commath.h"
#include "comgeom.h"

int debug=0; /* 0--no display (default), 1--less info, 2--more info; */
unsigned int numpoly=0, numsegpts=0; /* point counters */
unsigned int numpt=0, numpt1=0, numpt2=0; /* complete polyline */
unsigned int npsect=0, npsect1=0, npsect2=0; /* section for fracture */
double GRID=0.001, InvGRID=1000.0; /* resolution grid (default 1nm) */
double MASKGRID=32.768; /* mask fracture grid (15bit A/D --> 2^15=32768nm ) */
double segres=1.0; /* line edge segment resolution (default 1 micron); */
double *LineX=NULL, *LineY=NULL; /* input line coordinates */
double *Line1X=NULL, *Line1Y=NULL, *Line2X=NULL, *Line2Y=NULL; /* split line */
double Xmin, Xmax, Ymin, Ymax, YXmin, YXmax; /* bounding box (boundary) */
double CurX, NextX, CurUpY, CurDownY, NextUpY, NextDownY; /* scan */
double *CrossX=NULL, *CrossY=NULL; /* crossing intersection */
double *SectX=NULL, *SectY=NULL; /* fracture section */
double *Sect1X=NULL, *Sect1Y=NULL, *Sect2X=NULL, *Sect2Y=NULL;
double *SegPY=NULL, *SegPXl=NULL, *SegPXr=NULL; /* segment snap (l:left,r:right) */
double *PolyX=NULL, *PolyY=NULL; /* fractured snapped polygons */

/* align resolution GRID */
double AlignGRID(double X){return(round(X*InvGRID)*GRID);}
/* Increase/Decrease X/Y MASKGRID (+EPS/-EPS IMPORTANT for points on MASKGRID) */
double IncMaskGrid(double X){return(ceil((X+EPS)/MASKGRID)*MASKGRID);}
double DecMaskGrid(double Y){return(floor((Y-EPS)/MASKGRID)*MASKGRID);}

/* sorted crossing intersection X/Y coordinates at given Y/X for line (PX,PY) */
/* maximum TWO intersections for CONVEX polygon (CLOSED/OPEN); */
int findCrossX(double *crossX, double Y, double *PX, double *PY, unsigned int PN)
{ /* calling syntax: N=findCrossX(&crossX[0],Y,PX,PY,PN); */
    unsigned int i=0, j=PN-1, N=0;
    double X=0.0;
    if (PN<2) { if ((Y==PY[0])&&(PN==1)) crossX[N++]=PX[0]; return(N); }
    /* for (i=0; i<PN; i++) { j=(i+1)%PN; /* cyclic j guarantee CLOSED polygon */
    for (i=0,j=PN-1; i<PN; j=i++) { /* initial i=0, j=N-1; loop j=i, i++; */
        switch (findLsegX(&X,Y,PX[i],PY[i],PX[j],PY[j])) { /* # intersections */
            case 2: crossX[N++]=PX[i]; crossX[N++]=PX[j]; break;
            case 1: crossX[N++]=X; break;
            default: break; /* case 0: break; */
        }
    }
    /* small data set */
    if (N>0) insert_unique_inc(crossX, &N); /* Unique in Ascending Order */
    return(N); /* N must be 0, 1, or 2 for CONVEX polygon (CLOSED/OPEN) */
}
int findCrossY(double *crossY, double X, double *PX, double *PY, unsigned int PN)
{ /* calling syntax: N=findCrossY(&crossY[0],X,PX,PY,PN); */
    unsigned int i=0, j=PN-1, N=0;
    double Y=0.0;
    if (PN<2) { if ((X==PX[0])&&(PN==1)) crossY[N++]=PY[0]; return(N); }
    /* for (i=0; i<PN; i++) { j=(i+1)%PN; /* cyclic j guarantee CLOSED polygon */
    for (i=0,j=PN-1; i<PN; j=i++) { /* initial i=0, j=N-1; loop j=i, i++; */
        switch (findLsegY(X,&Y,PX[i],PY[i],PX[j],PY[j])) { /* # intersections */
            case 2: crossY[N++]=PY[i]; crossY[N++]=PY[j]; break;
            case 1: crossY[N++]=Y; break;
            default: break; /* case 0: break; */
        }
    }
    /* small data set */
    if (N>0) insert_unique_inc(crossY, &N); /* Unique in Ascending Order */
    return(N); /* N must be 0, 1, or 2 for CONVEX polygon (CLOSED/OPEN) */
}

/* POLYGON crossing Y for given X (vetical cut); U--Up; D--Down; */
bool cutLineY(double X, double *YU, double *YD)
{ /* when X out of polygon range: Ny=0 and (*YU=Ymax; *YD=Ymin;) */
    unsigned int Ny=findCrossY(CrossY,X,LineX,LineY,numpt);
    if (Ny>0) { minmax(CrossY,Ny,YD,YU); return(true); }
    else { *YD=Ymin; *YU=Ymax; return(false); }
}
/* SECTION crossing X for given Y (horizontal cut); L:Left; R:Right; */
bool cutSectX(double Y, double *XL, double *XR)
{ /* when Y out of section range: Nx=0 and (*XL=CurX; *XR=NextX;) */
    unsigned int Nx=findCrossX(CrossX,Y,SectX,SectY,npsect);
    if (Nx>0) { minmax(CrossX,Nx,XL,XR); return(true); }
    else { *XL=CurX; *XR=NextX; return(false); }
}

/* find POLYGON next X grid point (vertical cut) */
bool nextXGrid(void)
{
    if (NextX>=Xmax) return(false); /* boundary */
    CurX=NextX; CurUpY=NextUpY; CurDownY=NextDownY; /* loop initial */
    NextX=IncMaskGrid(CurX); /* increase X grid */
    if (NextX>=Xmax) NextX=Xmax; /* limit within range */
    return(cutLineY(NextX,&NextUpY,&NextDownY)); /* vertical cut */
}
/* find SECTION next Y grid point (horizontal cut) */
double sectYGrid(double Y)
{
    unsigned int i;
    double Yv=DecMaskGrid(Y);
    for (i=0;i<npsect;i++) /* { if (isInL(Y,Yv,SectY[i])) Yv=SectY[i]; } */
        { if ((Y>SectY[i])&&(Yv<=SectY[i])) Yv=SectY[i]; }
    return(Yv);
}

/* Monotone Trapezoidalization (Partitioning) with horizontal sweep lines */
/* O'Rourke1998 Chapter 2; http://en.wikipedia.org/wiki/Monotone_polygon */
/* 20111112 renamed from both_sectY(); both edges snapping along Y axis */
bool monotoneY(double Ys, double Ye, double Xsl, double Xsr, double Xel, double Xer)
{ /* Ref.: qdrim.c & q1rim.c both_edgeY(); s:start, e:end; l:left, r:right; */
    unsigned int i=1, j=0, numsel=1;
    double Xl, Xr, dY; /* scan coordinates; l:left, r:right; */
    double PX[4], PY[4]; /* output quadrilateral */

    if (fabs(Ys-Ye)<0.5*GRID) { /* zero area after AlignGRID() */
        if (debug>=2) printf("qdcvx.c monotoneY() ZERO AREA!\n");
        return(false);
    }

    /* fracture snapping option for the segment of the SECTION */
    if (segres<GRID) numsel=0; /* NO snap when below minimum resolution */
    else numsel=(unsigned int)(ceil(0.5*(hypot(Xel-Xsl,Ye-Ys)+hypot(Xer-Xsr,Ye-Ys))/segres));
    /* (ceil(0.5*(PntDist(Xsl,Ys,Xel,Ye)+PntDist(Xsr,Ys,Xer,Ye))/segres)); */
    if (numsel<1) {
        PX[0]=Xsl; PY[0]=Ys;
        PX[1]=Xel; PY[1]=Ye;
        PX[2]=Xer; PY[2]=Ye;
        PX[3]=Xsr; PY[3]=Ys;
        AddPOLY(PX,PY,4,PolyX,PolyY,&numpoly);
        return(true);
    }

    numsegpts=numsel+1; /* number of points at each side; [0] to [N]; */
    dY=(Ys-Ye)/((double)numsel); /* positive or negative */
    SegPY[0]=Ys; SegPXl[0]=Xsl; SegPXr[0]=Xsr;
    SegPY[numsel]=Ye; SegPXl[numsel]=Xel; SegPXr[numsel]=Xer;
    for ( i=1,j=0; i<numsel; i++,j++ ) {
        SegPY[i]=SegPY[j]-dY; /* Ys-dY*i; */
        SegPXl[i] = (findLineX(&Xl,SegPY[i],Xsl,Ys,Xel,Ye))? Xl : SegPXl[j];
        SegPXr[i] = (findLineX(&Xr,SegPY[i],Xsr,Ys,Xer,Ye))? Xr : SegPXr[j];
    }

    /* split (oversized) DouBLe edges to <NGDSPOLY in output polygon */
    AddDBL(SegPY,SegPXl,SegPXr,numsegpts,NGDSPOLY,PolyX,PolyY,&numpoly);
    return(true);
}

/* Cut CLOSED Section (p1-p2-...-pN-p1) within X range [CurX,NextX] */
bool SectionX(void)
{ /* REF.: DivPolyLU() polygon LU decomposition (lower & upper) */
    unsigned int i=0;

    /* Line Section 1 (lower, counterclockwise) */
    npsect1=0;
    if (CurUpY!=CurDownY) {Sect1X[npsect1]=CurX; Sect1Y[npsect1++]=CurUpY;}
    Sect1X[npsect1]=CurX; Sect1Y[npsect1++]=CurDownY;
    for (i=0; i<numpt1; i++) { /* if ((Line1X[i]-CurX)*(Line1X[i]-NextX)<0) */
        if ((CurX<Line1X[i])^(NextX<=Line1X[i])) { /* X within (CurX, NextX) */
            Sect1X[npsect1]=Line1X[i]; Sect1Y[npsect1++]=Line1Y[i];
        } /* comgeom.h v8.00 isIn() (CurX<NexX) */
     }
    Sect1X[npsect1]=NextX; Sect1Y[npsect1++]=NextDownY;

    /* Line Section 2 (upper, clockwise) */
    npsect2=0;
    Sect2X[npsect2]=CurX; Sect2Y[npsect2++]=CurUpY;
    for (i=0; i<numpt2; i++) { /* if ((Line2X[i]-CurX)*(Line2X[i]-NextX)<0) */
        if ((CurX<Line2X[i])^(NextX<=Line2X[i])) { /* X within (CurX, NextX) */
            Sect2X[npsect2]=Line2X[i]; Sect2Y[npsect2++]=Line2Y[i];
        } /* comgeom.h v8.00 isIn() (CurX<NexX) */
    }
    Sect2X[npsect2]=NextX; Sect2Y[npsect2++]=NextUpY;
    if (NextUpY!=NextDownY) {Sect2X[npsect2]=NextX; Sect2Y[npsect2++]=NextDownY;}

    /* form CLOSED section polyline Sect1+revseq(Sect2) (counterclockwise) */
    /* catpoly() (npsect1-1: redundant joint point; npsect2: CLOSED polyline; */
    /* revseqxy(Sect2X,Sect2Y,npsect2);
    catpoly(SectX,SectY,&npsect,Sect1X,Sect1Y,npsect1-1,Sect2X,Sect2Y,npsect2);*/
    catrev2(SectX,SectY,&npsect,Sect1X,Sect1Y,npsect1-1,Sect2X,Sect2Y,npsect2);

    /* remove collinear (X,Y) coordinates; nocollinear()/nocolinear(); */
    nocolinear(SectX,SectY,&npsect,EPS);

    /* print splitted sections */
    if (debug>=2) {
        printf("\n(CurX,CurUpY,CurDownY)=(%.4f,%.4f,%.4f);",\
            CurX,CurUpY,CurDownY);
        printf("\n(NextX,NextUpY,NextDownY)=(%.4f,%.4f,%.4f);",\
            NextX,NextUpY,NextDownY);
        printf("\nSect1:");
        for (i=0;i<npsect1;i++) printf(" (%.4f,%.4f)", Sect1X[i], Sect1Y[i]);
        printf("\nSect2:");
        for (i=0;i<npsect2;i++) printf(" (%.4f,%.4f)", Sect2X[i], Sect2Y[i]);
        printf("\nSect :");
        for (i=0;i<npsect;i++) printf(" (%.4f,%.4f)", SectX[i], SectY[i]);
        printf("\nnpsect=%d; (npsect1,npsect2)=(%d,%d);\n",\
            npsect,npsect1,npsect2);
        /* AddPOLY(SectX,SectY,npsect,PolyX,PolyY,&numpoly); */
    }
    /* if (npsect<4) return(false); else return(true); */
    return(npsect>3); /* >=4 vertices: minimum CLOSED polyline p1-p2-p3-p1 */
}

/* Incremental fracture along -Y direction */
void FractureY(void)
{
    double YT, YB; /* YT & YB: top & bottom Y (boundary; fracture range) */
    double UpY, DownY, Xlu, Xld, Xru, Xrd; /* l:left, r:right; u:up, d:down; */
    bool flagU, flagD;

    minmax(SectY,npsect,&YB,&YT);
    UpY=YT; flagU=cutSectX(UpY,&Xlu,&Xru); /* intial */
    do {
        DownY=sectYGrid(UpY); if (DownY<YB) DownY=YB;
        flagD=cutSectX(DownY,&Xld,&Xrd);
        if ( flagU && flagD ) monotoneY(UpY,DownY,Xlu,Xru,Xld,Xrd);
        UpY=DownY; Xlu=Xld; Xru=Xrd; flagU=flagD; /* loop to next */
    } while (DownY>YB);

    return;
}

/* main function */
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
{
    bool flagFrac=true; /* fracture or not; always snapping & GDSII compliant; */
    unsigned int NBSZ=1024; /* memory block buffer size; stdio.h BUFSIZ; */
    unsigned int i=0, Nx=1, Ny=1, NP; /* (Nx,Ny)fracture pieces at (x,y)-axis */
    double dX, dY;
    double *ptr=NULL;

    /* check for proper number of arguments */
    if (nrhs > 6) MsgErr("qdcvx.c Input arguments incorrect!\n");
    if (nlhs > 3) MsgErr("qdcvx.c Output arguments incorrect!\n");

    /* number of total polygon points */
    ptr=mxGetPr(prhs[1]); numpt=(unsigned int)ptr[0];
    /* allocate dynamic memory: (mx)malloc(N*size); (mx)calloc(N,size); FREE(); */
    /* Matlab mex file CRASH if malloc() return NULL; AVOID use in SUBfunctions;
     * 20120507 for better management <#define malloc mxMalloc> in comcnst.h; */
    NBSZ = numpt*sizedouble; /* (double*)malloc(): Casting and type safety */
    LineX = malloc(NBSZ); LineY = malloc(NBSZ);
    Line1X = malloc(NBSZ); Line1Y = malloc(NBSZ);
    Line2X = malloc(NBSZ); Line2Y = malloc(NBSZ);
    if ( !( LineX && LineY && Line1X && Line1Y && Line2X && Line2Y ) )
        MsgErr("qdcvx.c OUT OF MEMORY!\n");

    /* read input polygon coordinates (CLOSED/OPEN)) */
    ptr=mxGetPr(prhs[0]); for (i=0;i<numpt;i++) LineX[i]=ptr[i];
    ptr=mxGetPi(prhs[0]); for (i=0;i<numpt;i++) LineY[i]=ptr[i];
    if(debug>=1) {
        printf("\nqdcvx.c Input (N=%d):",numpt);
        for (i=0;i<numpt;i++) printf(" (%.4f,%.4f)", LineX[i],LineY[i]);
    }

    /* resolution (optional. default = 1.0 um) */
    if (nrhs < 3) segres = 1.0;
    else { ptr=mxGetPr(prhs[2]); segres=ptr[0]; }
    /* GRID (optional. default=0.001) Resolution GRID (default 1nm) */
    if (nrhs < 4)  { GRID = 0.001; InvGRID = 1000.0; }
    else { ptr= mxGetPr(prhs[3]); GRID = ptr[0]; InvGRID = 1.0/GRID; }
    /* inverse of GRID: 1/GRID <=> 1*InvGRID; */
    /* MASKGRID (optional. default=32.768) MASK fracture GRID */
    if (nrhs < 5) MASKGRID = 32.768; /*  (A/D 15bits --> 2^15=32768nm) */
    else { ptr = mxGetPr(prhs[4]); MASKGRID = ptr[0]; flagFrac=(MASKGRID>0); }
    /* debug (optional. default=0.) */
    if (nrhs < 6) debug = 0; else { ptr=mxGetPr(prhs[5]); debug=(int)ptr[0]; }

    /* remove collinear (X,Y) coordinates; nocollinear()/nocolinear(); */
    nocolinear(LineX,LineY,&numpt,EPS);
    if (numpt<3) { /* valid polygon minimum 3 points */
        if (debug>=1) printf("qdcvx.c Input need >=3 noncollinear vertices!\n");
        plhs[0]=mxCreateDoubleMatrix(0,0,mxCOMPLEX); /* empty */
        if (nlhs > 1) plhs[1]=mxCreateDoubleMatrix(0,0,mxCOMPLEX);
        if (nlhs > 2) plhs[2]=mxCreateDoubleMatrix(0,0,mxCOMPLEX);
        FREE(LineX); FREE(LineY);
        FREE(Line1X); FREE(Line1Y); FREE(Line2X); FREE(Line2Y);
        return;
    }
    if (numpt>NGDSPOLY) { /* Artwork_Rule_MASK_AutoCAD_DXF_GDSII.pdf */
        if (debug>=2) printf("qdcvx.c Input >NGDSPOLY(%d) vertices!\n",NGDSPOLY);
        if (numpt>NGDS2MAX) { /* GDSII absolute maximum is 8192-1 vertices */
            flagFrac=false;
            printf("qdcvx.c Input >NGDS2MAX(%d) vertices!\n",NGDS2MAX);
        }
    }

    /* for non-convex polygons (although may work for slightly concave) */
    if ( PolyConvex(LineX,LineY,numpt)<=0 ) {
        flagFrac=false; /* NO fracture for non-convex polygons */
        if (debug>=2) {
            printf("qdcvx.c Input polygon not strictly convex!");
            printf("\t N=%d vertices (%.4f,%.4f):(%.4f,%.4f):...:(%.4f,%.4f)!\n",\
                numpt, LineX[0], LineY[0], LineX[1], LineY[2],\
                LineX[numpt-1], LineY[numpt-1]);
        }
    }

    /* polygon LU decomposition (20120325 DivPolyLU() replace LineDivDU()) */
    /* split polyline to Line1(lower,CCW) + Line2(upper,CW) in [HXmin,LXmax] */
    DivPolyLU(Line1X,Line1Y,&numpt1, Line2X,Line2Y,&numpt2, LineX,LineY,numpt);
    minmax(LineY,numpt,&Ymin,&Ymax); /* [Ymin,Ymax] */
    Xmin=Line1X[0]; YXmin=Line1Y[0]; /* Highest Leftmost (HXmin) vertex */
    i=numpt1-1; Xmax=Line1X[i]; YXmax=Line1Y[i]; /* Lowest Rightmost (LXmax) */
    dX=Xmax-Xmin; dY=Ymax-Ymin;
    /* print splitted polylines */
    if (debug>=2) {
        printf("\nqdcvx.c Line  (N=%d):",numpt);
        for (i=0;i<numpt;i++) printf(" (%.4f,%.4f)", LineX[i],LineY[i]);
        printf("\nqdcvx.c Line1 (N=%d):",numpt1);
        for (i=0;i<numpt1;i++) printf(" (%.4f,%.4f)", Line1X[i],Line1Y[i]);
        printf("\nqdcvx.c Line2 (N=%d):",numpt2);
        for (i=0;i<numpt2;i++) printf(" (%.4f,%.4f)", Line2X[i],Line2Y[i]);
        printf("\n(Xmin,Xmax)=(%.4f,%.4f);(YXmin,YXmax)=(%.4f,%.4f);",\
            Xmin,Xmax,YXmin,YXmax);
        printf("(Ymin,Ymax)=(%.4f,%.4f);(dX,dY)=(%.4f,%.4f);\n",Ymin,Ymax,dX,dY);
    }

    /* allocate dynamic memory: (mx)malloc(N*size); (mx)calloc(N,size); FREE(); */
    /* Matlab mex file CRASH if malloc() return NULL; AVOID use in SUBfunctions;
     * 20120507 for better management <#define malloc mxMalloc> in comcnst.h; */
    Nx += (unsigned int)ceil(dX/MASKGRID);
    Ny += (unsigned int)ceil(dY/MASKGRID);
    NP=4*(Nx+Ny); /* each fracture introduces 2 more points each side */
    NP += 2*(numpt1+numpt2+4); /*  +4 boundaries; += for splits; */
    if (segres>=GRID) { /* NO snap if segres<GRID */
        NP += (unsigned int)(ceil(2*PolyPeri(LineX,LineY,numpt)+dX+dY)/segres);
    } /* snapping if segres>=GRID; 2* due to joint points used twice; */
    /* NP *= 2; /* 2*NP to double memory for safety; */
    if (NP>16384) {NP=16384; flagFrac=false;} /* too big(>16k); NGDS2MAX=8192;*/
    /* if (debug>=2) printf("qdcvx.c NP=%d! ",NP); */
    NBSZ = NP*sizedouble;
    PolyX = malloc(NBSZ); PolyY = malloc(NBSZ); /* fractured polygons */
    SegPY = malloc(NBSZ); /* segment points snapping */
    SegPXl = malloc(NBSZ); SegPXr = malloc(NBSZ);
    NBSZ = numpt*sizedouble; /* maximum number of crossings < numpt */
    CrossX = malloc(NBSZ); CrossY = malloc(NBSZ);
    NBSZ = (numpt+8)*sizedouble; /* +8 due to +4 for both sections */
    SectX  = malloc(NBSZ); SectY  = malloc(NBSZ);
    NBSZ = (numpt1+4)*sizedouble; /* +4 boundary vertices from vertical cut */
    Sect1X = malloc(NBSZ); Sect1Y = malloc(NBSZ);
    NBSZ = (numpt2+4)*sizedouble;
    Sect2X = malloc(NBSZ); Sect2Y = malloc(NBSZ);
    if ( !( PolyX && PolyY && SegPY && SegPXl && SegPXr && CrossX && CrossY\
            && SectX && SectY && Sect1X && Sect1Y && Sect2X && Sect2Y ) )
        MsgErr("qdcvx.c OUT OF MEMORY!\n");
    /* if (debug>=2) printf("qdcvx.c memory allocated (NP=%d)!\n",NP); */

    numpoly=0; /* MANDATORY (declaration does NOT set value) */
    if (flagFrac) { /* fracture */
        NextX=Xmin; cutLineY(NextX,&NextUpY,&NextDownY); /* initial boundary */
        while (nextXGrid()) { /* loop to draw between current and next X grid */
            if ( SectionX() )
                FractureY();
        }
    } else { /* GDSII compliant */
        AddBOTH(Line1X,Line1Y,numpt1,Line2X,Line2Y,numpt2,NGDSPOLY,\
                PolyX,PolyY,&numpoly);
    }

    /* check overflow */
    if (debug>=1) printf("(NP,numpoly)=(%d,%d);\n",NP,numpoly);
    if (numpoly>NP) { /* fracturing FAIL; profile only (GDSII compliant); */
        printf("qdcvx.c Output > Maximum Allocation!\n");
        flagFrac=false; numpoly=0; /* RESET COUNTER */
        AddBOTH(Line1X,Line1Y,numpt1,Line2X,Line2Y,numpt2,NGDSPOLY,\
                PolyX,PolyY,&numpoly); /* GDSII compliant */
    }

    /* OUTPUT */
    /* polygon output */
    if (numpoly<3) {
        plhs[0]=mxCreateDoubleMatrix(0,0,mxCOMPLEX); /* empty */
    } else {
        plhs[0] = mxCreateDoubleMatrix(numpoly, 1, mxCOMPLEX);
        NBSZ = numpoly*sizedouble;
        ptr = mxGetPr(plhs[0]); memcpy(ptr, PolyX, NBSZ);
        ptr = mxGetPi(plhs[0]); memcpy(ptr, PolyY, NBSZ);
    }
    /* seperated line output */
    if (nlhs > 1) { /* line1 */
        plhs[1] = mxCreateDoubleMatrix(numpt1, 1, mxCOMPLEX);
        if (numpt1>1) {
            NBSZ = numpt1*sizedouble;
            ptr = mxGetPr(plhs[1]); memcpy(ptr, Line1X, NBSZ);
            ptr = mxGetPi(plhs[1]); memcpy(ptr, Line1Y, NBSZ);
        }
    }
    if (nlhs > 2) { /* line2 */
        plhs[2] = mxCreateDoubleMatrix(numpt2, 1, mxCOMPLEX);
        if (numpt2>1) {
            NBSZ = numpt2*sizedouble;
            ptr = mxGetPr(plhs[2]); memcpy(ptr, Line2X, NBSZ);
            ptr = mxGetPi(plhs[2]); memcpy(ptr, Line2Y, NBSZ);
        }
    }

    /* free() CRASH non-allocated! ( http://en.wikipedia.org/wiki/Malloc ) */
    FREE(LineX); FREE(LineY);
    FREE(Line1X); FREE(Line1Y); FREE(Line2X); FREE(Line2Y);
    FREE(PolyX); FREE(PolyY);
    FREE(SegPY); FREE(SegPXl); FREE(SegPXr);
    FREE(CrossX); FREE(CrossY);
    FREE(SectX); FREE(SectY);
    FREE(Sect1X); FREE(Sect1Y); FREE(Sect2X); FREE(Sect2Y);
    return;
}
