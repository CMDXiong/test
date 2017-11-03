function writegds(lib,name)

% WRITEGDS(L) outputs a structure L to a GDS file.
% WRITEGDS(L,N) names the structure N and the GDS file [N '.gds'].

% SF, January 27, 2006

fid = fopen([name '.gds'],'w');
fwrite(fid,record(0,2,lib.version)); % header
fwrite(fid,record(1,2,lib.libdate)); % bgnlib
fwrite(fid,record(2,6,lib.libname)); % libname
fwrite(fid,record(3,5,lib.units)); % units
fwrite(fid,record(5,2,lib.strdate)); % bgnstr
fwrite(fid,record(6,6,lib.name)); % strname
conv = lib.units(2)*1e6;
p = lib.poly;
for x = 1:length(p)
    q = p(x);
	if q.n > 201
        disp('Number of polygon vertices exceeds 201.')
    end
	fwrite(fid,record(8)); % boundary
	fwrite(fid,record(13,2,q.layer)); % layer
	fwrite(fid,record(14,2,q.datatype)); % datatype
    xy = [];
    xy(1:2:q.n*2) = q.x/conv;
    xy(2:2:q.n*2) = q.y/conv;
	fwrite(fid,record(16,3,xy)); % xy
	fwrite(fid,record(17)); % endel
end
p = lib.path;
for x = 1:length(p)
    q = p(x);
	fwrite(fid,record(9)); % path
	fwrite(fid,record(13,2,q.layer)); % layer
	fwrite(fid,record(14,2,q.datatype)); % datatype
    if q.pathtype
		fwrite(fid,record(33,2,q.pathtype)); % pathtype
    end
	fwrite(fid,record(15,3,q.width/conv)); % width
    xy = [];
    xy(1:2:q.nc*2) = q.xc/conv;
    xy(2:2:q.nc*2) = q.yc/conv;
	fwrite(fid,record(16,3,xy)); % xy
	fwrite(fid,record(17)); % endel
end
fwrite(fid,record(7)); % endstr
fwrite(fid,record(4)); % endlib
fclose(fid);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5

function r = record(tk,d,dt)

% RECORD(TOKEN,DATATYPE,DATA) returns a record

if nargin < 2
    d = 0;
    dt = [];
end
len = length(dt);
switch d
case 0 % no data
    data = [];
case 2 % two-byte signed integer
    %u = find(dt<0);
    %dt(u) = 2^16+1 - dt(u); 
    for x = 1:len
        data(2*x+[-1 0]) = floor(dt(x)./2.^[8 0]) - 256*floor(dt(x)./2.^[16 8]);                
    end
case 3 % four-byte signed integer
    %u = find(dt<0);
    %dt(u) = 2^32+1 - dt(u); 
    for x = 1:len
        data(4*x+[-3:0]) = floor(dt(x)./2.^[24:-8:0]) - 256*floor(dt(x)./2.^[32:-8:8]);                
    end
case 5 % eight-byte real
    for x = 1:len
        expn = ceil(log(dt(x))/log(2)/4);
        data(8*x-7) = expn + 64 + 128*(dt(x)<0);
        mant = dt(x)/16^expn;
        data(8*x+[-6:0]) = floor(mant./2.^[-8:-8:-56]) - 256*floor(mant./2.^[0:-8:-48]);                
    end
case 6 % ASCII string
    data = double(dt);
    if mod(length(dt),2)
        data(end+1) = 0; % add pad character
    end
otherwise
    disp(['Unknown data type ' num2str(type)])
end

len = length(data)+4;
r(1) = floor(len/256);
r = [r len-256*r(1) tk d data];
