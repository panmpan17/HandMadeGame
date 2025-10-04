#include "post_process_inspector.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../post_process/render_process_queue.h"
#include "../post_process/bloom_test.h"
#include "../post_process/order_dithering.h"
#include "../post_process/difference_of_gaussian.h"
#include "../debug_macro.h"
#include "../serialization/type_registry.h"


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
    // ImGui::Text("%s", typeName.c_str());
    bool bActive = pProcess->isActive();
    if (ImGui::Checkbox(typeName.c_str(), &bActive))
    {
        pProcess->setActive(bActive);
    }

    if (BloomTest* pBloom = dynamic_cast<BloomTest*>(pProcess))
    {
        int nStep = pBloom->getBloomProcessDebugStep();
        if (ImGui::DragInt("Debug Step", &nStep, 1, 0, 4))
        {
            pBloom->setBloomProcessDebugStep(nStep);
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
