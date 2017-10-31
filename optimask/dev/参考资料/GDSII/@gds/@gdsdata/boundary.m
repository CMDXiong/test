function [xmin, ymin, xmax, ymax] = boundary(lib,layer)

% This method calculates LIB boundaries; layer is inactive for now
% SF, January 30, 2006

xmin = inf;
ymin = inf;
xmax = -inf;
ymax = -inf;

    
if nargin < 2 
    
    Len1 = length(lib.poly);
    
    if Len1 > 0
        
        xmin = min(lib.poly(1).x);
        ymin = min(lib.poly(1).y);
        xmax = max(lib.poly(1).x);
        ymax = max(lib.poly(1).y);
        
        if Len1 > 1
            
            for i = 2:Len1
                
                xmin = min(xmin, min(lib.poly(i).x));
                ymin = min(ymin, min(lib.poly(i).y));
                xmax = max(xmax, max(lib.poly(i).x));
                ymax = max(ymax, max(lib.poly(i).y));
                
            end
            
        end
        
    end
    
    Len2 = length(lib.path);
    
    if Len2 > 0
        
        xmin = min(xmin, min(lib.path(1).x));
        ymin = min(ymin, min(lib.path(1).y));
        xmax = max(xmax, max(lib.path(1).x));
        ymax = max(ymax, max(lib.path(1).y));
        
        if Len2 > 1
            
            for i = 2:Len2
                
                xmin = min(xmin, min(lib.path(i).x));
                ymin = min(ymin, min(lib.path(i).y));
                xmax = max(xmax, max(lib.path(i).x));
                ymax = max(ymax, max(lib.path(i).y));
                
            end
            
        end
        
    end
    
    if Len1 + Len2 == 0
        
        xmin = 0;
        ymin = 0;
        xmax = 0;
        ymax = 0;
        
    end
    
end
        
        
        
