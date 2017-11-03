/* ========================================================================== */
/* COMFEM.H    COMmon Finite-Element-Method (FEM) header file
 *
 * REFERENCE:
 *  [0]. PhD2005Deng [hdeng2005.pdf], Henghua Deng, PhD Thesis, UWaterloo, 2005;
 *       \Research2001to2005\FEMBPM\FEMSCFV6\ (intTri.m, intMix.m, etc.);
 *  [1]. Masanori Koshiba, Optical Waveguide Theory by the Finite Element Method,
 *         KTK Scientific Publishers (Tokyo) / Kluwer Academic Press, 1992;
 *       MTT92Feb371 & JLT94Mar495.
 *       M.Koshiba & Y.Tsuji, J.Lightw. Tech.,18(5), May 2000, pp.737-743.
 *       Koshiba&Tsuji IJNM00Mar115(Int.J.Numer.Model.2000;13:115-126): BPM;
 *       OQE98Dec995(Opt. Quant. Electron. 30(1998)995-1003): Eigenmode Solver.
 *       IEEE Proceedings-J, vol.139, no.2, April 1992, pp.166
 *       K.Saitoh & M.Koshiba, J.Quantum Electron.,38(7),July 2002,pp.927-933.
 *  [2]. Jianming Jin 1993 Wiley(2nd ed. 2003);
 *       Matthew N.O.Sadiku 1992 CRC(2nd ed. 2001);
 *       John L.Volakis 1998 IEEE, Kenji Kawano 2001 Wiley;
 *       Katsunari Okamoto 2000 Academic;
 *       F.Fernandez 1996 Research Studies Press / Wiley;
 *       P.L.George, Automatic Mesh Generation, Wiley, 1991;
 *
 * COPYRIGHT 2001-2011 Henghua Deng.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.10$ (20111130 Henghua Deng) */
/* HISTORY: 20111011(v1.00)from matlab codes (folder FEMSCFV6, PhD thesis);
 * 20111128(v1.10)Scalar, Semi-Vectorial, Full-Vectorial cases;
/* ========================================================================== */

/* (_x_,_y_,_z_)=(0,1,2) for Coordinate/Vector indexing (O'Rourke1998 pp.25); */
#ifndef _x_
  #define _x_ 0
#endif
#ifndef _y_
  #define _y_ 1
#endif
#ifndef _z_
  #define _z_ 2
#endif

/* ########################################################################## */
/* Shape Functions ( & Derivatives), Integrals & Element Matrices
 *   triangular nodal elements FEMBPM\FEMSCFV6\intTri.m;
 *   triangular mixed elements FEMBPM\FEMSCFV6\intMix.m; */
/* for 1st order (linear) elements (nodal or mixed/hybrid):
 *     Shape Function and Derivatives omit variables *[1 x y].';
 *     (actual should be Ne*[1 x y].');
 * for 2nd order (quadratic) elements (nodal or mixed/hybrid):
 *     Shape Function and Derivatives omit variables *[1 x y x^2 x*y y^2].';
 *     (actual should be Ne*[1 x y x^2 x*y y^2].'); */
/* 1st order triangular nodal elements: linear elements;
 * 2nd order triangular nodal elements: quadratic elements;
 * 1st order triangular mixed elements: constant edge and linear nodal elements;
 * 2nd order triangular mixed elements: linear edge and quadratic nodal elements; */
/* -------------------------------------------------------------------------- */
/* MATLAB Arithmetic Operators:
 *   ' Matrix transpose. A' is the linear algebraic transpose of A.
 *       For complex matrices, this is the complex conjugate transpose.
 *   .' Array transpose. A.' is the array transpose of A.
 *       For complex matrices, this does not involve conjugation. */
/* ########################################################################## */

/* ========================================================================== */
/* Basic Info for the triangle: 1st order (linear) & 2nd order (quadratic) */
/* JacobMat=[X2-X1, Y2-Y1; X3-X1, Y3-Y1]; Eq.(1.81-1.85) Koshiba book1992;
 *   NOTE: det(JacobMat)=2*Ae for regular triangular elements (determinant).
 *   Ae=TriDet(X1,Y1,X2,Y2,X3,Y3); Ae2=TriDet2(X1,Y1,X2,Y2,X3,Y3); (comgeom.h)
 */
double triabc(double a[3], double b[3], double c[3],\
           double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: Ae2=triabc(a,b,c,X1,Y1,X2,Y2,X3,Y3); (Ae2=2*Ae;) */
    a[0]=X2*Y3-X3*Y2; b[0]=Y2-Y3; c[0]=X3-X2;
    a[1]=X3*Y1-X1*Y3; b[1]=Y3-Y1; c[1]=X1-X3;
    a[2]=X1*Y2-X2*Y1; b[2]=Y1-Y2; c[2]=X2-X1;
    return(c[2]*b[1]-c[1]*b[2]); /* TWICE of triangle signed area (determinant) */
} /* Eq. (8)-(13) of JLT94Mar495Koshiba */
double TRIabc(double a[3], double b[3], double c[3], double *X, double *Y)
{ /* calling syntax: Ae2=TRIabc(a,b,c,X,Y); or Ae2=TRIabc(a,b,c,&X[0],&Y[0]); */
    a[0]=X[1]*Y[2]-X[2]*Y[1]; b[0]=Y[1]-Y[2]; c[0]=X[2]-X[1];
    a[1]=X[2]*Y[0]-X[0]*Y[2]; b[1]=Y[2]-Y[0]; c[1]=X[0]-X[2];
    a[2]=X[0]*Y[1]-X[1]*Y[0]; b[2]=Y[0]-Y[1]; c[2]=X[1]-X[0];
    return(c[2]*b[1]-c[1]*b[2]); /* TWICE of triangle signed area (determinant) */
}
double TRIABC(double a[3], double b[3], double c[3], double P[3][2])
{ /* calling syntax: double P[3][2]; Ae2=TRIABC(a,b,c,P); TRIABC(a,b,c,&P[0][0]); */
    a[0]=P[1][0]*P[2][1]-P[2][0]*P[1][1]; /* P[][0]: X; P[][1]: Y; */
    a[1]=P[2][0]*P[0][1]-P[0][0]*P[2][1]; /* #define _x_ 0 */
    a[2]=P[0][0]*P[1][1]-P[1][0]*P[0][1]; /* #define _y_ 1 */
    b[0]=P[1][1]-P[2][1]; c[0]=P[2][0]-P[1][0];
    b[1]=P[2][1]-P[0][1]; c[1]=P[0][0]-P[2][0];
    b[2]=P[0][1]-P[1][1]; c[2]=P[1][0]-P[0][0];
    return(c[2]*b[1]-c[1]*b[2]); /* TWICE of triangle signed area (determinant) */
}
/* Signed Length of the  three triangle edges: Eq.(10) of JLT94Mar495Koshiba; */
void trisgnl(double l[3], double b[3], double c[3])
{ /* calling syntax: double b[3], c[3], l[3]; trisgnl(l,b,c); */

    l[0]=hypot(b[2],c[2]);
    l[1]=hypot(b[0],c[0]);
    l[2]=hypot(b[1],c[1]);
    l[0] = (b[2]==0)? ( (c[2]>0)? l[0] : -l[0] ) : ( (b[2]>0)? -l[0] : l[0] );
    l[1] = (b[0]==0)? ( (c[0]>0)? l[1] : -l[1] ) : ( (b[0]>0)? -l[1] : l[1] );
    l[2] = (b[1]==0)? ( (c[1]>0)? l[2] : -l[2] ) : ( (b[1]>0)? -l[2] : l[2] );
    return;
}
/* Unit Tangential Vector of triangle sides : Eq.(14) of JLT94Mar495Koshiba */
void trivec(double V[3][2],\
            double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: double V[3][2]; trivec(V,X1,Y1,X2,Y2,X3,Y3); */
    double b[3], c[3], l[3];
    b[0]=Y2-Y3; c[0]=X3-X2;
    b[1]=Y3-Y1; c[1]=X1-X3;
    b[2]=Y1-Y2; c[2]=X2-X1;
    trisgnl(l,b,c); /* signed lengths: Eq.(10) of JLT94Mar495Koshiba; */
    V[0][0]=c[2]/l[0]; V[0][1]=-b[2]/l[0];  /*[][0]: Vx; [][1]: Vy; */
    V[1][0]=c[0]/l[1]; V[1][1]=-b[0]/l[1];
    V[2][0]=c[1]/l[2]; V[2][1]=-b[1]/l[2];
    return;
}
void TRIvec(double V[3][2], double *X, double *Y)
{ /* calling syntax: double V[3][2]; TRIvec(V,X,Y); or TRIvec(V,&X[0],&Y[0]); */
    trivec(V,X[0],Y[0],X[1],Y[1],X[2],Y[2]);
    return;
}

/* -------------------------------------------------------------------------- */
/* Shape Function of 1st order (linear) triangular nodal element */
void FnShapeTri1(double Ne[3][3],\
         double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: double Ne[3][3]; FnShapeTri1(Ne,X1,Y1,X2,Y2,X3,Y3); */
    double Ae2, d, b[3], c[3];

    b[0]=Y2-Y3; c[0]=X3-X2;
    b[1]=Y3-Y1; c[1]=X1-X3;
    b[2]=Y1-Y2; c[2]=X2-X1;
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=1/Ae2; /* common denominator 1/(2*Ae) */
    Ne[0][0]=(X2*Y3-X3*Y2)*d; /*a[0]*b; */ Ne[0][1]=b[0]*d; Ne[0][2]=c[0]*d;
    Ne[1][0]=(X3*Y1-X1*Y3)*d; /*a[1]*b; */ Ne[1][1]=b[1]*d; Ne[1][2]=c[1]*d;
    Ne[2][0]=(X1*Y2-X2*Y1)*d; /*a[2]*b; */ Ne[2][1]=b[2]*d; Ne[2][2]=c[2]*d;
    return; /* Eq.(1.75) of Kishiba book1992 */
}
/* array form of Shape Function of 1st order (linear) triangular nodal element */
void FnShapeTRI1(double Ne[3][3], double *X, double *Y)
{ /* double Ne[3][3]; FnShapeTRI1(Ne,X,Y); or FnShapeTRI1(Ne,&X[0],&Y[0]); */
    double Ae2, d, b[3], c[3];

    b[0]=Y[1]-Y[2]; c[0]=X[2]-X[1];
    b[1]=Y[2]-Y[0]; c[1]=X[0]-X[2];
    b[2]=Y[0]-Y[1]; c[2]=X[1]-X[0];
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=1/Ae2; /* common denominator 1/(2*Ae) */
    Ne[0][0]=(X[1]*Y[2]-X[2]*Y[1])*d; /*a[0]*b; */ Ne[0][1]=b[0]*d; Ne[0][2]=c[0]*d;
    Ne[1][0]=(X[2]*Y[0]-X[0]*Y[2])*d; /*a[1]*b; */ Ne[1][1]=b[1]*d; Ne[1][2]=c[1]*d;
    Ne[2][0]=(X[0]*Y[1]-X[1]*Y[0])*d; /*a[2]*b; */ Ne[2][1]=b[2]*d; Ne[2][2]=c[2]*d;
    return; /* Eq.(1.75) of Kishiba book1992 */
}
/* Shape Function of 2nd order (quadratic) triangular nodal element */
void FnShapeTri2(double Ne[6][6],\
         double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: double Ne[6][6]; FnShapeTri2(Ne,X1,Y1,X2,Y2,X3,Y3); */
    double Ae, a[3], b[3], c[3];
    double t, d, da[3], db[3], dc[3]; /* for speed */

    da[0]=a[0]=X2*Y3-X3*Y2; db[0]=b[0]=Y2-Y3; dc[0]=c[0]=X3-X2;
    da[1]=a[1]=X3*Y1-X1*Y3; db[1]=b[1]=Y3-Y1; dc[1]=c[1]=X1-X3;
    da[2]=a[2]=X1*Y2-X2*Y1; db[2]=b[2]=Y1-Y2; dc[2]=c[2]=X2-X1;
    Ae=0.5*(c[2]*b[1]-c[1]*b[2]); /* triangle signed area (determinant) */

    d=0.5/(Ae*Ae); /* common denominator */
    da[0]*=d; da[1]*=d; da[2]*=d;
    db[0]*=d; db[1]*=d; db[2]*=d;
    dc[0]*=d; dc[1]*=d; dc[2]*=d;

    t=a[0]-Ae; Ne[0][0]=da[0]*t; t+=a[0]; Ne[0][1]=db[0]*t; Ne[0][2]=dc[0]*t;
    t=a[1]-Ae; Ne[1][0]=da[1]*t; t+=a[1]; Ne[1][1]=db[1]*t; Ne[1][2]=dc[1]*t;
    t=a[2]-Ae; Ne[2][0]=da[2]*t; t+=a[2]; Ne[2][1]=db[2]*t; Ne[2][2]=dc[2]*t;
    Ne[0][3]=db[0]*b[0]; Ne[0][5]=dc[0]*c[0]; c[0]+=c[0]; Ne[0][4]=db[0]*c[0];
    Ne[1][3]=db[1]*b[1]; Ne[1][5]=dc[1]*c[1]; c[1]+=c[1]; Ne[1][4]=db[1]*c[1];
    Ne[2][3]=db[2]*b[2]; Ne[2][5]=dc[2]*c[2]; c[2]+=c[2]; Ne[2][4]=db[2]*c[2];
    a[0]+=a[0]; a[1]+=a[1]; a[2]+=a[2]; b[0]+=b[0]; b[1]+=b[1]; b[2]+=b[2];
    /* new a[], b[], c[] all doubled (*2 for the following calculation) */
    Ne[3][0]=da[0]*a[1];
    Ne[3][1]=da[0]*b[1]+db[0]*a[1]; Ne[3][2]=da[0]*c[1]+dc[0]*a[1];
    Ne[3][3]=db[0]*b[1]; Ne[3][4]=db[0]*c[1]+dc[0]*b[1]; Ne[3][5]=dc[0]*c[1];
    Ne[4][0]=da[1]*a[2];
    Ne[4][1]=da[1]*b[2]+db[1]*a[2]; Ne[4][2]=da[1]*c[2]+dc[1]*a[2];
    Ne[4][3]=db[1]*b[2]; Ne[4][4]=db[1]*c[2]+dc[1]*b[2]; Ne[4][5]=dc[1]*c[2];
    Ne[5][0]=da[2]*a[0];
    Ne[5][1]=da[2]*b[0]+db[2]*a[0]; Ne[5][2]=da[2]*c[0]+dc[2]*a[0];
    Ne[5][3]=db[2]*b[0]; Ne[5][4]=db[2]*c[0]+dc[2]*b[0]; Ne[5][5]=dc[2]*c[0];
    return;
}
/* array form of Shape Function of 2nd order (quadratic) triangular element */
void FnShapeTRI2(double Ne[6][6], double *X, double *Y)
{ /* double Ne[6][6]; FnShapeTRI2(Ne,X,Y); or FnShapeTRI2(Ne,&X[0],&Y[0]); */
    double Ae, a[3], b[3], c[3];
    double t, d, da[3], db[3], dc[3]; /* for speed */

    da[0]=a[0]=X[1]*Y[2]-X[2]*Y[1]; db[0]=b[0]=Y[1]-Y[2]; dc[0]=c[0]=X[2]-X[1];
    da[1]=a[1]=X[2]*Y[0]-X[0]*Y[2]; db[1]=b[1]=Y[2]-Y[0]; dc[1]=c[1]=X[0]-X[2];
    da[2]=a[2]=X[0]*Y[1]-X[1]*Y[0]; db[2]=b[2]=Y[0]-Y[1]; dc[2]=c[2]=X[1]-X[0];
    Ae=0.5*(c[2]*b[1]-c[1]*b[2]); /* triangle signed area (determinant) */

    d=0.5/(Ae*Ae); /* common denominator */
    da[0]*=d; da[1]*=d; da[2]*=d;
    db[0]*=d; db[1]*=d; db[2]*=d;
    dc[0]*=d; dc[1]*=d; dc[2]*=d;

    t=a[0]-Ae; Ne[0][0]=da[0]*t; t+=a[0]; Ne[0][1]=db[0]*t; Ne[0][2]=dc[0]*t;
    t=a[1]-Ae; Ne[1][0]=da[1]*t; t+=a[1]; Ne[1][1]=db[1]*t; Ne[1][2]=dc[1]*t;
    t=a[2]-Ae; Ne[2][0]=da[2]*t; t+=a[2]; Ne[2][1]=db[2]*t; Ne[2][2]=dc[2]*t;
    Ne[0][3]=db[0]*b[0]; Ne[0][5]=dc[0]*c[0]; c[0]+=c[0]; Ne[0][4]=db[0]*c[0];
    Ne[1][3]=db[1]*b[1]; Ne[1][5]=dc[1]*c[1]; c[1]+=c[1]; Ne[1][4]=db[1]*c[1];
    Ne[2][3]=db[2]*b[2]; Ne[2][5]=dc[2]*c[2]; c[2]+=c[2]; Ne[2][4]=db[2]*c[2];
    a[0]+=a[0]; a[1]+=a[1]; a[2]+=a[2]; b[0]+=b[0]; b[1]+=b[1]; b[2]+=b[2];
    /* new a[], b[], c[] all doubled (*2 for the following calculation) */
    Ne[3][0]=da[0]*a[1];
    Ne[3][1]=da[0]*b[1]+db[0]*a[1]; Ne[3][2]=da[0]*c[1]+dc[0]*a[1];
    Ne[3][3]=db[0]*b[1]; Ne[3][4]=db[0]*c[1]+dc[0]*b[1]; Ne[3][5]=dc[0]*c[1];
    Ne[4][0]=da[1]*a[2];
    Ne[4][1]=da[1]*b[2]+db[1]*a[2]; Ne[4][2]=da[1]*c[2]+dc[1]*a[2];
    Ne[4][3]=db[1]*b[2]; Ne[4][4]=db[1]*c[2]+dc[1]*b[2]; Ne[4][5]=dc[1]*c[2];
    Ne[5][0]=da[2]*a[0];
    Ne[5][1]=da[2]*b[0]+db[2]*a[0]; Ne[5][2]=da[2]*c[0]+dc[2]*a[0];
    Ne[5][3]=db[2]*b[0]; Ne[5][4]=db[2]*c[0]+dc[2]*b[0]; Ne[5][5]=dc[2]*c[0];
    return;
}
/* -------------------------------------------------------------------------- */
/* Derivatives {Nx}=div{N}/div(x), {Ny}=div{N}/div(y);*/
/* if NeDx and NeDy are defined with "static", default initializer is Zero;
     http://en.wikipedia.org/wiki/C_syntax & Static_variable */
void FnShapeTri1Drv(double NeDx[3][3], double NeDy[3][3], double Ne[3][3])
{ /* (static) double NeDx[3][3], NeDy[3][3]; FnShapeTri1Drv(NeDx,NeDy,Ne); */
    NeDx[0][0]=Ne[0][1]; NeDx[1][0]=Ne[1][1]; NeDx[2][0]=Ne[2][1];
    NeDy[0][0]=Ne[0][2]; NeDy[1][0]=Ne[1][2]; NeDy[2][0]=Ne[2][2];
    NeDx[0][1]=NeDx[0][2]=0; NeDx[1][1]=NeDx[1][2]=0; NeDx[2][1]=NeDx[2][2]=0;
    NeDy[0][1]=NeDy[0][2]=0; NeDy[1][1]=NeDy[1][2]=0; NeDy[2][1]=NeDy[2][2]=0;
    return; /* (Eq.(1.75) & Table 1.2) of Koshiba book1992 */
}
void FnShapeTri2Drv(double NeDx[6][6], double NeDy[6][6], double Ne[6][6])
{ /* (static) double NeDx[6][6], NeDy[6][6]; FnShapeTri2Drv(NeDx,NeDy,Ne); */
    NeDx[0][0]=Ne[0][1]; NeDx[0][1]=Ne[0][3]+Ne[0][3]; NeDx[0][2]=Ne[0][4];
    NeDx[1][0]=Ne[1][1]; NeDx[1][1]=Ne[1][3]+Ne[1][3]; NeDx[1][2]=Ne[1][4];
    NeDx[2][0]=Ne[2][1]; NeDx[2][1]=Ne[2][3]+Ne[2][3]; NeDx[2][2]=Ne[2][4];
    NeDx[3][0]=Ne[3][1]; NeDx[3][1]=Ne[3][3]+Ne[3][3]; NeDx[3][2]=Ne[3][4];
    NeDx[4][0]=Ne[4][1]; NeDx[4][1]=Ne[4][3]+Ne[4][3]; NeDx[4][2]=Ne[4][4];
    NeDx[5][0]=Ne[5][1]; NeDx[5][1]=Ne[5][3]+Ne[5][3]; NeDx[5][2]=Ne[5][4];
    NeDy[0][0]=Ne[0][2]; NeDy[0][1]=Ne[0][4]; NeDy[0][2]=Ne[0][5]+Ne[0][5];
    NeDy[1][0]=Ne[1][2]; NeDy[1][1]=Ne[1][4]; NeDy[1][2]=Ne[1][5]+Ne[1][5];
    NeDy[2][0]=Ne[2][2]; NeDy[2][1]=Ne[2][4]; NeDy[2][2]=Ne[2][5]+Ne[2][5];
    NeDy[3][0]=Ne[3][2]; NeDy[3][1]=Ne[3][4]; NeDy[3][2]=Ne[3][5]+Ne[3][5];
    NeDy[4][0]=Ne[4][2]; NeDy[4][1]=Ne[4][4]; NeDy[4][2]=Ne[4][5]+Ne[4][5];
    NeDy[5][0]=Ne[5][2]; NeDy[5][1]=Ne[5][4]; NeDy[5][2]=Ne[5][5]+Ne[5][5];
    NeDx[0][3]=NeDx[0][4]=NeDx[0][5]=0;
    NeDx[1][3]=NeDx[1][4]=NeDx[1][5]=0;
    NeDx[2][3]=NeDx[2][4]=NeDx[2][5]=0;
    NeDx[3][3]=NeDx[3][4]=NeDx[3][5]=0;
    NeDx[4][3]=NeDx[4][4]=NeDx[4][5]=0;
    NeDx[5][3]=NeDx[5][4]=NeDx[5][5]=0;
    NeDy[0][3]=NeDy[0][4]=NeDy[0][5]=0;
    NeDy[1][3]=NeDy[1][4]=NeDy[1][5]=0;
    NeDy[2][3]=NeDy[2][4]=NeDy[2][5]=0;
    NeDy[3][3]=NeDy[3][4]=NeDy[3][5]=0;
    NeDy[4][3]=NeDy[4][4]=NeDy[4][5]=0;
    NeDy[5][3]=NeDy[5][4]=NeDy[5][5]=0;
    return; /* (Eq.(1.75) & Table 1.2) of Koshiba book1992 */
}
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Integrals (Shape Functions & Derivatives) 1st order triangular nodal element */
void IntTri1NNT(double NNT[3][3],\
                double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: double NNT[3][3]; IntTri1NNT(NNT,X1,Y1,X2,Y2,X3,Y3); */
    double Ae2, b[3], c[3];

    b[0]=Y2-Y3; c[0]=X3-X2;
    b[1]=Y3-Y1; c[1]=X1-X3;
    b[2]=Y1-Y2; c[2]=X2-X1;
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    NNT[0][0]=NNT[1][1]=NNT[2][2]=Ae2/12;
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=Ae2/24;
    return;
}
void IntTri1(double NNT[3][3], double NxNxT[3][3], double NyNyT[3][3],\
             double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: double NNT[3][3], NxNxT[3][3], NyNyT[3][3];
     IntTri1(NNT,NxNxT,NyNyT,X1,Y1,X2,Y2,X3,Y3); */
    double Ae2, b[3], c[3], d, db[3], dc[3];

    db[0]=b[0]=Y2-Y3; dc[0]=c[0]=X3-X2;
    db[1]=b[1]=Y3-Y1; dc[1]=c[1]=X1-X3;
    db[2]=b[2]=Y1-Y2; dc[2]=c[2]=X2-X1;
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    NNT[0][0]=NNT[1][1]=NNT[2][2]=Ae2/12;
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=Ae2/24;

    d=0.5/Ae2; /* 1/(2*Ae2) */
    db[0]*=d; db[1]*=d; db[2]*=d;
    dc[0]*=d; dc[1]*=d; dc[2]*=d;

    NxNxT[0][0]=db[0]*b[0];
    NxNxT[1][1]=db[1]*b[1];
    NxNxT[2][2]=db[2]*b[2];
    NxNxT[0][1]=NxNxT[1][0]=db[0]*b[1];
    NxNxT[0][2]=NxNxT[2][0]=db[0]*b[2];
    NxNxT[1][2]=NxNxT[2][1]=db[1]*b[2];

    NyNyT[0][0]=dc[0]*c[0];
    NyNyT[1][1]=dc[1]*c[1];
    NyNyT[2][2]=dc[2]*c[2];
    NyNyT[0][1]=NyNyT[1][0]=dc[0]*c[1];
    NyNyT[0][2]=NyNyT[2][0]=dc[0]*c[2];
    NyNyT[1][2]=NyNyT[2][1]=dc[1]*c[2];
    return;
}
void IntTri1Drv(double NNT[3][3], double NxNxT[3][3], double NyNyT[3][3],\
         double NxNyT[3][3], double NyNxT[3][3],\
         double NxNT[3][3], double NyNT[3][3],\
         double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: double NNT[3][3], NxNxT[3][3], NyNyT[3][3];
     double NxNyT[3][3], NyNxT[3][3], NxNT[3][3], NyNT[3][3];
     IntTri1Drv(NNT,NxNxT,NyNyT,NxNyT,NyNxT,NxNT,NyNT,X1,Y1,X2,Y2,X3,Y3); */
    double Ae2, b[3], c[3], d, db[3], dc[3];

    db[0]=b[0]=Y2-Y3; dc[0]=c[0]=X3-X2;
    db[1]=b[1]=Y3-Y1; dc[1]=c[1]=X1-X3;
    db[2]=b[2]=Y1-Y2; dc[2]=c[2]=X2-X1;
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    NNT[0][0]=NNT[1][1]=NNT[2][2]=Ae2/12;
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=Ae2/24;

    d=0.5/Ae2; /* 1/(2*Ae2) */
    db[0]*=d; db[1]*=d; db[2]*=d;
    dc[0]*=d; dc[1]*=d; dc[2]*=d;

    NxNxT[0][0]=db[0]*b[0];
    NxNxT[1][1]=db[1]*b[1];
    NxNxT[2][2]=db[2]*b[2];
    NxNxT[0][1]=NxNxT[1][0]=db[0]*b[1];
    NxNxT[0][2]=NxNxT[2][0]=db[0]*b[2];
    NxNxT[1][2]=NxNxT[2][1]=db[1]*b[2];

    NyNyT[0][0]=dc[0]*c[0];
    NyNyT[1][1]=dc[1]*c[1];
    NyNyT[2][2]=dc[2]*c[2];
    NyNyT[0][1]=NyNyT[1][0]=dc[0]*c[1];
    NyNyT[0][2]=NyNyT[2][0]=dc[0]*c[2];
    NyNyT[1][2]=NyNyT[2][1]=dc[1]*c[2];

    NxNyT[0][0]=db[0]*c[0]; NxNyT[0][1]=db[0]*c[1]; NxNyT[0][2]=db[0]*c[2];
    NxNyT[1][0]=db[1]*c[0]; NxNyT[1][1]=db[1]*c[1]; NxNyT[1][2]=db[1]*c[2];
    NxNyT[2][0]=db[2]*c[0]; NxNyT[2][1]=db[2]*c[1]; NxNyT[2][2]=db[2]*c[2];

    NyNxT[0][0]=dc[0]*b[0]; NyNxT[0][1]=dc[0]*b[1]; NyNxT[0][2]=dc[0]*b[2];
    NyNxT[1][0]=dc[1]*b[0]; NyNxT[1][1]=dc[1]*b[1]; NyNxT[1][2]=dc[1]*b[2];
    NyNxT[2][0]=dc[2]*b[0]; NyNxT[2][1]=dc[2]*b[1]; NyNxT[2][2]=dc[2]*b[2];

    NxNT[0][0]=NxNT[0][1]=NxNT[0][2]=b[0]/6;
    NxNT[1][0]=NxNT[1][1]=NxNT[1][2]=b[1]/6;
    NxNT[2][0]=NxNT[2][1]=NxNT[2][2]=b[2]/6;

    NyNT[0][0]=NyNT[0][1]=NyNT[0][2]=c[0]/6;
    NyNT[1][0]=NyNT[1][1]=NyNT[1][2]=c[1]/6;
    NyNT[2][0]=NyNT[2][1]=NyNT[2][2]=c[2]/6;
    return;
}
/* -------------------------------------------------------------------------- */
/* array form of Integrals (1st order (linear) Shape Functions & Derivatives) */
void IntTRI1NNT(double NNT[3][3], double *X, double *Y)
{ /* double NNT[3][3]; IntTRI1NNT(NNT,X,Y); or IntTRI1NNT(NNT,&X[0],&Y[0]); */
    double Ae2, b[3], c[3];

    b[0]=Y[1]-Y[2]; c[0]=X[2]-X[1];
    b[1]=Y[2]-Y[0]; c[1]=X[0]-X[2];
    b[2]=Y[0]-Y[1]; c[2]=X[1]-X[0];
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    NNT[0][0]=NNT[1][1]=NNT[2][2]=Ae2/12;
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=Ae2/24;
    return;
}
void IntTRI1(double NNT[3][3], double NxNxT[3][3], double NyNyT[3][3],\
             double *X, double *Y)
{ /* calling syntax: double NNT[3][3], NxNxT[3][3], NyNyT[3][3];
     IntTRI1(NNT,NxNxT,NyNyT,X,Y); or IntTRI1(NNT,NxNxT,NyNyT,&X[0],&Y[0]); */
    double Ae2, b[3], c[3], d, db[3], dc[3];

    db[0]=b[0]=Y[1]-Y[2]; dc[0]=c[0]=X[2]-X[1];
    db[1]=b[1]=Y[2]-Y[0]; dc[1]=c[1]=X[0]-X[2];
    db[2]=b[2]=Y[0]-Y[1]; dc[2]=c[2]=X[1]-X[0];
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    NNT[0][0]=NNT[1][1]=NNT[2][2]=Ae2/12;
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=Ae2/24;

    d=0.5/Ae2; /* 1/(2*Ae2) */
    db[0]*=d; db[1]*=d; db[2]*=d;
    dc[0]*=d; dc[1]*=d; dc[2]*=d;

    NxNxT[0][0]=db[0]*b[0];
    NxNxT[1][1]=db[1]*b[1];
    NxNxT[2][2]=db[2]*b[2];
    NxNxT[0][1]=NxNxT[1][0]=db[0]*b[1];
    NxNxT[0][2]=NxNxT[2][0]=db[0]*b[2];
    NxNxT[1][2]=NxNxT[2][1]=db[1]*b[2];

    NyNyT[0][0]=dc[0]*c[0];
    NyNyT[1][1]=dc[1]*c[1];
    NyNyT[2][2]=dc[2]*c[2];
    NyNyT[0][1]=NyNyT[1][0]=dc[0]*c[1];
    NyNyT[0][2]=NyNyT[2][0]=dc[0]*c[2];
    NyNyT[1][2]=NyNyT[2][1]=dc[1]*c[2];
    return;
}
void IntTRI1Drv(double NNT[3][3], double NxNxT[3][3], double NyNyT[3][3],\
                double NxNyT[3][3], double NyNxT[3][3],\
                double NxNT[3][3], double NyNT[3][3],\
                double *X, double *Y)
{ /* calling syntax: double NNT[3][3], NxNxT[3][3], NyNyT[3][3];
     double NxNyT[3][3], NyNxT[3][3], NxNT[3][3], NyNT[3][3];
     IntTRI1Drv(NNT,NxNxT,NyNyT,NxNyT,NyNxT,NxNT,NyNT,X,Y); or
     IntTRI1Drv(NNT,NxNxT,NyNyT,NxNyT,NyNxT,NxNT,NyNT,&X[0],&Y[0]); */
    double Ae2, b[3], c[3], d, db[3], dc[3];

    db[0]=b[0]=Y[1]-Y[2]; dc[0]=c[0]=X[2]-X[1];
    db[1]=b[1]=Y[2]-Y[0]; dc[1]=c[1]=X[0]-X[2];
    db[2]=b[2]=Y[0]-Y[1]; dc[2]=c[2]=X[1]-X[0];
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    NNT[0][0]=NNT[1][1]=NNT[2][2]=Ae2/12;
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=Ae2/24;

    d=0.5/Ae2; /* 1/(2*Ae2) */
    db[0]*=d; db[1]*=d; db[2]*=d;
    dc[0]*=d; dc[1]*=d; dc[2]*=d;

    NxNxT[0][0]=db[0]*b[0];
    NxNxT[1][1]=db[1]*b[1];
    NxNxT[2][2]=db[2]*b[2];
    NxNxT[0][1]=NxNxT[1][0]=db[0]*b[1];
    NxNxT[0][2]=NxNxT[2][0]=db[0]*b[2];
    NxNxT[1][2]=NxNxT[2][1]=db[1]*b[2];

    NyNyT[0][0]=dc[0]*c[0];
    NyNyT[1][1]=dc[1]*c[1];
    NyNyT[2][2]=dc[2]*c[2];
    NyNyT[0][1]=NyNyT[1][0]=dc[0]*c[1];
    NyNyT[0][2]=NyNyT[2][0]=dc[0]*c[2];
    NyNyT[1][2]=NyNyT[2][1]=dc[1]*c[2];

    NxNyT[0][0]=db[0]*c[0]; NxNyT[0][1]=db[0]*c[1]; NxNyT[0][2]=db[0]*c[2];
    NxNyT[1][0]=db[1]*c[0]; NxNyT[1][1]=db[1]*c[1]; NxNyT[1][2]=db[1]*c[2];
    NxNyT[2][0]=db[2]*c[0]; NxNyT[2][1]=db[2]*c[1]; NxNyT[2][2]=db[2]*c[2];

    NyNxT[0][0]=dc[0]*b[0]; NyNxT[0][1]=dc[0]*b[1]; NyNxT[0][2]=dc[0]*b[2];
    NyNxT[1][0]=dc[1]*b[0]; NyNxT[1][1]=dc[1]*b[1]; NyNxT[1][2]=dc[1]*b[2];
    NyNxT[2][0]=dc[2]*b[0]; NyNxT[2][1]=dc[2]*b[1]; NyNxT[2][2]=dc[2]*b[2];

    NxNT[0][0]=NxNT[0][1]=NxNT[0][2]=b[0]/6;
    NxNT[1][0]=NxNT[1][1]=NxNT[1][2]=b[1]/6;
    NxNT[2][0]=NxNT[2][1]=NxNT[2][2]=b[2]/6;

    NyNT[0][0]=NyNT[0][1]=NyNT[0][2]=c[0]/6;
    NyNT[1][0]=NyNT[1][1]=NyNT[1][2]=c[1]/6;
    NyNT[2][0]=NyNT[2][1]=NyNT[2][2]=c[2]/6;
    return;
}
/* -------------------------------------------------------------------------- */
/* Integrals (Shape Functions & Derivatives): 2nd order triangular nodal element */
void IntTri2NNT(double NNT[6][6],\
                double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: double NNT[6][6]; IntTri2NNT(NNT,X1,Y1,X2,Y2,X3,Y3); */
    double Ae2, b[3], c[3], d; /* Eq.(1.91) of Koshiba book1992 */

    b[0]=Y2-Y3; c[0]=X3-X2;
    b[1]=Y3-Y1; c[1]=X1-X3;
    b[2]=Y1-Y2; c[2]=X2-X1;
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    /* double N[6][6]={{6,-1,-1,0,-4,0},{-1,6,-1,0,0,-4},{-1,-1,6,-4,0,0},
                    {0,0,-4,32,16,16},{-4,0,0,16,32,16},{0,-4,0,16,16,32}};
    unsigned int i, j;
    for (i=0;i<6;i++) { for (j=0;j<6;j++) NNT[i][j]=N[i][j]*d; } */
    d=Ae2/360; /* Ae/180 */
    NNT[0][0]=NNT[1][1]=NNT[2][2]=6*d;
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=-d;
    NNT[0][4]=NNT[1][5]=NNT[2][3]=NNT[3][2]=NNT[4][0]=NNT[5][1]=-4*d;
    NNT[0][3]=NNT[0][5]=NNT[1][3]=NNT[1][4]=NNT[2][4]=NNT[2][5]=0;
    NNT[3][0]=NNT[3][1]=NNT[4][1]=NNT[4][2]=NNT[5][0]=NNT[5][2]=0;
    NNT[3][3]=NNT[4][4]=NNT[5][5]=32*d;
    NNT[3][4]=NNT[3][5]=NNT[4][3]=NNT[4][5]=NNT[5][3]=NNT[5][4]=16*d;
    return;
}
void IntTri2(double NNT[6][6], double NxNxT[6][6], double NyNyT[6][6],\
             double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: double NNT[6][6], NxNxT[6][6], NyNyT[6][6];
     IntTri2(NNT,NxNxT,NyNyT,X1,Y1,X2,Y2,X3,Y3); */
    unsigned int i, j;
    double Ae2, b[3], c[3], d, d4, Ae_4, Ae_12; /* double Ae, Ae_6, Ae_3; */
    double Cx[6][4]={0}, Cy[6][4]={0}; /* Table 1.4 of Koshiba book1992 */
    double CxR[6]={0}, CyR[6]={0}; /* sum of each row of Cx and Cy */
    /* unspecified elements are set to 0 if short initialization is specified */

    b[0]=Y2-Y3; c[0]=X3-X2;
    b[1]=Y3-Y1; c[1]=X1-X3;
    b[2]=Y1-Y2; c[2]=X2-X1;
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=Ae2/360; /* Ae/180 */
    NNT[0][0]=NNT[1][1]=NNT[2][2]=6*d; /* Eq.(1.91) of Koshiba book1992 */
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=-d;
    NNT[0][4]=NNT[1][5]=NNT[2][3]=NNT[3][2]=NNT[4][0]=NNT[5][1]=-4*d;
    NNT[0][3]=NNT[0][5]=NNT[1][3]=NNT[1][4]=NNT[2][4]=NNT[2][5]=0;
    NNT[3][0]=NNT[3][1]=NNT[4][1]=NNT[4][2]=NNT[5][0]=NNT[5][2]=0;
    NNT[3][3]=NNT[4][4]=NNT[5][5]=32*d;
    NNT[3][4]=NNT[3][5]=NNT[4][3]=NNT[4][5]=NNT[5][3]=NNT[5][4]=16*d;

    d=1/Ae2; /* common denominator 1/(2*Ae) */
    d4=4*d; Ae_4=Ae2/8; Ae_12=Ae2/24; /* Ae=0.5*Ae2; */
    /* all other elements in Cx and Cy are 0 due to short initialization */
    Cx[0][3]=-d*b[0]; Cx[1][3]=-d*b[1]; Cx[2][3]=-d*b[2];
    Cx[0][0]=Cx[3][1]=Cx[5][2]=d4*b[0];
    Cx[1][1]=Cx[3][0]=Cx[4][2]=d4*b[1];
    Cx[2][2]=Cx[4][1]=Cx[5][0]=d4*b[2];
    Cy[0][3]=-d*c[0]; Cy[1][3]=-d*c[1]; Cy[2][3]=-d*c[2];
    Cy[0][0]=Cy[3][1]=Cy[5][2]=d4*c[0];
    Cy[1][1]=Cy[3][0]=Cy[4][2]=d4*c[1];
    Cy[2][2]=Cy[4][1]=Cy[5][0]=d4*c[2];
    /* for (i=0; i<6; i++) {
        CxR[i]=Cx[i][0]+Cx[i][1]+Cx[i][2]+Cx[i][3];
        CyR[i]=Cy[i][0]+Cy[i][1]+Cy[i][2]+Cy[i][3];
     } */ /* from Table 1.4 of Koshiba book1992 */
    CxR[0]=Cx[0][0]+Cx[0][3]; CxR[1]=Cx[1][1]+Cx[1][3]; CxR[2]=Cx[2][2]+Cx[2][3];
    CxR[3]=Cx[3][0]+Cx[3][1]; CxR[4]=Cx[4][1]+Cx[4][2]; CxR[5]=Cx[5][0]+Cx[5][2];
    CyR[0]=Cy[0][0]+Cy[0][3]; CyR[1]=Cy[1][1]+Cy[1][3]; CyR[2]=Cy[2][2]+Cy[2][3];
    CyR[3]=Cy[3][0]+Cy[3][1]; CyR[4]=Cy[4][1]+Cy[4][2]; CyR[5]=Cy[5][0]+Cy[5][2];
    /* (NxNxT,NyNyT) are symmetric matrices (M[i][j]=M[j][i]) */
    for (i=0; i<6; i++) { /* simplified Eq.(1.92) of Koshiba book1992 */
        for (j=i; j<6; j++) { /* NOTE: initial j=i; for symmetric matrices; */
            /* NxNxT[i][j]=NxNxT[j][i]=
                Ae_6*(Cx[i][0]*Cx[j][0]+Cx[i][1]*Cx[j][1]+Cx[i][2]*Cx[j][2])
              +Ae_12*(Cx[i][0]*(Cx[j][1]+Cx[j][2])+Cx[i][1]*(Cx[j][0]+Cx[j][2])
                     +Cx[i][2]*(Cx[j][0]+Cx[j][1]))
              + Ae_3*((Cx[i][0]+Cx[i][1]+Cx[i][2])*Cx[j][3]
                     +Cx[i][3]*(Cx[j][0]+Cx[j][1]+Cx[j][2]))
              + Ae*Cx[i][3]*Cx[j][3]; */
            /* NyNyT[i][j]=NyNyT[j][i]=
                Ae_6*(Cy[i][0]*Cy[j][0]+Cy[i][1]*Cy[j][1]+Cy[i][2]*Cy[j][2])
              +Ae_12*(Cy[i][0]*(Cy[j][1]+Cy[j][2])+Cy[i][1]*(Cy[j][0]+Cy[j][2])
                     +Cy[i][2]*(Cy[j][0]+Cy[j][1]))
              + Ae_3*((Cy[i][0]+Cy[i][1]+Cy[i][2])*Cy[j][3]
                     +Cy[i][3]*(Cy[j][0]+Cy[j][1]+Cy[j][2]))
              + Ae*Cy[i][3]*Cy[j][3]; */
            /* NxNxT[i][j]=NxNxT[j][i]=
                Ae_12*((CxR[i]-Cx[i][3])*(CxR[j]-Cx[j][3])
                       +Cx[i][0]*Cx[j][0]+Cx[i][1]*Cx[j][1]+Cx[i][2]*Cx[j][2])
              + Ae_3*(CxR[i]*Cx[j][3]+Cx[i][3]*CxR[j]+Cx[i][3]*Cx[j][3]); */
            /* NyNyT[i][j]=NyNyT[j][i]=
                Ae_12*((CyR[i]-Cy[i][3])*(CyR[j]-Cy[j][3])
                       +Cy[i][0]*Cy[j][0]+Cy[i][1]*Cy[j][1]+Cy[i][2]*Cy[j][2])
              + Ae_3*(CyR[i]*Cy[j][3]+Cy[i][3]*CyR[j]+Cy[i][3]*Cy[j][3]); */
            NxNxT[i][j]=NxNxT[j][i]=
                Ae_12*(CxR[i]*CxR[j]+5*Cx[i][3]*Cx[j][3]
                      +Cx[i][0]*Cx[j][0]+Cx[i][1]*Cx[j][1]+Cx[i][2]*Cx[j][2])
              + Ae_4*(CxR[i]*Cx[j][3]+Cx[i][3]*CxR[j]);
            NyNyT[i][j]=NyNyT[j][i]=
                Ae_12*(CyR[i]*CyR[j]+5*Cy[i][3]*Cy[j][3]
                      +Cy[i][0]*Cy[j][0]+Cy[i][1]*Cy[j][1]+Cy[i][2]*Cy[j][2])
              + Ae_4*(CyR[i]*Cy[j][3]+Cy[i][3]*CyR[j]);
        }
    }
    return;
}
void IntTri2Drv(double NNT[6][6], double NxNxT[6][6], double NyNyT[6][6],\
         double NxNyT[6][6], double NyNxT[6][6],\
         double NxNT[6][6], double NyNT[6][6],\
         double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: double NNT[6][6], NxNxT[6][6], NyNyT[6][6];
     double NxNyT[6][6], NyNxT[6][6], NxNT[6][6], NyNT[6][6];
     IntTri2Drv(NNT,NxNxT,NyNyT,NxNyT,NyNxT,NxNT,NyNT,X1,Y1,X2,Y2,X3,Y3); */
    unsigned int i, j;
    double Ae2, b[3], c[3], d, d4, Ae_4, Ae_12, Ae_15, Ae_60;
    double Cx[6][4]={0}, Cy[6][4]={0}; /* Table 1.4 of Koshiba book1992 */
    double CxR[6]={0}, CyR[6]={0}; /* sum of each row of Cx and Cy */
    /* unspecified elements are set to 0 if short initialization is specified */

    b[0]=Y2-Y3; c[0]=X3-X2;
    b[1]=Y3-Y1; c[1]=X1-X3;
    b[2]=Y1-Y2; c[2]=X2-X1;
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=Ae2/360; /* Ae/180 */
    NNT[0][0]=NNT[1][1]=NNT[2][2]=6*d; /* Eq.(1.91) of Koshiba book1992 */
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=-d;
    NNT[0][4]=NNT[1][5]=NNT[2][3]=NNT[3][2]=NNT[4][0]=NNT[5][1]=-4*d;
    NNT[0][3]=NNT[0][5]=NNT[1][3]=NNT[1][4]=NNT[2][4]=NNT[2][5]=0;
    NNT[3][0]=NNT[3][1]=NNT[4][1]=NNT[4][2]=NNT[5][0]=NNT[5][2]=0;
    NNT[3][3]=NNT[4][4]=NNT[5][5]=32*d;
    NNT[3][4]=NNT[3][5]=NNT[4][3]=NNT[4][5]=NNT[5][3]=NNT[5][4]=16*d;

    d=1/Ae2; /* common denominator 1/(2*Ae) */
    d4=4*d; Ae_4=Ae2/8; Ae_12=Ae2/24; Ae_15=Ae2/30; Ae_60=Ae_15/4; /* Ae=Ae2/2; */
    /* all other elements in Cx and Cy are 0 due to short initialization */
    Cx[0][3]=-d*b[0]; Cx[1][3]=-d*b[1]; Cx[2][3]=-d*b[2];
    Cx[0][0]=Cx[3][1]=Cx[5][2]=d4*b[0];
    Cx[1][1]=Cx[3][0]=Cx[4][2]=d4*b[1];
    Cx[2][2]=Cx[4][1]=Cx[5][0]=d4*b[2];
    Cy[0][3]=-d*c[0]; Cy[1][3]=-d*c[1]; Cy[2][3]=-d*c[2];
    Cy[0][0]=Cy[3][1]=Cy[5][2]=d4*c[0];
    Cy[1][1]=Cy[3][0]=Cy[4][2]=d4*c[1];
    Cy[2][2]=Cy[4][1]=Cy[5][0]=d4*c[2];
    CxR[0]=Cx[0][0]+Cx[0][3]; CxR[1]=Cx[1][1]+Cx[1][3]; CxR[2]=Cx[2][2]+Cx[2][3];
    CxR[3]=Cx[3][0]+Cx[3][1]; CxR[4]=Cx[4][1]+Cx[4][2]; CxR[5]=Cx[5][0]+Cx[5][2];
    CyR[0]=Cy[0][0]+Cy[0][3]; CyR[1]=Cy[1][1]+Cy[1][3]; CyR[2]=Cy[2][2]+Cy[2][3];
    CyR[3]=Cy[3][0]+Cy[3][1]; CyR[4]=Cy[4][1]+Cy[4][2]; CyR[5]=Cy[5][0]+Cy[5][2];
    for (i=0; i<6; i++) { /* simplified Eq.(1.92) of Koshiba book1992 */
        /* (NxNxT,NyNyT) are symmetric matrices (M[i][j]=M[j][i]) */
        for (j=i; j<6; j++) { /* NOTE: initial j=i; for symmetric matrices; */
            NxNxT[i][j]=NxNxT[j][i]=
                Ae_12*(CxR[i]*CxR[j]+5*Cx[i][3]*Cx[j][3]
                      +Cx[i][0]*Cx[j][0]+Cx[i][1]*Cx[j][1]+Cx[i][2]*Cx[j][2])
              + Ae_4*(CxR[i]*Cx[j][3]+Cx[i][3]*CxR[j]);
            NyNyT[i][j]=NyNyT[j][i]=
                Ae_12*(CyR[i]*CyR[j]+5*Cy[i][3]*Cy[j][3]
                      +Cy[i][0]*Cy[j][0]+Cy[i][1]*Cy[j][1]+Cy[i][2]*Cy[j][2])
              + Ae_4*(CyR[i]*Cy[j][3]+Cy[i][3]*CyR[j]);
        }
        /* (NxNyT,NyNxT,NxNT,NyNT) are NOT symmetric matrices (M[i][j]!=M[j][i]) */
        for (j=0; j<6; j++) { /* simplification details see IntTri2() */
            NxNyT[i][j]=
                Ae_12*(CxR[i]*CyR[j]+5*Cx[i][3]*Cy[j][3]
                      +Cx[i][0]*Cy[j][0]+Cx[i][1]*Cy[j][1]+Cx[i][2]*Cy[j][2])
              + Ae_4*(CxR[i]*Cy[j][3]+Cx[i][3]*CyR[j]);
            NyNxT[i][j]=
                Ae_12*(CyR[i]*CxR[j]+5*Cy[i][3]*Cx[j][3]
                      +Cy[i][0]*Cx[j][0]+Cy[i][1]*Cx[j][1]+Cy[i][2]*Cx[j][2])
              + Ae_4*(CyR[i]*Cx[j][3]+Cy[i][3]*CxR[j]);
        }
        NxNT[i][0]=Ae_60*(Cx[i][0]-Cx[i][1]+Cx[i][0]-Cx[i][2]); /* Eq.(1.93) */
        NxNT[i][1]=Ae_60*(Cx[i][1]-Cx[i][0]+Cx[i][1]-Cx[i][2]); /* Eq.(1.94) */
        NxNT[i][2]=Ae_60*(Cx[i][2]-Cx[i][0]+Cx[i][2]-Cx[i][1]); /* Eq.(1.95) */
        d=CxR[i]+CxR[i]+3*Cx[i][3]; /* 2*CxR[i]+3*Cx[i][3]; */
        NxNT[i][3]=Ae_15*(d-Cx[i][2]); /* Eq.(1.96) */
        NxNT[i][4]=Ae_15*(d-Cx[i][0]); /* Eq.(1.97) */
        NxNT[i][5]=Ae_15*(d-Cx[i][1]); /* Eq.(1.98) */
        NyNT[i][0]=Ae_60*(Cy[i][0]-Cy[i][1]+Cy[i][0]-Cy[i][2]); /* Eq.(1.93) */
        NyNT[i][1]=Ae_60*(Cy[i][1]-Cy[i][0]+Cy[i][1]-Cy[i][2]); /* Eq.(1.94) */
        NyNT[i][2]=Ae_60*(Cy[i][2]-Cy[i][0]+Cy[i][2]-Cy[i][1]); /* Eq.(1.95) */
        d=CyR[i]+CyR[i]+3*Cy[i][3]; /* 2*CyR[i]+3*Cy[i][3]; */
        NyNT[i][3]=Ae_15*(d-Cy[i][2]); /* Eq.(1.96) */
        NyNT[i][4]=Ae_15*(d-Cy[i][0]); /* Eq.(1.97) */
        NyNT[i][5]=Ae_15*(d-Cy[i][1]); /* Eq.(1.98) */
    }
    return;
}
/* -------------------------------------------------------------------------- */
/* array form of Integrals (2nd order (quadratic) Shape Functions & Derivatives) */
void IntTRI2NNT(double NNT[6][6], double *X, double *Y)
{ /* double NNT[6][6]; IntTRI2NNT(NNT,X,Y); or IntTRI2NNT(NNT,&X[0],&Y[0]); */
    double Ae2, b[3], c[3], d;

    b[0]=Y[1]-Y[2]; c[0]=X[2]-X[1];
    b[1]=Y[2]-Y[0]; c[1]=X[0]-X[2];
    b[2]=Y[0]-Y[1]; c[2]=X[1]-X[0];
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=Ae2/360; /* Ae/180 */
    NNT[0][0]=NNT[1][1]=NNT[2][2]=6*d;
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=-d;
    NNT[0][4]=NNT[1][5]=NNT[2][3]=NNT[3][2]=NNT[4][0]=NNT[5][1]=-4*d;
    NNT[0][3]=NNT[0][5]=NNT[1][3]=NNT[1][4]=NNT[2][4]=NNT[2][5]=0;
    NNT[3][0]=NNT[3][1]=NNT[4][1]=NNT[4][2]=NNT[5][0]=NNT[5][2]=0;
    NNT[3][3]=NNT[4][4]=NNT[5][5]=32*d;
    NNT[3][4]=NNT[3][5]=NNT[4][3]=NNT[4][5]=NNT[5][3]=NNT[5][4]=16*d;
    return;
}
void IntTRI2(double NNT[6][6], double NxNxT[6][6], double NyNyT[6][6],\
             double *X, double *Y)
{ /* calling syntax: double NNT[6][6], NxNxT[6][6], NyNyT[6][6];
     IntTRI2(NNT,NxNxT,NyNyT,X,Y); or IntTRI2(NNT,NxNxT,NyNyT,&X[0],&Y[0]); */
    unsigned int i, j;
    double Ae2, b[3], c[3], d, d4, Ae_4, Ae_12; /* double Ae, Ae_6, Ae_3; */
    double Cx[6][4]={0}, Cy[6][4]={0}; /* Table 1.4 of Koshiba book1992 */
    double CxR[6]={0}, CyR[6]={0}; /* sum of each row of Cx and Cy */
    /* unspecified elements are set to 0 if short initialization is specified */

    b[0]=Y[1]-Y[2]; c[0]=X[2]-X[1];
    b[1]=Y[2]-Y[0]; c[1]=X[0]-X[2];
    b[2]=Y[0]-Y[1]; c[2]=X[1]-X[0];
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=Ae2/360; /* Ae/180 */
    NNT[0][0]=NNT[1][1]=NNT[2][2]=6*d; /* Eq.(1.91) of Koshiba book1992 */
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=-d;
    NNT[0][4]=NNT[1][5]=NNT[2][3]=NNT[3][2]=NNT[4][0]=NNT[5][1]=-4*d;
    NNT[0][3]=NNT[0][5]=NNT[1][3]=NNT[1][4]=NNT[2][4]=NNT[2][5]=0;
    NNT[3][0]=NNT[3][1]=NNT[4][1]=NNT[4][2]=NNT[5][0]=NNT[5][2]=0;
    NNT[3][3]=NNT[4][4]=NNT[5][5]=32*d;
    NNT[3][4]=NNT[3][5]=NNT[4][3]=NNT[4][5]=NNT[5][3]=NNT[5][4]=16*d;

    d=1/Ae2; /* common denominator 1/(2*Ae) */
    d4=4*d; Ae_4=Ae2/8; Ae_12=Ae2/24; /* Ae=0.5*Ae2; */
    /* all other elements in Cx and Cy are 0 due to short initialization */
    Cx[0][3]=-d*b[0]; Cx[1][3]=-d*b[1]; Cx[2][3]=-d*b[2];
    Cx[0][0]=Cx[3][1]=Cx[5][2]=d4*b[0];
    Cx[1][1]=Cx[3][0]=Cx[4][2]=d4*b[1];
    Cx[2][2]=Cx[4][1]=Cx[5][0]=d4*b[2];
    Cy[0][3]=-d*c[0]; Cy[1][3]=-d*c[1]; Cy[2][3]=-d*c[2];
    Cy[0][0]=Cy[3][1]=Cy[5][2]=d4*c[0];
    Cy[1][1]=Cy[3][0]=Cy[4][2]=d4*c[1];
    Cy[2][2]=Cy[4][1]=Cy[5][0]=d4*c[2];
    CxR[0]=Cx[0][0]+Cx[0][3]; CxR[1]=Cx[1][1]+Cx[1][3]; CxR[2]=Cx[2][2]+Cx[2][3];
    CxR[3]=Cx[3][0]+Cx[3][1]; CxR[4]=Cx[4][1]+Cx[4][2]; CxR[5]=Cx[5][0]+Cx[5][2];
    CyR[0]=Cy[0][0]+Cy[0][3]; CyR[1]=Cy[1][1]+Cy[1][3]; CyR[2]=Cy[2][2]+Cy[2][3];
    CyR[3]=Cy[3][0]+Cy[3][1]; CyR[4]=Cy[4][1]+Cy[4][2]; CyR[5]=Cy[5][0]+Cy[5][2];
    /* (NxNxT,NyNyT) are symmetric matrices (M[i][j]=M[j][i]) */
    for (i=0; i<6; i++) { /* simplified Eq.(1.92) of Koshiba book1992 */
        for (j=i; j<6; j++) { /* NOTE: initial j=i; for symmetric matrices; */
            NxNxT[i][j]=NxNxT[j][i]=
                Ae_12*(CxR[i]*CxR[j]+5*Cx[i][3]*Cx[j][3]
                      +Cx[i][0]*Cx[j][0]+Cx[i][1]*Cx[j][1]+Cx[i][2]*Cx[j][2])
              + Ae_4*(CxR[i]*Cx[j][3]+Cx[i][3]*CxR[j]);
            NyNyT[i][j]=NyNyT[j][i]=
                Ae_12*(CyR[i]*CyR[j]+5*Cy[i][3]*Cy[j][3]
                      +Cy[i][0]*Cy[j][0]+Cy[i][1]*Cy[j][1]+Cy[i][2]*Cy[j][2])
              + Ae_4*(CyR[i]*Cy[j][3]+Cy[i][3]*CyR[j]);
        }
    }
    return;
}
void IntTRI2Drv(double NNT[6][6], double NxNxT[6][6], double NyNyT[6][6],\
                double NxNyT[6][6], double NyNxT[6][6],\
                double NxNT[6][6], double NyNT[6][6],\
                double *X, double *Y)
{ /* calling syntax: double NNT[6][6], NxNxT[6][6], NyNyT[6][6];
     double NxNyT[6][6], NyNxT[6][6], NxNT[6][6], NyNT[6][6];
     IntTRI2Drv(NNT,NxNxT,NyNyT,NxNyT,NyNxT,NxNT,NyNT,X,Y); or
     IntTRI2Drv(NNT,NxNxT,NyNyT,NxNyT,NyNxT,NxNT,NyNT,&X[0],&Y[0]); */
    unsigned int i, j;
    double Ae2, b[3], c[3], d, d4, Ae_4, Ae_12, Ae_15, Ae_60;
    double Cx[6][4]={0}, Cy[6][4]={0}; /* Table 1.4 of Koshiba book1992 */
    double CxR[6]={0}, CyR[6]={0}; /* sum of each row of Cx and Cy */
    /* unspecified elements are set to 0 if short initialization is specified */

    b[0]=Y[1]-Y[2]; c[0]=X[2]-X[1];
    b[1]=Y[2]-Y[0]; c[1]=X[0]-X[2];
    b[2]=Y[0]-Y[1]; c[2]=X[1]-X[0];
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=Ae2/360; /* Ae/180 */
    NNT[0][0]=NNT[1][1]=NNT[2][2]=6*d; /* Eq.(1.91) of Koshiba book1992 */
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=-d;
    NNT[0][4]=NNT[1][5]=NNT[2][3]=NNT[3][2]=NNT[4][0]=NNT[5][1]=-4*d;
    NNT[0][3]=NNT[0][5]=NNT[1][3]=NNT[1][4]=NNT[2][4]=NNT[2][5]=0;
    NNT[3][0]=NNT[3][1]=NNT[4][1]=NNT[4][2]=NNT[5][0]=NNT[5][2]=0;
    NNT[3][3]=NNT[4][4]=NNT[5][5]=32*d;
    NNT[3][4]=NNT[3][5]=NNT[4][3]=NNT[4][5]=NNT[5][3]=NNT[5][4]=16*d;

    d=1/Ae2; /* common denominator 1/(2*Ae) */
    d4=4*d; Ae_4=Ae2/8; Ae_12=Ae2/24; Ae_15=Ae2/30; Ae_60=Ae_15/4; /* Ae=Ae2/2; */
    /* all other elements in Cx and Cy are 0 due to short initialization */
    Cx[0][3]=-d*b[0]; Cx[1][3]=-d*b[1]; Cx[2][3]=-d*b[2];
    Cx[0][0]=Cx[3][1]=Cx[5][2]=d4*b[0];
    Cx[1][1]=Cx[3][0]=Cx[4][2]=d4*b[1];
    Cx[2][2]=Cx[4][1]=Cx[5][0]=d4*b[2];
    Cy[0][3]=-d*c[0]; Cy[1][3]=-d*c[1]; Cy[2][3]=-d*c[2];
    Cy[0][0]=Cy[3][1]=Cy[5][2]=d4*c[0];
    Cy[1][1]=Cy[3][0]=Cy[4][2]=d4*c[1];
    Cy[2][2]=Cy[4][1]=Cy[5][0]=d4*c[2];
    CxR[0]=Cx[0][0]+Cx[0][3]; CxR[1]=Cx[1][1]+Cx[1][3]; CxR[2]=Cx[2][2]+Cx[2][3];
    CxR[3]=Cx[3][0]+Cx[3][1]; CxR[4]=Cx[4][1]+Cx[4][2]; CxR[5]=Cx[5][0]+Cx[5][2];
    CyR[0]=Cy[0][0]+Cy[0][3]; CyR[1]=Cy[1][1]+Cy[1][3]; CyR[2]=Cy[2][2]+Cy[2][3];
    CyR[3]=Cy[3][0]+Cy[3][1]; CyR[4]=Cy[4][1]+Cy[4][2]; CyR[5]=Cy[5][0]+Cy[5][2];
    for (i=0; i<6; i++) { /* simplified Eq.(1.92) of Koshiba book1992 */
        /* (NxNxT,NyNyT) are symmetric matrices (M[i][j]=M[j][i]) */
        for (j=i; j<6; j++) { /* NOTE: initial j=i; for symmetric matrices; */
            NxNxT[i][j]=NxNxT[j][i]=
                Ae_12*(CxR[i]*CxR[j]+5*Cx[i][3]*Cx[j][3]
                      +Cx[i][0]*Cx[j][0]+Cx[i][1]*Cx[j][1]+Cx[i][2]*Cx[j][2])
              + Ae_4*(CxR[i]*Cx[j][3]+Cx[i][3]*CxR[j]);
            NyNyT[i][j]=NyNyT[j][i]=
                Ae_12*(CyR[i]*CyR[j]+5*Cy[i][3]*Cy[j][3]
                      +Cy[i][0]*Cy[j][0]+Cy[i][1]*Cy[j][1]+Cy[i][2]*Cy[j][2])
              + Ae_4*(CyR[i]*Cy[j][3]+Cy[i][3]*CyR[j]);
        }
        /* (NxNyT,NyNxT,NxNT,NyNT) are NOT symmetric matrices (M[i][j]!=M[j][i]) */
        for (j=0; j<6; j++) { /* simplification details see IntTri2() */
            NxNyT[i][j]=
                Ae_12*(CxR[i]*CyR[j]+5*Cx[i][3]*Cy[j][3]
                      +Cx[i][0]*Cy[j][0]+Cx[i][1]*Cy[j][1]+Cx[i][2]*Cy[j][2])
              + Ae_4*(CxR[i]*Cy[j][3]+Cx[i][3]*CyR[j]);
            NyNxT[i][j]=
                Ae_12*(CyR[i]*CxR[j]+5*Cy[i][3]*Cx[j][3]
                      +Cy[i][0]*Cx[j][0]+Cy[i][1]*Cx[j][1]+Cy[i][2]*Cx[j][2])
              + Ae_4*(CyR[i]*Cx[j][3]+Cy[i][3]*CxR[j]);
        }
        NxNT[i][0]=Ae_60*(Cx[i][0]-Cx[i][1]+Cx[i][0]-Cx[i][2]); /* Eq.(1.93) */
        NxNT[i][1]=Ae_60*(Cx[i][1]-Cx[i][0]+Cx[i][1]-Cx[i][2]); /* Eq.(1.94) */
        NxNT[i][2]=Ae_60*(Cx[i][2]-Cx[i][0]+Cx[i][2]-Cx[i][1]); /* Eq.(1.95) */
        d=CxR[i]+CxR[i]+3*Cx[i][3]; /* 2*CxR[i]+3*Cx[i][3]; */
        NxNT[i][3]=Ae_15*(d-Cx[i][2]); /* Eq.(1.96) */
        NxNT[i][4]=Ae_15*(d-Cx[i][0]); /* Eq.(1.97) */
        NxNT[i][5]=Ae_15*(d-Cx[i][1]); /* Eq.(1.98) */
        NyNT[i][0]=Ae_60*(Cy[i][0]-Cy[i][1]+Cy[i][0]-Cy[i][2]); /* Eq.(1.93) */
        NyNT[i][1]=Ae_60*(Cy[i][1]-Cy[i][0]+Cy[i][1]-Cy[i][2]); /* Eq.(1.94) */
        NyNT[i][2]=Ae_60*(Cy[i][2]-Cy[i][0]+Cy[i][2]-Cy[i][1]); /* Eq.(1.95) */
        d=CyR[i]+CyR[i]+3*Cy[i][3]; /* 2*CyR[i]+3*Cy[i][3]; */
        NyNT[i][3]=Ae_15*(d-Cy[i][2]); /* Eq.(1.96) */
        NyNT[i][4]=Ae_15*(d-Cy[i][0]); /* Eq.(1.97) */
        NyNT[i][5]=Ae_15*(d-Cy[i][1]); /* Eq.(1.98) */
    }
    return;
}
/* ========================================================================== */

/* ========================================================================== */
/* Shape Functions ( & Derivatives), Integrals & Element Matrices
 * of triangular mixed elements (\Research2001to2005\FEMBPM\FEMSCFV6\intMix.m) */
/* NOTE: (ShapeNe,ShapeNeDx,ShapeNeDy) same as in nodal case (intTri.m) */
/* Shape Functions and Derivatives omit *[1 x y].' for 1st order mixed elements,
 *     and omit *[1 x y x^2 x*y y^2].' for 2nd order mixed elements; */
/* -------------------------------------------------------------------------- */
void FnShapeMix1UV(double Ue[3][3], double Ve[3][3],\
         double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* double Ue[3][3], Ve[3][3]; FnShapeMix1UV(Ue,Ve,X1,Y1,X2,Y2,X3,Y3); */
    double Ae2, b[3], c[3], l[3], d, ld[3];

    b[0]=Y2-Y3; c[0]=X3-X2;
    b[1]=Y3-Y1; c[1]=X1-X3;
    b[2]=Y1-Y2; c[2]=X2-X1;
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=1/Ae2; /* common denominator 1/(2*Ae) */
    trisgnl(l,b,c); /* signed lengths: Eq.(10) of JLT94Mar495Koshiba; */
    ld[0]=l[0]*d; ld[1]=l[1]*d; ld[2]=l[2]*d;
    Ue[0][0]=ld[0]*Y3; Ue[0][1]=0; Ue[0][2]=-ld[0];
    Ue[1][0]=ld[1]*Y1; Ue[1][1]=0; Ue[1][2]=-ld[1];
    Ue[2][0]=ld[2]*Y2; Ue[2][1]=0; Ue[2][2]=-ld[2];
    Ve[0][0]=-ld[0]*X3; Ue[0][1]=ld[0]; Ue[0][2]=0;
    Ve[1][0]=-ld[1]*X1; Ue[1][1]=ld[1]; Ue[1][2]=0;
    Ve[2][0]=-ld[2]*X2; Ue[2][1]=ld[2]; Ue[2][2]=0;
    return;
}
void FnShapeMix1UVN(double Ue[3][3], double Ve[3][3], double Ne[3][3],\
         double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax: double Ue[3][3], Ve[3][3], Ne[3][3];
     FnShapeMix1UVN(Ue,Ve,Ne,X1,Y1,X2,Y2,X3,Y3); */
    double Ae2, b[3], c[3], l[3], d, ld[3];

    b[0]=Y2-Y3; c[0]=X3-X2;
    b[1]=Y3-Y1; c[1]=X1-X3;
    b[2]=Y1-Y2; c[2]=X2-X1;
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=1/Ae2; /* common denominator 1/(2*Ae) */
    trisgnl(l,b,c); /* signed lengths: Eq.(10) of JLT94Mar495Koshiba; */
    ld[0]=l[0]*d; ld[1]=l[1]*d; ld[2]=l[2]*d;
    Ue[0][0]=ld[0]*Y3; Ue[0][1]=0; Ue[0][2]=-ld[0];
    Ue[1][0]=ld[1]*Y1; Ue[1][1]=0; Ue[1][2]=-ld[1];
    Ue[2][0]=ld[2]*Y2; Ue[2][1]=0; Ue[2][2]=-ld[2];
    Ve[0][0]=-ld[0]*X3; Ue[0][1]=ld[0]; Ue[0][2]=0;
    Ve[1][0]=-ld[1]*X1; Ue[1][1]=ld[1]; Ue[1][2]=0;
    Ve[2][0]=-ld[2]*X2; Ue[2][1]=ld[2]; Ue[2][2]=0;
    Ne[0][0]=(X2*Y3-X3*Y2)*d; Ne[0][1]=b[0]*d; Ne[0][2]=c[0]*d;
    Ne[1][0]=(X3*Y1-X1*Y3)*d; Ne[1][1]=b[1]*d; Ne[1][2]=c[1]*d;
    Ne[2][0]=(X1*Y2-X2*Y1)*d; Ne[2][1]=b[2]*d; Ne[2][2]=c[2]*d;
    return;
}
void FnShapeMIX1UV(double Ue[3][3], double Ve[3][3], double *X, double *Y)
{ /* calling syntax: double Ue[3][3], Ve[3][3]; FnShapeMIX1UV(Ue,Ve,X,Y); */
    double Ae2, b[3], c[3], l[3], d, ld[3];

    b[0]=Y[1]-Y[2]; c[0]=X[2]-X[1];
    b[1]=Y[2]-Y[0]; c[1]=X[0]-X[2];
    b[2]=Y[0]-Y[1]; c[2]=X[1]-X[0];
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=1/Ae2; /* common denominator 1/(2*Ae) */
    trisgnl(l,b,c); /* signed lengths: Eq.(10) of JLT94Mar495Koshiba; */
    ld[0]=l[0]*d; ld[1]=l[1]*d; ld[2]=l[2]*d;
    Ue[0][0]=ld[0]*Y[2]; Ue[0][1]=0; Ue[0][2]=-ld[0];
    Ue[1][0]=ld[1]*Y[0]; Ue[1][1]=0; Ue[1][2]=-ld[1];
    Ue[2][0]=ld[2]*Y[1]; Ue[2][1]=0; Ue[2][2]=-ld[2];
    Ve[0][0]=-ld[0]*X[2]; Ue[0][1]=ld[0]; Ue[0][2]=0;
    Ve[1][0]=-ld[1]*X[0]; Ue[1][1]=ld[1]; Ue[1][2]=0;
    Ve[2][0]=-ld[2]*X[1]; Ue[2][1]=ld[2]; Ue[2][2]=0;
    return;
}
void FnShapeMIX1UVN(double Ue[3][3], double Ve[3][3], double Ne[3][3],\
         double *X, double *Y)
{ /* calling syntax: double Ue[3][3], Ve[3][3], Ne[3][3];
     FnShapeMIX1UVN(Ue,Ve,Ne,X,Y); or FnShapeMIX1UVN(Ue,Ve,Ne,&X[0],&Y[0]); */
    double Ae2, b[3], c[3], l[3], d, ld[3];

    b[0]=Y[1]-Y[2]; c[0]=X[2]-X[1];
    b[1]=Y[2]-Y[0]; c[1]=X[0]-X[2];
    b[2]=Y[0]-Y[1]; c[2]=X[1]-X[0];
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=1/Ae2; /* common denominator 1/(2*Ae) */
    trisgnl(l,b,c); /* signed lengths: Eq.(10) of JLT94Mar495Koshiba; */
    ld[0]=l[0]*d; ld[1]=l[1]*d; ld[2]=l[2]*d;
    Ue[0][0]=ld[0]*Y[2]; Ue[0][1]=0; Ue[0][2]=-ld[0];
    Ue[1][0]=ld[1]*Y[0]; Ue[1][1]=0; Ue[1][2]=-ld[1];
    Ue[2][0]=ld[2]*Y[1]; Ue[2][1]=0; Ue[2][2]=-ld[2];
    Ve[0][0]=-ld[0]*X[2]; Ue[0][1]=ld[0]; Ue[0][2]=0;
    Ve[1][0]=-ld[1]*X[0]; Ue[1][1]=ld[1]; Ue[1][2]=0;
    Ve[2][0]=-ld[2]*X[1]; Ue[2][1]=ld[2]; Ue[2][2]=0;
    Ne[0][0]=(X[1]*Y[2]-X[2]*Y[1])*d; Ne[0][1]=b[0]*d; Ne[0][2]=c[0]*d;
    Ne[1][0]=(X[2]*Y[0]-X[0]*Y[2])*d; Ne[1][1]=b[1]*d; Ne[1][2]=c[1]*d;
    Ne[2][0]=(X[0]*Y[1]-X[1]*Y[0])*d; Ne[2][1]=b[2]*d; Ne[2][2]=c[2]*d;
    return;
}
/* -------------------------------------------------------------------------- */
void FnShapeMix2UV(double Ue[6][6], double Ve[6][6],\
         double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* double Ue[6][6], Ve[6][6]; FnShapeMix2UV(Ue,Ve,X1,Y1,X2,Y2,X3,Y3); */
    double Ae2, d, a[3], b[3], c[3], l[3], ld[3];

    a[0]=X2*Y3-X3*Y2; b[0]=Y2-Y3; c[0]=X3-X2;
    a[1]=X3*Y1-X1*Y3; b[1]=Y3-Y1; c[1]=X1-X3;
    a[2]=X1*Y2-X2*Y1; b[2]=Y1-Y2; c[2]=X2-X1;
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    d=1/(Ae2*Ae2); /* common denominator 1/(2*Ae)^2=1/(4*Ae*Ae); */
    trisgnl(l,b,c); /* signed lengths: Eq.(10) of JLT94Mar495Koshiba; */
    ld[0]=l[0]*d; ld[1]=l[1]*d; ld[2]=l[2]*d;
    Ue[0][0]= ld[0]*b[1]*a[0];Ue[0][1]= ld[0]*b[1]*b[0];Ue[0][2]= ld[0]*b[1]*c[0];
    Ue[1][0]= ld[1]*b[2]*a[1];Ue[1][1]= ld[1]*b[2]*b[1];Ue[1][2]= ld[1]*b[2]*c[1];
    Ue[2][0]= ld[2]*b[0]*a[2];Ue[2][1]= ld[2]*b[0]*b[2];Ue[2][2]= ld[2]*b[0]*c[2];
    Ue[3][0]=-ld[0]*b[0]*a[1];Ue[3][1]=-ld[0]*b[0]*b[1];Ue[3][2]=-ld[0]*b[0]*c[1];
    Ue[4][0]=-ld[1]*b[1]*a[2];Ue[4][1]=-ld[1]*b[1]*b[2];Ue[4][2]=-ld[1]*b[1]*c[2];
    Ue[5][0]=-ld[2]*b[2]*a[0];Ue[5][1]=-ld[2]*b[2]*b[0];Ue[5][2]=-ld[2]*b[2]*c[0];
    Ve[0][0]= ld[0]*c[1]*a[0];Ve[0][1]= ld[0]*c[1]*b[0];Ve[0][2]= ld[0]*c[1]*c[0];
    Ve[1][0]= ld[1]*c[2]*a[1];Ve[1][1]= ld[1]*c[2]*b[1];Ve[1][2]= ld[1]*c[2]*c[1];
    Ve[2][0]= ld[2]*c[0]*a[2];Ve[2][1]= ld[2]*c[0]*b[2];Ve[2][2]= ld[2]*c[0]*c[2];
    Ve[3][0]=-ld[0]*c[0]*a[1];Ve[3][1]=-ld[0]*c[0]*b[1];Ve[3][2]=-ld[0]*c[0]*c[1];
    Ve[4][0]=-ld[1]*c[1]*a[2];Ve[4][1]=-ld[1]*c[1]*b[2];Ve[4][2]=-ld[1]*c[1]*c[2];
    Ve[5][0]=-ld[2]*c[2]*a[0];Ve[5][1]=-ld[2]*c[2]*b[0];Ve[5][2]=-ld[2]*c[2]*c[0];
    Ue[0][3]=Ue[0][4]=Ue[0][5]=0; Ve[0][3]=Ve[0][4]=Ve[0][5]=0;
    Ue[1][3]=Ue[1][4]=Ue[1][5]=0; Ve[1][3]=Ve[1][4]=Ve[1][5]=0;
    Ue[2][3]=Ue[2][4]=Ue[2][5]=0; Ve[2][3]=Ve[2][4]=Ve[2][5]=0;
    Ue[3][3]=Ue[3][4]=Ue[3][5]=0; Ve[3][3]=Ve[3][4]=Ve[3][5]=0;
    Ue[4][3]=Ue[4][4]=Ue[4][5]=0; Ve[4][3]=Ve[4][4]=Ve[4][5]=0;
    Ue[5][3]=Ue[5][4]=Ue[5][5]=0; Ve[5][3]=Ve[5][4]=Ve[5][5]=0;
    return;
}
void FnShapeMIX2UV(double Ue[6][6], double Ve[6][6], double *X, double *Y)
{ /* double Ue[6][6], Ve[6][6]; FnShapeMIX2UV(Ue,Ve,X,Y); */
    FnShapeMix2UV(Ue,Ve,X[0],Y[0],X[1],Y[1],X[2],Y[2]);
    return;
}
void FnShapeMix2UVN(double Ue[6][6], double Ve[6][6], double Ne[6][6],\
         double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* double Ue[6][6], Ve[6][6], Ne[6][6];
     FnShapeMix2UVN(Ue,Ve,Ne,X1,Y1,X2,Y2,X3,Y3); */
    FnShapeMix2UV(Ue,Ve,X1,Y1,X2,Y2,X3,Y3);
    FnShapeTri2(Ne,X1,Y1,X2,Y2,X3,Y3);
    return;
}
void FnShapeMIX2UVN(double Ue[6][6], double Ve[6][6], double Ne[6][6],\
         double *X, double *Y)
{ /* double Ue[6][6], Ve[6][6], Ne[6][6]; FnShapeMIX2UVN(Ue,Ve,Ne,X,Y); */
    FnShapeMIX2UV(Ue,Ve,X,Y);
    FnShapeTRI2(Ne,X,Y);
    return;
}
/* -------------------------------------------------------------------------- */
/* Shape function {U},{V},{N} derivatives (NOTE that {Ux}={0} and {Vy}={0};
  {Uy}=div{U}/div(y),{Vx}=div{V}/div(x);{Nx}=div{N}/div(x),{Ny}=div{N}/div(y). */
/* Derivatives of 1st order Mixed Shape Function; omit *[1 x y].'; */
void FnShapeMix1Drv(double UeDy[3][3], double VeDx[3][3],\
                    double NeDx[3][3], double NeDy[3][3],\
                    double Ue[3][3], double Ve[3][3], double Ne[3][3])
{ /* calling syntax: (static) double UeDy[3][3], VeDx[3][3], NeDx[3][3], NeDy[3][3];
     FnShapeMix1Drv(UeDy,VeDx,NeDx,NeDy,Ue,Ve,Ne); */
    /* derivative {Uy}=div{U}/div(y); Table II of JLT94Mar495Koshiba */
    UeDy[0][0]=Ue[0][2]; UeDy[1][0]=Ue[1][2]; UeDy[2][0]=Ue[2][2];
    UeDy[0][1]=UeDy[0][2]=0; UeDy[1][1]=UeDy[1][2]=0; UeDy[2][1]=UeDy[2][2]=0;
    /* derivative {Vx}=div{V}/div(x); Table II of JLT94Mar495Koshiba */
    VeDx[0][0]=Ve[0][1]; VeDx[1][0]=Ve[1][1]; VeDx[2][0]=Ve[2][1];
    VeDx[0][1]=VeDx[0][2]=0; VeDx[1][1]=VeDx[1][2]=0; VeDx[2][1]=VeDx[2][2]=0;
    /* derivatives {Nx}=div{N}/div(x), {Ny}=div{N}/div(y) */
    NeDx[0][0]=Ne[0][1]; NeDx[1][0]=Ne[1][1]; NeDx[2][0]=Ne[2][1];
    NeDy[0][0]=Ne[0][2]; NeDy[1][0]=Ne[1][2]; NeDy[2][0]=Ne[2][2];
    NeDx[0][1]=NeDx[0][2]=0; NeDx[1][1]=NeDx[1][2]=0; NeDx[2][1]=NeDx[2][2]=0;
    NeDy[0][1]=NeDy[0][2]=0; NeDy[1][1]=NeDy[1][2]=0; NeDy[2][1]=NeDy[2][2]=0;
    return; /* (Eq.(1.75) & Table 1.2) of Koshiba book1992 */
}
/* Derivatives of 2nd order Mixed Shape Function; omit *[1 x y x^2 x*y y^2].'; */
void FnShapeMix2Drv(double UeDy[6][6], double VeDx[6][6],\
                    double NeDx[6][6], double NeDy[6][6],\
                    double Ue[6][6], double Ve[6][6], double Ne[6][6])
{ /* calling syntax: (static) double UeDy[6][6], VeDx[6][6], NeDx[6][6], NeDy[6][6];
     FnShapeMix1Drv(UeDy,VeDx,NeDx,NeDy,Ue,Ve,Ne); */
    double M[6][6]={0};
    FnShapeTri2Drv(NeDx,NeDy,Ne);
    FnShapeTri2Drv(M,UeDy,Ue); FnShapeTri2Drv(VeDx,M,Ve); /* {Ux}={0}; {Vy}={0}; */
    return;
}
/* ========================================================================== */

/* ========================================================================== */
/* Integrals (Shape Functions & Derivatives) of triangular mixed element;
 * (NNT,NxNxT,NyNyT) all same as triangular nodal elements; */
/* array form of Integrals of 1st order triangular mixed element */
/* intMix.m; Eq.(A1)-(A10) of JLT94Mar495Koshiba; */
void IntMIX1(double UUT[3][3], double VVT[3][3], double NNT[3][3],\
             double UyUyT[3][3], double VxVxT[3][3],\
             double NxNxT[3][3], double NyNyT[3][3],\
             double UNxT[3][3], double VNyT[3][3],\
             double UyVxT[3][3], double VxUyT[3][3],\
             double *X, double *Y)
{ /* calling syntax: double UUT[3][3],VVT[3][3],NNT[3][3],UyUyT[3][3],VxVxT[3][3];
     double NxNxT[3][3],NyNyT[3][3],UNxT[3][3],VNyT[3][3],UyVxT[3][3],VxUyT[3][3];
     IntTRI1(UUT,VVT,NNT,UyUyT,VxVxT,NxNxT,NyNyT,UNxT,VNyT,UyVxT,VxUyT,X,Y); */
    double Ae2, b[3], c[3], d, db[3], dc[3];
    double l[3], Xc, Yc, xt, yt, t[3];

    db[0]=b[0]=Y[1]-Y[2]; dc[0]=c[0]=X[2]-X[1];
    db[1]=b[1]=Y[2]-Y[0]; dc[1]=c[1]=X[0]-X[2];
    db[2]=b[2]=Y[0]-Y[1]; dc[2]=c[2]=X[1]-X[0];
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    /* (NNT,NxNxT,NyNyT) same as 1st order triangle nodal elements: MixTRI1() */
    NNT[0][0]=NNT[1][1]=NNT[2][2]=Ae2/12;
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=Ae2/24;

    d=0.5/Ae2; /* 1/(2*Ae2) */
    db[0]*=d; db[1]*=d; db[2]*=d;
    dc[0]*=d; dc[1]*=d; dc[2]*=d;

    NxNxT[0][0]=db[0]*b[0];
    NxNxT[1][1]=db[1]*b[1];
    NxNxT[2][2]=db[2]*b[2];
    NxNxT[0][1]=NxNxT[1][0]=db[0]*b[1];
    NxNxT[0][2]=NxNxT[2][0]=db[0]*b[2];
    NxNxT[1][2]=NxNxT[2][1]=db[1]*b[2];

    NyNyT[0][0]=dc[0]*c[0];
    NyNyT[1][1]=dc[1]*c[1];
    NyNyT[2][2]=dc[2]*c[2];
    NyNyT[0][1]=NyNyT[1][0]=dc[0]*c[1];
    NyNyT[0][2]=NyNyT[2][0]=dc[0]*c[2];
    NyNyT[1][2]=NyNyT[2][1]=dc[1]*c[2];

    trisgnl(l,b,c); /* signed lengths: Eq.(10) of JLT94Mar495Koshiba; */
    /* SYMMETRIC (UUT,VVT,UyUyT,VxVxT,UyVxT,VxUyT) (ONLY calculate Upper Half) */
    UUT[0][0]=VVT[0][0]=UyUyT[0][0]=d*l[0]*l[0];
    UUT[1][1]=VVT[1][1]=UyUyT[1][1]=d*l[1]*l[1];
    UUT[2][2]=VVT[2][2]=UyUyT[2][2]=d*l[2]*l[2];
    UUT[0][1]=UUT[1][0]=VVT[0][1]=VVT[1][0]=UyUyT[0][1]=UyUyT[1][0]=d*l[0]*l[1];
    UUT[0][2]=UUT[2][0]=VVT[0][2]=VVT[2][0]=UyUyT[0][2]=UyUyT[2][0]=d*l[0]*l[2];
    UUT[1][2]=UUT[2][1]=VVT[1][2]=VVT[2][1]=UyUyT[1][2]=UyUyT[2][1]=d*l[1]*l[2];
/* Simply MatMIX1() for 1st order mixed element due to UyVxT=VxUyT=-UyUyT=-VxVxT;
 *      Eq.(18a)&(A3)JLT94Mar495Koshiba */
/*  VxVxT[0][0]=UyUyT[0][0]; VxVxT[1][1]=UyUyT[1][1]; VxVxT[2][2]=UyUyT[2][2];
    VxVxT[0][1]=VxVxT[1][0]=UyUyT[0][1];
    VxVxT[0][2]=VxVxT[2][0]=UyUyT[0][2];
    VxVxT[1][2]=VxVxT[2][1]=UyUyT[1][2];
    UyVxT[0][0]=-UyUyT[0][0]; UyVxT[1][1]=-UyUyT[1][1]; UyVxT[2][2]=-UyUyT[2][2];
    UyVxT[0][1]=UyVxT[1][0]=-UyUyT[0][1];
    UyVxT[0][2]=UyVxT[2][0]=-UyUyT[0][2];
    UyVxT[1][2]=UyVxT[2][1]=-UyUyT[1][2];
    VxUyT[0][0]=UyVxT[0][0]; VxUyT[1][1]=UyVxT[1][1]; VxUyT[2][2]=UyVxT[2][2];
    VxUyT[0][1]=VxUyT[1][0]=UyVxT[0][1];
    VxUyT[0][2]=VxUyT[2][0]=UyVxT[0][2];
    VxUyT[1][2]=VxUyT[2][1]=UyVxT[1][2];
*/

    Xc=(X[0]+X[1]+X[2])/3;
    Yc=(Y[0]+Y[1]+Y[2])/3;
    xt=(X[0]*X[0]+X[1]*X[1]+X[2]*X[2]+Xc*Xc)/12;
    yt=(Y[0]*Y[0]+Y[1]*Y[1]+Y[2]*Y[2]+Yc*Yc)/12;
    UUT[0][0]*=(Y[2]*Y[2]-Yc*(Y[2]+Y[2])+yt);
    UUT[1][1]*=(Y[0]*Y[0]-Yc*(Y[0]+Y[0])+yt);
    UUT[2][2]*=(Y[1]*Y[1]-Yc*(Y[1]+Y[1])+yt);
    UUT[0][1]*=(Y[2]*Y[0]-Yc*(Y[2]+Y[0])+yt); UUT[1][0]=UUT[0][1];
    UUT[0][2]*=(Y[2]*Y[1]-Yc*(Y[2]+Y[1])+yt); UUT[2][0]=UUT[0][2];
    UUT[1][2]*=(Y[0]*Y[1]-Yc*(Y[0]+Y[1])+yt); UUT[2][1]=UUT[1][2];
    VVT[0][0]*=(X[2]*X[2]-Xc*(X[2]+X[2])+xt);
    VVT[1][1]*=(X[0]*X[0]-Xc*(X[0]+X[0])+xt);
    VVT[2][2]*=(X[1]*X[1]-Xc*(X[1]+X[1])+xt);
    VVT[0][1]*=(X[2]*X[0]-Xc*(X[2]+X[0])+xt); VVT[1][0]=VVT[0][1];
    VVT[0][2]*=(X[2]*X[1]-Xc*(X[2]+X[1])+xt); VVT[2][0]=VVT[0][2];
    VVT[1][2]*=(X[0]*X[1]-Xc*(X[0]+X[1])+xt); VVT[2][1]=VVT[1][2];

    t[0]=Y[0]-Yc; t[1]=Y[1]-Yc; t[2]=Y[2]-Yc;
    UNxT[0][0]=l[0]*db[0]*t[2];
    UNxT[1][1]=l[1]*db[1]*t[0];
    UNxT[2][2]=l[2]*db[2]*t[1];
    UNxT[0][1]=l[0]*db[1]*t[2]; UNxT[1][0]=l[1]*db[0]*t[0];
    UNxT[0][2]=l[0]*db[2]*t[2]; UNxT[2][0]=l[2]*db[0]*t[1];
    UNxT[1][2]=l[1]*db[2]*t[0]; UNxT[2][1]=l[2]*db[1]*t[1];

    t[0]=Xc-X[0]; t[1]=Xc-X[1]; t[2]=Xc-X[2];
    VNyT[0][0]=l[0]*dc[0]*t[2];
    VNyT[1][1]=l[1]*dc[1]*t[0];
    VNyT[2][2]=l[2]*dc[2]*t[1];
    VNyT[0][1]=l[0]*dc[1]*t[2]; VNyT[1][0]=l[1]*dc[0]*t[0];
    VNyT[0][2]=l[0]*dc[2]*t[2]; VNyT[2][0]=l[2]*dc[0]*t[1];
    VNyT[1][2]=l[1]*dc[2]*t[0]; VNyT[2][1]=l[2]*dc[1]*t[1];

    return;
}
void IntMix1(double UUT[3][3], double VVT[3][3], double NNT[3][3],\
             double UyUyT[3][3], double VxVxT[3][3],\
             double NxNxT[3][3], double NyNyT[3][3],\
             double UNxT[3][3], double VNyT[3][3],\
             double UyVxT[3][3], double VxUyT[3][3],\
             double X1, double Y1, double X2, double Y2, double X3, double Y3)
{
    double X[3]={X1,X2,X3}, Y[3]={Y1,Y2,Y3};
    IntMIX1(UUT,VVT,NNT,UyUyT,VxVxT,NxNxT,NyNyT,UNxT,VNyT,UyVxT,VxUyT,X,Y);
    return;
}

/* array form of Integrals of 2nd order triangular mixed element */
/* intMix.m; Table IV and Eq.(A11)-(A31) of JLT94Mar495Koshiba; */
void IntMIX2(double UUT[6][6], double VVT[6][6], double NNT[6][6],\
             double UyUyT[6][6], double VxVxT[6][6],\
             double NxNxT[6][6], double NyNyT[6][6],\
             double UNxT[6][6], double VNyT[6][6],\
             double UyVxT[6][6], double VxUyT[6][6],\
             double *X, double *Y)
{ /* calling syntax: double UUT[6][6],VVT[6][6],NNT[6][6],UyUyT[6][6],VxVxT[6][6];
     double NxNxT[6][6],NyNyT[6][6],UNxT[6][6],VNyT[6][6],UyVxT[6][6],VxUyT[6][6];
     IntMIX2(UUT,VVT,NNT,UyUyT,VxVxT,NxNxT,NyNyT,UNxT,VNyT,UyVxT,VxUyT,X,Y); */
    unsigned int i, j;
    double Ae2, b[3], c[3], d, d4, Ae, Ae_4, Ae_6, Ae_12;
    double l[3], db[3], dc[3], Aeuy[6], Aevx[6], t, tx[6], ty[6];
    double u[6], v[6], uy[6], vx[6]; /* Table IV JLT94Mar495Koshiba; */
    double Cx[6][4]={0}, Cy[6][4]={0}; /* Table 1.4 of Koshiba book1992 */
    double CxR[6]={0}, CyR[6]={0}; /* sum of each row of Cx and Cy */
    /* unspecified elements are set to 0 if short initialization is specified */
    double Ae_6u[6], Ae_12u[6], Ae_6v[6], Ae_12v[6];

    db[0]=b[0]=Y[1]-Y[2]; dc[0]=c[0]=X[2]-X[1];
    db[1]=b[1]=Y[2]-Y[0]; dc[1]=c[1]=X[0]-X[2];
    db[2]=b[2]=Y[0]-Y[1]; dc[2]=c[2]=X[1]-X[0];
    Ae2=c[2]*b[1]-c[1]*b[2]; /* TWICE of triangle signed area (determinant) */

    /* (NNT,NxNxT,NyNyT) same as 2nd order triangle nodal elements: MixTRI2() */
    d=Ae2/360; /* Ae/180 */
    NNT[0][0]=NNT[1][1]=NNT[2][2]=6*d; /* Eq.(1.91) of Koshiba book1992 */
    NNT[0][1]=NNT[0][2]=NNT[1][0]=NNT[1][2]=NNT[2][0]=NNT[2][1]=-d;
    NNT[0][4]=NNT[1][5]=NNT[2][3]=NNT[3][2]=NNT[4][0]=NNT[5][1]=-4*d;
    NNT[0][3]=NNT[0][5]=NNT[1][3]=NNT[1][4]=NNT[2][4]=NNT[2][5]=0;
    NNT[3][0]=NNT[3][1]=NNT[4][1]=NNT[4][2]=NNT[5][0]=NNT[5][2]=0;
    NNT[3][3]=NNT[4][4]=NNT[5][5]=32*d; /* Eq.(A29) JLT94Mar395 */
    NNT[3][4]=NNT[3][5]=NNT[4][3]=NNT[4][5]=NNT[5][3]=NNT[5][4]=16*d;

    d=1/Ae2; /* common denominator 1/(2*Ae) */
    db[0]*=d; db[1]*=d; db[2]*=d;
    dc[0]*=d; dc[1]*=d; dc[2]*=d;
    d4=4*d; Ae_4=Ae2/8; Ae_6=Ae2/12; Ae_12=Ae2/24; Ae=0.5*Ae2;
    /* edge elements: Table IV and Eq.(A11)-(A28) of JLT94Mar495Koshiba; */
    trisgnl(l,b,c); /* signed lengths: Eq.(10) of JLT94Mar495Koshiba; */
    u[0]= l[0]*db[1]; v[0]= l[0]*dc[1]; uy[0]=u[0]*dc[0]; vx[0]=v[0]*db[0];
    u[1]= l[1]*db[2]; v[1]= l[1]*dc[2]; uy[1]=u[1]*dc[1]; vx[1]=v[1]*db[1];
    u[2]= l[2]*db[0]; v[2]= l[2]*dc[0]; uy[2]=u[2]*dc[2]; vx[2]=v[2]*db[2];
    u[3]=-l[0]*db[0]; v[3]=-l[0]*dc[0]; uy[3]=u[3]*dc[1]; vx[3]=v[3]*db[1];
    u[4]=-l[1]*db[1]; v[4]=-l[1]*dc[1]; uy[4]=u[4]*dc[2]; vx[4]=v[4]*db[2];
    u[5]=-l[2]*db[2]; v[5]=-l[2]*dc[2]; uy[5]=u[5]*dc[0]; vx[5]=v[5]*db[0];
    for (i=0; i<6; i++) {Aeuy[i]=Ae*uy[i]; Aevx[i]=Ae*vx[i];}
    /* all other elements in Cx and Cy are 0 due to short initialization */
    Cx[0][3]=-d*b[0]; Cx[1][3]=-d*b[1]; Cx[2][3]=-d*b[2];
    Cx[0][0]=Cx[3][1]=Cx[5][2]=d4*b[0];
    Cx[1][1]=Cx[3][0]=Cx[4][2]=d4*b[1];
    Cx[2][2]=Cx[4][1]=Cx[5][0]=d4*b[2];
    Cy[0][3]=-d*c[0]; Cy[1][3]=-d*c[1]; Cy[2][3]=-d*c[2];
    Cy[0][0]=Cy[3][1]=Cy[5][2]=d4*c[0];
    Cy[1][1]=Cy[3][0]=Cy[4][2]=d4*c[1];
    Cy[2][2]=Cy[4][1]=Cy[5][0]=d4*c[2];
    CxR[0]=Cx[0][0]+Cx[0][3]; CxR[1]=Cx[1][1]+Cx[1][3]; CxR[2]=Cx[2][2]+Cx[2][3];
    CxR[3]=Cx[3][0]+Cx[3][1]; CxR[4]=Cx[4][1]+Cx[4][2]; CxR[5]=Cx[5][0]+Cx[5][2];
    CyR[0]=Cy[0][0]+Cy[0][3]; CyR[1]=Cy[1][1]+Cy[1][3]; CyR[2]=Cy[2][2]+Cy[2][3];
    CyR[3]=Cy[3][0]+Cy[3][1]; CyR[4]=Cy[4][1]+Cy[4][2]; CyR[5]=Cy[5][0]+Cy[5][2];
    for (i=0; i<6; i++) {
        /* (NxNxT,NyNyT,UyUyT,VxVxT) are symmetric matrices (M[i][j]=M[j][i]) */
        for (j=i; j<6; j++) { /* NOTE: initial j=i; for symmetric matrices; */
            /* IntTRI2(): simplified (NxNxT,NyNyT) Eq.(1.92) Koshiba book1992 */
            NxNxT[i][j]=NxNxT[j][i]=
                Ae_12*(CxR[i]*CxR[j]+5*Cx[i][3]*Cx[j][3]
                      +Cx[i][0]*Cx[j][0]+Cx[i][1]*Cx[j][1]+Cx[i][2]*Cx[j][2])
              + Ae_4*(CxR[i]*Cx[j][3]+Cx[i][3]*CxR[j]); /* Eq.(A30) JLT94Mar395 */
            NyNyT[i][j]=NyNyT[j][i]=
                Ae_12*(CyR[i]*CyR[j]+5*Cy[i][3]*Cy[j][3]
                      +Cy[i][0]*Cy[j][0]+Cy[i][1]*Cy[j][1]+Cy[i][2]*Cy[j][2])
              + Ae_4*(CyR[i]*Cy[j][3]+Cy[i][3]*CyR[j]); /* Eq.(A31) JLT94Mar395 */
            UyUyT[i][j]=UyUyT[j][i]=Aeuy[i]*uy[j]; /* Eq.(A13) JLT94Mar395 */
            VxVxT[i][j]=VxVxT[j][i]=Aevx[i]*vx[j]; /* Eq.(A14) JLT94Mar395 */
        }
        /* (UyVxT,VxUyT,UNxT,VNyT) are NOT symmetric matrices (M[i][j]!=M[j][i]) */
        for (j=0; j<6; j++) { /* NOTE: initial j=0; for asymetric matrices; */
            UyVxT[i][j]=Aeuy[i]*vx[j]; /* Eq.(A15) JLT94Mar395 */
            VxUyT[i][j]=Aevx[i]*uy[j]; /* Eq.(A16) JLT94Mar395 */
            tx[j]=CxR[j]+3*Cx[j][3]; /* common terms only */
            ty[j]=CyR[j]+3*Cy[j][3]; /* common terms only */
        }
        t=Ae_12*u[i];
        UNxT[i][0]=t*(tx[0]+Cx[0][0]); /* Eq.(A17) JLT94Mar395 */
        UNxT[i][1]=t*(tx[1]+Cx[1][1]); /* Eq.(A18) JLT94Mar395 */
        UNxT[i][2]=t*(tx[2]+Cx[2][2]); /* Eq.(A19) JLT94Mar395 */
        UNxT[i][3]=t*(tx[3]+Cx[3][1]); /* Eq.(A20) JLT94Mar395 */
        UNxT[i][4]=t*(tx[4]+Cx[4][2]); /* Eq.(A21) JLT94Mar395 */
        UNxT[i][5]=t*(tx[5]+Cx[5][0]); /* Eq.(A22) JLT94Mar395 */
        t=Ae_12*v[i];
        VNyT[i][0]=t*(ty[0]+Cy[0][0]); /* Eq.(A23) JLT94Mar395 */
        VNyT[i][1]=t*(ty[1]+Cy[1][1]); /* Eq.(A24) JLT94Mar395 */
        VNyT[i][2]=t*(ty[2]+Cy[2][2]); /* Eq.(A25) JLT94Mar395 */
        VNyT[i][3]=t*(ty[3]+Cy[3][1]); /* Eq.(A26) JLT94Mar395 */
        VNyT[i][4]=t*(ty[4]+Cy[4][2]); /* Eq.(A27) JLT94Mar395 */
        VNyT[i][5]=t*(ty[5]+Cy[5][0]); /* Eq.(A28) JLT94Mar395 */
    }

    /* SYMMETRIC (UUT,VVT) (ONLY calculate Upper Half) */
    Ae_6u[0]=Ae_6*u[0]; Ae_6u[1]=Ae_6*u[1]; Ae_6u[2]=Ae_6*u[2];
    Ae_6u[3]=Ae_6*u[3]; Ae_6u[4]=Ae_6*u[4]; Ae_6u[5]=Ae_6*u[5];
    UUT[0][0]=Ae_6u[0]*u[0]; UUT[1][1]=Ae_6u[1]*u[1]; UUT[2][2]=Ae_6u[2]*u[2];
    UUT[3][3]=Ae_6u[3]*u[3]; UUT[4][4]=Ae_6u[4]*u[4]; UUT[5][5]=Ae_6u[5]*u[5];
    UUT[0][5]=UUT[5][0]=Ae_6u[0]*u[5];
    UUT[1][3]=UUT[3][1]=Ae_6u[1]*u[3];
    UUT[2][4]=UUT[4][2]=Ae_6u[2]*u[4];
    UUT[0][1]=UUT[1][0]=Ae_12u[0]*u[1]; UUT[0][2]=UUT[2][0]=Ae_12u[0]*u[2];
    UUT[0][3]=UUT[3][0]=Ae_12u[0]*u[3]; UUT[0][4]=UUT[4][0]=Ae_12u[0]*u[4];
    UUT[1][2]=UUT[2][1]=Ae_12u[1]*u[2];
    UUT[1][4]=UUT[4][1]=Ae_12u[1]*u[4]; UUT[1][5]=UUT[5][1]=Ae_12u[1]*u[5];
    UUT[2][3]=UUT[3][2]=Ae_12u[2]*u[3]; UUT[2][5]=UUT[5][2]=Ae_12u[2]*u[5];
    UUT[3][4]=UUT[4][3]=Ae_12u[3]*u[4]; UUT[3][5]=UUT[5][3]=Ae_12u[3]*u[5];
    UUT[4][5]=UUT[5][4]=Ae_12u[4]*u[5];

    Ae_6v[0]=Ae_6*v[0]; Ae_6v[1]=Ae_6*v[1]; Ae_6v[2]=Ae_6*v[2];
    Ae_6v[3]=Ae_6*v[3]; Ae_6v[4]=Ae_6*v[4]; Ae_6v[5]=Ae_6*v[5];
    VVT[0][0]=Ae_6v[0]*v[0]; VVT[1][1]=Ae_6v[1]*v[1]; VVT[2][2]=Ae_6v[2]*v[2];
    VVT[3][3]=Ae_6v[3]*v[3]; VVT[4][4]=Ae_6v[4]*v[4]; VVT[5][5]=Ae_6v[5]*v[5];
    VVT[0][5]=VVT[5][0]=Ae_6v[0]*v[5];
    VVT[1][3]=VVT[3][1]=Ae_6v[1]*v[3];
    VVT[2][4]=VVT[4][2]=Ae_6v[2]*v[4];
    VVT[0][1]=VVT[1][0]=Ae_12v[0]*v[1]; VVT[0][2]=VVT[2][0]=Ae_12v[0]*v[2];
    VVT[0][3]=VVT[3][0]=Ae_12v[0]*v[3]; VVT[0][4]=VVT[4][0]=Ae_12v[0]*v[4];
    VVT[1][2]=VVT[2][1]=Ae_12v[1]*v[2];
    VVT[1][4]=VVT[4][1]=Ae_12v[1]*v[4]; VVT[1][5]=VVT[5][1]=Ae_12v[1]*v[5];
    VVT[2][3]=VVT[3][2]=Ae_12v[2]*v[3]; VVT[2][5]=VVT[5][2]=Ae_12v[2]*v[5];
    VVT[3][4]=VVT[4][3]=Ae_12v[3]*v[4]; VVT[3][5]=VVT[5][3]=Ae_12v[3]*v[5];
    VVT[4][5]=VVT[5][4]=Ae_12v[4]*v[5];

    return;
}
void IntMix2(double UUT[6][6], double VVT[6][6], double NNT[6][6],\
             double UyUyT[6][6], double VxVxT[6][6],\
             double NxNxT[6][6], double NyNyT[6][6],\
             double UNxT[6][6], double VNyT[6][6],\
             double UyVxT[6][6], double VxUyT[6][6],\
             double X1, double Y1, double X2, double Y2, double X3, double Y3)
{
    double X[3]={X1,X2,X3}, Y[3]={Y1,Y2,Y3};
    IntMIX2(UUT,VVT,NNT,UyUyT,VxVxT,NxNxT,NyNyT,UNxT,VNyT,UyVxT,VxUyT,X,Y);
    return;
}
/* ========================================================================== */

/* ========================================================================== */
/* Element Matrices of triangular nodal elements: Koshiba book1992 Eq.(3.80-84) */
/* MatTRI1(): Semi-Vectorial Element Matrices of 1st order nodal elements */
void MatTRI1(double KE[3][3], double ME[3][3],\
             double k0, double p[3][3], double q[3][3], double *X, double *Y)
{ /* calling syntax; double KE[3][3], ME[3][3]; MatTRI1(KE,ME,k0,p,q,X,Y); */
    unsigned int i, j;
    double k0k0=k0*k0;
    double px=p[0][0], py=p[1][1], pz=p[2][2], qx=q[0][0], qy=q[1][1], qz=q[2][2];
    double NNT[3][3], NxNxT[3][3], NyNyT[3][3];

    IntTRI1(NNT,NxNxT,NyNyT,X,Y);
    for (i=0; i<3; i++) { /* NOTE: inner loop initial j=i; for symmetric matrix; */
        KE[i][i]=qz*k0k0*NNT[i][i]-px*NxNxT[i][i]-py*NyNyT[i][i];
        ME[i][i]=pz*NNT[i][i]; /* Eq.(3.83)&(3.84) diagonal portion */
        /* NOTE: initial j=i+1; for symmetric matrices upper triangle portion; */
        for (j=i+1; j<3; j++) {
            KE[i][j]=KE[j][i]=qz*k0k0*NNT[i][j]-px*NxNxT[i][j]-py*NyNyT[i][j];
            ME[i][j]=ME[j][i]=pz*NNT[i][j]; /* Eq.(3.83)&(3.84) */
        }
    }
    return;
}
void MatTri1(double KE[3][3], double ME[3][3],\
             double k0, double p[3][3], double q[3][3],\
             double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax; double KE[3][3], ME[3][3];
     MatTri1(KE,ME,k0,p,q,X1,Y1,X2,Y2,X3,Y3); */
    double X[3]={X1,X2,X3}, Y[3]={Y1,Y2,Y3};
    MatTRI1(KE,ME,k0,p,q,X,Y);
    return;
}

/* MatTRI2(): Semi-Vectorial Element Matrices of 2nd order nodal Elements */
void MatTRI2(double KE[6][6], double ME[6][6],\
             double k0, double p[3][3], double q[3][3], double *X, double *Y)
{ /* calling syntax; double KE[3][3], ME[3][3]; MatTRI2(KE,ME,k0,p,q,X,Y); */
    unsigned int i, j;
    double k0k0=k0*k0;
    double px=p[0][0], py=p[1][1], pz=p[2][2], qx=q[0][0], qy=q[1][1], qz=q[2][2];
    double NNT[6][6], NxNxT[6][6], NyNyT[6][6];

    IntTRI2(NNT,NxNxT,NyNyT,X,Y);
    for (i=0; i<6; i++) { /* NOTE: inner loop initial j=i; for symmetric matrix; */
        KE[i][i]=qz*k0k0*NNT[i][i]-px*NxNxT[i][i]-py*NyNyT[i][i];
        ME[i][i]=pz*NNT[i][i]; /* Eq.(3.83)&(3.84) diagonal portion */
        /* NOTE: initial j=i+1; for symmetric matrices upper triangle portion; */
        for (j=i+1; j<6; j++) {
            KE[i][j]=KE[j][i]=qz*k0k0*NNT[i][j]-px*NxNxT[i][j]-py*NyNyT[i][j];
            ME[i][j]=ME[j][i]=pz*NNT[i][j]; /* Eq.(3.83)&(3.84) */
        }
    }
    return;
}
void MatTri2(double KE[6][6], double ME[6][6],\
             double k0, double p[3][3], double q[3][3],\
             double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax; double KE[3][3], ME[3][3];
     MatTri2(KE,ME,k0,p,q,X1,Y1,X2,Y2,X3,Y3); */
    double X[3]={X1,X2,X3}, Y[3]={Y1,Y2,Y3};
    MatTRI2(KE,ME,k0,p,q,X,Y);
    return;
}
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* PhD2005Deng Scalar, Semi-Vectorial, Full-Vectorial FEM algorithms */
/* input p[]/q[] definition MatSC/SV1():MatTRI1() and MatSC/SV2():MatTRI2() */
/* MatSC1(): Scalar Element Matrices of 1st order triangular nodal elements */
void MatSC1(double KE[3][3], double ME[3][3],\
            double k0, double n, double *X, double *Y)
{ /* calling syntax; double KE[3][3], ME[3][3]; MatSC1(KE,ME,k0,n,X,Y); */
    unsigned int i, j;
    double nnk0k0=n*n*k0*k0;
    double NNT[3][3], NxNxT[3][3], NyNyT[3][3];

    IntTRI1(NNT,NxNxT,NyNyT,X,Y);
    for (i=0; i<3; i++) { /* NOTE: inner loop initial j=i; for symmetric matrix; */
        KE[i][i]=nnk0k0*NNT[i][i]-NxNxT[i][i]-NyNyT[i][i];
        ME[i][i]=NNT[i][i]; /* Eq.(3.11) diagonal portion PhD2005Deng */
        /* NOTE: initial j=i+1; for symmetric matrices upper triangle portion; */
        for (j=i+1; j<3; j++) {
            KE[i][j]=KE[j][i]=nnk0k0*NNT[i][j]-NxNxT[i][j]-NyNyT[i][j];
            ME[i][j]=ME[j][i]=NNT[i][j]; /* Eq.(3.12) of PhD2005Deng */
        }
    }
    return;
}
void MatSc1(double KE[3][3], double ME[3][3],\
            double k0, double n,\
            double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax; double KE[3][3], ME[3][3];
     MatSc1(KE,ME,k0,n,X1,Y1,X2,Y2,X3,Y3); */
    double X[3]={X1,X2,X3}, Y[3]={Y1,Y2,Y3};
    MatSC1(KE,ME,k0,n,X,Y);
    return;
}

/* MatSC2(): Scalar Element Matrices of 2nd order triangular nodal Elements */
void MatSC2(double KE[6][6], double ME[6][6],\
            double k0, double n, double *X, double *Y)
{ /* calling syntax; double KE[3][3], ME[3][3]; MatSC2(KE,ME,k0,n,X,Y); */
    unsigned int i, j;
    double nnk0k0=n*n*k0*k0;
    double NNT[6][6], NxNxT[6][6], NyNyT[6][6];

    IntTRI2(NNT,NxNxT,NyNyT,X,Y);
    for (i=0; i<6; i++) { /* NOTE: inner loop initial j=i; for symmetric matrix; */
        KE[i][i]=nnk0k0*NNT[i][i]-NxNxT[i][i]-NyNyT[i][i];
        ME[i][i]=NNT[i][i]; /* Eq.(3.11) diagonal portion PhD2005Deng */
        /* NOTE: initial j=i+1; for symmetric matrices upper triangle portion; */
        for (j=i+1; j<6; j++) {
            KE[i][j]=KE[j][i]=nnk0k0*NNT[i][j]-NxNxT[i][j]-NyNyT[i][j];
            ME[i][j]=ME[j][i]=NNT[i][j]; /* Eq.(3.12) of PhD2005Deng */
        }
    }
    return;
}
void MatSc2(double KE[6][6], double ME[6][6],\
            double k0, double n,\
            double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax; double KE[3][3], ME[3][3];
     MatSc2(KE,ME,k0,n,X1,Y1,X2,Y2,X3,Y3); */
    double X[3]={X1,X2,X3}, Y[3]={Y1,Y2,Y3};
    MatSC2(KE,ME,k0,n,X,Y);
    return;
}
/* -------------------------------------------------------------------------- */
/* MatSV1(): Semi-Vectorial Element Matrices of 1st order nodal elements */
void MatSV1(double KE[3][3], double ME[3][3],\
            double k0, double p[3], double q, double *X, double *Y)
{ /* calling syntax; double KE[3][3], ME[3][3]; MatSV1(KE,ME,k0,p,q,X,Y); */
    unsigned int i, j;
    double k0k0=k0*k0;
    double NNT[3][3], NxNxT[3][3], NyNyT[3][3];

    IntTRI1(NNT,NxNxT,NyNyT,X,Y);
    for (i=0; i<3; i++) { /* NOTE: inner loop initial j=i; for symmetric matrix; */
        KE[i][i]=q*k0k0*NNT[i][i]-p[_x_]*NxNxT[i][i]-p[_y_]*NyNyT[i][i];
        ME[i][i]=p[_z_]*NNT[i][i]; /* Eq.(3.20) diagonal portion PhD2005Deng */
        /* NOTE: initial j=i+1; for symmetric matrices upper triangle portion; */
        for (j=i+1; j<3; j++) {
            KE[i][j]=KE[j][i]=\
                q*k0k0*NNT[i][j]-p[_x_]*NxNxT[i][j]-p[_y_]*NyNyT[i][j];
            ME[i][j]=ME[j][i]=p[_z_]*NNT[i][j]; /* Eq.(3.21) of PhD2005Deng */
        }
    }
    return;
}
void MatSv1(double KE[3][3], double ME[3][3],\
            double k0, double p[3], double q,\
            double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax; double KE[3][3], ME[3][3];
     MatSv1(KE,ME,k0,p,q,X1,Y1,X2,Y2,X3,Y3); */
    double X[3]={X1,X2,X3}, Y[3]={Y1,Y2,Y3};
    MatSV1(KE,ME,k0,p,q,X,Y);
    return;
}

/* MatSV2(): Semi-Vectorial Element Matrices of 2nd order nodal Elements */
void MatSV2(double KE[6][6], double ME[6][6],\
            double k0, double p[3], double q, double *X, double *Y)
{ /* calling syntax; double KE[3][3], ME[3][3]; MatSV2(KE,ME,k0,p,q,X,Y); */
    unsigned int i, j;
    double k0k0=k0*k0;
    double NNT[6][6], NxNxT[6][6], NyNyT[6][6];

    IntTRI2(NNT,NxNxT,NyNyT,X,Y);
    for (i=0; i<6; i++) { /* NOTE: inner loop initial j=i; for symmetric matrix; */
        KE[i][i]=q*k0k0*NNT[i][i]-p[_x_]*NxNxT[i][i]-p[_y_]*NyNyT[i][i];
        ME[i][i]=p[_z_]*NNT[i][i]; /* Eq.(3.20) diagonal portion PhD2005Deng */
        /* NOTE: initial j=i+1; for symmetric matrices upper triangle portion; */
        for (j=i+1; j<6; j++) {
            KE[i][j]=KE[j][i]=\
                q*k0k0*NNT[i][j]-p[_x_]*NxNxT[i][j]-p[_y_]*NyNyT[i][j];
            ME[i][j]=ME[j][i]=p[_z_]*NNT[i][j]; /* Eq.(3.21) of PhD2005Deng */
        }
    }
    return;
}
void MatSv2(double KE[6][6], double ME[6][6],\
            double k0, double p[3], double q,\
            double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax; double KE[3][3], ME[3][3];
     MatSv2(KE,ME,k0,p,q,X1,Y1,X2,Y2,X3,Y3); */
    double X[3]={X1,X2,X3}, Y[3]={Y1,Y2,Y3};
    MatSV2(KE,ME,k0,p,q,X,Y);
    return;
}
/* -------------------------------------------------------------------------- */
/* Full-Vectorial (FV) Element Matrices of triangular mixed elements:
 *   Eq.(32)(33) MTT92Feb371Koshiba; Eq.(9)-(15) JLT00May737Koshiba;
 *   Eq.(2)(3)(17)(18)(19) JLT94Mar495Koshiba; */
/* Global Matrix Assembly over all elements:
 *   [K]=[[Ktt],[0]; [0],[0]]; [M]=[[Mtt],[Mtz]; [Mzt],[Mzz]];
 */
/* MatMIX1(): Element Matrices of 1st order triangular mixed elements */
void MatMIX1(double KEtt[3][3], double MEtt[3][3],\
             double MEtz[3][3], double MEzt[3][3], double MEzz[3][3],\
             double k0, double p[3][3], double q[3][3], double *X, double *Y)
{ /* calling syntax; double KEtt[3][3],MEtt[3][3],MEtz[3][3],MEzt[3][3],MEzz[3][3];
     MatMIX1(KEtt,MEtt,MEtz,MEzt,MEzz,k0,p,q,X,Y); */
    unsigned int i, j;
    double k0k0=k0*k0;
    double px=p[0][0], py=p[1][1], pz=p[2][2], qx=q[0][0], qy=q[1][1], qz=q[2][2];
    double UUT[3][3],VVT[3][3],NNT[3][3],UyUyT[3][3],VxVxT[3][3];
    double NxNxT[3][3],NyNyT[3][3],UNxT[3][3],VNyT[3][3],UyVxT[3][3],VxUyT[3][3];
    double pz4=4*pz;

    IntMIX1(UUT,VVT,NNT,UyUyT,VxVxT,NxNxT,NyNyT,UNxT,VNyT,UyVxT,VxUyT,X,Y);
    /* JLT94Mar495 1st order mixed element: UyVxT=VxUyT=-UyUyT=-VxVxT */
    /* pz4*UyVxT[i][j] = pz*(UyVxT[i][j]+VxUyT[i][j]-UyUyT[i][j]-VxVxT[i][j]); */
    for (i=0; i<3; i++) { /* NOTE: inner loop initial j=i; for symmetric matrix; */
        /* matrix diagonals */
        KEtt[i][i]=k0k0*(qx*UUT[i][i]+qy*VVT[i][i])+pz4*UyVxT[i][i];
        MEtt[i][i]=py*UUT[i][i]+px*VVT[i][i];
        MEzz[i][i]=py*NxNxT[i][i]+px*NyNyT[i][i]-qz*k0k0*NNT[i][i];
        /* NOTE: initial j=i+1; for symmetric matrices upper triangular part; */
        for (j=i+1; j<3; j++) {
            KEtt[i][j]=KEtt[j][i]=k0k0*(qx*UUT[i][j]+qy*VVT[i][j])+pz4*UyVxT[i][j];
            MEtt[i][j]=MEtt[j][i]=py*UUT[i][j]+px*VVT[i][j];
            MEzz[i][j]=MEzz[j][i]=py*NxNxT[i][j]+px*NyNyT[i][j]-qz*k0k0*NNT[i][j];
        }
        /* (MEtz,MEzt) are NOT symmetric matrices; inner loop initial j=0; */
        for (j=0; j<3; j++) { MEtz[i][j]=MEzt[j][i]=py*UNxT[i][j]+px*VNyT[i][j]; }
    }
    return;
}
void MatMix1(double KEtt[3][3], double MEtt[3][3],\
             double MEtz[3][3], double MEzt[3][3], double MEzz[3][3],\
             double k0, double p[3][3], double q[3][3],\
             double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax; double KEtt[3][3],MEtt[3][3],MEtz[3][3],MEzt[3][3],MEzz[3][3];
     MatMix1(KEtt,MEtt,MEtz,MEzt,MEzz,k0,p,q,X1,Y1,X2,Y2,X3,Y3); */
    double X[3]={X1,X2,X3}, Y[3]={Y1,Y2,Y3};
    MatMIX1(KEtt,MEtt,MEtz,MEzt,MEzz,k0,p,q,X,Y);
    return;
}

/* MatMIX2(): Element Matrices of 2nd order Mixed Elements */
void MatMIX2(double KEtt[6][6], double MEtt[6][6],\
             double MEtz[6][6], double MEzt[6][6], double MEzz[6][6],\
             double k0, double p[3][3], double q[3][3], double *X, double *Y)
{ /* calling syntax; double KEtt[6][6],MEtt[6][6],MEtz[6][6],MEzt[6][6],MEzz[6][6];
     MatMIX2(KEtt,MEtt,MEtz,MEzt,MEzz,k0,p,q,X,Y); */
    unsigned int i, j;
    double k0k0=k0*k0;
    double px=p[0][0], py=p[1][1], pz=p[2][2], qx=q[0][0], qy=q[1][1], qz=q[2][2];
    double UUT[6][6],VVT[6][6],NNT[6][6],UyUyT[6][6],VxVxT[6][6];
    double NxNxT[6][6],NyNyT[6][6],UNxT[6][6],VNyT[6][6],UyVxT[6][6],VxUyT[6][6];

    IntMIX2(UUT,VVT,NNT,UyUyT,VxVxT,NxNxT,NyNyT,UNxT,VNyT,UyVxT,VxUyT,X,Y);
    for (i=0; i<6; i++) {
        /* matrix diagonals */
        KEtt[i][i]=k0k0*(qx*UUT[i][i]+qy*VVT[i][i])
                  + pz*(UyVxT[i][i]+VxUyT[i][i]-UyUyT[i][i]-VxVxT[i][i]);
        MEtt[i][i]=py*UUT[i][i]+px*VVT[i][i];
        MEzz[i][i]=py*NxNxT[i][i]+px*NyNyT[i][i]-qz*k0k0*NNT[i][i];
        /* NOTE: initial j=i+1; for symmetric matrices upper triangular part; */
        for (j=i+1; j<6; j++) {
            KEtt[i][j]=KEtt[j][i]=k0k0*(qx*UUT[i][j]+qy*VVT[i][j])
                      + pz*(UyVxT[i][j]+VxUyT[i][j]-UyUyT[i][j]-VxVxT[i][j]);
            MEtt[i][j]=MEtt[j][i]=py*UUT[i][j]+px*VVT[i][j];
            MEzz[i][j]=MEzz[j][i]=py*NxNxT[i][j]+px*NyNyT[i][j]-qz*k0k0*NNT[i][j];
        }
        /* (MEtz,MEzt) are NOT symmetric matrices; inner loop initial j=0; */
        for (j=0; j<6; j++) { MEtz[i][j]=MEzt[j][i]=py*UNxT[i][j]+px*VNyT[i][j]; }
    }
    return;
}
void MatMix2(double KEtt[6][6], double MEtt[6][6],\
             double MEtz[6][6], double MEzt[6][6], double MEzz[6][6],\
             double k0, double p[3][3], double q[3][3],\
             double X1, double Y1, double X2, double Y2, double X3, double Y3)
{ /* calling syntax; double KEtt[6][6],MEtt[6][6],MEtz[6][6],MEzt[6][6],MEzz[6][6];
     MatMix2(KEtt,MEtt,MEtz,MEzt,MEzz,k0,p,q,X1,Y1,X2,Y2,X3,Y3); */
    double X[3]={X1,X2,X3}, Y[3]={Y1,Y2,Y3};
    MatMIX2(KEtt,MEtt,MEtz,MEzt,MEzz,k0,p,q,X,Y);
    return;
}
/* ========================================================================== */

/* ########################################################################## */
/* Global Matrix Assemble (FEM_coreMat.m, FEM_varMesh.m, meshTypeConvert.m);
 * (REF.: John Volakis book1996 pp110, Jianming Jin book1992 pp83.) */
/* 0. e -- Global Element Number #;
 * 1. Node Location Table -- NodeLocation(i,xi,yi); i=Global Node #;
 * 2. Triangle Nodal Connectivity Table -- GlobalNode(e,:);
 *      Global Node numbers for local nodes in element e:
 *        local nodes 1~3 for 1st order (linear) elements;
 *        local nodes 1~6 for 2nd order (quadratic) elements;
 * 3. Boundary Node Table -- BoundaryNodes(:) store all the boundary nodes;
 * 4. Triangle Edge Connectivity Table -- GlobalEdge(e,:) (for mixed elements);
 * 5. Boundary Edge Table -- BoundaryEdges(:) (for edge/mixed elements);
 * 6. Material Group Table -- MaterialGroup(e,group#); for material index [p][q];
 * 7. Material Group Domain Set Index Table -- Three Cases (Ch3, hdeng2005.pdf):
 *    (SC). Scalar SetIndex(group#,n) (Ng*1 1D array)
 *            k-th row is the refractive index of material group #k;
 *    (SV). Semi-Vectorial SetIndex(group#,p[3],q) (use 4Ng*1 1D array for the
 *            Ng*4 2D matrix) k-th row is {px,py,pz,q} of material group #k;
 *    (FV). Full-Vectorial SetIndex(group#,p[3][3],q[3][3]) (use 18Ng*1 1D array
 *              for the Ng*18 2D matrix or Ng*9*2 3D matrix)
 *              k-th row has 18 values for material index info of group #k as
 *    p([0][0],[0][1],[0][2];[1][0],[1][1],[1][2];[2][0],[2][1],[2][2]) (1~9) &
 *    q([0][0],[0][1],[0][2];[1][0],[1][1],[1][2];[2][0],[2][1],[2][2]) (10~18);
 *    Although SetIndex is intrinsically 1D/2D/3D matrices for SC/SV/FV cases,
 *    we always employ the 1D array format for all three cases for consistency.
 *
 * NodeLocation(i,xi,yi), GlobalNode(e,:) and GlobalEdge(e,:) are for assembling;
 * BoundaryNodes(:) and BoundaryEdges(:) are for applying boundary conditions;
 * MaterialGroup(e,group#) and SC/SV/FV SetIndex(group#, n or [p][q])
 *   to efficiently obtain material indices for each elements and each domain.
 *
 * Ng=size(SetIndex(2D format),1); %number of material groups (domain sets);
 * Ne=size(GlobalNode,1); %number of elements (regardless of types); Ne=size(tm,2);
 * Np=size(NodeLocation,1); %number of nodes; %Np=max(GlobalNode(:));
 * Nv=size(pm,2); %number of vertex nodes (exclude edge nodes);
 * Nt=max(GlobalEdge(:)); %number of (tangential) edges; may not=number of sides;
 * Nm=Np or Nt+Np; %global matrices nodal Np*Np, mixed-elements (Nt+Np)*(Nt+Np);
 *
 * nprime--LocEdges: the global numbers for local nodes of element e;
 * mprime--LocNodes: the global numbers for local edges of element e;
 * global n(m)prime(1~3) (n1',n2',n3') for local (1,2,3) for 1st order elements;
 * global n(m)prime(1~6) (n1',n2',n3',n4',n5',n6') for 2nd order elements.
/* -------------------------------------------------------------------------- */
/* MATLAB sparse matrix (via sparse()): Compressed sparse column (CSC or CCS)
 *   http://en.wikipedia.org/wiki/Sparse_matrix
/* dynamic array http://people.sc.fsu.edu/~jburkardt/c_src/c/dynamic_array_2d.c */
/*  double **a; int i;
    a = (double **) malloc(m*n*sizeof (double *));
    for (i=0; i<m; i++) a[i]=(double *)malloc(n*sizeof(double));
 */
/* ########################################################################## */

/* ========================================================================== */
/* get material [p] and [q] info from Material Group Domain Set Index Table */
/* Full-Vectorial (FV) Material Group Domain Set Index Table */
void pqfv(double p[3][3], double q[3][3], unsigned int k, double *SetIndex)
{ /* calling syntax: double p[3][3], q[3][3]; int k; pqfv(p,q,k,SetIndex); */
    unsigned int i, j;
    double *ptr1=SetIndex+k*18, *ptr2=ptr1+9; /* &SetIndex[0]+k*18 */
    /* if 2D storage: p=&SetIndex[0][0]+k*18; q=p+9; */
    for (i=0; i<3; i++) {
        for (j=0; j<3; j++) {
            p[i][j]=*ptr1++; /* p[i][j]=*ptr1; ptr1++; */
            q[i][j]=*ptr2++; /* q[i][j]=*ptr2; ptr2++; */
        }
    }
    return;
}
/* Semi-Vectorial (SV) Material Group Domain Set Index Table */
void pqsv(double p[3], double q[1], unsigned int k, double *SetIndex)
{ /* calling syntax: double p[3], q[1](or *q); int k; pqsv(p,q,k,SetIndex);
     or double p[3], q; int k; pqsv(p,&q,k,SetIndex); */
    double *ptr=SetIndex+k*4; /* &SetIndex[0]+k*4 */
    p[_x_]=ptr[0]; p[_y_]=ptr[1]; p[_z_]=ptr[2];
    q[0]=ptr[3];
    return;
}
/* -------------------------------------------------------------------------- */
/* Global Matrix Assembling for Scalar FEM with triangular nodal elements */
void GlobalMatSC(double **KG, double **MG,\
         double k0, unsigned int ElemOrder,\
         unsigned int Ne, unsigned int **GlobalNode,\
         unsigned int Np, double **NodeLocation,\
         unsigned int *MaterialGroup, double *SetIndex)
{ /* calling syntax: double KG[Np][Np]={0}, MG[Np][Np]={0}; (NOTE: SPARSE MATRICES)
     GlobalMatSV(); */
    unsigned int i, j, II, JJ;
    unsigned k = (ElemOrder==1)? 3 : 6; /* 3 for 1st order; 6 for 2nd order; */
    unsigned int e; /* element number */
    unsigned int *LocNodes;
    double X[6], Y[6];
    double n;
    double KE[6][6], ME[6][6];

    for (e=0; e<Ne; e++) {
        LocNodes=&GlobalNode[e][0]; /* the address */
        for (i=0; i<k; i++) {
            X[i]=NodeLocation[LocNodes[i]][0];
            Y[i]=NodeLocation[LocNodes[i]][1];
        }
        n=SetIndex[MaterialGroup[e]]; /* Scalar material index */
        MatSC2(KE,ME,k0,n,X,Y);
        for (i=0; i<k; i++) {
            II=LocNodes[i]; 
            for (j=i; j<k; j++) { /* NOTE: initial j=i; for symmetric matrices; */
                JJ=LocNodes[j];  /* only write upper half (due to symmetry) */
                KG[II][JJ] += KE[i][j]; /* KG[JJ][II]=KG[II][JJ]; */
                MG[II][JJ] += ME[i][j]; /* MG[JJ][II]=MG[II][JJ]; */
            }
        }
    }
    /* FILL IN the lower half (symmetric to upper half) of the matrices */
    for (i=0; i<Np; i++) {
        for (j=i+1; j<Np; j++) { /* NOTE: initial j=i+1; excluding diagonals; */
            KG[j][i]=KG[i][j];
            MG[j][i]=MG[i][j];
        }
    }
    return;
}
/* Global Matrix Assembling for Semi-Vectorial FEM with triangular nodal elements */
void GlobalMatSV(double **KG, double **MG,\
         double k0, unsigned int ElemOrder,\
         unsigned int Ne, unsigned int **GlobalNode,\
         unsigned int Np, double **NodeLocation,\
         unsigned int *MaterialGroup, double *SetIndex)
{ /* calling syntax: double KG[Np][Np]={0}, MG[Np][Np]={0}; (NOTE: SPARSE MATRICES)
     GlobalMatSV(); */
    unsigned int i, j, II, JJ;
    unsigned k = (ElemOrder==1)? 3 : 6; /* 3 for 1st order; 6 for 2nd order; */
    unsigned int e; /* element number */
    unsigned int *LocNodes;
    double X[6], Y[6];
    double p[3], q=1;
    double KE[6][6], ME[6][6];

    for (e=0; e<Ne; e++) {
        LocNodes=&GlobalNode[e][0]; /* the address */
        for (i=0; i<k; i++) {
            X[i]=NodeLocation[LocNodes[i]][0];
            Y[i]=NodeLocation[LocNodes[i]][1];
        }
        pqsv(p,&q,MaterialGroup[e],SetIndex); /* Semi-Vectorial material [p][q] */
        MatSV2(KE,ME,k0,p,q,X,Y);
        for (i=0; i<k; i++) {
            II=LocNodes[i]; 
            for (j=i; j<k; j++) { /* NOTE: initial j=i; for symmetric matrices; */
                JJ=LocNodes[j];  /* only write upper half (due to symmetry) */
                KG[II][JJ] += KE[i][j]; /* KG[JJ][II]=KG[II][JJ]; */
                MG[II][JJ] += ME[i][j]; /* MG[JJ][II]=MG[II][JJ]; */
            }
        }
    }
    /* FILL IN the lower half (symmetric to upper half) of the matrices */
    for (i=0; i<Np; i++) {
        for (j=i+1; j<Np; j++) { /* NOTE: initial j=i+1; excluding diagonals; */
            KG[j][i]=KG[i][j];
            MG[j][i]=MG[i][j];
        }
    }
    return;
}
/* Global Matrix Assembling for Full-Vector FEM with triangular mixed elements */
void GlobalMatMIX(double **KG, double **MG,\
         double k0, unsigned int ElemOrder,\
         unsigned int Ne, unsigned int **GlobalNode,\
         unsigned int Np, double **NodeLocation,\
         unsigned int Nt, unsigned int **GlobalEdge,\
         unsigned int *MaterialGroup, double *SetIndex)
{ /* calling syntax: double KG[Np][Np]={0}, MG[Np][Np]={0}; (NOTE: SPARSE MATRICES)
     GlobalMatMIX(); */
    unsigned int i, j, It, Jt, Iz, Jz;
    unsigned k = (ElemOrder==1)? 3 : 6; /* 3 for 1st order; 6 for 2nd order; */
    unsigned int Nm=Nt+Np; /* Nm*Nm global matrices */
    unsigned int e; /* element number */
    unsigned int *LocNodes, *LocEdges;
    double X[6], Y[6];
    double p[3][3], q[3][3];
    double KEtt[6][6], MEtt[6][6], MEtz[6][6], MEzt[6][6], MEzz[6][6];

    for (e=0; e<Ne; e++) {
        LocNodes=&GlobalNode[e][0]; /* the address of the nodes */
        LocEdges=&GlobalEdge[e][0]; /* the address of the edges */
        for (i=0; i<k; i++) {
            X[i]=NodeLocation[LocNodes[i]][0];
            Y[i]=NodeLocation[LocNodes[i]][1];
        }
        pqfv(p,q,MaterialGroup[e],SetIndex); /* Full-Vectorial material [p][q] */
        MatMIX2(KEtt,MEtt,MEtz,MEzt,MEzz,k0,p,q,X,Y);
        for (i=0; i<k; i++) {
            It=LocEdges[i]; Iz=LocNodes[i];
            /* only write upper half (due to symmetry) */
            for (j=i; j<k; j++) { /* NOTE: initial j=i; for symmetric matrices; */
                Jt=LocEdges[j]; Jz=LocNodes[j];
                KG[It][Jt] += KEtt[i][j];
                MG[It][Jt] += MEtt[i][j];
                MG[Nt+Iz][Nt+Jz] += MEzz[i][j];
            }
            for (j=0; j<k; j++) { /* [Mtz] at upper half (itself symmetric) */
                Jz=LocNodes[j];
                MG[It][Nt+Jz] += MEtz[i][j];
            }
        }
    }
    /* FILL IN the lower half (symmetric to upper half) of the matrices */
    for (i=0; i<Nm; i++) {
        for (j=i+1; j<Nm; j++) { /* NOTE: initial j=i+1; excluding diagonals; */
            KG[j][i]=KG[i][j];
            MG[j][i]=MG[i][j];
        }
    }

    return;
}
/* ========================================================================== */

/* ########################################################################## */
/* ## Eigenvalue Solvers http://en.wikipedia.org/wiki/Eigenvalue_algorithm ## */
/* ########################################################################## */

/* ========================================================================== */

/* ========================================================================== */
