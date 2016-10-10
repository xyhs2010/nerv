function [newimg, rectlines] = rectimg(img)
MAXL = 300;
COL_STEP = 10;
MIN_POINT_THRED = 200;

if (length(size(img)) == 3)
    img = rgb2gray(img);
end
rate = MAXL / max(size(img));
img0 = double(imresize(img, rate));
img0 = 255 - (37/(255-mean(img0(:)))) * (255 - img0);

vffts = zeros(46, 3);
for i = 1:3
    h = round((1/4 + i/8) * size(img0, 2));
    line = zscore(img0(:, h));
    tmp = abs(fft(line));
    vffts(:, i) = tmp(20:65) - tmp(19:64);
end

hffts = zeros(46, 3);
for i = 1:3
    v = round((1/4 + i/8) * size(img0, 1));
    line = zscore(img0(v, :));
    tmp = abs(fft(line'));
    hffts(:, i) = tmp(20:65) - tmp(19:64);
end

if (mean(abs(hffts(:))) > mean(abs(vffts(:))))
    img = imrotate(img, 90);
    img0 = imrotate(img0, 90);
    vffts = hffts;
end

img1 = img0;

% obtain startr
startr = round(size(img1, 2)/2);
for i = 1:size(img1, 2)/2
    line = zscore(double(img0(:, i)));
    tmp = abs(fft(line));
    vfft = tmp(20:65) - tmp(19:64);
    if (mean(abs(vfft(:))) > mean(abs(vffts(:))))
        startr = i;
        break;
    end
end

% init
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

% vetical polifit
for k = 2 : size(keypoints, 2)
    line = keypoints(:, k);
    valid = find(line > 0);
    if (length(valid) > 4)
        x = keypoints(valid, 1);
        fitres = polifitline(x, line(line > 0));
        keypoints(valid, k) = fitres;
    end
end

paintpoints = round(keypoints);

% importance rect
wrect = ones(size(keypoints));
wrect(keypoints < 0) = 0;
for i = 1 : size(keypoints, 1)
    for j = 2 : size(keypoints, 2) - 1
        if (keypoints(i, j) < 0)
            wrect(i, j-1) = 0.1;
            if (j > 2)
                wrect(i, j-2) = 0.5;
            end
            break;
        end
    end
end

for row = 1 : size(keypoints, 1)
    for col = 2 : size(keypoints, 2)
        if (wrect(row, col) < 1)
            keypoints(row, col) = rectinterp(row, col, keypoints, wrect);
            if (keypoints(row, col)<0)
                keypoints(row, col:end) = -1;
                break;
            end
        end
    end
end

% polyfit
originpoints = keypoints;
params = -ones(size(keypoints, 1), 4);
tmp = zeros(size(keypoints, 1), 1);
for i = 1:size(keypoints, 1)
    if (keypoints(i, round(size(keypoints, 2)/2)) > 0)
        tmp(i) = 1;
        x = startr : COL_STEP : size(img1, 2);
        x = x / size(img1, 2);
        line = originpoints(i,:);
        [fitres, params(i, :)] = polifitline(x(line > 0), (line(line > 0) - line(1)) / size(img1, 1));
        keypoints(i,1:length(fitres)) = fitres * size(img1, 1) + originpoints(i, 1); 
    end
end
params = params(tmp > 0, :);
rowanchs = keypoints(tmp > 0,1) / size(img1, 1);

newimg = dorectimg(double(img), params, rowanchs);
% newimg = dorectimg(img1, params, rowanchs);
newimg = uint8(newimg);

% % get rid of bad point
% for k = 2 : size(keypoints, 2)
%     col = keypoints(:, k);
%     index = find(col>0);
%     m = col(index) - keypoints(index, k-1);
%     x = keypoints(index);
%     p = polyfit(x, m, 1);
%     m1 = p(1)*x + p(2);
%     bps = find(abs(m1 - m) > 2.5);
%     if (k == 12)
%         fprintf('');
%     end
%     if (~isempty(bps))
%         for i = 1:length(bps)
%             originpoints(index(i), k:end) = -1;
%             if (originpoints(index(i), 3) > 0)
%                 keypoints(index(i),:) = polifitline(originpoints(index(i),:));
%             end
%         end
%     end
% end

% paint
paintpoints(paintpoints < 1 & paintpoints >= 0) = 1;
paintpoints(paintpoints > size(img1, 1)) = size(img1, 1);
for k = 1:size(paintpoints, 2) - 1
    r = startr + COL_STEP * (k - 1);
    points = paintpoints(:, k);
    points1 = paintpoints(:, k+1);
    for i = 1: length(points)
        if (points1(i) < 0 || points(i) < 0)
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
rectlines = uint8(img1);
end
