function p = ptest(str,i)

% Tests the contents of polygons inside gds data struct in AWG.

if nargin > 1
    p = display(str.gds,1,i);
else
    p = display(str.gds,1);
end
