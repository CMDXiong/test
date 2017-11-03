%function AREF_Coord_Calc_P0_P1_P2(P0, ANG, N_col, N_row, d_col, d_row)
%example from Instance.gds 
%2017-03-22 Henghua Deng for Xiong Pan. %See QQ logs.

%Let's ALWAYS define ANGLE in degree, and ANG in radian.
ANGLE = 45;         %ANGLE unit in degree; 
ANG = (ANGLE/180)*pi; %ANG unit in radian;

P0 = 10 + 10j; % For AREF, (Array) Reference Point; for SREF, only need P0.

N_col = 3; N_row = 3; %COLROW = [3 3]; %COLROW 3 3
d_col = 20; %distance between adjacent columns
d_row = 20; %distance between adjacent rows

P1 = P0 + d_col*N_col*exp(1j*ANG); %axis 1 (rotated from X-axis) 
P2 = P0 + d_row*N_row*exp(1j*(ANG+pi/2)); %axis 2 (rotated from Y-axis) 

% ????(1816023692)11:16:04 PM
% ??????? P= ( x+j*y)  ?????angle?????? ( x+j*y) *exp(j*angle)?
% ??exp????????j??????
% % ( x+j*y) *exp(j*ang)???ang??????????? 
% ????????????? exp(j*ang)=cos(ang)+j*sin(ang)???????????? 
% ( x+j*y) *exp(j*ang)
% =( x+j*y) *[cos(ang)+j*sin(ang)] 
% = [x*cos(ang)-y*sin(ang)]+j*[x*sin(ang)+y*cos(ang)]
% ??????[X,Y]????

