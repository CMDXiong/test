function [d,dfull,sqdum] = dummy(s,bdry,asq,dsq,dfeat,n,flag)

% D = DUMMY(L,B,A,S,D,N,FLAG)
% 
% Returns a structure library containing square dummy features for the
% input structure library L. The dummy features are placed only within the 
% boundary, which is defined by B = [X1 X2 Y1 Y2] for a rectangular area.
%
% The squares have sides of length A and edge-to-edge spacing S. 
% A minimum distance D is kept between the squares and the lines enclosing 
% the features in library L. 
% 
% The boundary area will be tiled by NX*NY rectangular regions, as defined 
% by the input variable  N = [NX NY].
%
% Note that in the current version, the inside of the mask features will be 
% filled in unless the enclosed area is too small to fit in a square or the
% feature is rectangular with edges parallel to the boundaries.
% 
% Dummy structures are not filled in inside exclusion zones (layer number 30).
% The exclusion zones are also assumed to be rectangles positioned with sides
% parallel to the boundaries.
% 
% FLAG is an optional scalar if D is non-zero:
% FLAG = 0 (or missing): uses squares of all the same size
% FLAG = 1: uses squares of different sizes
% 
% Example:
%       lib = library('TM6_1_testmirrors_nodummy.gds')
% 		dum = dummy(lib,[0 20000 0 20000],10,0,30,[8 8])
% 		draw(lib+dum)
% 		writegds(lib+dum,'TM6_1_testmirrors')

% Attila Mekis, May 1, 2003

if nargin < 6
    n = [1 1];
end
nx = n(1);
ny = n(2);
% size of squares
if nargin > 6 & flag
    maxsize = min(min(diff(bdry(1:2))/nx,diff(bdry(3:4))/ny),100);
    nstep = floor(log(maxsize/(asq+dsq))/log(2))+1;
else
    flag = 0;
    nstep = 1;
end
if ~dfeat
    flag = 1;
end
sqsize = (2.^[nstep-1:-1:0]*(asq+dsq)-dsq);
dx = diff(bdry(1:2))/nx;
dy = diff(bdry(3:4))/ny;

% dummy feature layer number
layer = 6;
% dummy feature exclusion zone layer number
layerx = 30;

str = s.poly(find([s.poly.layer] == layer));
strx = s.poly(find([s.poly.layer] == layerx));
% strx(16).x([2 3]) = 16150;
% Calculate area covered by features
% bdarea = diff(bdry(1:2))*diff(bdry(3:4));
% for x = 1:length(str)
%     q = str(x);
%     % Count only polygons that are entirely within boundaries
%     if q.x>=bdry(1) & q.x<=bdry(2) & q.y>=bdry(3) & q.y<=bdry(4)
%         strarea(x) = abs(sum(q.x(1:end-1).*q.y(2:end)-q.x(2:end).*q.y(1:end-1)))/2;
%     end
% end
% strarea = sum(strarea);
% darea = 0; % area covered by dummy features

disp('Finding rectangles...')
% find rectangles
par = [];
rect = find([str.n] == 5);
for x = 1:length(rect)
    q = str(rect(x));
    par(x) = length(unique(q.x))==2 & length(unique(q.y))==2;     
end
rect = rect(find(par));
strrect = str(rect);
strnorect = str;
strnorect(rect) = [];
poly = strnorect;
rect = [strx strrect];

disp('Assigning regions...')
% drop polygons with no vertices or edges inside boudaries
polyout = zeros(length(poly));
for x = 1:length(poly)
    q = poly(x);
    if q.x<=bdry(1) | q.x>=bdry(2) | q.y<=bdry(3) | q.y>=bdry(4)
        polyout(x) = 1;
    end
    if ~sum(q.x>bdry(1) & q.x<bdry(2) & q.y>bdry(3) & q.y<bdry(4)) % no vertex inside
        polyout(x) = 1;
        for y = 1:length(q.x)-1
            lx = q.x(y+[0 1]);
            ly = q.y(y+[0 1]);
            if linecross(lx,ly,bdry([1 2]),bdry([3 3])) | linecross(lx,ly,bdry([1 2]),bdry([4 4])) | linecross(lx,ly,bdry([1 1]),bdry([3 4])) | linecross(lx,ly,bdry([2 2]),bdry([3 4]))
                polyout(x) = 0;
            end
        end
        if q.x<=bdry(1) | q.x>=bdry(2) | q.y<=bdry(3) | q.y>=bdry(4)
            polyout(x) = 1;
        end
    end
end
rectout = zeros(length(rect));
for x = 1:length(rect)
    q = rect(x);
    if q.x<=bdry(1) | q.x>=bdry(2) | q.y<=bdry(3) | q.y>=bdry(4)
        rectout(x) = 1;
    end
    if ~sum(q.x>bdry(1) & q.x<bdry(2) & q.y>bdry(3) & q.y<bdry(4)) % no vertex inside
        rectout(x) = 1;
        for y = 1:length(q.x)-1
            lx = q.x(y+[0 1]);
            ly = q.y(y+[0 1]);
            if linecross(lx,ly,bdry([1 2]),bdry([3 3])) | linecross(lx,ly,bdry([1 2]),bdry([4 4])) | linecross(lx,ly,bdry([1 1]),bdry([3 4])) | linecross(lx,ly,bdry([2 2]),bdry([3 4]))
                rectout(x) = 0;
            end
        end
        if q.x<=bdry(1) | q.x>=bdry(2) | q.y<=bdry(3) | q.y>=bdry(4)
            rectout(x) = 1;
        end
    end
end
poly(find(polyout)) = [];
rect(find(rectout)) = [];

% find polygons that may intersect a region yet have no vertex there
for x = 1:length(poly)
    q = poly(x);
    bigpoly(x) = max(abs(diff(q.x))) > dx | max(abs(diff(q.y))) > dy;
end
for x = 1:length(rect)
    q = rect(x);
    bigrect(x) = max(abs(diff(q.x))) > dx | max(abs(diff(q.y))) > dy;
end
% find polygons belonging to each region
D = max(sqsize)*sqrt(2) + dfeat;
if nx > 1 | ny > 1
	for bx = 1:nx
        for by = 1:ny
			b = [bdry(1)+dx*(bx+[-1 0]) bdry(3)+dy*(by+[-1 0])];
			bdryc{bx,by} = b;
            for x = 1:length(poly)
                q = poly(x);
                polyin(bx,by,x) = max(q.x>=b(1)-D & q.x<=b(2)+D & q.y>=b(3)-D & q.y<=b(4)+D);
            end
            for x = 1:length(rect)
                q = rect(x);
                rectin(bx,by,x) = max(q.x>=b(1)-D & q.x<=b(2)+D & q.y>=b(3)-D & q.y<=b(4)+D);
			end
        end
	end
	% include polygons that intersect a region yet have no vertex in that region
	for x = 1:length(bigpoly)
        [ux,uy] = find(polyin(:,:,x));
        polyin(min(ux):max(ux),min(uy):max(uy),x) = 1;
	end
	for x = 1:length(bigrect)
        [ux,uy] = find(rectin(:,:,x));
        rectin(min(ux):max(ux),min(uy):max(uy),x) = 1;
	end
else
    polyin = ones(1,1,length(poly));
    rectin = ones(1,1,length(rect));
    bdryc{1,1} = bdry;
end

nsqx = dx/sqsize(end);
nsqy = dy/sqsize(end);
disp(['Creating dummy features...'])
dum = [];
for bx = 1:nx
    for by = 1:ny
		sqdum = [];
        done = (by+(bx-1)*ny)/nx/ny;
        if floor(done*10) ~= floor((done-1/nx/ny)*10)
            fprintf(2,'%d%% ',floor(done*10)*10);
        end
        for sz = 1:nstep
            update = (sz < nstep);
            asq = sqsize(sz);
            p = poly(find(polyin(bx,by,:)));
            r = rect(find(rectin(bx,by,:)));
            if dsq
                [sqdum,dumX{sz},dumY{sz}] = findsquares(p,r,sqdum,bdryc{bx,by},asq,dsq,dfeat,update);
            else
                sqdum = findsquares(p,r,sqdum,bdryc{bx,by},asq,dsq,dfeat,update);
            end
%             darea = darea + asq^2*length(find(sqdum));
		end
%         dummy([1:nsqx]+(bx-1)*nsqx,[1:nsqy]+(by-1)*nsqy) = sqdum;
		if ~dsq % merge induvidual squares to make polygons
            [vx,vy] = findpoly(sqdum);
            n = length(vx);
            if n
                nd = length(dum);
                [dum(nd+[1:n]).layer] = deal(layer);
                [dum(nd+[1:n]).datatype] = deal(0);
				for x = 1:n
                    dum(nd+x).x = bdry(1) + asq*((bx-1)*nsqx + vx{x});
                    dum(nd+x).y = bdry(3) + asq*((by-1)*nsqy + vy{x});
                    dum(nd+x).n = length(vx{x});
				end
            end
		end
    end
end
fprintf(2,'\n')

if dsq % create squares
	disp('Creating dummy squares...')
    dum = [];
	for x = 1:length(sqsize)
        asq = sqsize(sz);
        n = length(dumX{sz});
        dum0 = [];
        if n
            [dum0(1:n).layer] = deal(layer);
            [dum0(1:n).datatype] = deal(0);
			for x = 1:n
                dum0(x).x = dumX{sz}(x) + asq/2*[-1 1 1 -1 -1];
                dum0(x).y = dumY{sz}(x) + asq/2*[-1 -1 1 1 -1];
                dum0(x).n = 5;
			end
		end
        dum = [dum dum0];
    end
end

ndum = length(dum);
disp([num2str(ndum) ' dummy features were created.'])
% disp(['Dummy feature coverage is ' num2str(darea/bdarea)])
% disp(['Pcore feature coverage is ' num2str(strarea/bdarea)])
% disp(['Total coverage is ' num2str((strarea+darea)/bdarea)])

d = library(dum,[s.name '_dummy'],s.units);
dfull = library([s.poly dum],[s.name '_pcoredummy'],s.units,[s.path]);
