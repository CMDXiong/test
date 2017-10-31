function [vertexX,vertexY] = findpoly(d,flag)

% FINDPOLY(D) returns a cell array of polygon vertices defined by the 
% square distribution D.

% Attila Mekis, June 17, 2003

if nargin < 2
    flag = 0;
end
sz = size(d);
d0 = zeros(sz+2);
d0(2:end-1,2:end-1) = d;
% find whether squares adjacent to vertex p(x,y) is filled
% identification # for squares: 4 squares 1 thru 4 clockwise; #1 is top right
p = zeros(sz(1)+1,sz(2)+1,4);
p(:,:,1) = d0(2:end,2:end);
p(:,:,2) = d0(2:end,1:end-1);
p(:,:,3) = d0(1:end-1,1:end-1);
p(:,:,4) = d0(1:end-1,2:end);
nbrs = sum(p,3);
% spy(d)
[vx,vy] = find(mod(nbrs,2)); % vertices of polygons
[vx2u,vy2u] = find(nbrs==2 & p(:,:,1)==p(:,:,3)); % polygons touching at a common vertex
vx2d = vx2u; % u is for polygons above vertex, d is for below vertex
vy2d = vy2u;

% find polygons
npoly = 0;
while length(vx) | length(vx2u)
    npoly = npoly + 1;
    % select first vertex
    if length(vx)
        cx0 = vx(1);
        cy0 = vy(1);
    else % only shared vertices are left
        cx0 = vx2u(1);
        cy0 = vy2u(1);
    end
    verx{npoly} = cx0-1;
    very{npoly} = cy0-1;
    cx = cx0;
    cy = cy0;
    % start walking along polygon edge; 4 directions 1 thru 4 clockwise; #1 is up
    switch nbrs(cx,cy)
    case 1
        dir = find(p(cx,cy,:)==1);
    case 2
        % if first vertex is shared, polygon is chosen to be above vertex
        dir = 2-p(cx,cy,1); 
    case 3
        dir = find(p(cx,cy,:)==0)+1;
    end
    dir = dir - 4*(dir>4);
    % walk along edge until we come back to starting vertex
    nver = 1;
    while nver==1 | ~(cx0 == cx & cy0 == cy)
        nver = nver + 1;
%         [cx cy dir ckws]
        % find next corner
        vx0 = [vx; vx2u; vx2d];
		vy0 = [vy; vy2u; vy2d];
		switch dir
		case 1
            y = find(vx0 == cx & vy0 > cy);
            [z,iz] = min(vy0(y));
		case 2
            y = find(vx0 > cx & vy0 == cy);
            [z,iz] = min(vx0(y));
		case 3
            y = find(vx0 == cx & vy0 < cy);
            [z,iz] = max(vy0(y));
		case 4
            y = find(vx0 < cx & vy0 == cy);
            [z,iz] = max(vx0(y));
		end
		y = y(iz);
        if isempty(y)
            'error'
        end
        cx = vx0(y);
		cy = vy0(y);
        if y <= length(vx) % single vertex
            vx(y) = [];
            vy(y) = [];
        else % shared vertex: check if polygon is above or below
%             [dir p(cx,cy,1)] should be one of [1 0; 2 1; 3 0; 4 1]
            if dir > 2 % above
                y = find(cx==vx2u & cy==vy2u);
                vx2u(y) = [];
                vy2u(y) = [];
            else % below
                y = find(cx==vx2d & cy==vy2d);
                vx2d(y) = [];
                vy2d(y) = [];
            end
        end
        verx{npoly}(nver) = cx-1;
        very{npoly}(nver) = cy-1;
        dir0 = dir;
        dir = dir + 1 + 2*(nbrs(cx,cy)==3);
        dir = dir - 4*(dir>4);
    end
%     [cornerx{npoly}' cornery{npoly}']
end

% find which polygons contain other polygons
contain = zeros(npoly);
for x = 1:npoly
    for y = 1:npoly
        if x~=y 
            contain(x,y) = polyinside(verx{y}(1),very{y}(2),verx{x},very{x});
        end
    end      
end
% calculate number of polygons enclosing each polygon
nenclosed = sum(contain,1);
% calculate number of polygons contained in each polygon
ncontain = sum(contain,2);
% decide which line is inner/outer edge
inner = find(mod(nenclosed,2));
outer = find(~mod(nenclosed,2));
encl = [];
% find corresponding outer polygons for each inner polygon
for x = 1:length(inner)
    % outer polygons enclosing this inner one
    op = find(contain(:,inner(x)));
    % find innermost outer polygon
    [tmp,minop] = min(ncontain(op));
    encl(x) = op(minop);
    % find closest vertex of inner polygon to enclosing outer polygon
    [x0,x1] = meshgrid(verx{inner(x)},verx{encl(x)});
    [y0,y1] = meshgrid(very{inner(x)},very{encl(x)});
    dist = (x0-x1).^2+(y0-y1).^2;
    [mind,tmp] = min(dist);
    [mind,breakin(x)] = min(mind);
    breakout(x) = tmp(breakin(x));
	% closest vertices are:
	%     [verx{inner(x)}(breakin(x)) very{inner(x)}(breakin(x))]
	%     [verx{encl(x)}(breakout(x)) very{encl(x)}(breakout(x))]
end
% outer polygons that are 'holey'
holey = unique(encl);
% outer polygons that are not 'holey'
whole = setdiff(outer,holey);
if ~isempty(whole)
    [vertexX{1:length(whole)}] = deal(verx{whole});
    [vertexY{1:length(whole)}] = deal(very{whole});
    [polytype{1:length(whole)}] = deal(0);
else
    vertexX = {};
    vertexY = {};
    polytype = {};
end
% combine inner and outer polygons
for x = 1:length(holey)
    % outer polygon
    out = holey(x);
    vxo = verx{out};
    vyo = very{out};
    % inner polygons inside this outer polygon
    u = find(out==encl);
    [tmp,ind] = sort(breakout(u));
    u = u(ind);
    in = inner(u);
    brin = breakin(u);
	brout = breakout(u);
    % sides of all polygons (outer one + inner ones)
    sidesx = [vxo(1:end-1)' vxo(2:end)'];
    sidesy = [vyo(1:end-1)' vyo(2:end)'];
    for y = 1:length(u)
        sidesx = [sidesx; verx{in(y)}(1:end-1)' verx{in(y)}(2:end)'];
        sidesy = [sidesy; very{in(y)}(1:end-1)' very{in(y)}(2:end)'];
    end
    vxO = [vxo vxo];
    vyO = [vyo vyo];
    nout = length(vxo);
    for y = 1:length(in)
        % breakpoints for inner/outer polygons
    bin = brin(y);
        bout = brout(y);
        vxi = verx{in(y)};
        vyi = very{in(y)};
		nin = length(vxi);
		% find line that does not self-intersect any polygons
        sx = 1;
        cin = 0;
		while cin <= nin & sx
            bin = brin(y)+cin;
            bin = bin - nin*(bin>nin);
            cout = 0;
			while cout <= nout & sx
                bout = brout(y)+cout;
                bout = bout - nout*(bout>nout);
	            x0 = vxi(bin);
	            y0 = vyi(bin);
                x1 = vxo(bout);
                y1 = vyo(bout);
                sx = sum(linecross([x0 x1],[y0 y1],sidesx,sidesy));
                cout = cout+1;
            end
            cin = cin+1;
        end
        if cin == nin+1
            disp('Cannot create polygons.')
        end
        brin(y) = bin;
        brout(y) = bout;
        % add new connecting line to set of polygon sides
        sidesx = [sidesx; x0 x1];
        sidesy = [sidesy; y0 y1];
%         disp([x0 y0 x1 y1])
%         line([x0 x1],[y0 y1]);pause
    end
    % reorder inner polygons according to breakpoints in outer polygon
    [tmp,ind] = sort(brout);
    in = in(ind);
    brin = brin(ind);
	brout = brout(ind);
    % define new polygon
    vx = [];
    vy = [];
	brout = [brout brout(1)+length(vxo)];
    for y = 1:length(in)
        % breakpoints for inner/outer polygons
        bin = brin(y);
        bout = brout(y);
        vxi = verx{in(y)};
        vyi = very{in(y)};
        vxI = [vxi vxi];
        vyI = [vyi vyi];
        vx = [vx vxI([bin:bin+length(vxi)]) vxO([bout:brout(y+1)])];
        vy = [vy vyI([bin:bin+length(vyi)]) vyO([bout:brout(y+1)])];
    end
	vertexX{end+1} = vx;
	vertexY{end+1} = vy;
    polytype{end+1} = 1;
end
% disp([num2str(length(holey)) ' holey polygons created.'])

if flag
	for x = 1:length(vertexY)
        plot(vertexX{x},vertexY{x});
        hold on
	end
    hold off
    axis equal
end

% for GDS format, break up polygons that are larger than 200 vertices
x = 0;
while x < length(vertexY)
    x = x+1;
    n = length(vertexX{x});
    if n > 200
        X = vertexX{x};
        Y = vertexY{x};
        [found,vx1,vy1,vx2,vy2] = breakup_polygon(X,Y,200);
        if found
            vertexX{x} = vx1;
            vertexY{x} = vy1;
            vertexX{end+1} = vx2;
            vertexY{end+1} = vy2;
        else 
            'Error: could not break up large polygon.'
        end
	end
end

