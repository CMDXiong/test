function res = linecross(x0,y0,x,y)

% Returns one if line segment (x0,y0) crosses the line segment(x,y)
% and zero otherwise.

n = size(x,1);
dx = diff(x0);
dy = diff(y0);
DX = diff(x,1,2);
DY = diff(y,1,2);
x1 = x0(1);
y1 = y0(1);

p = DX*dy-DY*dx; 
np = find(p);
par = find(~p);
% parallel line segments
if ~isempty(par)
    % check if segments lie on same line
	u = dx*y1-dy*x1-DX(par).*y(par,1)+DY(par).*x(par,1) == 0 | (x(par,1) == x1 & ~dx) | (y(par,1) == y1 & ~dy);
    res(par(find(~u))) = 0;
    parline = par(find(u)); % segments that lie on same line
	if ~isempty(parline)
        u = abs(DX(parline)) > abs(DY(parline));
		ux = find(u);
		uy = find(~u);
		if ~isempty(ux)
            z1(ux) = (x(parline,1)-x1).*(x1-x(parline,2));
            z2(ux) = (x(parline,1)-x0(2)).*(x0(2)-x(parline,2));
            z3(ux) = (x(parline,1)-x1).*(x0(2)-x(parline,1));
            z4(ux) = (x(parline,2)-x1).*(x0(2)-x(parline,2));
		end
		if ~isempty(uy)
            z1(uy) = (y(parline,1)-y1).*(y1-y(parline,2));
            z2(uy) = (y(parline,1)-y0(2)).*(y0(2)-y(parline,2));
            z3(uy) = (y(parline,1)-y1).*(y0(2)-y(parline,1));
            z4(uy) = (y(parline,2)-y1).*(y0(2)-y(parline,2));
		end
        res(parline) = (z1 > 0) | (z2 > 0) | (z3 > 0) | (z4 > 0);
%         [x0;y0;res(parline)]
    end
end
% non-parallel line segments
if ~isempty(np)
	p = p(np);
	x = x(np,:);
	y = y(np,:);
	X1 = x(:,1);
	Y1 = y(:,1);
	DX = DX(np);
	DY = DY(np);
	xn = -dx*DY.*X1 + dy*DX*x1 + dx*(Y1-y1).*DX;
	yn = -dy*DX.*Y1 + dx*DY*y1 + dy*(X1-x1).*DY;
	% coordinates of intersection
	xint = xn./p;
	yint = -yn./p;
	if abs(dx) > abs(dy)
		z = (x1-xint).*(xint-x0(2));
	else
        z = (y1-yint).*(yint-y0(2));
	end
	u = abs(DX) > abs(DY);
	ux = find(u);
	uy = find(~u);
	if ~isempty(ux)
        Z(ux) = (X1(ux)-xint(ux)).*(xint(ux)-x(ux,2));
	end
	if ~isempty(uy)
		Z(uy) = (Y1(uy)-yint(uy)).*(yint(uy)-y(uy,2));
	end
	Z = Z';
	r = z >= 0 & Z >= 0 & z+Z >0;
	res(np) = r;
end