function lib = readgds(fid)

% Reads the GDS file with ID FID and returns a structure library.

% Attila Mekis, May 5, 2003

lib = library;
a = fread(fid);
disp('Processing GDS file...')
t = 0;
npoly = 0;
npath = 0;
endlib = 0;
done = 0;
el = -1;
while ~endlib & t < length(a)
    if floor(t/length(a)*10) > done
        done = done+1;
        fprintf(2,'%d%% ',done*10)
    end
    % record length
    len = 256*a(t+1) + a(t+2);
    token = a(t+3); % record type       
    type = a(t+4); % data type
    data = a(t+[5:len]); % data
%     fprintf(2,[dec2hex(token,2) dec2hex(type,2)])
%     fprintf(2,dec2hex(data))
%     fprintf(2,'\n')
    t = t+len;
    len = len-4;
    d = [];
    switch type
    case 0 % no data
    case 2 % two-byte signed integer
        for x = 1:2:len
            b = sum((2.^[8 0]').*data([x x+1]));
            sgn = bitget(data(x),8);
            if sgn
                b = bitcmp(b,16)+1;
            end
            d((x+1)/2) = (1-2*sgn)*b;                
        end
    case 3 % four-byte signed integer
        for x = 1:4:len
            b = sum((2.^[24:-8:0]').*data(x+[0:3]));
            sgn = bitget(data(x),8);
            if sgn
                b = bitcmp(b,32)+1;
            end
            d((x+3)/4) = (1-2*sgn)*b;                
        end
    case 5 % eight-byte real
        for x = 1:8:len
            sgn = bitget(data(x),8);
            expn = data(x) - 128*sgn -64;
            mant = sum((2.^[-8:-8:-56]').*data(x+[1:7]));
            d((x+7)/8) = (1-2*sgn)*mant*16^expn;
        end
    case 6 % ASCII string
        d = char(data(1:end-(~data(len))))'; % drop pad character if exists
    otherwise
        disp(['Unknown data type ' num2str(type)])
    end
    switch token
    case 0 % header
        lib.version = d;
    case 1 % bgnlib
        lib.libdate = d;
    case 2 % libname
        lib.libname = d;
    case 3 % units
        lib.units = d;
        % everything will be stored in nanometers internally
        conv = d(2)*1e6;
    case 4 % endlib
        endlib = 1;
    case 5 % bgnstr
        lib.strdate = d;
    case 6 % strname
        lib.name = d;
    case 7 % endstr
    case 8 % boundary
        npoly = npoly + 1;
        el = 0;
    case 9 % path
        npath = npath + 1;
        el = 1;
        % default pathtype
        lib.path(npath).pathtype = 0;
    case 13 % layer
        switch el
        case 0
            lib.poly(npoly).layer = d;
        case 1
            lib.path(npath).layer = d;
        end
    case 14 % datatype
        switch el
        case 0
            lib.poly(npoly).datatype = d;
        case 1
            lib.path(npath).datatype = d;
        end
    case 15 % width
        lib.path(npath).width = d*conv;
    case 16 % xy
        d = d*conv;
        switch el
        case 0
            lib.poly(npoly).x = d(1:2:end);
            lib.poly(npoly).y = d(2:2:end);
            lib.poly(npoly).n = length(d)/2;
        case 1
            % path centers
            lib.path(npath).xc = d(1:2:end);
            lib.path(npath).yc = d(2:2:end);
            % path boundaries
            [lib.path(npath).x,lib.path(npath).y] = pathbdry(lib.path(npath));
            lib.path(npath).n = length(lib.path(npath).x);
            lib.path(npath).nc = length(lib.path(npath).xc);
        end
    case 17 % endel
    case 33 % pathtype
        lib.path(npath).pathtype = d;
    otherwise
        disp(['Unknown token ' num2str(token)])
    end
end
fprintf(2,'\n')