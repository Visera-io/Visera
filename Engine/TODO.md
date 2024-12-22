
## Design Principles
### User Interface
- Layer(User Invisible) -> Module(User Visible) -> Component(Module Visible)
  Example:
- Logic: ViseraRender::Render::Vulkan
- Interface: Render::GetVulkan();

## TODO
- Error Handling
- Configuration Files {Vulkan;}.
- Vulkan Internal Context (Save Mem, e.g., SizeOf(Fence) from 16B to 8B)