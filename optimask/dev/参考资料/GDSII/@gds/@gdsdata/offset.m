function newlib = offset(lib,off,layer)

% This method offsets all or some structures in LIB by off=[x y]

newlib = lib;

if nargin < 2
    off = [0 0]
end
    
if nargin < 3 
        
    for i = 1:length(newlib.poly)
            
        newlib.poly(i).x = newlib.poly(i).x + off(1);
        newlib.poly(i).y = newlib.poly(i).y + off(2);
        
    end
    
    for i = 1:length(newlib.path)
            
        newlib.path(i).x = newlib.path(i).x + off(1);
        newlib.path(i).y = newlib.path(i).y + off(2);
        
    end
    
else
    
    for i = 1:length(newlib.poly)
            
        if newlib.poly(i).layer == layer
            newlib.poly(i).x = newlib.poly(i).x + off(1);
            newlib.poly(i).y = newlib.poly(i).y + off(2);
        end 
        
    end
    
    for i = 1:length(newlib.path)
        
        if newlib.poly(i).layer == layer
            newlib.path(i).x = newlib.path(i).x + off(1);
            newlib.path(i).y = newlib.path(i).y + off(2); 
        end 
            
    end
    
end
