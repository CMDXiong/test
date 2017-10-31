function lib = fix(d,a)

bdry = [0 10000 0 4000];
p = d.poly;
n = 0;
for x = 1:length(p)
    % Count only polygons that are entirely within boundaries
    q = p(x);
    if q.x>=bdry(1) & q.x<=bdry(2) & q.y>=bdry(3) & q.y<=bdry(4)
        n = n+1;
        if ~isempty(find(q.x == 9610))
            q.x(find(q.x==9610)) = 9690;
        end
        if ~isempty(find(q.x == 1610))
            q.x(find(q.x==1610)) = 1690;
        end
        pnew(n) = q;
    end
end

for x = 1:4
    ns = 11.5-7*abs(x-2.5);
    s1 = square(2605-[1:ns]*20,1113+575*(x-1),10,6);
    s2 = square(7385+[1:ns]*20,1113+575*(x-1),10,6);
    pnew = [pnew s1 s2];
end

n = length(pnew);
% pol = pnew;
for x = 1:n
    q = pnew(x);
    pol(x+[0:9]*n) = q;
    for y = 0:4
        pol(x+n*y).y = q.y + y*4000;
        pol(x+n*(y+5)).y = q.y + y*4000;
        pol(x+n*(y+5)).x = q.x + 10000;
	end
end

length(pol)

lib = library([a.poly pol],'TM6_1_arrays');

function s = square(X,Y,a,layer)

% SQUARE(X,Y,A,L) returns an array of squares with centers (X(I),Y(I))
% and sides A on layer L.

n = length(X);
[s(1:n).element] = deal('   BOUNDARY');
[s(1:n).layer] = deal(layer);
[s(1:n).type] = deal(0);
[s(1:n).pathtype] = deal([]);
[s(1:n).width] = deal([]);
for x = 1:n
    s(x).x = X(x) + a/2*[-1 1 1 -1 -1];
    s(x).y = Y + a/2*[-1 -1 1 1 -1];
    s(x).n = 5;
end
