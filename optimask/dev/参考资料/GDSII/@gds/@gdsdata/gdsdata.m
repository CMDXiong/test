function lib = gdsdata(s,name,units,p)

% GDSdata is the constructor for the GDSdata class.
% GDSdata(F) reads in a GDS file F and returns a GDSdata object.
% GDSdata(STR,N) creates a structure library of name N with default units.
% GDSdata(STR,N,U) uses units given in U.
% SF, January 27, 2006

if nargin < 1
    s = [];
end

if nargin < 2
    name = 'NONAME';
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
	lib.n = length(s)+length(p);
	lib = class(lib,'GDSdata');
else
    s = [s '.gds'];
	disp('Reading GDS file...')
	fid = fopen(s);
	lib = readgds(fid);
	fclose(fid);
	lib.n = length(lib.poly)+length(lib.path);
	disp([num2str(lib.n) ' structures in GDS file.'])    
end

