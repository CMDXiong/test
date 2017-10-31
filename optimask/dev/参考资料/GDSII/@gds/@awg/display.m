function s = display(str,field)

% This is a standard display function for AWG class
% Use 'field' to show the contents of its fields
% SF, January 31, 2006

disp(['AWG structure "' str.name '"']);

if nargin > 1 & ischar(field)
    
    switch field
        
    case 'param'
        
        disp('Contents of ".param" field:');
        disp(str.param);
        
    case 'calc'
        
        disp('Contents of ".calc" field:');
        disp(str.calc);
        
    case 'gds'
        
        if ~isempty(str.gds)
            draw(str.gds,6);
        else
            disp('No gds data ...');
        end
        
    case 'comment'
        
        disp('Contents of ".comment" field:');
        disp(str.comment);
        
    otherwise
        
        disp(['Cannot show contents of "' field '" field']);
        
    end
    
end

disp(' ');
