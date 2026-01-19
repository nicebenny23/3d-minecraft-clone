#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../util/userinput.h"
#include <GLFW/glfw3.h>
#include "../game/time.h"
#include "../game/rigidbody.h"
#include "../world/voxeltraversal.h"
#include "../game/entitystate.h"
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
        lastGroundedTime = CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().now();
        jumpBufferTime = CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().now() - 1.0f;
        slamUsed = false;
    }
};

struct PlayerMovementSys : ecs::System
{
    using Dependencies = TypeList::TypeList<RigidbodySystem>;

    const float coyoteDuration = 0.15f;
    const float bufferDuration = 0.10f;
    const float jumpStrength = 150.0f * 12.0f / 200.0f;
    const float slamStrength = -30.0f;    // tweak for slam speed
    
    virtual void run(ecs::Ecs& ecs)
    {
        auto view = ecs::View<rigidbody, playermovement, playerclimb>(ecs);

        for (auto [body, movement, climb] : view)
        {


            if (isnan(body.owner().get_component<rigidbody>().velocity.x))
            {
                int l = 2;
            }
            float dt = CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().dt;
            timename::time now = CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().now();
            // — horizontal movement unchanged —
            float slowdown = 2.0f;
            float speed = 16.0f;
            float effSpeed = dt * speed * slowdown;

            Vec3 forward = normal({ camera::GetCamFront().x, 0, camera::GetCamFront().z });
            Vec3 right = normal({ camera::GetCamRight().x,   0, camera::GetCamRight().z });

            if (CtxName::ctx.Inp->getKey('w').held) body.velocity += forward * effSpeed;
            if (CtxName::ctx.Inp->getKey('s').held) body.velocity -= forward * effSpeed;
            if (CtxName::ctx.Inp->getKey('d').held) body.velocity += right * effSpeed;
            if (CtxName::ctx.Inp->getKey('a').held) body.velocity -= right * effSpeed;

            if (body.isonground)
            {
				if (!body.owner().has_component<playerclimb>()) {
					int l=1;
				}
                movement.slamUsed = false;
                movement.lastGroundedTime = now;
                movement.has_jumped = false;
            }

            // — stamp jumpBufferTime on press —
            if (CtxName::ctx.Inp->getKey(' ').pressed)
                movement.jumpBufferTime = now;

            // — liquid / rope vertical smoothing (unchanged) —
            bool normalState = true;

            if (body.inliquid)
            {
                normalState = false;
                float targetY = CtxName::ctx.Inp->getKey(userinput::shift_key).held ? -10.0f
                    : CtxName::ctx.Inp->getKey(' ').held ? 10.0f
                    : 0.0f;
                body.velocity.y = lerp(body.velocity.y, targetY, 0.1f);
            }

            if (climb.onrope)
            {
                normalState = false;
                float targetY = CtxName::ctx.Inp->getKey(userinput::shift_key).held ? -5.0f
                    : CtxName::ctx.Inp->getKey(' ').held ? 5.0f
                    : 0.0f;
                body.velocity.y = lerp(body.velocity.y, targetY, 0.1f);
            }

            // — jumping, slam, and sneak-down —
            if (normalState)
            {
                double sinceGround = (now - movement.lastGroundedTime);
                float sinceBuffer = (now - movement.jumpBufferTime);

                // slam: one-time hard downward thrust in mid-air
                if (!body.isonground
                    && CtxName::ctx.Inp->getKey(userinput::shift_key).pressed
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
                    && CtxName::ctx.Inp->getKey(userinput::shift_key).held)
                {
                    body.velocity.y -= effSpeed;
                }
					//body.owner().get_component<ecs::transform_comp>().transform.position += Vec3(3, 0, 0);
				
			
                
            }
        }
    }
};
