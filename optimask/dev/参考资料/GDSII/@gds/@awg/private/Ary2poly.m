function poly = ary2poly(layername,array2,array1)

% This function returns a polygon composed by two curves, 
% array1 and array2, forming a closed pattern.
% The function takes as inputs the name of the layer and two 2-D 
% arrays containing the vertics of the pattern.
%
% Arguments:
% layername    Type: string   name of layer to draw on
% array1       Type: 2 x N array, coordinates of vertics on one side of the pattern
% array2       Type: 2 x M array, coordinates of vertics on the other side of the pattern
%              N and M many not be equal.

poly = [];
n = length(array1(1,:));
m = length(array2(1,:));
big_vert = 75;   %half vertex number in one ploygon.
%Note: Maximum number of vertics of a polygon allowed in ICED is 199. Here we use
%      150 as a unit to plot a polygon.

if n > m     %Exchange two array, number of vertics of array2 should be larger
  mtemp = m;
  m = n;
  n = mtemp;
  array_temp = array2;
  array2 = array1;
  array1 = array_temp;
end;

realnumber = (m/big_vert);
polynumber = ceil(realnumber);   
if ( rem(m,big_vert) == 1 )
  polynumber = polynumber - 1;   %One more vertic is fine, won't exceed 199
end;
%polynumber is the number of polygons required to plot the whole pattern.
if n <= polynumber
   ERROR('Increase number of points for one curve, not enough vertics to plot polygons!');
end;

small_vert = n/realnumber;
%total_vert = ceil(small_vert)+big_vert;

k1=0;
k2=0;

% draw the first polynumber-1 polygons, if polynumber<2, this for-loop will be passed
for i = 0:polynumber-2
   k1 = round(i*small_vert);
   k2 = round((i+1)*small_vert);
   if k2 >= n   %this will happen when n is much smaller than m
      k2 = n-1;
   end;
   
   outarray(:,1:k2-k1+1) = array1(:,1+k1:1+k2);
   outarray(:,k2-k1+2:k2-k1+big_vert+2) = fliplr(array2(:,1+i*big_vert:1+(i+1)*big_vert));  
   poly = add2poly(poly,layername,outarray);
   clear outarray;
   
end;

% draw the last polygon

outarray(:,1:n-k2) = array1(:,1+k2:n);
outarray(:,n-k2+1:n-k2+m-(polynumber-1)*big_vert) = fliplr(array2(:,1+(polynumber-1)*big_vert:m));
poly = add2poly(poly,layername,outarray);


