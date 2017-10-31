function arccoor = arccospn(arccent,radius,theta0,theta_span,width,dtheta)
% This function returns a 4 x N array consisting coordinates of
% an arc with width. (1,n), (2,n), (3,n), (4,n) are the x, y coordinates
% of the inner arc and the x, y coordinates of the outer arc, respectively.
% The function takes as inputs the
% parameters of the desired arc.  The arc is described in polar
% fashion, using its center, radius, starting angle, and angle span
% Positive angles are counter-clockwise.  Additional parameters include
% the width of the arc and the constant angle increments 
%

% Arguments:
% arccent      Type: vector   Coordinates of center of curvature
% radius       Type: number   Radius of curvature at centerline
% theta0       Type: number   Angle of starting point referenced to +x axis
% theta_span   Type: number   Angle span (CCW positive)
% width        Type: number   Width of arc
% dtheta       Type: number   Length of segments used to draw arc, in rad
% theta        Type: number   Angular span of arc in radians (CCW positive)

% polysize is the number of vertices to use for arc segments
% polysize should be an even number, 4 or greater.
polysize = 100;

% Error Checking
if radius == 0
   error('Called arc with radius = 0');
end
if theta_span == 0
   error('Called arc with theta = 0');
end
if dtheta == 0
   error('Called arc with dtheta = 0');
end


% If the user wants to draw a clockwise arc, he should use a negative value 
% of theta0. Only the absolute value of dtheta matters.
dtheta = abs(dtheta);

% totsegs is the total number of dtheta-sized arc subsegments
totsegs = ceil(abs(theta_span/dtheta));
dtheta = theta_span/totsegs;

inradius = radius-width/2;
outradius = radius+width/2;

theta=theta0 + dtheta.*(0:totsegs);
arccoor(1,:) = inradius.*cos(theta)+arccent(1);
arccoor(2,:) = inradius.*sin(theta)+arccent(2);
arccoor(3,:) = outradius.*cos(theta)+arccent(1);
arccoor(4,:) = outradius.*sin(theta)+arccent(2);
