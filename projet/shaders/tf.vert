#version 330 core


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vitesse;


out vec3 outputPosition;
out vec3 outputSpeed;

    //uniform float dt ;
    
float rand2D(in vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{

    float dt = 0.01f;
    float m = 0.05;
    float g = 9.81;
    if(position.y<-1.0f){
        outputSpeed = 2*vec3(rand2D(position.yz)-0.5, rand2D(position.xy)-0.5, rand2D(position.xz)-0.5);
        outputPosition = vec3(0.,0.,0.);
    }
    else{
        outputPosition = position + vitesse*dt;
        outputSpeed = vitesse + m*g*dt*vec3(0.,-1.,0.);
    }
  
}

