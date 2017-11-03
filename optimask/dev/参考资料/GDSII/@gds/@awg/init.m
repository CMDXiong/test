function str = init(str)

% INIT is used to initialize 1st group of AWG parameter fields to their default values.
% SF, January 30, 2006

%DEFAULT PARAMETERS
    
                                %CONSTANTS:

p.c_light = 299792458.0;        %Speed of light in m/sec
p.pi180 = pi/180.0;

                                %FIXED INPUT PARAMETERS:
                                    
p.min_D = 9.0; %13.5;           %Minimum receiver waveguide pitch in microns
p.inslab = 1.46280;             %Slab index
p.inchannel = 1.45587;          %Channel index
p.dNc_dLambda = -2.028e-5;      %Dispersion in 1/nm
p.width = 4;                    %WG width in microns
p.cdloss = 0.2;                 %CD loss between corresponding widths on a mask and on a wafer
p.gap = 1.3;                    %Smallest gap between waveguides
p.dum = 3;                      %Number of grating dummy waveguides on each side

                                %VARIED INPUT PARAMETERS
                                
p.freq0 = 193200.0;               %Center frequency in GHz
p.channel = 100.0;                %AWG channel spacing in GHz
p.order = 38;                   %Grating order
p.pitch = 8.0;                    %Gratin pitch in microns
p.roland = 6000.0;                %Roland circle diameter in microns
p.inwave = 9.0;                   %Input waveguide width
p.tap_len = 800;                %Grating taper length
p.num = 100 + p.dum;            %Half of active waveguides
p.wgno = 2*p.num + 1;           %All waveguides including dummies
p.spacing = 'c';                %Equal angle 'a' or chord 'c' spacing

                                %MAIN DESIGN VARIABLES

p.Lx = 4500.0;                    %Design parameter Lx, ~one half length of AWG
p.thetaang = 65.0;                %Design parameter Theta0, angle of the grating central waveguide
p.theta0 = p.pi180*p.thetaang;  %Same in radians
p.betaang = 20.0;                 %Design parameter Beta1, ~grating angular spread
p.beta1 = p.pi180*p.betaang;    %Same in radians

                                %OTHERS
                                
p.max_curve_err = 8e-5;         %Maximum deviation from ideal arc

str.param = p;                  %Load into str
str = pcalc(str);               %Feed the rest of parameters

