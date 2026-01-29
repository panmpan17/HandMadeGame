#pragma once

#if __APPLE__
#include "renderer_metal.h"
#endif // __APPLE__


enum class GraphicAPI
{
    OpenGL,
    Metal
};


class Renderer
{
public:
    static inline GraphicAPI getGraphicAPI() { return sm_eGraphicAPI; }
    static inline void setGraphicAPI(GraphicAPI api) { sm_eGraphicAPI = api; }

    static inline bool isUsingMetal() { return sm_eGraphicAPI == GraphicAPI::Metal; }
    static inline bool isUsingOpenGL() { return sm_eGraphicAPI == GraphicAPI::OpenGL; }

private:
    static inline GraphicAPI sm_eGraphicAPI = GraphicAPI::OpenGL;
};