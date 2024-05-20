
编译：

g++ main.cpp glad.c  -o main.exe -lglfw3 -lgdi32 -lopengl32


### 5.18
`off_screen.cpp`生成 `output.avi`，需要加 opencv 库

### 5.20
`test_3d.cpp`
参数加注释了
鼠标控制镜头还是关着的（开的话把`mouse_callback(`解除注释），但是开了键盘 wasd 移动