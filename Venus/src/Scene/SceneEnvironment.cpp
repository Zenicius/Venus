#include "pch.h"
#include "SceneEnvironment.h"

namespace Venus {

    Ref<SceneEnvironment> SceneEnvironment::Create(const Ref<TextureCube>& radianceMap, const Ref<TextureCube>& irradianceMap)
    {
        return CreateRef<SceneEnvironment>(radianceMap, irradianceMap);
    }
}