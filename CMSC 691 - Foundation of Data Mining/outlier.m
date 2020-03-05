function distECA()

%------
% local
%------
[X1, m1, n] = genData(1200, 3, 70);
[X2, m2, n] = genData(1200, 3, 50);
[X3, m3, n] = genData(900, 3, 30);
[X4, m4, n] = genData(1200, 3, 100);
[X5, m5, n] = genData(900, 3, 20);

b = 10;

I1 = computeMutualInformation(X1, m1, n, b);
I2 = computeMutualInformation(X2, m2, n, b);
I3 = computeMutualInformation(X3, m3, n, b);
I4 = computeMutualInformation(X4, m4, n, b);
I5 = computeMutualInformation(X5, m5, n, b);

[V1, Diag1] = eig(I1);
[V2, Diag2] = eig(I2);
[V3, Diag3] = eig(I3);
[V4, Diag4] = eig(I4);
[V5, Diag5] = eig(I5);

%----------------
% global
%----------------

%----------------
%distributed
%----------------
%1 wtavg
%----------------
%m = m1 + m2 + m3 + m4 + m5;
%V = (m1 / m) * V1 + (m2 / m) * V2 + (m3 / m) * V3 + (m4 / m) * V4 + (m5 / m) * V5;
%projection
%U = V(:, 2:3); 
  
%----------------
%2 svd
%----------------
%A = [V1 V2 V3 V4 V5];
%[U S V] = svd(A);
% projection
%U = U(:, 2:3); 

%A = [V1' V2' V3' V4' V5']';
%[U S V] = svd(A);
% projection
%U = V(:, 2:3); 

%----------------
%3 adddecomp
%----------------
I = (m1 * I1 + m2 * I2 + m3 * I3 + m4 * I4 + m5 * I5) / (m1 + m2 + m3 + m4 + m5);
[V, Diag] = eig(I);
% projection
U = V(:, 2:3); 

subplot(3, 2, 1), plot(X1, X1 * U * U', m1, n);
subplot(3, 2, 2), plot(X2, X2 * U * U', m2, n);
subplot(3, 2, 3), plot(X3, X3 * U * U', m3, n);
subplot(3, 2, 4), plot(X4, X4 * U * U', m4, n);
subplot(3, 2, 5), plot(X5, X5 * U * U', m5, n);

%----------------
% centralized
%----------------
X = [X1' X2' X3' X4' X5']';
m = size(X, 1);
I = computeMutualInformation(X, m, n, b);
[V, Diag] = eig(I);

% projection
U = V(:, 2:3); 

subplot(3, 2, 6), plot(X, X * U * U', m, n);
 
end

function distPCA()

%------
% local
%------
[X1, m1, n] = genData(1200, 3, 70);
[X2, m2, n] = genData(1200, 3, 50);
[X3, m3, n] = genData(900, 3, 30);
[X4, m4, n] = genData(1200, 3, 100);
[X5, m5, n] = genData(900, 3, 20);

C1 = computeCovariance(X1, m1, n);
C2 = computeCovariance(X2, m2, n);
C3 = computeCovariance(X3, m3, n);
C4 = computeCovariance(X4, m4, n);
C5 = computeCovariance(X5, m5, n);

[V1, Diag1] = eig(C1);
[V2, Diag2] = eig(C2);
[V3, Diag3] = eig(C3);
[V4, Diag4] = eig(C4);
[V5, Diag5] = eig(C5);

%----------------
% global
%----------------

%----------------
%distributed
%----------------
%1 wtavg
%----------------
%m = m1 + m2 + m3 + m4 + m5;
%V = (m1 / m) * V1 + (m2 / m) * V2 + (m3 / m) * V3 + (m4 / m) * V4 + (m5 / m) * V5;
%projection
%U = V(:, 2:3); 
  
%----------------
%2 svd
%----------------
%A = [V1 V2 V3 V4 V5];
%[U S V] = svd(A);
% projection
%U = U(:, 2:3); 

%A = [V1' V2' V3' V4' V5']';
%[U S V] = svd(A);
% projection
%U = V(:, 2:3); 

%----------------
%3 adddecomp
%----------------
C = (m1 * C1 + m2 * C2 + m3 * C3 + m4 * C4 + m5 * C5) / (m1 + m2 + m3 + m4 + m5);
[V, Diag] = eig(C);
% projection
U = V(:, 2:3); 

subplot(3, 2, 1), plot(X1, X1 * U * U', m1, n);
subplot(3, 2, 2), plot(X2, X2 * U * U', m2, n);
subplot(3, 2, 3), plot(X3, X3 * U * U', m3, n);
subplot(3, 2, 4), plot(X4, X4 * U * U', m4, n);
subplot(3, 2, 5), plot(X5, X5 * U * U', m5, n);

%----------------
% centralized
%----------------
X = [X1' X2' X3' X4' X5']';
m = size(X, 1);
C = computeCovariance(X, m, n);
[V, Diag] = eig(C);

% projection
U = V(:, 2:3); 

subplot(3, 2, 6), plot(X, X * U * U', m, n);
 
end

function outlier1(b)

[X, m, n] = genData(5100, 3, 100);
subplot(2,1,1), outlierECA(X, m, n, b);
subplot(2,1,2), outlierPCA(X, m, n, b);

end

function outlierECA(X, m, n, b)

I = computeMutualInformation(X, m, n, b);

% eigen deomposition
[V, Diag] = eig(I);

% projection
U = V(:, 2:3); 
Y = X * U * U';

plot(X, Y, m, n);

end

function outlierPCA(X, m, n, b)

C = computeCovariance(X, m, n);

[V, Diag] = eig(C);

% projection
U = V(:, 2:3); 
Y = X * U * U';

plot(X, Y, m, n);

end


function [X, m, n] = genData(m, n, k)
X = -250 + 100 * randn((m - k) / 5, n);

%Z = 100 + 25 * randn((m - k) / 40, n);
%Z1 = [X' Z'];
%X = Z1';

%Z = 250 + 25 * randn(3 * (m - k) / 40, n);
%Z1 = [X' Z'];
%X = Z1';

Z = 500 + 50 * randn(3 * (m - k) / 10, n);
Z1 = [X' Z'];
X = Z1';

Z = 1000 + 100 * randn((m - k) / 2, n);
Z1 = [X' Z'];
X = Z1';

Z = 750 + 500 * randn(k, n);
Z1 = [X' Z'];
X = Z1';

X = zscore(X); % normalize
end

function [I] = computeMutualInformation(X, m, n, b)
% entropy
P = D(X, b);	% hist
H = zeros(1, n);
for j = 1 : n
	for k = 1 : b
		H(j) = H(j) - P(k, j) * Log(P(k, j));
	end
end

% joint entropy
JH = zeros(n, n);
for i = 1 : n
	for j = 1 : n
		JP = JD(X, i, j, b); % joint distribution
		for ik = 1 : b
			for jk = 1 : b
				JH(i, j) = JH(i, j) - JP(ik, jk) * Log(JP(ik, jk));
			end
		end			
	end
end

% matrix I
I = zeros(n, n);
for i = 1 : n
	I(i, i) = H(i);
end
for i = 1 : n
	for j = i + 1 : n
		I(i, j) = H(i) + H(j) - JH(i, j);
		I(j, i) = I(i, j);
	end
end
end

function [C] = computeCovariance(X, m, n)
C = zeros(n, n);
for i = 1 : n	% cov(X)
	for j = i : n
		for k = 1 : m 
			C(i, j) = C(i, j) + X(k, i) * X(k, j);
		end			
		C(i, j) = C(i, j) ./ (m - 1);
		C(j, i) = C(i, j);
	end
end
end

% find distribution -- my implementation of hist
function [P] = D(X, b)
	m = size(X, 1);
	n = size(X, 2);
	Prob = zeros(b, n);
	minX = min(min(X));
	maxX = max(max(X));
	iX = (maxX - minX) / b;
	bMin = zeros(1, b);
	bMin(1) = minX;
	for k = 2 : b
		bMin(k) = bMin(k - 1) + iX;
	end
	for i = 1 : m	% can be calculated more efficiently if sorted
		for j = 1 : n
			for k = 1 : b - 1
				if X(i, j) >= bMin(k) & X(i, j) < bMin(k + 1)
					Prob(k, j) = Prob(k, j) + 1;
				end	
			end	
			if X(i, j) >= bMin(b)
				Prob(b, j) = Prob(b, j) + 1;
			end	
		end
	end
	for j = 1 : n
		for k = 1 : b
			Prob(k, j) = Prob(k, j) ./ m;
		end	
	end
	P = Prob;
end

% find joint distribution -- my implementation of joint hist
function [JP] = JD(X, i, j, b)
	m = size(X, 1);
	n = size(X, 2);
	Prob = zeros(b, b);
	minX = min(min(X));
	maxX = max(max(X));
	iX = (maxX - minX) / b;
	bMin = zeros(1, b);
	bMin(1) = minX;
	for k = 2 : b
		bMin(k) = bMin(k - 1) + iX;
	end
	for k = 1 : m
		for ki = 1 : b
			for kj = 1 : b
				% if tuple values fall in ki & kj th bin
				if X(k, i) >= bMin(ki) & (ki == b | X(k, i) < bMin(ki + 1)) & X(k, j) >= bMin(kj) & (kj == b | X(k, j) < bMin(kj + 1))
					Prob(ki, kj) = Prob(ki, kj) + 1;
				end	
			end	
		end
	end
	for ki = 1 : b
		for kj = 1 : b
			Prob(ki, kj) = Prob(ki, kj) ./ m;
		end	
	end
	JP = Prob;
end

function [L] = Log(X)
	lg2 = 0;
	if X > 0
		lg2 = log2(X);
	end	
	L = lg2;
end

function plot(X, Y, m, n) 
Z = zeros(m, 1);
for i=1:m
	for j=1:n
		Z(i) = Z(i) + (X(i, j) - Y(i, j)) * (X(i, j) - Y(i, j));
	end;	
end;

color = zeros(m, 3);
for i = 1 : m
	color(i, :) = [1 - Z(i)/max(Z) 0 1 - min(Z) / Z(i)];
end;
scatter3(X(:, 1), X(:, 2), X(:, 3), 15, color, 'p')
end