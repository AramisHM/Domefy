#ifndef MYCUSTOMAPPLICATION_H
#define MYCUSTOMAPPLICATION_H

#include <Application/Components/AnatomicViewer/AnatomicViewer.h>
#include <Application/Components/GrabbableUI/GrabbableUI.h>
#include <Application/Components/Slide/Slide.h>
#include <Application/Components/SlideAnimator/SlideAnimator.h>
#include <Application/Components/VHP/VHP.h>
#include <Application/Components/WebBrowser/WebBrowser.h>
#include <Application/Sample.h>
#include <FPMED.H>
#include <Urho3D.h>
#include <Urho3DAll.h>

#ifdef CEF_INTEGRATION
// cef requirement
#include <Application/Sample.h>

#include "UBrowserImage.h"
#include "simple_app.h"
#endif

namespace Urho3D {
class Button;
class Connection;
class Scene;
class Text;
class UIElement;
}  // namespace Urho3D

// cef requirement
class UCefApp;
class UCefBrowserWin;

class MyCustomApplication : public Sample {
  URHO3D_OBJECT(MyCustomApplication, Sample);

 public:
  MyCustomApplication(Context *context);
  ~MyCustomApplication();
  void Start();
  void Stop();
  void CreateScene();
  void MoveCamera(float timeStep);
  void HandleUpdates(StringHash eventType, VariantMap &eventData);
  void updateCameraPosition();
  void updateRemoteControls();
  Slide *slideComponent;
  // Registers the custom componenets and other functions taht are not
  // natively distributed with Urho3D.
  void RegisterCustomScriptAPI();

 protected:
  /* this will be used to show the debug info of this prototype app */
  Urho3D::Text *debTex;

  // The viewer node itself, contains a sub-node named "ViewerModel" with the
  // model
  // Node* viewerNode;
  GrabbableUI *viewerGrab;
  // The anatomic viewer componenet
  // AnatomicViewer *anatomicViewer;
  // Camera Grabbable
  GrabbableUI *cameraGrab;
  // Visible Human Project Component - The actual model
  VHP *vhp;
  GrabbableUI *slideGrab;
#ifdef CEF_INTEGRATION
  WebBrowser *webbrowser;
#endif
  // experimental, used for OGV videos feature
 private:
#ifdef CEF_INTEGRATION
  SharedPtr<UCefApp> uCefApp_;
  bool cefAppCreatedOnce_;
#endif
};
#endif