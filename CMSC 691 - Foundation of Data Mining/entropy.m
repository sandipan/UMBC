% find distribution -- my implementation of hist
function [Hp] = entropy(X, b)

parallelcoords(X);

X = zscore(X); % Normalize
m = size(X, 1);
n = size(X, 2);
%b = 10;

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

% eigen deomposition
[V, Diag] = eig(I);
% projection
Y = X * V;

%Y = X * [V(:, 1 : 2)] * [V(:, 1 : 2)]';
Y = X * [V(:, 1 : n - 1)] * [V(:, 1 : n - 1)]';
Y = Y - min(min(Y));
Y = round(Y * 128);
size(Y)
dlmwrite('einstein.dat', Y, ' ');

%Y = X * [V(:, n) zeros(n, n - 1)] * [V(:, n) zeros(n, n - 1)]';
%Y = X * [V(:, n - 1) zeros(n, n - 1)] * [V(:, n - 1) zeros(n, n - 1)]';

%subplot(2, 2, 1), scatter(Y(:,3), Y(:, 4), 15, [1,0,0], 'p')	% scatter plot in the projected space
%subplot(2, 2, 2), scatter(Y(:,1), Y(:, 2), 15, [0,1,0], 'p')	% scatter plot in the projected space

%subplot(3, 1, 1), 

%scatter(X(:, n - 1), X(:, n), 15, [1,0,0], 'p')	% scatter plot in the projected space %hold on;

%subplot(3, 1, 2), 

%scatter(Y(:, n - 1), Y(:, n), 15, [1,0,0], 'p')	% scatter plot in the projected space 
%hold on;
%plot([0 V(1, 2)], [0 V(2, 2)])
%hold on;
%plot([0 V(1, 1)], [0 V(2, 1)])

parallelcoords(Y);

%bar(Y)

%C = zeros(n, n);
C = cov(X);
%for i = 1 : n	% cov(X)
%	for j = i : n
%		for k = 1 : m 
%			C(i, j) = C(i, j) + X(k, i) * X(k, j);
%		end			
%		C(i, j) = C(i, j) ./ (m - 1);
%		C(j, i) = C(i, j);
%	end
%end
[V, Diag] = eig(C);
Y = X * V;

Y = X * [V(:, 1 : n - 1)] * [V(:, 1 : n - 1)]';
Y = Y - min(min(Y));
Y = round(Y * 128);
%size(Y)
%dlmwrite('girl.dat', Y, ' ');

%parallelcoords(Y);

%Y = X * [V(:, 1 : 2)] * [V(:, 1 : 2)]';
%Y = X * [V(:, n - 1 : n)] * [V(:, n - 1 : n)]';

%Y = X * [V(:, n) zeros(n, n - 1)] * [V(:, n) zeros(n, n - 1)]';
%Y = X * [V(:, n - 1) zeros(n, n - 1)] * [V(:, n - 1) zeros(n, n - 1)]';
%subplot(2, 2, 3), scatter(Y(:,3), Y(:, 4), 15, [0,0,1], 'p')	% scatter plot in the projected space
%subplot(2, 2, 4), scatter(Y(:,1), Y(:, 2), 15, [1,1,0], 'p')	% scatter plot in the projected space

%subplot(3, 1, 3), 

%scatter(Y(:, n - 1), Y(:, n), 15, [0,0,1], 'p')	% scatter plot in the projected space
%hold on;
%plot([0 V(1, 2)], [0 V(2, 2)])
%hold on;
%plot([0 V(1, 1)], [0 V(2, 1)])

%parallelcoords(Y);

%bar(Y)

Hp = Y;
end

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

%X = -250 + 100 * randn(500, 2);

%Z = 100 + 25 * randn(150, 2);
%Z1 = [X' Z'];
%X = Z1';

%Z = 250 + 25 * randn(200, 2);
%Z1 = [X' Z'];
%X = Z1';

%Z = 500 + 50 * randn(500, 2);
%Z1 = [X' Z'];
%X = Z1';

%Z = 1000 + 100 * randn(1000, 2);
%Z1 = [X' Z'];
%X = Z1';

%Z = 750 + 500 * randn(50, 2);
%Z1 = [X' Z'];
%X = Z1';

%X1 = -250 + 50 * randn(500, 1);
%Y1 = -300 + 100 * randn(200, 1);

%X2 = 100 + 50 * randn(200, 1);
%Y2 = 200 + 25 * randn(500, 1);

%X3 = 250 + 25 * randn(200, 1);
%Y3 = 200 + 50 * randn(200, 1);

%X4 = 500 + 100 * randn(500, 1);
%Y4 = 500 + 25 * randn(200, 1);

%X5 = 1000 + 25 * randn(200, 1);
%Y5 = 1000 + 10 * randn(500, 1);

%X = [X1 Y2];

%Z = [X2 Y3];
%Z1 = [X' Z'];
%X = Z1';

%Z = [X3 Y4];
%Z1 = [X' Z'];
%X = Z1';

%Z = [X4 Y5];
%Z1 = [X' Z'];
%X = Z1';

%Z = [X5 Y1];
%Z1 = [X' Z'];
%X = Z1';

%Z = 750 + 500 * randn(50, 2);
%Z1 = [X' Z'];
%X = Z1';

%X = -250 + 100 * randn(500, 20);

%Z = 100 + 25 * randn(150, 20);
%Z1 = [X' Z'];
%X = Z1';

%Z = 250 + 25 * randn(200, 20);
%Z1 = [X' Z'];
%X = Z1';

%Z = 500 + 50 * randn(500, 20);
%Z1 = [X' Z'];
%X = Z1';

%Z = 1000 + 100 * randn(1000, 20);
%Z1 = [X' Z'];
%X = Z1';

%Z = 750 + 500 * randn(50, 20);
%Z1 = [X' Z'];
%X = Z1';



%X = -250 + 100 * randn(500, 3);

%Z = 100 + 25 * randn(150, 3);
%Z1 = [X' Z'];
%X = Z1';

%Z = 250 + 25 * randn(200, 3);
%Z1 = [X' Z'];
%X = Z1';

%Z = 500 + 50 * randn(500, 3);
%Z1 = [X' Z'];
%X = Z1';

%Z = 1000 + 100 * randn(1000, 3);
%Z1 = [X' Z'];
%X = Z1';

%Z = 750 + 500 * randn(50, 3);
%Z1 = [X' Z'];
%X = Z1';

%X = zscore(X);
%m = size(X, 1);
%n = size(X, 2);

%color = zeros(m, 3);
%for i=1:m
%color(i,1) = 1 - Z(i)/max(Z);
%color(i,3)=1 - min(Z)/Z(i);
%end;
%scatter3(X(:, 1), X(:, 2), X(:, 3), 15, color, 'p')