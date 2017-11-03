function str = pcalc(str)

% PCALC calculates the values of the 2nd AWG parameter group.
% SF, January 31, 2006

p = str.param;

p.lambda0 = p.c_light/p.freq0/1000.0;                           %center wavelength in um
p.Nc_nc = 1-(p.lambda0*1000.0/p.inchannel)*p.dNc_dLambda;
p.dlambda = p.c_light/p.freq0^2*p.channel/1000.0;               %channel spacing in um
p.fsr = p.freq0/p.order;                                        %free spectral range
p.dL = p.order*p.lambda0/p.inchannel;                           %constant light path difference (in um)
p.D = p.roland*p.Nc_nc*p.order*p.dlambda/(p.pitch*p.inslab);    %receiver pitch in um

% Simulate slab propagation and calculate divergence angle at 1% peak amplitude
% for 1.5%-delta SM wavegude taper input:
% for 10um wide taper input half_1percent_div_angle = 0.12;   
% approximation:

div_angle = 7.56*10^-4*p.inwave^2 -0.024608*p.inwave +0.28754 ; % half width of 1% peak amplitude profile
p.num = ceil(p.roland*sin(div_angle)/p.pitch)+p.dum;            % 1/2 number of grating waveguides
p.wgno = 2*p.num+1;                                             % number of all waveguides

str.param = p;

