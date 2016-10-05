MAXL = 300;
COL_STEP = 10;
MIN_POINT_THRED = 200;

img = imread('../data/1.jpg');
rate = MAXL / max(size(img));
img0 = rgb2gray(imresize(img, rate));
img1 = img0;
startr = 48;
for r = startr:10:size(img1, 2) - COL_STEP
    if (r == startr)
        points = minpoint(img1(:,startr), MIN_POINT_THRED);
        points1 = nextcol(img1, r, points, COL_STEP);
    else
        lastrows = points;
        points = points1;
        points1 = nextcol(img1, r, points, COL_STEP, lastrows);
    end
    for i = 1: length(points)
        if (points1(i) == -1)
            continue;
        end
        if (points(i) <= 0 || points1(i) <= 0 || points(i) > size(img1, 1) ||  points1(i) > size(img1, 1))
            fprintf('test wrong');
        end
        output = calcline(r, points(i), r + COL_STEP, points1(i));
        for j = 1:length(output)
            img1(output(j, 2), output(j, 1)) = 0;
        end
    end
end
