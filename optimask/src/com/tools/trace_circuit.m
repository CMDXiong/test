% Circuit fracture data trace
clc

% clear variables
% fprintf('Getting DPI data...\n');
% [polypts1,polypts2]=cmd2line('gffbus2.CMD');
% fprintf('Sorting data...\n');
% linepts1=[polypts1 polypts2];
% clear polypts1 polypts2
% linepts2=sort(linepts1,2);
% clear linepts1
% linepts1=unique(linepts2,'rows');
% clear linepts2
% linepts1=reshape(linepts1,[],1);
% [linepts2,IX]=sort(linepts1);
% linepts1=reshape(fliplr(reshape(linepts1,[],2)),[],1);
% linepts=[linepts2 linepts1(IX)];
% clear linepts1 linepts2 IX

% trace waveguides
tracepts=[];
startpt=-988.075-j*1212.197;
endpt=-988.636-j*1212.198;
trace=traceWG(linepts(:,1),linepts(:,2),startpt,endpt,0,1,0.55);
tracepts=[tracepts; trace; startpt];
% startpt=1411.295+j*99.275;
% endpt=1197-j*263.72;
% trace=traceWG(linepts(:,1),linepts(:,2),startpt,endpt,pi,1,0.55);
% tracepts=[tracepts; trace];
% startpt=1197-j*250.52;
% endpt=3570.769-j*548.64;
% trace=traceWG(linepts(:,1),linepts(:,2),startpt,endpt,0,1,0.55);
% tracepts=[tracepts; trace];
% startpt=3570.769-j*550.141;
% trace=traceWG(linepts(:,1),linepts(:,2),startpt,endpt,pi,1,0.55);
% tracepts=[tracepts; trace];
% [startval,startidx]=min(abs(startpt-linepts(:,1)));
% startpt=linepts(startidx,1);

% plot
if length(tracepts)>1000000
    tracepts=tracepts(1:10000);
end
stangle=diff(unwrap(angle(diff(tracepts))));
% stangle=[0; stangle];
numplot=1; stang=zeros(size(stangle));

% output to ICED command file
fid=fopen('trace.cmd','wt');
for i=1:length(tracepts)-2
    if abs(stangle(i))<pi/450
        stang(numplot)=stangle(i); numplot=numplot+1;
        fprintf(fid,'ad w l8 w.002 %.12g %.12g %.12g %.12g\n',...
            real(tracepts(i)),imag(tracepts(i)),real(tracepts(i+1)),imag(tracepts(i+1)));
    else
        fprintf(fid,'ad w l40 w.002 %.12g %.12g %.12g %.12g\n',...
            real(tracepts(i)),imag(tracepts(i)),real(tracepts(i+1)),imag(tracepts(i+1)));
    end
end
fclose(fid);
fprintf('Done!\n');
stang=stang(1:numplot-1);
figure;
subplot(2,1,1);
plot(stang*180/pi); grid on; % ylim([-1 1]);
xlabel('Points'); ylabel('Steering angle (degree)');
subplot(2,1,2);
plot(tracepts);
for pts=100:100:length(tracepts)
    text(real(tracepts(pts)),imag(tracepts(pts)),['\leftarrow',num2str(pts)]);
end

