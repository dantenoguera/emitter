#version 430

layout (local_size_x = 16) in;

struct particle
{
    vec4 pos;
    vec4 vel;
    float lt;
};

layout(std430, binding = 0) buffer Pos {
   particle particles[];
};

uniform float dt;
uniform vec3 attr;


// http://amindforeverprogramming.blogspot.com/2013/07/random-floats-in-glsl-330.html
uint hash(uint x) 
{
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

float random(float f) 
{
    const uint mantissaMask = 0x007FFFFFu;
    const uint one          = 0x3F800000u;
   
    uint h = hash( floatBitsToUint( f ) );
    h &= mantissaMask;
    h |= one;
    
    float  r2 = uintBitsToFloat( h );
    return r2 - 1.0;
}

void main() 
{
    uint gid = gl_GlobalInvocationID.x;

    vec3 pos = particles[gid].pos.xyz;
    vec3 vel = particles[gid].vel.xyz;

    vec3 dir = attr - pos;
    float dist = length(dir);

    float min_dist = 0.14f;
    if (dist < min_dist) {
        float low = -0.5f;
        float high = 0.5f;

        pos = vec3(0.0f, 0.5f, 0.0f);

        vel.x = low + random(vel.x) * (high - low);
        vel.y = low + random(vel.y) * (high - low);
        vel.z = low + random(vel.z) * (high - low);
        
        dir = attr - pos;
        dist = length(dir);
    }

    float G = 5.0f;
    vec3 a = G / (dist * dist) * normalize(dir);

    vel += a * dt;
    pos += vel * dt;

    particles[gid].pos.xyz = pos;
    particles[gid].vel.xyz = vel;
    particles[gid].lt = dist;
}
