function str = awg(name)

% AWG is the constructor for the AWG class.
% It either initializes str to the default parameters or loads
% previously calculated AWG parameters from "name.awg" text file.
% SF, January 31, 2006

if nargin < 1
    name = [];
end

if ~ischar(name)                    % Empty AWG structure
    
    str.name = 'EMPTY';             % Name field
    str.param = [];                 % Structure array for fixed and some calculated input parameters
    str.calc = [];                  % Structure array for calculated variables
    str.gds = gdsdata;              % Data for GDS output
    str.comment = [];               % Comments section
    
    str = class(str,'AWG');
    
else
    
    fname = [name '.mat'];
    fid = fopen(fname);
    str = awg;
    
    if fid == -1
        
        disp('No such file, creating new AWG structure ...');
        str = rename(str,name);
        
    else
        
        disp('File found, reading data ...');
        fclose(fid);
        cont = whos('-file',name,'str');
        
        if ~isempty(cont)
            
            load(name);
            
        else
            disp('This file does not contain required variables, creating new AWG structure ...');
            str = rename(str,name);
        end
        
    end
    
end


    
    