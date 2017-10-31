function str = update(str)

% This function allows interactive update of VARIED parameters.
% SF, January 31, 2006

p = str.param;

p.freq0 = input('Enter center frequency [default is 193200 GHz] = ');
if isempty(p.freq0)        
   p.freq0 = 193200.0;   %
end;

p.channel = input('Enter channel spacing [default is 100 GHz] = '); % old dfreq
if isempty(p.channel)
   p.channel=100.0;   %channel spacing (GHz)
end;
p.dlambda = p.c_light/p.freq0^2*p.channel/1000.0;               %channel spacing in um

p.order = input('Enter grating order [default is 38] = ');   % grating order
if isempty(p.order)
   p.order = 38;            %
end;

p.pitch = input('Enter grating pitch [default is 8um] = ');
if isempty(p.pitch)        
   p.pitch = 8.0;   % in microns
end;

default_rsl = p.min_D*(p.pitch*p.inslab)/(p.Nc_nc*p.order*p.dlambda);   %um
disp(sprintf('Default Roland circle diameter=%.2fum for min_D=%.1fum', default_rsl, p.min_D));
disp(sprintf('Pitch=%f,Nslab=%f,Nc_Nc=%f,Order=%f,Dlambda=%f', p.pitch,p.inslab,p.Nc_nc,p.order,p.dlambda));
% Roland circle diameter (um), rsl means raidus of the slab region
p.roland = input('Enter Roland circle diameter rsl (um) = ');
if isempty(p.roland)
   p.roland = ceil(default_rsl/10)*10;   %um
end;

p.inwave = input('Enter the width of the input waveguide [default is 9um] = ');
if isempty(p.inwave)        
   p.inwave = 9.0;   % um
end;

p.tap_len = input('tapering WG length [default is 800 um] = ');
if isempty(p.tap_len)
  p.tap_len = 800;   
end;

p.spacing = input('Equal chord spacing or angle spacing? [c/a]  ', 's');
if isempty(p.spacing)
   p.spacing = 'c';
end;

disp(' ');

str.param = p;
str = pcalc(str);   %update the rest
  



