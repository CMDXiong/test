function str = design(str)

% This function allows an interactive design of AWG layout.
% SF, January 31, 2006

str = update(str);  %fixed parameter update
str = loop(str);    %tweaking three main variables

awgsave(str);       % save AWG design in an associated .mat file



