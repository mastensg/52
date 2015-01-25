import sys

from numpy import *
import numpy.matlib as matlib

def eprint(*v):
    sys.stderr.write(" ".join(map(str, v)) + "\n")

class Image:
    def __init__(self, w, h):
        self.h, self.w = h, w
        self.pixels = zeros((h, w, 3), uint8)
        self.S = translate(w/2, h/2, 1) * scale(w/2, -h/2, 1)

    def totga(self):
        h, w = self.h, self.w

        header = array([0, 0, 2, 0, 0, 0, 0, 0, 0, 0, h % 256, h / 256,
            w % 256, w / 256, h % 256, h / 256, 24, 32], uint8)

        return header.tostring() + self.pixels.tostring()

    def vertex(self, pos, col=255):
        o = self.S * pos
        x, y, _, _ = map(int, o)

        if not ((0 <= y < self.h) and (0 <= x < self.w)):
            return

        self.pixels[y][x] = col

def scale(x, y, z):
    return matrix([[x,0,0,0],
                   [0,y,0,0],
                   [0,0,z,0],
                   [0,0,0,1]])

def translate(x, y, z):
    T = matlib.identity(4)
    T[3] = x, y, z, 1
    return T.T

def project(v):
    V = matrix(v)
    V[0][0] /= V[2][0]
    V[1][0] /= V[2][0]
    return V

def vec(*c):
    return matrix(c).T

def mapmul(A, Bs):
    return [A * B for B in Bs]

def draw_points(M, ps, cs=None):
    vs = map(project, mapmul(M, ps))

    for i in xrange(len(vs)):
        v = vs[i]
        c = cs[i] if cs else 255
        fb.vertex(v, c)

fb = Image(800,800)

if 0:
    cube = array([
            vec(-1,-1,-1, 1),
            vec(-1,-1, 1, 1),
            vec(-1, 1,-1, 1),
            vec(-1, 1, 1, 1),
            vec( 1,-1,-1, 1),
            vec( 1,-1, 1, 1),
            vec( 1, 1,-1, 1),
            vec( 1, 1, 1, 1),
            ])

    draw_points(translate(2, -2, 4) * scale(.25, .25, .25), cube)
else:
    sphere = []
    colors = []
    I = 32
    J = 4 * I
    for i in xrange(I):
        s = pi * i / I
        y = cos(s)
        r = sin(s)
        for j in xrange(J):
            t = 2 * pi * (j-J/2.) / (J/2.)
            x = r * cos(t)
            z = r * sin(t)
            v = vec(x,y,z,1)
            sphere.append(v)
            c = 127 * (float(v.T * vec(0,0,-.8,1)))
            colors.append(c)

    draw_points(translate(0, 0, 2), sphere, colors)

sys.stdout.write(fb.totga())
