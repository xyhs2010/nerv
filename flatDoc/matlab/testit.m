img = imread('../data/1.jpg');
rate = 300 / max(size(img));
img0 = rgb2gray(imresize(img, rate));
img1 = img0;
startr = 48;
for r = startr:10:size(img1, 2) - 10
    if (r == startr)
        points = minpoint(img1(:,startr), 200);
        points1 = nextcol(img1, r, points, 10);
    else
        lastrows = points;
        points = points1;
        points1 = nextcol(img1, r, points, 10, lastrows);
    end
    for i = 1: length(points)
        if (points(i) <= 0 || points1(i) <= 0 || points(i) > size(img1, 1) ||  points1(i) > size(img1, 1))
            fprintf('test wrong');
        end
        output = calcline(r, points(i), r + 10, points1(i));
        for j = 1:length(output)
            img1(output(j, 2), output(j, 1)) = 0;
        end
    end
end
