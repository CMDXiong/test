function d = polyinside(x,y,X,Y)

% Returns 1 if point is inside and 0 if outside polygon.

a = 0;
for t = 1:length(X)-1
    x1 = X(t)-x;
    x2 = X(t+1)-x;
    y1 = Y(t)-y;
    y2 = Y(t+1)-y;
    a = a + atan2(x1*y2-y1*x2,x1*x2+y1*y2);
end
d = (abs(a)>pi);
