# [TODO]
- ImGui uses Window Framebuffer size, so we have to handle window resize event in RHI.
- Configuration Files {Vulkan;}.
- Vulkan Internal Context (Save Mem, e.g., SizeOf(Fence) from 16B to 8B)

# Design Principles
## User Interface
- Layer(User Invisible) -> Module(User Visible) -> Component(Module Visible)