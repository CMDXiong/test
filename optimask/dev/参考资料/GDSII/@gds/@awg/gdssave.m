function s = gdssave(str,layer,name)

% GDSSAVE saves all or part of str.gds data into GDSII file with an associated file name
% % SF, May 2 2007

if nargin < 3
    name = str.name
end

if nargin < 2
    
    fid = fopen([name '.gds']);
    if fid == -1
        writegds(str.gds,name);
        s = name;
        disp(['AWG gds data saved in ' s '.gds file']);
    else
        fclose(fid);
        disp(['Filename ' name '.gds exists.']);
        f = input('Overwrite? (Y/N):','s');
        if isempty(f)
            f = 'Y';
        end
        if f == 'N' | f == 'n'
            s = 'none';
            disp(['AWG design is not saved']);
        else
            writegds(str.gds,name);
            s = name;
            disp(['AWG gds data saved in ' s '.gds file']);
        end
    end
else
    fid = fopen([name '.gds']);
    if fid == -1
        writegdslayer(str.gds,layer,name);
        s = name;
        disp(['AWG gds data saved in ' s '.gds file']);
    else
        fclose(fid);
        disp(['Filename ' name '.gds exists.']);
        f = input('Overwrite? (Y/N):','s');
        if isempty(f)
            f = 'Y';
        end
        if f == 'N' | f == 'n'
            s = 'none';
            disp(['AWG design is not saved']);
        else
            writegdslayer(str.gds,layer,name);
            s = name;
            disp(['AWG gds data saved in ' s '.gds file']);
        end
    end
end

        