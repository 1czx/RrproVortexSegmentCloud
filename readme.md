
编译：

g++ main.cpp glad.c  -o main.exe -lglfw3 -lgdi32 -lopengl32


### about
never use the glReadPixels, it's not off-screen.

we used PIXEL_BUFFER_OBJ, but why still read_pixels...

draw to a renderbuffer: if you just want to render/read-back. (to a texture if want to re-use)
still...

PIXEL_BUFFER_OBJ to make read pixels asynchronous

