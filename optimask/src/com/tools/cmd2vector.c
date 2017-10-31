/*=================================================================
 *
 * CMD2VECTOR.C	.MEX file converting ICED commands to vectors
 *
 * The calling syntax is:
 *
 *		[vector] = cmd2vector(filename)
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
    unsigned int buflen, cur_idx, memsize, ret; 
    
    /* Check for proper number of arguments */
    if (nrhs != 1) { 
	mexErrMsgTxt("Filename input argument required."); 
    } else if (nlhs > 1) {
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
    memsize = BLOCKSIZE;
    
    /* Actual writing */
    fid = fopen(filename,"r");
    cur_idx = 0;  
    /* get rid of starting comments */
    fgets(linebuf, 800, fid); fgets(linebuf, 800, fid);
    /* read data */
    while (!feof(fid)) {
        if (cur_idx >= memsize-20) {
            polyr = (double *) mxRealloc(polyr, (memsize+BLOCKSIZE)*sizeof(double));
            polyi = (double *) mxRealloc(polyi, (memsize+BLOCKSIZE)*sizeof(double));
            memsize += BLOCKSIZE;
        }
        fgets(linebuf, 800, fid);
        if (strstr(linebuf, "POLYGON")) {
            while (1) {
                ret = 2; bufpt = linebuf;
                while (strstr(bufpt, "(")) {
                    bufpt = strstr(bufpt, "(") + 1; 
                    ret = sscanf(bufpt, "%lf, %lf", &polyr[cur_idx], &polyi[cur_idx]);
                    cur_idx++;
                }
                if (!strstr(linebuf, "&")) break;
                fgets(linebuf, 800, fid);
            }
        }
        if (strstr(linebuf, "BOX")) {
            while (1) {
                ret = 2; bufpt = linebuf;
                while (strstr(bufpt, "(")) {
                    bufpt = strstr(bufpt, "(") + 1; 
                    ret = sscanf(bufpt, "%lf, %lf", &polyr[cur_idx], &polyi[cur_idx]);
                    cur_idx++;
                }
                if (!strstr(linebuf, "&")) break;
                fgets(linebuf, 800, fid);
            }
            /* duplicate box other two points */
            polyr[cur_idx] = polyr[cur_idx-2]; polyi[cur_idx] = polyi[cur_idx-1];
            cur_idx++;
            polyr[cur_idx] = polyr[cur_idx-2]; polyi[cur_idx] = polyi[cur_idx-3];
            cur_idx++;
        }
    }
    plhs[0] = mxCreateDoubleMatrix(cur_idx, 1, mxCOMPLEX);
    ptr = mxGetPr(plhs[0]);
    memcpy(ptr, polyr, (cur_idx)*sizeof(double));
    ptr = mxGetPi(plhs[0]);
    memcpy(ptr, polyi, (cur_idx)*sizeof(double));
    
    fclose(fid);
    mxFree(filename); mxFree(linebuf); mxFree(polyr); mxFree(polyi);
    return;
}
