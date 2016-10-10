function [ newimg ] = dorectimg( img, params, rowanchs )
%RECTIMG Summary of this function goes here
%   Detailed explanation goes here

newimg = zeros(size(img));
for row = 1:size(img, 1)
    pr = row / size(img, 1);
    x = 1 : size(img, 2);
    x = x / size(img, 2);
    if (pr < rowanchs(1))
        ib1 = 1;
        ib2 = - 1;
    elseif (pr > rowanchs(end))
        ib1 = length(rowanchs);
        ib2 = - 1;
    else 
        for i = 1:length(rowanchs)
            if (pr < rowanchs(i))
                ib1 = i;
                break;
            end
        end
        ib2 = ib1 - 1;
    end
    param = params(ib1, :);
    line1 = param(1) * x.^3 + param(2) * x.^2 + param(3) * x + param(4);
    if (ib2 ~= -1)
        param = params(ib2, :);
        line2 = param(1) * x.^3 + param(2) * x.^2 + param(3) * x + param(4);
        imp2 = (rowanchs(ib1) - pr) / (rowanchs(ib1) - rowanchs(ib2));
        imp1 = (pr - rowanchs(ib2)) / (rowanchs(ib1) - rowanchs(ib2));
        delta = (imp1 * line1 + imp2 *line2) * size(img, 1);
    else
        delta = line1 * size(img, 1);
    end
    delta = row + round(delta);
    delta(delta < 1) = 1;
    delta(delta > size(img, 1)) = size(img, 1);
    for col = 1 : size(img, 2)
        newimg(row, col) = img(delta(col), col);
    end
end

end

