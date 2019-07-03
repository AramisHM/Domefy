// Rotator script object class. Script objects to be added to a scene node must implement the empty ScriptObject interface
class Fpmed : ScriptObject
{
    void FpmedStart()
    {
        // Create "Hello World" Text
        CreateText();

        // Finally, hook-up this HelloWorld instance to handle update events
        SubscribeToEvents();

        CreateScene();
    }

    void CreateText()
    {
        // Construct new Text object
        Text@ helloText = Text();

        // Set String to display
        helloText.text = "Hello World from Urho3D!";

        // Set font and text color
        helloText.SetFont(cache.GetResource("Font", "Fonts/Anonymous Pro.ttf"), 30);
        helloText.color = Color(0.0f, 1.0f, 0.0f);

        // Align Text center-screen
        helloText.horizontalAlignment = HA_CENTER;
        helloText.verticalAlignment = VA_CENTER;

        // Add Text instance to the UI root element
        ui.root.AddChild(helloText);
    }

    void CreateScene()
    {
        Node@ planeNode = node.CreateChild("Plane");
        planeNode.scale = Vector3(100.0f, 1.0f, 100.0f);
        StaticModel@ planeObject = planeNode.CreateComponent("StaticModel");
        planeObject.model = cache.GetResource("Model", "Models/Plane.mdl");
        planeObject.material = cache.GetResource("Material", "Materials/StoneTiled.xml");

        // Create a directional light to the world so that we can see something. The light scene node's orientation controls the
        // light direction; we will use the SetDirection() function which calculates the orientation from a forward direction vector.
        // The light will use default settings (white light, no shadows)
        Node@ lightNode = node.CreateChild("DirectionalLight");
        lightNode.direction = Vector3(0.6f, -1.0f, 0.8f); // The direction vector does not need to be normalized
        Light@ light = lightNode.CreateComponent("Light");
        light.lightType = LIGHT_DIRECTIONAL;

        // Create more StaticModel objects to the scene, randomly positioned, rotated and scaled. For rotation, we construct a
        // quaternion from Euler angles where the Y angle (rotation about the Y axis) is randomized. The mushroom model contains
        // LOD levels, so the StaticModel component will automatically select the LOD level according to the view distance (you'll
        // see the model get simpler as it moves further away). Finally, rendering a large number of the same object with the
        // same material allows instancing to be used, if the GPU supports it. This reduces the amount of CPU work in rendering the
        // scene.
        const uint NUM_OBJECTS = 200;
        for (uint i = 0; i < NUM_OBJECTS; ++i)
        {
            Node@ mushroomNode = node.CreateChild("Mushroom");
            mushroomNode.position = Vector3(Random(90.0f) - 45.0f, 0.0f, Random(90.0f) - 45.0f);
            mushroomNode.rotation = Quaternion(0.0f, Random(360.0f), 0.0f);
            mushroomNode.SetScale(0.5f + Random(2.0f));
            StaticModel@ mushroomObject = mushroomNode.CreateComponent("StaticModel");
            mushroomObject.model = cache.GetResource("Model", "Models/Mushroom.mdl");
            mushroomObject.material = cache.GetResource("Material", "Materials/Mushroom.xml");
        }
    }

    void SubscribeToEvents()
    {
        // Subscribe HandleUpdate() function for processing update events
        SubscribeToEvent("Update", "HandleUpdate");
    }

    void HandleUpdate(StringHash eventType, VariantMap& eventData)
    {
        // Do nothing for now, could be extended to eg. animate the display
    }
}