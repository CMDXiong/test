function  p = display(lib,i,j)

% This method allows to inspect LIB and its contents
% Use display(lib,i) to access contents of polygons(i=1) and paths(i=2)
% Use display(lib,i,j) to access contents of polygon or path #j

if nargin < 2
    
    disp(' ');
    disp(['GDS data "' lib.name '":']);
    disp(['GDSII file version is ', num2str(lib.version)]);
    disp(['Data created on ' sprintf('%d/%d/%d',lib.libdate(1,2),lib.libdate(1,3),lib.libdate(1,1))]); 
    disp(['Database unit = ' num2str(lib.units(1)) ' user units']);
    disp(['User unit = ' num2str(lib.units(2)*1e9) ' nm']);
    disp(['Number of structures = ' num2str(lib.n)]);
    disp(' ');
    
else
    
    if nargin < 3
        switch i
            
        case 1 % polygons
            
            p = lib.poly;
            
        case 2 % paths
            
            p = lib.path;
            
        otherwise % nothing else
            
        end
        
    else
        
        switch i
            
        case 1 % polygons
            
            if j > length(lib.poly)
                j=length(lib.poly);
            end
            p = lib.poly(j);
            
        case 2 % paths
            
            if j > length(lib.path)
                j=length(lib.path);
            end
            p = lib.path;
            
        otherwise % nothing else
            
        end
        
    end
    
    
    
    
    
end
