function str = comment(str,newcom)

% COMMENT adds new comments to AWG structure
% SF, February 1 2006.

str.comment = [str.comment newcom 13 10];