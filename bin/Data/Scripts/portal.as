//#include "Player.as";

class portal : ScriptObject
{
	String sceneName;
	void DelayedStart()
    {
		SubscribeToEvent(node, "NodeCollisionStart", "HandleNodeCollisionStart");
		
		log.Info("portal init");
	}
	
	void Update(float timeStep)
	{
		Node@ lightNode = node.GetChild("Light");
		float t = time.elapsedTime;
		lightNode.position =  Vector3(Sin(t * 200),Cos(t*273.5),Sin(t*-151.12));
	}
	
	void HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData)
    {
   		Node@ colNode = eventData["OtherNode"].GetPtr();
		//PlayerController@ plr = cast<PlayerController>(colNode.scriptObject);
		
		
		//if (plr !is null)
		//FML, nothing works		
		if(colNode.GetComponent("ScriptInstance").GetAttribute("Class Name").GetString()=="PlayerController")
		{
			log.Info("portal msg");
			VariantMap p_data;
			p_data["map"] = sceneName;
			SendEvent("SwitchScene", p_data);
		}
	}
}