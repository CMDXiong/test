function [curve_coor] = io_taper_curve2(origin,phi,dx,max_xx,coef0,coef1)

% This matlab function returns a 2xN array consisting coordinates of
% a curve. (1,n), (2,n) are the x, y coordinates of the curve, respectively.
% The function takes as inputs the parameters of the desired curve.  The curve 
% is described as yy=f(xx) in XX-YY Cartesian cooridinate, originated at (xx0,yy0)
% and an angle phi(CCW positive) relative to the absolute origin (0,0) and 
% X-axis, respectively. Coordinates transformation will calculate the absolute
% positions and return with a 2xN array, curve_coor.
%
% Arguments:
% origin       Type: vector   Coordinates origin (xx0, yy0)
% phi          Type: number   Rotation angle of the coordinates (CCW positive)
% dx           Type: number   Resolution along absolute X-axis
% max_xx       Type: number   (max_xx - 0), along the XX-axis
% coef0        Type: number   Required function coefficient 0
% coef1        Type: number   Required function coefficient 1
%
% Output:      Returns values. No side effects or I/O.
% curve_coor   Type: 2xN array coordinates of the curve


% polysize is the number of vertices to use for curve segments
% polysize should be an even number, 4 or greater.
polysize = 100;

% Error Checking
if dx < 0.001
   error('Resolution must be larger than 0.001');
end;
if max_xx < dx
   error('Total section must be larger than the resolution');
end;


% totsegs is the total number of dtheta-sized curve subsegments
totsegs = ceil(max_xx/dx);
dx = max_xx/totsegs;

xx = dx.*(0:totsegs);
yy = sqrt(coef1*xx+coef0^2)/2;

curve_coor(1,:) = cos(phi)*xx - sin(phi)*yy + origin(1);
curve_coor(2,:) = sin(phi)*xx + cos(phi)*yy + origin(2);
