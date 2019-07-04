class world : ScriptObject
{
	bool initiated = false;
	bool liveUpdate = false;
	bool sdfdebug = false;
	String FractalTyp = "FCTYP_1";
	float myst = 0.01;
	float farclip = 120000;
	ResourceRef SkyTex;
	SkyTex.type = "Image";
	
	
	bool ambient;
	
	void Init()
	{
		UpdateParams();
		log.Info("world_init");
	}
	
	void Update(float timeStep)
	{
		if(initiated==false)
		{
			Init();
			initiated = true;
		}
		if(liveUpdate) UpdateParams();
		
	}
	
	void UpdateParams()
	{
		RenderPath@ renderpath = renderer.viewports[0].renderPath.Clone();
		RenderPathCommand rpc;
		
		String param = FractalTyp;
		
		sdfManager@ sdfMng = node.GetComponent("sdfManager");
		if(sdfMng!=null) sdfMng.SetDefines(param);
		
		
		for (int i=3; i<7; i++)
		{
			rpc = renderpath.commands[i];
			rpc.pixelShaderDefines = "PREMARCH " + param;
			renderpath.commands[i] = rpc;
		}
		rpc = renderpath.commands[7];
		rpc.pixelShaderDefines = "DEFERRED " + param;
		renderpath.commands[7] = rpc;
		
		if (ambient) param += " FOG";
		if (sdfdebug) param += " SDFFEBUG";
		
		rpc = renderpath.commands[8];
		rpc.pixelShaderDefines = "DEFERRED "  + param;
		renderpath.commands[8] = rpc;
		
		renderer.viewports[0].renderPath = renderpath;
		renderer.viewports[0].camera.farClip = farclip;
		
	}
}