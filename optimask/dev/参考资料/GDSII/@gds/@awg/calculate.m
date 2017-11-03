function str = calculate(str)

% This function calculates CALC fields needed for AWG layout.
% SF, January 31, 2006

global alphai;
global lx;
global theta0;
global dl;
global bcoo;
global ji;

p = str.param;  %load AWG parameters
str.calc = [];  %delete previous AWG calculations
lx = p.Lx;      %initializing globals
theta0 = p.theta0;
dl = p.dL;
c = [];
  
if ((p.spacing(1)=='A') | (p.spacing(1)=='a'))      %equal angles
   c.dalpha = p.pitch/p.roland;
   for i = 1:p.num
      c.alpha(p.num+1+i) = i*c.dalpha;
      c.alpha(p.num+1-i) = -c.alpha(p.num+1+i);
   end;
else                                                %equal spacing
   for i = 1:p.num          
      c.alpha(p.num+1+i) = asin(i*p.pitch/p.roland);      %smaller diameter is prefered for larger wg-wg spacing
      c.alpha(p.num+1-i) = -c.alpha(p.num+1+i);
   end;
end;
c.alpha(p.num+1) = 0;


% bcoo is (the half of the length of the shortest grating wg)+ rsl
bcoo = lx/cos(theta0-p.beta1)*(cos(p.beta1+c.alpha(1))+sin(p.beta1+c.alpha(1))*(theta0+c.alpha(1)));
  
for i = 1:p.wgno
    ji = i-1;
    alphai = c.alpha(i);
    [c.beta(i),c.dddl(i)] = fzero(@deltpath,theta0);
end;

c.r = sin(c.beta+c.alpha).*lx./cos(c.beta-theta0);
c.di = cos(c.beta+c.alpha).*lx./cos(c.beta-theta0)-p.roland;   % straight WGs before bending
c.ao = lx.*tan(theta0-c.beta);
c.v = c.r+c.ao;
c.dv = c.v(2:p.wgno)-c.v(1:p.wgno-1);   % spacings of parallel arms

[c.min_di, c.min_di_i] = min(c.di);   %min_di_i is the location of this number in the array
  
if c.min_di_i == p.wgno
     c.min_space_before_bend = (p.roland+c.min_di)*( c.alpha(p.wgno)-c.alpha(p.wgno-1) );
else
     c.min_space_before_bend = (p.roland+c.min_di)*( c.alpha(c.min_di_i+1)-c.alpha(c.min_di_i) );
end;

str.calc = c;  %load calculation results





function ddl = deltpath(betai)

% This matlab function returns the length difference of  a set of 
% curved waveguides to the center waveguide plus n*dl
% ddl = length - bcoo - n*dl 

global alphai;
global lx;
global theta0;
global dl;
global bcoo;
global ji;

ddl=lx/cos(theta0-betai)*(cos(betai+alphai)+sin(betai+alphai)*(theta0+alphai))-bcoo-ji*dl/2;

