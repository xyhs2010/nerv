function [ drows ] = nextcol( src, col, rows, step, varargin)
%NEXTCOL Summary of this function goes here
%   Detailed explanation goes here
drows = zeros(size(rows));
lastrows = -1;
if (nargin > 4)
    lastrows = varargin{1};
end
expes = zeros(10);
for i = 1 : length(rows)
    expL = 0;
    expweight = 0;
    expe = 0;
    if (nargin > 4)
        expes(expL + 1) = rows(i) - lastrows(i);
        expL = expL + 1;
    end
    if (i > 1)
        expes(expL + 1) = drows(i - 1) - rows(i - 1);
        expL = expL + 1;
    end
    if (expL > 0)
        expe = mean(expes(1:expL));
        expweight = 1/(8 * step * step);
    end
    
    row = rows(i);
    drow = nextrow(src, col, row, step, expe, expweight);
    drows(i) = drow;
end

end

function dr = nextrow(src, sc, sr, step, varargin)
dc = sc + step;
minr = 0;
minsum = 200;
expe = 0;
expweight = 0;
if (nargin > 5)
    expe = varargin{1};
    expweight = varargin{2};
end
startr = sr - step;
endr = sr + step;
if (expweight ~= 0)
    startr = round(expe + sr - step / 2);
    endr = round(expe + sr + step / 2);
end
for ir = startr : endr
    if (ir > 0 && ir <= size(src, 1))
        sum = linesum(src, sc, sr, dc, ir) / (255 * (abs(step) + 1));
        if (expweight > 0)
            sum = sum + (ir - sr - expe) ^ 2 * expweight;
        end
        if ( sum <= minsum )
            minsum = sum;
            minr = ir;
        end
    end
end
dr = minr;
if (dr <= 0 || dr > size(src, 1))
    fprintf('nextrow wrong');
end
end