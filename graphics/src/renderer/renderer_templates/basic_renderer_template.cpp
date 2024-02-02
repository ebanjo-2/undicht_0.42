#include "basic_renderer_template.h"
#include "core/vulkan/formats.h"

namespace undicht {

    namespace vulkan {

        void BasicRendererTemplate::init(VkDevice device, VkRenderPass render_pass, VkExtent2D view_port) {

            _device_handle = device;
            _render_pass_handle = render_pass;

            // should be called in the init() function of the derived class
            // initShaderModules()
            // initPipeLine()

        }

        void BasicRendererTemplate::cleanUp() {

            _vertex_shader.cleanUp();
            _fragment_shader.cleanUp();
            _pipeline.cleanUp();
        }

        void BasicRendererTemplate::setViewPort(VkExtent2D view_port) {

            _pipeline.cleanUp();
            _pipeline.setViewport(view_port);
            _pipeline.init(_device_handle, _render_pass_handle);
        }

        void BasicRendererTemplate::begin(vulkan::CommandBuffer& draw_cmd) {

            draw_cmd.bindGraphicsPipeline(_pipeline.getPipeline());
        }

        void BasicRendererTemplate::end(vulkan::CommandBuffer& draw_cmd) {
            // just here because other renderers might need this function

        }

        ////////////////////////////// functions to initialize parts of the renderer /////////////////////////////

        void BasicRendererTemplate::initShaderModules() {
            
            // example:
            //_vertex_shader.init(_device_handle, VK_SHADER_STAGE_VERTEX_BIT, vertex_shader);
            //_fragment_shader.init(_device_handle, VK_SHADER_STAGE_FRAGMENT_BIT, fragment_shader);
        }

        void BasicRendererTemplate::initPipeLine(VkExtent2D view_port) {
            
            setShaderInput();
            setVertexAttributes();
            setRasterizationState();
            addShaderModules();
            setBlending();
            setDepthStencilState();
            setMultisampleState();

            _pipeline.setViewport(view_port);
            _pipeline.init(_device_handle, _render_pass_handle);

        }

        ///////////////////////////// functions that initialize parts of the pipeline //////////////////////////

        void BasicRendererTemplate::setShaderInput() {

            // example:
            //_pipeline.setShaderInput(global_descriptor_layout, 0);
            //_pipeline.setShaderInput(material_descriptor_layout, 1);
            //_pipeline.setShaderInput(node_descriptor_layout, 2);

        }

        void BasicRendererTemplate::setVertexAttributes() {

            // default / example:
            _pipeline.addVertexBinding(0, 9 * sizeof(float));
            _pipeline.addVertexAttribute(0, 0, 0 * sizeof(float), translate(UND_VEC3F)); // position
            _pipeline.addVertexAttribute(0, 1, 3 * sizeof(float), translate(UND_VEC3F)); // tex coord
            _pipeline.addVertexAttribute(0, 2, 6 * sizeof(float), translate(UND_VEC3F)); // normal
            _pipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        }

        void BasicRendererTemplate::setRasterizationState() {
            // default
            _pipeline.setRasterizationState(false, VK_FRONT_FACE_CLOCKWISE);
        }

        void BasicRendererTemplate::addShaderModules() {
            // default
            _pipeline.addShaderModule(_vertex_shader);
            _pipeline.addShaderModule(_fragment_shader);
        }

        void BasicRendererTemplate::setBlending() {
            // default
            _pipeline.setBlending(0, false);
        }

        void BasicRendererTemplate::setDepthStencilState() {
            // default
            _pipeline.setDepthStencilState(true, true, VK_COMPARE_OP_LESS, false);
        }

        void BasicRendererTemplate::setMultisampleState() {
            // default
            _pipeline.setMultisampleState(false);
        }

    } // vulkan 

} // undicht