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



    static inline void resetDrawCallCount() { sm_nDrawCallCount = 0; sm_nTriangleCount = 0; }
    static inline int getDrawCallCount() { return sm_nDrawCallCount; }
    static inline int getTriangleCount() { return sm_nTriangleCount; }
    static inline void increaseDrawCallCount() { ++sm_nDrawCallCount; }
    static inline void increaseDrawCallCount(int nTriangleCount) { ++sm_nDrawCallCount; sm_nTriangleCount += nTriangleCount; }

private:
    static inline GraphicAPI sm_eGraphicAPI = GraphicAPI::OpenGL;

    static inline int sm_nDrawCallCount = 0;
    static inline int sm_nTriangleCount = 0;
};

#if IS_DEBUG_VERSION
#define INCREASE_DRAW_CALL_COUNT(n) Renderer::increaseDrawCallCount(n)
#else
#define INCREASE_DRAW_CALL_COUNT(n) do {} while (0)
#endif // IS_DEBUG_VERSION
