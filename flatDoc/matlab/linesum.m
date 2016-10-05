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
a = (sc - dc) / (sr - dr);
b = dc - a * dr;
sum = 0;
for c = sc : dc
    tmp = a * c + b;
    tmpf = floor(tmp);
    sum = sum + (tmp - tmpf) * mat(c, tmpf);
    sum = sum + (tmpf + 1 - tmp) * mat(c, tmpf + iv1);
end

end

