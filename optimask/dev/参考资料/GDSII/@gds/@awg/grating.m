function str = grating(str,res,layer)

% GRATING generates polygons for the grating section of AWG.
% If resolution not specified, it will ask for one.
% If layer is not specified, the layer is defaulted to 6.
% Starting this method eliminates previously calculated gdsdata.
% SF, February 1 2006.

if nargin < 3
    layer = 6;          % Default layer
end

p = str.param;
c = str.calc;
g = gdsdata;
plg = [];

if nargin < 2
    disp('Maximum allowed length error for curved waveguide');
    p.max_curve_err = input('[default is 0.08 nm, 1000 for low resolution] = ');
    if isempty(p.max_curve_err) 
        p.max_curve_err = 50; %0.08/1000 ;     %um
    else
        p.max_curve_err = p.max_curve_err/1000 ;      %um
    end;
else
    p.max_curve_err = res;
end

ang_span = p.theta0 + c.alpha;

%%%%%%%%%%% Define individual taper_ang(i) so that the gap is a constant number %%%%%%%%

for i = 2:p.wgno
   taper_ang(i) = 0.5*( ang_span(i-1) + ang_span(i) );
end;

taper_ang(1) = ang_span(1) - (taper_ang(2)-ang_span(1));
gap_span = p.gap/p.roland;  % in radian, along the arc

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%Generate arc sections of the array waveguides
cent_coor(1:p.wgno,1) = p.Lx.*ones(size(c.ao'));
cent_coor(1:p.wgno,2) = c.ao';

for i = 1+p.dum:p.wgno-p.dum
   
   dtheta(i) = 2*sqrt(3*p.max_curve_err/c.r(i)/ang_span(i)) ;
   array = arccospn(cent_coor(i,:),c.r(i),pi/2,ang_span(i),p.width,dtheta(i));
   g = g + gdsdata( ary2poly( layer,array(1:2,:),array(3:4,:) ) );
   
   if i == 1+p.dum
       bot = array(1:2,:);
   end
   if i == p.wgno-p.dum
       top = array(3:4,:);
   end
   
end;

g = g + gdsdata(ardummy(top,bot)); % dummy structures in place of grating

%Tapered and straight WGs %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

for i=1:p.wgno
   
   %Straight WGs
   poly(1,1)=-(c.r(i)-p.width/2)*sin(ang_span(i))+cent_coor(i,1);        %x1
   poly(2,1)= (c.r(i)-p.width/2)*cos(ang_span(i))+cent_coor(i,2);        %y1
   poly(1,2)=-(c.r(i)+p.width/2)*sin(ang_span(i))+cent_coor(i,1);        %x2
   poly(2,2)= (c.r(i)+p.width/2)*cos(ang_span(i))+cent_coor(i,2);        %y2
   poly(1,3)=poly(1,2)-(c.di(i)-p.tap_len)*cos(ang_span(i));             %x3
   poly(2,3)=poly(2,2)-(c.di(i)-p.tap_len)*sin(ang_span(i));             %y3
   poly(1,4)=poly(1,1)-(c.di(i)-p.tap_len)*cos(ang_span(i));             %x4
   poly(2,4)=poly(2,1)-(c.di(i)-p.tap_len)*sin(ang_span(i));             %y4
   g = g + gdsdata(add2poly([],layer,poly));
   g = g + gdsdata(add2poly([],30,poly)); % default dummy repeat on layer 30

   %Linear tapers with gaps
   
   if i < p.wgno
      tap_poly(1,1)=p.roland*cos( taper_ang(i) + gap_span/2 );   %taper_x1
      tap_poly(2,1)=p.roland*sin( taper_ang(i) + gap_span/2 );   %taper_y1
      tap_poly(1,2)=p.roland*cos( taper_ang(i+1) - gap_span/2 );   %taper_x2
      tap_poly(2,2)=p.roland*sin( taper_ang(i+1) - gap_span/2 );   %taper_y2
   else   
      tap_poly(1,1)=p.roland*cos( taper_ang(i) + gap_span/2 );   %taper_x1
      tap_poly(2,1)=p.roland*sin( taper_ang(i) + gap_span/2 );   %taper_y1
      last_taper_span = ang_span(p.wgno) - taper_ang(p.wgno);      
      tap_poly(1,2)=p.roland*cos( ang_span(i) + last_taper_span - gap_span/2 );   %taper_x2
      tap_poly(2,2)=p.roland*sin( ang_span(i) + last_taper_span - gap_span/2 );   %taper_y2
   end;
      
   tap_poly(:,3)=poly(:,3);
   tap_poly(:,4)=poly(:,4);
   g = g + gdsdata(add2poly([],layer,tap_poly));
   g = g + gdsdata(add2poly([],30,tap_poly)); % default dummy repeat on layer 30
   
end;

%%%%%%%%%%%% Add a "perfect" arc along the junction where grating waveguides start/end %%%%%%%%%%%%%%%%%%%%%%

if p.max_curve_err > (20/1000)
   linear_res = 20;    %um, draw low resolution patterns
else   
   linear_res = 0.6;   %um, This is good enough for 1.3um gap.  It will be used to draw segmenting arcs, too.
end;

angle_res = linear_res/p.roland;
arc_begin_angle = ang_span(1)-0.01;
arc_span = ang_span(p.wgno)-ang_span(1)+0.02;
arc_wd = 10*ceil(0.1*p.roland*(1-cos(arc_span/2))) ;   %um
arc_array = arccospn([0,0],p.roland-arc_wd/2,arc_begin_angle,arc_span,arc_wd,angle_res);

g = g + gdsdata(ary2poly(layer,arc_array(1:2,:), arc_array(3:4,:)));
g = g + gdsdata(ary2poly(30,arc_array(1:2,:), arc_array(3:4,:))); % dummy repeat
   
%%%%%%%%% Add slab waveguide %%%%%%%%%%%%%%%%%%%%%%%%%%%%

theta_slab = 14*p.pi180;
theta_io   =  10*p.pi180;

slab_poly(1,1) = p.roland*cos(p.theta0-theta_slab) ;                % x1
slab_poly(2,1) = p.roland*sin(p.theta0-theta_slab)  ;               % y1
slab_poly(1,2) = p.roland*cos(arc_begin_angle) ;                    % x2
slab_poly(2,2) = p.roland*sin(arc_begin_angle) ;                    % x2
slab_poly(1,3) = p.roland*cos(arc_begin_angle+arc_span) ;           % x3
slab_poly(2,3) = p.roland*sin(arc_begin_angle+arc_span) ;           % x3
slab_poly(1,4) = p.roland*cos(p.theta0+theta_slab) ;                % x4
slab_poly(2,4) = p.roland*sin(p.theta0+theta_slab)  ;               % y4

slab_poly(1,5) = ( (cos(p.theta0)-cos(p.theta0-theta_io))*p.roland/2 + p.roland*cos(p.theta0+theta_slab) )/2 ;
slab_poly(2,5) = ( (sin(p.theta0)-sin(p.theta0-theta_io))*p.roland/2 + p.roland*sin(p.theta0+theta_slab) )/2 ;
slab_poly(1,6) = ( (cos(p.theta0)-cos(p.theta0+theta_io))*p.roland/2 + p.roland*cos(p.theta0-theta_slab) )/2 ;
slab_poly(2,6) = ( (sin(p.theta0)-sin(p.theta0+theta_io))*p.roland/2 + p.roland*sin(p.theta0-theta_slab) )/2 ;

g = g + gdsdata(add2poly([],layer,slab_poly));
g = g + gdsdata(add2poly([],30,slab_poly)); % dummy repeat
g = g + gdsdata(slabdummy(slab_poly));  %extra dummy structures

%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Add segmenting arcs %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

switch p.pitch
   
   case {14}
      num_of_arc = 9 ;   %um, for d=14
      arc_wd = 15 ;        %um, for d=14
      gap_step_size = 0.7 ;   % um
   
   case {12}   
      num_of_arc = 7 ;   %um, for d=12, 9
      arc_wd = 17 ;        %um, for d=12
      gap_step_size = 0.7 ;   % um

   case {9}
      num_of_arc = 7 ;   %um, for d=12, 9
      arc_wd = 5 ;          %um, for d=9
      gap_step_size = 0.5 ;   % um
      
   case{6}
      num_of_arc = 5 ;   %um, for d=6
      arc_wd = 4 ;          %um, for d=6
      gap_step_size = 0.5 ;   % um
      
  otherwise
      num_of_arc = 7 ;   %um, for d=12, 9
      arc_wd = 5 ;          %um, for d=9
      gap_step_size = 0.5 ;   % um

end;


gap1 = 2 ;         % um, drawn on mask?


for j=1:num_of_arc   
     
     r_arc = p.roland + gap1*j + gap_step_size*j*(j-1)/2 + (j-1)*arc_wd + arc_wd/2 ;;
           
      if j==num_of_arc
         
           if (p.pitch ==6 ) | (p.pitch == 12)
              % the last arc is only half wide as the others for d=6, 12
              arc_wd = arc_wd/2 ;
              r_arc = r_arc-arc_wd/2 ;
           end;
           
      end; 
          
     angle_res = linear_res/r_arc;
     arc2_array = arccospn([0,0],r_arc,arc_begin_angle,arc_span,arc_wd+p.cdloss*2,angle_res);
     seg = ary2poly(layer,arc2_array(1:2,:), arc2_array(3:4,:));
     
     g = g + gdsdata(seg);
     
end;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

gout = offset( mirror( offset(g,[-p.Lx 0]),-1 ),[p.Lx 0] ); % reflection around x=Lx plane
g = g + gout;

clear s;
s=['AWG design input parameters', 13,10];
s=[s,sprintf('Channel spacing=  %d GHz', p.channel), 13,10];
s=[s,sprintf('Free Spectrum Range= %d GHz', p.fsr), 13,10];
s=[s,sprintf('Grating order m= %d, pitch d= %6.2f um', p.order, p.pitch), 13,10];
s=[s,sprintf('Roland circle diameter= %.4f um', p.roland), 13,10];
s=[s,sprintf('slab index = %10.6f, channel index = %10.6f', p.inslab, p.inchannel), 13,10];
s=[s,sprintf('Center wavelength = %12.4f nm', p.lambda0*1000), 13,10];
s=[s,sprintf('Lx= %12.3f mm', p.Lx), 13,10];
s=[s,sprintf('theta0=%12.4f deg',p.thetaang), 13,10];
s=[s,sprintf('beta1= %12.4f deg', p.betaang), 13,10, 13,10];
s=[s,'Calculated Results',13,10];
s=[s,sprintf('Arrayed WG number= %d', p.wgno-p.dum*2), 13,10];
s=[s,sprintf('beta max= %6.3f deg, beta min= %6.3f deg', max(c.beta)/p.pi180, min(c.beta)/p.pi180), 13,10];
s=[s,sprintf('Constant path difference dl= %.6f nm', p.dL*1000), 13,10];
s=[s,sprintf('dl error max= %e, min= %e', max(c.dddl), min(c.dddl)), 13,10];
s=[s,sprintf('r max= %6.3f mm, r min= %6.3f mm', max(c.r)/1000, min(c.r)/1000), 13,10];
s=[s,sprintf('Straight WG length max= %7.3f mm, min= %7.3f mm', max(c.di)/1000, min(c.di)/1000), 13,10];
s=[s,sprintf('WG height max= %6.3f mm, min= %6.3f mm', max(c.v)/1000, min(c.v)/1000), 13,10];
s=[s,sprintf('WG spacing max= %6.2f um,  min= %6.2f um', max(c.dv), min(c.dv)), 13,10];
s=[s,sprintf('Minimum WG spacing before bending= %.2f um', c.min_space_before_bend), 13,10];
s=[s,sprintf('Output waveguide spacing= %.3f um', p.D), 13,10, 13,10];
s=[s,'New data',13,10];  
s=[s,sprintf('Taper WG length (um) =%.2f', p.tap_len),13,10];
s=[s,sprintf('After taper straight WG length max= %.3f um, min= %.3f um', max(c.di)-p.tap_len, min(c.di)-p.tap_len), 13,10];
s=[s,sprintf('Maximum grating waveguide angular resolution = %.3f deg', max(dtheta)/p.pi180), 13,10, 13,10];
s=[s,'Add segmenting arcs', 13,10];
s=[s,sprintf('Number of segmenting arcs = %d', num_of_arc), 13,10];
s=[s,sprintf('Segmenting arc width = %4.2f (um)', arc_wd), 13,10];
s=[s,sprintf('Gap step size = %4.3f (um)', gap_step_size), 13,10];
s=[s,sprintf('Initial gap drawn on mask = %4.3f (um)', gap1-p.cdloss), 13,10];

str.comment = s;
str.param = p;
str.gds = g;


