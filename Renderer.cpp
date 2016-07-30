
#include "Renderer.hpp"
#include "Shader.hpp"

Renderer::Renderer() {}

Renderer::~Renderer() {
    for (Texture * texture : textures)
        delete texture;
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
    shading_shader.addSourceFile(GL_VERTEX_SHADER, "Shading.vs");
    shading_shader.addSourceFile(GL_FRAGMENT_SHADER, "Shading.fs");
    shading_shader.link();
    
    // Load finalization shader (gamma and HDR resolution)
    finalize_shader.addSourceFile(GL_VERTEX_SHADER, "Finalize.vs");
    finalize_shader.addSourceFile(GL_FRAGMENT_SHADER, "Finalize.fs");
    finalize_shader.link();
    
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

GLuint Renderer::loadMesh(std::string const & path) {
    auto it = meshNames.find(path);
    if (it != meshNames.end())
        return it->second;
    Mesh mesh;
    // TODO check for error
    mesh.load(path);
    GLuint index = meshDatas.size();
    meshDatas.push_back(mesh);
    meshNames[path] = index;
    return index;
}

GLuint Renderer::loadImage(std::string const & path) {
    auto it = imageNames.find(path);
    if (it != imageNames.end())
        return it->second;
    Image image;
    // TODO check for error
    image.load(path);
    GLuint index = imageDatas.size();
    imageDatas.push_back(image);
    imageNames[path] = index;
    return index;
}

void Renderer::pack() {
    // TODO allow pack-less resource loading!
    
    // Count total vertices
    GLuint count = 0;
    for (Mesh & mesh : meshDatas)
        count += mesh.getCount();
    
    // Allocate memory
    buffer.bind(GL_ARRAY_BUFFER);
    buffer.setData(count * 4 * (3 + 3 + 2), nullptr, GL_STATIC_DRAW);
    
    // Upload data
    GLuint offset = 0;
    meshMaps.clear();
    for (Mesh & mesh : meshDatas) {
        buffer.setSubData(offset * 4 * 3, mesh.getCount() * 4 * 3, mesh.getPositions());
        buffer.setSubData(count * 4 * 3 + offset * 4 * 3, mesh.getCount() * 4 * 3, mesh.getNormals());
        buffer.setSubData(offset * 4 * 2 + count * 4 * (3 + 3), mesh.getCount() * 4 * 2, mesh.getCoordinates());
        meshMaps.push_back({offset, mesh.getCount()});
        offset += mesh.getCount();
    }
    
    // Configure vertex array object
    array.bind();
    array.addAttribute(0, 3, GL_FLOAT, 0, 0);
    array.addAttribute(1, 3, GL_FLOAT, 0, count * 4 * 3);
    array.addAttribute(2, 2, GL_FLOAT, 0, count * 4 * (3 + 3));
    
    // Clear old textures
    for (Texture * texture : textures)
        delete texture;
    
    // Create textures
    textures.resize(imageDatas.size());
    for (GLuint index = 0; index < imageDatas.size(); ++index) {
        textures[index] = new Texture();
        textures[index]->createColor(imageDatas[index], true);
        textures[index]->setAnisotropy(true);
    }
}

void Renderer::addLight(LightInfo const & light) {
    lights.push_back(light);
}

void Renderer::addMesh(MeshInfo const & mesh) {
    meshes.push_back(mesh);
}

void Renderer::clear() {
    lights.clear();
    meshes.clear();
}

void Renderer::render(GLuint framebuffer, glm::mat4 const & projection, glm::mat4 const & view) {
    
    // Use the same vertex array for everything
    array.bind();
    
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
    render_shader.setUniform("projection", projection);
    render_shader.setUniform("view", view);
    
    // Draw textured geometry and store diffuse, emissive, position and normals
    drawTexturedObjects(render_shader);
    
    // Select light-only render buffer
    render_light_framebuffer.bind();
    
    // Do not overwrite depth
    glDepthMask(GL_FALSE);
    
    // Enable stencil to render shadows
    glEnable(GL_STENCIL_TEST);
    
    // Disable depth test
    glDisable(GL_DEPTH_TEST);
    
    // For each light...
    for (LightInfo & light : lights) {

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
        extrusion_shader.setUniform("projection", projection);
        extrusion_shader.setUniform("view", view);
        extrusion_shader.setUniform("light_position", light.position);
        extrusion_shader.setUniform("light_radius", light.radius);

        // TODO depth clamp?
        // see https://www.opengl.org/wiki_132/index.php?title=Vertex_Post-Processing&redirect=no#Depth_clamping

        // Draw geometry
        drawCasterObjects(extrusion_shader);

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
        shading_shader.use();
        shading_shader.setUniform("light_position", light.position);
        shading_shader.setUniform("light_color", light.color);
        shading_shader.setUniform("light_radius", light.radius);
        shading_shader.setUniform("texture_position", 1);
        shading_shader.setUniform("texture_normal", 2);

        // Draw geometry again to shade surfaces properly
        drawSquare();

        // Restore default values
        glDisable(GL_BLEND);
    }
    
    // Restore defaults
    glDisable(GL_STENCIL_TEST);
    glDepthMask(GL_TRUE);
    
    // Combine result on screen
    // TODO antialiasing, bloom, hdr, tone mapping, gamma correction
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    finalize_shader.use();
    finalize_shader.setUniform("texture_color", 0);
    finalize_shader.setUniform("texture_position", 1);
    finalize_shader.setUniform("texture_normal", 2);
    finalize_shader.setUniform("texture_light", 3);
    drawSquare();
    
}

void Renderer::drawUntexturedObjects(Shader & shader) {
    // TODO use https://www.opengl.org/wiki/GLAPI/glMultiDrawArrays
    // Need https://www.opengl.org/sdk/docs/man/html/glVertexAttribDivisor.xhtml
    // And http://www.g-truc.net/post-0518.html
    // i.e. use element draw asap
    for (MeshInfo & mesh : meshes) {
        shader.setUniform("model", mesh.transform);
        glm::ivec2 m = meshMaps[mesh.mesh];
        glDrawArrays(GL_TRIANGLES, m.x, m.y);
    }
}

void Renderer::drawCasterObjects(Shader & shader) {
    // TODO add parameter to disable shadowcast on some meshes
    drawUntexturedObjects(shader);
}

void Renderer::drawTexturedObjects(Shader & shader) {
    shader.setUniform("texture", 4);
    for (MeshInfo & mesh : meshes) {
        shader.setUniform("model", mesh.transform);
        textures[mesh.color]->bind(4);
        // TODO other textures
        glm::ivec2 m = meshMaps[mesh.mesh];
        glDrawArrays(GL_TRIANGLES, m.x, m.y);
    }
}

void Renderer::drawSquare() {
    glDrawArrays(GL_TRIANGLES, meshMaps[0].x, meshMaps[0].y);
}
