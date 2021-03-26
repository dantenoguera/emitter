#version 430

layout (local_size_x = 2, local_size_y = 1, local_size_z = 1) in;

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
    float lt = particles[gid].lt;

    vec3 dir = attr - pos;
    float min_dist = 0.1f;
    float dist = length(dir);

    if (dist < min_dist) {
        dist = min_dist;
    }

    float G = 10000.0f;

    vec3 a = G / (dist * dist) * normalize(dir);
    vel += a * dt;
    pos += vel * dt;

    lt -= 0.008f;

    if (lt < 0.0f)
    {
        float low = -20.0f;
        float high = 20.0f;

        lt = 1.0f;
        pos = vec3(0.0f, 0.5f, 0.0f);
        vel.x = low + random(vel.x) * (high - low);
        vel.y = low + random(vel.y) * (high - low);
        vel.z = low + random(vel.z) * (high - low);
    }


    // Write back
    particles[gid].pos.xyz = pos;
    particles[gid].vel.xyz = vel;
    particles[gid].lt = lt;
}
