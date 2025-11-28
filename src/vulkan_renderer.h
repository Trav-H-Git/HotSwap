#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

struct VulkanSettings {
    uint32_t width = 1280;
    uint32_t height = 720;
    bool enableValidation = true;
    bool vsync = true;
    const char* appName = "Simple Vulkan Renderer";
};

class VulkanRenderer {
public:
    void Init(const VulkanSettings& settings);
    void BeginFrame();
    void DrawTriangle();  // Example simple draw call
    void EndFrame();
    void Shutdown();

private:
    // === Core Vulkan Objects ===
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    VkRenderPass m_RenderPass = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> m_Framebuffers;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    // === Sync ===
    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;

    uint32_t m_CurrentFrame = 0;

    // === Helpers ===
    void CreateInstance(const VulkanSettings& settings);
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSwapchain(const VulkanSettings& settings);
    void CreateRenderPass();
    void CreateGraphicsPipeline();
    void CreateFramebuffers();
    void CreateCommandBuffers();
    void CreateSyncObjects();
};
