/*
 * File: VirtualObject.cpp
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Saturday, 25th May 2019 2:35:49 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Saturday, 25th May 2019 2:37:30 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#include <Application/Components/VirtualObject/VirtualObject.h>
#include <Core/auxiliarAppFuncs.h>

// constructor and destructor
VirtualObject::VirtualObject(Urho3D::Context* context)
    : Urho3D::LogicComponent(context) {
  // load hologram textures
  {
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    int seq = 8650;

    for (int i = 0; i < 399; ++i) {
      char filename[512];
      char finalPath[1024];
      sprintf(filename, "SAM_%d.xml", seq);
      sprintf(finalPath, ".\\converted_images\\Materials\\%s", filename);

      // TODO: make loadable from a JSON
      _images[i] = cache->GetResource<Material>(finalPath);

      ++seq;
    }
  }
  // TODO: set the plane model and material
}
VirtualObject::~VirtualObject() {}

// TODO: set nImages function
// TODO: set cameref function

// Update
void VirtualObject::Update(float timeStep) {
  ResourceCache* cache = GetSubsystem<ResourceCache>();
  Vector3 posCam, posHolo;

  posHolo = _rootNode->GetPosition();
  posCam = _camRef->GetPosition();

  Urho3D::Camera* camComp = _camRef->GetComponent<Urho3D::Camera>();
  double angle_Z_axis = triangulateTarget(posHolo.x_, posHolo.y_, posHolo.z_,
                                          posCam.x_, posCam.y_, posCam.z_);
  int sector = angle_Z_axis / 0.9f;

  if (sector <= nImages) {
    // hologramMaterial->SetTexture(TU_DIFFUSE, holoTextArray[sector]);
    rootModel->SetMaterial(_images[sector]);
  }

  // TODO: pitch and yaw need redefinition
  float yaw_ = 0.0f, pitch_ = 0.0f;

  // _rootNode->SetRotation(Quaternion(-90,0,270-angle_Z_axis));

  _rootNode->SetRotation(camComp->GetFaceCameraRotation(
      _rootNode->GetWorldPosition(),
      _rootNode->GetWorldRotation() + Quaternion(0.0f, 90.0f, 0.0f),
      FC_ROTATE_XYZ));
  Quaternion q = camComp->GetFaceCameraRotation(
      _rootNode->GetWorldPosition(),
      _rootNode->GetWorldRotation() + Quaternion(0.0f, 90.0f, 0.0f),
      FC_ROTATE_XYZ);
  Vector3 v = q.EulerAngles();
  _rootNode->SetRotation(q);
  _rootNode->Rotate(Quaternion(-90.0f, 0.0f, 0.0f));
  // _rootNode->SetWorldRotation(Quaternion(20.0f, 0.0f, 0.0f));
  char text[512];

  // debug text to see the rotation
  // sprintf(text,
  //         "SECTOR: %d\nCAM: %f, %f, %f\nZ AXIS ANGLE: %f\nyaw: %f pitch: "
  //         "%f\nFface cam: %f, %f, %f",
  //         sector, posCam.x_, posCam.y_, posCam.z_, angle_Z_axis, yaw_,
  //         pitch_, v.x_, v.y_, v.z_);
  // String txt = String(text);
  // debTex->SetText(txt);
  // debTex->SetColor(Color(255, 0, 0));
}
