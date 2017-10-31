/* ========================================================================== */
/* COMMATH.H    COMmon MATHematical functions header file
 *
 * REFERENCE: icedpoly.c v11.00, qdrim.c v16.00, q1rim.c v15.00, qdcvx.c v9.00;
 *
 * COPYRIGHT 2012 Henghua Deng.
 * -------------------------------------------------------------------------- */
/* $REVISION: 8.60$ (20120918 Henghua Deng) */
/* HISTORY: 20110504(v1.00)from qdrim.c v10.60, q1rim.c v9.60;
 * 20110602(v2.22)sorting; 20110926(v5.99)swap pointers; products; rotations;
 * 20110930(v6.00)#ifndef; quick sort; 20111211(v7.00)FFT;
/* ========================================================================== */

typedef struct {
    double real;
    double imag;
} sCOMPLEX ;

/* utility macros #define functions: data type insensitive; */
/* BRACKETS ( ) correctly handle priority in complicated expressions */
#define uintdec(N) { if (N) N--; } /* N-- for unsigned int N; avoid overflow; */
/* round half(up/down,away/to 0, even/odd) en.wikipedia.org/wiki/Rounding */
#ifndef round /* round() not in C89/C90 math.h, but in C99; */
  #define round(X) ( floor((X)+0.5) ) /* round half up (widely used) */
#endif /* round (Matlab round.m: round half away from 0 (towards infinity) ) */

/* C math.h abs() for integer type, fabs() for float type; */
#ifndef abs
  #define abs(X) ( ((X)>=0)? (X) : -(X) ) /* conditional operator */
#endif /* abs */
#ifndef fabs
  #define fabs(X) ( ((X)>=0)? (X) : -(X) )
#endif /* fabs */

#define maxi(X,Y) ( ((X)>(Y))? (X) : (Y) ) /* maximum; Matlab max(X,Y); C99 fmax; */
#define mini(X,Y) ( ((X)<(Y))? (X) : (Y) ) /* minimum; Matlab min(X,Y); C99 fmin; */

/* Xor -- Exclusive OR: TRUE iff exactly one argument is true. */
/* Joseph O'Rourke book1998 \Comp_Geom_C\Ccode2\tri\tri.c Xor() */
/* bool Xor(bool x, bool y) { return( !x ^ !y ); } /* negation to ensure 0/1 */
#define Xor(x,y) ( !(x) ^ !(y) )

/* signum function y=sgn(x); http://en.wikipedia.org/wiki/Sign_function */
/* #define sign(X) ( ((X)>0)? 1 : ( ((X)<0)? -1 : 0 ) ) /* (Matlab: sign.m) */
/* #define sign2(X) ( ((X)<0)? -1 : 1 ) /* only check non-negativeness */
#define sign(X) ( ((X)>0) - ((X)<0) ) /* (Matlab: sign.m) without branching */
#define sign2(X) ( ((X)>=0) - ((X)<0) ) /* only check non-negativeness */
/* http://bits.stephan-brumme.com/ and Anderson_Bit_Twiddling_Hacks.pdf */
/* true iff x and y signs opposite (different cases for 0) */
#define isopsign(X,Y) ( ((X)>=0) ^ ((Y)>=0) ) /* FALSE (0,+x); TRUE (0,-x); */
#define issignop(X,Y) ( (X)*(Y)<0 ) /* FALSE (0,+x); FALSE (0,-x); */
/* true iff x and y have same signs (different cases for 0) */
#define iseqsign(X,Y) ( ((X)>=0) == ((Y)>=0) ) /* TRUE (0,+x); FALSE (0,-x); */
#define issigneq(X,Y) ( (X)*(Y)>=0 ) /* TRUE (0,+x); TRUE (0,-x); */
/* compare two values (X,Y) : return -1 if X<Y, 0 if X==Y, +1 if X>Y; */
#define lessmore(X,Y) ( ((X)>(Y)) - ((X)<(Y)) ) /* ( sign((X)-(Y)) ) */

/* sqsum(X,Y)=X*X+Y*Y; in case sqrt() in hypot(X,Y) operations are expensive; */
#define square(X) ( (X)*(X) ) /* pow((X),2) */
#define sqsum(X,Y) ( (X)*(X)+(Y)*(Y) )
#define sqsum3(X,Y,Z) ( (X)*(X)+(Y)*(Y)+(Z)*(Z) )
#define sqdiff(X,Y) ( (X)*(X)-(Y)*(Y) ) /* pow((X),2)-pow((Y),2) */

/* macros to avoide multiplication; similar for quadruple/quintuple/sextuple;*/
#define twice(X) ( (X)+(X) )
#define triple(X) ( (X)+(X)+(X) )

/* C (ISO 1999), c=a%b, c=fmod(a,b) follow the sign of dividend(numerator) a; */
bool isodd(int n) { return( n % 2 ); } /* return( n % 2 != 0 ); */
bool iseven(int n) { return( n % 2 == 0 ); }

/* http://en.wikipedia.org/wiki/Power_of_two N=2^m here ^ means power of */
bool isPOW2(unsigned int N)
{ /* Regan2009_10Ways_Check_Power_Of_2_in_C.pdf */
    /* return ( ((N & (~N + 1)) == N) && N ); /* &&(N != 0) <=> &&N */
    return ( !((N-1)&N) && N ); /* Anderson_Bit_Twiddling_Hacks.pdf */
}
unsigned int ceilpow2(unsigned int x)
{ /* get the ceiling of power of 2 ==> 2^(m-1)<x<(N=2^m); */
    unsigned int N = 1U; /* pow(2,0)=1; 1U: unsigned 1; */
    while (N<x) N <<= 1; /* http://en.wikipedia.org/wiki/Bit_shift */
    return(N);
    /* return( pow(2, ceil(log(x)/log(2)) ); /* change base to log2(); */
}
unsigned int POW2(unsigned int m)
{ /* calling syntax: N=POW2(m); return N=2^m; where m=LOG_2(N); */
    unsigned int N = 1U; /* pow(2,0)=1; 1U: unsigned 1; */
    N <<= m; /* http://en.wikipedia.org/wiki/Bit_shift */
    return(N);
}
/* find the log base 2 of an integer with the MSB N set in O(N) operations */
/* i.e., position of the highest bit set (or most significant bit set, MSB) */
/* graphics.stanford.edu/~seander/bithacks.html Anderson_Bit_Twiddling_Hacks.pdf */
unsigned int LOG_2(unsigned int N)
{ /* calling syntax: m=LOG_2(N); m=floor(log(N)/log(2.0)); log() natural base */
  /* N=2^m if isPOW2(N) (otherwise N=2^m+rem() thus 2^m <= N < 2^(m+1)); */
    unsigned int m=0U;
    while ( N >>= 1 ) m++;
    return(m); /* LOG_2(0/1/2)=0/0/1; LOG_2(255/256/257)=7/8/8; */
}
unsigned int LOG2m(unsigned int N)
{ /* (Matlab nextpow2.m) calling syntax: m=LOG2m(N); m=ceil(log(N)/log(2.0)); */
  /* N=2^m if isPOW2(N) (otherwise 2^m=ceilpow2(N) thus 2^(m-1) < N <= 2^m; */
    unsigned int m = 0U, x = 1U; /* pow(2,0)=1; 1U: unsigned 1; */
    while (x<N) { x <<= 1; m++; }
    return(m); /* LOG2m(0/1/2)=0/0/1; LOG2m(255/256/257)=8/8/9; */
}

/* ========================================================================== */
/* swap the values of two variables */
#define SWAP(x,y,t) { t = x; x = y; y = t; }
void swap(double *X, double *Y)
{ /* calling syntax: swap(X,Y); or swap(&X[i],&Y[j]); */
    double T = *X;
    *X = *Y; *Y = T;
    return;
}
void swapint(int *X, int *Y)
{ /* calling syntax: swapint(X,Y); or swapint(&X[i],&Y[j]); */
    int T = *X;
    *X = *Y; *Y = T;
    return;
}
void swapuint(unsigned int *X, unsigned int *Y)
{ /* calling syntax: swapuint(X,Y); or swapuint(&X[i],&Y[j]); */
    unsigned int T = *X;
    *X = *Y; *Y = T;
    return;
}
/* XOR swap: use XOR bitwise operation to swap values of distinct variables
 * having the same data type without using a temporary variable */
/* http://en.wikipedia.org/wiki/XOR_swap_algorithm */
void xorswap(int *x, int *y)
{ /* calling syntax: xorswap(X,Y); or xorswap(&X[i],&Y[j]); */
    if (x != y) {
        *x ^= *y;
        *y ^= *x;
        *x ^= *y;
    }
    return;
}
/* graphics.stanford.edu/~seander/bithacks.html Anderson_Bit_Twiddling_Hacks.pdf */
#define swapxor(a,b) ( ((a)^(b)) && ( ((a)^=(b)), ((b)^=(a)), ((a)^=(b)) ) )

/* Swap Pointers: A generic pointer swap function for structs & arrays */
/* USAGE EXAMPLE: \codelib\newlib\draft\COM_C\test\TestSwapPtr3.c
    double *A, *B; //definition: pointer to array
    A=malloc(4*sizeof(double)); B=malloc(4*sizeof(double)); //memory allocation
    A[0]=0;A[1]=1;A[2]=2;A[3]=3; B[0]=4;B[1]=5;B[2]=6;B[3]=7; //initialization
    swapptr((void*)&A,(void*)&B); //swap the two array pointers
 * EXTREMELY CAUTIOUS whenever intent to substitute swapptr() for memcpy()!
 * ( Matlab mex file output memcpy() CANNOT be replaced with swapptr() ); */
void swapptr(void** ptr1, void** ptr2)
{ /* calling syntax: swapptr((void*)&A,(void*)&B) or swapptr(&A,&B);
   * where A and B are pointers, and (void*) indicates pointer of any type; */
    void* ptr;
    ptr = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = ptr;
    return;
}
void swapptrd(double** ptr1, double** ptr2)
{ /* calling syntax: swapptrd(&A,&B); where A and B are double pointers; */
    double* ptr;
    ptr = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = ptr;
    return;
}
/* ========================================================================== */

/* ========================================================================== */
/* statistics; max() min() exists in math.h */
double maxim(double *X, unsigned int N)
{ /* (Matlab max.m; max(X[1:N]);) calling syntax: maxim(X,N); */
    unsigned int i;
    double Y=X[0];
    for (i=0;i<N;i++) { if (X[i]>Y) Y=X[i]; }
    return(Y);
}
double maxidx(double *X, unsigned int N, unsigned int *idx)
{ /* (Matlab max.m with indexing) calling syntax: maxidx(X,N,&idx); */
    unsigned int i;
    double Y=X[0];
    *idx=0;
    for (i=0;i<N;i++) { if (X[i]>Y) {Y=X[i]; *idx=i;} }
    return(Y);
}
double minim(double *X, unsigned int N)
{ /* (Matlab min.m; min(X[1:N]);) calling syntax: minim(X,N); */
    unsigned int i;
    double Y=X[0];
    for (i=0;i<N;i++) { if (X[i]<Y) Y=X[i]; }
    return(Y);
}
double minidx(double *X, unsigned int N, unsigned int *idx)
{ /* (Matlab min.m with indexing) calling syntax: minidx(X,N,&idx); */
    unsigned int i;
    double Y=X[0];
    *idx=0;
    for (i=0;i<N;i++) { if (X[i]<Y) {Y=X[i]; *idx=i;} }
    return(Y);
}
void minmax(double *X, unsigned int N, double *Xmin, double *Xmax)
{ /* get min and max simultaneously; calling syntax: minmax(X,N,&Xmin,&Xmax); */
    double *Xi=X, *XN=X+N;
    *Xmin=*Xmax=*X;
    for ( ; Xi<XN; Xi++ ) {
        if (*Xi<*Xmin) *Xmin=*Xi;
        if (*Xi>*Xmax) *Xmax=*Xi;
    }
    return;
}
void minmaxidx(double *X, unsigned int N, double *Xmin, unsigned int *idxmin,
               double *Xmax, unsigned int *idxmax) /* min&max with indexing */
{ /* calling syntax: minmaxidx(X,N,&Xmin,&idxmin,&Xmax,&idxmax); */
    unsigned int i;
    *Xmin=*Xmax=X[0];
    *idxmin=*idxmax=0;
    for (i=0;i<N;i++) {
        if (X[i]<*Xmin) { *Xmin=X[i]; *idxmin=i; }
        if (X[i]>*Xmax) { *Xmax=X[i]; *idxmax=i; }
    }
    return;
}
double avrgn(double *X, unsigned int N)
{ /* (Matlab mean.m) average; calling syntax: avrgn(X,N); */
    unsigned int i;
    double avg=0.0;
    if (N<1) return(0.0);
    for (i=0;i<N;i++) avg+=X[i];
    avg/=N;
    return(avg);
}
double stdvt(double *X, unsigned int N)
{ /* (Matlab std.m) standard deviation; calling syntax: stdvt(X,N); */
    unsigned int i;
    double d=0.0, avg=0.0, sigma=0.0;
    if (N<1) return(0.0);
    for (i=0;i<N;i++) avg+=X[i];
    avg/=N;
    for (i=0;i<N;i++) { d=X[i]-avg; sigma+=d*d; }
    sigma=sqrt(sigma/N);
    return(sigma);
}
/* statistics (mean, standard deviation, maximum, minimum) of a data set */
/* MATLAB functions mean.m, std.m, max.m, and min.m */
bool DataStat(double *mean, double *std, double *max, double *min,
              double *data, unsigned int num)
{ /* calling syntax: DataStat(&mean,&std,&max,&min,data,num); */
    unsigned int i;
    double avg=0.0, sigma=0.0, d=0.0, maxim=data[0], minim=data[0];

    if (num<1) {mean=std=max=min=NULL; return(false);} /* NULL pointer */
    avg=sigma=0.0; maxim=minim=data[0];
    for (i=0;i<num;i++) {
        avg+=data[i];
        if (data[i]>maxim) maxim=data[i];
        if (data[i]<minim) minim=data[i];
    }
    avg/=num;
    for (i=0;i<num;i++) { d=data[i]-avg; sigma+=d*d; }
    sigma=sqrt(sigma/num);

    *mean=avg; *std=sigma; *max=maxim; *min=minim;
    return(true);
}
/* DataStatIdx() is expansion of DataStat() with indexing returned */
bool DataStatIdx(double *mean, double *std, double *max, double *min,
                 double *data, unsigned int num,
                 unsigned int *idxmax, unsigned int *idxmin)
{ /* calling syntax: DataStatIdx(&mean,&std,&max,&min,data,num,&idxmax,&idxmin); */
    unsigned int i;
    double avg=0.0, sigma=0.0, d=0.0, maxim=data[0], minim=data[0];

    if (num<1) {mean=std=max=min=NULL; idxmax=idxmin=NULL; return(false);}
    avg=sigma=0.0; maxim=minim=data[0]; idxmax[0]=idxmin[0]=0;
    for (i=0;i<num;i++) {
        avg+=data[i];
        if (data[i]>maxim) {maxim=data[i]; idxmax[0]=i;}
        if (data[i]<minim) {minim=data[i]; idxmin[0]=i;}
    }
    avg/=num;
    for (i=0;i<num;i++) { d=data[i]-avg; sigma+=d*d; }
    sigma=sqrt(sigma/num);

    *mean=avg; *std=sigma; *max=maxim; *min=minim;
    return(true);
}
/* ========================================================================== */

/* ========================================================================== */
/* modulo (operators & signs: http://en.wikipedia.org/wiki/Modulo_operation) */
/* In c=a/b, a dividend(numerator), b divisor(denominator) and c quotient. */
/* C (ISO 1999): c=a%b, c=fmod(a,b) has the same sign as dividend a; */
/* Matlab: mod(a,b) same sign as divisor b, rem(a,b) same sign as dividend a; */
#ifndef rem
  #define rem(X,Y) fmod((X),(Y)) /* C99 math.h div,ldiv,lldiv,remainder,remquo */
#endif
double mod(double X, double Y)
{ /* (Matlab: mod.m): MOD(x,y) is x - n.*y where n = floor(x./y) if y ~= 0.
     By convention: MOD(x,0) is x; MOD(x,x) is 0;
         MOD(x,y), for x~=y and y~=0, has the same sign as y (divisor). */
  /* C: fmod(730,360)/(-730,360)/(730,-360)/(-730,-360)=10/-10/10/-10;
     Matlab: mod(730,360)/(-730,360)/(730,-360)/(-730,-360)=10/350/-350/-10; */
    double X0;
    if (Y==0.0) return(X); if (X==Y) return(0.0); /* mod(X,0)=X; mod(X,X)=0; */
    X0=fmod(X,Y); /* quotient X/Y; X-numerator; Y-denominator; */
    /* if (X0*Y<0) X0+=Y; /* if opposite sign, follow sign of Y (divisor); */
    /* if ( ( (X0<0)&&(Y>0) ) || ( (X0>0)&&(Y<0)) ) X0+=Y; */
    if isopsign(X0,Y) X0+=Y; /* X0=X0+Y; */
    return(X0);
}

/* coordinate mirror/flip: to origin or axis */
/* array negation: obtain the negative values of array */
void neg(double *X, unsigned int N)
{ /* calling syntax: neg(X,N); or neg(&X[0],N); */
    unsigned int i;
    for (i=0;i<N;i++) X[i]=-X[i];
    return;
}
void negxy(double *X, double *Y, unsigned int N)
{ /* calling syntax: negxy(X,Y,N); or negxy(&X[0],&Y[0],N); */
    unsigned int i;
    for (i=0;i<N;i++) { X[i]=-X[i]; Y[i]=-Y[i]; }
    return;
}
void negxyz(double *X, double *Y, double *Z, unsigned int N)
{ /* calling syntax: negxyz(X,Y,Z,N); or negxy(&X[0],&Y[0],&Z[0],N); */
    unsigned int i;
    for (i=0;i<N;i++) { X[i]=-X[i]; Y[i]=-Y[i]; Z[i]=-Z[i]; }
    return;
}
/* REF.: fawgvh/360.m faltvh/ud/lr(); altud: conj, altlr: -conj, altvh: j*conj; */
/* obtain conjugate values of complex array conj(X+1j*Y)=X-1j*Y; */
void conjn(double *X, double *Y, unsigned int N) /* mirror to y=0; */
{ /* (Matlab conj.m) calling syntax: conjn(X,Y,N); or conjn(&X[0],&Y[0],N); */
    unsigned int i;
    for (i=0;i<N;i++) Y[i]=-Y[i]; /* neg(Y,N); or neg(&Y[0],N); */
    return;
} /* C++ has built-in conj(); */
/* obtain negative conjugate values of complex array -conj(X+1j*Y)=-X+1j*Y; */
void negconjn(double *X, double *Y, unsigned int N) /* mirror to x=0; */
{ /* calling syntax: negconjn(X,Y,N); or negconjn(&X[0],&Y[0],N); */
    unsigned int i;
    for (i=0;i<N;i++) X[i]=-X[i]; /* neg(X,N); or neg(&X[0],N); */
    return;
}
/* obtain rotate conjugate values of complex array 1j*conj(X+1j*Y)=Y+1j*X; */
void jconjn(double *X, double *Y, unsigned int N) /* mirror to y=x; */
{ /* calling syntax: jconjn(X,Y,N); or jconjn(&X[0],&Y[0],N); */
    /* unsigned int i; */
    /* double T; */
    /* for (i=0;i<N;i++) { T=X[i]; X[i]=Y[i]; Y[i]=T; } */
    swapptrd(&X,&Y);
    return;
}

/* coordinate rotation: http://en.wikipedia.org/wiki/Rotation_matrix */
/* http://en.wikipedia.org/wiki/Rotation_(mathematics) */
/* http://en.wikipedia.org/wiki/Euler's_formula */
/* Euler's formula: exponential function exp(1j*ang)=cos(ang)+1j*sin(ang); */
void exp1j(double *X, double *Y, double ang)
{ /* exp(1j*ang); calling syntax: exp1j(&X,&Y,ang); */
    *X=cos(ang); /* X[0]=cos(ang); */
    *Y=sin(ang); /* Y[0]=sin(ang); */
    return;
}
/* #define exp1jang exp1j */
void exp1jRang(double *X, double *Y, double R, double ang)
{ /* R*exp(1j*ang); calling syntax: exp1jRang(&X,&Y,R,ang); */
    *X=R*cos(ang); /* X[0]=R*cos(ang); */
    *Y=R*sin(ang); /* Y[0]=R*sin(ang); */
    return;
}
void exp1jORang(double *X, double *Y, double OX, double OY, double R, double ang)
{ /* ORG+R*exp(1j*ang); calling syntax: exp1jORang(&X,&Y,OX,OY,R,ang); */
    *X=OX+R*cos(ang); /* X[0]=OX+R*cos(ang); */
    *Y=OY+R*sin(ang); /* Y[0]=OY+R*sin(ang); */
    return;
}

/* rotate coordinate based on (0,0): P=P0*expjang; */
void rotxy0(double *X, double *Y, double X0, double Y0, double ang)
{ /* calling syntax: rotxy0(&X,&Y,X0,Y0,ang); */
    double cosang=cos(ang), sinang=sin(ang);
    *X=X0*cosang-Y0*sinang;
    *Y=X0*sinang+Y0*cosang;
    return;
} /* rotxy0() for single coordinate; rotxy0n() for multiple coordinates; */
void rotxy0n(double *X, double *Y,\
             double *X0, double *Y0, unsigned int N, double ang)
{ /* syntax: rotxy0n(X,Y,X0,Y0,N,ang); rotxy0n(&X[0],&Y[0],&X0[0],&Y0[0],N,ang); */
    unsigned int i;
    double cosang=cos(ang), sinang=sin(ang);
    for (i=0;i<N;i++) {
        X[i]=X0[i]*cosang-Y0[i]*sinang;
        Y[i]=X0[i]*sinang+Y0[i]*cosang;
    }
    return; /* X+1j*Y=(X0+1j*Y0)*exp(1j*ang)=(X0+1j*Y0)*(cos(ang)+1j*sin(ang)); */
}
/* rotate coordinate based on origin (OX,OY): P=O+(Pv-O)*expjang; */
void rotxy(double *X, double *Y,\
           double Xv, double Yv, double OX, double OY, double ang)
{ /* calling syntax: rotxy(&X,&Y,Xv,Yv,OX,OY,ang); */
    double cosang=cos(ang), sinang=sin(ang), X0=Xv-OX, Y0=Yv-OY;
    *X=OX+(X0*cosang-Y0*sinang);
    *Y=OY+(X0*sinang+Y0*cosang);
    return;
} /* rotxy() for single coordinate; rotxyn() for multiple coordinates; */
void rotxyn(double *X, double *Y,\
            double *Xv, double *Yv, unsigned int N,\
            double OX, double OY, double ang)
{ /* calling syntax: rotxyn(X,Y,Xv,Yv,N,OX,OY,ang);
   *   or rotxyn(&X[0],&Y[0],&Xv[0],&Yv[0],N,OX,OY,ang); */
    unsigned int i;
    double cosang=cos(ang), sinang=sin(ang), X0i, Y0i;
    for (i=0;i<N;i++) {
        X0i=Xv[i]-OX; Y0i=Yv[i]-OY;
        X[i]=OX+(X0i*cosang-Y0i*sinang);
        Y[i]=OY+(X0i*sinang+Y0i*cosang);
    }
    return; /* X+1j*Y=(OX+1j*OY)+[(Xv+1j*Yv)-(OX+1j*OY)]*exp(1j*ang); */
}

/* obtain complex product of two complex values: X+j*Y=(X1+j*Y1)*(X2+j*Y2); */
void cprod(double *X, double *Y, double X1, double Y1, double X2, double Y2)
{ /* calling syntax: cprod(&X,&Y,X1,Y1,X2,Y2); */
    *X=X1*X2-Y1*Y2;
    *Y=X1*Y2+Y1*X2;
    return;
}
/* dot (scalar) product: http://en.wikipedia.org/wiki/Dot_product */
/* obtain dot product of two 2D vectors: (X1,Y1) dot (X2,Y2); */
double dprod2(double X1, double Y1, double X2, double Y2)
{ /* calling syntax: dprod2(X1,Y1,X2,Y2); */
    return(X1*X2+Y1*Y2);
}
/* obtain dot product of two 3D vectors: (X1,Y1,Z1) dot (X2,Y2,Z2); */
double dprod3(double X1, double Y1, double Z1, double X2, double Y2, double Z2)
{ /* calling syntax: dprod3(X1,Y1,Z1,X2,Y2,Z2); */
    return(X1*X2+Y1*Y2+Z1*Z2);
}
/* obtain angle of two 2D vectors (from dot product) */
/* hypotenuse http://en.wikipedia.org/wiki/Hypot (avoid precision errors) */
double angdot2(double X1, double Y1, double X2, double Y2)
{ /* calling syntax: angdot2(X1,Y1,X2,Y2); */
    double D1=hypot(X1,Y1), D2=hypot(X2,Y2); /* hypot(X,Y)=sqrt(X*X+Y*Y); */
    return( ((D1>0.0)&&(D2>0.0))? acos((X1*X2+Y1*Y2)/(D1*D2)) : NAN );
}
/* obtain angle of two 3D vectors (from dot product) */
double angdot3(double X1, double Y1, double Z1, double X2, double Y2, double Z2)
{ /* calling syntax: angdot3(X1,Y1,Z1,X2,Y2,Z2); */
    double D1D2=sqrt((X1*X1+Y1*Y1+Z1*Z1)*(X2*X2+Y2*Y2+Z2*Z2)); /* D1D2=D1*D2 */
    return( (D1D2>0.0)? acos((X1*X2+Y1*Y2+Z1*Z2)/D1D2) : NAN );
}
/* cosine of the angle of two 2D vectors V(O-XY1)&V(O-XY2) (from dot product) */
double cosangdV2(double X1, double Y1, double X2, double Y2)
{ /* calling syntax: cosangdV2(X1,Y1,X2,Y2); */
    double D1=hypot(X1,Y1), D2=hypot(X2,Y2); /* hypot(X,Y)=sqrt(X*X+Y*Y); */
    return( ((D1>0.0)&&(D2>0.0))? (X1*X2+Y1*Y2)/(D1*D2) : NAN );
}
/* sine of the angle of two 2D vectors V(O-XY1)&V(O-XY2) (from cross product) */
double sinangdV2(double X1, double Y1, double X2, double Y2)
{ /* calling syntax: sinangdV2(X1,Y1,X2,Y2); */
    double D1=hypot(X1,Y1), D2=hypot(X2,Y2); /* hypot(X,Y)=sqrt(X*X+Y*Y); */
    return( ((D1>0.0)&&(D2>0.0))? (X1*Y2-Y1*X2)/(D1*D2) : NAN ); 
    /* or TriDet2(X1,Y1,X2,Y2,0,0)/(D1*D2); /* sign(+-): orientation */
}

/* cross (vector) product: http://en.wikipedia.org/wiki/Cross_product */
/* obtain cross product amplitude of two 2D vectors: (X1,Y1) x (X2,Y2);*/
/* ( also the determinant of matrix | [X1,Y1; X2,Y2] | ) */
double xprod2(double X1, double Y1, double X2, double Y2)
{ /* calling syntax: xprod2(X1,Y1,X2,Y2); discrete form of comgeom.h XPROD2(); */
    return(X1*Y2-Y1*X2); /* direction is on the Z-axis */
}
/* obtain cross product of two 3D vectors: (X1,Y1,Z1) x (X2,Y2,Z2); */
/* ( also the determinant of matrix | [ax,ay,az; X1,Y1,Z1; X2,Y2,Z2] | ) */
void xprod3(double *X, double *Y, double *Z,
            double X1, double Y1, double Z1, double X2, double Y2, double Z2)
{ /* calling syntax: xprod3(&X,&Y,&Z,X1,Y1,Z1,X2,Y2,Z2); */
    *X=Y1*Z2-Z1*Y2;
    *Y=Z1*X2-X1*Z2;
    *Z=X1*Y2-Y1*X2;
    return;
}
/* ========================================================================== */

/* ========================================================================== */
/* reverse the sequence of elements (i.e., Ascending <--> Descending Order) */
void revseq(double *X, unsigned int N)
{ /* calling syntax: revseq(X,N); or revseq(&X[0],N); */
    /* double *Xi=X, *Xj=X+N-1, *Xk=X+(unsigned int)(0.5*N); */
    /* for ( ; Xi<Xk; Xi++,Xj-- ) swap(Xi,Xj); /* initial Xi=X, Xj=X+N-1; */
    double T; /* swap(Xi,Xj) MUCH FASTER than swap(&X[i],&X[j]) */
    unsigned int i=0, j=N-1, k=(unsigned int)0.5*N; /* NOTE 0.5*N; */
    for (i=0,j=N-1; i<k; i++,j--) {
        T=X[i]; X[i]=X[j]; X[j]=T; /* swap(&X[i],&X[j]); */
    }
    return;
}
void revseqxy(double *X, double *Y, unsigned int N)
{ /* calling syntax: revseqxy(X,Y,N); or revseqxy(&X[0],&Y[0],N); */
    /* double *Xi=X, *Yi=Y, *Xj=X+N-1, *Yj=Y+N-1, *Xk=X+(unsigned int)(0.5*N); */
    /* for ( ; Xi<Xk; Xi++,Yi++,Xj--,Yj-- ) { swap(Xi,Xj); swap(Yi,Yj); } */
    double T; unsigned int i=0, j=N-1, k=(unsigned int)0.5*N; /* NOTE 0.5*N; */
    for (i=0,j=N-1; i<k; i++,j--) {
        T=X[i]; X[i]=X[j]; X[j]=T; /* swap(&X[i],&X[j]); */
        T=Y[i]; Y[i]=Y[j]; Y[j]=T; /* swap(&Y[i],&Y[j]); */
    }
    return;
}
void revseqxyz(double *X, double *Y, double *Z, unsigned int N)
{ /* calling syntax: revseqxyz(X,Y,Z,N); or revseqxyz(&X[0],&Y[0],&Z[0],N); */
    double T; unsigned int i=0, j=N-1, k=(unsigned int)0.5*N; /* NOTE 0.5*N; */
    for (i=0,j=N-1; i<k; i++,j--) {
        T=X[i]; X[i]=X[j]; X[j]=T; /* swap(&X[i],&X[j]); */
        T=Y[i]; Y[i]=Y[j]; Y[j]=T; /* swap(&Y[i],&Y[j]); */
        T=Z[i]; Z[i]=Z[j]; Z[j]=T; /* swap(&Z[i],&Z[j]); */
    }
    return;
}

/* remove adjacently repeated entry; norepeat()/norep(): with/NO malloc(); */
/* (Ref.: comgeom.h nocollinear() & nocolinear() ) */
void norepeat(double *X, unsigned int *N)
{ /* calling syntax: norepeat(X,N); or norepeat(&X[0],&N[0]); */
    unsigned int N0=*N, k=0, i0=0, i1=1;
    double *GX=NULL;

    if (N0<2) return;
    /* allocate dynamic memory: (mx)malloc(N*size); (mx)calloc(N,size); FREE(); */
    /* Matlab mex file CRASH if malloc() return NULL; AVOID use in SUBfunctions;
     * 20120507 for better management <#define malloc mxMalloc> in comcnst.h; */
    GX = malloc(N0*sizeof(double)); /* GX = calloc(N0, sizeof(double)); */
    if (GX==NULL) MsgErr("commath.h norepeat() OUT OF MEMORY!\n");
    GX[k++]=X[i0]; /* FIRST value */
    while (i1<N0) { /* loop OTHER values (including the LAST value) */
        while ( (X[i0]==X[i1]) && (i1<N0-1) ) i1++;
        if (X[i0]!=X[i1]) GX[k++]=X[i1];
        i0=i1++; /* i0=i1; i1++; */
    }
    memcpy(X, GX, k*sizeof(double));
    free(GX); GX=NULL;

    *N=k; /* or N[0]=k; */
    return;
}
void norep(double *X, unsigned int *N)
{ /* (IMPROVED norepeat(); NO malloc();) norep(X,N); or norep(&X[0],&N[0]); */
    unsigned int N0=*N, k=1; /* with pointers, NO malloc()&memcpy() */
    double *Xi=X, *Xj=X+1, *XN=X+N0, *XN_1=X+N0-1;

    /* if (N0<2) return; */
    while (Xj<XN) {
        while ( (*Xi==*Xj) && (Xj<XN_1) ) Xj++;
        if (*Xi!=*Xj) X[k++]=*Xj;
        Xi=Xj++; /* Xi=Xj; Xj++; */
    }

    *N=k; /* or N[0]=k; */
    return;
}
/* ========================================================================== */

/* ########################################################################## */
/* USE Function Pointer to select between sorting algrithms: */
/*     typedef void (*Sort)(double *X, unsigned int N); /* type defintion */
/*     Sort funcs[] = {bubble_sort, quick_sort, insert_sort}; /* methods */
/* ########################################################################## */
/* determine if elements are in sorted order (Ascending (Increasing) Order) */
bool issortinc(double *X, unsigned int N)
{ /* (Matlab: issorted.m) calling syntax: issortinc(X,N); issortinc(&X[0],N); */
    if (N<2) return(true);
    while ( --N >= 1 ) {
        if ( X[N] < X[N-1] ) return (false);
    } /* http://rosettacode.org/wiki/Sorting_algorithms/Bogosort#C */
    return(true);
}
/* determine if elements are in sorted order (Descending (Decreasing) Order) */
bool issortdec(double *X, unsigned int N)
{ /* calling syntax: issortdec(X,N); or issortdec(&X[0],N); */
    if (N<2) return(true);
    while ( --N >= 1 ) {
        if ( X[N] > X[N-1] ) return (false);
    } /* http://rosettacode.org/wiki/Sorting_algorithms/Bogosort#C */
    return(true);
}
/* ================== BUBBLE (SINKING) SORT ALGORITHM ======================= */
/* Bubble (Sinking) Sort: FAST for small # of unsorted items (or PRE-SORTED) */
/* LESS efficient than insertion sort; */
/* http://en.wikipedia.org/wiki/Sorting_algorithm & Bubble_sort;
   http://rosettacode.org/wiki/Sorting_algorithms/Bubble_sort#C (& #C.2B.2B); */
/* bubble_sort() -- Bubble Sort in Ascending Order */
void bubble_sort(double *X, unsigned int N)
{ /* calling syntax: bubble_sort(X,N); or bubble_sort(&X[0],N); */
    unsigned int i=0, j=1; double T;
    bool flag=true; /* flag: TRUE means sorting INCOMPLETE */
    /* if (N<2) return; */
    while (flag) {
        flag=false;
        for ( i=0,j=1; j<N; i++,j++ ) {
            if (X[i]>X[j]) { /* swap(X+i,X+j); */
                T=X[i]; X[i]=X[j]; X[j]=T; /* swap(&X[i],&X[j]); */
                flag=true;
            }
        }
    }
    return;
}
/* sink_sort() -- Bubble (Sinking) Sort in Descending Order */
void sink_sort(double *X, unsigned int N)
{ /* calling syntax: sink_sort(X,N); or sink_sort(&X[0],N); */
    unsigned int i=0, j=1; double T;
    bool flag=true; /* flag: TRUE means sorting INCOMPLETE */
    /* if (N<2) return; */
    while (flag) {
        flag=false;
        for ( i=0,j=1; j<N; i++,j++ ) {
            if (X[i]<X[j]) { /* swap(X+i,X+j); */
                T=X[i]; X[i]=X[j]; X[j]=T; /* swap(&X[i],&X[j]); */
                flag=true;
            }
        }
    }
    return;
}
/* Bubble Sort (Ascending Order) with indexing (see qsort1(); qsortidx();) */
void bubble_sort_idx(double *X, unsigned int N, unsigned int *idx)
{ /* calling syntax: bubble_sort_idx(X,N,idx); bubble_sort_idx(&X[0],N,&idx[0]);
   * idx: input original sequence ([0]:[N-1]), output sorted indexing; */
    unsigned int i=0, j=1, k; double T;
    bool flag=true; /* flag: TRUE means sorting INCOMPLETE */
    /* if (N<2) return; */
    while (flag) {
        flag=false;
        for ( i=0,j=1; j<N; i++,j++ ) {
            if (X[i]>X[j]) { /* swap(&X[i],&X[j]); swapuint(&idx[i],&idx[j]); */
                T=X[i]; X[i]=X[j]; X[j]=T; /* swap(X+i,X+j); */
                k=idx[i]; idx[i]=idx[j]; idx[j]=k; /* swapuint(idx+i,idx+j); */
                flag=true;
            }
        }
    }
    return;
}
/* sink_sort_idx() -- Bubble Sort in Descending Order and return indexing */
void sink_sort_idx(double *X, unsigned int N, unsigned int *idx)
{ /* calling syntax: sink_sort_idx(X,N,idx); sink_sort_idx(&X[0],N,&idx[0]);
   * idx: input original sequence ([0]:[N-1]), output sorted indexing; */
    unsigned int i=0, j=1, k; double T;
    bool flag=true; /* flag: TRUE means sorting INCOMPLETE */
    /* if (N<2) return; */
    while (flag) {
        flag=false;
        for ( i=0,j=1; j<N; i++,j++ ) {
            if (X[i]<X[j]) { /* swap(&X[i],&X[j]); swapuint(&idx[i],&idx[j]); */
                T=X[i]; X[i]=X[j]; X[j]=T; /* swap(X+i,X+j); */
                k=idx[i]; idx[i]=idx[j]; idx[j]=k; /* swapuint(idx+i,idx+j); */
                flag=true;
            }
        }
    }
    return;
}

/* Bubble Unique in Ascending Order (Matlab unique.m) */
void bubble_unique(double *X, unsigned int *N)
{ /* calling syntax: bubble_unique(X,&N); or bubble_unique(&X[0],&N); */
  /* unsigned int k=*N; bubble_sort(X,k); norep(X,&k); *N=k; */
    bubble_sort(X,N[0]);
    norep(X,N);
    return;
}
/* Bubble (Sinking) Unique in Descending Order */
void sink_unique(double *X, unsigned int *N)
{ /* calling syntax: sink_unique(X,&N); or sink_unique(&X[0],&N); */
    sink_sort(X,N[0]);
    norep(X,N);
    return;
}
/* ========================================================================== */

/* ===================== INSERTION SORT ALGORITHM =========================== */
/* Insertion Sort better than Bubble Sort; FAST for small sets & PRE-SORTED */
/* GOOD for final finishing for O(n*log(n)) mergesort/quicksort algorithms */
/* http://en.wikipedia.org/wiki/Insertion_sort */
/* http://rosettacode.org/wiki/Sorting_algorithms/Insertion_sort */
/* insert_sort_inc() -- Insertion Sort in Ascending Order */
void insert_sort_inc(double *X, unsigned int N)
{ /* calling syntax: insert_sort_inc(X,N); or insert_sort_inc(&X[0],N); */
    unsigned int i=1, j=1;
    double T; /* temporary (current) value */
    for (i=1; i<N; i++) {
        T = X[i];
        for (j=i; (j>0)&&(T<X[j-1]); j--) X[j] = X[j-1];
        X[j] = T;
    }
    return;
}
/* insert_sort_dec() -- Insertion Sort in Descending Order */
void insert_sort_dec(double *X, unsigned int N)
{ /* calling syntax: insert_sort_dec(X,N); or insert_sort_dec(&X[0],N); */
    unsigned int i=1, j=1;
    double T; /* temporary (current) value */
    for (i=1; i<N; i++) {
        T = X[i];
        for (j=i; (j>0)&&(T>X[j-1]); j--) X[j] = X[j-1];
        X[j] = T;
    }
    return;
}
/* insortupidx() -- Insertion Sort in Ascending Order & return indexing */
void insortupidx(double *X, unsigned int N, unsigned int *idx)
{ /* calling syntax: insortupidx(X,N,idx); or insortupidx(&X[0],N,idx);
   * idx: input original sequence ([0]:[N-1]), output sorted indexing; */
    unsigned int i=1, j=1, ix; /* ix: temporary indexing */
    double T; /* temporary (current) value */
    for (i=1; i<N; i++) {
        T = X[i]; ix=idx[i];
        for (j=i; (j>0)&&(T<X[j-1]); j--) { X[j] = X[j-1]; idx[j] = idx[j-1]; }
        X[j] = T; idx[j]=ix;
    }
    return;
}
/* insortdnidx() -- Insertion Sort in Descending Order & return indexing */
void insortdnidx(double *X, unsigned int N, unsigned int *idx)
{ /* calling syntax: insortdnidx(X,N,idx); or insortdnidx(&X[0],N,idx);
   * idx: input original sequence ([0]:[N-1]), output sorted indexing; */
    unsigned int i=1, j=1, ix; /* ix: temporary indexing */
    double T; /* temporary (current) value */
    for (i=1; i<N; i++) {
        T = X[i]; ix=idx[i];
        for (j=i; (j>0)&&(T>X[j-1]); j--) { X[j] = X[j-1]; idx[j] = idx[j-1]; }
        X[j] = T; idx[j]=ix;
    }
    return;
}
/* Insertion Unique in Ascending Order (Matlab unique.m) */
void insert_unique_inc(double *X, unsigned int *N)
{ /* calling syntax: insert_unique_inc(X,&N); or insert_unique_inc(&X[0],&N); */
  /* unsigned int k=*N; insert_sort_dec(X,k); norep(X,&k); *N=k; */
    insert_sort_inc(X,N[0]);
    norep(X,N);
    return;
}
/* Insertion Unique in Descending Order */
void insert_unique_dec(double *X, unsigned int *N)
{ /* calling syntax: insert_unique_dec(X,&N); or insert_unique_dec(&X[0],&N); */
    insert_sort_dec(X,N[0]);
    norep(X,N);
    return;
}
/* ========================================================================== */

/* ======================= QUICK SORT ALGORITHM ============================= */
/* C standard library has qsort(); */
/* QuickSort O(n*log(n)) good when interested in a good average-case result; */
/* very INEFFICIENT for presorted data (for which Insertion/Bubble Sort FAST) */
/* http://en.wikipedia.org/wiki/Quicksort & Sorting_algorithm */
/* quick_sort_inc() -- QuickSort for Ascending Order */
/* ( modified from http://rosettacode.org/wiki/Sorting_algorithms/Quicksort ) */
void quick_sort_inc(double *X, unsigned int N)
{ /* calling syntax: quick_sort_inc(X,N); or quick_sort_inc(&X[0],N); */
    double p=X[(unsigned int)0.5*N];
    double *l=X, *r=X+N-1, t;
    if (N<2) return;
    while (l<=r) {
        while (*l<p) l++;
        while (*r>p) r--;
        if (l<=r) { /* swap values */
            t = *l;
            *l++ = *r;
            *r-- = t;
        }
    }
    /* recursion for both partitions http://en.wikipedia.org/wiki/Recursion */
    quick_sort_inc(X, r-X+1);
    quick_sort_inc(l, X+N-l);
    return;
}
/* quick_sort_inc() -- QuickSort for Descending Order */
void quick_sort_dec(double *X, unsigned int N)
{ /* calling syntax: quick_sort_dec(X,N); or quick_sort_dec(&X[0],N); */
    double p=X[(unsigned int)0.5*N];
    double *l=X, *r=X+N-1, t;
    if (N<2) return;
    while (l<=r) {
        while (*l>p) l++;
        while (*r<p) r--;
        if (l<=r) { /* swap values */
            t = *l;
            *l++ = *r;
            *r-- = t;
        }
    }
    /* recursion for both partitions http://en.wikipedia.org/wiki/Recursion */
    quick_sort_dec(X, r-X+1);
    quick_sort_dec(l, X+N-l);
    return;
}
/* QuickSort Unique in Ascending Order (Matlab unique.m) */
void quick_unique_inc(double *X, unsigned int *N)
{ /* calling syntax: quick_unique_inc(X,&N); or quick_unique_inc(&X[0],&N); */
  /* unsigned int k=*N; quick_sort_inc(X,k); norep(X,&k); *N=k; */
    quick_sort_inc(X,N[0]);
    norep(X,N);
    return;
}
/* QuickSort Unique in Descending Order */
void quick_unique_dec(double *X, unsigned int *N)
{ /* calling syntax: quick_unique_dec(X,&N); or quick_unique_dec(&X[0],&N); */
    quick_sort_dec(X,N[0]);
    norep(X,N);
    return;
}
/* -------------------- C standard library has qsort(); --------------------- */
/* ----------------- qsort1() Quick Sort with Indexing ---------------------- */
/* qsortdn/qsortup: quick sort subroutine for descending/ascending sort */
/* www.koders.com/c/ \Koders_Williamson_Tops_Comp_Env\math.c (D.R.Williamson) */
void qsortdn(double *X, unsigned int *p, unsigned int left, unsigned int right)
{
    register unsigned int i=left, j=right, q;
    double a=*(X+(i+j)/2), b;

    do {
        while(*(X+i)>a && i<right) i++;
        while(a>*(X+j) && j>left) j--;
        if(i<=j) {
            b=*(X+j); *(X+j)=*(X+i); *(X+i)=b; /* swap(X+i,X+j); */
            q=*(p+j); *(p+j)=*(p+i); *(p+i)=q; /* swapint(p+i,p+j); */
            i++; j--;
        }
    } while(i<=j);

    if(left<j) qsortdn(X,p,left,j);
    if(i<right) qsortdn(X,p,i,right);
    return;
}
void qsortup(double *X, unsigned int *p, unsigned int left, unsigned int right)
{
    register unsigned int i=left, j=right, q;
    double a=*(X+(i+j)/2), b;

    do {
        while(*(X+i)<a && i<right) i++;
        while(a<*(X+j) && j>left) j--;
        if(i<=j) {
            b=*(X+j); *(X+j)=*(X+i); *(X+i)=b; /* swap(X+i,X+j); */
            q=*(p+j); *(p+j)=*(p+i); *(p+i)=q; /* swapint(p+i,p+j); */
            i++; j--;
        }
    } while(i<=j);

    if(left<j) qsortup(X,p,left,j);
    if(i<right) qsortup(X,p,i,right);
    return;
}
/* WARNING: the calling function must FREE the returned pointer p */
unsigned int *qsort1(double *X, unsigned int N, const bool dir)
{/* Quick Sort algorithm (1D); ascending if dir=1, descending if dir=0.*/
 /* From the quicksort invented and named by C.A.R. Hoare and presented in:
      Schlict, H., "C: The Complete Reference," Osborne McGraw-Hill, 1995. */
 /* Sorts X and returns a mallocked unsigned integer pointer p to a list of 
    un-sorted row numbers (0-based): the list index is a row number in sorted X,
    and the value in the list is a row number in unsorted X. */
    register unsigned int i=0;
    static unsigned int *p=NULL;

    if (N==0) return(NULL);
    p=malloc(N*sizeof(unsigned int)); /* memory allocation for indexing */
    if (p==NULL) { /* memory allocation fail */
        /* stkerr(" qsort1: ",MEMNOT); */
        return(NULL);
    }
    for (;i<N;i++) *(p+i)=i; /* original sequence indexing([0]:[N-1]) */

    (dir)? qsortup(X,p,0,N-1) : qsortdn(X,p,0,N-1);
    return(p); /* WARNING: calling function must FREE returned pointer p */
}
/* modified qsort1() with input original indexing modified at output */
void qsortidx(double *X, unsigned int N, const bool dir, unsigned int *idx)
{ /* calling syntax: qsortidx(X,N,dir,idx); (REF.: bubble_sort_idx();)
   * idx: input original sequence indexing([0]:[N-1]), output sorted indexing; */
    (dir)? qsortup(X,idx,0,N-1) : qsortdn(X,idx,0,N-1);
    return;
}
/* ========================================================================== */

/* ########################################################################## */
/* ################### Discrete/FAST Fourier Transform ###################### */
/* Discrete_Fourier_Transform_Wikipedia, Bourke1993_Fast_Fourier_Transform_C.pdf */
/* dir=true(1) (sign=-1) for FFT, dir=false(0) (sign=1) for IFFT (inverse);
/* for FFT/IFFT: N2=2*N, N=2^m, m=LOG_2(N); here ^ means power of; */
/*      N-1           (forward FFT) ||         N-1        (inverse FFT (IFFT))
 *      ---                         ||         ---
 *      \         - j k 2 pi n / N  ||       1 \         j k 2 pi n / N
 * X(n)= >  x(k) e                  || X(n)=--- >  x(k) e
 *      /      (n=0..N-1)           ||       N /      (n=0..N-1)
 *      ---                         ||         ---
 *      k=0                         ||         k=0                            */
/* ########################################################################## */

/* Normal(Gaussian)Distribution 1/(sqrt(2*pi)*std)*exp(-0.5*((X-avg)/std).^2) */
double Gauss1(double X, double avg, double std) /* fGaussian.m 20111214 */
{ /* calling syntax: Gauss1(X,avg,std); max.amp.=1; */
    return ( exp(-0.5*pow((X-avg)/std,2)) ); /* max.amp.=1 */
}
double Gauss1D(double X, double avg, double std) /* fGaussian.m 20111214 */
{ /* calling syntax: Gauss1D(X,avg,std); Integral(fx)(-Inf,Inf)=1; */
    return ( SqrtInv2PI/std*exp(-0.5*pow((X-avg)/std,2)) );
}
void Gaussian1(double *X, double *Y, unsigned long int N, double avg, double std)
{ /* calling syntax: Gaussian1(X,Y,N,avg,std); max.amp.=1; */
    unsigned long int i;
    for (i=0;i<N;i++) Y[i]=exp(-0.5*pow((X[i]-avg)/std,2));
    return;
}
void Gaussian1D(double *X, double *Y, unsigned long int N, double avg, double std)
{ /* calling syntax: Gaussian1D(X,Y,N,avg,std); Integral(-Inf,Inf)=1; */
    unsigned long int i;
    double ymax=SqrtInv2PI/std; /* ymax=1/(sqrt(2*pi)*abs(std)); */
    for (i=0;i<N;i++) Y[i]=ymax*exp(-0.5*pow((X[i]-avg)/std,2));
    return;
}

/* ========================================================================== */
bool DFT(double *x1, double *y1, unsigned long int N, const bool dir)
{ /* Direct Fourier Transform (DFT); O(N*N) time complexity; */
  /* complex-to-complex: x and y are real and imaginary arrays of N points. */
    unsigned long int i, k;
    double arg, cosarg, sinarg;
    double *x2=NULL, *y2=NULL;

    x2 = malloc(N*sizeof(double));
    y2 = malloc(N*sizeof(double));
    if ( !(x2 && y2) ) return(false);

    for (i=0;i<N;i++) {
        x2[i] = 0;
        y2[i] = 0;
        arg = PI2 * ( (double)i / (double)N ) ;
        arg = (dir)? -arg : arg;
        for (k=0;k<N;k++) {
            cosarg = cos(k * arg);
            sinarg = sin(k * arg);
            x2[i] += (x1[k] * cosarg - y1[k] * sinarg);
            y2[i] += (x1[k] * sinarg + y1[k] * cosarg);
        }
    }

    /* Copy the data back */
    if (dir) {
        for (i=0;i<N;i++) {
            x1[i] = x2[i];
            y1[i] = y2[i];
        }
    } else {
        for (i=0;i<N;i++) {
            x1[i] = x2[i] / (double)N;
            y1[i] = y2[i] / (double)N;
        }
    }

    free(x2); free(y2); x2=y2=NULL;
    return(true);
}

bool FFT(double *x, double *y, unsigned int m, const bool dir)
{ /* Fast Fourier Transform (FFT); O(N*log(N)) time complexity; */
  /* complex-to-complex: x/y are real/imaginary arrays of N=2^m points. */
    unsigned long int N,N_1,i,i1,j,k,i2,l,l1,l2;
    double c1,c2,tx,ty,t1,t2,u1,u2,z;

    N=POW2(m); /* number of points N=2^m; m=LOG_2(N); */
    N_1=N-1;

    /* Do the bit reversal */
    i2 = N >> 1;
    j = 0;
    for (i=0;i<N_1;i++) {
        if (i < j) {
            tx = x[i]; ty = y[i];
            x[i] = x[j]; y[i] = y[j];
            x[j] = tx; y[j] = ty;
        }
        k = i2;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }

    /* Compute the FFT */
    c1 = -1.0; 
    c2 = 0.0;
    l2 = 1;
    for (l=0;l<m;l++) {
        l1 = l2;
        l2 <<= 1;
        u1 = 1.0; 
        u2 = 0.0;
        for (j=0;j<l1;j++) {
            for (i=j;i<N;i+=l2) {
                i1 = i + l1;
                t1 = u1 * x[i1] - u2 * y[i1];
                t2 = u1 * y[i1] + u2 * x[i1];
                x[i1] = x[i] - t1; 
                y[i1] = y[i] - t2;
                x[i] += t1;
                y[i] += t2;
            }
            z =  u1 * c1 - u2 * c2;
            u2 = u1 * c2 + u2 * c1;
            u1 = z;
        }
        c2 = sqrt((1.0 - c1) / 2.0);
        if (dir) c2 = -c2; /* c2 == (dir) ? c2 : -c2; */
        c1 = sqrt((1.0 + c1) / 2.0);
    }

    /* Scaling inverse FFT */
    if (!dir) {
        for (i=0;i<N;i++) {
            x[i] /= N;
            y[i] /= N;
        }
    }
    return(true);
}

bool FFT2(double *X, unsigned long int N2, const bool dir)
{ /* FFT2(X,N2,1) for FFT, FFT2(X,N2,0) for IFFT; MUST X[N2],N2=2*N,and N=2^m;
 The Discrete Fast Fourier Transform (FFT and IFFT):
     dir=true(1) (sign = -1) for FFT, dir=false(0) (sign=1) for IFFT (inverse).
 Output X[0...2*N-1] is Fourier transform of input time domain data X[0...2*N-1].
 The FFT array takes/returns the cosine and sine parts in an interleaved manner
 ie. X[0] = cosPart[0], X[1] = sinPart[0], and N must be a power of 2 (N=2^m).
 It expects a complex input signal, ie. when working with 'common' audio signals
 the input has to be passed as {in[0],0.,in[1],0.,in[2],0.,...}; In that case,
 the transform of the frequencies of interest is in X[0...N].
 REF.: S.M.Bernsee, Bernsee19990921_Fourier_Transform_1Day.pdf (List1.4).
 */
    double wr, wi, arg, *p1, *p2, T;
    double tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i; /* r:real; i:imag;*/
    unsigned long int i, j, k, bitm, le, le2;
    unsigned long int N = N2>>1; /* N=N/2; wikipedia.org/wiki/Bit_shift */
    unsigned long int logN=(long)(log(N2)/log(2.0)+0.5); /* change base: log2() */

    if (!isPOW2(N2)) return(false);
    for (i = 2; i < N2-2; i += 2) {
        for (bitm = 2, j = 0; bitm < N2; bitm <<= 1) {
            if (i & bitm) j++;
            j <<= 1;
        }
        if (i < j) {
            p1 = X+i; p2 = X+j;
            T = *p1; *(p1++) = *p2; *(p2++) = T;
            T = *p1; *p1 = *p2; *p2 = T;
        }
    }

    for (k = 0, le = 2; k < logN; k++) {
        le <<= 1;
        le2 = le>>1;
        ur = 1.0;
        ui = 0.0;
        arg = PI / (le2>>1);
        wr = cos(arg); /* int sign=(dir)? -1 : 1; */
        wi = (dir)? -sin(arg) : sin(arg); /* wi = sign*sin(arg); */
        for (j = 0; j < le2; j += 2) {
            p1r = X+j; p1i = p1r+1;
            p2r = p1r+le2; p2i = p2r+1;
            for (i = j; i < N2; i += le) {
                tr = *p2r * ur - *p2i * ui;
                ti = *p2r * ui + *p2i * ur;
                *p2r = *p1r - tr; *p2i = *p1i - ti;
                *p1r += tr; *p1i += ti;
                p1r += le; p1i += le;
                p2r += le; p2i += le;
            }
            tr = ur*wr - ui*wi;
            ui = ur*wi + ui*wr;
            ur = tr;
        }
    }

    /* Scaling inverse FFT(Discrete_Fourier_Transform_Wikipedia_20111208.pdf) */
    if (!dir) { for (i=0;i<N2;i++) X[i] /= N; }
    return(true);
}

/*-------------------------------------------------------------------------
 * Perform a 2D FFT inplace given a complex 2D array (size of array (nx,ny) )
 * Return false if memory fail or the dimensions are not powers of 2; */
bool FFT2D(sCOMPLEX **c, long unsigned int nx, long unsigned int ny, bool dir)
{ /* calling syntax: double sCOMPLEX C[nx][ny]; FFT2D(C, nx, ny, dir); */
    long unsigned int i, j, m; /* m=LOG_2(N)=log(N)/log(2.0); */
    double *real=NULL, *imag=NULL;

    if ( !isPOW2(nx) || !isPOW2(ny) ) return(false);

    /* Transform the rows */
    real = (double *)malloc(nx * sizeof(double));
    imag = (double *)malloc(nx * sizeof(double));
    if (real == NULL || imag == NULL) return(false);
    m = (long) LOG_2(nx);
    for (j=0;j<ny;j++) {
        for (i=0;i<nx;i++) {
            real[i] = c[i][j].real;
            imag[i] = c[i][j].imag;
        }
        FFT(real,imag,m,dir);
        for (i=0;i<nx;i++) {
            c[i][j].real = real[i];
            c[i][j].imag = imag[i];
        }
    }
    free(real); free(imag); real=imag=NULL;

    /* Transform the columns */
    real = (double *)malloc(ny * sizeof(double));
    imag = (double *)malloc(ny * sizeof(double));
    if (real == NULL || imag == NULL) return(false);
    m = (long) LOG_2(ny);
    for (i=0;i<nx;i++) {
        for (j=0;j<ny;j++) {
            real[j] = c[i][j].real;
            imag[j] = c[i][j].imag;
        }
        FFT(real,imag,m,dir);
        for (j=0;j<ny;j++) {
            c[i][j].real = real[j];
            c[i][j].imag = imag[j];
        }
    }
    free(real); free(imag); real=imag=NULL;

    return(true);
}
/* ========================================================================== */
