
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

Node @characterNode;

class Character : ScriptObject
{
    VariantMap controlsInput;
    Controls controls;
    bool onGround = false;
    bool okToJump = true;
    float inAirTimer = 0.0f;

    void Start()
    {
        SubscribeToEvent(node, "NodeCollision", "HandleNodeCollision");
    }

    void Load(Deserializer &deserializer)
    {
        controls.yaw = deserializer.ReadFloat();
        controls.pitch = deserializer.ReadFloat();
    }

    void Save(Serializer &serializer)
    {
        serializer.WriteFloat(controls.yaw);
        serializer.WriteFloat(controls.pitch);
    }

    void HandleNodeCollision(StringHash eventType, VariantMap &eventData)
    {
        VectorBuffer contacts = eventData["Contacts"].GetBuffer();
        while (!contacts.eof)
        {
            Vector3 contactPosition = contacts.ReadVector3();
            Vector3 contactNormal = contacts.ReadVector3();
            float contactDistance = contacts.ReadFloat();
            float contactImpulse = contacts.ReadFloat();

            // If contact is below node center and pointing up, assume it's a
            // ground contact
            if (contactPosition.y < (node.position.y + 1.0f))
            {
                float level = contactNormal.y;
                if (level > 0.75)
                    onGround = true;
            }
        }
    }

    void FixedUpdate(float timeStep)
    {
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

        if (controls.IsDown(CTRL_FORWARD))
            moveDir += Vector3::FORWARD;
        if (controls.IsDown(CTRL_BACK))
            moveDir += Vector3::BACK;
        if (controls.IsDown(CTRL_LEFT))
            moveDir += Vector3::LEFT;
        if (controls.IsDown(CTRL_RIGHT))
            moveDir += Vector3::RIGHT;

        moveDir += Vector3(0.0f, 0.0f, 0.0f);

        // Normalize move vector so that diagonal strafing is not faster
        if (moveDir.lengthSquared > 0.0f)
            moveDir.Normalize();

        // If in air, allow control, but slower than when on ground
        body.ApplyImpulse(rot * moveDir *
                          (softGrounded ? MOVE_FORCE : INAIR_MOVE_FORCE));

        if (softGrounded)
        {
            // When on ground, apply a braking force to limit maximum ground
            // velocity
            Vector3 brakeForce = -planeVelocity * BRAKE_FORCE;
            body.ApplyImpulse(brakeForce);

            // Jump. Must release jump control between jumps
            if (controls.IsDown(CTRL_JUMP))
            {
                if (okToJump)
                {
                    body.ApplyImpulse(Vector3::UP * JUMP_FORCE);
                    okToJump = false;
                    animCtrl.PlayExclusive("Models/Mutant/Mutant_Jump1.ani", 0,
                                           false, 0.2f);
                }
            }
            else
                okToJump = true;
        }

        if (!onGround)
        {
            animCtrl.PlayExclusive("Models/Mutant/Mutant_Jump1.ani", 0, false,
                                   0.2f);
        }
        else
        {
            // Play walk animation if moving on ground, otherwise fade it out
            if (softGrounded && !moveDir.Equals(Vector3::ZERO))
            {
                animCtrl.PlayExclusive("Models/Mutant/Mutant_Run.ani", 0, true,
                                       0.2f);
                // Set walk animation speed proportional to velocity
                animCtrl.SetSpeed("Models/Mutant/Mutant_Run.ani",
                                  planeVelocity.length * 0.3f);
            }
            else
                animCtrl.PlayExclusive("Models/Mutant/Mutant_Idle0.ani", 0,
                                       true, 0.2f);
        }

        // Reset grounded flag for next frame
        onGround = false;
    }
}

class Fpmed : ScriptObject
{
    void DataGate(VariantMap vin, VariantMap &vout)
    {
        String str = vin["CMD"].GetString();
        // log.Info("angelscript received: " + str);

        String[] @cmds = str.Split(';');

        // vout["RET"] = "Hello from Angelscript! :D";

        Character @character = cast<Character>(characterNode.scriptObject);

        if (character is null)
            return;

        // Arbitrary move
        if (cmds[0] == "MOVE")
        {
            float triggerVal = 25.0f;
            float mvx = cmds[1].ToFloat();
            float mvy = cmds[2].ToFloat();
            // float sss = cmds[3].ToFloat() / 15.0f;
            log.Info(str);
            character.controlsInput["ACTIVE"] = true;

            character.controlsInput["BACK"] =
                Abs(mvy) > triggerVal && mvy > 0.0f;
            character.controlsInput["FORWARD"] =
                Abs(mvy) > triggerVal && mvy < 0.0f;
            character.controlsInput["RIGHT"] =
                Abs(mvx) > triggerVal && mvx > 0.0f;
            character.controlsInput["LEFT"] =
                Abs(mvx) > triggerVal && mvx < 0.0f;
        }
        if (cmds[0] == "NMOVE")
        {
            character.controlsInput["BACK"] = false;
            character.controlsInput["FORWARD"] = false;
            character.controlsInput["RIGHT"] = false;
            character.controlsInput["LEFT"] = false;
            character.controlsInput["ACTIVE"] = false;
        }

        if (cmds[0] == "VIEW")
        {
            character.controls.yaw += cmds[1].ToFloat() * 1.2f;
            character.controls.pitch += cmds[2].ToFloat() / 2.0f; // hemisphere vieweing
            log.Info(str);
        }

        if (cmds[0] == "SPACEBAR")
        {
            log.Info(str);
            character.controls.Set(CTRL_JUMP, true);
        }
    }

    void FpmedStart()
    {
        SampleStart();
        CreateScene();
        CreateCharacter();
        CreateInstructions();
        SampleInitMouseMode(MM_RELATIVE);
        SubscribeToEvents();
    }

    void CreateScene()
    {
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

        XMLFile @space =
            cache.GetResource("XMLFile", "Objects/space-scene-prefab.xml");
        XMLFile @ship =
            cache.GetResource("XMLFile", "Objects/spaceship-prefab.xml");
        // Node @spaceshipPrefab = scene.InstantiateXML(
        //     space, Vector3(20.0f, 4.0f, 0.0f), Quaternion(0.0f, 0.0f, 0.0f));
        // Node @spaceScenePrefab = scene.InstantiateXML(
        //     ship, Vector3(0.0f, 4.0f, 0.0f), Quaternion(0.0f, 0.0f, 0.0f));

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

        // Create the floor object
        Node @floorNode = scene_.CreateChild("Floor");
        floorNode.position = Vector3(0.0f, -0.5f, 0.0f);
        floorNode.scale = Vector3(200.0f, 1.0f, 200.0f);
        StaticModel @object = floorNode.CreateComponent("StaticModel");
        object.model = cache.GetResource("Model", "Models/Box.mdl");
        object.material = cache.GetResource("Material", "Materials/Stone.xml");

        RigidBody @body = floorNode.CreateComponent("RigidBody");
        // Use collision layer bit 2 to mark world scenery. This is what we will
        // raycast against to prevent camera from going inside geometry
        body.collisionLayer = 2;
        CollisionShape @shape = floorNode.CreateComponent("CollisionShape");
        shape.SetBox(Vector3::ONE);

        // music and ambient sounds
        SoundSource @musicSource;
        SoundSource @ambientSource;
        @musicSource = scene_.CreateComponent("SoundSource");
        @ambientSource = scene_.CreateComponent("SoundSource");
        musicSource.soundType = SOUND_MUSIC;
        ambientSource.soundType = SOUND_MUSIC;
        Sound @music =
            cache.GetResource("Sound", "Music/The Monster In Me.ogg");
        Sound @ambientSound = cache.GetResource("Sound", "Sounds/ambient.ogg");
        ambientSound.looped = true;
        musicSource.gain = 0.6f;
        //ambientSource.Play(ambientSound);
    }

    void CreateCharacter()
    {
        characterNode = scene_.CreateChild("Jack");
        characterNode.position = Vector3(0.0f, 1.0f, 0.0f);

        Node @adjNode = characterNode.CreateChild("AdjNode");
        adjNode.rotation = Quaternion(180.0f, Vector3::UP);
        AnimatedModel @object = adjNode.CreateComponent("AnimatedModel");
        object.model = cache.GetResource("Model", "Models/Mutant/Mutant.mdl");
        object.material = cache.GetResource(
            "Material", "Models/Mutant/Materials/mutant_M.xml");
        object.castShadows = true;
        adjNode.CreateComponent("AnimationController");
        object.skeleton.GetBone("Mutant:Head").animated = false;
        RigidBody @body = characterNode.CreateComponent("RigidBody");
        body.collisionLayer = 1;
        body.mass = 1.0f;
        body.angularFactor = Vector3::ZERO;
        body.collisionEventMode = COLLISION_ALWAYS;
        CollisionShape @shape = characterNode.CreateComponent("CollisionShape");
        shape.SetCapsule(0.7f, 1.8f, Vector3(0.0f, 0.9f, 0.0f));
        characterNode.CreateScriptObject(scriptFile, "Character");
    }

    void CreateInstructions() {}

    void SubscribeToEvents()
    {
        //  before physics simulation
        SubscribeToEvent("Update", "HandleUpdate");
        // physics simulation
        SubscribeToEvent("PostUpdate", "HandlePostUpdate");

        // Unsubscribe the SceneUpdate event from base class as the camera node
        // is being controlled in HandlePostUpdate() in this sample
        UnsubscribeFromEvent("SceneUpdate");
    }

    void HandleUpdate(StringHash eventType, VariantMap &eventData)
    {
        if (characterNode is null)
            return;

        Character @character = cast<Character>(characterNode.scriptObject);
        if (character is null)
            return;

        // Clear previous controls
        character.controls.Set(
            CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP,
            false);

        // Update controls using keys (desktop)
        if (ui.focusElement is null)
        {
            character.controls.Set(CTRL_FORWARD, input.keyDown[KEY_W]);
            character.controls.Set(CTRL_BACK, input.keyDown[KEY_S]);
            character.controls.Set(CTRL_LEFT, input.keyDown[KEY_A]);
            character.controls.Set(CTRL_RIGHT, input.keyDown[KEY_D]);
            character.controls.Set(CTRL_JUMP, input.keyDown[KEY_SPACE]);
            if (character.controlsInput["ACTIVE"].GetBool() == true)
            {
                character.controls.Set(
                    CTRL_FORWARD, character.controlsInput["FORWARD"].GetBool());
                character.controls.Set(
                    CTRL_BACK, character.controlsInput["BACK"].GetBool());
                character.controls.Set(
                    CTRL_LEFT, character.controlsInput["LEFT"].GetBool());
                character.controls.Set(
                    CTRL_RIGHT, character.controlsInput["RIGHT"].GetBool());
            }

            // Add character yaw & pitch from the mouse motion or touch input
            character.controls.yaw += input.mouseMoveX * YAW_SENSITIVITY;
            character.controls.pitch += input.mouseMoveY * YAW_SENSITIVITY;

            // Limit pitch
            character.controls.pitch =
                Clamp(character.controls.pitch, -80.0f, 80.0f);
            // Set rotation already here so that it's updated every rendering
            // frame instead of every physics frame
            characterNode.rotation =
                characterNode.rotation.Slerp(Quaternion(character.controls.yaw, Vector3::UP), 0.18f);

            // Switch between 1st and 3rd person
            if (input.keyPress[KEY_F])
                firstPerson = !firstPerson;

            // Check for loading / saving the scene
            if (input.keyPress[KEY_F5])
            {
                File saveFile(
                    fileSystem.programDir + "Data/Scenes/CharacterDemo.xml",
                    FILE_WRITE);
                scene_.SaveXML(saveFile);
            }
            if (input.keyPress[KEY_F7])
            {
                File loadFile(
                    fileSystem.programDir + "Data/Scenes/CharacterDemo.xml",
                    FILE_READ);
                scene_.LoadXML(loadFile);
                // After loading we have to reacquire the character scene node,
                // as it has been recreated Simply find by name as there's only
                // one of them
                characterNode = scene_.GetChild("Jack", true);
                if (characterNode is null)
                    return;
            }
        }
    }

    void HandlePostUpdate(StringHash eventType, VariantMap &eventData)
    {
        if (characterNode is null)
            return;

        Character @character = cast<Character>(characterNode.scriptObject);
        if (character is null)
            return;

        // Get camera lookat dir from character yaw + pitch
        Quaternion rot = characterNode.rotation;
        Quaternion dir =
            rot * Quaternion(character.controls.pitch, Vector3::RIGHT);

        // Turn head to camera pitch, but limit to avoid unnatural animation
        Node @headNode = characterNode.GetChild("Mutant:Head", true);
        float limitPitch = Clamp(character.controls.pitch, -45.0f, 45.0f);
        Quaternion headDir =
            rot * Quaternion(limitPitch, Vector3(1.0f, 0.0f, 0.0f));
        // This could be expanded to look at an arbitrary target, now just look
        // at a point in front
        Vector3 headWorldTarget =
            headNode.worldPosition + headDir * Vector3(0.0f, 0.0f, -1.0f);
        headNode.LookAt(headWorldTarget, Vector3(0.0f, 1.0f, 0.0f));

        if (firstPerson)
        {
            // First person camera: position to the head bone + offset slightly
            // forward & up
            cameraNode.position =
                headNode.worldPosition + rot * Vector3(0.0f, 0.15f, 0.2f);
            cameraNode.rotation = dir;
        }
        else
        {
            // Third person camera: position behind the character
            Vector3 aimPoint =
                characterNode.position +
                rot * Vector3(0.0f, 1.7f,
                              0.0f); // You can modify x Vector3 value to
                                     // translate the fixed character position
                                     // (indicative range[-2;2])

            // Collide camera ray with static physics objects (layer bitmask 2)
            // to ensure we see the character properly
            Vector3 rayDir =
                dir * Vector3::BACK; // For indoor scenes you can use dir *
                                     // Vector3(0.0, 0.0, -0.5) to prevent
                                     // camera from crossing the walls
            float rayDistance = cameraDistance;
            PhysicsRaycastResult result = scene_.physicsWorld.RaycastSingle(
                Ray(aimPoint, rayDir), rayDistance, 2);
            if (result.body !is null)
                rayDistance = Min(rayDistance, result.distance);
            rayDistance = Clamp(rayDistance, CAMERA_MIN_DIST, cameraDistance);

            cameraNode.position = aimPoint + rayDir * rayDistance;
            cameraNode.rotation = dir;
        }
    }
}