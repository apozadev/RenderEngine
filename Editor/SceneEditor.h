#include <vector>

#include "Graphics/MaterialLibrary.h"

class Scene;
class Material;

void ViewScene(Scene* _pScene);

void ViewMaterial(const std::vector<MaterialLibrary::MatEntry>& _lstMaterials);
