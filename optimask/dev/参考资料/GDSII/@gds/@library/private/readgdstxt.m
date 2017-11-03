function lib = readgdstxt(fid)

% Reads the GDSTXT file with ID FID and returns a structure library.

% Attila Mekis, May 5, 2003

lib = library;
a = read(fid,'HEADER');
lib.version = str2num(a);
a = read(fid,'BGNLIB');
lib.libdate = str2date(a);
a = read(fid,'LIBNAME');
lib.libname = strrep(a,' ','');
a = read(fid,'UNITS');
lib.units = str2num(a);
% everything will be stored in nm internally
conv = lib.units(2)*1e6;
a = read(fid,'BGNSTR');
lib.strdate = str2date(a);
a = read(fid,'STRNAME');
lib.name = strrep(a,' ','');
fgetl(fid);
% Read in strucures
npoly = 1;
npath = 1;
r = fgetl(fid);
while ~strcmp(r,'ENDSTR')
    r = r(4:end);
    switch r
    case 'BOUNDARY'
        a = read(fid,'      LAYER');
		lib.poly(npoly).layer = str2num(a);
        a = read(fid,'      DATATYPE');
		lib.poly(npoly).datatype = str2num(a);
        a = read(fid,'      XY');
        xy0 = str2num(strrep(a,':',' '));
        xy = fscanf(fid,'%d:%d\n',inf);
        lib.poly(npoly).x = [xy0(1) xy(1:2:end)']*conv;
        lib.poly(npoly).y = [xy0(2) xy(2:2:end)']*conv;
        lib.poly(npoly).n = 1+size(xy,1)/2;
        npoly = npoly+1;
    case 'PATH'
        a = read(fid,'      LAYER');
		lib.path(npath).layer = str2num(a);
        a = read(fid,'      DATATYPE');
		lib.path(npath).datatype = str2num(a);
%         a = read(fid,'      PATHTYPE');
%     	lib.path(npath).pathtype = str2num(a);
    	lib.path(npath).pathtype = 0;
        a = read(fid,'      WIDTH');
    	lib.path(npath).width = str2num(a)*conv;
        a = read(fid,'      XY');
        xy0 = str2num(strrep(a,':',' '));
        xy = fscanf(fid,'%d:%d\n',inf);
        % path centers
        lib.path(npath).xc = [xy0(1) xy(1:2:end)']*conv;
        lib.path(npath).yc = [xy0(2) xy(2:2:end)']*conv;
        % path boundaries
        [lib.path(npath).x,lib.path(npath).y] = pathbdry(lib.path(npath));
        lib.path(npath).n = length(lib.path(npath).x);
        lib.path(npath).nc = length(lib.path(npath).xc);
        npath = npath+1;
    otherwise
        disp(['Unknown type of element: ' r])
    end
    fgetl(fid);
    fgetl(fid);
    fgetl(fid);
    if ~mod(npoly+npath,1000)
        disp([num2str(npoly+npath) ' structures read...'])
    end
    r = fgetl(fid);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function a = read(fid,s)

% READ(FID,S) reads a file FID until it finds string S and returns the line
% w/o the string and the ending ';'

while 1
    a = fgetl(fid);
    if ~isempty(a) & (length(a) >= length(s) & strcmp(a(1:length(s)),s) | a == -1)
        break
    end
end
a = strrep(a,s,'');
if strcmp(a(end),';')
    a(end) = [];
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function d = str2date(s)

% Converts a '    06/23/2003 14:45:43 06/23/2003 14:45:43' type string to 
% a vector containing two dates.

s = fliplr(deblank(fliplr(s)));
u = findstr(s,' ');
d1 = datevec(datenum(s(1:u(1)-1)) + datenum(s(u(1)+1:u(2)-1)));
d2 = datevec(datenum(s(u(2)+1:u(3)-1)) + datenum(s(u(3)+1:end)));
d = fix([d1 d2]);