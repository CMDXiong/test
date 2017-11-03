function str = test(str,theta,lx,beta,disp)

% This is a function for testing AWG layout
% SF, April 5, 2006

if nargin < 5
    disp = 1;
end

if nargin < 4
    beta = 20;
end

if nargin < 3
    lx = 4500;
end

if nargin < 2
    theta = 65;
end

str.param.thetaang = theta;
str.param.theta0 = str.param.pi180*str.param.thetaang;
str.param.Lx = lx;
str.param.betaang = beta;       
str.param.beta1 = str.param.betaang*str.param.pi180;

str = calculate(str);
str = grating(str,1);

if disp == 1
    display(str,'gds');
end

