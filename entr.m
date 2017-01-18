P = dlmread('Distribution.dat');
n = size(P,1); %размер распределения

p = 0.5; %вероятность

entr1 = 0;
for i = 1:n
    entr1 = entr1 + (i+1) * p * (1-p)^(i-1);
end
entr2 = 0;
for i = 1:n
    entr2 = entr2 + log2(p * (1-p)^(i-1)) * p * (1-p)^(i-1);
end
entr2 = (-1)* entr2;

mas = P';

mas2 = zeros(n,1);
for i = 1:n
    mas2(i) = p * (1-p)^(i-1);
end

hold on;
plot (mas/sum(mas));
plot (mas2);
hold off;


entr1
entr2