
#include "Renderer.hpp"
#include "Shader.hpp"

Renderer::Renderer(GLFWwindow * window) : window(window) {}

Renderer::~Renderer() {
    for (Texture * texture : textures)
        delete texture;
}

bool Renderer::initialize() {
    // TODO handle errors
    
    // Load depth-only rendering shader
    depth_shader.addSourceFile(GL_VERTEX_SHADER, "Depth.vs");
    depth_shader.addSourceFile(GL_FRAGMENT_SHADER, "Depth.fs");
    depth_shader.link();

    // Load shadow volume extrusion shader
    extrusion_shader.addSourceFile(GL_VERTEX_SHADER, "Extrusion.vs");
    extrusion_shader.addSourceFile(GL_GEOMETRY_SHADER, "Extrusion.gs");
    extrusion_shader.addSourceFile(GL_FRAGMENT_SHADER, "Extrusion.fs");
    extrusion_shader.link();

    // Load shading shader
    shading_shader.addSourceFile(GL_VERTEX_SHADER, "Shading.vs");
    shading_shader.addSourceFile(GL_FRAGMENT_SHADER, "Shading.fs");
    shading_shader.link();

    // Load texture shader
    texture_shader.addSourceFile(GL_VERTEX_SHADER, "Texture.vs");
    texture_shader.addSourceFile(GL_FRAGMENT_SHADER, "Texture.fs");
    texture_shader.link();
    
    // Load texture shader
    resolve_shader.addSourceFile(GL_VERTEX_SHADER, "Resolve.vs");
    resolve_shader.addSourceFile(GL_FRAGMENT_SHADER, "Resolve.fs");
    resolve_shader.link();
    
    // Prepare matrices
    glfwGetFramebufferSize(window, &width, &height);
    projection = glm::perspective(PI / 3.0f, (float)width / (float)height, 0.1f, 100.0f);
    view = glm::lookAt(glm::vec3(-2.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Create render target
    render_color.createColor(width, height, true, 4);
    render_depthStencil.createDepthStencil(width, height, 4);
    render_framebuffer.bind();
    render_framebuffer.attach(render_color);
    render_framebuffer.attach(render_depthStencil);
    render_framebuffer.validate();
    
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

void Renderer::setView(glm::mat4 const & view) {
    this->view = view;
}

void Renderer::render() {
    
    // Use the same vertex array for everything
    array.bind();
    
    // Select render framebuffer
    render_framebuffer.bind();
    
    // Geometry rendering is multisampled
    glEnable(GL_MULTISAMPLE);
    
    // Clear everything
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // Enable depth test for geaometry rendering
    glEnable(GL_DEPTH_TEST);
    
    // Select depth shader
    depth_shader.use();
    depth_shader.setUniform("projection", projection);
    depth_shader.setUniform("view", view);
    
    // Draw geometry (depth-only))
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    drawUntexturedObjects(depth_shader);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    // TODO if using mipmap/anisotropic filtering, render texture here, since it needs depth/normal
    // and then, use deferred shading to compute lightmap

    // Do not override old depth
    glDepthMask(GL_FALSE);

    // Enable stencil to render shadows
    glEnable(GL_STENCIL_TEST);

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

        // Use stencil to only draw on non-zero area
        glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_EQUAL, 0, ~(GLint)0);
        glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);

        // Render only fragments that are exactly on geometry
        glDepthFunc(GL_EQUAL);

        // Use additive blend to combine lightmaps
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        // Select shading shader
        shading_shader.use();
        shading_shader.setUniform("projection", projection);
        shading_shader.setUniform("view", view);
        shading_shader.setUniform("light_position", light.position);
        shading_shader.setUniform("light_color", light.color);
        shading_shader.setUniform("light_radius", light.radius);

        // Draw geometry again to shade surfaces properly
        drawUntexturedObjects(shading_shader);

        // Restore default values
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
    }

    // Disable stencil
    glDisable(GL_STENCIL_TEST);

    // Render only fragments that are exactly on geometry
    glDepthFunc(GL_EQUAL);

    // Use multiplicative blend to combine light and texture
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);

    // Select texture shader
    texture_shader.use();
    texture_shader.setUniform("projection", projection);
    texture_shader.setUniform("view", view);
    texture_shader.setUniform("texture", 0);

    // Draw geometry again zo have textures
    drawTexturedObjects(texture_shader);

    // Restore defaults
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE);
    
    // Resolve multisampled buffer
    //processing_framebuffer[0].bind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    render_color.bind(0);
    resolve_shader.use();
    resolve_shader.setUniform("texture", 0);
    drawSquare();
    
    // Bloom
    // TODO 
    
    // TODO gamma correction and HDR resolution
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
    shader.setUniform("texture", 0);
    for (MeshInfo & mesh : meshes) {
        shader.setUniform("model", mesh.transform);
        textures[mesh.color]->bind(0);
        // TODO other textures
        glm::ivec2 m = meshMaps[mesh.mesh];
        glDrawArrays(GL_TRIANGLES, m.x, m.y);
    }
}

void Renderer::drawSquare() {
    glDrawArrays(GL_TRIANGLES, meshMaps[0].x, meshMaps[0].y);
}
