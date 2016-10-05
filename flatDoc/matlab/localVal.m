function [ des ] = localVal( src, rad )
%LOCALVAL Summary of this function goes here
%   Detailed explanation goes here

if (ndims(src) ~= 2)
    fprintf('error, dim should be 2');
    des = 0;
    return;
end
conv = fspecial('average', rad);
meansrc = filter2(conv, src);
des = double(src) - meansrc;
des = des .* des;
des = filter2(conv, des);

end