#include "post_process_inspector.h"

#include <sstream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../engine/render/post_process/render_process_queue.h"
#include "../engine/render/post_process/bloom_test.h"
#include "../engine/render/post_process/order_dithering.h"
#include "../engine/render/post_process/difference_of_gaussian.h"
#include "../engine/core/debug_macro.h"
#include "../engine/core/serialization/type_registry.h"
#include "../engine/core/window.h"


PostProcessInspector::PostProcessInspector()
{
}

PostProcessInspector::~PostProcessInspector()
{
}

void PostProcessInspector::update(float fDeltaTime)
{
    if (ImGui::Begin("Post Process Inspector", &m_bCollapsed))
    {
        ImGui::SetWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
        ImGui::SetWindowPos(ImVec2(305, 155), ImGuiCond_FirstUseEver);

        if (Window::ins)
        {
            bool bEnablePostProcess = Window::ins->isPostProcessEnabled();
            if (ImGui::Checkbox("Enable Post Process", &bEnablePostProcess))
            {
                Window::ins->setPostProcessEnabled(bEnablePostProcess);
            }
        }

        if (RenderProcessQueue::ins)
        {
            bool bSplitScreen = RenderProcessQueue::ins->isSplitScreen();
            if (ImGui::Checkbox("Split Screen Effect", &bSplitScreen))
            {
                RenderProcessQueue::ins->setSplitScreen(bSplitScreen);
            }

            if (bSplitScreen)
            {
                float fSplitFactor = RenderProcessQueue::ins->getSplitFactor();
                if (ImGui::DragFloat("Split Factor", &fSplitFactor, 0.01f, 0.0f, 1.0f))
                {
                    RenderProcessQueue::ins->setSplitFactor(fSplitFactor);
                }
            }

            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            
            ImGui::Text("Render Processes:");
            ImGui::Separator();

            int nSize = RenderProcessQueue::ins->getProcessCount();
            for (int i = 0; i < nSize; i++)
            {
                IRenderProcess* pProcess = RenderProcessQueue::ins->getProcessAt(i);
                if (pProcess)
                {
                    updateRenderProcessQueue(pProcess);
                }
            }
        }
        else
        {
            ImGui::Text("No RenderProcessQueue instance.");
        }
    }

    ImGui::End();
}

void PostProcessInspector::updateRenderProcessQueue(IRenderProcess* pProcess)
{
    const std::string typeName = TypeRegistry::instance().getTitlizedName(typeid(*pProcess));
    bool bActive = pProcess->isActive();
    if (ImGui::Checkbox(typeName.c_str(), &bActive))
    {
        pProcess->setActive(bActive);
    }

    std::ostringstream ss;
    ss << '^' << "##" << pProcess;
    ImGui::SameLine();
    if (ImGui::Button(ss.str().c_str()))
    {
        int nIndex = RenderProcessQueue::ins->getProcessIndex(pProcess);
        if (nIndex > 0)
        {
            RenderProcessQueue::ins->swapProcesses(nIndex, nIndex - 1);
        }
    }
    ss.clear();
    ss.str("");
    ss << 'v' << "##" << pProcess;
    ImGui::SameLine();
    if (ImGui::Button(ss.str().c_str()))
    {
        int nIndex = RenderProcessQueue::ins->getProcessIndex(pProcess);
        if (nIndex >= 0 && nIndex < RenderProcessQueue::ins->getProcessCount() - 1)
        {
            RenderProcessQueue::ins->swapProcesses(nIndex, nIndex + 1);
        }
    }

    if (BloomTest* pBloom = dynamic_cast<BloomTest*>(pProcess))
    {
        int nStep = pBloom->getBloomProcessDebugStep();
        if (ImGui::DragInt("Debug Step", &nStep, 1, 0, 4))
        {
            pBloom->setBloomProcessDebugStep(nStep);
        }

        float threshold = pBloom->getHighlightThreshold();
        if (ImGui::DragFloat("Highlight Threshold", &threshold, 0.01f, 0.0f, 1.0f))
        {
            pBloom->setHighlightThreshold(threshold);
        }

        float intensity = pBloom->getIntensity();
        if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 10.0f))
        {
            pBloom->setIntensity(intensity);
        }

        float radius = pBloom->getBlurRadius();
        if (ImGui::DragFloat("Blur Radius", &radius, 0.1f, 0.0f, 20.0f))
        {
            pBloom->setBlurRadius(radius);
        }

        float sigma = pBloom->getBlurSigma();
        if (ImGui::DragFloat("Blur Sigma", &sigma, 0.1f, 0.0f, 20.0f))
        {
            pBloom->setBlurSigma(sigma);
        }
    }
    else if (OrderDithering* pDithering = dynamic_cast<OrderDithering*>(pProcess))
    {
        // int nSize = pDithering->getDitherSize();
        // if (ImGui::DragInt("Dither Size", &nSize, 1, 2, 16))
        // {
        //     pDithering->setDitherSize(nSize);
        // }
    }
    else if (DifferenceOfGaussian* pDoG = dynamic_cast<DifferenceOfGaussian*>(pProcess))
    {
        // float fSigma1 = pDoG->getSigma1();
        // if (ImGui::DragFloat("Sigma 1", &fSigma1, 0.1f, 0.1f, 20.0f))
        // {
        //     pDoG->setSigma1(fSigma1);
        // }

        // float fSigma2 = pDoG->getSigma2();
        // if (ImGui::DragFloat("Sigma 2", &fSigma2, 0.1f, 0.1f, 20.0f))
        // {
        //     pDoG->setSigma2(fSigma2);
        // }

        // float fThreshold = pDoG->getThreshold();
        // if (ImGui::DragFloat("Threshold", &fThreshold, 0.01f, 0.0f, 1.0f))
        // {
        //     pDoG->setThreshold(fThreshold);
        // }
    }

    ImGui::Separator();
}
