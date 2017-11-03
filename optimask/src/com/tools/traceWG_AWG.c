/*=================================================================
 *
 * TRACEWG.C	.MEX file trace WaveGuide from given start point
 *
 * The calling syntax is:
 *
 *		[wgtrace] = traceWG(linepts1,linepts2,startpt,startangle)
 *
 * Copyright 2004-2008 Little Optics, Inc.
 *
 *=================================================================*/
/* $Revision: 1.10 $ */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "matrix.h"
#include "mex.h"
#define PI		3.14159265358979323846

int numline, numtrace, curidx1, curidx2;
double *line1x, *line1y, *line2x, *line2y;
double curX, curY, lastX, lastY, curAngle;
double wgtraceX[20000], wgtraceY[20000];    /* output trace storage */

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

int findBroken()
{
    int i;
    for (i=curidx1;i>=0;i--) {
        if (sqrt(line1x[i]*line1x[i]+line1y[i]*line1y[i])+1e-6<sqrt(lastX*lastX+lastY*lastY)-0.005)
            break;
        if ( (fabs(fabs(line1x[i]-lastX)-0.005)<1e-6) && (line1y[i]==lastY) ) {
            curX=line1x[i]; curY=line1y[i];
            wgtraceX[numtrace]=curX; wgtraceY[numtrace]=curY;
            numtrace++;
            return(1);
        }
    }
    for (i=curidx2;i<numline;i++) {
        if (sqrt(line1x[i]*line1x[i]+line1y[i]*line1y[i])-1e-6>sqrt(lastX*lastX+lastY*lastY)+0.005)
            break;
        if ( (fabs(fabs(line1x[i]-lastX)-0.005)<1e-6) && (line1y[i]==lastY) ) {
            curX=line1x[i]; curY=line1y[i];
            wgtraceX[numtrace]=curX; wgtraceY[numtrace]=curY;
            numtrace++;
            return(1);
        }
    }
    return(0);
}
    
int pickNext()
{
    int i, nidx=-1;
    double tdist, idist, A, B, C, diffX, diffY, nextAngle;
    /* criteria: smallest distance to the extension of current WG */
    tdist=32.768*5;
    for (i=curidx1;i<curidx2;i++) {
        /* opposite direction exclusion */
        nextAngle=atan2(line2y[i]-curY,line2x[i]-curX);
        if (fabs(nextAngle-curAngle)>PI*0.6) continue;
        
        A=tan(curAngle); B=-1; C=curY-A*curX;
        idist=fabs(A*line2x[i]+B*line2y[i]+C)/sqrt(A*A+B*B);
        if (idist<tdist) {
            nidx=i; tdist=idist; }
    }
    lastX=curX; lastY=curY;
    if (nidx==-1) return(0); /* nothing to judge */
    
    curX=line2x[nidx]; curY=line2y[nidx];
    diffX=curX-lastX; diffY=curY-lastY;
    nextAngle=atan2(diffY,diffX);

    if ( (sqrt(diffX*diffX+diffY*diffY)>0.04*5) /* not a junction offset */
    && (fabs(nextAngle-curAngle)>PI/20) )    /* and angle difference > pi/4 */
        return(0);
        
    /* looks like good next point */
    if (sqrt(diffX*diffX+diffY*diffY)>0.5*5*1.2)    /* long enough */
        curAngle=nextAngle;
    wgtraceX[numtrace]=curX; wgtraceY[numtrace]=curY;
    numtrace++;
    return(1);
}

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] )
{ 
    double *ptr;
    double startX, startY, startAngle, langle, llangle;
    
    /* Check for proper number of arguments */
    if (nrhs!=4)
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
    ptr=mxGetPr(prhs[3]); startAngle=ptr[0];
    
    mexPrintf("%.4f %.4f %.4f\n",startX,startY,startAngle/PI*180);
    /* Trace WaveGuide */
    findStart(startX,startY);
    curAngle=startAngle; numtrace=1;
    curX=startX; curY=startY; wgtraceX[0]=curX; wgtraceY[0]=curY;
    while (1) {
        if (!pickNext()) {
            if (!findBroken())
                break;
            else {
                findStart(curX,curY);
                mexPrintf("Restart at broken point %.3f %.3f %.3f %d %d\n",curX,curY,curAngle*180/PI,curidx1,curidx2);
                pickNext();
            }
        }
        findNext();
    }
    /* check last point */
    while (1) {
        llangle=atan2(wgtraceY[numtrace-1]-wgtraceY[numtrace-2],wgtraceX[numtrace-1]-wgtraceX[numtrace-2]);
        langle=atan2(wgtraceY[numtrace-2]-wgtraceY[numtrace-3],wgtraceX[numtrace-2]-wgtraceX[numtrace-3]);
        if (fabs(llangle-langle)<PI/4) break;
        else numtrace--;
    }
    mexPrintf("%d %.4f %.4f\n",numtrace,wgtraceX[numtrace-1],wgtraceY[numtrace-1]);
    
    /* data output */
    plhs[0] = mxCreateDoubleMatrix(numtrace, 1, mxCOMPLEX);
    ptr = mxGetPr(plhs[0]);
    memcpy(ptr, wgtraceX, numtrace*sizeof(double));
    ptr = mxGetPi(plhs[0]);
    memcpy(ptr, wgtraceY, numtrace*sizeof(double));
    
    mxFree(line1x); mxFree(line1y); mxFree(line2x); mxFree(line2y);
    return;
}
