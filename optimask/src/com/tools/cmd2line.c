/*=================================================================
 *
 * CMD2LINE.C	.MEX file converting ICED commands to lines
 *
 * The calling syntax is:
 *
 *		[pt1, pt2] = cmd2line(filename)
 *
 * Copyright 2004-2008 Little Optics, Inc.
 *
 *=================================================================*/
/* $Revision: 1.10 $ */
#include <stdio.h>
#include <string.h>
#include "mex.h"

#define BLOCKSIZE 1024*512
#define POLYNUM 200

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] )
     
{ 
    double *polyr1, *polyi1, *polyr2, *polyi2, *ptr;
    double liner[POLYNUM], linei[POLYNUM];  /* buffer to store line polygon points */
    char *filename, *linebuf, *bufpt;
    FILE *fid;
    unsigned int i, buflen, cur_idx, line_idx, memsize, ret; 
    
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
    polyr1 = (double *) mxCalloc(BLOCKSIZE, sizeof(double));
    polyi1 = (double *) mxCalloc(BLOCKSIZE, sizeof(double));
    polyr2 = (double *) mxCalloc(BLOCKSIZE, sizeof(double));
    polyi2 = (double *) mxCalloc(BLOCKSIZE, sizeof(double));
    memsize = BLOCKSIZE;
    
    /* Actual writing */
    fid = fopen(filename,"r");
    cur_idx = 0;  
    /* get rid of starting comments */
    fgets(linebuf, 800, fid); fgets(linebuf, 800, fid);
    /* read data */
    while (!feof(fid)) {
        if (cur_idx >= memsize-POLYNUM) {
            polyr1 = (double *) mxRealloc(polyr1, (memsize+BLOCKSIZE)*sizeof(double));
            polyi1 = (double *) mxRealloc(polyi1, (memsize+BLOCKSIZE)*sizeof(double));
            polyr2 = (double *) mxRealloc(polyr2, (memsize+BLOCKSIZE)*sizeof(double));
            polyi2 = (double *) mxRealloc(polyi2, (memsize+BLOCKSIZE)*sizeof(double));
            memsize += BLOCKSIZE;
        }
        fgets(linebuf, 800, fid);
        if (strstr(linebuf, "POLYGON")) {
            line_idx=0;
            while (1) {
                ret = 2; bufpt = linebuf;
                while (strstr(bufpt, "(")) {
                    bufpt = strstr(bufpt, "(") + 1; 
                    ret = sscanf(bufpt, "%lf, %lf", &liner[line_idx], &linei[line_idx]);
                    line_idx++;
                }
                if (!strstr(linebuf, "&")) break;
                fgets(linebuf, 800, fid);
            }
            /* translate polygon to lines */
            for (i=0;i<line_idx-1;i++) {
                polyr1[cur_idx]=liner[i]; polyi1[cur_idx]=linei[i];
                polyr2[cur_idx]=liner[i+1]; polyi2[cur_idx]=linei[i+1];
                cur_idx++;
            }
        }
        if (strstr(linebuf, "BOX")) {
            line_idx=0;
            ret = 2; bufpt = linebuf;
            while (strstr(bufpt, "(")) {
                bufpt = strstr(bufpt, "(") + 1; 
                ret = sscanf(bufpt, "%lf, %lf", &liner[line_idx], &linei[line_idx]);
                line_idx+=2;
            }
            /* translate polygon to lines and duplicate BOX other two points */
            liner[1]=liner[2]; linei[1]=linei[0];
            liner[3]=liner[0]; linei[3]=linei[2];
            liner[4]=liner[0]; linei[4]=linei[0];
            for (i=0;i<4;i++) {
                polyr1[cur_idx]=liner[i]; polyi1[cur_idx]=linei[i];
                polyr2[cur_idx]=liner[i+1]; polyi2[cur_idx]=linei[i+1];
                cur_idx++;
            }
        }
    }
    fclose(fid);
    
    /* data output */
    plhs[0] = mxCreateDoubleMatrix(cur_idx, 1, mxCOMPLEX);
    ptr = mxGetPr(plhs[0]);
    memcpy(ptr, polyr1, (cur_idx)*sizeof(double));
    ptr = mxGetPi(plhs[0]);
    memcpy(ptr, polyi1, (cur_idx)*sizeof(double));
    plhs[1] = mxCreateDoubleMatrix(cur_idx, 1, mxCOMPLEX);
    ptr = mxGetPr(plhs[1]);
    memcpy(ptr, polyr2, (cur_idx)*sizeof(double));
    ptr = mxGetPi(plhs[1]);
    memcpy(ptr, polyi2, (cur_idx)*sizeof(double));
    
    mxFree(filename); mxFree(linebuf); mxFree(polyr1); mxFree(polyi1); mxFree(polyr2); mxFree(polyi2);
    return;
}
