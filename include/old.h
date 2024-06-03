#pragma once
#include"Eigen/Dense"
#include<vector>
#include<list>
#include<string>
#include<iostream>
#include"../glm/glm.hpp"
using namespace Eigen;
using std::vector;
using std::list;
using std::string;



#include <random>
Vector2d rand_off() {
    double offs = 0.01;

	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_real_distribution<double> dis(-offs, offs);

    Vector2d ret(dis(eng), dis(eng));
    return ret;
}



const vector<double> CRKc{0,0.5,0.5,1};
const vector<double> CRKb{1.0/6,1.0/3,1.0/3,1.0/6};

struct VortexSegment2D{
    VortexSegment2D(const Vector2d & p, const Vector2d & ve, const Vector3d & c, const double & vo = 0 ):pos{p}, velocity{ve}, color{c}, vortex{vo}{}
    
    Vector2d pos;
    Vector2d velocity;
    double vortex;
    Vector3d color;
};

bool out_of_scr(VortexSegment2D vor) {
    return (vor.pos(0) > 2.5);
}

class VortexSegmentCloud2D{
    public:

    VortexSegmentCloud2D() {}

    VortexSegmentCloud2D( double x, double y ): domainX{x}, domainY(y) {}

    const list<VortexSegment2D> & getSegments (){ return segments; }

    void addSegment( const VortexSegment2D & newSeg ){ segments.push_back(newSeg); }

    void addSegment( const vector<VortexSegment2D> & newSegs ){ for(auto & newSeg: newSegs) segments.push_back(newSeg); }
    
    void addTracer( const VortexSegment2D & newSeg ){ tracer.push_back(newSeg); }

    void addTracer( const vector<VortexSegment2D> & newSegs ){ for(auto & newSeg: newSegs) tracer.push_back(newSeg); }

    void get_seg(std::vector<glm::vec4> & vec_out) {
        std::vector<glm::vec4> new_vec;
        for(auto & seg : segments) {
            new_vec.push_back(glm::vec4(seg.pos(0), seg.pos(1), 0.0, 1.0));
        }
        vec_out.swap(new_vec);
    }
    void get_tracer(std::vector<glm::vec4> & vec_out) {
        std::vector<glm::vec4> new_vec;
        for(auto & seg : tracer) {
            new_vec.push_back(glm::vec4(seg.pos(0), seg.pos(1), 0.0, 1.0));
        }
        vec_out.swap(new_vec);
    }

    vector<glm::vec3> oneStepTemporalEvolution( const double & K, int n = 1){
        double k = K/n;
        vector<glm::vec3> poses;
        // int nt = 5;
        // double radi = 0.04;
        // vector<VortexSegment2D> tra;
        // for(int i=0; i<nt; i++) {
        //     Vector2d new_pt( radi * cos(3.14159 * 2 / nt * i)-1.6, radi * sin(3.14159 * 2 / nt * i));
        //     tra.push_back(VortexSegment2D(new_pt,{0,0},{1,0,0}));
        // }
        for(int times = 0; times < n; times++ ) {
        // if(times%20 ==0) addTracer(tra);
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
            for(auto i = segments.begin(); i != segments.end(); i++ ){
                VortexSegment2D & seg = *i;
                seg.pos = tempP[iter] + k*tempV[iter];
                if( abs(seg.pos(0)) < domainX && abs(seg.pos(1)) < domainY) poses.push_back(glm::vec3{seg.pos(0),seg.pos(1),1});
                else segments.erase(i--);
                iter++;
            }
            for(auto i = tracer.begin(); i != tracer.end(); i++ ){
                VortexSegment2D & seg = *i;
                seg.pos = tempP[iter] + k*tempV[iter];
                if( abs(seg.pos(0)) < domainX && abs(seg.pos(1)) < domainY) poses.push_back(glm::vec3{seg.pos(0),seg.pos(1),1});
                else tracer.erase(i--);
                iter++;
            }
        }
        // for(auto & seg: segments){
        //     seg.pos = tempP[iter] + k*tempV[iter];
        //     poses.push_back(glm::vec3{seg.pos(0),seg.pos(1),1});
        //     iter++;
        // }
        // for(auto & seg: tracer){
        //     seg.pos = tempP[iter] + k*tempV[iter];
        //     poses.push_back(glm::vec3{seg.pos(0),seg.pos(1),1});
        //     iter++;
        // }
        boundaryTreatment();
        std::cout << segments.size() << " "; 
        // for(auto & seg: boundarySegments) poses.push_back(glm::vec3{seg(0),seg(1),1});
        return poses;
    }

    // void setBoundary( const string & filename  ) {} //read a boundary model file and set the Vertexes of model as boundary sample points 

    void setDomain( double x, double y ) { domainX = x; domainY = y; }

    void setBoundary( const vector<Vector2d> & b, const vector<Vector2d> & bs ){
        boundary = b;
        boundarySegments = bs;
        int n = boundary.size();
        int a = boundarySegments.size();
        MatrixXd K = MatrixXd::Zero(2*n,a);
        for( int i = 0; i < 2*n; i += 2 ){
            for( int j = 0; j < a; j++){
                Vector2d temp = boundary[i/2] - boundarySegments[j];
                temp = Vector2d{-temp(1),temp(0)}/(2*M_PI*(temp.squaredNorm()+0.0001*0.0001));
                K(i,j) = temp(0);
                K(i+1,j) = temp(1);
            }
        }
        // B = (K.transpose()*K+3*MatrixXd::Identity(a,a)).inverse()*K.transpose();
        B = (K.transpose()*K+5*MatrixXd::Identity(a,a)).inverse()*K.transpose();
    } 

    void set_back_vel(double x, double y) {
        backgroundVelocity = Vector2d(x, y);
    }


    private:

    Vector2d velocity( const Vector2d & pos ){
        Vector2d u{backgroundVelocity};
        for( auto & seg: segments){
            Vector2d temp = pos - seg.pos;
            u += seg.vortex*Vector2d{-temp(1),temp(0)}/(2*M_PI*(temp.squaredNorm()+0.001*0.001));
        }
        return u;
    }

    void boundaryTreatment( ){
        int n = boundary.size();
        int a = boundarySegments.size();
        VectorXd U = VectorXd::Zero(2*n);
        for(int i = 0; i < 2*n; i+=2){
            Vector2d v = velocity(boundary[i/2]);
            U(i) = -v(0);
            U(i+1) = -v(1);
        }
        VectorXd Gamma = B*U;
        for(int i = 0; i < a; i++ ) {
            Vector2d n_pos = boundarySegments[i];
            // Vector2d n_pos = boundarySegments[i] + rand_off();
            if(abs(Gamma(i)) > 1e-3) segments.push_back(VortexSegment2D(n_pos,Vector2d{0,0},Vector3d{0,0,0},Gamma(i)));
        } 

    }

    list<VortexSegment2D> segments;
    list<VortexSegment2D> tracer;
    vector<Vector2d> boundary;
    vector<Vector2d> boundarySegments;
    Vector2d backgroundVelocity;
    MatrixXd B;
    double domainX;
    double domainY;

};

struct VortexSegment3D{
    VortexSegment3D(const Vector3d & pp, const Vector3d & pn, const Vector3d & veP, const Vector3d & veN,const Vector3d & c, const double & vo = 0 ):posP{pp}, posN(pn), velocityP{veP}, velocityN{veN}, color{c}, vortex{vo}{}
    
    Vector3d posP;
    Vector3d posN;
    Vector3d velocityP;
    Vector3d velocityN;
    double vortex;
    Vector3d color;
};

class VortexSegmentCloud3D{
    
    public:

    VortexSegmentCloud3D() {}

    VortexSegmentCloud3D( double x, double y, double z ): domainX{x}, domainY(y), domainZ{z} {}

    const list<VortexSegment3D> & getSegments (){ return segments; }

    void addSegment( const VortexSegment3D & newSeg ){ segments.push_back(newSeg); }

    void addSegment( const vector<VortexSegment3D> & newSegs ){ for(auto & newSeg: newSegs) segments.push_back(newSeg); }
    
    void addTracer( const VortexSegment3D & newSeg ){ tracer.push_back(newSeg); }
    void addTracer_sec( const VortexSegment3D & newSeg ){ tracer_sec.push_back(newSeg); }


    void addTracer( const vector<VortexSegment3D> & newSegs ){ for(auto & newSeg: newSegs) tracer.push_back(newSeg); }

    vector<glm::vec4> oneStepTemporalEvolution( const double & K, int n = 1){
        double k = K/n;
        vector<glm::vec4> poses;
        for(int times = 0; times < n; times++ ) {
        // if(times%20 ==0) addTracer(tra);
            vector<Vector3d> tempPP;
            vector<Vector3d> tempPN;
            // vector<Vector3d> tempVP{segments.size()+tracer.size(), Vector3d{0,0,0}};
            // vector<Vector3d> tempVN{segments.size()+tracer.size(), Vector3d{0,0,0}};
            
            vector<Vector3d> tempVP{segments.size()+tracer.size() + tracer_sec.size(), Vector3d{0,0,0}};
            vector<Vector3d> tempVN{segments.size()+tracer.size() + tracer_sec.size(), Vector3d{0,0,0}};
            for(auto & seg: segments){ tempPP.push_back(seg.posP);tempPN.push_back(seg.posN);}
            for(auto & seg: tracer) {tempPP.push_back(seg.posP);tempPN.push_back(seg.posN);}
            for(auto & seg: tracer_sec) {tempPP.push_back(seg.posP);tempPN.push_back(seg.posN);}

            for( int i = 0; i < 4; i++ ){
                for(auto & seg: segments) {seg.velocityP = velocity(seg.posP);seg. velocityN = velocity(seg.posN);}
                for(auto & seg: tracer) {seg.velocityP = velocity(seg.posP);seg. velocityN = velocity(seg.posN);}
                for(auto & seg: tracer_sec) {seg.velocityP = velocity(seg.posP);seg. velocityN = velocity(seg.posN);}
                int iter = 0;
                for(auto & seg: segments){
                    tempVP[iter] = tempVP[iter] + CRKb[i]*seg.velocityP;
                    seg.posP = tempPP[iter] + k*CRKc[i]*seg.velocityP;
                    tempVN[iter] = tempVN[iter] + CRKb[i]*seg.velocityN;
                    seg.posN = tempPP[iter] + k*CRKc[i]*seg.velocityN;
                    iter++;
                }
                for(auto & seg: tracer){
                    tempVP[iter] = tempVP[iter] + CRKb[i]*seg.velocityP;
                    seg.posP = tempPP[iter] + k*CRKc[i]*seg.velocityP;
                    tempVN[iter] = tempVN[iter] + CRKb[i]*seg.velocityN;
                    seg.posN = tempPP[iter] + k*CRKc[i]*seg.velocityN;
                    iter++;
                }
                for(auto & seg: tracer_sec){
                    tempVP[iter] = tempVP[iter] + CRKb[i]*seg.velocityP;
                    seg.posP = tempPP[iter] + k*CRKc[i]*seg.velocityP;
                    tempVN[iter] = tempVN[iter] + CRKb[i]*seg.velocityN;
                    seg.posN = tempPP[iter] + k*CRKc[i]*seg.velocityN;
                    iter++;
                }
            }
            int iter = 0;
            for(auto i = segments.begin(); i != segments.end(); i++ ){
                VortexSegment3D & seg = *i;
                seg.posP = tempPP[iter] + k*tempVP[iter];
                seg.posN = tempPN[iter] + k*tempVN[iter];
                Vector3d pos = 0.5*(seg.posP+seg.posN);
                if( abs(pos(0)) < domainX && abs(pos(1)) < domainY && abs(pos(2)) < domainZ ) poses.push_back(glm::vec4{pos(0),pos(1),pos(2),1});
                else segments.erase(i--);
                iter++;
            }
            for(auto i = tracer.begin(); i != tracer.end(); i++ ){
                VortexSegment3D & seg = *i;
                seg.posP = tempPP[iter] + k*tempVP[iter];
                seg.posN = tempPN[iter] + k*tempVN[iter];
                Vector3d pos = 0.5*(seg.posP+seg.posN);
                if( pos(0) < domainX && pos(1) < domainY && pos(2) < domainZ ) poses.push_back(glm::vec4{pos(0),pos(1),pos(2),1});
                else segments.erase(i--);
                iter++;
            }
            for(auto i = tracer_sec.begin(); i != tracer_sec.end(); i++ ){
                VortexSegment3D & seg = *i;
                seg.posP = tempPP[iter] + k*tempVP[iter];
                seg.posN = tempPN[iter] + k*tempVN[iter];
                Vector3d pos = 0.5*(seg.posP+seg.posN);
                if( pos(0) < domainX && pos(1) < domainY && pos(2) < domainZ ) poses.push_back(glm::vec4{pos(0),pos(1),pos(2),1});
                else segments.erase(i--);
                iter++;
            }
        }
        // std::cout << segments.size() << " " << std::endl; 
        return poses;
    }

    void get_seg(std::vector<glm::vec4> & vec_out) {
        std::vector<glm::vec4> new_vec;
        for(auto & seg : segments) {
            new_vec.push_back(glm::vec4((seg.posP(0) + seg.posN(0)) / 2, (seg.posP(1) + seg.posN(1)) / 2, (seg.posP(2) + seg.posN(2)) / 2, 1.0));
        }
        vec_out.swap(new_vec);
    }
    void get_tracer(std::vector<glm::vec4> & vec_out) {
        std::vector<glm::vec4> new_vec;
        for(auto & seg : tracer) {
            new_vec.push_back(glm::vec4(seg.posP(0), seg.posP(1), seg.posP(2), 1.0));
        }
        vec_out.swap(new_vec);
    }
    void get_tracer_sec(std::vector<glm::vec4> & vec_out) {
        std::vector<glm::vec4> new_vec;
        for(auto & seg : tracer_sec) {
            new_vec.push_back(glm::vec4(seg.posP(0), seg.posP(1), seg.posP(2), 1.0));
        }
        vec_out.swap(new_vec);
    }

    void get_seg_p(std::vector<glm::vec4> & vec_out) {
        std::vector<glm::vec4> new_vec;
        for(auto & seg : segments) {
            new_vec.push_back(glm::vec4(seg.posP(0), seg.posP(1), seg.posP(2), 1.0));
        }
        vec_out.swap(new_vec);
    }

    void get_seg_n(std::vector<glm::vec4> & vec_out) {
        std::vector<glm::vec4> new_vec;
        for(auto & seg : segments) {
            new_vec.push_back(glm::vec4(seg.posN(0), seg.posN(1), seg.posN(2), 1.0));
        }
        vec_out.swap(new_vec);
    }



    private:
    Vector3d velocity( const Vector3d & pos ){
        Vector3d u{backgroundVelocity};
        double R = 0.001;
        for( auto & seg: segments){
            Vector3d temp1 = seg.posP - pos;
            Vector3d temp2 = seg.posN - pos;
            Vector3d temp3 = temp2.cross(temp1);
            u += seg.vortex/(4*M_PI)*(temp1/(temp1.lpNorm<2>()+R)-temp2/(temp2.lpNorm<2>()+R)).dot(seg.posP - seg.posN)*temp3/(temp3.squaredNorm() + R*R);
        }
        return u;
    }

    void segTopoChange(){
        //split
        double maxLength = 2;
        for( auto i = segments.begin(); i != segments.end(); i++ ){
            auto & seg = *i;
            if( (seg.posP-seg.posN).lpNorm<2>() > maxLength ) {
                auto newSeg = seg;
                seg.posP = 0.5*(seg.posP+seg.posN);
                newSeg.posN = seg.posN;
                segments.insert(i,newSeg);
            }
        }

        //merge
        double minDistance = 1;
        double theta = 5*M_PI/6;
        for( auto i = segments.begin(); i != segments.end(); i++ ){
            auto & segi = *i;
            for( auto j = ++i; j != segments.end(); j++ ){
                auto & segj = *j;
                auto xi = segi.posP-segi.posN;
                auto xj = segi.posP-segi.posN;
                if( 0.5*(segi.posP+segi.posN-segj.posP+segj.posN).lpNorm<2>() < minDistance &&  xi.dot(xj)/(xi.lpNorm<2>()*xj.lpNorm<2>()) > cos(theta) ){
                    auto pos = 0.25*(segi.posP+segi.posN+segj.posP+segj.posN);
                    double length = 0.5*(xi.norm()+xj.norm());
                    Vector3d gamma = (segi.vortex*xi+segj.vortex*xj)/length;
                    segi.vortex = gamma.norm();
                    gamma = gamma/segi.vortex;
                    segi.posP = pos+0.5*length*gamma;
                    segi.posN = pos-0.5*length*gamma;
                    segments.erase(j--);
                }
            }
        }

        boundaryTreatment();

        //delete
        double minVortex = 1e-3;
        for( auto i = segments.begin(); i != segments.end(); i++ ){
            auto & seg = *i;
            if( seg.vortex < minVortex ) segments.erase(i--);
        }
    }

    void boundaryTreatment() {}

    list<VortexSegment3D> segments;
    list<VortexSegment3D> tracer;
    list<VortexSegment3D> tracer_sec;

    vector<Vector3d> boundary;
    vector<Vector3d> boundarySegments;
    Vector3d backgroundVelocity;
    MatrixXd B;
    double domainX;
    double domainY;
    double domainZ;
};