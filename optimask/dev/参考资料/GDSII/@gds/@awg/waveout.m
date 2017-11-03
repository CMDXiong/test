function s = waveout(str,field)

% WAVEOUT produces output waveguide coordinates for AWG structure
% FIELD='A','X' or 'Y' for angles, x coordinates or y coordinates, respectively
% SF, April 27 2007

if nargin < 2
    field = 'A';
end

s = [];

switch field
    
case 'A'

    N = length(str.calc.outang);
    if N > 0
    s = [s sprintf('%.6f',str.calc.outang(1))];
    end

    if N > 1
        for i=2:N
            s = [s ',' sprintf('%.6f',str.calc.outang(i))];
        end
    end
    
case 'X'
    
    N = length(str.calc.outx);
    if N > 0
    s = [s sprintf('%.3f',str.calc.outx(1))];
    end

    if N > 1
        for i=2:N
            s = [s ',' sprintf('%.3f',str.calc.outx(i))];
        end
    end
    
case 'Y'
    
    N = length(str.calc.outy);
    if N > 0
    s = [s sprintf('%.3f',str.calc.outy(1))];
    end

    if N > 1
        for i=2:N
            s = [s ',' sprintf('%.3f',str.calc.outy(i))];
        end
    end  
    
end
