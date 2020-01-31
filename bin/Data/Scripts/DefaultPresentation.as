
#include "Scripts/Utilities/Sample.as"

const int CTRL_FORWARD = 1;
const int CTRL_BACK = 2;
const int CTRL_LEFT = 4;
const int CTRL_RIGHT = 8;
const int CTRL_JUMP = 16;

const float MOVE_FORCE = 0.8f;
const float INAIR_MOVE_FORCE = 0.02f;
const float BRAKE_FORCE = 0.2f;
const float JUMP_FORCE = 7.0f;
const float YAW_SENSITIVITY = 0.1f;
const float INAIR_THRESHOLD_TIME = 0.1f;
bool firstPerson = false;

const float CAMERA_MIN_DIST = 1.0f;
const float CAMERA_MAX_DIST = 20.0f;
float cameraDistance = 5.0;

// Slide @slideComp;

VHP @vhpComp;
class Character : ScriptObject {
    VariantMap controlsInput;
    Controls controls;
    bool onGround = false;
    bool okToJump = true;
    float inAirTimer = 0.0f;

    void Start() {
        SubscribeToEvent(node, "NodeCollision", "HandleNodeCollision");
    }

    void Load(Deserializer &deserializer) {
        controls.yaw = deserializer.ReadFloat();
        controls.pitch = deserializer.ReadFloat();
    }

    void Save(Serializer &serializer) {
        serializer.WriteFloat(controls.yaw);
        serializer.WriteFloat(controls.pitch);
    }

    void HandleNodeCollision(StringHash eventType, VariantMap &eventData) {
        VectorBuffer contacts = eventData["Contacts"].GetBuffer();
        while (!contacts.eof) {
            Vector3 contactPosition = contacts.ReadVector3();
            Vector3 contactNormal = contacts.ReadVector3();
            float contactDistance = contacts.ReadFloat();
            float contactImpulse = contacts.ReadFloat();

            // If contact is below node center and pointing up, assume it's a
            // ground contact
            if (contactPosition.y < (node.position.y + 1.0f)) {
                float level = contactNormal.y;
                if (level > 0.75) onGround = true;
            }
        }
    }

    void FixedUpdate(float timeStep) {
        RigidBody @body = node.GetComponent("RigidBody");
        AnimationController @animCtrl =
            node.GetComponent("AnimationController", true);

        // Update the in air timer. Reset if grounded
        if (!onGround)
            inAirTimer += timeStep;
        else
            inAirTimer = 0.0f;
        // When character has been in air less than 1/10 second, it's still
        // interpreted as being on ground
        bool softGrounded = inAirTimer < INAIR_THRESHOLD_TIME;

        // Update movement & animation
        Quaternion rot = node.rotation;
        Vector3 moveDir(0.0f, 0.0f, 0.0f);
        Vector3 velocity = body.linearVelocity;
        // Velocity on the XZ plane
        Vector3 planeVelocity(velocity.x, 0.0f, velocity.z);

        if (controls.IsDown(CTRL_FORWARD)) moveDir += Vector3::FORWARD;
        if (controls.IsDown(CTRL_BACK)) moveDir += Vector3::BACK;
        if (controls.IsDown(CTRL_LEFT)) moveDir += Vector3::LEFT;
        if (controls.IsDown(CTRL_RIGHT)) moveDir += Vector3::RIGHT;

        moveDir += Vector3(0.0f, 0.0f, 0.0f);

        // Normalize move vector so that diagonal strafing is not faster
        if (moveDir.lengthSquared > 0.0f) moveDir.Normalize();

        // If in air, allow control, but slower than when on ground
        body.ApplyImpulse(rot * moveDir *
                          (softGrounded ? MOVE_FORCE : INAIR_MOVE_FORCE));

        if (softGrounded) {
            // When on ground, apply a braking force to limit maximum ground
            // velocity
            Vector3 brakeForce = -planeVelocity * BRAKE_FORCE;
            body.ApplyImpulse(brakeForce);

            // Jump. Must release jump control between jumps
            if (controls.IsDown(CTRL_JUMP)) {
                if (okToJump) {
                    body.ApplyImpulse(Vector3::UP * JUMP_FORCE);
                    okToJump = false;
                    animCtrl.PlayExclusive("Models/Mutant/Mutant_Jump1.ani", 0,
                                           false, 0.2f);
                }
            } else
                okToJump = true;
        }

        if (!onGround) {
            animCtrl.PlayExclusive("Models/Mutant/Mutant_Jump1.ani", 0, false,
                                   0.2f);
        } else {
            // Play walk animation if moving on ground, otherwise fade it out
            if (softGrounded && !moveDir.Equals(Vector3::ZERO)) {
                animCtrl.PlayExclusive("Models/Mutant/Mutant_Run.ani", 0, true,
                                       0.2f);
                // Set walk animation speed proportional to velocity
                animCtrl.SetSpeed("Models/Mutant/Mutant_Run.ani",
                                  planeVelocity.length * 0.3f);
            } else
                animCtrl.PlayExclusive("Models/Mutant/Mutant_Idle0.ani", 0,
                                       true, 0.2f);
        }

        // Reset grounded flag for next frame
        onGround = false;
    }
}

class Fpmed : ScriptObject {
    void DataGate(VariantMap vin, VariantMap &vout) {
        String str = vin["CMD"].GetString();
        // log.Info("angelscript received: " + str);

        String[] @cmds = str.Split(';');

        if (cmds[0] == "SLIDEMOVE") {
            log.Info(str);
            // slideComp.ApplyMouseMove(IntVector2(cmds[1].ToFloat() * 75.0f,
            //                                     cmds[2].ToFloat() * 75.0f));
        }
        if (cmds[0] == "SLIDEZOOM") {
            log.Info(str);
            // slideComp.SetZoom(cmds[1].ToFloat() / 5.0f);
        }
        if (cmds[0] == "NEXTSLIDE") {
            log.Info(str);
            // slideComp.NextSlide();
        }
        if (cmds[0] == "PREVIOUSSLIDE") {
            log.Info(str);
            // slideComp.PreviousSlide();
        }
    }

    void FpmedStart() {
        SampleStart();
        CreateScene();

        CreateInstructions();
        SampleInitMouseMode(MM_RELATIVE);
        SubscribeToEvents();
    }

    void CreateScene() {
        scene_ = node;

        cameraNode = scene_.GetChild("CameRef");
        Camera @camera = cameraNode.GetComponent("Camera");
        camera.farClip = 5000.0f;
        renderer.viewports[0] = Viewport(scene_, camera);

        // Create a Zone component for ambient lighting & fog control
        Node @zoneNode = scene_.CreateChild("Zone");
        Zone @zone = zoneNode.CreateComponent("Zone");
        zone.boundingBox = BoundingBox(-6000.0f, 6000.0f);
        zone.ambientColor = Color(0.0f, 0.0f, 0.0f);
        zone.fogColor = Color(0.0f, 0.0f, 0.0f);
        zone.fogStart = 5800.0f;
        zone.fogEnd = 6000.0f;

        // Create a directional light to the world. Enable cascaded shadows on
        // it
        Node @lightNode = scene_.CreateChild("DirectionalLight");
        lightNode.direction = Vector3(0.6f, -1.0f, 0.8f);
        Light @light = lightNode.CreateComponent("Light");
        light.lightType = LIGHT_DIRECTIONAL;
        light.castShadows = true;
        light.shadowBias = BiasParameters(0.00025f, 0.5f);
        // Set cascade splits at 10, 50 and 200 world units, fade shadows out at
        // 80% of maximum shadow distance
        light.shadowCascade =
            CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f);

        // load prefab
        XMLFile @spaceSceneFile =
            cache.GetResource("XMLFile", "Objects/marco-presentation.xml");
        Node @spaceScene =
            scene.InstantiateXML(spaceSceneFile, Vector3(20.0f, 4.0f, 0.0f),
                                 Quaternion(0.0f, 0.0f, 0.0f));

        // slideComp = cameraNode.CreateComponent("Slide");
        // slideComp.CreateSlide("./slides/aramis/aramis.json");

        cameraNode.position = Vector3(0.0f, 0.0f, 0.0f);
        pitch = 80.0f;
        cameraNode.rotation = Quaternion(0.0f, 0.0f, 0.0f);
    }

    void CreateInstructions() {}

    void SubscribeToEvents() {
        //  before physics simulation
        SubscribeToEvent("Update", "HandleUpdate");
        // physics simulation
        SubscribeToEvent("PostUpdate", "HandlePostUpdate");

        // Unsubscribe the SceneUpdate event from base class as the camera node
        // is being controlled in HandlePostUpdate() in this sample
        UnsubscribeFromEvent("SceneUpdate");
    }

    void HandleUpdate(StringHash eventType, VariantMap &eventData) {}

    void HandlePostUpdate(StringHash eventType, VariantMap &eventData) {}
}