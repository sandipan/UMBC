clear;
X = load('table.csv');
m = size(X, 1);
n = size(X, 2);
X = zscore(X);
ilen = 30;
V = zeros(2 * m / ilen, n);
coeff = zeros(n, n);
j = 1;
for i = 1:ilen:m
	coeff = princomp(X(i:i+ilen-1, :));
	V(j, :) = coeff(:, 1);
	V(j + 1, :) = coeff(:, 2);
	j = j + 2;
end
scatter(V(:,1), V(:,2))
save('vec.txt', 'V', '-ASCII');