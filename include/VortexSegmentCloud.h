#pragma once
#include"Eigen/Dense"
#include<vector>
#include<list>
#include<string>
#include"../glm/glm.hpp"
using namespace Eigen;
using std::vector;
using std::list;
using std::string;

const vector<double> CRKc{0,0.5,0.5,1};
const vector<double> CRKb{1.0/6,1.0/3,1.0/3,1.0/6};

struct VortexSegment2D{
    VortexSegment2D(const Vector2d & p, const Vector2d & ve, const Vector3d & c, const double & vo = 0 ):pos{p}, velocity{ve}, color{c}, vortex{vo}{}
    
    Vector2d pos;
    Vector2d velocity;
    double vortex;
    Vector3d color;
};

class VortexSegmentCloud2D{
    public:

    const list<VortexSegment2D> & getSegments (){ return segments; }

    void addSegment( const VortexSegment2D & newSeg ){ segments.push_back(newSeg); }

    void addSegment( const vector<VortexSegment2D> & newSegs ){ for(auto & newSeg: newSegs) segments.push_back(newSeg); }
    
    void addTracer( const VortexSegment2D & newSeg ){ tracer.push_back(newSeg); }

    void addTracer( const vector<VortexSegment2D> & newSegs ){ for(auto & newSeg: newSegs) tracer.push_back(newSeg); }

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

    vector<glm::vec3> oneStepTemporalEvolution( const double & k){
        // boundaryTreatment();
        vector<glm::vec3> poses;
        vector<Vector2d> tempP;
        vector<Vector2d> tempV{segments.size()+tracer.size(), Vector2d{0,0}};
        for(auto & seg: segments){ tempP.push_back(seg.pos);}
        for(auto & seg: tracer) tempP.push_back(seg.pos);
        for( int i = 0; i < 4; i++ ){
            for(auto & seg: segments) seg.velocity = velocity(seg.pos);
            for(auto & seg: tracer) seg.velocity = velocity(seg.pos);
            int iter = 0;
            for(auto & seg: segments){
                tempV[iter] = tempV[iter] + CRKb[i]*seg.velocity;
                seg.pos = tempP[iter] + k*CRKc[i]*seg.velocity;
                iter++;
            }
            for(auto & seg: tracer){
                tempV[iter] = tempV[iter] + CRKb[i]*seg.velocity;
                seg.pos = tempP[iter] + k*CRKc[i]*seg.velocity;
                iter++;
            }
        }
        int iter = 0;
        for(auto & seg: segments){
            seg.pos = tempP[iter] + k*tempV[iter];
            poses.push_back(glm::vec3{seg.pos(0),seg.pos(1),1});
            iter++;
        }
        for(auto & seg: tracer){
            seg.pos = tempP[iter] + k*tempV[iter];
            poses.push_back(glm::vec3{seg.pos(0),seg.pos(1),1});
            iter++;
        }
        return poses;
    }

    // void setBoundary( const string & filename  ) {} //read a boundary model file and set the Vertexes of model as boundary sample points 

    // void setBoundary( const vector<Vector2d> & b ){
    //     boundary = b;
    //     int n = boundary.size();
    //     int a = boundarySegments.size();
    //     MatrixXd K = MatrixXd::Zero(2*n,a);
    //     for( int i = 0; i < 2*n; i += 2 ){
    //         for( int j = 0; j < a; j++){
    //             Vector2d temp = boundary[i/2] - boundarySegments[j];
    //             temp = Vector2d{-temp(1),temp(0)}/(2*M_PI*(temp.squaredNorm()+0.0001*0.0001));
    //             K(i,j) = temp(0);
    //             K(i+1,j) = temp(1);
    //         }
    //     }
    //     B = (K.transpose()*K+0.00001*MatrixXd::Identity(a,a)).inverse()*K.transpose();
    // } 


    private:

    Vector2d velocity( const Vector2d & pos ){
        Vector2d u{backgroundVelocity};
        for( auto & seg: segments){
            Vector2d temp = pos - seg.pos;
            u += seg.vortex*Vector2d{-temp(1),temp(0)}/(2*M_PI*(temp.squaredNorm()+0.1*0.1));
        }
        return u;
    }

    // void boundaryTreatment( ){
    //     int n = boundary.size();
    //     int a = boundarySegments.size();
    //     VectorXd U = MatrixXd::Zero(2*n);
    //     for(int i = 0; i < 2*n; i+=2){
    //         Vector2d v = velocity(boundary[i/2]);
    //         U(i) = -v(0);
    //         U(i+1) = -v(1);
    //     }
    //     VectorXd Gamma = B*U;
    //     for(int i = 0; i < a; i++ ) segments.push_back(VortexSegment2D(boundarySegments[i],Vector2d{0,0},Vector3d{0,0,0},Gamma(i)));
    // }

    list<VortexSegment2D> segments;
    list<VortexSegment2D> tracer;
    vector<Vector2d> boundary;
    vector<Vector2d> boundarySegments;
    Vector2d backgroundVelocity;
    MatrixXd B;

};

class VortexSegment3D{

};

class VortexSegmentCloud3D{

};