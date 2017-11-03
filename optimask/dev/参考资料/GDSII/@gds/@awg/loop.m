function str = loop(str)

% This function loops AWG layout process.
% SF, January 31, 2006

run_flag = 'n';

while (run_flag == 'N' | run_flag == 'n') 
  
  str.param.thetaang = input('Enter theta0 [default is 65 degree] = ');
  if isempty(str.param.thetaang)         
     str.param.thetaang = 65;
  end;
  str.param.theta0 = str.param.pi180*str.param.thetaang;
  
  min_lx0 = (str.param.roland+1000)*cos(str.param.theta0)/1000;
  disp(sprintf('Lx must be larger than this value (mm) = %f',min_lx0));
  
  lx0 = input('Enter Lx [default is 4.5 mm] = ');
  if isempty(lx0)
     lx0 = 4.5;    % mm
  end;
  str.param.Lx = lx0*1000;   % um

  
  str.param.betaang = input('Enter beta1 [default is 20 degree] = ');   
  if isempty(str.param.betaang)           
     str.param.betaang = 20;
  end;       
  str.param.beta1 = str.param.betaang*str.param.pi180;

  testflag=input('Are the above input parameters correct? [yes/no]  ', 's');
  if isempty(testflag)
     testflag='y';
  end;
  if ((testflag(1)=='n') | (testflag(1)=='N'))   % any other key will pass
     fclose('all');
     disp('Please select again');
     disp('Program stopped');
     return;
  end;
  
  str = calculate(str);
  
  clear c;
  c=[13,10, 'AWG design input parameters', 13,10];
  c=[c,sprintf('Channel spacing=  %d GHz', str.param.channel), 13,10];
  c=[c,sprintf('Free Spectrum Range= %d GHz', str.param.fsr), 13,10];
  c=[c,sprintf('Grating order m= %d, pitch d= %6.2f um', str.param.order, str.param.pitch), 13,10];
  c=[c,sprintf('Roland circle diameter= %.4f um', str.param.roland), 13,10];
  c=[c,sprintf('slab index = %10.6f, channel index = %10.6f', str.param.inslab, str.param.inchannel), 13,10];
  c=[c,sprintf('Input waveguide width = %2.1f', str.param.inwave), 13,10];
  c=[c,sprintf('Center wavelength = %12.4f nm', str.param.lambda0*1000), 13,10];
  c=[c,sprintf('Lx= %12.2f mm', str.param.Lx), 13,10];
  c=[c,sprintf('theta0=%12.2f deg',str.param.thetaang), 13,10];
  c=[c,sprintf('beta1= %12.2f deg', str.param.betaang), 13,10, 13,10];
  c=[c,'Calculated Results',13,10];
  c=[c,sprintf('Arrayed WG number= %d', str.param.wgno-2*str.param.dum), 13,10];
  c=[c,sprintf('beta max= %6.3f deg, beta min= %6.3f deg', max(str.calc.beta)/str.param.pi180, min(str.calc.beta)/str.param.pi180), 13,10];
  c=[c,sprintf('Constant path difference dl= %.3f nm', str.param.dL*1000), 13,10];
  c=[c,sprintf('dl error max= %e, min= %e', max(str.calc.dddl), min(str.calc.dddl)), 13,10];
  c=[c,sprintf('r max= %6.3f mm, r min= %6.3f mm', max(str.calc.r)/1000, min(str.calc.r)/1000), 13,10];
  c=[c,sprintf('Straight WG length max= %7.3f mm, min= %7.3f mm', max(str.calc.di)/1000, min(str.calc.di)/1000), 13,10];
  c=[c,sprintf('WG height max= %6.3f mm, min= %6.3f mm', max(str.calc.v)/1000, min(str.calc.v)/1000), 13,10];
  c=[c,sprintf('WG spacing max= %6.2f um,  min= %6.2f um', max(str.calc.dv), min(str.calc.dv)), 13,10];
  c=[c,sprintf('Minimum WG spacing before bending= %.2f um', str.calc.min_space_before_bend), 13,10];
  c=[c,sprintf('Output waveguide spacing= %.3f um', str.param.D), 13,10];
  
  disp(c)

  run_flag=input('Finish the calculation? [yes/no]  ', 's');
  if isempty(run_flag)
     run_flag='n';
  end;
  if run_flag ~= 'y'
      run_flag = 'n';
  end
    
end;  %%% End of while loop



