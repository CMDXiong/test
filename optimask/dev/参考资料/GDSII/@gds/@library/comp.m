function [m1,m2] = comp(lib1,lib2)

% [D1,D2] = COMP(LIB1,LIB2) compares two libraries. D1 contains structures that are in
% LIB1 but not in LIB2 and D2 contains structures that are in LIB2 but not in LIB1. 

% Attila Mekis, May 8, 2003

if lib1.units ~= lib2.units
    disp('Units are different.')
    return
end
units = lib1.units;
name1 = lib1.name;
name2 = lib2.name;
s1 = lib1.poly;
s2 = lib2.poly;
p1 = lib1.path;
p2 = lib2.path;
clear lib*; % free up memory

x = 1;
while x <= length(s1)
    y = 1;
    while y <= length(s2) & (s1(x).n~=s2(y).n | s1(x).x~=s2(y).x | s1(x).y~=s2(y).y)
        y = y+1;
    end
    if y <= length(s2) % if matching structure is found
        s2(y) = [];
        s1(x) = [];
    else
        x = x+1;
	end
end
x = 1;
while x <= length(p1)
    y = 1;
    while y <= length(p2) & (p1(x).n~=p2(y).n | p1(x).x~=p2(y).x | p1(x).y~=p2(y).y)
        y = y+1;
    end
    if y <= length(p2) % if matching structure is found
        p2(y) = [];
        p1(x) = [];
    else
        x = x+1;
	end
end

if ~length(s1) & ~ length(s2) & ~length(p1) & ~ length(p2)
    disp('Libraries are identical.')
end
m1 = library(s1,[name1 ' - ' name2],units,p1);
m2 = library(s2,[name2 ' - ' name1],units,p2);
