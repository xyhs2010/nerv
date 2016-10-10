function [newline, param] = polifitline(x, line)
%POLIFITLINE Summary of this function goes here
%   Detailed explanation goes here
    param = polyfit(x, line, 3);
    newline= param(1) * x.^3 + param(2) * x.^2 + param(3) * x + param(4);

end

