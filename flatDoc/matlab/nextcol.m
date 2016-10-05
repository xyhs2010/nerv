function [ drows ] = nextcol( src, col, rows, step )
%NEXTCOL Summary of this function goes here
%   Detailed explanation goes here
drows = zeros(size(rows));
for i = 1 : length(rows)
    row = rows(i);
    drow = nextrow(src, col, row, col + step);
    drows(i) = drow;
end

end

function dr = nextrow(src, sc, sr, dc)
minr = 0;
minsum = 255 * (dc - sc);
for ir = sr - (dc - sc) : sr + (dc - sc)
    sum = linesum(src, sc, sr, dc, ir);
    if ( sum < minsum )
        minsum = sum;
        minr = ir;
    end
end
dr = minr;
end