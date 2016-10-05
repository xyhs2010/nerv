function [ points ] = minpoint( input, thred )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
wholeL = length(input);
divs = zeros(2, wholeL);
divLength = 0;
belowthred = 0;
for i = 1 : wholeL
    if (belowthred > 0)
        if (input(i) > thred)
            divs(2, divLength + 1) = i;
            divLength = divLength + 1;
            belowthred = 0;
        end
    else
        if (input(i) <= thred)
            divs(1, divLength + 1) = i;
            belowthred = 1;
        end
    end
end

points = round(mean(divs(:, 1 : divLength), 1));

end

