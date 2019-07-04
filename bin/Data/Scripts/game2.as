#include "freelookCam.as";
#include "Player.as";
Scene@ scene_;
bool wireframe =false;
RenderPath@ renderpath;

Node@ PlayerNode;
Node@ cameraNode;

bool freeLookMode = false;
String spawnGate;

bool sceneReload;
String sceneName;
Text@ fpsText;
UIElement@ LegendNode;

void Start()
{
	cache.autoReloadResources = true;

    scene_ = Scene();
	scene_.LoadXML(cache.GetFile("Scenes/d_hub.xml"));
	
	CreateConsoleAndDebugHud();

	SubscribeToEvent("KeyDown", "HandleKeyDown");

 
	
	cameraNode = scene_.CreateChild("CamNode");
	cameraNode.position = Vector3(30.0f , 14.0f , -190.0f);
    Camera@ camera = cameraNode.CreateComponent("Camera");
	Viewport@ mainVP = Viewport(scene_, camera);
	
	freelookCam@ flcam = cast<freelookCam>(cameraNode.CreateScriptObject(scriptFile, "freelookCam"));
    flcam.Init();
	cameraNode.enabled = false;

	renderer.hdrRendering = true;
	renderer.viewports[0] = mainVP;
	renderpath = mainVP.renderPath.Clone();
	renderpath.Load(cache.GetResource("XMLFile","RenderPaths/Deferred.xml"));
	renderer.viewports[0].renderPath = renderpath;

	SpawnPlayer();
	
	//SubscribeToEvent("Update", "HandleUpdate");
	SubscribeToEvent("PostUpdate", "HandlePostUpdate");
	SubscribeToEvent("Update", "HandleUpdate");
	//SubscribeToEvent("PostRenderUpdate", "HandlePostRenderUpdate");
	/*
	float rad = 128.;
	float bright = 16.;
	Vector3 pos = Vector3(150.,-150.,90.);
	
	for(int i = 0; i<16; i++)
	{
		for(int u = 0; u<16; u++)
		{
			CreateLight(pos,bright,rad);
			bright *= 0.5;
			pos.x += 32.0;
			
		}
		pos.x = 150.0;
		pos.y += 32.0;
		rad *= 0.75;
		bright = 16.;
		PlayerNode.position = pos;
	}
	*/
	//File saveFile(fileSystem.programDir + "Data/Scenes/apotest.xml", FILE_WRITE);
	//scene_.SaveXML(saveFile);
	
	 SubscribeToEvent("SwitchScene", "HandleSwitchScene");
	 RegisterEventName("SwitchScene");
	 
	fpsText = ui.root.CreateChild("Text");
	fpsText.SetFont(cache.GetResource("Font", "Fonts/Anonymous Pro.ttf"), 10);
	fpsText.horizontalAlignment = HA_LEFT;
	fpsText.verticalAlignment = VA_TOP;
	fpsText.SetPosition(0,0);
	fpsText.color = Color(1,1,0.5);;
	
	LegendNode = ui.root.CreateChild("UIElement");
	LegendNode.SetPosition(200 , 10);
	LegendNode.horizontalAlignment = HA_LEFT;
	LegendNode.verticalAlignment = VA_TOP;
	
	Text@ helpText = LegendNode.CreateChild("Text");
	helpText.SetFont(cache.GetResource("Font", "Fonts/Anonymous Pro.ttf"), 10);
	helpText.horizontalAlignment = HA_LEFT;
	helpText.verticalAlignment = VA_TOP;
	helpText.SetPosition(0,0);
	helpText.color = Color(1,1,0.5);
	helpText.text =
					"F1 - toggle help \n"
					"F2 - show profiler \n"
					"F12 - take screenshot \n\n"
					
					"WASD and Mouse - walk around\n"
					"Space - jump and jetpack\n"
					"Jetpack gives you 6 seconds of propulsion\n"
					"Mouse wheel - Zoom\n"
					"F - toggle flashlight\n"
					"LMB - shoot flares\n"
					"Backspace - return to starting area\n\n"
					
					"F5 - toggle freelook camera\n"
					"Shift - fly faster \n"
					"Ctrl - fly slower \n"
					"Alt - super speed \n\n";
}

void SpawnPlayer()
{
	Vector3 playerPos = Vector3(0.,20.,0.);
	Quaternion playerRot = Quaternion(0,0,0);
	Node@ PlayerStartNode = scene_.GetChild("PlayerStart", true);
	if(PlayerStartNode!=null)
	{
		playerPos = PlayerStartNode.worldPosition;
		playerRot = PlayerStartNode.worldRotation;
		
	}
	
    PlayerNode = scene_.InstantiateXML(cache.GetResource("XMLFile", "Objects/Player.xml"), playerPos,playerRot);
	
	cameraNode.parent = scene_;
	
	Node@ fakeboxNode = scene_.CreateChild("Plane");
	fakeboxNode.scale = Vector3(6000.0f, 6000.0f, 6000.0f);
	StaticModel@ fakeboxObject = fakeboxNode.CreateComponent("StaticModel");
	fakeboxObject.model = cache.GetResource("Model", "Models/Box.mdl");
}

void CreateLight(Vector3 pos, float bright, float rad)
{
	Color col = Color(1.0,0.4,0.1);
	Node@ testLightNode = scene_.CreateChild("");
	testLightNode.position = pos;
	Light@ testLight = testLightNode.CreateComponent("Light");
	testLight.brightness = bright;
	testLight.range = rad;
	testLight.color = col;
	log.Info(pos.ToString());
	log.Info(testLight.id);
	log.Info(scene_.id);
}

void CreateConsoleAndDebugHud()
{
    // Get default style
    XMLFile@ xmlFile = cache.GetResource("XMLFile", "UI/DefaultStyle.xml");
    if (xmlFile is null)
        return;

    // Create console
    Console@ console = engine.CreateConsole();
    console.defaultStyle = xmlFile;
    console.background.opacity = 0.8f;

    // Create debug HUD
    DebugHud@ debugHud = engine.CreateDebugHud();
    debugHud.defaultStyle = xmlFile;

}

void HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    int key = eventData["Key"].GetInt();

    // Close console (if open) or exit when ESC is pressed
    if (key == KEY_ESCAPE)
    {
        if (!console.visible)
            engine.Exit();
        else
            console.visible = false;
    }

    // Toggle console with F1
    else if (key == 96)
        console.Toggle();
	
	else if (key == KEY_F1)
	{
		if (LegendNode.visible) 
		{
			LegendNode.visible = false;
		} else {
			LegendNode.visible = true;
		}
	} 
    // Toggle debug HUD with F2
    else if (key == KEY_F2)
        debugHud.ToggleAll();

    // Take screenshot
    else if (key == KEY_F5)
        {
				if(freeLookMode){
					//Ca@ pl = PlayerNode.GetComponent("ScriptInstance");
					renderer.viewports[0].camera = PlayerNode.GetComponent("Camera", true);
					PlayerNode.enabled = true;
					PlayerNode.position = cameraNode.position;
					PlayerNode.rotation = cameraNode.rotation;
					cameraNode.enabled = false;
					freeLookMode = false;
				} else {
					renderer.viewports[0].camera = cameraNode.GetComponent("Camera");
					cameraNode.enabled = true;
					cameraNode.position = PlayerNode.position;
					cameraNode.rotation = PlayerNode.rotation;
					PlayerNode.enabled = false;
					freeLookMode = true;
				}
		}else if (key == KEY_F12)
        {
            Image@ screenshot = Image();
            graphics.TakeScreenShot(screenshot);
            // Here we save in the Data folder with date and time appended
            screenshot.SavePNG(fileSystem.programDir + "Data/Screenshot_" +
                time.timeStamp.Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
        } else if (key == KEY_V)
        {

			Camera@ cam = renderer.viewports[0].camera;
            if (wireframe){
                cam.fillMode = FILL_SOLID;
                wireframe = false;
            } else {
                cam.fillMode = FILL_WIREFRAME;
                wireframe = true;
            }

        }


}

void HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
	if(sceneReload)
	{
		scene_.LoadXML(cache.GetFile("Scenes/" + sceneName + ".xml"));
	
		SpawnPlayer();
		//fractalWorld@ fW = scene_.GetComponent("fractalWorld");
		//fW.UpdateParams();
		
		
		//OOOOOH SUPAHACK NOTHING WORKS
		Node@ camNode = PlayerNode.GetChild("Cam_Node", false);
		
		Camera@ cam = camNode.GetComponent("Camera");
		cam.farClip = scene_.GetComponent("fractalWorld").GetAttribute("Far clip").GetFloat();
		
		sceneReload = false;
	}
}


void HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	float timeStep = Max(eventData["TimeStep"].GetFloat(),0.0001);
	int fps = int(1./timeStep);
	fpsText.text = "fps: " + fps;
	if (fps>55) fpsText.color = Color(0.2,1.,0.2);
	else if (fps>25) fpsText.color = Color(1.0,0.8,0.1);
	else fpsText.color = Color(1.0,0.0,0.0);
}

void HandleSwitchScene(StringHash eventType, VariantMap& eventData)
{
	log.Info("sceneSwitch!");
	//scene_.RemoveAllComponents();
	//scene_.RemoveAllChildren();
	//scene_.LoadXML(cache.GetFile("Scenes/" + eventData["map"].GetString() + ".xml"));
	
	//SpawnPlayer();
	scene_.RemoveComponent("sdfManager");
	sceneName = eventData["map"].GetString();
	sceneReload = true;
}