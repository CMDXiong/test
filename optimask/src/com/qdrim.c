/* ========================================================================== */
/* QDRIM.C    .MEX file to fracture arbitrary multi-quadrant RIM ([0,360]deg) in
 *     Cartesian coordinate for Linear (default) and Elliptical R=R(theta) arcs.
 * RIM located in any Quadrant(s)(span >=0deg to <=360deg (both arc1 and arc2)).
 *     (Quadrants II,III,IV obtained by transforming positions from Quadrant I;
 *      QDRIM.C (>v17) draw without fracturing for special RIMs (dQ>3).)
 * Both radii and origins of inner/outer arcs (ARC1/ARC2) may be different.
 *
 * USAGE:
 *  (COMPILE in MATLAB: mex qdrim.c; mex qdrim.c -output qdrim.dll;
 *   with C header: mex qdrim.c commath.c comgeom.c -output qdrim.dll;)
 *
 * FOR LINEAR ARC (arcorder=1):
 *  [polys, ARC1,ARC2, Rerr1,Rerr2]
 *  = qdrim(O1,O2,P1,P2,P3,P4, arcorder,arcratio,arcedge, GRID,MASKGRID, debug);
 * FOR ELLIPTIC ARC (arcorder=2):
 *  [polys, ARC1,ARC2, Rerr1,Rerr2]
 *  = qdrim(O1,O2,P1,P2,P3,P4, arcorder,arcratio,angaxis, GRID,MASKGRID, debug);
 *
 *  where O1=O1X+j*O1Y, O2=O2X+j*O2Y; (ARC origins)
 *   (R1x,R1y)/(R2x,R2y) are radius (at 0&pi/2) for transformed Q1 arc1/arc2;
 *   P1=H1X+j*H1Y, P2=H2X+j*H2Y; (start points. H--High)
 *   P3=L1X+j*L1Y, P4=L2X+j*L2Y; (stop points. L--Low)
 *   (H/L: vector angle O-P(H/L) [0,2*pi] larger/smaller; L->H counterclockwise)
 *   arcorder (optional,default=1): 1 for linear and 2 for elliptic R=R(theta);
 *   arcratio (optional,default=1(one-GRID)) controls arcres (angular resolution);
 *     Artwork_Autocad_Rules_MASK_DXF_GDSII.pdf: ARCRES(resolution),ARCSAG(error);
 *   arcedge (optional,-1:inner,1:outer,default=0:both) importance of arc edges;
 *     if arcorder=1, argument arcedge prevail (force angaxis=0);
 *   angaxis (optional, only when arcorder=2) angle(in radian) of ellipse axis;
 *     if arcorder=2, argument angaxis prevail (force arcedge=0);
 *   GRID (optional,default=0.001) resolution GRID (default minimum 1nm);
 *   MASKGRID (optional,default=32.768) MASK fracture GRID (A/D 15bits -->
 *     2^15=32768nm) Horizontally (MASKGRID/32=2^10=1024nm Vertically);
 *     (when MASKGRID<=0, NO fracturing, but snapping and GDSII compliant;)
 *   debug (optional,default=0,value(0,1,2)) controls display information;
 * polys(complex)--output fractured polygons (GDSII compliant);
 * ARC1/ARC2(optional,complex)--traces (SORTED Descending Order by angles (CW));
 * Rerr1/Rerr2(optional)--radial error pool for quality statistics (ARCSAG);
 *
 * REFERENCE: Q1RIM.C; FQDRIM.M; QD1RIM.C; QDCVX.C; STPOLY.C; arbrim.c; qtrim2.c;
 * Applicable to: fawg360.m, fawgwl.m, fawgvh.m, fawgvar.m, fawgumbc.m etc.
 *
 * COPYRIGHT 2012 Henghua Deng.
 * -------------------------------------------------------------------------- */
/* $REVISION: 18.50$ (20120509 Henghua Deng) */
/* HISTORY: 20080307to0421 AWGRIM2.c and AWGRIM1.C;
 *  20080421(v1.00 QD1RIM.C)AWGRIM2.C (vAWG) v2.42 + AWGRIM1.C (hAWG) v1.43;
 *  20081204(v4.00 QD1RIM1/2.C) 1209(v5.0 Q1RIM.C)Linear/elliptic R=R(theta);
 *  20100830(v7.30)findx()&findy() as v5.60 (v8.30) thetaZONE as v5.10/8.10;
 *  20100831~1116(v9.00)redefine input; global GRID/MASKGRID input; +arcedge;
 *  20101119~20110526(Q1RIM.C v9.66; QDRIM.C v10.66)dynamic memory; N>=200;
 *    irregular whole piece; QDRIM.C from Q1RIM.C v9.66 and FQDRIM.M v1.50;
 *  20110601(Q1RIM.C v11.10; QDRIM.C v12.10)sort trace (NO collinear points);
 *  20110704(v13.20) 0710(v14.00)optimization; 20110712(v15.00)memory management;
 *  20110926(v16.00)com*.h v6.00; 20120201(v17.00)drawWhole();MASKGRID<=0 option;
 *  20120223(v18.00)off-axis ellipse(angaxis option); usage backward-compatible;
/* ========================================================================== */

#define MATLAB_MEX_CODE /* #include "mex.h" "matrix.h" in #include "comcnst.h" */
#include "comcnst.h" /* #include <stdio.h><string.h><stdlib.h><math.h><time.h> */
#include "commath.h"
#include "comgeom.h"

/* Variables with X or Y -- in Global Coordinate 
 *   Quadrant I (Q1) Partial Arc (Initial Capital); WHOLE ARC (ALL UPPER CASE);
 * variables with x or y -- in local coordinate (all lowercase characters). */
/* Radius and angle values -- in local coordinate. */
/* Global Coordinate for Pattern Judgement (fracture) (for accuracy);
 * local coordinate for arc/points calculation (snap) (for efficiency). */
/* ********************************************************** */
/* debug--for information display in code debugging; */
/* (arc1,arc2)--(inner,outer) arc; arcorder--arc order; */
/* arcratio--arcres ratio; arcres--arc angular resolution; */
/* arcedge--importance of arc edges; angaxis--angle of ellipse (major) axis; */
/* numpoly/Npoly--counter of polygon points for partial/WHOLE arc; */
/* (PolyX,PolyY)/(POLYSX,POLYSY)--polygon storage for partial/WHOLE arc; */
/* (numarc1,numarc2)/(Narc1,Narc2)--counter of points for partial/WHOLE arcs;
 * (Pool1X,Pool1Y)/(Pool2X,Pool2Y)--points pool for partial arc1/arc2;
 * (ARC1X,ARC1Y)/(ARC2X,ARC2Y)--points pool for WHOLE ARC1/ARC2; (tracing) */
/* rdist--error (radial distance) between accurate and actual/on-grid values;
 * (rdist1,rdist2)--rdist pool for quality statistics of Q1 (arc1,arc2);
 * (Rerr1,Rerr2)--radial distance error for the WHOLE ARC (ARC1,ARC2); */
/* numarcpts--Arc Section number of points; ArcDist--rdist of arc points;
 * (ArcPtsX,ArcPtsY)--Arc Section global (X,Y) locations; */
/* (O1X,O1Y)/(O2X,O2Y)--global (X,Y) of origins of the WHOLE ARC1/ARC2;
 * (Org1X,Org1Y)/(Org2X,Org2Y)--transformed origins of arc1/arc2 in Q1;
 * (R1x,R1y)/(R2x,R2y)--local radius at (x,y) axis of arc1/arc2; */
/* (H1X,H1Y)/(H2X,H2Y)&(L1X,L1Y)/(L2X,L2Y)--start&stop points of the WHOLE ARC;
 * (PosH1X,PosH1Y)/(PosH2X,PosH2Y)--start (High) points of Quadrant I arc1/arc2;
 * (PosL1X,PosL1Y)/(PosL2X,PosL2Y)--stop (Low) points of Quadrant I arc1/arc2;
 * (H/L: vector angle O-P(H/L) [0,2*pi] larger/smaller; L->H counterclockwise)*/
/* (StartX, Start1Y, Start2Y)--First Piece treatment (Y-Plane Cut);
 * (Stop1X, Stop2X, StopY)--Last Piece treatment (X-Plane Cut); */
/* CurX and (CurDownY,CurUpY)--current X and intersect Y with Q1 (arc1,arc2);
 * NextX and (NextDownY,NextUpY)--next X and intersect Y with Q1 (arc1,arc2);
 */
/* GDSII boundary(CLOSED) N<=200 vertices; GDSII-compliant polygon(OPEN) N<200; */
const unsigned int NMAXPNTS=NGDSPOLY-1; /* NGDSPOLY: 199; -1 for extra vertex */
int debug=0; /* 0--no display (default), 1--less info, 2--more info; */
int arcorder=1; /* 1 for linear (default) and 2 for elliptical R=R(theta); */
int arcedge=0; /* -1--inner, 0--both (default), 1--outer arc importance */
double angaxis=0.0; /* angle(in radian) of ellipse (major) axis */
unsigned int Npoly=0, Narc1=0, Narc2=0; /* global counters for the WHOLE ARC */
unsigned int numpoly=0, numarcpts=0, numarc1=0, numarc2=0; /* local counters */
double GRID=0.001, InvGRID=1000, S0=1.25e-7; /* resolution grid (default 1nm) */
double MASKGRID=32.768; /* mask fracture grid (15bit A/D --> 2^15=32768nm ) */
double arcratio=1.0, arcres1, arcres2; /* arc angular resolution */
double O1X, O1Y, O2X, O2Y; /* origin of the WHOLE arc */
double R1x, R1y, R2x, R2y, Org1X, Org1Y, Org2X, Org2Y; /* Q1 arc */
double H1X, H1Y, H2X, H2Y, L1X, L1Y, L2X, L2Y; /* vertices of the WHOLE arc */
double PosH1X, PosH1Y, PosH2X, PosH2Y; /* first piece vertices in Q1. H--High */
double PosL1X, PosL1Y, PosL2X, PosL2Y; /* last piece vertices in Q1. L--Low */
double StartX, Start1Y, Start2Y, StopY, Stop1X, Stop2X; /* boundary treatment */
double CurX, CurUpY, CurDownY, NextX, NextUpY, NextDownY; /* scan */
/* double *PolyX=NULL, *PolyY=NULL; /* Q1 arc fractured polygons */
/* double *rdist1=NULL, *rdist2=NULL; /* Q1 arc radial error statistics */
/* double *Pool1X=NULL, *Pool1Y=NULL, *Pool2X=NULL, *Pool2Y=NULL; /* Q1 arc traces */
double *ArcPtsX=NULL, *ArcPtsY=NULL, *ArcDist=NULL; /* Q1 arc section; */
double *POLYSX=NULL, *POLYSY=NULL; /* WHOLE ARC fractured polygons */ 
double *Rerr1=NULL, *Rerr2=NULL; /* WHOLE ARC quality statistics */
double *ARC1X=NULL, *ARC1Y=NULL, *ARC2X=NULL, *ARC2Y=NULL; /* WHOLE ARC traces */
bool PreSort1=false, PreSort2=false; /* flag to control trace Pre-SORTing */

/* align/search resolution GRID and MASK GRID */
/* floor, ceil, round (align) global coordinate X or Y; */
double AlignGRID(double X){return(round(X*InvGRID)*GRID);}
/* Increase/Decrease X/Y MASKGRID (+EPS/-EPS IMPORTANT for points on MASKGRID) */
/* For arc in Quadrant I, Y decreases when X increases */
/* (Compare floor(0), floor(eps), ceil(0), ceil(eps) in Matlab) */
double IncMaskGrid(double X){return(ceil((X+EPS)/MASKGRID)*MASKGRID);}
double DecMaskGrid(double Y){return(floor((Y-EPS)/MASKGRID)*MASKGRID);}

/* obtain angle of (x,y) (theta), accurate radius (Racc/Rtheta), and rdist error;
 * rdist--error(radial distance, + or - ) of accurate & actual/on-grid values; */
double Racc(double x, double y, double Rx, double Ry)
{
    return(arcQ1R(arcorder,atan2(y,x),Rx,Ry));
}
double getrdist(double x, double y, double Rx, double Ry)
{
    return(hypot(x,y)-Racc(x,y,Rx,Ry));
}
void thetardist(double *theta, double *rdist, double x, double y, double Rx, double Ry)
{ /* calling syntax: thetardist(&theta,&rdist,x,y,Rx,Ry); */
    *theta=atan2(y,x);
    *rdist=hypot(x,y)-arcQ1R(arcorder,*theta,Rx,Ry);
    return;
}

/* Rerr pool for quality statistics of arcs;
 * (ARC1X,ARC1Y)/(ARC2X,ARC2Y) pool for tracing arcs (pre-sorted);
 * may not sorted due to first/last piece, junction, & reverse arc direction. */
void StatARC1(void)
{ /* inner ARC statistics and trace */
    unsigned int i;
    for (i=0;i<numarcpts;i++) {
        Rerr1[Narc1]=ArcDist[i]; /*statistics*/
        ARC1X[Narc1]=ArcPtsX[i];
        ARC1Y[Narc1++]=ArcPtsY[i]; /*trace*/
    }
    return;
}
void StatARC2(void)
{ /* outer ARC statistics and trace */
    unsigned int i;
    for (i=0;i<numarcpts;i++) {
        Rerr2[Narc2]=ArcDist[i]; /*statistics*/
        ARC2X[Narc2]=ArcPtsX[i];
        ARC2Y[Narc2++]=ArcPtsY[i]; /*trace*/
    }
    return;
}

/* Snap arc section between two vertices in Quadrant I */
/* ArcSect() returns global variables (numarcpts, ArcPtsX, ArcPtsY, ArcDist)
 *     numarcpts -- number of arc section points
 *    (ArcPtsX, ArcPtsY) -- global coordinates (X,Y) (already on-GRID)
 *     ArcDist -- rdist of arc section points (for quality statistics) */
/* (OX,OY) -- global (X,Y) for arc origin center */
/* (Rx,Ry) -- local radius in Quadrant I at (0,pi/2) degree */
/* (xs,xe) -- local x coordinate of (start,end) Q1 points. LET 0<=(xs,xe)<=Rx */
/* arcres -- arc angular resolution */
bool ArcSect(double OX, double OY, double Rx, double Ry, double xs, double xe, double arcres)
{ /* (20120226 Version) */
    unsigned int i, numsel=1;
    double x1, y1, x2, y2, x, y, xi, yi; /* position */
    double upx, downx, yacc, upy, downy, xacc, R, Ri; /* scan */
    double theta1, theta2, thetaInc, thetaZONE, theta, thetai; /* angle */
    double ddist, dist, dist1, dist2; /* radial error */
    double targetrdist, prevrdist, endrdist, prevtheta, endtheta; /* evaluate */
    double DZONE=GRID+GRID; /* initial zone for point snapping */

    /* (xs,xe)--(start,end) Q1 point; LET 0<=x1<x2<=Rx, otherwise numsel<0. */
    if (xs<xe) {x1=xs; x2=xe;} else {x1=xe; x2=xs;}
    if ((x1<-EPS)||(x2>Rx+EPS)) {
        numarcpts=0;
        printf("qdrim.c ArcSect() (xs,xe) out of range!\n");
        return(false);
    }

    /* 0<=x1<x2<=Rx and Ry>=y1>y2>=0. atan(y/x)=atan2(y,x) for Quadrant I. */
    y1=arcQ1y(arcorder,Rx,Ry,x1);
    y2=arcQ1y(arcorder,Rx,Ry,x2);
    if (x1<EPS) theta1=PI_2; else theta1=atan2(y1,x1);
    if (x2>Rx-EPS) theta2=0; else theta2=atan2(y2,x2);

    numsel=(unsigned int)ceil((theta1-theta2)/arcres);
    if (numsel<1) numsel=1; /* numsel--number of selected subsections */
    numarcpts=numsel+1; /* numarcpts--number of arc points; [0] to [numsel] */
    if (hypot(x2-x1,y1-y2)<0.5*GRID) { /* zero area after AlignGRID() */
        numarcpts=0;
        if (debug>=2) printf("qdrim.c ArcSect() NO ARC (ZERO AREA)!\n");
        return(false);
    } /* hypot(x2-x1,y2-y1)=PntDist(x1,y1,x2,y2); */

    thetaInc=(theta1-theta2)/(double)numsel; /* theta increase step */
    /* scan ZONE; thetaZONE=thetaInc*FZONE in previous versions */
    /* (Notes20081210Deng R*theta>sqrt(2)*GRID to cover adjacent grid */
    R=mini(Rx,Ry); /* minimum */
    thetaZONE=9*GRID/R; /*3*2*sqrt(2)*GRID/R; 2*GRID&min(R) for minimum scan */

    /* AlignGRID(x or y)&AlignGRID(X or Y) not same for off-GRID (OX,OY) */
    ArcPtsX[0]=AlignGRID(x1+OX);
    ArcPtsY[0]=AlignGRID(y1+OY);
    ArcPtsX[numsel]=AlignGRID(x2+OX);
    ArcPtsY[numsel]=AlignGRID(y2+OY);
    /* last point theta and rdist */
    thetardist(&endtheta,&endrdist,ArcPtsX[numsel]-OX,ArcPtsY[numsel]-OY,Rx,Ry);
    for ( i=1,thetai=theta1-thetaInc; i<numsel; i++,thetai-=thetaInc ) {
        /* or below for(): thetai=theta1-thetaInc*i; /* angle direction */
        Ri=arcQ1R(arcorder,thetai,Rx,Ry); /* Linear & Elliptical R=R(theta) */
        xi=Ri*cos(thetai); yi=Ri*sin(thetai); /* Polar2XY() */
        ArcPtsX[i]=AlignGRID(xi+OX);
        ArcPtsY[i]=AlignGRID(yi+OY); /* initial (accurate) coordinates */
        
        /* CONTINUE to minimize steering angle (STOP HERE for minimum rdist) */
        ddist=DZONE; /* to limit radial error */
        /* target rdist for elliptical R=R(theta); OR minimize rdist; */
        if (arcorder==2) {targetrdist=getrdist(xi,yi,Rx,Ry);}
        /* previous point theta and rdist */
        thetardist(&prevtheta,&prevrdist,ArcPtsX[i-1]-OX,ArcPtsY[i-1]-OY,Rx,Ry);
        /* 20080410 seperate thetai cases to minimize error */
        if (thetai>PI_4) { /* between (pi/4,pi/2), x-range larger */
            theta=thetai+thetaZONE;
            if (theta>theta1) upx=ArcPtsX[0]-OX; /* boundary */
            else {R=arcQ1R(arcorder,theta,Rx,Ry); upx=AlignGRID(R*cos(theta)+OX)-OX;}
            theta=thetai-thetaZONE;
            if (theta<theta2) downx=ArcPtsX[numsel]-OX; /* boundary */
            else {R=arcQ1R(arcorder,theta,Rx,Ry); downx=AlignGRID(R*cos(theta)+OX)-OX;}
            for (x=upx;x<=downx;x+=GRID) { /* (X,Y)=(x+OX,y+OY) on-GRID */
                yacc=arcQ1y(arcorder,Rx,Ry,x); /*yacc--yaccurate*/
                theta=atan2(yacc,x);
                /* target rdist for linear R=R(theta); OR minimize steering angle; */
                if (arcorder==1) targetrdist=prevrdist+(endrdist-prevrdist)*(prevtheta-theta)/(prevtheta-endtheta);
                y=AlignGRID(yacc+OY)-OY;
                dist1=fabs(getrdist(x,y,Rx,Ry)-targetrdist);
                y+=GRID; /* y=y+GRID; */
                dist2=fabs(getrdist(x,y,Rx,Ry)-targetrdist);
                if (dist1<dist2) {dist=dist1; y-=GRID;} else dist=dist2;
                if (dist<ddist) {ArcDist[i]=ddist=dist; ArcPtsX[i]=x+OX; ArcPtsY[i]=y+OY;}
            } /* search minimum ddist */
        } else { /* between (0,pi/4), y-range larger */
            theta=thetai+thetaZONE;
            if (theta>theta1) upy=ArcPtsY[0]-OY; /* boundary */
            else {R=arcQ1R(arcorder,theta,Rx,Ry); upy=AlignGRID(R*sin(theta)+OY)-OY;}
            theta=thetai-thetaZONE;
            if (theta<theta2) downy=ArcPtsY[numsel]-OY; /* boundary */
            else {R=arcQ1R(arcorder,theta,Rx,Ry); downy=AlignGRID(R*sin(theta)+OY)-OY;}
            for (y=upy;y>=downy;y-=GRID) { /* (X,Y)=(x+OX,y+OY) on-GRID */
                xacc=arcQ1x(arcorder,Rx,Ry,y); /*xacc--xaccurate*/
                theta=atan2(y,xacc);
                /* target rdist for linear R=R(theta); OR minimize steering angle; */
                if (arcorder==1) targetrdist=prevrdist+(endrdist-prevrdist)*(prevtheta-theta)/(prevtheta-endtheta);
                x=AlignGRID(xacc+OX)-OX;
                dist1=fabs(getrdist(x,y,Rx,Ry)-targetrdist);
                x+=GRID; /* x=x+GRID; */
                dist2=fabs(getrdist(x,y,Rx,Ry)-targetrdist);
                if (dist1<dist2) {dist=dist1; x-=GRID;} else dist=dist2;
                if (dist<ddist) {ddist=dist; ArcPtsX[i]=x+OX; ArcPtsY[i]=y+OY;}
            } /* search minimum ddist */
        }
    }

    /* quality statistics */
    for (i=0;i<numarcpts;i++) { /* from [0] to [numsel] */
        ArcDist[i]=getrdist(ArcPtsX[i]-OX,ArcPtsY[i]-OY,Rx,Ry); /* sign + or - */
    }

    /* reverse sequence */
    if (xs>xe) revseqxyz(ArcPtsX,ArcPtsY,ArcDist,numarcpts);
    /* revseqxy(ArcPtsX,ArcPtsY,numarcpts); revseq(ArcDist,numarcpts); */

    return(true);
}

/* (inner_TopY(), outer_BottomY(), inner_RightX(), outer_LeftX(), arcpie();
 * inner/outer edge (arc1/arc2) centered at (Org1X,Org1Y)/(Org2X,Org2Y);
 * (Xs,Xe) and (Ys,Ye)--Global coordinate X and Y of (start,end) points;
 * Quadrant I arc, Xs<Xe and Ys>Ye for X-increment scan!
 * Global (numarcpts, ArcPtsX, ArcPtsY, ArcDist) modified by ArcSect();
 * ARC Points already aligned to ResGRID!
 * fabs()<EPS to eliminate precision error 20080924;
 */
bool inner_TopY(double TopY, double Xs, double Xe)
{ /* inner arc section with TopY line */
    double Ys, Ye; /* Quadrant I (Xs<Xe,Ys>Ye) */

    if (!ArcSect(Org1X,Org1Y,R1x,R1y,Xs-Org1X,Xe-Org1X,arcres1)) return(false);
    StatARC1(); /* ArcSect()-->(numarcpts, ArcPtsX, ArcPtsY, ArcDist) */

    /* input (TopY,Xs,Xe) may not on ResGRID for boundary points */
    TopY=AlignGRID(TopY); Xs=ArcPtsX[0]; Xe=ArcPtsX[numarcpts-1];
    Ys=ArcPtsY[0]; Ye=ArcPtsY[numarcpts-1]; /* on ResGRID */
    if ((fabs(Xs-Xe)<EPS)||((fabs(Ys-Ye)<EPS)&&(fabs(TopY-Ys)<EPS))) {
        if (debug>=2) {
            printf("qdrim.c inner_TopY() ZERO AREA!\n");
            printf("(Xs,Ys)=(%.3f,%.3f),(Xe,Ye)=(%.3f,%.3f),TopY=%.3f\n",Xs,Ys,Xe,Ye,TopY);
        }
        return(false);
    } /* ((Xs==Xe)||((Ys==Ye)&&(TopY==Ys))) */

    if (numarcpts<NMAXPNTS) { /* one section; 197+2 points okay; */
        if (TopY>Ys) {
            ArcPtsX[numarcpts]=Xe; ArcPtsY[numarcpts]=TopY;
            ArcPtsX[numarcpts+1]=Xs; ArcPtsY[numarcpts+1]=TopY;
            AddPOLY(ArcPtsX,ArcPtsY,numarcpts+2,POLYSX,POLYSY,&Npoly);
        } else {
            ArcPtsX[numarcpts]=Xe; ArcPtsY[numarcpts]=Ys;
            AddPOLY(ArcPtsX,ArcPtsY,numarcpts+1,POLYSX,POLYSY,&Npoly);
        }
    } else { /* multiple sections */
        AddBOX(Xs,Ys,Xe,TopY,POLYSX,POLYSY,&Npoly);
        AddPIE(ArcPtsX,ArcPtsY,numarcpts,Xe,Ys,POLYSX,POLYSY,&Npoly);
    }
    return(true);
}

bool outer_BottomY(double BottomY, double Xs, double Xe)
{ /* outer arc section with BottomY line */
    double Ys, Ye; /* Quadrant I (Xs<Xe,Ys>Ye) */

    if (!ArcSect(Org2X,Org2Y,R2x,R2y,Xs-Org2X,Xe-Org2X,arcres2)) return(false);
    StatARC2(); /* ArcSect()-->(numarcpts, ArcPtsX, ArcPtsY, ArcDist) */

    /* input (BottomY,Xs,Xe) may not on ResGRID for boundary points */
    BottomY=AlignGRID(BottomY); Xs=ArcPtsX[0]; Xe=ArcPtsX[numarcpts-1];
    Ys=ArcPtsY[0]; Ye=ArcPtsY[numarcpts-1]; /* on ResGRID */
    if ((fabs(Xs-Xe)<EPS)||((fabs(Ys-Ye)<EPS)&&(fabs(BottomY-Ye)<EPS))) {
        if (debug>=2) {
            printf("qdrim.c outer_BottomY() ZERO AREA!\n");
            printf("(Xs,Ys)=(%.3f,%.3f),(Xe,Ye)=(%.3f,%.3f),BottomY=%.3f\n",Xs,Ys,Xe,Ye,BottomY);
        }
        return(false);
    } /* ((Xs==Xe)||((Ys==Ye)&&(BottomY==Ye))) */

    if (numarcpts<NMAXPNTS) { /* one section; 197+2 points okay; */
        if (BottomY<Ye) {
            ArcPtsX[numarcpts]=Xe; ArcPtsY[numarcpts]=BottomY;
            ArcPtsX[numarcpts+1]=Xs; ArcPtsY[numarcpts+1]=BottomY;
            AddPOLY(ArcPtsX,ArcPtsY,numarcpts+2,POLYSX,POLYSY,&Npoly);
        } else {
            ArcPtsX[numarcpts]=Xs; ArcPtsY[numarcpts]=Ye;
            AddPOLY(ArcPtsX,ArcPtsY,numarcpts+1,POLYSX,POLYSY,&Npoly);
        }
    } else { /* multiple sections */
        AddBOX(Xs,BottomY,Xe,Ye,POLYSX,POLYSY,&Npoly);
        AddPIE(ArcPtsX,ArcPtsY,numarcpts,Xs,Ye,POLYSX,POLYSY,&Npoly);
    }
    return(true);
}

bool inner_RightX(double RightX, double Ys, double Ye)
{ /* inner arc section with RightX line */
    double xs, xe, Xs, Xe; /* Quadrant I (Xs<Xe,Ys>Ye) */

    xs=arcQ1x(arcorder,R1x,R1y,Ys-Org1Y);
    xe=arcQ1x(arcorder,R1x,R1y,Ye-Org1Y);
    if (!ArcSect(Org1X,Org1Y,R1x,R1y,xs,xe,arcres1)) return(false);
    StatARC1(); /* ArcSect()-->(numarcpts, ArcPtsX, ArcPtsY, ArcDist) */

    /* input (RightX,Ys,Ye) may not on ResGRID for boundary points */
    RightX=AlignGRID(RightX); Ys=ArcPtsY[0]; Ye=ArcPtsY[numarcpts-1];
    Xs=ArcPtsX[0]; Xe=ArcPtsX[numarcpts-1]; /* on ResGRID */
    if ((fabs(Ys-Ye)<EPS)||((fabs(Xs-Xe)<EPS)&&(fabs(RightX-Xe)<EPS))) {
        if (debug>=2) {
            printf("qdrim.c inner_RightX() ZERO AREA!\n");
            printf("(Xs,Ys)=(%.3f,%.3f),(Xe,Ye)=(%.3f,%.3f),RightX=%.3f\n",Xs,Ys,Xe,Ye,RightX);
        }
        return(false);
    } /* ((Ys==Ye)||((Xs==Xe)&&(RightX==Xe))) */

    if (numarcpts<NMAXPNTS) { /* one section; 197+2 points okay; */
        if (RightX>Xe) {
            ArcPtsX[numarcpts]=RightX;   ArcPtsY[numarcpts]=Ye;
            ArcPtsX[numarcpts+1]=RightX; ArcPtsY[numarcpts+1]=Ys;
            AddPOLY(ArcPtsX,ArcPtsY,numarcpts+2,POLYSX,POLYSY,&Npoly);
        } else {
            ArcPtsX[numarcpts]=Xe; ArcPtsY[numarcpts]=Ys;
            AddPOLY(ArcPtsX,ArcPtsY,numarcpts+1,POLYSX,POLYSY,&Npoly);
        }
    } else { /* multiple sections */
        AddBOX(Xe,Ye,RightX,Ys,POLYSX,POLYSY,&Npoly);
        AddPIE(ArcPtsX,ArcPtsY,numarcpts,Xe,Ys,POLYSX,POLYSY,&Npoly);
    }
    return(true);
}

bool outer_LeftX(double LeftX, double Ys, double Ye)
{ /* outer arc section with LeftX line */
    double xs, xe, Xs, Xe; /* Quadrant I (Xs<Xe,Ys>Ye) */

    xs=arcQ1x(arcorder,R2x,R2y,Ys-Org2Y);
    xe=arcQ1x(arcorder,R2x,R2y,Ye-Org2Y);
    if (!ArcSect(Org2X,Org2Y,R2x,R2y,xs,xe,arcres2)) return(false);
    StatARC2(); /* ArcSect()-->(numarcpts, ArcPtsX, ArcPtsY, ArcDist) */

    /* input (LeftX,Ys,Ye) may not on ResGRID for boundary points */
    LeftX=AlignGRID(LeftX); Ys=ArcPtsY[0]; Ye=ArcPtsY[numarcpts-1];
    Xs=ArcPtsX[0]; Xe=ArcPtsX[numarcpts-1]; /* on ResGRID */
    if ((fabs(Ys-Ye)<EPS)||((fabs(Xs-Xe)<EPS)&&(fabs(LeftX-Xs)<EPS))) {
        if (debug>=2) {
            printf("qdrim.c outer_LeftX() ZERO AREA!\n");
            printf("(Xs,Ys)=(%.3f,%.3f),(Xe,Ye)=(%.3f,%.3f),LeftX=%.3f\n",Xs,Ys,Xe,Ye,LeftX);
        }
        return(false);
    } /* ((Ys==Ye)||((Xs==Xe)&&(LeftX==Xs))) */

    if (numarcpts<NMAXPNTS) { /* one section; 197+2 points okay; */
        if (LeftX<Xs) {
            ArcPtsX[numarcpts]=LeftX;   ArcPtsY[numarcpts]=Ye;
            ArcPtsX[numarcpts+1]=LeftX; ArcPtsY[numarcpts+1]=Ys;
            AddPOLY(ArcPtsX,ArcPtsY,numarcpts+2,POLYSX,POLYSY,&Npoly);
        } else {
            ArcPtsX[numarcpts]=Xs; ArcPtsY[numarcpts]=Ye;
            AddPOLY(ArcPtsX,ArcPtsY,numarcpts+1,POLYSX,POLYSY,&Npoly);
        }
    } else { /* multiple sections */
        AddBOX(Xs,Ys,LeftX,Ye,POLYSX,POLYSY,&Npoly);
        AddPIE(ArcPtsX,ArcPtsY,numarcpts,Xs,Ye,POLYSX,POLYSY,&Npoly);
    }
    return(true);
}

/* 20090120 combine inner_pie()&outer_pie(); Notes20090119Deng edge direction;
 * 20080925/26 Pie shape splices; refine with MASKGRID; ONLY ARC IMPORTANT! */
bool arcpie(unsigned int arcn, double Xs, double Ys, double Xe, double Ye, double Xv, double Yv)
{ /* arcn (1--arc1; 2--arc2; Quadrant I); if Xs<Xe, +x scan; else -x scan; */
    double OX, OY, Rx, Ry, arcres; /* orgin/radius/resolution */
    double UpY, DownY, DivX; /* scan */
    unsigned int idxpcs=1, oddeven; double x0, x1, x2; /* alternate direction */

    /* distinguishable triangle minimum area S0 = GRID/2*GRID/2*0.5 (= 1.25e-7); */
    if (TriArea(Xs,Ys,Xe,Ye,Xv,Yv)<S0) { /* see icedpoly.c */
        if (debug>=2) {
            printf("qdrim.c arcpie() ZERO AREA!\n");
            printf("(Xs,Ys)=(%.3f,%.3f),(Xe,Ye)=(%.3f,%.3f),(Xv,Yv)=(%.3f,%.3f)\n",Xs,Ys,Xe,Ye,Xv,Yv);
        } /* zero area after AlignGRID() */
        return(false);
    }

    if (arcn==1) {OX=Org1X; OY=Org1Y; Rx=R1x; Ry=R1y; arcres=arcres1;} /* arc1 */
    else {OX=Org2X; OY=Org2Y; Rx=R2x; Ry=R2y; arcres=arcres2;} /* arc2 */
    /* even(0)/odd(1) OPPOSITE for inner/outer_pie() */
    oddeven=(TriArea(OX,OY,Xs,Ys,Xe,Ye)>=(TriArea(OX,OY,Xs,Ys,Xv,Yv)+TriArea(OX,OY,Xv,Yv,Xe,Ye)));

    if (Xs<Xe) { /* Xs<Xe; +x direction scan */
        CurX=Xs; UpY=Ys;
        do{
            NextX=IncMaskGrid(CurX); /* next Mask Grid X */
            if (NextX>Xe) NextX=Xe;
            DownY=DecMaskGrid(UpY); /* next Mask Grid Y */
            if (DownY<Ye) {DownY=Ye; DivX=Xe;}
            else DivX=arcQ1x(arcorder,Rx,Ry,DownY-OY)+OX;
            if ((DivX>CurX)&&(DivX<NextX)) NextX=DivX;
            if (CurX<NextX) {
                x1=CurX-OX; x2=NextX-OX; /* x1<x2 */
                /* alternate to make common interface same direction */
                if (arcn==1) {if (idxpcs%2!=oddeven) {x0=x1; x1=x2; x2=x0;}}
                if (arcn==2) {if (idxpcs%2==oddeven) {x0=x1; x1=x2; x2=x0;}}
                if (ArcSect(OX,OY,Rx,Ry,x1,x2,arcres)) {
                    /* ArcSect()-->(numarcpts, ArcPtsX, ArcPtsY, ArcDist) */
                    (arcn==1)? StatARC1() : StatARC2() ;
                    if (numarcpts<NMAXPNTS) { /* one section */
                        ArcPtsX[numarcpts]=Xv; ArcPtsY[numarcpts]=Yv; /* pie vertex */
                        AddPOLY(ArcPtsX,ArcPtsY,numarcpts+1,POLYSX,POLYSY,&Npoly);
                        idxpcs++; /* index for next piece */
                    } else { /* multiple sections */
                        AddPIE(ArcPtsX,ArcPtsY,numarcpts,Xv,Yv,POLYSX,POLYSY,&Npoly);
                    }
                }
            }
            CurX=NextX;
            UpY=arcQ1y(arcorder,Rx,Ry,CurX-OX)+OY;
        } while (NextX<Xe);
    } else { /* Xs>Xe; -x direction scan */
        CurX=Xs; DownY=Ys;
        do{
            NextX=DecMaskGrid(CurX); /* next Mask Grid X */
            if (NextX<Xe) NextX=Xe;
            UpY=IncMaskGrid(DownY); /* next Mask Grid Y */
            if (UpY>Ye) {UpY=Ye; DivX=Xe;}
            else DivX=arcQ1x(arcorder,Rx,Ry,UpY-OY)+OX;
            if ((DivX<CurX)&&(DivX>NextX)) NextX=DivX;
            if (NextX<CurX) {
                x1=NextX-OX; x2=CurX-OX; /* x1<x2 */
                /* alternate to make common interface same direction */
                if (arcn==1) {if (idxpcs%2==oddeven) {x0=x1; x1=x2; x2=x0;}}
                if (arcn==2) {if (idxpcs%2!=oddeven) {x0=x1; x1=x2; x2=x0;}}
                if (ArcSect(OX,OY,Rx,Ry,x1,x2,arcres)) {
                    /* ArcSect()-->(numarcpts, ArcPtsX, ArcPtsY, ArcDist) */
                    (arcn==1)? StatARC1() : StatARC2() ;
                    if (numarcpts<NMAXPNTS) { /* one section */
                        ArcPtsX[numarcpts]=Xv; ArcPtsY[numarcpts]=Yv; /* pie vertex */
                        AddPOLY(ArcPtsX,ArcPtsY,numarcpts+1,POLYSX,POLYSY,&Npoly);
                        idxpcs++; /* index for next piece */
                    } else { /* multiple sections */
                        AddPIE(ArcPtsX,ArcPtsY,numarcpts,Xv,Yv,POLYSX,POLYSY,&Npoly);
                    }
                }
            }
            CurX=NextX;
            DownY=arcQ1y(arcorder,Rx,Ry,CurX-OX)+OY;
        } while (NextX>Xe);
    }

    return(true);
}

/* Monotone Trapezoidalization (Partitioning) with horizontal sweep lines */
/* O'Rourke1998 Chapter 2; http://en.wikipedia.org/wiki/Monotone_polygon */
/* both edges snapping along Y direction (for X-increment scan in Quadrant I). */
/* Y-sweep line scan: for each Y, search on-GRID X1 and X2 simutaneously. */
bool monotoneY(double Ys, double Ye) /* 20111112 renamed from both_edgeY(); */
{ /* (20120226 Version) inner/outer edge center (Org1X,Org1Y)/(Org2X,Org2Y); */
    unsigned int i, numsel=1, numsel1, numsel2; /* 1/2:arc1/arc2; */
    unsigned int N1s, N2s, N1e, N2e, N1i, N2i; /* indexing to trace ARC1/ARC2 */
    double Yt, Yb; /* t/b:top/bottom; */
    double y1s, y1e, y2s, y2e, x1s, x1e, x2s, x2e; /* 1/2:arc1/2; s/e:start/end; */
    double UpY, DownY, Y, y, x, x1, x2, Yi, x1i, x2i, R, R1i; /* scan&snap */
    double theta1, theta2, thetaInc, thetaZONE, theta, thetai;
    double ddist, dist, dist1, dist2; /* radial error */
    double prevrdist1, endrdist1, prevtheta1, endtheta1, targetrdist1;
    double prevrdist2, endrdist2, prevtheta2, endtheta2, targetrdist2;
    double DZONE=2*GRID*GRID; /* initial zone for point snapping */

    if (fabs(Ys-Ye)<0.5*GRID) { /* zero area after AlignGRID() */
        numarcpts=0;
        if (debug>=2) printf("qdrim.c monotoneY() NO ARC (ZERO AREA)!\n");
        return(false);
    }
    /* (Ys,Ye)--(start,end) global Y; LET local y: 0<=yb<yt<=Ry, otherwise numsel<0; */
    if (Ys>Ye) {Yt=Ys; Yb=Ye;} else {Yt=Ye; Yb=Ys;}
    y1s=Yt-Org1Y; y1e=Yb-Org1Y; y2s=Yt-Org2Y; y2e=Yb-Org2Y;
    x1s=arcQ1x(arcorder,R1x,R1y,y1s); x1e=arcQ1x(arcorder,R1x,R1y,y1e);
    x2s=arcQ1x(arcorder,R2x,R2y,y2s); x2e=arcQ1x(arcorder,R2x,R2y,y2e);
    theta1=atan2(y1s,x1s); theta2=atan2(y1e,x1e);
    numsel1=(unsigned int)ceil((theta1-theta2)/arcres1);
    numsel2=(unsigned int)ceil((atan2(y2s,x2s)-atan2(y2e,x2e))/arcres2);
    switch (arcedge) { /* arcedge 20101116; REF.: arbrim.c v1.10 */
        case -1:
            numsel = numsel1; break; /* inner arc important */
        case 1:
            numsel = numsel2; break; /* outer arc important */
        default: /* both arc important */
            numsel = maxi(numsel1,numsel2); /* maximum; */
            /* Average: numsel=(unsigned int)(0.5*(numsel1+numsel2)); */
            break;
    }
    if (numsel<1) numsel=1; /* numsel--number of selected subsections */
    numarcpts=numsel+1; /* numarcpts--number of arc points; [0] to [numsel] */

    N1s=Narc1; N2s=Narc2; /* start index [0] to traces (ARC1X,ARC1Y)&(ARC2X,ARC2Y) */
    N1e=Narc1+numsel; N2e=Narc2+numsel; /* end index [numsel] to traces ARC1&ARC2 */

    thetaInc=(theta1-theta2)/(double)numsel; /* theta increase step */
    /* scan ZONE; thetaZONE=thetaInc*FZONE*2 in previous versions */
    /* (Notes20081210Deng R*theta>sqrt(2)*GRID to cover adjacent grid */
    R=mini(R1x,R1y); /* minimum */
    thetaZONE=18*GRID/R; /*6*2*sqrt(2)*GRID/R; 2*GRID&min(R) for minimum scan */

    /* AlignGRID(x or y)&AlignGRID(X or Y) not same for off-GRID (OX,OY) */
    ARC1Y[N1s]=ARC2Y[N2s]=AlignGRID(Yt);
    ARC1X[N1s]=AlignGRID(x1s+Org1X);
    ARC2X[N2s]=AlignGRID(x2s+Org2X);
    ARC1Y[N1e]=ARC2Y[N2e]=AlignGRID(Yb);
    ARC1X[N1e]=AlignGRID(x1e+Org1X);
    ARC2X[N2e]=AlignGRID(x2e+Org2X);
    /* y-range larger for more vertical situation (Ref. ArcSect()) */
    /* last point theta and rdist */
    thetardist(&endtheta1,&endrdist1,ARC1X[N1e]-Org1X,ARC1Y[N1e]-Org1Y,R1x,R1y);
    thetardist(&endtheta2,&endrdist2,ARC2X[N2e]-Org2X,ARC2Y[N2e]-Org2Y,R2x,R2y);
    for ( i=1,thetai=theta1-thetaInc; i<numsel; i++, thetai-=thetaInc ) {
        /* or below for(): thetai=theta1-thetaInc*i; /* angle direction */
        N1i=N1s+i; N2i=N2s+i; /* indexing to traces (ARC1X,ARC1Y)&(ARC2X,ARC2Y) */
        R1i=arcQ1R(arcorder,thetai,R1x,R1y); /* Linear & Elliptical R=R(theta) */
        x1i=R1i*cos(thetai); /* Polar2XY() */
        Yi=Org1Y+R1i*sin(thetai); /* global coordinate */
        x2i=arcQ1x(arcorder,R2x,R2y,Yi-Org2Y); /* local coordinate */
        ARC1X[N1i]=AlignGRID(x1i+Org1X);
        ARC2X[N2i]=AlignGRID(x2i+Org2X);
        ARC1Y[N1i]=ARC2Y[N2i]=AlignGRID(Yi); /* initial (accurate) coordinates */
        
        /* CONTINUE to minimize steering angle (STOP HERE for minimum rdist) */
        ddist=DZONE; /* to limit radial error */
        /* target rdist for elliptical R=R(theta); OR minimize rdist; */
        if (arcorder==2) {
            targetrdist1=getrdist(x1i,Yi-Org1Y,R1x,R1y);
            targetrdist2=getrdist(x2i,Yi-Org2Y,R2x,R2y);
        }
        /* previous point theta and rdist */
        thetardist(&prevtheta1,&prevrdist1,ARC1X[N1i-1]-Org1X,ARC1Y[N1i-1]-Org1Y,R1x,R1y);
        thetardist(&prevtheta2,&prevrdist2,ARC2X[N2i-1]-Org2X,ARC2Y[N2i-1]-Org2Y,R2x,R2y);
        /* scan range */
        theta=thetai+thetaZONE; R=arcQ1R(arcorder,theta,R1x,R1y);
        UpY=AlignGRID(R*sin(theta)+Org1Y);
        theta=thetai-thetaZONE; R=arcQ1R(arcorder,theta,R1x,R1y);
        DownY=AlignGRID(R*sin(theta)+Org1Y);
        for (Y=UpY;Y>=DownY;Y-=GRID) {
            dist=0; /* initial value */
            /* inner edge */
            y=Y-Org1Y; x=arcQ1x(arcorder,R1x,R1y,y); /* x&y--accurate */
            theta=atan2(y,x);
            /* target rdist for linear R=R(theta); OR minimize steering angle; */
            if (arcorder==1) targetrdist1=prevrdist1+(endrdist1-prevrdist1)*(prevtheta1-theta)/(prevtheta1-endtheta1);
            x1=AlignGRID(x+Org1X)-Org1X;
            dist1=fabs(getrdist(x1,y,R1x,R1y)-targetrdist1);
            x1+=GRID;
            dist2=fabs(getrdist(x1,y,R1x,R1y)-targetrdist1);
            if (dist1<dist2) {dist+=dist1*dist1; x1-=GRID;}
            else dist+=dist2*dist2;
            /* outer edge */
            y=Y-Org2Y; x=arcQ1x(arcorder,R2x,R2y,y); /* x&y--accurate */
            theta=atan2(y,x);
            /* target rdist for linear R=R(theta); OR minimize steering angle; */
            if (arcorder==1) targetrdist2=prevrdist2+(endrdist2-prevrdist2)*(prevtheta2-theta)/(prevtheta2-endtheta2);
            x2=AlignGRID(x+Org2X)-Org2X;
            dist1=fabs(getrdist(x2,y,R2x,R2y)-targetrdist2);
            x2+=GRID;
            dist2=fabs(getrdist(x2,y,R2x,R2y)-targetrdist2);
            if (dist1<dist2) {dist+=dist1*dist1; x2-=GRID;}
            else dist+=dist2*dist2;
            if (dist<ddist) {
                ddist=dist;
                ARC1X[N1i]=x1+Org1X; ARC1Y[N1i]=Y; /* trace1 */
                ARC2X[N2i]=x2+Org2X; ARC2Y[N2i]=Y; /* trace2 */
            } /* search minimum ddist */
        }
    }

    /* quality statistics */
    for (i=0;i<numarcpts;i++) { /* from [0] to [numsel] */
        N1i=N1s+i; N2i=N2s+i;
        Rerr1[N1i]=getrdist(ARC1X[N1i]-Org1X,ARC1Y[N1i]-Org1Y,R1x,R1y);
        Rerr2[N2i]=getrdist(ARC2X[N2i]-Org2X,ARC2Y[N2i]-Org2Y,R2x,R2y);
    } /* sign + or - */

    /* split (oversized) DouBLe edges to <NMAXPNTS in output polygon */
    AddDBL(&ARC1Y[N1s],&ARC1X[N1s],&ARC2X[N2s],numarcpts,NMAXPNTS,
           POLYSX,POLYSY,&Npoly); /* 20110630 comgeom.h AddDBL() */

    /* UPDATE COUNTER (important) */
    Narc1+=numarcpts; Narc2+=numarcpts;
    return(true);
}

/* draw whole piece (NO fracturing, split oversized polygons) (GDSII compliant) */
void drawWhole(unsigned int N1, unsigned int N2)
{
    Npoly=0; /* OVERRIDE special cuts from irregular shapes (if any) */
    /* arc traces */
    arcPnts(ARC1X,ARC1Y,N1,arcorder,angaxis,O1X,O1Y,H1X,H1Y,L1X,L1Y);
    arcPnts(ARC2X,ARC2Y,N2,arcorder,angaxis,O2X,O2Y,H2X,H2Y,L2X,L2Y);
    /* add both edge to output polygon (GDSII compliant) with statistics */
    align2stat(ARC1X,ARC1Y,N1,GRID,Rerr1);
    align2stat(ARC2X,ARC2Y,N2,GRID,Rerr2);
    AddBOTH(ARC1X,ARC1Y,N1,ARC2X,ARC2Y,N2,NMAXPNTS,POLYSX,POLYSY,&Npoly);
    Narc1=N1; Narc2=N2; /* UPDATE COUNTER */
    return;
}

/* fracture regular piece with X axis incremental search */
void drawRegularX(void)
{ /* judge CONDITION CurDownY<=NextUpY for center portion */
    double UpY, DownY, DivY1, DivY2;

    if (CurDownY<NextUpY) {DivY1=CurDownY; DivY2=NextUpY;}
    else {DivY1=NextUpY; DivY2=CurDownY;}

    /* inner edge */
    if (NextDownY<NextUpY) { /* otherwise outside arc */
        UpY=DivY1;
        do {
            DownY=DecMaskGrid(UpY);
            if (DownY<NextDownY) DownY=NextDownY;
            if (UpY>DownY) inner_RightX(NextX,UpY,DownY);
            UpY=DownY;
        } while (DownY>NextDownY);
    }

    /* outer edge */
    UpY=CurUpY;
    do {
        DownY=DecMaskGrid(UpY);
        if (DownY<DivY2) DownY=DivY2;
        if (UpY>DownY) outer_LeftX(CurX,UpY,DownY);
        UpY=DownY;
    } while (DownY>DivY2);

    /* middle section (BOX or monotoneY) */
    if (CurDownY<NextUpY) { /* BOX */
        AddBOX(CurX,DivY1,NextX,DivY2,POLYSX,POLYSY,&Npoly);
    } else if (CurDownY>NextUpY) {
        UpY=CurDownY;
        do {
            DownY=DecMaskGrid(UpY);
            if (DownY<NextUpY) DownY=NextUpY;
            if (UpY>DownY) monotoneY(UpY,DownY);
            UpY=DownY;
        } while (DownY>NextUpY);
    } else return; /* (CurDownY==NextUpY) no middle section */

    return;
}

/* First Piece Cut. ONLY ARC IMPORTANT. Henghua20080320to0416Notes */
/* 20090120 edge direction; 20080926 Simplify qd1rim.c v2.70 drawFirstX() */
void drawFirstPie(void)
{
    double Y;

    if (PosH1X<PosH2X) {
        if (debug>=2) printf("FirstInnerPie!\n");
        Y=arcQ1y(arcorder,R1x,R1y,PosH2X-Org1X)+Org1Y;
        arcpie(1,PosH1X,PosH1Y,PosH2X,Y,PosH2X,PosH2Y);
    } else if (PosH1X>PosH2X) {
        if (debug>=2) printf("FirstOuterPie!\n");
        Y=arcQ1y(arcorder,R2x,R2y,PosH1X-Org2X)+Org2Y;
        arcpie(2,PosH2X,PosH2Y,PosH1X,Y,PosH1X,PosH1Y);
    } else {if (debug>=2) printf("FirstTarget PosH1X==PosH2X!\n");}
    return;
}

/* Last Piece Cut. ONLY ARC IMPORTANT. 20080414to15.*/
/* 20090120 edge direction; 20080926 Simplify qd1rim.c v2.70 drawLastY() */
void drawLastPie(void)
{
    double X;

    if (PosL1Y<PosL2Y) {
        if (debug>=2) printf("LastInnerPie!\n");
        X=arcQ1x(arcorder,R1x,R1y,PosL2Y-Org1Y)+Org1X;
        arcpie(1,PosL1X,PosL1Y,X,PosL2Y,PosL2X,PosL2Y);
    } else if (PosL1Y>PosL2Y) {
        if (debug>=2) printf("LastOuterPie!\n");
        X=arcQ1x(arcorder,R2x,R2y,PosL1Y-Org2Y)+Org2X;
        arcpie(2,PosL2X,PosL2Y,X,PosL1Y,PosL1X,PosL1Y);
    } else {if (debug>=2) printf("LastTarget PosL1Y==PosL2Y!\n");}
    return;
}

/* 20080925/26 irregular piece with X axis incremental search; */
/* when BoundaryCut invalid ((PosH2X>PosL1X)||(PosL2Y>PosH1Y)) */
/* ONLY ARC IMPORTANT. Henghua20080411(20080924)Notes; 20090120 edge direction; */
void drawIrregular(void)
{
    if (debug>=2) printf("drawIrregular() after BoundaryCut()! \n");
    if (PosL2Y>PosH1Y){ /* Last Piece cut invalid */
        if (debug>=2) printf("Case Irregular (PosL2 above PosH1)\n");
        arcpie(1,PosL1X,PosL1Y,PosH1X,PosH1Y,PosL2X,PosL2Y);
        arcpie(2,PosH2X,PosH2Y,PosL2X,PosL2Y,PosH1X,PosH1Y);
    } else if (PosH2X>PosL1X) { /* First Piece cut invalid */
        if (debug>=2) printf("Case Irregular (PosH2 right to PosL1)\n");
        arcpie(1,PosH1X,PosH1Y,PosL1X,PosL1Y,PosH2X,PosH2Y);
        arcpie(2,PosL2X,PosL2Y,PosH2X,PosH2Y,PosL1X,PosL1Y);
    } else printf("qdrim.c drawIrregular() must ((PosL2Y>PosH1Y)||(PosH2X>PosL1X))!\n");
    return;
}

/* boundary cut for First Piece or Last Piece treatment */
/* return false if any cut invalid; return true if both cut successful; */
bool BoundaryCut(void)
{
    if (PosH2X>PosL1X) { /* First Piece cut invalid */
        return(false);
    } else { /* First Piece X-Cut */
        if (PosH1X==PosH2X) {StartX=PosH1X; Start1Y=PosH1Y; Start2Y=PosH2Y;}
        else {StartX=maxi(PosH1X,PosH2X); /* maximum */
            Start1Y=arcQ1y(arcorder,R1x,R1y,StartX-Org1X)+Org1Y;
            Start2Y=arcQ1y(arcorder,R2x,R2y,StartX-Org2X)+Org2Y;
        }
    }

    if (PosL2Y>PosH1Y){ /* Last Piece cut invalid */
        return(false);
    } else { /* Last Piece Y-Cut */
        if (PosL1Y==PosL2Y) {StopY=PosL1Y; Stop1X=PosL1X; Stop2X=PosL2X;}
        else { StopY=maxi(PosL1Y,PosL2Y); /* maximum */
            Stop1X=arcQ1x(arcorder,R1x,R1y,StopY-Org1Y)+Org1X;
            Stop2X=arcQ1x(arcorder,R2x,R2y,StopY-Org2Y)+Org2X;
        }
    }

    return(true); /* Both First and Last Pieces cut successful */
}

/* scan Next X MaskGrid for main body */
bool GotoNextXMaskGrid(void)
{
    if (NextX>=Stop2X) return(false); /* outside outer arc */

    CurX=NextX; CurUpY=NextUpY; CurDownY=NextDownY;
    NextX=IncMaskGrid(CurX); /* next Mask Grid X */

    if (NextX>=Stop1X) NextDownY=StopY; /* outside inner arc */
    else NextDownY=arcQ1y(arcorder,R1x,R1y,NextX-Org1X)+Org1Y;
    if (NextX>=Stop2X) NextUpY=StopY; /* outside outer arc */
    else NextUpY=arcQ1y(arcorder,R2x,R2y,NextX-Org2X)+Org2Y;

    return(true);
}

/* fracture arbitrary RIM in Cartesian coordinate Quadrant I (Q1) */
/* input (Org1X/Y,Org2X/Y,PosH1X/Y,PosH2X/Y,PosL1X/Y,PosL2X/Y);
 * output (PolyX,PolyY, rdist1,rdist2, Pool1X,Pool1Y,Pool2X,Pool2Y); */
void q1rim(void)
{ /* Q1RIM.c */
    unsigned int idx;
    double RpH1, RpH2, RpL1, RpL2; /* radii */
    double angpH1, angpH2, angpL1, angpL2, angspan1, angspan2; /* angles */

    /* radii and angles to origin */
    XY2Polar(&RpH1,&angpH1,Org1X,Org1Y,PosH1X,PosH1Y);
    XY2Polar(&RpH2,&angpH2,Org2X,Org2Y,PosH2X,PosH2Y);
    XY2Polar(&RpL1,&angpL1,Org1X,Org1Y,PosL1X,PosL1Y);
    XY2Polar(&RpL2,&angpL2,Org2X,Org2Y,PosL2X,PosL2Y);
    /* Q1RIM must have pi/2>=angH>angL>=0; */
    angspan1=ArcSpan(angpH1,angpL1); /* PntAng() may return 2*pi for -0 angle */
    angspan2=ArcSpan(angpH2,angpL2); /* ArcSpan() range in (0, 2*pi] */
    angpL1=angpH1-angspan1; /* if (angpL1==PI2) angpL1=0; */
    angpL2=angpH2-angspan2; /* if (angpL2==PI2) angpL2=0; */
    /* x radius (at 0-degree) and y radius (at 90-degree); */
    arcQ1RxRy(&R1x,&R1y,arcorder,RpH1,RpL1,angpH1,angpL1);
    arcQ1RxRy(&R2x,&R2y,arcorder,RpH2,RpL2,angpH2,angpL2);

    /* print input parameters */
    if (debug>=1) {
        printf("qdrim.c q1rim() Arc section transformed to Quadrant I:\n");
        printf("  (Org1X,Org1Y)=(%.4f,%.4f),(Org2X,Org2Y)=(%.4f,%.4f),\n", Org1X,Org1Y,Org2X,Org2Y);
        printf("  (RpH1,RpH2)=(%.4f,%.4f),(RpL1,RpL2)=(%.4f,%.4f),\n", RpH1,RpH2,RpL1,RpL2);
        printf("  (R1x,R1y)=(%.4f,%.4f),(R2x,R2y)=(%.4f,%.4f),\n", R1x,R1y,R2x,R2y);
        printf("  (PosH1X,PosH1Y)=(%.4f,%.4f),(PosH2X,PosH2Y)=(%.4f,%.4f),\n", PosH1X,PosH1Y,PosH2X,PosH2Y);
        printf("  (PosL1X,PosL1Y)=(%.4f,%.4f),(PosL2X,PosL2Y)=(%.4f,%.4f).\n", PosL1X,PosL1Y,PosL2X,PosL2Y);
        printf("  (angpH1,angpH2)=(%.4f,%.4f)deg,(angpL1,angpL2)=(%.4f,%.4f)deg,\n",
            angpH1*RAD,angpH2*RAD,angpL1*RAD,angpL2*RAD); /* (1*RAD = 1/DEG) */
        printf("  (angspan1,angspan2)=(%.4f,%.4f)deg.\n", angspan1*RAD,angspan2*RAD);
    } /* mathematically 1*RAD = 1/DEG; BUT large numerical error for 1/DEG */
    /* check parameters */
    if ((angpH1<-EPS)||(angpH1>PI_2+EPS)||(angpH2<-EPS)||(angpH2>PI_2+EPS))
        MsgErr("  qdrim.c q1rim() Start points misplaced (not Quadrant I arc)!\n");
    if ((angpL1<-EPS)||(angpL1>PI_2+EPS)||(angpL2<-EPS)||(angpL2>PI_2+EPS))
        MsgErr("  qdrim.c q1rim() Stop points misplaced (not Quadrant I arc)!\n");
    if ((R1x<=0)||(R1y<=0)||(R2x<=0)||(R2y<=0))
        MsgErr("  qdrim.c q1rim() Radius non-positive!\n");

    /* Q1 local counter REFERENCE value (IMPORTANT) */
    numpoly=Npoly; numarc1=Narc1; numarc2=Narc2;

    /* fracture through loop between current and next X MaskGrid */
    if (BoundaryCut()) { /* regular shapes (qd1rim.c v2.24 and earlier) */
        /* First->Main->Last to let (Pool1X,Pool1Y)&(Pool2X,Pool2Y) pre-sorted */
        drawFirstPie(); /* first piece X-Cut (drawFirstX() in qd1rim.c v2.70) */
        /* main piece */
        NextX=StartX; NextUpY=Start2Y; NextDownY=Start1Y;
        while (GotoNextXMaskGrid()) drawRegularX();
        drawLastPie(); /* last piece Y-Cut (drawLastY() in qd1rim.c v2.70) */
    } else drawIrregular();

    /* ACTUAL Q1 local counter (IMPORTANT) */
    numpoly=Npoly-numpoly; numarc1=Narc1-numarc1; numarc2=Narc2-numarc2;
    if (debug>=1) {
        printf("  (numpoly,numarc1,numarc2)=(%d,%d,%d)\n", numpoly,numarc1,numarc2);
    }

    /* SORT trace in Descending Order by angles (clockwise) in Quadrant I (Q1) */
    if (PreSort1) {
        idx=Narc1-numarc1;
        SortAngQ1(&ARC1X[idx],&ARC1Y[idx],numarc1,Org1X,Org1Y);
    }
    if (PreSort2) {
        idx=Narc2-numarc2;
        SortAngQ1(&ARC2X[idx],&ARC2Y[idx],numarc2,Org2X,Org2Y);
    }
    return;
}

/* MAP fracture Quadrant II/III/IV arc from Q1; RESERVE originals & statistics; */
/* global output (POLYSX,POLYSY, Rerr1,Rerr2, ARC1X,ARC1Y,ARC2X,ARC2Y) */
void qkrim(unsigned int Q)
{ /* fqdrim.m fqkrim(); Notes20081215Deng; */
    unsigned int idx, NBSZ=1024; /* memory block buffer size; stdio.h BUFSIZ; */
    double *ptr=NULL; /* pointer */
    double X=0.0, Y=0.0;

    /* MAP fracture Quadrant II/III/IV from Quadrant I; RESERVE originals; */
    if (debug>=1) printf("qdrim.c qkrim(Quadrant %d)\t", Q);
    switch (Q) {
        case 1: /* Q1 original */
            q1rim();
            break;
        case 2: /* Q2->Q1: -conj(x+j*y)=-x+j*y; NEED swap(H1,L1)&swap(H2,L2); */
            Org1X=-Org1X;
            Org2X=-Org2X;
            swap(&PosH1Y,&PosL1Y); X=-PosH1X; PosH1X=-PosL1X; PosL1X=X;
            swap(&PosH2Y,&PosL2Y); X=-PosH2X; PosH2X=-PosL2X; PosL2X=X;
            q1rim();
            swap(&PosH2Y,&PosL2Y); X=-PosH2X; PosH2X=-PosL2X; PosL2X=X;
            swap(&PosH1Y,&PosL1Y); X=-PosH1X; PosH1X=-PosL1X; PosL1X=X;
            Org2X=-Org2X;
            Org1X=-Org1X;
            neg(&POLYSX[Npoly-numpoly],numpoly);
            neg(&ARC1X[Narc1-numarc1],numarc1);
            neg(&ARC2X[Narc2-numarc2],numarc2);
            /* PRE-SORT trace in Descending Order by polar angles (clockwise) */
            if (PreSort1) {idx=Narc1-numarc1; revseqxy(&ARC1X[idx],&ARC1Y[idx],numarc1);}
            if (PreSort2) {idx=Narc2-numarc2; revseqxy(&ARC2X[idx],&ARC2Y[idx],numarc2);}
            break;
        case 3: /* Q3-->Q1: -(x+j*y) = -x-j*y; */
            Org1X=-Org1X;   Org1Y=-Org1Y;
            Org2X=-Org2X;   Org2Y=-Org2Y;
            PosH1X=-PosH1X; PosH1Y=-PosH1Y;
            PosH2X=-PosH2X; PosH2Y=-PosH2Y;
            PosL1X=-PosL1X; PosL1Y=-PosL1Y;
            PosL2X=-PosL2X; PosL2Y=-PosL2Y;
            q1rim();
            PosL2X=-PosL2X; PosL2Y=-PosL2Y;
            PosL1X=-PosL1X; PosL1Y=-PosL1Y;
            PosH2X=-PosH2X; PosH2Y=-PosH2Y;
            PosH1X=-PosH1X; PosH1Y=-PosH1Y;
            Org2X=-Org2X;   Org2Y=-Org2Y;
            Org1X=-Org1X;   Org1Y=-Org1Y;
            idx=Npoly-numpoly; negxy(&POLYSX[idx],&POLYSY[idx],numpoly);
            idx=Narc1-numarc1; negxy(&ARC1X[idx],&ARC1Y[idx],numarc1);
            idx=Narc2-numarc2; negxy(&ARC2X[idx],&ARC2Y[idx],numarc2);
            break;
        case 4: /* Q4->Q1: conj(x+j*y)=x-j*y; NEED swap(H1,L1)&swap(H2,L2); */
            Org1Y=-Org1Y;
            Org2Y=-Org2Y;
            swap(&PosH1X,&PosL1X); Y=-PosH1Y; PosH1Y=-PosL1Y; PosL1Y=Y;
            swap(&PosH2X,&PosL2X); Y=-PosH2Y; PosH2Y=-PosL2Y; PosL2Y=Y;
            q1rim();
            swap(&PosH2X,&PosL2X); Y=-PosH2Y; PosH2Y=-PosL2Y; PosL2Y=Y;
            swap(&PosH1X,&PosL1X); Y=-PosH1Y; PosH1Y=-PosL1Y; PosL1Y=Y;
            Org2Y=-Org2Y;
            Org1Y=-Org1Y;
            neg(&POLYSY[Npoly-numpoly],numpoly);
            neg(&ARC1Y[Narc1-numarc1],numarc1);
            neg(&ARC2Y[Narc2-numarc2],numarc2);
            /* PRE-SORT trace in Descending Order by polar angles (clockwise) */
            if (PreSort1) {idx=Narc1-numarc1; revseqxy(&ARC1X[idx],&ARC1Y[idx],numarc1);}
            if (PreSort2) {idx=Narc2-numarc2; revseqxy(&ARC2X[idx],&ARC2Y[idx],numarc2);}
            break;
        default: /* statistics unchanged; */
            break;
    }

    return;
}

/* DIVIDE arbitrary SINGLE arc PIE in any Quadrant of Cartesian coordinate */
/* global output (POLYSX,POLYSY, Rerr1,Rerr2, ARC1X,ARC1Y,ARC2X,ARC2Y) */
/* Ref.: arcpie() and Q1RIM()+QKRIM() */
bool qkpie(unsigned int arcn, double XH, double YH, double XL, double YL, double Xv, double Yv)
{ /* arcn (1--ARC1; 2--ARC2; any Quadrant); L-->H counterclockwise; */
    unsigned int NBSZ=1024; /* memory block buffer size; stdio.h BUFSIZ; */
    unsigned int i, numsel=1; /* counter */
    double *ptr=NULL; /* pointer */
    double OX, OY, arcres, X, Y; /* ARC */
    double R, RH, RL, Rrate; /* radius */
    double ang, angH, angL, angspan, thetaInc; /* angle */

    /* distinguishable triangle minimum area S0 = GRID/2*GRID/2*0.5 (= 1.25e-7); */
    if (TriArea(XH,YH,XL,YL,Xv,Yv)<S0) { /* see icedpoly.c */
        if (debug>=2) {
            printf("  qdrim.c qkpie() ZERO AREA!\n");
            printf("  (XH,YH)=(%.3f,%.3f),(XL,YL)=(%.3f,%.3f),(Xv,Yv)=(%.3f,%.3f)\n",XH,YH,XL,YL,Xv,Yv);
        } /* zero area after AlignGRID() */
        return(false);
    }

    if (arcn==1) {OX=O1X; OY=O1Y; arcres=arcres1;} /* ARC1 */
    else {OX=O2X; OY=O2Y; arcres=arcres2;} /* ARC2 */

    /* radius and angles to origin */
    XY2Polar(&RH,&angH,OX,OY,XH,YH);
    XY2Polar(&RL,&angL,OX,OY,XL,YL);
    /* L-->H counterclockwise; ArcSpan() range in (0, 2*pi]; */
    angspan=ArcSpan(angH,angL); /* PntAng() may return 2*pi for -0 angle */
    angL=angH-angspan; /* if (angL==PI2) angL=0; */
    Rrate=(RH-RL)/angspan; /* variation rate of radius */

    if (debug>=2) {
        printf("qdrim.c qkpie() Separated arc%d pie section:\n",arcn);
        printf("  (OX,OY)=(%.4f,%.4f),(RH,RL)=(%.4f,%.4f),(angH,angL)=(%.4f,%.4f)deg,\n",OX,OY,RH,RL,angH*RAD,angL*RAD);
        printf("  (XH,YH)=(%.3f,%.3f),(XL,YL)=(%.3f,%.3f),(Xv,Yv)=(%.3f,%.3f).\n",XH,YH,XL,YL,Xv,Yv);
    }

    numsel=(unsigned int)ceil(angspan/arcres);
    if (numsel<1) numsel=1; /* numsel--number of selected subsections */
    thetaInc=(double)(angspan/numsel); /* theta increse step */
    numarcpts=numsel+1; /* numarcpts--number of arc points; [0] to [numsel] */

    ArcPtsX[0]=AlignGRID(XH);
    ArcPtsY[0]=AlignGRID(YH);
    ArcDist[0]=hypot(ArcPtsX[0]-OX,ArcPtsY[0]-OY)-RH;
    /* PntDist(OX,OY,ArcPtsX[0],ArcPtsY[0])-RH; */
    ArcPtsX[numsel]=AlignGRID(XL);
    ArcPtsY[numsel]=AlignGRID(YL);
    ArcDist[numsel]=hypot(ArcPtsX[numsel]-OX,ArcPtsY[numsel]-OY)-RL;
    /* PntDist(OX,OY,ArcPtsX[numsel],ArcPtsY[numsel])-RL; */
    for ( i=1,ang=angH-thetaInc; i<numsel; i++,ang-=thetaInc ){
        /* or below for(): ang=angH-thetaInc*i; /* angle direction */
        R=RH-Rrate*ArcSpan(angH,ang);
        Polar2XY(&X,&Y,OX,OY,R,ang);
        ArcPtsX[i]=AlignGRID(X);
        ArcPtsY[i]=AlignGRID(Y);
        ArcDist[i]=hypot(ArcPtsX[i]-OX,ArcPtsY[i]-OY)-R;
        /* PntDist(OX,OY,ArcPtsX[i],ArcPtsY[i])-R; */
    }
    AddPIE(ArcPtsX,ArcPtsY,numarcpts,Xv,Yv,POLYSX,POLYSY,&Npoly);
    (arcn==1)? StatARC1() : StatARC2() ; /* statistics */

    return(true); /* trace sorted in Descending Order by angles (clockwise) */
}

/* main function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    bool flagFrac=true; /* fracture or not; always snapping & GDSII compliant; */
    unsigned int NBSZ=1024; /* memory block buffer size; stdio.h BUFSIZ */
    unsigned int N1=0, N2=0, NQ=1; /* counters */
    unsigned int dQ, Q1, Q2, QDH1, QDH2, QDL1, QDL2, QDH, QDL; /* Quadrants */
    unsigned int MAXOUTPUT=10240; /* all points (two ARCs + other vertices) */
    unsigned int MAXPNTARC=4096; /* one ARC points (MAXOUTPUT~~2*MAXPNTARC) */
    double *ptr=NULL; /* pointer */
    double DivH1X, DivH1Y, DivH2X, DivH2Y, DivL1X, DivL1Y, DivL2X, DivL2Y; /* boundaries */
    double R, ang, R1, R2, R3, R4, ang1, ang2, ang3, ang4, XH, YH, XL, YL; /* loop variables */
    double RH1, RH2, RL1, RL2, Rrate1, Rrate2; /* radii */
    double AngH1, AngH2, AngL1, AngL2, ArcSpan1, ArcSpan2; /* angles */
    double rdavg[3]={0,0,0}, rdstd[3]={0,0,0}, rdmax[3]={0,0,0}, rdmin[3]={0,0,0};
    /* [0]--arc1+arc2, [1]--arc1, [2]--arc2 */

    /* Check for proper number of arguments */
    if ( (nrhs<6) || (nrhs>12) ) MsgErr("qdrim.c Input arguments incorrect!\n");
    if ( (nrhs<1) || (nlhs>5) ) MsgErr("qdrim.c Output arguments incorrect!\n");
    /* Get input arguments (value=0 when pointer NULL!! 20100714)*/
    /* origin center point 1 */
    ptr = mxGetPr(prhs[0]); O1X = (ptr)? ptr[0] : 0 ;
    ptr = mxGetPi(prhs[0]); O1Y = (ptr)? ptr[0] : 0 ;
    /* origin center point 2 */
    ptr = mxGetPr(prhs[1]); O2X = (ptr)? ptr[0] : 0 ;
    ptr = mxGetPi(prhs[1]); O2Y = (ptr)? ptr[0] : 0 ;
    /* start point 1 and 2 */
    ptr = mxGetPr(prhs[2]); H1X = (ptr)? ptr[0] : 0 ;
    ptr = mxGetPi(prhs[2]); H1Y = (ptr)? ptr[0] : 0 ;
    ptr = mxGetPr(prhs[3]); H2X = (ptr)? ptr[0] : 0 ;
    ptr = mxGetPi(prhs[3]); H2Y = (ptr)? ptr[0] : 0 ;
    /* end point 1 and 2 */
    ptr = mxGetPr(prhs[4]); L1X = (ptr)? ptr[0] : 0 ;
    ptr = mxGetPi(prhs[4]); L1Y = (ptr)? ptr[0] : 0 ;
    ptr = mxGetPr(prhs[5]); L2X = (ptr)? ptr[0] : 0 ;
    ptr = mxGetPi(prhs[5]); L2Y = (ptr)? ptr[0] : 0 ;
    /* arcorder (optional. default=1.) */
    if (nrhs < 7) arcorder = 1; else {ptr = mxGetPr(prhs[6]); arcorder = (int)ptr[0];}
    /* arcres ratio (optional. default=1.) */
    if (nrhs < 8) arcratio = 1.0; else {ptr = mxGetPr(prhs[7]); arcratio = ptr[0];}
    /* arcedge (optional,-1:inner,1:outer,default=0:both) importance of arc edges;
     *   if arcorder=1, argument arcedge prevail (force angaxis=0);
     * angaxis (optional, only when arcorder=2) angle(in radian) of ellipse axis;
     *   if arcorder=2, argument angaxis prevail (force arcedge=0); */
    if (nrhs < 9) { arcedge = 0; angaxis = 0.0; }
    else {
        ptr = mxGetPr(prhs[8]);
        if (arcorder==1) { arcedge = (int)ptr[0]; angaxis = 0.0; }
        else { arcedge = 0; angaxis = ptr[0]; }
    }
    /* GRID (optional. default=0.001) Resolution GRID (default 1nm) */
    if (nrhs < 10) { GRID = 0.001; InvGRID = 1000.0; S0=1.25e-7; }
    else {ptr=mxGetPr(prhs[9]); GRID=ptr[0]; InvGRID=1.0/GRID; S0=GRID*GRID*0.125;}
    /* inverse of GRID: 1/GRID <=> 1*InvGRID; */
    /* distinguishable triangle minimum area S0 = GRID/2*GRID/2*0.5 (= 1.25e-7); */
    /* MASKGRID (optional. default=32.768) MASK fracture GRID */
    if (nrhs < 11) MASKGRID = 32.768; /* (A/D 15bits --> 2^15=32768nm) */
    else {ptr=mxGetPr(prhs[10]); MASKGRID=ptr[0]; flagFrac=(MASKGRID>0);}
    /* debug (optional. default=0.) */
    if (nrhs < 12) debug = 0; else {ptr = mxGetPr(prhs[11]); debug = (int)ptr[0];}
    /* flag to control trace Pre-SORTing */
    if (nlhs > 1) {PreSort1=true; PreSort2=false;}
    if (nlhs > 2) {PreSort1=true; PreSort2=true;}

    /* radii and angles to origin */
    XY2Polar(&RH1,&AngH1,O1X,O1Y,H1X,H1Y);
    XY2Polar(&RH2,&AngH2,O2X,O2Y,H2X,H2Y);
    XY2Polar(&RL1,&AngL1,O1X,O1Y,L1X,L1Y);
    XY2Polar(&RL2,&AngL2,O2X,O2Y,L2X,L2Y);
    ArcSpan1=ArcSpan(AngH1,AngL1); /* PntAng() may return 2*pi for -0 angle */
    ArcSpan2=ArcSpan(AngH2,AngL2); /* ArcSpan() range in (0, 2*pi] */
    if (AngH1<=AngL1) AngL1=AngH1-ArcSpan1;
    if (AngH2<=AngL2) AngL2=AngH2-ArcSpan2;
    Rrate1=(RH1-RL1)/ArcSpan1;
    Rrate2=(RH2-RL2)/ArcSpan2;

    /* minimum arcres (arc angular resolution) (by maximum R) to avoid redundant points */
    R1=maxi(RH1,RL1); arcres1=acos(1-arcratio*GRID/R1); /* arc1 */
    R2=maxi(RH2,RL2); arcres2=acos(1-arcratio*GRID/R2); /* arc2 */

    /* locate the Quadrant of points; NOTE boundaries (+-EPS)!! */
    QDH1=QDI(AngH1-EPS); QDH2=QDI(AngH2-EPS); /* -EPS! (CW) for High points */
    QDL1=QDI(AngL1+EPS); QDL2=QDI(AngL2+EPS); /* +EPS! (CCW) for Low points */

    /* print input parameters */
    if (debug>=1) {
        printf("(GRID,MASKGRID)=(%.4f,%.4f)\n", GRID,MASKGRID);
        printf("(O1X,O1Y)=(%.4f,%.4f),(O2X,O2Y)=(%.4f,%.4f),\n", O1X,O1Y,O2X,O2Y);
        printf("(RH1,RH2)=(%.4f,%.4f),(RL1,RL2)=(%.4f,%.4f),\n", RH1,RH2,RL1,RL2);
        printf("(H1X,H1Y)=(%.4f,%.4f),(H2X,H2Y)=(%.4f,%.4f),\n", H1X,H1Y,H2X,H2Y);
        printf("(L1X,L1Y)=(%.4f,%.4f),(L2X,L2Y)=(%.4f,%.4f).\n", L1X,L1Y,L2X,L2Y);
        /* mathematically 1*RAD = 1/DEG; BUT large numerical error for 1/DEG */
        printf("(AngH1,AngH2)=(%.4f,%.4f)deg,(AngL1,AngL2)=(%.4f,%.4f)deg,\n",
            AngH1*RAD,AngH2*RAD,AngL1*RAD,AngL2*RAD); /* (1*RAD = 1/DEG) */
        printf("(ArcSpan1,ArcSpan2)=(%.4f,%.4f)deg.\n", ArcSpan1*RAD,ArcSpan2*RAD);
        printf("(arcres1,arcres2)=(%.4f,%.4f)deg.\n", arcres1*RAD,arcres2*RAD);
        printf("(QDH1,QDL1)=(%d,%d),(QDH2,QDL2)=(%d,%d).\n", QDH1,QDL1,QDH2,QDL2);
        if (arcorder==2) { /* reuse parameter R1&R2 for ellipse parameter a&b */
            printf("Ellipse Axis Angle angaxis=%.4fdeg,\n",angaxis*RAD);
            Ellipab(&R1,&R2,angaxis,O1X,O1Y,H1X,H1Y,L1X,L1Y);
            printf("Ellipse Arc1 a=%.4f,b=%.4f,e=b/a=%.4f,\n",R1,R2,R2/R1);
            Ellipab(&R1,&R2,angaxis,O2X,O2Y,H2X,H2Y,L2X,L2Y);
            printf("Ellipse Arc2 a=%.4f,b=%.4f,e=b/a=%.4f.\n",R1,R2,R2/R1);
        }
    }

    /* initialization (important) */
    Npoly=0; Narc1=0; Narc2=0; numarcpts=0; numpoly=0; numarc1=0; numarc2=0;
    N1=(unsigned int)ceil(ArcSpan1/arcres1); if (N1<2) N1=2;
    N2=(unsigned int)ceil(ArcSpan2/arcres2); if (N2<2) N2=2;
    MAXPNTARC = (N1>N2)? (unsigned int)(2*N1) : (unsigned int)(2*N2); /* twice maximum */
    MAXOUTPUT = (unsigned int)ceil(2.2*MAXPNTARC); /* (MAXOUTPUT~~2*MAXPNTARC) with margins */
    /* allocate dynamic memory: (mx)malloc(N*size); (mx)calloc(N,size); FREE(); */
    /* Matlab mex file CRASH if malloc() return NULL; AVOID use in SUBfunctions;
     * 20120507 for better management <#define malloc mxMalloc> in comcnst.h; */
    NBSZ = MAXOUTPUT*sizedouble; /* (double*)malloc(): Casting and type safety */
    POLYSX = malloc(NBSZ); POLYSY = malloc(NBSZ); /* polygons */
    NBSZ = MAXPNTARC*sizedouble;
    Rerr1 = malloc(NBSZ); Rerr2 = malloc(NBSZ); /* statistics */
    ARC1X = malloc(NBSZ); ARC1Y = malloc(NBSZ); /* trace1 */
    ARC2X = malloc(NBSZ); ARC2Y = malloc(NBSZ); /* trace2 */
    /* allocate dynamic memory for partial arc in Quadrant I */
    NQ = ( maxi(ArcSpan1,ArcSpan2)<PI_2 )?
         MAXPNTARC : (unsigned int)(MAXPNTARC*PI_2/mini(ArcSpan1,ArcSpan2));
    NBSZ = NQ*sizedouble;
    ArcPtsX = malloc(NBSZ); ArcPtsY = malloc(NBSZ); /* ArcSect() */
    ArcDist = malloc(NBSZ); /* numarcpts to control number of arc points */
    if ( !( POLYSX && POLYSY && Rerr1 && Rerr2 && ArcPtsX && ArcPtsY && ArcDist\
            && ARC1X && ARC1Y && ARC2X && ARC2Y ) )
        MsgErr("qdrim.c OUT OF MEMORY!\n");

    /* check for (ir)regular shapes */
    dQ=(unsigned int)mod(QDH1-QDH2,4); /* start points */
    if (debug>=2) printf("(QDH1,QDH2)=(%d,%d),dQ=%d.\n", QDH1,QDH2,dQ);
    switch (dQ) {
        case 0: /* regular: same Quadrant */
            QDH=QDH1;
            DivH1X=H1X; DivH1Y=H1Y; DivH2X=H2X; DivH2Y=H2Y;
            break;
        case 1: /* adjacent QDH2-->QDH1 counterclockwise; cut arc1; */
            QDH=QDH2;
            XH=H1X; YH=H1Y;
            ang=QDH2*PI_2; R=RH1-Rrate1*ArcSpan(AngH1,ang);
            Polar2XY(&XL,&YL,O1X,O1Y,R,ang);
            qkpie(1,XH,YH,XL,YL,H2X,H2Y);
            DivH1X=XL; DivH1Y=YL; DivH2X=H2X; DivH2Y=H2Y; /* loop boundary */
            break;
        case 3: /* adjacent QDH1-->QDH2 counterclockwise; cut arc2; */
            QDH=QDH1;
            XH=H2X; YH=H2Y;
            ang=QDH1*PI_2; R=RH2-Rrate2*ArcSpan(AngH2,ang);
            Polar2XY(&XL,&YL,O2X,O2Y,R,ang);
            qkpie(2,XH,YH,XL,YL,H1X,H1Y);
            DivH1X=H1X; DivH1Y=H1Y; DivH2X=XL; DivH2Y=YL; /* loop boundary */
            break;
        default: /* otherwise: differ more than two Quadrants */
            if (debug>=1) printf("qdrim.c Start points misplaced ( >2 Quadrants )!\n");
            flagFrac=false; /* to drawWhole(); */
            break;
    } /* L-->H ((XL,YL)-->(XH,YH)) counterclockwise */
    dQ=(unsigned int)mod(QDL2-QDL1,4); /* stop points */
    if (debug>=2) printf("(QDL2,QDL1)=(%d,%d),dQ=%d.\n", QDL2,QDL1,dQ);
    switch (dQ) {
        case 0: /* regular: same Quadrant */
            QDL=QDL1;
            DivL1X=L1X; DivL1Y=L1Y; DivL2X=L2X; DivL2Y=L2Y;
            break;
        case 1: /* adjacent QDL1-->QDL2 counterclockwise; cut arc1; */
            QDL=QDL2;
            XL=L1X; YL=L1Y;
            ang=QDL1*PI_2; R=RL1+Rrate1*ArcSpan(ang,AngL1);
            Polar2XY(&XH,&YH,O1X,O1Y,R,ang);
            qkpie(1,XH,YH,XL,YL,L2X,L2Y);
            DivL1X=XH; DivL1Y=YH; DivL2X=L2X; DivL2Y=L2Y; /* loop boundary */
            break;
        case 3: /* adjacent QDL2-->QDL1 counterclockwise; cut arc2; */
            QDL=QDL1;
            XL=L2X; YL=L2Y;
            ang=QDL2*PI_2; R=RL2+Rrate2*ArcSpan(ang,AngL2);
            Polar2XY(&XH,&YH,O2X,O2Y,R,ang);
            qkpie(2,XH,YH,XL,YL,L1X,L1Y);
            DivL1X=L1X; DivL1Y=L1Y; DivL2X=XH; DivL2Y=YH; /* loop boundary */
            break;
        default: /* otherwise: differ more than two Quadrants */
            if (debug>=1) printf("qdrim.c Stop points misplaced ( >2 Quadrants )!\n");
            flagFrac=false; /* to drawWhole(); */
            break;
    }
    if ((arcorder>=2)&&(!isAtAxis(angaxis))) { /* Y=f(X) multivalued function */
        if (debug>=2) printf("qdrim.c Off-Axis Ellipse Arc!\n");
        flagFrac=false; /* to drawWhole(); */
    }

    /* L->H counterclockwise; QDI() may return 0; NOTE boundaries (+-EPS)!! */
    if (QDH==0) QDH=1; if (QDL==0) QDL=4;
    /* construct info for each quadrant; Notes20081215C; */
    NQ=(unsigned int)mod(QDH-QDL,4)+1; /* number of quadrant sections */
    if ( (QDH==QDL) && (ArcSpan1>(PI+PI_2)) ) NQ=5; /* >270-deg AND QDpH==QDpL */
    if (debug>=2) printf("(QDH,QDL)=(%d,%d), NQ=%d.\n", QDH,QDL,NQ);

    if (flagFrac) {
        /* loop fracture each quadrant (start from high points to low points) */
        Org1X=O1X; Org1Y=O1Y; Org2X=O2X; Org2Y=O2Y;
        /* if regular: PosH1X=H1X; PosH1Y=H1Y; PosH2X=H2X; PosH2Y=H2Y; */
        PosH1X=DivH1X; PosH1Y=DivH1Y; PosH2X=DivH2X; PosH2Y=DivH2Y; /* (ir)regular */
        while (NQ>=2) { /* loop quadrant pieces (except the last piece) */
            XY2Polar(&R1,&ang1,Org1X,Org1Y,PosH1X,PosH1Y);
            XY2Polar(&R2,&ang2,Org2X,Org2Y,PosH2X,PosH2Y);
            Q1=QDI(ang1-EPS); Q2=QDI(ang2-EPS); /* -EPS! for High points */
            if (debug>=2) printf("  loop: NQ=%d, (QH1,QH2)=(%d,%d), (ang1,ang2)=(%.4f,%.4f)deg;\n", NQ,Q1,Q2,ang1*RAD,ang2*RAD);
            ang3=(Q1-1)*PI_2; R3=R1-Rrate1*ArcSpan(ang1,ang3);
            ang4=(Q2-1)*PI_2; R4=R2-Rrate2*ArcSpan(ang2,ang4);
            Polar2XY(&PosL1X,&PosL1Y,Org1X,Org1Y,R3,ang3);
            Polar2XY(&PosL2X,&PosL2Y,Org2X,Org2Y,R4,ang4);
            /* map q1rim() Q1 fracture arc to target Quadrant */
            if (Q1==Q2) qkrim(Q1); /* regular shape */
            /* else  irregular shape */
            /* loop to the other quadrant piece (clockwise previous) */
            PosH1X=PosL1X; PosH1Y=PosL1Y; PosH2X=PosL2X; PosH2Y=PosL2Y;
            NQ--; /* Q=QDprev(Q); */
        }
        /* last quadrant piece */
        /* if regular: PosL1X=L1X; PosL1Y=L1Y; PosL2X=L2X; PosL2Y=L2Y; */
        PosL1X=DivL1X; PosL1Y=DivL1Y; PosL2X=DivL2X; PosL2Y=DivL2Y; /* (ir)regular */
        qkrim(QDL);
    } else drawWhole(N1,N2); /* GDSII compliant */

    /* check overflow */
    if (debug>=1) {
        printf("(Npoly,Narc1,Narc2)=(%d,%d,%d)\n", Npoly,Narc1,Narc2);
        printf("MAXOUTPUT=%d, MAXPNTARC=%d.\n", MAXOUTPUT,MAXPNTARC);
    }
    if (Npoly>MAXOUTPUT) MsgErr("qdrim.c Output > Maximum Allocation!\n");
    if (Narc1>MAXPNTARC) MsgErr("qdrim.c Arc1 > Maximum Allocation!\n");
    if (Narc2>MAXPNTARC) MsgErr("qdrim.c Arc2 > Maximum Allocation!\n");

    /* OUTPUT */
    /* polygon output */
    plhs[0] = mxCreateDoubleMatrix(Npoly, 1, mxCOMPLEX);
    NBSZ = Npoly*sizedouble;
    ptr = mxGetPr(plhs[0]); memcpy(ptr, POLYSX, NBSZ);
    ptr = mxGetPi(plhs[0]); memcpy(ptr, POLYSY, NBSZ);
    /* trace output */
    if (nlhs > 1) { /* arc1 */
        /* SORT trace in Descending Order by polar angles (clockwise) */
        N1=Narc1; /* use N1 to protect Nacr1 */
        SortAngQD(ARC1X,ARC1Y,N1,O1X,O1Y,AngL1,atan(GRID/mini(RH1,RL1)));
        nocolinear(ARC1X,ARC1Y,&N1,EPS);
        /* sortangQD(ARC1X,ARC1Y,&N1,O1X,O1Y,AngL1,GRID); */
        plhs[1] = mxCreateDoubleMatrix(N1, 1, mxCOMPLEX);
        NBSZ = N1*sizedouble;
        ptr = mxGetPr(plhs[1]); memcpy(ptr, ARC1X, NBSZ);
        ptr = mxGetPi(plhs[1]); memcpy(ptr, ARC1Y, NBSZ);
    }
    if (nlhs > 2) { /* arc2 */
        /* SORT trace in Descending Order by polar angles (clockwise) */
        N2=Narc2; /* use N2 to protect Nacr2 */
        SortAngQD(ARC2X,ARC2Y,N2,O2X,O2Y,AngL2,atan(GRID/mini(RH2,RL2)));
        nocolinear(ARC2X,ARC2Y,&N2,EPS);
        /* sortangQD(ARC2X,ARC2Y,&N2,O2X,O2Y,AngL2,GRID); */
        plhs[2] = mxCreateDoubleMatrix(N2, 1, mxCOMPLEX);
        NBSZ = N2*sizedouble;
        ptr = mxGetPr(plhs[2]); memcpy(ptr, ARC2X, NBSZ);
        ptr = mxGetPi(plhs[2]); memcpy(ptr, ARC2Y, NBSZ);
    }
    /* statistics output */
    if (nlhs > 3) { /* Rerr1 */
        plhs[3] = mxCreateDoubleMatrix(Narc1, 1, mxREAL);
        ptr = mxGetPr(plhs[3]); memcpy(ptr, Rerr1, Narc1*sizedouble);
    }
    if (nlhs > 4) { /* Rerr2 */
        plhs[4] = mxCreateDoubleMatrix(Narc2, 1, mxREAL);
        ptr = mxGetPr(plhs[4]); memcpy(ptr, Rerr2, Narc2*sizedouble);
    }
    if (debug>=1) {/* statistics display */
        DataStat(&rdavg[1],&rdstd[1],&rdmax[1],&rdmin[1],Rerr1,Narc1); /*arc1*/
        printf("qdrim.c Rerr1(mean,max,min,std)=(%.3f,%.3f,%.3f,%.3f)GRID\n",
            rdavg[1]*InvGRID,rdmax[1]*InvGRID,rdmin[1]*InvGRID,rdstd[1]*InvGRID);
        DataStat(&rdavg[2],&rdstd[2],&rdmax[2],&rdmin[2],Rerr2,Narc2); /*arc2*/
        printf("qdrim.c Rerr2(mean,max,min,std)=(%.3f,%.3f,%.3f,%.3f)GRID\n",
            rdavg[2]*InvGRID,rdmax[2]*InvGRID,rdmin[2]*InvGRID,rdstd[2]*InvGRID);
        /* both arcs (arc1+arc2) */
        if ((Narc1+Narc2)>0) {
            rdavg[0]=(rdavg[1]*Narc1+rdavg[1]*Narc1)/(Narc1+Narc2);
            rdstd[0]=sqrt( ( rdstd[1]*rdstd[1]*Narc1 + rdstd[2]*rdstd[2]*Narc1 )
                           / (Narc1+Narc2) );
        } else { rdavg[0]=rdstd[0]=0; }
        rdmax[0]=maxi(rdmax[1],rdmax[2]);
        rdmin[0]=mini(rdmin[1],rdmin[2]);
        printf("qdrim.c Rerr (mean,max,min,std)=(%.3f,%.3f,%.3f,%.3f)GRID\n\n",
            rdavg[0]*InvGRID,rdmax[0]*InvGRID,rdmin[0]*InvGRID,rdstd[0]*InvGRID);
    }

    /* free() CRASH non-allocated! ( http://en.wikipedia.org/wiki/Malloc ) */
    FREE(POLYSX); FREE(POLYSY);
    FREE(Rerr1); FREE(Rerr2);
    FREE(ARC1X); FREE(ARC1Y); FREE(ARC2X); FREE(ARC2Y);
    FREE(ArcPtsX); FREE(ArcPtsY); FREE(ArcDist);
    return;
}
