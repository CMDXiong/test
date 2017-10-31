function p = ardummy(top,bot,num,layer)

% SLABDUMMY fills the AWG grating with num dummy rectangles on a separate LAYER

if nargin < 4
    layer = 30; % default
end

if nargin < 3
    num = 50; % default
end

p = [];
L1 = length(top(1,:));
L2 = length(bot(1,:));
top(1:2,L1+1) = top(1:2,L1);
bot(1:2,L2+1) = bot(1:2,L2);

if L1*L2 > 0

    for i = 0:2*num-1
        t = i*(L1-1)/(2*num-1);
        f = floor(t);
        ft = t - f;
        a1(1:2,i+1) = top(1:2,f+1)*(1-ft)+top(1:2,f+2)*ft;
        t = i*(L2-1)/(2*num-1);
        f = floor(t);
        ft = t - f;
        a2(1:2,i+1) = bot(1:2,f+1)*(1-ft)+bot(1:2,f+2)*ft;
    end
    
    for i = 1:2:2*num
        p = add2poly(p,layer,[a1(1:2,i) a1(1:2,i+1) a2(1:2,i+1) a2(1:2,i)]);
    end
    
else 
    disp('Empty array in ardummy input');
end




    
    
