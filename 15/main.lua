tilesize = 16

world1 = {
    "                                        ",
    "                                        ",
    "                                        ",
    "   78888888888888888888888888888888889  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555555555555555555555555555555556  ",
    "   45555556    45555555555555555555556  ",
    "   45555556    45555555555555555555556  ",
    "   45555556    45555555555555555555556  ",
    "   45555556    45555555555555555555556  ",
    "   45555556    45555555555555555555556  ",
    "   45555556    45555555555555555555556  ",
    "   45555556    45555555555555555555556  ",
    "   45555556    45555555555555555555556  ",
    "   12222223    12222222222222222222223  ",
    "   zxxxxxxc    zxxxxxxxxxxxxxxxxxxxxxc  ",
    "   ZXXXXXXC    ZXXXXXXXXXXXXXXXXXXXXXC  ",
    "                                        ",
    "                                        ",
    "                                        ",
}

---------------------------------------

function love.load()
    local setsize

    tileset = love.graphics.newImage("tiles_packed_1.png")
    setw = tileset:getWidth()
    seth = tileset:getHeight()

    local T = {}

    T["7"] = {0, 0}
    T["8"] = {1, 0}
    T["9"] = {5, 0}

    T["4"] = {0, 1}
    T["5"] = {4, 1}
    T["6"] = {5, 1}

    T["1"] = {0, 4}
    T["2"] = {1, 4}
    T["3"] = {5, 4}

    T["z"] = {0, 5}
    T["x"] = {1, 5}
    T["c"] = {4, 5}

    T["Z"] = {0, 6}
    T["X"] = {1, 6}
    T["C"] = {4, 6}

    T[" "] = {12, 5}
    T["_"] = {3, 1}
    T["r"] = {3, 2}

    tileQuads = {}
    for k, v in pairs(T) do
        tileQuads[k] = love.graphics.newQuad(v[1] * tilesize, v[2] * tilesize,
        tilesize, tilesize, setw, seth)
    end
end

function love.draw()
    love.graphics.setBackgroundColor(41, 38, 52)
    love.graphics.scale(1)

    for i = 0, 30-1 do
        local r = world1[i+1]

        for j = 0, 40-1 do
            local t = string.sub(r,j+1,j+1)
            if " " ~= t then
                love.graphics.draw(tileset, tileQuads[t], j * tilesize, i * tilesize)
            end
        end
    end
end

function love.keypressed(k)
   if "escape" == k then
      love.event.quit()
   end
end
