class flare : ScriptObject
{
	float ttl = 40.;
	void Init()
    {
		
	}
	
	void FixedUpdate(float timeStep)
	{
		sdfCollider@ collider = node.GetComponent("sdfCollider");
		RigidBody@ rb;
		rb = node.GetComponent("RigidBody");
		
		
		float radius = collider.GetRadius();
		float dist =  collider.GetDist();
		Vector3 n = collider.GetNorm();
		
		Light@ lght = node.GetComponent("Light");
		lght.range = Clamp(dist * 5. + n.length * 100. , 3.,30.);
		
		ttl -= (Clamp(5.-dist*5.,0.,5.) + 1.)*timeStep;
		lght.brightness = Clamp(ttl,0.,1.);
		if (ttl<0) node.Remove();
		lght.color = Color(1.0,Lerp(1.,0.1,ttl/40.),Lerp(0.7,0.0,ttl/40.));
		if (dist<radius/5.)
		{
			
			n.Normalize();
			Vector3 vel = rb.linearVelocity;
			if(0.0>n.DotProduct(vel.Normalized()))
			{
				Vector3 localpos = n * -1. * (radius/5.);//rb.rotation * 
				
				Vector3 locVel = rb.GetVelocityAtPoint(localpos);
				float dt =  vel.DotProduct(n);
				//rb.ApplyImpulse(locVel*-1.*timeStep,localpos);
				rb.ApplyImpulse(n*(radius-dist)*rb.mass*timeStep * 400.);
				
				
			}
		}  else 
		{
			n.Normalize();
			rb.ApplyImpulse(n*timeStep*rb.mass);
		}
		
		

	}
	
}