

#include "../util/userinput.h"
#include <GLFW/glfw3.h>
#include "../game/time.h"
#include "../game/rigidbody.h"
#include "../world/voxeltraversal.h"
#include "playerclimb.h"
#include "../game/close.h"

struct playermovement : ecs::component
{
    double lastGroundedTime;
    double jumpBufferTime;
    bool slamUsed=false;                  // track whether slam was already done this airtime
    bool has_jumped = false;
	playermovement(timing::WorldClock& clock)
    {
        lastGroundedTime = clock.now();
        jumpBufferTime = clock.now() - 1.0f;
    }
};

struct PlayerMovementSys : ecs::System
{
    const float coyoteDuration = 0.15f;
    const float bufferDuration = 0.10f;
    const float jumpStrength = 150.0f * 12.0f / 200.0f;
    const float slamStrength = -30.0f;  
    
    virtual void run(ecs::Ecs& ecs)
    {
		if (!player::in_game(ecs)) {
			return;
		}

        auto view = ecs::View<physics::RigidBody,playermovement,playerclimb,core::LocalTransform>(ecs);

        for (auto [body, movement, climb,transform] : view)
        {
			userinput::InputManager& man=ecs.get_resource<userinput::InputManager>();
            float dt = ecs.ensure_resource<timing::WorldClock>().dt;
            double now = ecs.ensure_resource<timing::WorldClock>().now();
            // — horizontal movement unchanged —
            float slowdown = 2.0f;
            float speed = 16.0f;
            float effSpeed = dt * speed * slowdown; 
            Vec3 forward = transform.transform.normal_dir().with_y(0).normal();
			Vec3 right = transform.transform.right_dir().with_y(0).normal();
            if (man.key('w').held) body.velocity += forward * effSpeed;
            if (man.key('s').held) body.velocity -= forward * effSpeed;
            if (man.key('d').held) body.velocity += right * effSpeed;
            if (man.key('a').held) body.velocity -= right * effSpeed;

            if (body.isonground)
            {
				
                movement.slamUsed = false;
                movement.lastGroundedTime = now;
                movement.has_jumped = false;
            }

            // — stamp jumpBufferTime on press —
            if (man.key(' ').pressed)
                movement.jumpBufferTime = now;

            // — liquid / rope vertical smoothing (unchanged) —
            bool normalState = true;

            if (climb.onrope)
            {
                normalState = false;
                float targetY = man.key(userinput::shift_key).held ? -5.0f
                    : man.key(' ').held ? 5.0f
                    : 0.0f;
                body.velocity.y = stn::lerp(body.velocity.y, targetY, 0.1f);
            }

            // — jumping, slam, and sneak-down —
            if (normalState)
            {
                double sinceGround = (now - movement.lastGroundedTime);
                float sinceBuffer = (now - movement.jumpBufferTime);

                // slam: one-timing hard downward thrust in mid-air
                if (!body.isonground
                    && man.key(userinput::shift_key).pressed
                    && !movement.slamUsed)
                {
                    body.velocity.y = slamStrength;
                    movement.slamUsed = true;
                }
                // buffered coyote jump
                else if (sinceGround < coyoteDuration
                    && sinceBuffer < bufferDuration&&!movement.has_jumped)
                {
                    movement.has_jumped=true;
                    body.velocity.y = jumpStrength;
                    movement.jumpBufferTime = now - (bufferDuration + 1.0f); // expire buffer
                }
                // sneak-down on ground
                else if (body.isonground
                    && man.key(userinput::shift_key).held)
                {
                    body.velocity.y -= effSpeed;
                }
				if (ecs.get_resource<grid::Grid>().chunks_loaded()==343) {
					//body.owner().get_component<core::LocalTransform>().transform.position += Vec3(32, 0, 0);

				}
				if (man.key('z').held) {
					transform.transform.position.y = 4;
				}
					if (man.key('f').held) {
					transform.transform.position += transform.transform.normal_dir()*4;
				}
					if (man.key('h').held) {
						transform.transform.position += transform.transform.normal_dir() * 10*dt;
					}
					
                
            }
        }
    }
};
