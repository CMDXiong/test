/* ========================================================================== */
/* COMCNST.H    COMmon CoNStanT (ConditioN STart/STring/SeTting) header file
 *
 * REFERENCE: C standard library <math.h>; <Constants.h> by SimTKcommon;
 *            http://reference.wolfram.com/
 *
 * COPYRIGHT 2012~2017 Henghua Deng.
 * -------------------------------------------------------------------------- */
/* $REVISION: 8.60$ (20120601 Henghua Deng) */
/* HISTORY: 20110504(v1.00)from qdrim.c v10.60, q1rim.c v9.60;
 * 20110602(v2.22)high precision; suffix "L" indicates "long double";
 * 20110630(v3.00)optimization; "const long double" vs. "#define" (speed);
 * 20110930(v6.00)#ifndef NAN, isnan, round etc.; 20111014(v6.10)timestamp();
 * 20120309(v8.00)128-digits evaluation with Mathematica 3.0 (comcnst.nb);
 *   scientific notation truncated within compiler limit (NO buffer overflow);
 * 20120309(v8.00)NAV/isNAV; 20120507(v8.50)#include <stdlib.h>, #define malloc;
 * 20120601(v8.60)#include <complex.h>; 20171001(v9.00)
 * ========================================================================== */

#define COM_VER 8 /* COM header file Version X.X.X */
#define COM_SUBVER 6
#define COM_SUBSUB 0
#define COM_DATE timestamp() /* "June 1, 2012" release date */
#define COM_COPYRIGHT "Copyright (c) Henghua DENG"

/* ========================================================================== */
/* Standard C library header files BEFORE custom headers */
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include <math.h>
 #include <stdbool.h>
/* standard C <stdlib.h>: the C Standard General Utilities Library
 *   macros NULL(0), EXIT_SUCCESS(0), EXIT_FAILURE(1); Environment: exit, abort;
 *   Dynamic memory management: malloc, calloc, realloc, free; and more... */
//#include <complex>  // P=X+j*Y, exp(), cis() etc.
//#include <valarray> // std::valarray class to represent & manipulate arrays of values.

//#ifdef MATLAB_MEX_CODE
//  // #include "mex.h"
//  // #include "matrix.h"
//  /* allocate dynamic memory: (mx)malloc(N*size); (mx)calloc(N,size); FREE(); */
//  /* Matlab mex file CRASH if malloc() return NULL; AVOID use in SUBfunctions;
//   * 20120507 for better management <#define malloc mxMalloc> in comcnst.h; */
//  #define malloc mxMalloc
//  #define calloc mxCalloc
//  #define MsgErr mexErrMsgTxt
//#else
//  #define MsgErr(p) { printf(p); exit(EXIT_FAILURE); }
//#endif

/* free memory allocation and set POINTER p to NULL */
/* O'Rourke1998 pp.26 code1.4; C standard free() CRASH for non-allocated! */
#define FREE(p) { if (p) free(p); p=NULL; }
/* ========================================================================== */

/* ========================================================================== */
/* custom data type (http://en.wikipedia.org/wiki/C_syntax) */
/* printf ("#include <float.h> double maximum DBL_MAX=%f \n", DBL_MAX ); */
typedef const long double cldb;

/* char (usually 8-bit integer, http://en.wikipedia.org/wiki/C_data_types)
 *   char type is distinct from both signed char and unsigned char. */
typedef signed char int8; /* signed 8-bit integer [-127~+127] (Matlab int8) */
typedef unsigned char uint8; /* unsigned 8-bit integer [0~255] (2^8-1=255) */
typedef unsigned int uint;   /* unsigned integer */
/* typedef unsigned int COUNT; /* counter */
/* typedef unsigned int INDEX; /* indexing */
/* typedef unsigned int LIST;  /* list */

const unsigned int sizedouble=sizeof(double);

/* typedef enum { FALSE, TRUE } bool; */
#ifndef boolean
  enum boolean {FALSE=0, TRUE=1, ERROR=(-1)};
#endif
/* ========================================================================== */

/* ========================================================================== */
/* TIMESTAMP()/TIMEPRINT(): return/print current YMDHMS date as time stamp. */
#include <time.h> /* http://en.wikipedia.org/wiki/Time.h (C Standard Library) */
char *timestamp() /* <Friday 2011-10-14 13:48:04PM Eastern Daylight Time> */
{ /* calling syntax: timestamp(); or timestamp(NULL); or timestr=timestamp();
     or printf("%s\n",timestamp()); or printf("%s\n", timestamp(NULL)); */
    static char timestr[64];
    const struct tm *tm;
    time_t now=time(NULL); /* obtain current time */
    tm=localtime(&now);
    /* strftime(timestr,64,"%d %B %Y %I:%M:%S %p",tm); */
    strftime(timestr,64,"<%A %Y-%m-%d %H:%M:%S%p %Z>",tm);
    return(timestr); /* return the address of timestr (&timestr[0]) */
}
void timeprint()
{ /* calling syntax: timeprint(); or timeprint(NULL); */
    static char timestr[64];
    time_t now=time(NULL); /* obtain current time */
    strftime(timestr,64,"<%A %Y-%m-%d %H:%M:%S%p %Z>",localtime(&now));
    printf("%s\n", timestr);
    return;
}
typedef struct { int year; uint8 month; uint8 day; } DATE ;
typedef struct { uint8 hour; uint8 minute; uint8 second; } TIME ;
typedef struct { int year; uint8 month; uint8 day;\
                 uint8 hour; uint8 minute; uint8 second; } DATETIME ;
/* ========================================================================== */

/* ========================================================================== */
/* ISO C99 <complex.h> Complex Number type */
/* #include <complex.h> */

/* RESERVE alias ( I, J, _i_, _j_ ) as imaginary unit I*I=-1 */
/* const float complex I; const float complex _Complex_I; */
#define I _Imaginary_I
#define J _Imaginary_I
/* #undef I */ /* if I is to be used other than the imaginary unit */

/* -------------------------------------------------------------------------- */
/* (_x_,_y_,_z_)=(0,1,2) for Coordinate/Vector indexing (O'Rourke1998 pp.25); */
#define _x_ 0
#define _y_ 1
#define _z_ 2

/* -------------------------------------------------------------------------- */
/* GDSII boundary entity is CLOSED polygon of <= 200 vertices (first==last);
 * SET 199 vertices for OPEN polygon; Artwork_Rule_MASK_AutoCAD_DXF_GDSII.pdf */
#define NGDS2MAX 8191 /* 8192(2^13)-1 absolute maximum GDSII boundary record */
#define NGDSBDRY 200 /* maximum vertices for GDSII BOUNDARY entity (CLOSED) */
#define NGDSPOLY 199 /* maximum vertices for GDSII-compliant polygon (OPEN) */
#define NGDSPATH 200 /* maximum vertices for GDSII PATH entity (N = 2 to 200) */

#define EPS 1e-9 /* +- to remove precision errors; 2^(-32)=2.3e-10; */

/* -------------------------------------------------------------------------- */
/* polygon divide symbol: NAV+1j*NAV in C (20120502); NaN+1j*NaN in Matlab; */
/* NAV: Not-A-Value/Not-AVailable (INDEPENDENT OF 16/32/64/128-bit machines) */
/* (float.h DBL_MAX) NOTE: NAV==NAV true, but NaN==NaN false; */
/* ~ bitwise inverse; ~1 is 1111........1110; (All 1 except the last digit) */
/* comdefn.h WAFER: 11.8inch D=300mm; 18inch(expected) D=450mm(4.5e5 um); */
/* 2^32=4,294,967,296<5e9; 2^64=18,446,744,073,709,551,616 < 2e19; */
#define NAV 1e10
#define isNAV(X) ( (X)>=(NAV) ) /* !((int)(X)^(int)(NAV)), (X)==(NAV) */
#define isAVL(X) ( (X)<(NAV) ) /* is-A-VaLue/is-AVaiLable !(!((X)^(NAV))) */

/* nan(s) not in C89, but in C99; http://en.wikipedia.org/wiki/Math.h */
/* C99: NAN--A constant expression of type float representing a quiet NaN. */
#ifdef MATLAB_MEX_CODE
  #define NAN mxGetNaN()
  #define isnan mxIsNaN
#else
  #ifndef NAN
    #define NAN NAV
  #endif
  #ifndef isnan
    #define isnan isNAV
  #endif
#endif

/* (REF.: \polylib\ref\progtips\NaN_Inf\ http://en.wikipedia.org/wiki/IEEE_754)
   +NAN float: 0x7fc00000; -NAN float: 0x7fc00000;(0x prefix means hexadecimal)
   +NAN double: 0x7ff8000000000000; -NAN double: 0x7ff8000000000000;
   +INF float: 0x7f800000; -INF float: 0xff800000;
   +INF double: 0x7ff0000000000000; -INF double: 0xfff0000000000000;
   Matlab R2009b R2010b (32-bit version):
     num2hex(NaN)=fff8000000000000; num2hex(-NaN)=num2hex(+NaN);
     num2hex(+Inf)=7ff0000000000000; num2hex(-Inf)=fff0000000000000;
     hex2num('7ff8000000000000')=NaN; hex2num('fff8000000000000')=NaN; */
/* #ifndef NAN /* if NAN (Not A Number) is not defined, then define it; */
/*  #define NAN mxGetNaN() /* ( sqrt(-1.0), log(-1.0), mxGetNaN() ) */
/*  /* #define NAN 0x7ff8000000000000 /* RESULTS NOT AS ANTICIPATED 20110929 */
/* #endif /* NAN (C99:NAN, Matlab:NaN) http://en.wikipedia.org/wiki/NaN */

/* #ifndef isnan /* FUNCTION isnan() not in C89, but in C99; */
/*   #define isnan mxIsNaN /* #define isnan !mxIsFinite /* SET isnan() to mxIsNaN() */
  /* crude effective test: IEEE NaN's never equal to anything, even themselves; */
  /* #define isnan(x) ((x) != (x)) /* NaN != NaN */
/* #endif /* C99 math.h isnan() */
/* ========================================================================== */

/* ========================================================================== */
cldb C0=299792458; /* EXACT speed of light in vacuum (SI unit: m/s) (metre definition) */
/* magnetic constant MU0 (EXACT(defined) 4*PI*1e-7 V+s/(A+m) ) (unit: H/m, N/A2) */
cldb MU0=1.25663706143591729538505735331180115367886775975004232838997783692312656251448359945121393013684682719285923460531292265884e-6L;
/* electric constant (vacuum permittivity): 1/(MU0*C0*C0) (F/m) */
cldb EP0=8.8541878176203898505365630317107502606083701665994498081024171524053950954599821142852891607182008932867329183837820510568e-12L;
/* impedance of free space IMPz0=MU0*C0=sqrt(MU0/EP0); about (119.9169832*PI); */
cldb IMPZ0=3.76730313461770655468198400420319308268623508352418655232074638296707269221307698880166875199565586693648509990325420943057e+2L;

/* http://en.wikipedia.org/wiki/Pi (PI2:2*pi, PI_2:pi/2, PI_3:pi/3, PI_4:pi/4) */
cldb PI9=2.82743338823081391461637904495155259577745245943759523887745013307703476565758809876523134280790536118393327786195407598238e+1L;
cldb PI8=2.51327412287183459077011470662360230735773551950008465677995567384625312502896719890242786027369365438571846921062584531768e+1L;
cldb PI7=2.19911485751285526692385036829565201893801857956257407468246121461547148440034629903962437773948194758750366055929761465297e+1L;
cldb PI6=1.88495559215387594307758602996770173051830163962506349258496675538468984377172539917682089520527024078928885190796938398826e+1L;
cldb PI5=1.57079632679489661923132169163975144209858469968755291048747229615390820314310449931401741267105853399107404325664115332355e+1L;
cldb PI4=1.25663706143591729538505735331180115367886775975004232838997783692312656251448359945121393013684682719285923460531292265884e+1L;
cldb PI3=9.42477796076937971538793014983850865259150819812531746292483377692344921885862699588410447602635120394644425953984691994128L;
cldb PI2=6.28318530717958647692528676655900576839433879875021164194988918461563281257241799725606965068423413596429617302656461329419L;
cldb PI =3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709L;
cldb PI_2=1.570796326794896619231321691639751442098584699687552910487472296153908203143104499314017412671058533991074043256641153323547L;
cldb PI_3=1.047197551196597746154214461093167628065723133125035273658314864102605468762069666209344941780705689327382695504427435549031L;
cldb PI_4=7.85398163397448309615660845819875721049292349843776455243736148076954101571552249657008706335529266995537021628320576661774e-1L;
cldb PI_5=6.28318530717958647692528676655900576839433879875021164194988918461563281257241799725606965068423413596429617302656461329419e-1L;
cldb PI_6=5.23598775598298873077107230546583814032861566562517636829157432051302734381034833104672470890352844663691347752213717774516e-1L;
cldb PI_7=4.48798950512827605494663340468500412028167057053586545853563513186830915183744142661147832191731009711735440930468900949585e-1L;
cldb PI_8=3.92699081698724154807830422909937860524646174921888227621868074038477050785776124828504353167764633497768510814160288330887e-1L;
cldb PI_9=3.49065850398865915384738153697722542688574377708345091219438288034201822920689888736448313926901896442460898501475811849677e-1L;
cldb PI_10=3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709e-1L;
cldb PI_11=2.85599332144526658042058489389045716745197218125009620088631326573437855116928090784366802303828824362013462410298391513372e-1L;
cldb PI_12=2.61799387799149436538553615273291907016430783281258818414578716025651367190517416552336235445176422331845673876106858887258e-1L;
cldb PI_13=2.41660973353061018343280260252269452630551492259623524690380353254447415868169922971387294257085928306319083577944792819007e-1L;
cldb PI_14=2.24399475256413802747331670234250206014083528526793272926781756593415457591872071330573916095865504855867720465234450474792e-1L;
cldb PI_15=2.09439510239319549230842892218633525613144626625007054731662972820521093752413933241868988356141137865476539100885487109806e-1L;
cldb PI_16=1.96349540849362077403915211454968930262323087460944113810934037019238525392888062414252176583882316748884255407080144165443e-1L;
cldb PI_17=1.84799567858223131674273140192911934364539376433829754174996740723989200369776999919296166196595121646008710971369547449829e-1L;
cldb PI_18=1.74532925199432957692369076848861271344287188854172545609719144017100911460344944368224156963450948221230449250737905924839e-1L;
cldb PI_19=1.65346981767883854655928599119973836010377336809216095840786557489885074015063631506738675018006161472744636132278016139321e-1L;
cldb PI_20=1.57079632679489661923132169163975144209858469968755291048747229615390820314310449931401741267105853399107404325664115332355e-1L;
cldb PI_21=1.49599650170942535164887780156166804009389019017862181951187837728943638394581380887049277397243669903911813643489633649862e-1L;

/* Inverse of PI (Inv2PI: 1/(2*pi); InvPI:1/pi, InvPI2:2/pi, InvPI3:3/pi) */
/* https://simtk.org/; Mathematica deg=N[Degree, 128]; rad=N[1/Degree,129]; */
/* #define DEG PI/180 /* to convert from degree to radian: *DEG */
/* #define RAD 180/PI /* to convert from radian to degree: *RAD */
cldb DEG=1.74532925199432957692369076848861271344287188854172545609719144017100911460344944368224156963450948221230449250737905924839e-2L;
cldb RAD=5.72957795130823208767981548141051703324054724665643215491602438612028471483215526324409689958511109441862233816328648932814e+1L;
cldb InvPI9=2.86478897565411604383990774070525851662027362332821607745801219306014235741607763162204844979255554720931116908164324466407L;
cldb InvPI3=9.54929658551372014613302580235086172206757874442738692486004064353380785805359210540682816597518515736437056360547748221357e-1L;
cldb InvPI2=6.36619772367581343075535053490057448137838582961825794990669376235587190536906140360455211065012343824291370907031832147572e-1L;
cldb InvPI =3.18309886183790671537767526745028724068919291480912897495334688117793595268453070180227605532506171912145685453515916073786e-1L;
cldb Inv2PI=1.59154943091895335768883763372514362034459645740456448747667344058896797634226535090113802766253085956072842726757958036893e-1L;

/* SqrtPI: square root of PI; CbrtPI: cubic root of PI;
 * SqrtPI2: sqrt(PI*2); SqrtInvPI:  sqrt(1/pi); SqrtInv2PI: sqrt(1/(2*pi)); */
cldb SqrtPI=1.77245385090551602729816748334114518279754945612238712821380778985291128459103218137495065673854466541622682362428257066624L;
cldb SqrtPI2=2.50662827463100050241576528481104525300698674060993831662992357634229365460784197494659583837805726611600997266520387964487L;
cldb SqrtInvPI=5.64189583547756286948079451560772585844050629328998856844085721710642468441493414486743660202107363443028347906361707351690e-1L;
cldb SqrtInv2PI=3.98942280401432677939946059934381868475858631164934657665925829670657925899301838501252333907306936430302558862635182685511e-1L;
cldb CbrtPI=1.46459188756152326302014252726379039173859685562793717435725593713839364979828626614568206782035382089750397001521899328092L;
cldb PIPI=9.86960440108935861883449099987615113531369940724079062641334937622004482241920524300177340371855223182402591377402314407777L;
#define PISQ PIPI /* PIPI: square of PI, PI*PI; PISQ3/4/5: PI squared3/4/5; */
cldb PISQ3=3.10062766802998201754763150671013952022252885658851076941445381038063949174657060375667010326028861930301219615723366223752e+1L;
cldb PISQ4=9.74090910340024372364403326887051112497275856726854216914678593899708554568271961901218672347529925509691732517238468173126e+1L;
cldb PISQ5=3.06019684785281453262741310043435606480300706628074990553492443686234992133614024485783500473505118904037420720260930296908e+2L;

/* EXP1: Natural Logarithmic Base; EULERS: Euler's Constant; GOLDRT: Golden Ratio; */
cldb EULERS=5.77215664901532860606512090082402431042159335939923598805767234884867726777664670936947063291746749514631447249807082480961e-1L;
cldb CATALAN=9.15965594177219015054603514932384110774149374281672134266498119621763019776254769479356512926115106248574422619196199579036e-1L;
cldb GOLDRT=1.61803398874989484820458683436563811772030917980576286213544862270526046281890244970720720418939113748475408807538689175213L;
/* EXP1: exponential constant E (base of natural logarithms) (Mathematica EXP1=N[E,129]); */
/* Exp[x]=E^x is the exponential function of x (Mathematica EXP2=N[Exp[2],129]); */
cldb EXP1=2.71828182845904523536028747135266249775724709369995957496696762772407663035354759457138217852516642742746639193200305992182L;
cldb EXP2=7.38905609893065022723042746057500781318031557055184732408712782252257379607905776338431248507912179477375316126547886612388L;
cldb EXP3=2.00855369231876677409285296545817178969879078385541501443789342296988458780919737312044971602530177021536076158519490028818e+1L;
cldb SqrtEXP1=1.64872127070012814684865078781416357165377610071014801157507931164066102119421560863277652005636664300286663775630779700467L;
cldb CbrtEXP1=1.39561242508608952862812531960258683759790651519940698261751670603173901564595184696978881729583022413521118441041886209612L;
cldb LN2=6.93147180559945309417232121458176568075500134360255254120680009493393621969694715605863326996418687542001481020570685733686e-1L;
cldb LN3=1.09861228866810969139524523692252570464749055782274945173469433363749429321860896687361575481373208878797002906595786574237L;
cldb LN4=1.38629436111989061883446424291635313615100026872051050824136001898678724393938943121172665399283737508400296204114137146737L;
cldb LN5=1.60943791243410037460075933322618763952560135426851772191264789147417898770765776463013387809317961079996630302171556289972L;
cldb LN7=1.94591014905531330510535274344317972963708472958186118845939014993757986275206926778765849858787152699306169420585114091172L;
cldb LN9=2.19722457733621938279049047384505140929498111564549890346938866727498858643721793374723150962746417757594005813191573148474L;
cldb LN10=2.30258509299404568401799145468436420760110148862877297603332790096757260967735248023599720508959829834196778404228624863341L;
cldb LOG2E=1.44269504088896340735992468100189213742664595415298593413544940693110921918118507988552662289350634449699751830965254425559L;
cldb LOG10E=4.34294481903251827651128918916605082294397005803666566114453783165864649208870774729224949338431748318706106744766303733642e-1L;

/* http://en.wikipedia.org/wiki/Square_root_of_2 (_3) (_5) */
cldb SQRT2=1.41421356237309504880168872420969807856967187537694807317667973799073247846210703885038753432764157273501384623091229702492L;
cldb SQRT3=1.73205080756887729352744634150587236694280525381038062805580697945193301690880003708114618675724857567562614141540670302997L;
cldb SQRT5=2.23606797749978969640917366873127623544061835961152572427089724541052092563780489941441440837878227496950817615077378350425L;
cldb SQRT6=2.44948974278317809819728407470589139196594748065667012843269256725096037745731502653985943310464023481859460122661418912486L;
cldb SQRT7=2.64575131106459059050161575363926042571025918308245018036833445920106882323028362776039288647454361061506457833849746309574L;
cldb SQRT8=2.82842712474619009760337744841939615713934375075389614635335947598146495692421407770077506865528314547002769246182459404985L;
cldb SQRT10=3.16227766016837933199889354443271853371955513932521682685750485279259443863923822134424810837930029518734728415284005514855L;
cldb SQRT11=3.31662479035539984911493273667068668392708854558935359705868214611648464260904384670884339912829065090701255784952745659228L;
cldb SQRT12=3.46410161513775458705489268301174473388561050762076125611161395890386603381760007416229237351449715135125228283081340605994L;
cldb CBRT2=1.25992104989487316476721060727822835057025146470150798008197511215529967651395948372939656243625509415431025603561566525940L;
cldb CBRT3=1.44224957030740838232163831078010958839186925349935057754641619454168759682999733985475547970564525668683508085448954996643L;
cldb CBRT4=1.58740105196819947475170563927230826039149332789985300980828576182521650562421917327354421326222095702293476168132201790350L;
cldb CBRT5=1.70997594667669698935310887254386010986805511054305492438286170744429592050417321625718701002018900220450328939045401808720L;
cldb CBRT6=1.81712059283213965889121175632726050242821046314121967148133429793130973945930186564714170412641707212634493249770240752706L;
cldb CBRT7=1.91293118277238910119911683954876028286243905034587576621064764044723427617923075600752544147728570990454191395879075922794L;
cldb CBRT9=2.08008382305190411453005682435788538633780534037326210969759108020010631139726877360605663679075748672867159208657452053891L;
cldb CBRT10=2.154434690031883721759293566519350495259344942192108582489235506346411106648340800185441503543243276101261220491780920446558L;

/* http://en.wikipedia.org/wiki/Repeating_decimal */
cldb Inv3=3.33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333e-1L;
cldb Inv6=1.66666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666667e-1L;
cldb Inv7=1.42857142857142857142857142857142857142857142857142857142857142857142857142857142857142857142857142857142857142857142857143e-1L;
cldb Inv9=1.11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111e-1L;
cldb Inv11=9.09090909090909090909090909090909090909090909090909090909090909090909090909090909090909090909090909090909090909090909090909e-2L;
cldb Inv12=8.33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333e-2L;
cldb Inv13=7.69230769230769230769230769230769230769230769230769230769230769230769230769230769230769230769230769230769230769230769230769e-2L;
cldb Inv14=7.14285714285714285714285714285714285714285714285714285714285714285714285714285714285714285714285714285714285714285714285714e-2L;
cldb Inv15=6.66666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666667e-2L;
cldb Inv17=5.88235294117647058823529411764705882352941176470588235294117647058823529411764705882352941176470588235294117647058823529412e-2L;
cldb Inv18=5.55555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555556e-2L;
cldb Inv19=5.26315789473684210526315789473684210526315789473684210526315789473684210526315789473684210526315789473684210526315789473684e-2L;
cldb Inv36=2.77777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777778e-2L;
cldb Inv45=2.22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222e-2L;
cldb Inv90=1.11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111e-2L;
cldb Inv180=5.55555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555556e-3L;
cldb Inv360=2.77777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777778e-3L;
/* ========================================================================== */