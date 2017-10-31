/*=================================================================
 * TRACEWG.C	.MEX file trace WaveGuide from given start point
 * The calling syntax is:
 *		[wgtrace] = traceWG(linepts1,linepts2,startpt,stoppt,startangle,xfactor,jcnoffset)
 * Copyright 2004-2008 Little Optics, Inc.
 *=================================================================*/
/* $Revision: 1.20 $ */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "matrix.h"
#include "mex.h"
#define PI		3.14159265358979323846
#define GRID 0.001
#define MASKGRID 32.768
#define EPS 1e-6
#define MAXPT 2000000

int numline, numtrace, curidx1, curidx2, xfactor;
double *line1x, *line1y, *line2x, *line2y;
double curX, curY, lastX, lastY, curAngle, jcnoffset;
double *wgtraceX, *wgtraceY;    /* output trace storage */

void findStart(double startX, double startY)
{
    int i;
    for (i=0;i<numline;i++)
        if ((line1x[i]==startX)&&(line1y[i]==startY)) break;
    curidx1=i;
    for (i=curidx1;i<numline;i++)
        if ((line1x[i]!=startX)||(line1y[i]!=startY)) break;
    curidx2=i;
}

void findNext()
{
    int i;
    /* assume line vectors already sorted */
    if (((curX*curX+curY*curY)<(lastX*lastX+lastY*lastY)) ||
    (((curX*curX+curY*curY)==(lastX*lastX+lastY*lastY)) && (atan2(curY,curX)<atan2(lastY,lastX)))) {
        for (i=curidx1;i>=0;i--)
            if ((line1x[i]==curX)&&(line1y[i]==curY)) break;
        curidx2=i+1;
        for (i=curidx2-1;i>=0;i--)
            if ((line1x[i]!=curX)||(line1y[i]!=curY)) break;
        curidx1=i+1;
    } else {
        for (i=curidx2;i<numline;i++)
            if ((line1x[i]==curX)&&(line1y[i]==curY)) break;
        curidx1=i;
        for (i=curidx1;i<numline;i++)
            if ((line1x[i]!=curX)||(line1y[i]!=curY)) break;
        curidx2=i;
    }
}

int findBroken(double jcnoffset)
{
    int i, nidx=-1;
    double offset, nextAngle, tdist;
    
    tdist=(jcnoffset+GRID)*xfactor;
    for (i=curidx1-1;i>=0;i--) {
        if (sqrt(line1x[i]*line1x[i]+line1y[i]*line1y[i])+EPS<sqrt(lastX*lastX+lastY*lastY)-2*jcnoffset*xfactor)
            break;
        nextAngle=atan2(line1y[i]-lastY,line1x[i]-lastX);
        if (cos(nextAngle-curAngle)<-EPS) continue;
        offset=sqrt((line1x[i]-lastX)*(line1x[i]-lastX)+(line1y[i]-lastY)*(line1y[i]-lastY));
        if (offset<tdist) {nidx=i; tdist=offset; }
    }
    for (i=curidx2;i<numline;i++) {
        if (sqrt(line1x[i]*line1x[i]+line1y[i]*line1y[i])-EPS>sqrt(lastX*lastX+lastY*lastY)+2*jcnoffset*xfactor)
            break;
        nextAngle=atan2(line1y[i]-lastY,line1x[i]-lastX);
        if (cos(nextAngle-curAngle)<-EPS) continue;
        offset=sqrt((line1x[i]-lastX)*(line1x[i]-lastX)+(line1y[i]-lastY)*(line1y[i]-lastY));
        if (offset<tdist) {nidx=i; tdist=offset; }
    }
    if (nidx==-1) return(0); /* nothing to judge */

    curX=line1x[nidx]; curY=line1y[nidx];
    wgtraceX[numtrace]=curX; wgtraceY[numtrace]=curY;
    numtrace++;
    return(1);
}
    
int pickNext()
{
    int i, nidx=-1;
    double tdist, idist, A, B, C, diffX, diffY, nextAngle, angleDiff;
    /* criteria: smallest distance to the extension of current WG */
    tdist=MASKGRID*xfactor;
    for (i=curidx1;i<curidx2;i++) {
        /* opposite direction exclusion */
        nextAngle=atan2(line2y[i]-curY,line2x[i]-curX);
        angleDiff=fabs(nextAngle-curAngle);
        angleDiff=fabs(floor(angleDiff/2/PI+0.5)*2*PI-angleDiff);
        if (angleDiff>PI/2) {
            continue; }
        /* junction treatment */
        if ( (fabs(curAngle)<EPS) && (fabs(nextAngle)<EPS) ) {
            diffX=curX-lastX; diffY=curY-lastY;
            if (fabs(atan2(diffY,diffX))>PI/4) {
                if ( (fmod(line2x[i]+EPS,MASKGRID)>2*EPS) && (fmod(wgtraceX[numtrace-3]+EPS,MASKGRID)>2*EPS) )
                    continue;
            }
        }
            
        A=tan(curAngle); B=-1; C=curY-A*curX;
        idist=fabs(A*line2x[i]+B*line2y[i]+C)/sqrt(A*A+B*B);
        idist/=sqrt((line2x[i]-curX)*(line2x[i]-curX)+(line2y[i]-curY)*(line2y[i]-curY));
        /* mask grid exclusion */
        if ( ((fabs(nextAngle)<EPS)||(fabs(nextAngle-PI)<EPS)||(fabs(nextAngle+PI)<EPS))
            && (fabs(floor(line2x[i]/MASKGRID/xfactor+0.5)*xfactor*MASKGRID-line2x[i])<EPS) )
            idist+=xfactor*10*GRID;
        if (idist<tdist) {
            nidx=i; tdist=idist; }
    }
    lastX=curX; lastY=curY;
    if (nidx==-1) return(0); /* nothing to judge */

    curX=line2x[nidx]; curY=line2y[nidx];
    diffX=curX-lastX; diffY=curY-lastY;
    nextAngle=atan2(diffY,diffX);
    angleDiff=fabs(nextAngle-curAngle);
    angleDiff=fabs(floor(angleDiff/2/PI+0.5)*2*PI-angleDiff);

    if ( (sqrt(diffX*diffX+diffY*diffY)>jcnoffset/2*xfactor) /* not a junction offset */
        && (angleDiff>PI/4) )    /* and angle difference > pi/4 */
        return(0);
    if ( (sqrt(diffX*diffX+diffY*diffY)>jcnoffset*xfactor) /* not a junction offset */
        && (angleDiff>PI/45) )    /* and angle difference > pi/45 */
        return(0);
        
    /* looks like a good next point */
    if (angleDiff<PI/45)    /* not a junction offset */
        curAngle=nextAngle;
    wgtraceX[numtrace]=curX; wgtraceY[numtrace]=curY;
    numtrace++;
    return(1);
}

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] )
{ 
    double *ptr;
    double startX, startY, stopX, stopY, startAngle, langle, llangle;
    
    /* Check for proper number of arguments */
    if (nrhs!=7)
        mexErrMsgTxt("Input arguments incorrect."); 
    else if (nlhs!=1)
        mexErrMsgTxt("Output arguments incorrect.");
    if (mxGetM(prhs[0])!=mxGetM(prhs[1]))
        mexErrMsgTxt("Input line vectors not equal in size.");

    /* get the input parameters */
    numline=mxGetM(prhs[0]);
    line1x=(double *) mxCalloc(numline, sizeof(double));
    line1y=(double *) mxCalloc(numline, sizeof(double));
    line2x=(double *) mxCalloc(numline, sizeof(double));
    line2y=(double *) mxCalloc(numline, sizeof(double));
    ptr=mxGetPr(prhs[0]);
    memcpy(line1x, ptr, numline*sizeof(double));
    ptr=mxGetPi(prhs[0]);
    memcpy(line1y, ptr, numline*sizeof(double));
    ptr=mxGetPr(prhs[1]);
    memcpy(line2x, ptr, numline*sizeof(double));
    ptr=mxGetPi(prhs[1]);
    memcpy(line2y, ptr, numline*sizeof(double));
    
    ptr=mxGetPr(prhs[2]); startX=ptr[0];
    ptr=mxGetPi(prhs[2]); 
    if (ptr) startY=ptr[0];
    else startY=0;
    ptr=mxGetPr(prhs[3]); stopX=ptr[0];
    ptr=mxGetPi(prhs[3]); 
    if (ptr) stopY=ptr[0];
    else stopY=0;
    ptr=mxGetPr(prhs[4]); startAngle=ptr[0];
    ptr=mxGetPr(prhs[5]); xfactor=(int)ptr[0];
    ptr=mxGetPr(prhs[6]); jcnoffset=ptr[0];
    
    mexPrintf("%.4f %.4f %.4f %.4f %.4f\n",startX,startY,stopX,stopY,startAngle/PI*180);
    /* Trace WaveGuide */
    wgtraceX=(double *) mxCalloc(MAXPT, sizeof(double));
    wgtraceY=(double *) mxCalloc(MAXPT, sizeof(double));
    
    findStart(startX,startY);
    curAngle=startAngle; numtrace=1;
    curX=startX; curY=startY; wgtraceX[0]=curX; wgtraceY[0]=curY;
    while (1) {
        if (numtrace>MAXPT-10) break;   /* we are done, not enough preallocated memory */
        if (!pickNext()) {
            if (!findBroken(jcnoffset))
                break;
            else {
                findStart(curX,curY);
                mexPrintf("Junction of %.3f at %.3f %.3f %.3f %d %d\n",
                    sqrt((curX-lastX)*(curX-lastX)+(curY-lastY)*(curY-lastY)),
                    curX,curY,curAngle*180/PI,curidx1,curidx2);
                pickNext();
            }
        }
        if ((curX==stopX)&&(curY==stopY)) break;    /* we are done! */
        findNext();
    }
    mexPrintf("%d %.4f %.4f %.4f %d %d\n",
        numtrace,wgtraceX[numtrace-1],wgtraceY[numtrace-1],curAngle*180/PI,curidx1,curidx2);
    
    /* data output */
    plhs[0] = mxCreateDoubleMatrix(numtrace, 1, mxCOMPLEX);
    ptr = mxGetPr(plhs[0]);
    memcpy(ptr, wgtraceX, numtrace*sizeof(double));
    ptr = mxGetPi(plhs[0]);
    memcpy(ptr, wgtraceY, numtrace*sizeof(double));
    
    mxFree(line1x); mxFree(line1y); mxFree(line2x); mxFree(line2y);
    mxFree(wgtraceX); mxFree(wgtraceY);
    return;
}
