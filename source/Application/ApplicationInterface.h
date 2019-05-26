#ifndef APPLICATION_INTERFACE_H
#define APPLICATION_INTERFACE_H

#include "Urho3D\Urho3D.h"
#include "Urho3D\UI\Button.h"
#include "Urho3D\Graphics\Camera.h"
#include "Urho3D\Physics\CollisionShape.h"
#include "Urho3D\Network\Connection.h"
#include "Urho3D\Input\Controls.h"
#include "Urho3D\Core\CoreEvents.h"
#include "Urho3D\UI\Cursor.h"
#include "Urho3D\Engine\Engine.h"
#include "Urho3D\UI\Font.h"
#include "Urho3D\Graphics\Graphics.h"
#include "Urho3D\Input\Input.h"
#include "Urho3D\Graphics\Light.h"
#include "Urho3D\UI\LineEdit.h"
#include "Urho3D\IO\Log.h"
#include "Urho3D\Graphics\Material.h"
#include "Urho3D\Graphics\Model.h"
#include "Urho3D\Network\Network.h"
#include "Urho3D\Network\NetworkEvents.h"
#include "Urho3D\Graphics\Octree.h"
#include "Urho3D\Physics\PhysicsEvents.h"
#include "Urho3D\Physics\PhysicsWorld.h"
#include "Urho3D\Graphics\Renderer.h"
#include "Urho3D\Physics\RigidBody.h"
#include "Urho3D\Resource\ResourceCache.h"
#include "Urho3D\Scene\Scene.h"
#include "Urho3D\Graphics\StaticModel.h"
#include "Urho3D\UI\Text.h"
#include "Urho3D\UI\UI.h"
#include "Urho3D\UI\UIEvents.h"
#include "Urho3D\Resource\XMLFile.h"
#include "Urho3D\Graphics\Zone.h"
#include "SceneReplication.h"
#include "Urho3D\DebugNew.h"
#include <Urho3D/Graphics/Technique.h>

#define n_screens 4
#define mxlen 800
#define mylen 600

#endif