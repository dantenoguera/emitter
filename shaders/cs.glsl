#version 430

layout (local_size_x = 2, local_size_y = 1, local_size_z = 1) in; // ???

struct particle
{
    vec4 pos;
    vec4 vel;
};

layout(std430, binding = 0) buffer Pos {
   particle particles[]; // ???
};

// Frame delta for calculations
uniform float dt;
uniform vec3 attr;

void main() 
{
    // Current SSBO index
    uint gid = gl_GlobalInvocationID.x;

    // Read position and velocity
    vec3 pos = particles[gid].pos.xyz;
    vec3 vel = particles[gid].vel.xyz;

    // Calculate new velocity depending on attraction point
    vel = normalize(attr - pos) * dt;

    // Move by velocity
    pos += vel;

    // Write back
    particles[gid].pos.xyz = pos;
    particles[gid].vel.xyz = vel;
}
