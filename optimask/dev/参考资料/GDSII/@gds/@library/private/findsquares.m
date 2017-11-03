function [sqdum,sqX,sqY] = findsquares(str,strx,sqdum,bdry,asq,dsq,dfeat,update)

% FINDSU
% create list of squares
gsq = asq+dsq;
nsqx = floor((bdry(2)-bdry(1))/gsq);
nsqy = floor((bdry(4)-bdry(3))/gsq);
sqx = bdry(1) + asq/2 + [0:nsqx-1]*gsq;
sqy = bdry(3) + asq/2 + [0:nsqy-1]*gsq;
[sqX,sqY] = meshgrid(sqx,sqy);
sqX = sqX(:)';
sqY = sqY(:)';
% drop rectangles already covered by dummy features
[dx,dy] = find(sqdum);
sqdum = sparse([2*dx; 2*dx-1; 2*dx; 2*dx-1],[2*dy; 2*dy; 2*dy-1; 2*dy-1],ones(4*length(dx),1),nsqx,nsqy);
sqX(find(sqdum')) = [];
sqY(find(sqdum')) = [];
% drop squares inside and near exclusion zones (all should be rectangles)
D = asq/2+dfeat;
% fprintf('Avoiding rectangles... ')
for x = 1:length(strx)
    u = x/length(strx)*10;
    if x == 1 | floor(x/length(strx)*10)-floor((x-1)/length(strx)*10) 
%         fprintf(2,'%d%% ',floor(x/length(strx)*10)*10)
    end
    qx = strx(x).x;
    qy = strx(x).y;
    sqcl = find(sqX>min(qx)-D & sqX<max(qx)+D & sqY>min(qy)-D & sqY<max(qy)+D);
    sqX(sqcl) = [];
	sqY(sqcl) = [];
end
% fprintf('\n')

% drop squares that are crossed by bounding lines of non-rectangular features
% fprintf('Avoiding other polygons... ')
x = 0;
while ~isempty(sqX) & x < length(str)
    x = x+1;
    u = x/length(str)*10;
    if x==1 | floor(x/length(str)*10)-floor((x-1)/length(str)*10) 
%         fprintf(2,'%d%% ',floor(x/length(str)*10)*10)
    end
    qx = str(x).x;
    qy = str(x).y;
    % find set of squares enclosing structure
    sqencl = find(sqX>min(qx)-D & sqX<max(qx)+D & sqY>min(qy)-D & sqY<max(qy)+D);
    sqXe = sqX(sqencl);
    sqYe = sqY(sqencl);
    for y = 1:length(qx)-1
        x1 = qx(y);
        x2 = qx(y+1);
        y1 = qy(y);
        y2 = qy(y+1);
        % find rectangular set of squares enclosing line segment
        sqcl = find(sqXe>min(x1,x2)-D & sqXe<max(x1,x2)+D & sqYe>min(y1,y2)-D & sqYe<max(y1,y2)+D);
        if ~isempty(sqcl)
            dx = x2-x1;
            dy = y2-y1;
            if dx | dy % not a repeated vertex
                len = dx^2+dy^2;
                sqX0 = sqXe(sqcl);
                sqY0 = sqYe(sqcl);
                % distances to line segment ends
                dist = [sqrt((x1-sqX0).^2 +  (y1-sqY0).^2);
                        sqrt((x2-sqX0).^2 +  (y2-sqY0).^2)];
                % coordinates of perpendicular drop point onto line segment
                x0 = (dx*(sqX0*dx+sqY0*dy)-(x2*y1-x1*y2)*dy)/len;
                y0 = (dy*(sqX0*dx+sqY0*dy)+(x2*y1-x1*y2)*dx)/len;
                % check whether drop point is inside segment
                if ~dx
                    u = (y0-y1).*(y2-y0)>=0;
                else
                    if ~dy
                        u = (x0-x1).*(x2-x0)>=0;
                    else
                        u = (x0-x1).*(x2-x0)>=0 & (y0-y1).*(y2-y0)>=0;
                    end
                end
                u(find(~u)) = NaN;
                dist(3,:) = u.*(sqrt((x0-sqX0).^2 +  (y0-sqY0).^2));
                % For now, we assume a circular exclusion area around square center
                sqtooclose = find(min(dist) <= asq/sqrt(2)+dfeat);
				sqXe(sqcl(sqtooclose)) = [];
				sqYe(sqcl(sqtooclose)) = [];
            end
        end
	end
    sqX(sqencl)=[];
    sqY(sqencl)=[];
    sqX = [sqX sqXe];
    sqY = [sqY sqYe];
end
% 	fprintf('\n')
if length(sqX) & (update | ~dsq)
%         disp('Updating dummy features...')
    indx = round((sqX-bdry(1)-asq/2)/gsq)+1;
    indy = round((sqY-bdry(3)-asq/2)/gsq)+1;
    sqdum(sub2ind([nsqx nsqy],indx,indy)) = 1;
% 		for x = 1:length(sqX)
%             sqdum(find(sqX(x)==sqx),find(sqY(x)==sqy)) = 1;
% 		end
end
