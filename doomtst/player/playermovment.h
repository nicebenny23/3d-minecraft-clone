

#include "../util/userinput.h"
#include <GLFW/glfw3.h>
#include "../game/time.h"
#include "../game/rigidbody.h"
#include "../world/voxeltraversal.h"
#include "playerclimb.h"

struct playermovement : ecs::component
{
    timename::time lastGroundedTime;
    timename::time jumpBufferTime;
    bool slamUsed;                  // track whether slam was already done this airtime
    bool has_jumped = false;
    void start()
    {
        has_jumped = false;
        lastGroundedTime = world().ensure_resource<timename::TimeManager>().now();
        jumpBufferTime = world().ensure_resource<timename::TimeManager>().now() - 1.0f;
        slamUsed = false;
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
		if (ecs.get_resource<ui::MenuState>().menu_open()) {
			return;
		}

        auto view = ecs::View<physics::rigidbody,playermovement,playerclimb,ecs::world_transform>(ecs);

        for (auto [body, movement, climb,transform] : view)
        {
			userinput::InputManager& man=ecs.get_resource<userinput::InputManager>();
            float dt = ecs.ensure_resource<timename::TimeManager>().dt;
            timename::time now = ecs.ensure_resource<timename::TimeManager>().now();
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
				if (!body.owner().has_component<playerclimb>()) {
				
				}
                movement.slamUsed = false;
                movement.lastGroundedTime = now;
                movement.has_jumped = false;
            }

            // — stamp jumpBufferTime on press —
            if (man.key(' ').pressed)
                movement.jumpBufferTime = now;

            // — liquid / rope vertical smoothing (unchanged) —
            bool normalState = true;

            if (body.inliquid)
            {
                normalState = false;
                float targetY = man.key(userinput::shift_key).held ? -10.0f
                    : man.key(' ').held ? 10.0f
                    : 0.0f;
                body.velocity.y = stn::lerp(body.velocity.y, targetY, 0.1f);
            }

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

                // slam: one-time hard downward thrust in mid-air
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
                    alert("Hershel vs super luigi");
                    movement.jumpBufferTime = now - (bufferDuration + 1.0f); // expire buffer
                }
                // sneak-down on ground
                else if (body.isonground
                    && man.key(userinput::shift_key).held)
                {
                    body.velocity.y -= effSpeed;
                }
				if (man.key('z').held) {
					body.owner().get_component<ecs::world_transform>().transform.position.y = 4;
				}
					if (man.key('f').held) {
					body.owner().get_component<ecs::world_transform>().transform.position += Vec3(4, 0, 0);
				
				}
					
                
            }
        }
    }
};
