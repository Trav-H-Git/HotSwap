#include "vulkan_renderer.h"

#include <vk_bootstrap.h> // optional: simplifies instance/swapchain creation
#include <stdexcept>
#include <iostream>

// ================================================================
// Init()
// ================================================================
void VulkanRenderer::Init(const RendererSettings& settings) {
    // --- 1. Create Vulkan instance ---
    // Prompt: Use vk-bootstrap (vkb::InstanceBuilder) or raw vkCreateInstance
    // Example:
    // vkb::InstanceBuilder builder;
    // auto inst_ret = builder.set_app_name(settings.appName)
    //                       .request_validation_layers(settings.enableValidation)
    //                       .build();
    // m_Instance = inst_ret.value().instance;
    CreateInstance(settings);

    // --- 2. Create surface ---
    // Prompt: Depends on platform (Win32, GLFW, SDL)
    // Example: glfwCreateWindowSurface(m_Instance, window, nullptr, &m_Surface);

    // --- 3. Select GPU (physical device) ---
    PickPhysicalDevice();

    // --- 4. Create logical device + queues ---
    CreateLogicalDevice();

    // --- 5. Create swapchain ---
    CreateSwapchain(settings);

    // --- 6. Create render pass (color attachment + subpass) ---
    CreateRenderPass();

    // --- 7. Create graphics pipeline ---
    // Prompt: Hardcode simple triangle vertex/fragment SPIR-V shaders
    // Example: load SPIR-V from file, create shader modules, then pipeline.
    CreateGraphicsPipeline();

    // --- 8. Create framebuffers for each swapchain image ---
    CreateFramebuffers();

    // --- 9. Create command buffers (one per frame) ---
    CreateCommandBuffers();

    // --- 10. Create synchronization objects (semaphores, fences) ---
    CreateSyncObjects();
}

// ================================================================
// BeginFrame()
// ================================================================
void VulkanRenderer::BeginFrame() {
    // Prompt: Wait for fence from previous frame
    // vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

    // Prompt: Acquire next swapchain image
    // vkAcquireNextImageKHR(..., &imageIndex);

    // Prompt: Reset command buffer and begin recording
    // vkBeginCommandBuffer(cmdBuf, &beginInfo);

    // Prompt: Begin render pass
    // VkRenderPassBeginInfo renderPassInfo = { ... };
    // vkCmdBeginRenderPass(cmdBuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

// ================================================================
// DrawTriangle()
// ================================================================
void VulkanRenderer::DrawTriangle() {
    // Prompt: Bind graphics pipeline
    // vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

    // Prompt: Bind vertex buffer (optional if using hardcoded verts)
    // vkCmdDraw(cmdBuf, 3, 1, 0, 0);
}

// ================================================================
// EndFrame()
// ================================================================
void VulkanRenderer::EndFrame() {
    // Prompt: End render pass and command buffer
    // vkCmdEndRenderPass(cmdBuf);
    // vkEndCommandBuffer(cmdBuf);

    // Prompt: Submit to graphics queue with semaphores
    // vkQueueSubmit(...);

    // Prompt: Present swapchain image
    // vkQueuePresentKHR(...);

    // Move to next frame index
    m_CurrentFrame = (m_CurrentFrame + 1) % m_Framebuffers.size();
}

// ================================================================
// Shutdown()
// ================================================================
void VulkanRenderer::Shutdown() {
    vkDeviceWaitIdle(m_Device);

    // Prompt: Destroy sync objects, framebuffers, pipeline, etc.
    // vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
    // vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
    // vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
    // vkDestroyDevice(m_Device, nullptr);
    // vkDestroyInstance(m_Instance, nullptr);
}
