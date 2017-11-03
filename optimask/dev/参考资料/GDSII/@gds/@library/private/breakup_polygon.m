function [found,vx1,vy1,vx2,vy2] = breakup_polygon(x,y,n)

% BREAKUP_POLYGON(X,Y,N) breaks up the polygon (X,Y) into two polygons, 
% one of which has no more than N vertices.

% Attila Mekis, December 23, 2003

X = x;
Y = y;
x = x(1:end-1);
y = y(1:end-1);
nvert = length(x);
% Scan through possible dividing lines and make sure they do not 
% intersect with any of the polygon sides.
found = 0;
dvert = n; % Start with lines given by points farthest apart.
while dvert > 1 & ~found
    dvert = dvert-1;
    v1 = 0;
    while v1 < nvert-dvert & ~found
        v2 = mod(v1+dvert-1,nvert)+1;
        v1 = v1+1;
%         [v1 v2 found]
        divx = x([v1 v2]);
        divy = y([v1 v2]);
        sx = sum(linecross(divx,divy,[x(1:end-1)' x(2:end)'],[y(1:end-1)' y(2:end)']));
%         hold on; plot(divx,divy,'r');hold off
        if ~sx % non-intersecting line
            % check if line is inside polygon
            found = polyinside(mean(divx),mean(divy),X,Y);
        end
    end
end 
if found
    vx1 = x([v1:v2 v1]);
    vy1 = y(+[v1:v2 v1]);
    vx2 = [x(v2:end) x(1:v1) x(v2)];
    vy2 = [y(v2:end) y(1:v1) y(v2)];
else
    vx1 = x;
    vy1 = y;
    vx2 = [];
    vy2 = [];
end
