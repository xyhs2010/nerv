function [ drows ] = nextcol( src, col, rows, step, varargin)
%NEXTCOL Summary of this function goes here
%   Detailed explanation goes here
drows = zeros(size(rows));
lastrows = -1;
if (nargin > 4)
    lastrows = varargin{1};
end
expes = zeros(10, 2);
for i = 1 : length(rows)
    expL = 0;
    expweight = 0;
    expe = 0;
    if (nargin > 4)
        if (lastrows(i) == -1)
            drows(i) = -1;
            continue;
        end
        expes(expL + 1, 1) = rows(i) - lastrows(i);
        expes(expL + 1, 2) = 1 / 16;
        expL = expL + 1;
    end
    
    j = i - 1;
    while (j > 0)
        if (drows(j) == -1)
            j = j - 1;
        else
            break;
        end
    end
    if (j >= 1)
        expes(expL + 1, 1) = drows(j) - rows(j);
        expes(expL + 1, 2) = (1 / 8) * (size(src, 1) / ((abs(rows(i) - rows(j)) + 1) * length(rows)));
        expL = expL + 1;
    end
    if (expL > 0)
        expweight = sum(expes(1:expL, 2));
        expe = sum(expes(1:expL, 1) .* expes(1:expL, 2)) / expweight;
    end
    
    row = rows(i);
    drow = nextrow(src, col, row, step, expe, expweight);
    if (j >= 1 && drow ~= -1)
        if (drow < drows(j) + 2)
            drow = -1;
        elseif (lastrows ~= -1)
            thred = (lastrows(j) - lastrows(i))^2 / 8 + (lastrows(j) - lastrows(i));
            if (drows(j) - drow > thred)
            	drow = -1;
            end
        end
    end
    drows(i) = drow;
end

end

function dr = nextrow(src, sc, sr, step, varargin)
dc = sc + step;
minr = -1;
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
    if (ir > 0 && ir <= size(src, 1) && sr > 0)
        sum = linesum(src, sc, sr, dc, ir) / (255);
        if (sum > 0.78)
            continue;
        end
        if (expweight > 0)
            sum = sum + ((ir - sr - expe) / step) ^ 2 * expweight;
        end
        if ( sum <= minsum )
            minsum = sum;
            minr = ir;
        end
    end
end
dr = minr;
if (dr == 0 || dr > size(src, 1))
    fprintf('nextrow wrong');
end
end