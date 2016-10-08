function [newline, param] = polifitline(line)
%POLIFITLINE Summary of this function goes here
%   Detailed explanation goes here
    newline = -ones(size(line));
    y = line(line > 0);
    x = 1 : length(y);
    param = polyfit(x, y, 3);
    y1= param(1) * x.^3 + param(2) * x.^2 + param(3) * x + param(4);
    newline(1 : length(y)) = y1;

end

