function p = pcheck(p);

% PCHECK checks if a polygon is closed and adds a point if not.

if (p.x(1) ~= p.x(p.n)) | (p.y(1) ~= p.y(p.n))
    p.n = p.n + 1;
    p.x(p.n) = p.x(1);
    p.y(p.n) = p.y(1);
end
