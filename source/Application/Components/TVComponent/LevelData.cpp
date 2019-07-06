#include "LevelData.h"
#include "TVComponent.h"

CLevelData::CLevelData(Context* context) : Object(context),
                                           scene_(NULL) {
    TVComponent::RegisterObject(context);
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(CLevelData, HandleUpdate));
}

CLevelData::~CLevelData() {
}

bool CLevelData::InitScene(Scene* scene) {
    scene_ = scene;
    return LoadSceneFromXMLFile();
}

bool CLevelData::LoadSceneFromXMLFile() {
    File sceneFile(context_,
                   GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/" + "Scene.xml",
                   FILE_READ);

    return scene_->LoadXML(sceneFile);
}

bool CLevelData::InitTVComponentForSceneNode(String nodeName) {
    bool ret = false;
    if (scene_) {
        Node* tvNode = scene_->GetChild(nodeName, true);
        if (tvNode) {
            tvc = tvNode->CreateComponent<TVComponent>();
            tvc->OpenFileName("./nice.ogv");
            //tvc->OpenFileName("trailer_400p.ogv"); //error!

            StaticModel* sm = tvNode->GetComponent<StaticModel>();
            tvc->SetOutputModel(sm);
            ret = true;
        };
        ;
    }
    return ret;
}

void CLevelData::HandleUpdate(StringHash eventType, VariantMap& eventData) {
    Input* input = GetSubsystem<Input>();

    if (input->GetKeyDown('1'))
        tvc->Play();

    if (input->GetKeyDown('2'))
        tvc->Pause();

    if (input->GetKeyDown('3'))
        tvc->Stop();
}