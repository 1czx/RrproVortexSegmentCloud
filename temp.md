没必要看的，边写边打的草稿。


screen space fluid rendering
marching cube --> surface

总之，先绘制出来点
至于其他的渲染之类的，以后有空再说。（反正一般情况下也需要单独把结果送到其他的里面渲染。。。）

### els
N-body: for each particle, consider the influence of all the rest

// another
git clone https://github.com/ZeusYang/FluidEngine.git --recursive



g++ main.cpp glad.c  -o main.exe -lglfw3 -lgdi32 -lopengl32


### from 2021 paper code:
just copy the calculation from the papercode
we need to rewrite, later



### point sprite

use point sprite to draw points
(and can be rendered later, but maybe useless...)

still need VAO / VBO, but somehow different
VAO: just gen
VBO: bind with `vector<glm::vec4>& position` (as the position of all points?s)
        or, in practice, get vbo as parameter `setParticleVBO`(`fluid->getPosVBO`), bind    buffer each time when render...
        we can bind buffer to data in float*, same way

when render: bind VAO, drawArrays(GL_POINTS)



// let's talk about how to integrate the work to GLSL
// or you'll need CUDA...
onAdvanceTimeStep() {
        begin:  clear all, numberOfParticles kept (tracers)

        timeIntegration(dt):    update pos / vel of vortex (from all the vortex)
                                solve the tracer        
        emitParticlesFromxxx    emit (limit < 100000), moving panel, 


        end:    naive swap buffer(velocity, position)(of tracers)
}
