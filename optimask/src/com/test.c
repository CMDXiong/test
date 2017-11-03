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
 * COPYRIGHT 2011 Henghua Deng
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00 $ (20110926 Henghua Deng)
 * HISTORY: 20110926 from ICEDPOLY.C v11.00 and COMGEOM.H v6.00;
 *  20110926(future improvement may need to remove overlapped polygon areas;)
/* ========================================================================== */

#define MATLAB_MEX_CODE /* #include "mex.h"&"matrix.h" in #include "comcnst.h" */
#include "comcnst.h"
#include "commath.h"
#include "comgeom.h"
//#include "comdefn.h"

#include "comfem.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    double *ptr=NULL, *PX=NULL, *PY=NULL; /* pointers */
    
    //PNT *points;
    
    unsigned int idxcur=0, idxnan=0, idxend=0;
    /* i--loop values; idxcur/nan/end--indexing for current/NaN/final points; */
    unsigned int nps=0; /* nps--number of points (per piece) */
    double S=0.0; /* total area of all polygons */
    
    double Tx[3]={1,2,3}, Ty[3]={4,6,5};//Tx[3]={0,1,2}, Ty[3]={0,2,1};
    double Ne[3][3], Ne2[6][6], NNT[3][3], NxNxT[3][3], NyNyT[3][3], NNT2[6][6], NxNxT2[6][6], NyNyT2[6][6];
    double NxNyT[3][3], NyNxT[3][3], NxNT[3][3], NyNT[3][3];
    double NxNyT2[6][6]={0}, NyNxT2[6][6]={0}, NxNT2[6][6]={0}, NyNT2[6][6]={0};
    unsigned int k;
    
    double Z1, Z2, Z3; //long int Z1, Z2;
    double X1, Y1, X2, Y2, X3, Y3, X4, Y4;
    double dx12=X2-X1, dx23=X3-X2, dx34=X4-X3, dx41=X1-X4, dx13=X3-X1, dx24=X4-X1;
    double dy12=Y2-Y1, dy23=Y3-Y2, dy34=Y4-Y3, dy41=Y1-Y4, dy13=Y3-Y1, dy24=Y4-Y1;

//     timeprint();
//     for (idxcur=0; idxcur<3000000; idxcur++) isOn(1,2,3);
//     timeprint();
//     return;

timeprint();

Z1=NAV;
Z2=Z1+1;
Z3=Z1-1;
printf("\tZ1=%f, Z2=%f, Z3=%f, Z1==NAV is %d, Z2==NAV is %d, Z3==NAV is %d\n", Z1, Z2, Z3, isNAV(Z1), isNAV(Z2),isNAV(Z3));
printf ("The maximum size of buffer: %d \n", BUFSIZ );
//printf ("double maximum: %f \n", DBL_MAX );

// printf("\t1025 is power of 2 : %d; IsOpSign=%d\n",isPOW2(1025), isopsign(EPS,EPS));
// printf("\tsizeof(int) : %d; sizeof(unsigned)=%d, CHAR_BIT=%d\n",sizeof(int),sizeof(unsigned),CHAR_BIT);
// printf("\tlog2(0) : %d; log2(1)=%d, log2(2)=%d\n",LOG_2(0),LOG_2(1),LOG_2(2));
// printf("\tlog2(255) : %d; log2(256)=%d, log2(257)=%d\n",LOG_2(255),LOG_2(256),LOG_2(257));
// printf("\tisPOW2(0) : %d; isPOW2(1)=%d, isPOW2(2)=%d\n",isPOW2(0),isPOW2(1),isPOW2(2));
// printf("\tisPOW2(255) : %d; isPOW2(256)=%d, isPOW2(257)=%d\n",isPOW2(255),isPOW2(256),isPOW2(257));
// printf("\tceilpow2(0) : %d; ceilpow2(1)=%d, ceilpow2(2)=%d\n",ceilpow2(0),ceilpow2(1),ceilpow2(2));
// printf("\tceilpow2(255) : %d; ceilpow2(256)=%d, ceilpow2(257)=%d\n",ceilpow2(255),ceilpow2(256),ceilpow2(257));
// printf("\tPOW2(0) : %d; POW2(1)=%d, POW2(2)=%d\n",POW2(0),POW2(1),POW2(2));
// printf("\tPOW2(4) : %d; POW2(5)=%d, POW2(6)=%d\n",POW2(4),POW2(5),POW2(6));
// FFT2D(plhs,k,k,1);
//printf("\tisAtAxis(%d)\n",isAtAxis(PI2-EPS*99999/100000));

// X1=-201.246; Y1=1000; X2=-30.599; Y2=1000; X3=-30.599; Y3=1050; X4=-201.246; Y4=1050;
// dx12=X2-X1; dx23=X3-X2; dx34=X4-X3; dx41=X1-X4; dx13=X3-X1; dx24=X4-X2;
// dy12=Y2-Y1; dy23=Y3-Y2; dy34=Y4-Y3; dy41=Y1-Y4; dy13=Y3-Y1; dy24=Y4-Y2;
// printf("\tdx=(%.4f,%.4f,%.4f,%.4f,%.4f,%.4f)\n\tdy=(%.4f,%.4f,%.4f,%.4f,%.4f,%.4f)\n",dx12,dx23,dx34,dx41,dx13,dx24,dy12,dy23,dy34,dy41,dy13,dy24);
// printf("\tD13-D24<EPS=%.4f; D13-D24<EPS=%.4f; D13-D24<EPS=%.4f; (S>0) (%.4f)\n",\
//         fabs(sqsum(dx13,dy13)-sqsum(dx24,dy24))<EPS,\
//         fabs(sqsum(dx12,dy12)-sqsum(dx34,dy34))<EPS,\
//         fabs(sqsum(dx23,dy23)-sqsum(dx41,dy41))<EPS,\
//         QuadDet2(X1,Y1,X2,Y2,X3,Y3,X4,Y4)!=0 );
// printf("\tD13-D24=%.12f; D13-D24=%.12f; D13-D24=%.12f; (S=%.12f)\n",\
//         sqsum(dx13,dy13)-sqsum(dx24,dy24),\
//         sqsum(dx12,dy12)-sqsum(dx34,dy34),\
//         sqsum(dx23,dy23)-sqsum(dx41,dy41),\
//         QuadDet2(X1,Y1,X2,Y2,X3,Y3,X4,Y4) );
// 
// printf("\tD13=%.12f; D24=%.12f; D12=%.12f; D34=%.12f; D13-D24=%.12f; (S=%.12f)\n",\
//         sqsum(dx13,dy13)-sqsum(dx24,dy24),\
//         sqsum(dx12,dy12)-sqsum(dx34,dy34),\
//         sqsum(dx23,dy23)-sqsum(dx41,dy41),\
//         QuadDet2(X1,Y1,X2,Y2,X3,Y3,X4,Y4) );



timeprint();
return;

//     timeprint();
//     for (idxnan=0; idxnan <1; idxnan++) {
//         for (idxend=0; idxend <1e5; idxend++) { for (idxcur=0; idxcur<3000000000; idxcur++) sign(5); }
//     }
//     timeprint();
//     return;


    /* check input/output arguments */
    //fprintf(stderr,"test.c Insufficient memory %d (%d bytes)\n", idxcur, idxend);
    //exit(0);
    if (nrhs < 2) mexErrMsgTxt("calcarea.c: input arguments incorrect!\n");
    if (nlhs > 1) mexErrMsgTxt("calcarea.c: output arguments incorrect!\n");

    /* read input polygon information */
    PX = mxGetPr(prhs[0]); PY = mxGetPi(prhs[0]); /* input coordinates */
    ptr = mxGetPr(prhs[1]); idxend = (unsigned int) ptr[0]; /* last indexing */

//    points=malloc(1000*sizeof(PNT));
    
    /* ========= loop polygon-pieces and process points ========= */
    /* (In C, while loop continues until condition becomes false.) */
    idxcur = 0; idxnan = 0; /* indexing initialization */
    while (idxcur<idxend) {
        /* find current polygon piece */
        /* (!_isnan(PX[])) && (!_isnan(PY[])) with #include <math.h> _isnan() */
        while ( (mxIsFinite(PX[idxnan])) && (mxIsFinite(PY[idxnan])) && (idxnan<idxend) ) idxnan++;
        nps = idxnan - idxcur; /* number of current piece points */
        
        /* i-th point value is (PX[i],PY[i]) or ( *(PX+i), *(PY+i) );
         * i-th point address is (&PX[i],&PY[i]) or (PX+i, PY+i); */
        S += PolyArea(&PX[idxcur],&PY[idxcur],nps); /* PolyArea(X,Y,N) */
        
        if (nps==3) {
            //Ne=intTri(1,[0 1 2],[0 2 1])
            //FnShapeTRI1(Ne,&PX[idxcur],&PY[idxcur]);
            //FnShapeTri1(Ne,Tx[0],Ty[0],Tx[1],Ty[1],Tx[2],Ty[2]);
            FnShapeTRI1(Ne,Tx,Ty);
            printf("\nShape Function Triangle 1st Order\n");
            printf("\t%.12g,%.12g,%.12g \n",Ne[0][0],Ne[0][1],Ne[0][2]);
            printf("\t%.12g,%.12g,%.12g \n",Ne[1][0],Ne[1][1],Ne[1][2]);
            printf("\t%.12g,%.12g,%.12g \n",Ne[2][0],Ne[2][1],Ne[2][2]);
            
            FnShapeTri2(Ne2,Tx[0],Ty[0],Tx[1],Ty[1],Tx[2],Ty[2]);
            //FnShapeTri2(Ne2,1,4,2,6,3,5);
            //FnShapeTRI2(Ne2,Tx,Ty);
            printf("\nShape Function Triangle 2nd Order\n");
            for (k=0;k<6;k++)
                printf("\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f \n",
                   Ne2[k][0],Ne2[k][1],Ne2[k][2],Ne2[k][3],Ne2[k][4],Ne2[k][5]);

//             //MatTri1Drv(NNT,NxNxT,NyNyT,NxNyT,NyNxT,NxNT,NyNT,Tx[0],Ty[0],Tx[1],Ty[1],Tx[2],Ty[2]);
//             MatTRI1Drv(NNT,NxNxT,NyNyT,NxNyT,NyNxT,NxNT,NyNT,Tx,Ty);
//             //MatTRI1(NNT,NxNxT,NyNyT,Tx,Ty);
//             printf("\nMatrices Triangle 1st Order\n");
//             printf("\t%.12g,%.12g,%.12g \n",NxNxT[0][0],NxNxT[0][1],NxNxT[0][2]);
//             printf("\t%.12g,%.12g,%.12g \n",NxNxT[1][0],NxNxT[1][1],NxNxT[1][2]);
//             printf("\t%.12g,%.12g,%.12g \n",NxNxT[2][0],NxNxT[2][1],NxNxT[2][2]);
//             printf("\nMatrices Triangle 1st Order\n");
//             printf("\t%.12g,%.12g,%.12g \n",NyNyT[0][0],NyNyT[0][1],NyNyT[0][2]);
//             printf("\t%.12g,%.12g,%.12g \n",NyNyT[1][0],NyNyT[1][1],NyNyT[1][2]);
//             printf("\t%.12g,%.12g,%.12g \n",NyNyT[2][0],NyNyT[2][1],NyNyT[2][2]);
            //intTri2NNT(NNT2,Tx[0],Ty[0],Tx[1],Ty[1],Tx[2],Ty[2]);
            //intTRI2NNT(NNT2,Tx,Ty);
            //intTri2(NxNxT2,NyNyT2,NNT2,Tx[0],Ty[0],Tx[1],Ty[1],Tx[2],Ty[2]);
            //intTri2Drv(NNT2,NxNxT2,NyNyT2,NxNyT2,NyNxT,NxNT2,NyNT2,Tx[0],Ty[0],Tx[1],Ty[1],Tx[2],Ty[2]);
            //intTRI2Drv(NNT2,NxNxT2,NyNyT2,NxNyT2,NyNxT,NxNT2,NyNT2,Tx,Ty);
            IntTRI2(NxNxT2,NyNyT2,NNT2,Tx,Ty);
            printf("\nMatrices Triangle 2nd Order\n");
            for (k=0;k<6;k++)
                printf("\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f \n",
                   NNT2[k][0],NNT2[k][1],NNT2[k][2],NNT2[k][3],NNT2[k][4],NNT2[k][5]);
        }
        
        /* loop to the next polygon piece */
        idxcur = ++idxnan; /* idxcur = idxnan + 1; idxnan = idxcur; */
    }
  timestamp();
  timestamp(NULL);
  printf("%s\n", timestamp(NULL));
  timeprint(NULL);
    plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
    memcpy(mxGetPr(plhs[0]), &S, sizeof(double));
    return;
}
