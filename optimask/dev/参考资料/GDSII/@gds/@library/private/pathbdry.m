function [x,y] = pathbdry(p)

% Returns coordniates of the boundary of path with center (p.xc,p.yc),
% width p.width and type p.pathtype

if p.pathtype 
    disp('Unknown path type.')
end

w = p.width/2;
xc = p.xc;
yc = p.yc;

n = length(xc);
dx = diff(xc);
dy = diff(yc);
l = sqrt(dx.^2+dy.^2);
% connecting lines: c*x+s*y = p
c = dy./l;
s = -dx./l;
u1 = [1:n-1];
u2 = [2:n];
p = (xc(u1).*yc(u2)-xc(u2).*yc(u1))./l;
% line intersections below path (xx,yx)
n = n-1;
u1 = [1:n-1];
u2 = [2:n];
p = p+w;
l = c(u1).*s(u2)-c(u2).*s(u1);
xx = (p(u1).*s(u2)-s(u1).*p(u2))./l;
yx = (p(u2).*c(u1)-c(u2).*p(u1))./l;
xb = [xc(1)+w*c(1) xx xc(end)+w*c(end)];
yb = [yc(1)+w*s(1) yx yc(end)+w*s(end)];
% line intersections above path (xx,yx)
p = p-2*w;
l = c(u1).*s(u2)-c(u2).*s(u1);
xx = (p(u1).*s(u2)-s(u1).*p(u2))./l;
yx = (p(u2).*c(u1)-c(u2).*p(u1))./l;
xa = [xc(1)-w*c(1) xx xc(end)-w*c(end)];
ya = [yc(1)-w*s(1) yx yc(end)-w*s(end)];

x = [xb fliplr(xa) xb(1)];
y = [yb fliplr(ya) yb(1)];
