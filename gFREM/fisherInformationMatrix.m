function I=fisherInformationMatrix(x,f1,f2, int1, int2, pixelversion, offset)
% I=fisherInformationMatrix(x,f1,f2, int1, int2, pixelversion, offset)

if ~exist('pixelversion','var')
    pixelversion = 0; 
end
if ~exist('offset','var')
    offset=0;
end

dx=x(2)-x(1);
if pixelversion    
    [fp1, xp]= pixelize1d(x,f1);
    fp2= pixelize1d(x,f2);
    gfp1=pixelize1d(x,gradient(f1,dx));
    gfp2=pixelize1d(x,gradient(f2,dx));
else     
    fp1=f1; 
    fp2=f2;
    gfp1=gradient(f1,dx);
    gfp2=gradient(f2,dx);
    xp=x;
end
I=zeros(2); %Fisher Information matrix 2x2;
p1 = int1*fp1+int2*fp2+offset;
p2 = int2*fp1+int1*fp2+offset;

prec = 10^-8;
mask = abs(gfp1.^2)>prec;
kernel1 = (1./p1(mask)).*(int1*gfp1(mask)).^2;
mask = abs(gfp2.^2)>prec;
kernel2 = (1./p2(mask)).*(int2*gfp2(mask)).^2;
mask = abs(gfp1.*gfp2)>prec;
kernel3 = (1./p1(mask))*int1.*int2.*gfp1(mask).*gfp2(mask);
% 
% figure (3)
% plot(x,gfp1.*gfp2, x, gfp1, x, gfp2)
% figure (4)
% hold on
% plot (x,(1./p1).*int1.*int2.*gfp1.*gfp2,x,p1-offset, x, p2-offset);
% figure (5)
% hold on
% plot (x,(1./p1).*int1.*int1.*gfp1.*gfp1,x,p1-offset, x, p2-offset);
% pause

% kernel1 = (1./p1).*(int1*gfp1).^2;
% kernel2 = (1./p2).*(int2*gfp2).^2;
% kernel3 = (1./p1).*int1.*int2.*gfp1.*gfp2; 
% % test
% col = rand(1,3);
% figure(22);
% subplot(2,2,1)
% hold on
% plot(kernel2, 'color',col);
% subplot(2,2,2)
% hold on
% plot(kernel1, 'color',col);
% subplot(2,2,3)
% hold on
% plot(kernel3, 'color',col);
% subplot(2,2,4)
% hold on
% plot(p1, 'color',col);
% pause 
  
if pixelversion
    I(1,1)=sum(kernel1);
    I(2,2)=sum(kernel2);
    I(1,2)=sum(kernel3);
    I(2,1)=I(1,2);
else
    
    I(1,1)=trapz(xp(1:length(kernel1)),kernel1);
    I(2,2)=trapz(xp(1:length(kernel2)),kernel2);
    I(1,2)=trapz(xp(1:length(kernel3)),kernel3);
    I(2,1)=I(1,2);
end