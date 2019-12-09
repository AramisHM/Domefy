/*
 * File: VirtualObject.h
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Wednesday, 13th November 2019 10:05:55 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Wednesday, 13th November 2019 10:06:13 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#ifndef VIRTUALOBJECT_H
#define VIRTUALOBJECT_H

#include <Core/CoreParameters.h>
#include <Math/vector2.h>
#include <Math/vector3.h>
#include <Urho3D.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Math/Quaternion.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Node.h>
// #include <Urho3DAll.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>  // c++ standar vector calss

using namespace fpmed;
using namespace Urho3D;

// NOTE: You must register the component to the engine in order for it to
// properly work use with context_->RegisterFactory<Name of your component>();
// to do so

#define MAX_NUM_SLICES 7000

/// Custom logic component: Used to create the Visible Human
class VirtualObject : public LogicComponent {
  // Must do this to register your class componenet
  URHO3D_OBJECT(VirtualObject, LogicComponent);

 private:
  Material *_images[MAX_NUM_SLICES];
  Node *_rootNode;         // The actual node to display images
  StaticModel *rootModel;  // the rootNodes's model
  Node *_camRef;           // The viewing reference node
  int nImages;  // Refers to the quantity of photos for a given scanned model

 public:
  VirtualObject(Context *context);
  ~VirtualObject();

  // Override
  virtual void Update(float timeStep) override;
};

#endif