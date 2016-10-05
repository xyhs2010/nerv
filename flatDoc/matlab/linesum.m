function [ sum ] = linesum( mat, sc, sr, dc, dr )
%LINESUM Summary of this function goes here
%   Detailed explanation goes here
if (sc > dc)
    tmp = sc;
    sc = dc;
    dc = tmp;
    tmp = sr;
    sr = dr;
    dr = tmp;
end
a = (sr - dr) / (sc - dc);
b = dr - a * dc;
sum = 0;
for c = sc + 1 : dc
    tmp = a * c + b;
    tmpf = floor(tmp + 0.001);
    if (tmpf <= 0 || tmpf > size(mat, 1))
        fprintf('something wrong');
    end
    sum = sum + (tmp - tmpf) * double(mat(tmpf, c));
    sum = sum + (tmpf + 1 - tmp) * double(mat(tmpf + 1, c));
end

end

