function s = awgsave(str)

% AWGSAVE saves str into mat file with an associated file name
% % SF, January 31, 2006

fid = fopen([str.name '.mat']);
if fid == -1
    save(str.name,'str');
    s = str.name;
    disp(['AWG design saved in ' s '.mat file']);
else
    fclose(fid);
    disp(['Filename ' str.name '.mat exist.']);
    f = input('Overwrite? (Y/N):','s');
    if isempty(f)
        f = 'Y';
    end
    if f == 'N' | f == 'n'
        s = 'none';
        disp(['AWG design is not saved']);
    else
        save(str.name,'str');
        s = str.name;
        disp(['AWG design saved in ' s '.mat file']);
    end
end

        