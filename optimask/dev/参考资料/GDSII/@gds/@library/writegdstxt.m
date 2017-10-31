function writegdstxt(lib,name)

% WRITEGDSTXT(L) outputs a structure library L to a GDSTXT file.
% WRITEGDSTXT(L,N) names the GDSTXT file [N '.txt'].

% Attila Mekis, May 5, 2003

if nargin > 1
    lib = rename(lib,name);
end
d = fix(clock);
fid = fopen([lib.name '.txt'],'w+');
fprintf(fid,'#------------------------------------------------------------------\n');
fprintf(fid,'# %s\n#\n',lib.name);
fprintf(fid,'# This file was created by the Inplane Photonics mask layout tool.\n');
fprintf(fid,'# (c) Inplane Photonics, Inc., 2003, Attila Mekis\n');
fprintf(fid,'#------------------------------------------------------------------\n\n');
fprintf(fid,'#----- GDS-II Stream Format: Version %d\n\n',lib.version);
fprintf(fid,'HEADER          %d;\n\n',lib.version);
fprintf(fid,'#----- Library: %s.txt\n\n',lib.libname);
fprintf(fid,'BGNLIB          %s;\n',date2str(lib.libdate));
fprintf(fid,'LIBNAME         %s.txt;\n',lib.libname);
fprintf(fid,'UNITS           %e %e;\n\n',lib.units);
fprintf(fid,'#----- Structure: %s\n\n',lib.name);
fprintf(fid,'BGNSTR          %s;\n',date2str(lib.strdate));
fprintf(fid,'STRNAME         %s;\n\n',lib.name);

conv = lib.units(2)*1e6;
poly = lib.poly;
for x = 1:length(poly)
    q = poly(x);
    xc = round(q.x/conv);
    yc = round(q.y/conv);
    fprintf(fid,['   BOUNDARY\n']);
    fprintf(fid,'      LAYER          %d;\n',q.layer);
    fprintf(fid,'      DATATYPE       %d;\n',q.datatype);
    fprintf(fid,'      XY             %11d:%11d\n',xc(1),yc(1));
    for y = 2:length(q.x)-1
        fprintf(fid,'                     %11d:%11d\n',xc(y),yc(y));
	end
    fprintf(fid,'                     %11d:%11d;\n',xc(end),yc(end));
    fprintf(fid,'   ENDEL\n\n');
end
path = lib.path;
for x = 1:length(path)
    q = path(x);
    xc = round(q.xc/conv);
    yc = round(q.yc/conv);
    fprintf(fid,['   PATH\n']);
    fprintf(fid,'      LAYER          %d;\n',q.layer);
    fprintf(fid,'      DATATYPE       %d;\n',q.datatype);
    if q.pathtype
        fprintf(fid,'      PATHTYPE       %d;\n',q.pathtype);
    end
    fprintf(fid,'      WIDTH          %d;\n',round(q.width/conv));
    fprintf(fid,'      XY             %11d:%11d\n',xc(1),yc(1));
    for y = 2:length(q.xc)-1
        fprintf(fid,'                     %11d:%11d\n',xc(y),yc(y));
	end
    fprintf(fid,'                     %11d:%11d;\n',xc(end),yc(end));
    fprintf(fid,'   ENDEL\n\n');
end
fprintf(fid,'ENDSTR\n\nENDLIB\n');
fclose(fid);
return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function s = date2str(d)

% Converts a vector containing two dates into a
% '    06/23/2003 14:45:43 06/23/2003 14:45:43' type string.

s = [datestr(datenum(d(1:3)),23) ' ' datestr(datenum(d(4:6)),13) ' '];
s = [s datestr(datenum(d(7:9)),23) ' ' datestr(datenum(d(10:12)),13)];