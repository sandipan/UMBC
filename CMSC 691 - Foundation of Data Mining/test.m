% matlab
% iris data already loaded in X
opts = statset('Display','final');
k = 3;												% # clusters
[idx,ctrs] = kmeans(X,k,...							% kmeans
                    'Distance','sqEuclidean',...	
                    'Replicates',5,...
                    'Options',opts);

plot(X(idx==1,1),X(idx==1,2),'rp','MarkerSize',8)
hold on
plot(X(idx==2,1),X(idx==2,2),'b*','MarkerSize',10)
hold on
plot(X(idx==3,1),X(idx==3,2),'g.','MarkerSize',15)
plot(ctrs(:,1),ctrs(:,2),'kx',...
     'MarkerSize',12,'LineWidth',2)
plot(ctrs(:,1),ctrs(:,2),'ko',...
     'MarkerSize',12,'LineWidth',2)
legend('Cluster 1','Cluster 2','Cluster 3','Centroids',...
       'Location','NW')

% Make a grouped plot of the raw data
load iris
labels = {'Sepal Length','Sepal Width',...
          'Petal Length','Petal Width'};
parallelcoords(meas,'group',species,'labels',labels);

% Plot only the median and quartiles of each group
parallelcoords(meas,'group',species,'labels',labels,... 
               'quantile',.25);

% Matlab
X = zscore(X); % Normalize
[COEFF,SCORE,latent] = princomp(X);	% PCA
A = COEFF(:, 1:2);						
Y = X * A;									% Projection on 2 most dominat PCs
scatter(Y(:,1), Y(:, 2), 15, [1,0,0], 'p')	% scatter plot in the projected space

Xc = Y * A';						% Inverse projection
mean(Xc - X);						% Nearly Zero


X = [
+1, +1, -1, +1, -1, -1, -1, +1, -1
];

N = size(X, 2);

pos = 0;
for i = 1 : N
	if (X(i) == 1)
		pos = pos +1;
	end;
end;
I(pos, N - pos);

a = [

];


% attribute A
% target class C
% split value s
function [G] = Gain(A, C, s)
    nc = size(A, 2); % size of target class labels
    posC = 0;
    for i = 1 : nc
        if (C(i) > 0)
            posC = posC + 1;
        end
    end
    G = I(posC, nc - posC) - E(A, C, s);
end

% attribute A
% target class C
% split value s
function [WH] = E(A, C, s)
    WH = 0;          % initialize weighted average entropy E(A)
    n = size(A, 2);  % size of attribute values
    nc = size(A, 2); % size of corresponding target class labels
    
    if (n == nc)     % must be equal 
        % attribute set A to be split into 2 subsets A1, A2: s being the
        % split point: all tuples with values of A < s will fall in A1,
        % otherwise fall into A2.
        nA1 = 0;
        nA2 = 0;
        posA1 = 0;  % positive class labels in A1
        posA2 = 0;  % positive class labels in A2
        for i = 1 : n
            if (A(i) >= s)
                nA1 = nA1 + 1;  % number of tuples in A1					
                if (C(i) > 0)   % if corresponding class label is positive
                    posA1 = posA1 + 1;
                end
            else
                nA2 = nA2 + 1;  % number of tuples in A2: nA2 = N - nA1
                if (C(i) > 0)      % if corresponding class label is positive
                    posA2 = posA2 + 1;
                end
            end    
        end        
        WH = (nA1 / n) * I(posA1, nA1 - posA1) + (nA2 / n) * I(posA2, nA2 - posA2);
    end
end

% find entropy
function [H] = I(a, b)
    p = a / (a + b);
    n = b / (a + b);
    logp = 0;
    if (p ~= 0)
        logp = log2(p);
    end
    logn = 0;
    if (n ~= 0)
        logn = log2(n);
    end
    H = -p * logp - n * logn
end

a1 = [
	true     true     true     false     false     false     false     true     false
];

a2 = [
	true     true     false     false     true     true		false     false     true
];

C = [			
		+1     +1     -1     	+1    	 	-1    	 -1   		-1     +1     	-1
];

Gain(a1, C, true);	% split point true / false
Gain(a2, C, true);	% split point true / false

a3 = [
	1.0     6.0     5.0     4.0     7.0     3.0		8.0     7.0     5.0
	NaN 	0.0072  0.0728  0.0026  0.0183	0.1427 	0.1020 	0.0183  0.0728
];

N = size(a3, 2);

for i = 1 : N
	Gain(a3, C, a3(i));	% a3 continuous, test every possible split point
end

for i = 1 : N
	testf(a3, C, a3(i))	% a3 continuous, test every possible split point
end


%L2 cache speed, bus speeds, main memory speed, and LI and L2 hit rates

ThitL2 = 1 : 1 : 50;
ThitL1 = 0;
(0 + .02 x (15 + 2 x 3.76 + .2 x 1.5 x (60 + (64 byte/16 byte) x 7.5)))
Tinst = (ThitL1 + MRL1*(ThitL2+TsendL2toL1+ MRL2 x(1 + Fdirty) x (Taccess + TsendRAMtoL2));
Tread = (ThitL1 + MRL1*(ThitL2+TsendL2toL1+ MRL2 x(1 + Fdirty) x (Taccess + TsendRAMtoL2));
Twrite = (Wbstall *(ThitL2+TsendL2toL1+ MRL2 x(1 + Fdirty) x (Taccess + TsendRAMtoL2));
CPI = CPPICPU + (Tinst + fload*Tread + fstores*Twrite)ns * Clockrate;
plot(ThitL2, CPI);

p = 0.001:0.001:0.999;
h1 = -p.*log2(p);
h2 = -(1-p).*log2(1-p);
h = h1 + h2;
plot(p, h1, 'r');
hold on;
plot(p, h2, 'g');
hold on;
plot(p, h, 'b');
legend('plog(1/p)', '(1-p)log(1/(1-p))', 'plog(1/p)+(1-p)log(1/(1-p))');

p = 0.001:0.001:0.999;
h1 = p.*log2(p) ./ (p.*log2(p) + (1-p).*log2(1-p));
h2 = (1-p).*log2(1-p) ./ (p.*log2(p) + (1-p).*log2(1-p));
plot(p, h1, 'r');
hold on;
plot(p, h2, 'g');
legend('%plog(1/p)', '%(1-p)log(1/(1-p))');



