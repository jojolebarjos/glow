
#include "Renderer.hpp"
#include "Shader.hpp"

Renderer::Renderer() : textures(nullptr) {}

Renderer::~Renderer() {
    delete textures;
}

bool Renderer::initialize(uint32_t width, uint32_t height) {
    // TODO handle errors
    
    // Load depth-only rendering shader
    render_shader.addSourceFile(GL_VERTEX_SHADER, "Render.vs");
    render_shader.addSourceFile(GL_FRAGMENT_SHADER, "Render.fs");
    render_shader.link();

    // Load shadow volume extrusion shader
    extrusion_shader.addSourceFile(GL_VERTEX_SHADER, "Extrusion.vs");
    extrusion_shader.addSourceFile(GL_GEOMETRY_SHADER, "Extrusion.gs");
    extrusion_shader.addSourceFile(GL_FRAGMENT_SHADER, "Extrusion.fs");
    extrusion_shader.link();

    // Load shading shader
    shading_shader.addSourceFile(GL_VERTEX_SHADER, "Processing.vs");
    shading_shader.addSourceFile(GL_FRAGMENT_SHADER, "Shading.fs");
    shading_shader.link();
    
    // Load finalization shader
    finalize_shader.addSourceFile(GL_VERTEX_SHADER, "Processing.vs");
    finalize_shader.addSourceFile(GL_FRAGMENT_SHADER, "Finalize.fs");
    finalize_shader.link();
    
    // Load FXAA shader
    antialiasing_shader.addSourceFile(GL_VERTEX_SHADER, "Antialiasing.vs");
    antialiasing_shader.addSourceFile(GL_FRAGMENT_SHADER, "Antialiasing.fs");
    antialiasing_shader.link();
    
    // Create render target
    render_color.createColor(width, height, true);
    render_position.createColor(width, height, true);
    render_normal.createColor(width, height, true);
    render_light.createColor(width, height, true);
    render_depthStencil.createDepthStencil(width, height);
    render_framebuffer.bind();
    render_framebuffer.attach(render_color);
    render_framebuffer.attach(render_position);
    render_framebuffer.attach(render_normal);
    render_framebuffer.attach(render_light);
    render_framebuffer.attach(render_depthStencil);
    render_framebuffer.validate();
    render_light_framebuffer.bind();
    render_light_framebuffer.attach(render_light);
    render_light_framebuffer.attach(render_depthStencil);
    render_light_framebuffer.validate();
    
    // Create processing targets
    for (int i = 0; i < 3; ++i) {
        processing_color[i].createColor(width, height, true);
        processing_framebuffer[i].bind();
        processing_framebuffer[i].attach(processing_color[i]);
        processing_framebuffer[i].validate();
    }
    
    // Load "default" mesh 0 used for processing
    loadMesh("Square.obj");
    
    return true;
}

uint32_t Renderer::loadMesh(std::string const & path) {
    auto it = meshNames.find(path);
    if (it != meshNames.end())
        return it->second;
    Mesh mesh;
    // TODO check for error
    mesh.load(path);
    uint32_t index = meshDatas.size();
    meshDatas.push_back(mesh);
    meshNames[path] = index;
    return index;
}

uint32_t Renderer::loadImage(std::string const & path) {
    auto it = imageNames.find(path);
    if (it != imageNames.end())
        return it->second;
    Image image;
    // TODO check for error
    image.load(path);
    uint32_t index = imageDatas.size();
    imageDatas.push_back(image);
    imageNames[path] = index;
    return index;
}

void Renderer::pack() {
    // TODO allow pack-less resource loading!
    
    // Count total vertices
    uint32_t count = 0;
    for (Mesh & mesh : meshDatas)
        count += mesh.getCount();
    
    // Allocate memory
    geometry_buffer.bind(GL_ARRAY_BUFFER);
    geometry_buffer.setData(count * 4 * (3 + 3 + 2), nullptr, GL_STATIC_DRAW);
    
    // Upload data
    uint32_t offset = 0;
    meshMaps.clear();
    for (Mesh & mesh : meshDatas) {
        geometry_buffer.setSubData(offset * 4 * 3, mesh.getCount() * 4 * 3, mesh.getPositions());
        geometry_buffer.setSubData(count * 4 * 3 + offset * 4 * 3, mesh.getCount() * 4 * 3, mesh.getNormals());
        geometry_buffer.setSubData(offset * 4 * 2 + count * 4 * (3 + 3), mesh.getCount() * 4 * 2, mesh.getCoordinates());
        meshMaps.push_back({offset, mesh.getCount()});
        offset += mesh.getCount();
    }
    
    // Configure vertex array object
    array.bind();
    array.addAttribute(0, 3, GL_FLOAT, 0, 0);
    array.addAttribute(1, 3, GL_FLOAT, 0, count * 4 * 3);
    array.addAttribute(2, 2, GL_FLOAT, 0, count * 4 * (3 + 3));
    permodel_buffer.bind(GL_ARRAY_BUFFER);
    array.addAttributeMat4(3, 80, 0, true);
    array.addAttribute(7, 4, GL_FLOAT, 80, 64, true);
    
    // Create textures
    delete textures;
    textures = new Texture();
    std::vector<Image const *> images;
    for (GLuint index = 0; index < imageDatas.size(); ++index)
        images.push_back(&imageDatas[index]);
    textures->createColorArray(images, true);
    textures->setAnisotropy(true);
}

void Renderer::clear() {
    lights.clear();
    models.clear();
}

void Renderer::addLight(Light const * light) {
    lights.push_back(light);
}

void Renderer::addModel(Model const * model) {
    models.push_back(model);
}

void Renderer::prepare() {
    
    // Cache per model parameters
    permodel_data.resize(models.size());
    for (size_t i = 0; i < models.size(); ++i) {
        permodel_data[i].transform = models[i]->getTransform();
        permodel_data[i].extra.x = models[i]->color;
    }
    
    // Upload to GPU
    // TODO use asynchronous streaming to avoid wait
    permodel_buffer.bind(GL_ARRAY_BUFFER);
    permodel_buffer.setData(models.size() * sizeof(PerModel), permodel_data.data(), GL_STREAM_DRAW);
    
    // Generate draw commands
    // TODO group models that have the same mesh?
    commands.resize(models.size());
    for (size_t i = 0; i < models.size(); ++i) {
        glm::ivec2 m = meshMaps[models[i]->mesh];
        commands[i].first = m.x;
        commands[i].count = m.y;
        commands[i].instanceCount = 1;
        commands[i].baseInstance = i;
    }
}

void Renderer::render(Camera const * camera) {
    
    // Use the same vertex array and texture array for everything
    array.bind();
    textures->bind(4);
    
    // Bind textures
    render_color.bind(0);
    render_position.bind(1);
    render_normal.bind(2);
    render_light.bind(3);
    
    // Select complete render framebuffer
    render_framebuffer.bind();
    
    // Clear everything
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // Enable depth test for geometry rendering
    glEnable(GL_DEPTH_TEST);
    
    // Select render shader
    render_shader.use();
    render_shader.setUniform("projection", camera->getProjection());
    render_shader.setUniform("view", camera->getView());
    render_shader.setUniform("textures", 4);
    
    // Draw textured geometry and store diffuse, emissive, position and normals
    glMultiDrawArraysIndirect(GL_TRIANGLES, commands.data(), commands.size(), 0);
    
    // Select light-only render buffer
    render_light_framebuffer.bind();
    
    // Do not overwrite depth
    glDepthMask(GL_FALSE);
    
    // Enable stencil to render shadows
    glEnable(GL_STENCIL_TEST);
    
    // Disable depth test
    glDisable(GL_DEPTH_TEST);
    
    // For each light...
    for (Light const * light : lights) {

        // Clear stencil
        glClear(GL_STENCIL_BUFFER_BIT);

        // Use Carmack's reverse shadow volume strategy
        glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, ~(GLint)0);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

        // Do not write color as well
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        
        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        
        // Select extrusion shader
        extrusion_shader.use();
        extrusion_shader.setUniform("projection", camera->getProjection());
        extrusion_shader.setUniform("view", camera->getView());
        extrusion_shader.setUniform("light_position", light->getPosition());
        extrusion_shader.setUniform("light_radius", light->getRadius());

        // TODO depth clamp?
        // see https://www.opengl.org/wiki_132/index.php?title=Vertex_Post-Processing&redirect=no#Depth_clamping

        // Draw geometry
        // TODO consider only relevant objects (i.e. filter CPU-side)
        // TODO filter non-caster objects
        glMultiDrawArraysIndirect(GL_TRIANGLES, commands.data(), commands.size(), 0);

        // Now, write color
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        
        // Disable depth test
        glDisable(GL_DEPTH_TEST);

        // Use stencil to only draw on non-zero area
        glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_EQUAL, 0, ~(GLint)0);
        glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
        
        // Use additive blend to combine lightmaps
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        // Select shading shader
        // TODO better shading model
        shading_shader.use();
        shading_shader.setUniform("light_position", light->getPosition());
        shading_shader.setUniform("light_color", light->getColor());
        shading_shader.setUniform("light_radius", light->getRadius());
        shading_shader.setUniform("texture_position", 1);
        shading_shader.setUniform("texture_normal", 2);

        // Draw geometry again to shade surfaces properly
        // TODO maybe should not draw full-screen quad and only cover expected area (e.g. using a sphere)
        glDrawArrays(GL_TRIANGLES, meshMaps[0].x, meshMaps[0].y);

        // Restore default values
        glDisable(GL_BLEND);
    }
    
    // Restore defaults
    glDisable(GL_STENCIL_TEST);
    glDepthMask(GL_TRUE);
    
    /*
    // Combine result on screen
    // TODO bloom, hdr, tone mapping, gamma correction
    processing_framebuffer[0].bind();
    finalize_shader.use();
    finalize_shader.setUniform("texture_color", 0);
    finalize_shader.setUniform("texture_position", 1);
    finalize_shader.setUniform("texture_normal", 2);
    finalize_shader.setUniform("texture_light", 3);
    glDrawArrays(GL_TRIANGLES, meshMaps[0].x, meshMaps[0].y);

    // Apply FXAA
    if (camera->getFramebuffer())
        camera->getFramebuffer()->bind();
    else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    processing_color[0].bind(0);
    antialiasing_shader.use();
    antialiasing_shader.setUniform("texture", 0);
    glDrawArrays(GL_TRIANGLES, meshMaps[0].x, meshMaps[0].y);
    */
    
    // Combine result on screen
    // TODO bloom, hdr, tone mapping, gamma correction
    if (camera->getFramebuffer())
        camera->getFramebuffer()->bind();
    else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    finalize_shader.use();
    finalize_shader.setUniform("texture_color", 0);
    finalize_shader.setUniform("texture_position", 1);
    finalize_shader.setUniform("texture_normal", 2);
    finalize_shader.setUniform("texture_light", 3);
    glDrawArrays(GL_TRIANGLES, meshMaps[0].x, meshMaps[0].y);
    
}
