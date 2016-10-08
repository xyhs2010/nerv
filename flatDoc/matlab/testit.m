MAXL = 300;
COL_STEP = 10;
MIN_POINT_THRED = 200;

img = imread('../data/1.jpg');
rate = MAXL / max(size(img));
img0 = rgb2gray(imresize(img, rate));
img1 = img0;

% init
startr = 48;
points = minpoint(img1(:,startr), MIN_POINT_THRED);
keypoints = zeros(size(points, 1), floor((size(img1, 2) - startr) / COL_STEP) + 1);
keypoints(:, 1) = points;

% calc text line
for k = 1 : size(keypoints, 2) - 1
    r = startr + COL_STEP * (k - 1);
    if (r == startr)
        points1 = nextcol(img1, r, points, COL_STEP);
    else
        lastrows = points;
        points = points1;
        points1 = nextcol(img1, r, points, COL_STEP, lastrows);
    end
    keypoints(:, k + 1) = points1;
end

% polyfit
originpoints = keypoints;
for i = 1:size(keypoints, 1)
    if (keypoints(i, 3) > 0)
        keypoints(i,:) = polifitline(originpoints(i,:));
    end
end

% get rid of bad point
for k = 2 : size(keypoints, 2)
    col = keypoints(:, k);
    index = find(col>0);
    m = col(index) - keypoints(index, k-1);
    x = keypoints(index);
    p = polyfit(x, m, 1);
    m1 = p(1)*x + p(2);
    bps = find(abs(m1 - m) > 2.5);
    if (k == 12)
        fprintf('');
    end
    if (~isempty(bps))
        for i = 1:length(bps)
            originpoints(index(i), k:end) = -1;
            if (originpoints(index(i), 3) > 0)
                keypoints(index(i),:) = polifitline(originpoints(index(i),:));
            end
        end
    end
end

% paint
paintpoints = round(keypoints);
paintpoints(paintpoints < 1 & paintpoints > 0) = 1;
paintpoints(paintpoints > size(img1, 1)) = size(img1, 1);
for k = 1:size(paintpoints, 2) - 1
    r = startr + COL_STEP * (k - 1);
    points = paintpoints(:, k);
    points1 = paintpoints(:, k+1);
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
