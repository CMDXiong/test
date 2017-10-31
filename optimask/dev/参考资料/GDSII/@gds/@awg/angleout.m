function s = angleout(str)

% ANGLEOUT produces output waveguide angles for AWG structure
% SF, April 27 2007

s = [];
N = length(str.calc.outang);
if N > 0
    s = [s sprintf('%.6f',str.calc.outang(1))];
end;
if N > 1
    for i=2:N
        s = [s ',' sprintf('%.6f',str.calc.outang(i))];
    end;
end;
