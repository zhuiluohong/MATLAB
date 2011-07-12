function I=numericalMeanEstimation(x,f1,f2, offset, nphot)
% I=numericalMeanEstimation(x,f1,f2, offset, nphot)

if ~exist('offset','var')
    offset=0;
end
if ~exist('nphot','var')
    nphot=1;
end
l(:,1)=nphot*f1;
l(:,2)=nphot*f2;
l(:,3)=nphot*(f1+f2);
l(:,4)=zeros(length(f1),1);
lbg=l+offset;               % adding background
N=size(l,2);
dx=x(2)-x(1);
precMask=10^-8;             % Precision for masking intenisty images     
prec=10^-10;                % Precision up to which n_k will be generated                    
pcdf=poisscdf(0:50*max(lbg(:)),max(lbg(:)));
nmax = sum(pcdf<1-prec);

n=0:nmax;
ln=length(n);
lf=length(f1);
nmat = repmat(n',1,lf);

r=zeros(ln,lf,N);
Po=zeros(ln,lf,N);
dlbgMat=zeros(ln,lf,N);
maskdl=zeros(ln,lf,N);
for jj=1:N  % Over 4 states
    lbgMat = repmat(lbg(:,jj)',ln,1);                           % Intensity of the Poisson
    maskdl(:,:,jj)=lbgMat>precMask;                             % Restriction to non-zero lambda (over space)
    dlbgMat(:,:,jj)= repmat(gradient(lbg(:,jj),dx)',ln,1);      % Derivative of the intensity of the Poisson
    r(:,:,jj)=(nmat-lbgMat)./lbgMat;                            % Linear ramp with offset and scaling
    Po(:,:,jj)=poissonpdfmulti(n,lbg(:,jj));                    % Poisson distribution
end

rPo=Po.*r.*maskdl;
sPo=sum(Po,3);      % Denominator term...
% sPo=sum(Po(:,:,1:3),3);


It11=dlbgMat(:,:,1).^2.*sum(rPo(:,:,[1,3]),3).^2;
It22=dlbgMat(:,:,2).^2.*sum(rPo(:,:,[2,3]),3).^2;
It12=dlbgMat(:,:,1).*dlbgMat(:,:,2).*sum(rPo(:,:,[1,3]),3).*sum(rPo(:,:,[2,3]),3);
% just testng something
% warning('just testng something in numericalMeanEstimation.m !!!')
% It11=sum(rPo(:,:,[1,3]),3).^2;
% It22=sum(rPo(:,:,[2,3]),3).^2;
% It12=sum(rPo(:,:,[1,3]),3).*sum(rPo(:,:,[2,3]),3);


mask = abs(It11)>precMask;
I(1,1)=1/4*dx*trapz(It11(mask)./sPo(mask));
mask = abs(It22)>precMask;
I(2,2)=1/4*dx*trapz(It22(mask)./sPo(mask));
mask = abs(It12)>precMask;
I(1,2)=1/4*dx*trapz(It12(mask)./sPo(mask));
I(2,1)=I(1,2); 