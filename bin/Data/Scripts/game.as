// #include "freelookCam.as";
Scene@ scene_;
String  fractaltyp = " ";
RenderPath@ renderpath;
Node@ cameraNode;

//sdfManager@ sdfMng;
class Fpmed : ScriptObject {
	void FpmedStart(Viewport@ specialVP) {
		cache.autoReloadResources = true;
		scene_ = node;
		SubscribeToEvent("KeyDown", "HandleKeyDown");
		renderer.hdrRendering = true;		
		SetupScene();
		spawnlights(cameraNode.position, 26);	
	}

	void SetupScene() {
		cameraNode = scene_.GetChild("CameRef");
		cameraNode.position = Vector3(0.0f , 14.0f , -20.0f);
		Camera@ camera = cameraNode.GetComponent("Camera");
		Viewport@ mainVP = Viewport(scene_, camera);
		renderer.viewports[0] = mainVP;
		renderpath = mainVP.renderPath.Clone();
		renderpath.Load(cache.GetResource("XMLFile","RenderPaths/Deferred.xml"));
		renderer.viewports[0].renderPath = renderpath;
		// freelookCam@ flcam = cast<freelookCam>(cameraNode.CreateScriptObject(scriptFile, "freelookCam"));
		// flcam.Init();
		fractaltyp = "FCTYP_6";
		setRndCommandParam(fractaltyp);
	}

	void HandleKeyDown(StringHash eventType, VariantMap& eventData) {	
		int key = eventData["Key"].GetInt();
		if (key == KEY_F12) {
			Image@ screenshot = Image();
			graphics.TakeScreenShot(screenshot);
			screenshot.SavePNG(fileSystem.programDir + "Data/Screenshot_" +
				time.timeStamp.Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
		} else if (key == KEY_Y) {
			spawnlights(cameraNode.position, 6);
		} else if (key == KEY_3) {
			fractaltyp = "FCTYP_3";
			setRndCommandParam(fractaltyp);
		} else if (key == KEY_6) {
			fractaltyp = "FCTYP_6";
			setRndCommandParam(fractaltyp);
		} 
	}

	void setRndCommandParam(String param) {
		renderpath = renderer.viewports[0].renderPath.Clone();
		RenderPathCommand rpc;
		for (int i=3; i<7; i++) {
			rpc = renderpath.commands[i];
			rpc.pixelShaderDefines = "PREMARCH " + param;
			renderpath.commands[i] = rpc;
		}
		rpc = renderpath.commands[7];
		rpc.pixelShaderDefines = "DEFERRED " + param;
		renderpath.commands[7] = rpc;
		// if (fog) param += " FOG";
		// if (sdfdebug) param += " SDFFEBUG";
		rpc = renderpath.commands[8];
		rpc.pixelShaderDefines = "DEFERRED "  + param;
		renderpath.commands[8] = rpc;
		renderer.viewports[0].renderPath = renderpath;
	}

	void spawnlights (Vector3 pos, int numLights) {
		float sc = 10;
		for (int i=0; i<numLights; i++) {
			Node@ plightNode = scene_.CreateChild("pointlight");
			plightNode.position = pos + Vector3(sc/2-Random(sc),sc/2-Random(sc),sc/2-Random(sc));
			Light@ plight = plightNode.CreateComponent("Light");
			plight.color = Color(0.2+Random(1.0f),0.2+Random(1.0f),0.2+Random(1.0f),1.0);
			plight.range = 190 + Random(60);
			plightNode.Rotate(Quaternion(Random(360),Random(360),0.f));
		}
	}
}