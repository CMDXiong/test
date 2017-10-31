/*=================================================================
 *
 * CMD2POLY.C	.MEX file converting ICED commands to polygons
 *
 * The calling syntax is:
 *
 *		[polygon,vertnum] = cmd2poly(filename)
 *
 * Copyright 2004-2008 Little Optics, Inc.
 *
 *=================================================================*/
/* $Revision: 1.10 $ */
#include <stdio.h>
#include <string.h>
#include "mex.h"

#define BLOCKSIZE 1024*512

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] )
     
{ 
    double *polyr, *polyi, *ptr; 
    char *filename, *linebuf, *bufpt;
    FILE *fid;
    unsigned int buflen, cur_idx, memsize, ret, numpoly;
    unsigned int *vertnum, vertmemsize;
    
    /* Check for proper number of arguments */
    if (nrhs != 1) { 
	mexErrMsgTxt("Filename input argument required."); 
    } else if (nlhs > 2) {
	mexErrMsgTxt("Too many output arguments required."); 
    } 

    /* get the length of the input string */
    buflen = (mxGetM(prhs[0]) * mxGetN(prhs[0])) + 1;
    filename = mxCalloc(buflen, sizeof(char));
    mxGetString(prhs[0], filename, buflen);
    
    /* allocate memory */
    linebuf = mxCalloc(800, sizeof(char));
    polyr = (double *) mxCalloc(BLOCKSIZE, sizeof(double));
    polyi = (double *) mxCalloc(BLOCKSIZE, sizeof(double));
    vertnum = (unsigned int *) mxCalloc(BLOCKSIZE, sizeof(unsigned int));
    memsize = BLOCKSIZE; vertmemsize = BLOCKSIZE;
    
    /* Actual writing */
    fid = fopen(filename,"r");
    cur_idx = 0;  
    /* get rid of starting comments */
    fgets(linebuf, 800, fid); fgets(linebuf, 800, fid);
    /* read data */
    numpoly = 0;
    while (!feof(fid)) {
        if (cur_idx >= memsize-200) {
            polyr = (double *) mxRealloc(polyr, (memsize+BLOCKSIZE)*sizeof(double));
            polyi = (double *) mxRealloc(polyi, (memsize+BLOCKSIZE)*sizeof(double));
            memsize += BLOCKSIZE;
        }
        if (numpoly >= vertmemsize-1) {
            vertnum = (unsigned int *) mxRealloc(vertnum, (vertmemsize+BLOCKSIZE)*sizeof(unsigned int));
            vertmemsize += BLOCKSIZE;
        }
        fgets(linebuf, 800, fid);
        if (strncmp(linebuf,"ADD",3)!=0) continue;
        if (strstr(linebuf, "POLY")) {
            vertnum[numpoly] = 0;
            while (1) {
                ret = 2; bufpt = linebuf;
                while (strstr(bufpt, "(")) {
                    bufpt = strstr(bufpt, "(") + 1; 
                    ret = sscanf(bufpt, "%lf, %lf", &polyr[cur_idx], &polyi[cur_idx]);
                    cur_idx++;
                    vertnum[numpoly]++;
                }
                if (!strstr(linebuf, "&")) break;
                fgets(linebuf, 800, fid);
            }
            numpoly++; 
        }
        if (strstr(linebuf, "BOX")) {
            while (1) {
                ret = 2; bufpt = linebuf;
                while (strstr(bufpt, "(")) {
                    bufpt = strstr(bufpt, "(") + 1; 
                    ret = sscanf(bufpt, "%lf, %lf", &polyr[cur_idx], &polyi[cur_idx]);
                    cur_idx+=2;
                }
                if (!strstr(linebuf, "&")) break;
                fgets(linebuf, 800, fid);
            }
            /* duplicate box other two points */
            polyr[cur_idx-3] = polyr[cur_idx-4]; polyi[cur_idx-3] = polyi[cur_idx-2];
            polyr[cur_idx-1] = polyr[cur_idx-2]; polyi[cur_idx-1] = polyi[cur_idx-4];
            vertnum[numpoly] = 4;
            numpoly++; 
        }
    }
    plhs[0] = mxCreateDoubleMatrix(cur_idx, 1, mxCOMPLEX);
    ptr = mxGetPr(plhs[0]);
    memcpy(ptr, polyr, (cur_idx)*sizeof(double));
    ptr = mxGetPi(plhs[0]);
    memcpy(ptr, polyi, (cur_idx)*sizeof(double));
    plhs[1] = mxCreateDoubleMatrix(numpoly, 1, mxREAL);
    ptr = mxGetPr(plhs[1]);
    for (ret=0; ret<numpoly; ret++)
        ptr[ret]=vertnum[ret];
    
    fclose(fid);
    mxFree(filename); mxFree(linebuf); mxFree(polyr); mxFree(polyi);
    return;
}
