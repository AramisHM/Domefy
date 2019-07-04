class sdfBody : ScriptObject
{
	bool LookForParent = false;
	void Init()
    {
		
	}
	
	void FixedUpdate(float timeStep)
	{
		sdfCollider@ collider = node.GetComponent("sdfCollider");
		RigidBody@ rb;
		if(LookForParent)
		{
				rb = node.parent.GetComponent("RigidBody");
		}else {
				rb = node.GetComponent("RigidBody");
		}
		
		float radius = collider.GetRadius();
		float dist =  collider.GetDist();
		
		if (dist<radius/5.)
		{
			Vector3 n = collider.GetNorm();
			n.Normalize();
			Vector3 vel = rb.linearVelocity;
			if(0.0>n.DotProduct(vel.Normalized()))
			{
				Vector3 localpos = n * -1. * (radius/5.);//rb.rotation * 
				if(LookForParent) localpos += node.parent.rotation * node.position;
				Vector3 locVel = rb.GetVelocityAtPoint(localpos);
				float dt =  vel.DotProduct(n);
				rb.ApplyImpulse(locVel*-1.,localpos);
				rb.ApplyImpulse(n*(radius/5.-dist)*rb.mass);
				
				//log.Info("col");
			}
		} 
		//else if (dist == 9e20)
		//{
		//	log.Info("BAM!");
		//}
		//rb.angularVelocity *= 0.99 ;
	}
	
}