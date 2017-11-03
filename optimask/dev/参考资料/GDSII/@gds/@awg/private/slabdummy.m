function p = slabdummy(slab,size,layer)

% SLABDUMMY fills the AWG slab with dummy rectangles of height SIZE on a separate LAYER

if nargin < 3
    layer = 30; % default
end

if nargin < 2
    size = 60; % default in um
end

p = [];

if length(slab(1,:)) == 6
    
    Lf = slab(1:2,4) - slab(1:2,5);                                 % left side
    Rt = slab(1:2,1) - slab(1:2,6);                                 % right side
    Ht = (slab(1:2,1)+slab(1:2,4))/2 - (slab(1:2,5)+slab(1:2,6))/2; % shift
    Len = sqrt(Ht(1)^2 + Ht(2)^2);                                  % height
    Num = floor(Len/size/2);                                        % number of dummies
    Lf = Lf/Len*size;
    Rt = Rt/Len*size;

    for i = 0:Num-1
        lf = slab(1:2,5) + 2*Lf*i;
        rt = slab(1:2,6) + 2*Rt*i;
        p = add2poly(p,layer,[lf rt rt+Rt lf+Lf]);                        % making Num of polygon dummies
    end
    
else 
    disp('Wrong size slab input in SLABDUMMY');
end


    
    
