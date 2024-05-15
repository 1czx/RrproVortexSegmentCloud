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

```c++
struct VortexSegment2D{
    VortexSegment2D(const Vector2d & p, const Vector2d & ve, const Vector3d & c, const double & vo = 0 ):pos{p}, velocity{ve}, color{c}, vortex{vo}{}
    
    Vector2d pos;
    Vector2d velocity;
    double vortex;
    Vector3d color;
};
```

初始化时不指定Vortex就是tracer pratical;
```c++
class VortexSegmentCloud2D{
    public:

    const list<VortexSegment2D> & getSegments (){ return segments; }

    void addSegment( const VortexSegment2D & newSeg ){ segments.push_back(newSeg); }

    void addSegment( const vector<VortexSegment2D> & newSegs ){ for(auto & newSeg: newSegs) segments.push_back(newSeg); }
    
    void addTracer( const VortexSegment2D & newSeg ){ tracer.push_back(newSeg); }

    void addTracer( const vector<VortexSegment2D> & newSegs ){ for(auto & newSeg: newSegs) tracer.push_back(newSeg); }

    vector<glm::vec3> oneStepTemporalEvolution( const double & k)

    void setBoundary( const string & filename  ) {} //read a boundary model file and set the Vertexes of model as boundary sample points 

    void setBoundary( const vector<Vector2d> & b ) { boundary = b; } 


    private:

    Vector2d velocity( const Vector2d & pos )

    void boundaryTreatment( ){

    }

    list<VortexSegment2D> segments;
    list<VortexSegment2D> tracer;
    vector<Vector2d> boundary;
    Vector2d backgroundVelocity;

};
```
初时设定完之后 每一帧调用一次oneStepTemporalEvolution(k),k为时间间隔, 可以先试试每秒30帧？ 会更新粒子位置 并返回一个包含所有粒子齐次坐标的`vector<glm::vec3>`



#### added...
leapfog: 编译运行`test.cpp`

`VortexSegmentCloud2D` 加了 `get_seg()`, `get_tracer`，就是取出点来绘制
```c++
    void get_seg(std::vector<glm::vec4> & vec_out) {
        int i=0;
        for(auto & seg : segments) {
            vec_out[i].x = seg.pos(0);
            vec_out[i].y = seg.pos(1);            
            i++;
        }
    }

    void get_tracer(std::vector<glm::vec4> & vec_out) {
        int i=0;
        for(auto & seg : tracer) {
            vec_out[i].x = seg.pos(0);
            vec_out[i].y = seg.pos(1);            
            i++;
        }
    }
```

glad.c 好像跟你的版本不一样，我这边会报错，你改成你的那个
感觉就两种点的话用不着单独传颜色，所以现在是靠区分 `draw()` 和 `draw_vor()`分别画红和黄。（后面需要的话再改）


#### 5.15.23
b_test.cpp