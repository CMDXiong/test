function lib = library(s,name,units,p)

% LIBRARY is the constructor for the LIBRARY class.
% LIBRARY(F) reads in a GDS file F and returns a LIBRARY
% object. F can be a GDSTXT file (ending in '.txt') or a GDSII
% format file (ending in '.gds').
%
% LIBRARY(STR,N) creates a structure library of name N with default units.
% LIBRARY(STR,N,U) uses units given in U.
%
% The following methods are available for the LIBRARY class:
%       DRAW,WRITEGDSTXT,FILLDUMMY,PLUS

% Attila Mekis, May 5, 2003

if nargin < 1
    s = [];
end
if nargin < 2
    name = 'EMPTY LIBRARY';
end
if nargin < 3
    units = [1e-3 1e-9];
end
if nargin < 4
    p = [];
end

if ~ischar(s)
    c = fix(clock);
	lib.version = 3;
	lib.libdate = [c c];
	lib.libname = name;
	lib.units = units;
	lib.strdate = [c c];
	lib.name = name;
    lib.poly = s;
    lib.path = p;
	lib.n = length(s);
	lib = class(lib,'library');
else
	disp('Reading GDS file...')
	fid = fopen(s);
	switch s(end-3:end)
	case '.txt'
        lib = readgdstxt(fid);
	case '.gds'
        lib = readgds(fid);
	end
	fclose(fid);
	lib.n = length(lib.poly)+length(lib.path);
	disp([num2str(lib.n) ' structures in GDS file.'])    
end

