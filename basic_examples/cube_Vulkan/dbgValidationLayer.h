
// Debug Validation Layer
/////////////////////////////////////////////
class debugMessengersLayers
{
public:
    /////////////////////////////////////////////
    void setupDebugMessenger(vk::Instance& instance) {
        auto createDebugUtilsMessenger = [](vk::Instance& instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) -> VkResult {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) return func(instance, reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT const *>(pCreateInfo), pAllocator, pDebugMessenger);
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        };

        using severityFlag = vk::DebugUtilsMessageSeverityFlagBitsEXT;
        using typeFlag = vk::DebugUtilsMessageTypeFlagBitsEXT;
        constexpr auto severityFlags = severityFlag::eVerbose | severityFlag::eWarning | severityFlag::eError | severityFlag::eInfo; // severityFlag::eInfo ==> add for more Infos
        constexpr auto typeFlags = typeFlag::eGeneral | typeFlag::eValidation | typeFlag::ePerformance;

        const vk::DebugUtilsMessengerCreateInfoEXT createInfo = {{}, severityFlags, typeFlags, debugCallback};
        if(createDebugUtilsMessenger(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) { throw std::runtime_error("failed to set up debug messenger!"); }
    }
    /////////////////////////////////////////////
    void destroyDebugMessenger(vk::Instance& instance) const {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) func(instance, debugMessenger, nullptr);
    }
    /////////////////////////////////////////////
    bool checkValLayer() const { // check validation layer support
        uint32_t layerCount;
        vk::detail::resultCheck(vk::enumerateInstanceLayerProperties(&layerCount, nullptr), "enumerateInstanceLayerProperties...");

        std::vector<vk::LayerProperties> availableLayers(layerCount);
        vk::detail::resultCheck(vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data()), "enumerateInstanceLayerProperties...");

        for (const char* layerName : validationLayers)
            for (const auto& layerProperties : availableLayers)
                if (!strcmp(layerName, layerProperties.layerName)) return true;

        return false;
    }
#ifdef ENABLE_VALIDATION_LAYER
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#else
    const std::vector<const char*> validationLayers = {};
#endif
private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }
    VkDebugUtilsMessengerEXT debugMessenger;
};

#ifdef ENABLE_VALIDATION_LAYER
    #define BUILD_DEBUG_MESSENGER(I) debug.setupDebugMessenger(I);
    #define DESTROY_DEBUG_MESSENGER(I) debug.destroyDebugMessenger(I);
    #define CHECK_VALIDATION_LAYER_SUPPORT() if(!debug.checkValLayer()) { throw std::runtime_error("validation layers requested, but not available!"); }
    #define PRINT_AVAILABLE_DEVICES(P) for(const auto& device : P) { std::cout << device.getProperties().deviceName << std::endl; }
    #define SPV_EXT ".dbg.spv"
    //#define ADDITIONAL_INFO // comment to suppress additional debug infos
#else
    #define CHECK_VALIDATION_LAYER_SUPPORT()
    #define BUILD_DEBUG_MESSENGER(I)
    #define DESTROY_DEBUG_MESSENGER(I)
    #define PRINT_AVAILABLE_DEVICES(P)
    #define SPV_EXT ".spv"
#endif

#define VK_CATCH(E) catch(vk::SystemError &e) { std::cout << "error: " E "  " << e.what() << "\n"; }
