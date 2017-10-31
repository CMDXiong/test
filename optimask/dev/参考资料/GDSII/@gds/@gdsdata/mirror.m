function newlib = mirror(lib,x,y)

% This method mirrors all structures in LIB around origin

newlib = lib;

if nargin < 3
    y = 1;
end

if nargin < 2
    x = 1;
end
     
        
for i = 1:length(newlib.poly)
            
    newlib.poly(i).x = newlib.poly(i).x*x;
    newlib.poly(i).y = newlib.poly(i).y*y;
        
end
    
for i = 1:length(newlib.path)
            
    newlib.path(i).x = newlib.path(i).x*x;
    newlib.path(i).y = newlib.path(i).y*y;
        
end
    
