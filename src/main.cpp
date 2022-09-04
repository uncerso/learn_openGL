#include "core/window.h"
#include "core/renderer.h"

#include <vector>
#include <iostream>
#include <string_view>
#include <string>
#include <exception>

namespace {

void printHelp(std::string_view prog_name) {
    auto& renderers = core::Renderer::renderers();
    std::cout << "Usage: " << prog_name << " [lesson name]\n\n";

    std::cout << "Available lesson names:\n";
    for (auto const * renderer : renderers)
        std::cout << renderer->name() << '\n';

    std::cout.flush();
}

core::Renderer * chooseRenderer(char const * renderer_raw_name) {
    auto& renderers = core::Renderer::renderers();
    if (renderers.empty())
        return nullptr;
    
    if (renderer_raw_name == nullptr)
        return renderers.back();
    
    std::string_view renderer_name = renderer_raw_name;
    for (auto * renderer : renderers) {
        if (renderer_name == renderer->name())
            return renderer;
    }

    return nullptr;
}

} // namespace

int main(int argc, char const * argv[]) {
    try {

        if (argc > 2) {
            printHelp(argv[0]);
            return 1;
        }

        auto* renderer = chooseRenderer(argc == 2 ? argv[1] : nullptr);
        if (!renderer) {
            if (argc == 1) {
                std::cout << "There is no solutions yet :(\n";
            } else {
                std::cout << "Cannot find the lesson solution with name: " << argv[1] << "\n\n";
                printHelp(argv[0]);
            }
            return 2;
        }

        std::cout << "Selected solution: " << renderer->name() << std::endl;

        auto& window = core::Window::create(800, 600);
        window.render(*renderer);

    } catch (std::exception const & e) {
        std::cerr << "Error: " << e.what() << std::endl;
    } catch (std::string const & e) {
        std::cerr << "Error: " << e << std::endl;
    } catch (char const * e) {
        std::cerr << "Error: " << e << std::endl;
    } catch (...) {
        std::cerr << "Unknown error :(" << std::endl;
    }

    return 0;
}
