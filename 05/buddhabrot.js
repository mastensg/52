w = window.innerWidth;
h = window.innerHeight;
s = Math.min(w, h);
C.height = s;
C.width = s;
c = C.getContext("2d");

function
dot(x, y)
{
    c.fillStyle = "rgba(0,0,0,0.5)";
    c.fillRect(x, y, 1, 1);
}

function
mandelbrot(x, y, max, f)
{
    a = -2 + 4 * x / s;
    b =  2 - 4 * y / s;

    za = a;
    zb = b;

    for (i = 0; i < max; ++i)
    {
        f(s * (za + 2) / 4, s * (-zb + 2) / 4);

        zaa = za * za;
        zbb = zb * zb;

        if (4 < zaa + zbb)
            break;

        zb = 2 * za * zb + b;
        za = zaa - zbb + a;
    }

    return i;
}

function
background()
{
    max = 128;

    c.fillStyle = "rgba(238,238,238,0.1)";
    c.fillRect(0, 0, s, s);
    c.fillStyle = "rgba(0,0,0,0.4)";

    for (y = 1; y < s; y += 2)
        for (x = 1; x < s; x += 2)
            if (mandelbrot(x, y, max, function() {}) < max)
                c.fillRect(x, y, 1, 1);
}

function
down(e)
{
    state = (state + 1) % 3;
    move(e);
}

function
move(e)
{
    max = 1000;
    px = e.pageX - C.offsetLeft;
    py = e.pageY - C.offsetTop;

    if (state < 1)
        c.putImageData(bg, 0, 0);

    if (state < 2)
        mandelbrot(px, py, max, dot);
}

background();
bg = c.getImageData(0, 0, s, s);

state = 0;

C.onmousemove = move;
C.onmousedown = down;
