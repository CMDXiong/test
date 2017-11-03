function libnew = picklayer(lib,layer)

% PICKLAYER(LIB,L) picks structures from the structure library LIB on layer L only.

p = lib.poly;
t = 1;
lp = [p.layer];
p(lp~=layer) = [];
libnew = lib;
libnew.poly = p;
libnew.n = length(p);