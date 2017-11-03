function a=find(lib,X,Y)

t = 0;
p = lib.path;
for x = 1:length(p)
    s = p(x);
    if sum(s.xc == X & s.yc == Y)
%     if sum(s.yc == Y)
        t = t+1;
        a(t) = s;
    end
end
[a.xc' a.yc']/1e3
a.xc(1)-7493
a.xc(3)-5743
a.xc(2)-5743
a = library([],lib.name,lib.units,a);
