function  str = rename(str,name)

% RENAME renames the STR to NAME; otherwise it is TEST.

if nargin < 2
    name = 'TEST';
end

str.name = name;
str.gds = rename(str.gds, name);
