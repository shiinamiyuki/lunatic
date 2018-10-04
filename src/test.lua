--
-- Created by IntelliJ IDEA.
-- User: xiaoc
-- Date: 2018/8/16
-- Time: 15:24
-- To change this template use File | Settings | File Templates.
--

point = {}
function point.create(x, y)
    local p = {}
    setmetatable(p, point)
    p.x = x
    p.y = y
    return p
end

function point:setPos(x, y)
    self.x = x
    self.y = y
end

function point:paintGL()
    gl.Begin(GL_POINTS)
    gl.Color3d(1.0, 1.0, 1.0)
    gl.Vertex3d(self.x, self.y, 0.0)
    gl.End()
end

scene = {}
k = 100
function scene.create()
    local s = table.clone(scene)
    local i = 0
    local a = 0
    s.points = {}
    while i < k do
        s.points[i] = point.create(math.sin(a) * 0.5, math.cos(a) * 0.5)
        i = i + 1
        a = a + 2 * 3.1415 / 100
    end
    s.t = 0
    return s
end

function scene:paint()
    local i = 0
    local a = self.t
    local x = 0.4
    local dx = 1 / k
    while i < k do
        self.points[i]:setPos(x*math.sin(a) * 0.5, x*math.cos(a) * 0.5)
        i = i + 1
        a = a + 4 * 3.1415 / k
        x = x + dx
    end
    self.t = self.t + 0.01
end

function main()
    local window = glfw.CreateWindow(640, 480, "Hello World")
    print('created')
    glfw.MakeContextCurrent(window)
    if not window then
        glfw.Terminate()
        return -1
    end
    gl.Viewport(0.0, 0.0, 640.0, 480.0)
    gl.Scaled(1.0, 640.0 / 480.0, 1.0)
    local s = scene.create()
    local i = 0
    while not glfw.WindowShouldClose(window) do
        gl.Clear(GL_COLOR_BUFFER_BIT)
        s:paint()
        glfw.SwapBuffers(window)
        glfw.PollEvents()
        Sleep(1)
        i = i + 1
    end
    glfw.Terminate()
end
function foo()
    local p = point.create(1,1.36)
    local x = 0.5
    local a = 0.74
    p:setPos(x*math.sin(a) * 0.5, x*math.cos(a) * 0.5)
    print(p)
end
main()