function poly = add2poly(poly,layer,data)

% Formats data and appends to a poly struct

if nargin > 2
    
    L = length(poly);

    poly(L+1).layer = layer;
    poly(L+1).datatype = 0;
    poly(L+1).n = length(data(1,:));
    poly(L+1).x = data(1,:);
    poly(L+1).y = data(2,:);
    poly(L+1) = pcheck(poly(L+1)); % closing polygon
    
end
   