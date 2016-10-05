function [ output ] = calcline( sc, sr, dc, dr )
%CALCLINE Summary of this function goes here
%   Detailed explanation goes here
if (sc > dc)
    tmp = sc;
    sc = dc;
    dc = tmp;
    tmp = sr;
    sr = dr;
    dr = tmp;
end
dest = zeros(2 * (dc - sr), 2);
destL = 0;
a = (sr - dr) / (sc - dc);
b = dr - a * dc;
for c = sc : dc
    tmp = a * c + b + 0.001;
    tmpf = floor(tmp);
    dest(destL + 1, :) = [c; tmpf];
    if (tmpf < sr && tmpf < dr)
        fprintf('calcline wrong');
    end
    destL = destL + 1;
    if (tmp > tmpf)
        dest(destL + 1, :) = [c; tmpf+ 1];
        destL = destL + 1;
    end
end
output = dest(1:destL, :);

end

