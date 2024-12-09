





def bresenhamsLineAlgorithm(x0, y0, x1, y1):
    pixels_coords = []
    dx = abs(x1 - x0)
    dy = abs(y1 - y0)
    x, y = x0, y0
    sx = -1 if x0 > x1 else 1
    sy = -1 if y0 > y1 else 1
    if dx > dy:
        err = dx / 2.0
        print(dx, dy)
        print('first err:=', err)
        while x != x1:
            pixels_coords.append((x, y))
            print(' err1 =', err)
            err -= dy
            print(' err2 =', err, '-dy', dy)
            if err < 0:
                y += sy
                print('berr1 =', err)
                err += dx
                print('berr2 =', err, '+dx', dx)
            x += sx
            print('----')
    else:
        err = dy / 2.0
        while y != y1:
            pixels_coords.append((x, y))
            err -= dx
            if err < 0:
                x += sx
                err += dy
            y += sy
    pixels_coords.append((x, y))
    return pixels_coords





coords = bresenhamsLineAlgorithm(0, 0, 31, 10)
print(len(coords))
