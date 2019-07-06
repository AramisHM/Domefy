#include "Urho3DAll.h"

class TVComponent;

class CLevelData : public Object {
    URHO3D_OBJECT(CLevelData, Object);

   public:
    CLevelData(Context* context);

    SharedPtr<Scene> scene_;
    SharedPtr<TVComponent> tvc;

    bool InitScene(Scene* scene);
    bool InitCameraFromSceneData(String camName = "Camera");
    bool InitTVComponentForSceneNode(String nodeName = "TV");

    virtual ~CLevelData();

   private:
    bool LoadSceneFromXMLFile();
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
};