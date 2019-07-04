class PlayerController : ScriptObject
{
	RigidBody@ rb;
	sdfCollider@ sdfCol;
	sdfCollider@ sdfFeet;
	
	float yaw = 0.0f; // Camera yaw angle
	float pitch = 0.0f;
	float zoom;
	Camera@ cam;
	Node@ camNode;
	bool init = false;
	bool onBPYGround;
	bool onSDFGround;
	bool okToJump;
	bool okToJet;
	float ArmPhase = 0.;
	Vector3 armPos;
	float flareReload = 0.;
	float jettime = 0.;
	
	void Init()
    {
		rb = node.GetComponent("RigidBody");
			
		sdfCol = node.GetComponent("sdfCollider");
		
		camNode = node.GetChild("Cam_Node", false);
		
		cam = camNode.GetComponent("Camera");

		renderer.viewports[0].camera = cam;
		
		Node@ feetNode = node.GetChild("Feet", false);
		sdfFeet = feetNode.GetComponent("sdfCollider");
		
		SubscribeToEvent(node, "NodeCollision", "HandleNodeCollision");
		
		armPos = node.GetChild("Arm").position;
		
		yaw = node.worldRotation.yaw;
		
		init = true;
			
	}
	
	void Update(float timeStep)
	{
		if(!init)Init();
		
		ApplyMouseControls(timeStep);
		
		float FeetDist = sdfFeet.GetDist();
		Vector3 FeetN = sdfFeet.GetNorm();
		FeetN.Normalize();
		
		float radius = sdfCol.GetRadius();
		float dist =  sdfCol.GetDist();
		Vector3 N = sdfCol.GetNorm();
		
		if ( FeetDist < 0.3)
		{
			if (FeetN.y > 0.6 or (FeetN.y<-0.3 and N.y>0.3))
			{
					onSDFGround = true;
					if ( FeetDist < 0.2)
					{
						Vector3 vel = rb.linearVelocity;
						rb.linearVelocity = Vector3(vel.x * 0.9,Max(vel.y,0.),vel.z * 0.9);
						rb.ApplyImpulse(Vector3(0.,4. * Max(1.-(FeetDist * 5.0),0.),0.));
					}
			} 
		}
		
		N.Normalize();
		if (dist<radius)
		{
			
			Vector3 vel = rb.linearVelocity;
			if(0.0>N.DotProduct(vel.Normalized()))
			{
				Vector3 localpos = N * -1. * (radius/2.);//rb.rotation * 
				
				Vector3 locVel = rb.GetVelocityAtPoint(localpos);
				float dt =  vel.DotProduct(N);
				rb.ApplyImpulse(locVel*-1.,localpos);
				rb.ApplyImpulse(N*(radius/2.-dist)*rb.mass * 4.);
				
			}
		} 
		float vel =  0.;
		if (onSDFGround or onBPYGround) vel = rb.linearVelocity.length;;
		ArmPhase += timeStep * (120 * vel);
		//log.Info(vel);
		node.GetChild("Arm").position =  armPos + Vector3(Cos(ArmPhase),Cos(ArmPhase * 2.),0.0) * 0.06;
		
		
		Color hudcol = Color(0.1,1.0,0.1,1.0);
        Color hudcol2 = Color(1.0,0.6,0.1,1.0);
		Color feetCol = hudcol2;
		if (FeetN.y < 0.6) feetCol = Color(0.7,0.1,0.0);
		if (onSDFGround) feetCol = Color(1.0,1.0,0.8);
		/*
		DebugRenderer@ hud = node.scene.debugRenderer;
		hud.AddCircle(camNode.worldPosition + (camNode.worldRotation * Vector3(70,0,100)),camNode.worldRotation * Vector3(0,0,1), radius * 20.0 ,hudcol,32 , false);
		hud.AddCircle(camNode.worldPosition + (camNode.worldRotation * Vector3(70,sdfFeet.node.position.y * 20.,100)),camNode.worldRotation * Vector3(0,0,1), sdfFeet.GetRadius() * 20. ,hudcol,32 , false);
		hud.AddCircle(camNode.worldPosition + (camNode.worldRotation * Vector3(70,sdfFeet.node.position.y * 20.,100)),camNode.worldRotation * Vector3(0,0,1), 0.2 * 20. ,hudcol,32 , false);
		
		hud.AddCircle(camNode.worldPosition + (camNode.worldRotation * Vector3(70,0,100)),camNode.worldRotation * Vector3(0,0,1), dist * 20.0 ,hudcol2,32 , false);
		hud.AddCircle(camNode.worldPosition + (camNode.worldRotation * Vector3(70,sdfFeet.node.position.y * 20.,100)),camNode.worldRotation * Vector3(0,0,1), FeetDist * 20. ,feetCol,32 , false);
		
		hud.AddCircle(camNode.worldPosition + (camNode.worldRotation * (Vector3(70,0,100)+node.rotation * -N * dist * 20.)),camNode.worldRotation * node.rotation * -N, radius * 20.0 ,hudcol2,32 , false);
		hud.AddCircle(camNode.worldPosition + (camNode.worldRotation * (Vector3(70,sdfFeet.node.position.y * 20.,100)+node.rotation * -FeetN * FeetDist * 20.)),camNode.worldRotation * node.rotation *  -FeetN, sdfFeet.GetRadius() * 20. ,feetCol,32 , false);
		*/
		if (input.keyPress['F'])
		{
			Node@ lightNode = node.GetChild("Arm").GetChild("SpotLight");
			lightNode.enabled = !lightNode.enabled;
		}
		
		if(input.keyPress[KEY_BACKSPACE])
		{	
			VariantMap p_data;
			p_data["map"] = "d_hub";
			SendEvent("SwitchScene", p_data);
		}
		
		flareReload -= timeStep;
	}
	
	void HandleNodeCollision(StringHash eventType, VariantMap& eventData)
    {
       
		VectorBuffer contacts = eventData["Contacts"].GetBuffer();
		//log.Info(eventType.ToString());
        while (!contacts.eof)
        {
            Vector3 contactPosition = contacts.ReadVector3();
            Vector3 contactNormal = contacts.ReadVector3();
            float contactDistance = contacts.ReadFloat();
            float contactImpulse = contacts.ReadFloat();
			//log.Info(contactNormal.y);
            // If contact is below node center and pointing up, assume it's a ground contact
            if (contactPosition.y < (node.position.y + 1.0f))
            {
				
                float level = Abs(contactNormal.y);
                if (level > 0.4){
					onBPYGround = true;
					//rb.ApplyImpulse(Vector3(0.,20.,0.));
				}
                    
            }
        }
    }
	
	void FixedUpdate(float timeStep)
	{
		
		
		ApplyControls(timeStep);
		
		
		onBPYGround = false;
		onSDFGround = false;	
	}
	
	void ApplyMouseControls(float timeStep)
	{
		if (ui.focusElement !is null)
        return;
		
		const float MOUSE_SENSITIVITY = 0.1 * 1/cam.zoom;
		
		IntVector2 mouseMove = input.mouseMove;
        yaw += MOUSE_SENSITIVITY * mouseMove.x;
        pitch += MOUSE_SENSITIVITY * mouseMove.y;
        pitch = Clamp(pitch, -90.0f, 90.0f);

         // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
        camNode.rotation = Quaternion(pitch, 0.0, 0.0f);
		node.rotation = Quaternion(0.0,yaw,0.0);
		node.GetChild("Arm").rotation = Quaternion(pitch, 0.0,0.0);

        int mousescroll = input.mouseMoveWheel;
        zoom = Clamp(zoom + mousescroll * zoom * 0.2, 0.6 , 20.0 );
		//if (input.mouseButtonDown[MOUSEB_RIGHT]) zoom = 5; else zoom = 0.6;
		cam.zoom = Lerp(cam.zoom, zoom, Min(5 * timeStep,1.));
		
		if(input.mouseButtonPress[MOUSEB_LEFT] && flareReload < 0.)
		{
			Node@ arm = node.GetChild("Arm");
			Vector3 pos = arm.worldPosition + arm.worldRotation * Vector3(0.,0.,1.);
			Node@ flareNode =  scene.InstantiateXML(cache.GetResource("XMLFile", "Objects/flare.xml"), pos,Quaternion(0,0,0));
			RigidBody@ flareRB =  flareNode.GetComponent("RigidBody");
			flareRB.linearVelocity = rb.linearVelocity + arm.worldDirection * 20.;
			if(Random()>0.9)
			{
				Light@ light = flareNode.GetComponent("Light");
				light.color =  Color(0.3+Random(0.7),0.3+Random(0.7),0.3+Random(0.7)) * 1.05;
			}
			
			
			flareReload = 1.;
		}
	}
	
	void ApplyControls(float timeStep)
	{
		
		float FeetDist = sdfFeet.GetDist();
		
		float speed = 30.;
		if (onSDFGround or onBPYGround)
			speed = 200.;
		//log.Info(onBPYGround);
		Vector3 walk = Vector3(0.,0.,0.);
		 if (input.keyDown['W'])
            walk += Vector3(0.0f, 0.0f, 1.0f);
        if (input.keyDown['S'])
            walk += Vector3(0.0f, 0.0f, -1.0f);
        if (input.keyDown['A'])
            walk += Vector3(-1.0f, 0.0f, 0.0f);
        if (input.keyDown['D'])
			walk += Vector3(1.0f, 0.0f, 0.0f);
		walk.Normalize();
		rb.ApplyImpulse(node.rotation * walk * speed * timeStep);
		
		
		if (onSDFGround or onBPYGround)
		{
			Vector3	vel = rb.linearVelocity;
			Vector3 planeVelocity = Vector3(vel.x,0.0,vel.z);
			rb.ApplyImpulse( -planeVelocity * 0.4);
			jettime = 0.;
		}
		okToJet = false;
		if (input.keyDown[' '])
		{
			if ((onSDFGround or onBPYGround))
			{
				if(okToJump)
				{
					rb.ApplyImpulse(node.rotation * Vector3(0.0f, 15.0f, 0.0f) * timeStep * speed);
					okToJump = false;
				}
				
				node.GetChild("JetPack").enabled = false;
			} else {
				if ( okToJump and FeetDist> sdfFeet.GetRadius() and jettime < 5.)
				{
					Vector3 jetVec = Vector3(0.,1.,0.);
					//jetVec += walk;	
					//jetVec.Normalize();
					//jetVec.x=jetVec.z=0;
					rb.ApplyImpulse(node.rotation * jetVec * timeStep * 110);
					Node@ jetNode = node.GetChild("JetPack");
					jetNode.enabled = true;
					jettime += timeStep;
					Light@ jetlight = jetNode.GetComponent("Light");
					jetlight.brightness = Min(jettime,1.);
				} else {
					node.GetChild("JetPack").enabled = false;
				}
			}
		

			
		} else {
			okToJump = true;
			node.GetChild("JetPack").enabled = false;
			
		}
		
		
	}
}