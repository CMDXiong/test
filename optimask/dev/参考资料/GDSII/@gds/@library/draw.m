function draw(lib,layer,cl)

% DRAW(LIB) draws the structure library LIB.
%
% DRAW(LIB,L) draws only layer L.

red = [1 0 0];
green = [0 1 0];
blue = [0 0 1];
yellow = [1 1 0];
cyan = [0 1 1];
magenta = [1 0 1];
black = [0 0 0];
white = [1 1 1];
orange = [1 0.5 0];
col = [zeros(5,3); 
    blue; % 6: pcore
    zeros(3,3);
    cyan; % 10: left mirror
    magenta; % 11: right mirror
    zeros(4,3);
    orange; % 16: metal
    zeros(1,3);
    white; % 18: trench
    yellow; % 19: solder
    zeros(10,3);
    red]; % 30: dummy exclusion
    
colordef black
newplot
str = lib.poly;
if nargin > 1
	str = str(find([str.layer] == layer));
end
for x = 1:length(str)
    q = str(x);
    lay = q.layer;
    h = line(q.x,q.y);
    if nargin < 3
        c = col(lay,:);
    else
        c = cl;
    end
    set(h,'Color',c)
end
str = lib.path;
if ~isempty(str)
	if nargin > 1
        str = str(find([str.layer] == layer));
	end
	for x = 1:length(str)
        q = str(x);
        lay = q.layer;
        h = line(q.x,q.y);
        if nargin < 3
            c = col(lay,:);
        else
            c = cl;
        end
        set(h,'Color',c)
	end
end
axis equal
