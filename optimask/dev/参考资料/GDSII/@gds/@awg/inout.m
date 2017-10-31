function str = inout(str,layer)

% INOUT generates input and output waveguides for AWG structure
% SF, February 2 2006

if nargin < 2
    layer = 6;  %default layer
end

p = str.param;
c = str.calc;
g = str.gds;

r_bend = 1500;   %bending radius = 1500 um
junction_offset = 0.2;   % um

% added for slab waveguide boundary
theta_slab = 14*p.pi180;
theta_io   = 10*p.pi180;

theta_i_c = input('The input angle of center wavelength = [0.0 degree] ');
if isempty(theta_i_c)
   theta_i_c = 0.0;
end;

theta_o_c = input('The output angle of center wavelength = [0.0 degree] ');
if isempty(theta_o_c)
   theta_o_c = 0.0;
end;

vernier = input('vernier ratio=[0.4 (of channel spacing, 0.8nm)]  ');
if isempty(vernier)
   vernier = 0.4 ;
end;
   wl_err = vernier*0.8/1000 ;      %um

taper_choice = input('Linear taper or Parabolic taper? [L/P, Enter=P]  ', 's');
if isempty(taper_choice)
   taper_choice='P';
end;

strip = input('Strip input and output waveguides? [Y/N, Enter=Y]  ', 's');
if isempty(strip)
   strip='Y';
end;
if strip =='n'
    strip = 'N';
end

tap_width = input('taper width (not including CD loss) = [waveguide width (Enter=9)]  ');
if isempty(tap_width)
   tap_width = 9 + p.cdloss ;     %um
else
   tap_width = tap_width + p.cdloss ;   % etching adjustment
end;

if taper_choice ~= 'L'          % draw parabolic tapers
   lambda_g = p.lambda0/p.inchannel; 
   pb_coef = (tap_width^2 - p.width^2)/(2*lambda_g*p.tap_len);
   tap_coef = 2*pb_coef*lambda_g;   %positive value
   dxy = 2;  %um
end;

st_len = input('straight WG length between bending WG and tapering WG=[10 um]  ');
if isempty(st_len)
   st_len = 10;
end;

dtheta_deg = input('Input mask angular resolution =[0.08 degree]  ');
if isempty(dtheta_deg)
   dtheta_deg = 0.08;
end;
dtheta = dtheta_deg*p.pi180;   %in radian

grt_cent(1) = p.roland*cos(p.theta0);
grt_cent(2) = p.roland*sin(p.theta0);


%%%%%%%%%%%%%%%%%%%%%  Calculate output angles first% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%  Vernier angles will be correted later %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

out_begin_no = input('Output waveguides starting number (including vernier channels) [default is -17] = ');
if isempty(out_begin_no)
   out_begin_no = -17;
end;

out_end_no = input('Output waveguides ending number (including vernier channels) [default is 16] = ');
if isempty(out_end_no)
   out_end_no = 16;
end;

out_ch_no = out_begin_no:out_end_no;
out_no = length(out_ch_no);

out_freq_ary  = p.freq0 - p.channel.*out_ch_no ;   %GHz
out_wl_ary = (p.c_light/1000.0) ./ out_freq_ary ;   %um   
out_dwl_ary = out_wl_ary-p.lambda0 ;   %um


sin_out_angle_ary = (p.Nc_nc*p.order.*out_dwl_ary)./(p.inslab*p.pitch) - sin(theta_i_c*p.pi180);   % this is sin(out_angle_ary)
out_angle_ary = asin(sin_out_angle_ary);   % this is output angle array


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Calculate input waveguide angles %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

begin_no = input('Input waveguides starting number (0 for non-vernier) = [-1]  ');
if isempty(begin_no)
   in_begin_no = -1;      % enter 0 for non-vernier input waveguide
else
   in_begin_no = begin_no;
end;

end_no = input('Input waveguides ending number (0 for non-vernier) = [+1]  ');
if isempty(end_no)
   in_end_no = 1;         % enter 0 for non-vernier input waveguide
else
   in_end_no = end_no;
end;

in_ch_no = in_begin_no:in_end_no;
in_no = length(in_ch_no);


%%%% Shifted center wavelengths, from positive shift to negative shift
center_wl = p.lambda0 - in_ch_no.*wl_err ;      %um

%%%% shifted Ncc
Ncc = p.inchannel.*center_wl./p.lambda0 ;

%%%% Input at +x channel, output at -x channel.  Notice array index starts from 1.
shifted_out_ch = -1*out_begin_no +1 -1.*in_ch_no ;    

sin_in_angle_ary = ( p.order*p.lambda0 - (Ncc+(p.lambda0-center_wl).*p.dNc_dLambda*1000).*p.dL )./(p.inslab*p.pitch)  ...
                   - sin_out_angle_ary(shifted_out_ch) ;


%%%%%%%%%% correct the angles of the output vernier channels
if in_end_no ~= 0     %% correct the positive shifted Ncc
   
   for j = in_end_no:1
      sin_out_angle_ary(out_no+(j-in_end_no)) = 1/(p.inslab*p.pitch)*  ...
         (  p.order*out_wl_ary(out_no+(j-in_end_no)-1) -  ...
            p.dL*( Ncc(1+(in_end_no-j)) +  ...
                 (out_wl_ary(out_no+(j-in_end_no)-1)-center_wl(1+(in_end_no-j)))*p.dNc_dLambda*1000 )  ...
         ) - sin_in_angle_ary(1+(in_end_no-j)) ;
   end;
   
end;   


if in_begin_no ~= 0    %% correct the negative shifted Ncc
   
   for j = in_begin_no:-1
      sin_out_angle_ary(1+(j-in_begin_no)) = 1/(p.inslab*p.pitch)*  ...
         (  p.order*out_wl_ary(1+(j-in_begin_no)+1) -  ...
            p.dL*( Ncc(in_no-(j-in_begin_no)) +  ...
                 (out_wl_ary(1+(j-in_begin_no)+1)-center_wl(in_no-(j-in_begin_no)))*p.dNc_dLambda*1000 )  ...
         ) - sin_in_angle_ary(in_no-(j-in_begin_no)) ;
   end;
   
end;   

in_angle_ary = asin(sin_in_angle_ary);   % this is input angle array
out_angle_ary = asin(sin_out_angle_ary);   % this is output angle array

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Input section  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%Note: angle_ary is the array of all input angles, including vernier waveguides

sin_angle_ary=sin_in_angle_ary;

p_ary = p.roland.*sin_angle_ary;   %p=rsl*sin(angle_ary) 
% line equation (x*sin(tilt)-y*cos(tilt)-p=0), p is distance from (0,0) to line
% for ch_no > 0, p>0, for ch_no<0, p<0 

angle_ary = asin(sin_angle_ary);   %this is the angles of all input waveguides
tilt_angle = angle_ary + p.theta0;


y_1st = -(p.roland+st_len+p.tap_len)*sin(tilt_angle(1)) + grt_cent(2) - r_bend*(1-cos(tilt_angle(1)));
ss = sprintf(' 1st input WG y < %13.3f um',y_1st);
disp(ss);
clear ss;
y_begin = input('1st input WG y position (um) = ');
if isempty(y_begin)
   y_begin = y_1st;
end;

y_space = input('Input WG y spacing = [127 um] ');
if isempty(y_space)
   y_space = 127;
end;

y_posi = y_begin - y_space.*(0:(in_no-1));

% r_bend=abs(x_posi*sin(tilt)-y*cos(tilt)-P)

x_posi = ((y_posi+r_bend).*cos(tilt_angle)+p_ary-r_bend)./sin(tilt_angle);

in_cent(:,1) = x_posi';
in_cent(:,2) = y_posi'+r_bend;


shortest_st = 50;   %um
io_st_x = floor((min(x_posi)-shortest_st)/shortest_st)*shortest_st;

for i = 1:in_no
    
    if strip == 'N' % stripping section
   
   %% if i ~= in_no   %%% Just draw taper for the central input waveguide in this mask design %%%%
    
       %draw the bending i/o array
       in_array2 = arccospn(in_cent(i,:),r_bend-junction_offset,-pi/2,tilt_angle(i),p.width,dtheta);
       g = g + gdsdata( ary2poly(layer,in_array2(1:2,:), in_array2(3:4,:)) );
       g = g + gdsdata( ary2poly(30,in_array2(1:2,:), in_array2(3:4,:)) );  % same on dummy layer
           
       %align the input straight wg start to io_st_x position 
       in_array2 = arccospn(in_cent(i,:),r_bend,-pi/2,tilt_angle(i),p.width,dtheta);
       
       poly(:,1)=in_array2(1:2,1);
       poly(:,2)=in_array2(3:4,1);
       poly(1,3)=io_st_x;
       poly(2,3)=in_array2(4,1);
       poly(1,4)=io_st_x;
       poly(2,4)=in_array2(2,1);
       g = g + gdsdata(add2poly([],layer,poly));
       g = g + gdsdata(add2poly([],30,poly));   % same on dummy layer
       
	
       %putting in straight WG between taper WG and bending WG
       co_ind2 = size(in_array2, 2);
       st_array2(1)=-(p.roland*cos(angle_ary(i))+p.tap_len)*cos(tilt_angle(i))+grt_cent(1)-p.width/2*sin(tilt_angle(i));
       st_array2(2)=-(p.roland*cos(angle_ary(i))+p.tap_len)*sin(tilt_angle(i))+grt_cent(2)+p.width/2*cos(tilt_angle(i));
       st_array2(3)=-(p.roland*cos(angle_ary(i))+p.tap_len)*cos(tilt_angle(i))+grt_cent(1)+p.width/2*sin(tilt_angle(i));
       st_array2(4)=-(p.roland*cos(angle_ary(i))+p.tap_len)*sin(tilt_angle(i))+grt_cent(2)-p.width/2*cos(tilt_angle(i));
	
       poly(:,1)=in_array2(1:2,co_ind2);
       poly(:,2)=in_array2(3:4,co_ind2);
       poly(:,3)=st_array2(3:4)';
       poly(:,4)=st_array2(1:2)';
       g = g + gdsdata(add2poly([],layer,poly));
       g = g + gdsdata(add2poly([],30,poly));   % same on dummy layer
       
     end            % end of stripping section
       
   %% end;
   
   io_taper_end_x(i) = -(p.roland*cos(angle_ary(i))+p.tap_len)*cos(tilt_angle(i))+grt_cent(1);
   io_taper_end_y(i) = -(p.roland*cos(angle_ary(i))+p.tap_len)*sin(tilt_angle(i))+grt_cent(2);
   
   
   %% Draw tapers
   tap_array2(1)=-(p.roland*cos(angle_ary(i)))*cos(tilt_angle(i))+grt_cent(1)-tap_width/2*sin(tilt_angle(i));
   tap_array2(2)=-(p.roland*cos(angle_ary(i)))*sin(tilt_angle(i))+grt_cent(2)+tap_width/2*cos(tilt_angle(i));
   tap_array2(3)=-(p.roland*cos(angle_ary(i)))*cos(tilt_angle(i))+grt_cent(1)+tap_width/2*sin(tilt_angle(i));
   tap_array2(4)=-(p.roland*cos(angle_ary(i)))*sin(tilt_angle(i))+grt_cent(2)-tap_width/2*cos(tilt_angle(i));
   
   
   if ((taper_choice(1)=='L') | (taper_choice(1)=='l'))
      
      %putting in linear tappered WG
      poly(:,1)=tap_array2(1:2)';
      poly(:,2)=tap_array2(3:4)';
      % poly(:,3) & poly(:,4) is the same
      g = g + gdsdata(add2poly([],layer,poly));
      g = g + gdsdata(add2poly([],30,poly));   % same on dummy layer
      
   else
       
      %%%%% draw parabolic tapers %%%%%%
      tap_cent(1)= -(p.roland*cos(angle_ary(i))+p.tap_len)*cos(tilt_angle(i)) + grt_cent(1);
      tap_cent(2)= -(p.roland*cos(angle_ary(i))+p.tap_len)*sin(tilt_angle(i)) + grt_cent(2);
      curve1=io_taper_curve1(tap_cent, tilt_angle(i), dxy, p.tap_len, p.width, tap_coef);   %lower arm of the taper
      curve2=io_taper_curve2(tap_cent, tilt_angle(i), dxy, p.tap_len, p.width, tap_coef);   %upper arm of the taper
      
      g = g + gdsdata( ary2poly(layer, curve1, curve2) );
      g = g + gdsdata(ary2poly(30, curve1, curve2));   % same on dummy layer
      c.incoord(1:2,i) = tap_cent;
 
   end;
      
   slab_poly(1,2*i  )=tap_array2(1);
   slab_poly(2,2*i  )=tap_array2(2);
   slab_poly(1,2*i+1)=tap_array2(3);
   slab_poly(2,2*i+1)=tap_array2(4);

   
end;    % end of the for loop %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%% draw slab region %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

slab_poly(1,1       ) = p.roland*cos(p.theta0-theta_slab);
slab_poly(2,1       ) = p.roland*sin(p.theta0-theta_slab);
slab_poly(1,2*in_no+2) = p.roland*cos(p.theta0+theta_slab);
slab_poly(2,2*in_no+2) = p.roland*sin(p.theta0+theta_slab);

io_poly1(1)=(cos(p.theta0)-cos(p.theta0+theta_io))*p.roland/2;
io_poly1(2)=(sin(p.theta0)-sin(p.theta0+theta_io))*p.roland/2;
io_poly2(1)=(cos(p.theta0)-cos(p.theta0-theta_io))*p.roland/2;
io_poly2(2)=(sin(p.theta0)-sin(p.theta0-theta_io))*p.roland/2;

m_poly1(1) = (io_poly1(1)+slab_poly(1,1))/2;
m_poly1(2) = (io_poly1(2)+slab_poly(2,1))/2;
m_poly2(1) = (io_poly2(1)+slab_poly(1,2*in_no+2))/2;
m_poly2(2) = (io_poly2(2)+slab_poly(2,2*in_no+2))/2;

slab_poly(:,1) = io_poly2(:);
slab_poly(:,2*in_no+2) = io_poly1(:);


poly = [m_poly2(1) slab_poly(1,:) m_poly1(1); m_poly2(2) slab_poly(2,:) m_poly1(2)];
g = g + gdsdata(add2poly([],layer,poly));
g = g + gdsdata(add2poly([],30,poly));   % same on dummy layer

poly = [slab_poly(1,1:2) slab_poly(1,end-1:end) m_poly1(1) m_poly2(1); ...
        slab_poly(2,1:2) slab_poly(2,end-1:end) m_poly1(2) m_poly2(2)];
g = g + gdsdata(slabdummy(poly));  %extra dummy structures

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

s=['Input waveguide design parameters', 13,10];
s=[s,sprintf('Roland circle diameter= %.2fum', p.roland), 13,10];
s=[s,sprintf('Vernier correction = %5.3fnm', wl_err*1000), 13,10];
s=[s,sprintf('Taper width (including CD loss) = %.2fum, taper length = %dum', tap_width, p.tap_len), 13,10];
s=[s,sprintf('Left edge of the I/O waveguides = %.2fum', io_st_x), 13,10];
s=[s,sprintf('Number of input waveguides = %d', in_no), 13,10];
s=[s, 13,10, 'Input angles (degrees):', 13,10];
for k=1:in_no
   s=[s,sprintf(' %.6f ,', angle_ary(k)*180/pi)];
   c.inang(k) = angle_ary(k)*180/pi;
end;

s=[s, 13,10, 'X taper end position (um):', 13,10];
for k=1:in_no
   s=[s,sprintf(' %.3f ,', io_taper_end_x(k))];
   c.inx(k) = io_taper_end_x(k);
end;

s=[s, 13,10, 'Y taper end position (um):', 13,10];
for k=1:in_no
   s=[s,sprintf(' %.3f ,', io_taper_end_y(k))];
   c.iny(k) = io_taper_end_y(k);
end;

s=[s, 13,10, 13,10, 'Number of output waveguides = '];
s=[s,sprintf('%d', out_no), 13,10];

s=[s, 13,10, 'Output angles (degrees):', 13,10];
for k=1:out_no
   s=[s,sprintf(' %.6f ,', out_angle_ary(k)*180/pi)];
end;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Output section %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear in_cent;
clear poly;
gout = gdsdata; %output portion

p_ary = p.roland.*sin_out_angle_ary;   % p=rsl*sin(angle_ary) 
% line equation (x*sin(tilt)-y*cos(tilt)-p=0), p is distance from (0,0) to line
% for ch_no > 0, p>0, for ch_no<0, p<0 

angle_ary = out_angle_ary;   % this is angle_ary
tilt_angle = angle_ary + p.theta0;

y_1st = -(p.roland+st_len+p.tap_len)*sin(tilt_angle(1)) + grt_cent(2) - r_bend*(1-cos(tilt_angle(1)));
ss = sprintf(' 1st output WG y < %13.3f um',y_1st);
disp(ss);
clear ss;

y_begin = input('1st output WG y position (um) = ');
if isempty(y_begin)
   y_begin = y_1st;
end;

y_space = input('Output WG y spacing = [127 um] ');
if isempty(y_space)
   y_space = 127;
end;

io_no = out_no;
y_posi = y_begin - y_space.*(0:(io_no-1));

% r_bend=abs(x_posi*sin(tilt)-y*cos(tilt)-P)
x_posi = ((y_posi+r_bend).*cos(tilt_angle)+p_ary-r_bend)./sin(tilt_angle);

in_cent(:,1) = x_posi';
in_cent(:,2) = y_posi'+r_bend;

shortest_st = 100;   %um
io_st_x = floor((min(x_posi)-shortest_st)/shortest_st)*shortest_st;


for i = 1:io_no
    
   if strip == 'N'   % stripping section
        
    % draw the bending i/o array
    in_array2 = arccospn(in_cent(i,:),r_bend-junction_offset,-pi/2,tilt_angle(i),p.width,dtheta);
    gout = gout + gdsdata( ary2poly(layer,in_array2(1:2,:),in_array2(3:4,:)) );
    gout = gout + gdsdata( ary2poly(30,in_array2(1:2,:),in_array2(3:4,:)) );   % same on dummy layer
    
    % align the input straight wg start to io_st_x position 
    in_array2 = arccospn(in_cent(i,:),r_bend,-pi/2,tilt_angle(i),p.width,dtheta);
 
    poly(:,1)=in_array2(1:2,1);
    poly(:,2)=in_array2(3:4,1);
    poly(1,3)=io_st_x;
    poly(2,3)=in_array2(4,1);
    poly(1,4)=io_st_x;
    poly(2,4)=in_array2(2,1);
    gout = gout + gdsdata(add2poly([],layer,poly));
    gout = gout + gdsdata(add2poly([],30,poly)); % same on dummy layer

    % putting in straight WG between taper WG and bending WG
    in_array2 = arccospn(in_cent(i,:),r_bend,-pi/2,tilt_angle(i),p.width,dtheta);
    co_ind2 = size(in_array2, 2);
    st_array2(1)=-(p.roland*cos(angle_ary(i))+p.tap_len)*cos(tilt_angle(i))+grt_cent(1)-p.width/2*sin(tilt_angle(i));
    st_array2(2)=-(p.roland*cos(angle_ary(i))+p.tap_len)*sin(tilt_angle(i))+grt_cent(2)+p.width/2*cos(tilt_angle(i));
    st_array2(3)=-(p.roland*cos(angle_ary(i))+p.tap_len)*cos(tilt_angle(i))+grt_cent(1)+p.width/2*sin(tilt_angle(i));
    st_array2(4)=-(p.roland*cos(angle_ary(i))+p.tap_len)*sin(tilt_angle(i))+grt_cent(2)-p.width/2*cos(tilt_angle(i));
 
    poly(:,1)=in_array2(1:2,co_ind2);
    poly(:,2)=in_array2(3:4,co_ind2);
    poly(:,3)=st_array2(3:4)';
    poly(:,4)=st_array2(1:2)';
    gout = gout + gdsdata(add2poly([],layer,poly));
    gout = gout + gdsdata(add2poly([],30,poly)); % same on dummy layer
    
  end               % end of stripping section
    
   io_taper_end_x(i) = -(p.roland*cos(angle_ary(i))+p.tap_len)*cos(tilt_angle(i))+grt_cent(1);
   io_taper_end_y(i) = -(p.roland*cos(angle_ary(i))+p.tap_len)*sin(tilt_angle(i))+grt_cent(2);

   % putting in tappering WG
   tap_array2(1)=-(p.roland*cos(angle_ary(i)))*cos(tilt_angle(i))+grt_cent(1)-tap_width/2*sin(tilt_angle(i));
   tap_array2(2)=-(p.roland*cos(angle_ary(i)))*sin(tilt_angle(i))+grt_cent(2)+tap_width/2*cos(tilt_angle(i));
   tap_array2(3)=-(p.roland*cos(angle_ary(i)))*cos(tilt_angle(i))+grt_cent(1)+tap_width/2*sin(tilt_angle(i));
   tap_array2(4)=-(p.roland*cos(angle_ary(i)))*sin(tilt_angle(i))+grt_cent(2)-tap_width/2*cos(tilt_angle(i));

   if taper_choice=='L'
      
      %putting in linear tappered WG
      poly(:,1)=tap_array2(1:2)';
      poly(:,2)=tap_array2(3:4)';
      % poly(:,3) & poly(:,4) is the same
      gout = gout + gdsdata(add2poly([],layer,poly));
      gout = gout + gdsdata(add2poly([],30,poly)); % same on dummy layer
      
   else
      %%%%% draw parabolic tapers %%%%%%
      tap_cent(1) = -(p.roland*cos(angle_ary(i))+p.tap_len)*cos(tilt_angle(i)) + grt_cent(1);
      tap_cent(2) = -(p.roland*cos(angle_ary(i))+p.tap_len)*sin(tilt_angle(i)) + grt_cent(2);
      curve1 = io_taper_curve1(tap_cent, tilt_angle(i), dxy, p.tap_len, p.width, tap_coef);   %lower arm of the taper
      curve2 = io_taper_curve2(tap_cent, tilt_angle(i), dxy, p.tap_len, p.width, tap_coef);   %upper arm of the taper
      
      gout = gout + gdsdata( ary2poly(layer, curve1, curve2) );
      gout = gout + gdsdata(ary2poly(30, curve1, curve2)); % same on dummy layer
      c.outcoord(1:2,i) = tap_cent;
 
   end;

   %%%%% Connect to slab waveguide %%%%%%
   slab_poly(1,2*i  )=tap_array2(1);
   slab_poly(2,2*i  )=tap_array2(2);
   slab_poly(1,2*i+1)=tap_array2(3);
   slab_poly(2,2*i+1)=tap_array2(4);

   
end;

%%%%%%%%%%%% Draw slab waveguide %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

slab_poly(1,1       ) = p.roland*cos(p.theta0-theta_slab);
slab_poly(2,1       ) = p.roland*sin(p.theta0-theta_slab);
slab_poly(1,2*io_no+2) = p.roland*cos(p.theta0+theta_slab);
slab_poly(2,2*io_no+2) = p.roland*sin(p.theta0+theta_slab);

io_poly1(1)=(cos(p.theta0)-cos(p.theta0+theta_io))*p.roland/2;
io_poly1(2)=(sin(p.theta0)-sin(p.theta0+theta_io))*p.roland/2;
io_poly2(1)=(cos(p.theta0)-cos(p.theta0-theta_io))*p.roland/2;
io_poly2(2)=(sin(p.theta0)-sin(p.theta0-theta_io))*p.roland/2;

m_poly1(1) = (io_poly1(1)+slab_poly(1,1))/2;
m_poly1(2) = (io_poly1(2)+slab_poly(2,1))/2;
m_poly2(1) = (io_poly2(1)+slab_poly(1,2*io_no+2))/2;
m_poly2(2) = (io_poly2(2)+slab_poly(2,2*io_no+2))/2;

slab_poly(:,1) = io_poly2(:);
slab_poly(:,2*io_no+2) = io_poly1(:);

poly = [m_poly2(1) slab_poly(1,:) m_poly1(1); m_poly2(2) slab_poly(2,:) m_poly1(2)];
gout = gout + gdsdata(add2poly([],layer,poly));
gout = gout + gdsdata(add2poly([],30,poly)); % same on dummy layer

poly = [slab_poly(1,1:2) slab_poly(1,end-1:end) m_poly1(1) m_poly2(1); ...
        slab_poly(2,1:2) slab_poly(2,end-1:end) m_poly1(2) m_poly2(2)];
gout = gout + gdsdata(slabdummy(poly));  %extra dummy structures

%%%%%%%%%%%%%%% End of Drawing Slab waveguide %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

gout = offset( mirror( offset(gout,[-p.Lx 0]),-1 ),[p.Lx 0] ); % reflection around x=Lx plane
g = g + gout;


s=[s,'AWG standard output channel design input parameters', 13,10];
s=[s,sprintf('Number of output channels = %d to %d', out_begin_no, out_end_no), 13,10];
s=[s,sprintf('Vernier correction = %5.3fnm', wl_err*1000), 13,10];
s=[s,sprintf('Taper width (including CD loss) = %.2fum', tap_width), 13,10];
s=[s,sprintf('Straight section between taper and bending = %.2fum', st_len), 13,10];
s=[s,sprintf('Right edge of the I/O waveguides = %.2fum', 2*p.Lx-io_st_x), 13,10];
s=[s,sprintf('Lowest output waveguides at y = %.2fum', y_posi(io_no)), 13,10];
s=[s,sprintf('Number of output waveguides = %d', io_no), 13,10, 13,10, 13,10];
s=[s,sprintf('Mirror at x = %d um', p.Lx), 13,10];
s=[s,sprintf('theta0 = %.1f degrees', p.theta0*180/pi), 13,10, 13,10, 13,10];

s=[s, 13,10, 13,10, 'Output angles (degrees):', 13,10];
for k=1:io_no
   s=[s,sprintf(' %.6f ,', angle_ary(k)*180/pi)];
   c.outang(k) = angle_ary(k)*180/pi;
end;

s=[s, 13,10, 13,10, 'X taper end position (um):', 13,10];
for k=1:io_no
   s=[s,sprintf(' %.3f ,', io_taper_end_x(k))];
   c.outx(k) = io_taper_end_x(k);
end;

clear k;
s=[s, 13,10, 13,10, 'Y taper end position (um):', 13,10];
for k=1:io_no
   s=[s,sprintf(' %.3f ,', io_taper_end_y(k))];
   c.outy(k) = io_taper_end_y(k);
end;

str.calc = c;
str.gds = g;
str = comment(str, s);
