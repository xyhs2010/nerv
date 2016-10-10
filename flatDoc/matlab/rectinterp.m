function [ newval ] = rectinterp( row, col, keypoints, wrect )
%RECTINTERP Summary of this function goes here
%   Detailed explanation goes here
ur = findvalid(row, col, 1, wrect);
dr = findvalid(row, col, 0, wrect);
if (ur < 0 && dr < 0)
    newval = keypoints(row, col);
    return;
end
if (ur < 0)
    ur = dr;
    dr = -1;
end
if (dr < 0)
    imp = 2 * wrect(row, col);
    newval = imp * keypoints(row, col) + ...
        (1 - imp) * (keypoints(row, col - 1) + keypoints(ur, col) - keypoints(ur, col - 1));
    return;
end
imp = wrect(row, col);
impu = (keypoints(dr, 1) - keypoints(row, 1)) / (keypoints(dr, 1) - keypoints(ur, 1));
newval = imp * keypoints(row, col) + ...
    (1 - imp) * impu * (keypoints(row, col - 1) + keypoints(ur, col) - keypoints(ur, col - 1)) + ...
    (1-imp) * (1-impu) * (keypoints(row, col - 1) + keypoints(dr, col) - keypoints(dr, col - 1));

end

function nr = findvalid(row, col, ifup, wrect)
d = 1;
if (~ifup)
    d = -1;
end
row = row + d;
if(row < 1 || row > size(wrect, 1))
    nr = -1;
    return;
end
while (wrect(row, col) ~= 1)
    row = row + d;
    if(row < 1 || row > size(wrect, 1))
        nr = -1;
        return;
    end
end
nr = row;
end