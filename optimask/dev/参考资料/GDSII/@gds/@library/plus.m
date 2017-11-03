function p = plus(lib1,lib2)

% PLUS(LIB1,LIB2) returns a library containing polygons both in LIB1 and LIB2.

p = library([lib1.poly lib2.poly],lib1.name,lib1.units,[lib1.path lib2.path]);
